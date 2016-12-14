#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "timer.h"

#define SOFTENING 1e-3f
#define BLOCK_SIZE 256

typedef struct { float4 *pos; float3 *vel; } Particle;

void print_usage() {
    printf("Usage: -d image dimensions\n-t time step\n-i number of iterations\n-w writing on off\n-p number of particles\n");
}

void initialize_particles(float *data, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 4; j++) {
            if (j == 3) {
                data[i * 4 + j] = 10.0f; //1.5f * (rand() / (float)RAND_MAX + 10.0);
            } else {
                data[i * 4 + j] = 2.0f * (rand() / (float)RAND_MAX) - 1.0f;
            }
        }
    }
}

__global__
void body_force(float4 *p, float3 *v, float dt, int n) {
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i < n) {
        float3 a = {0.0f, 0.0f, 0.0f};

        for (int tile = 0; tile < gridDim.x; tile++) {
            __shared__ float4 spos[BLOCK_SIZE];
            float4 tpos = p[tile * blockDim.x + threadIdx.x];
            spos[threadIdx.x] = make_float4(tpos.x, tpos.y, tpos.z, tpos.w);
            __syncthreads();

            #pragma unroll
            for (int j = 0; j < BLOCK_SIZE; j++) {
                float3 r;
                r.x = spos[j].x - p[i].x;
                r.y = spos[j].y - p[i].y;
                r.z = spos[j].z - p[i].z;
                
                float dist_sqr = r.x*r.x + r.y*r.y + r.z*r.z + SOFTENING;
                float inv_dist = rsqrtf(dist_sqr);
                float inv_dist3 = inv_dist * inv_dist * inv_dist;

                float ac = spos[j].w * inv_dist3;

                a.x += r.x * ac; 
                a.y += r.y * ac; 
                a.z += r.z * ac;
            }
            __syncthreads();
        }

        v[i].x += a.x * dt; 
        v[i].y += a.y * dt; 
        v[i].z += a.z * dt;
    }
}

int main(int argc, char* argv[]) {
    
    int                 num_part            = 1000000;
    int                 bytes               = num_part * sizeof(float4) + num_part * sizeof(float3);
    int                 num_iter            = 10;    // simulation iterations   
    float               dt                  = 0.0001f;    // time step
    int                 img_dim             = 1500;
    int                 option              = -1;
    double              writing_time        = 0;
    bool                writing             = false;
    int                 img_len;
    int                 nBlocks, frame, i;
    float               *buf, *d_buf;
    double              total_frame_time, avg_time, comp_time;
    int                 loc, x, y, a;
    char                frame_name[47];
    char                *out_buffer;

    // Parse commandline
    while ((option = getopt(argc, argv, "d:t:i:wp:")) != -1) {        
        switch (option) {
             case 'd' : 
                 img_dim = atoi(optarg);
                 break;
             case 't' : 
                 dt = atoi(optarg) * 0.0001;
                 break;
             case 'i' : 
                 num_iter = atoi(optarg);
                 break;
             case 'w' :
                 writing = true;
                 break;
             case 'p' :
                 num_part = atoi(optarg);
                 break;
             default:
                 print_usage(); 
                 exit(1);
        }
    }
    
    img_len = img_dim * img_dim;
    
    buf = (float*) malloc(bytes);
    Particle Host_Particle = { (float4*)buf, ((float3*)buf) + num_part };

    initialize_particles(buf, num_part); // Init pos / vel data / mass
    
    out_buffer = (char *) calloc(img_dim * img_dim, sizeof(char));

    cudaMalloc(&d_buf, bytes);
    Particle Device_Particle = { (float4*) d_buf, ((float3*)d_buf) + num_part };

    nBlocks = (num_part + BLOCK_SIZE - 1) / BLOCK_SIZE;
    total_frame_time = 0.0;

        //for (i = 0 ; i < num_part; i++) {
            //printf("X: %f\tY: %f\tZ: %f\tMASS: %f\n", Host_Particle.pos[i].x, Host_Particle.pos[i].y, Host_Particle.pos[i].z, Host_Particle.pos[i].w);
        //}

    for (frame = 1; frame <= num_iter; frame++) {
        StartTimer();

        cudaMemcpy(d_buf, buf, bytes, cudaMemcpyHostToDevice);
        body_force<<<nBlocks, BLOCK_SIZE>>>(Device_Particle.pos, Device_Particle.vel, dt, num_part);
        cudaMemcpy(buf, d_buf, bytes, cudaMemcpyDeviceToHost);

        for (i = 0 ; i < num_part; i++) { // integrate position
            Host_Particle.pos[i].x += Host_Particle.vel[i].x * dt;
            Host_Particle.pos[i].y += Host_Particle.vel[i].y * dt;
            Host_Particle.pos[i].z += Host_Particle.vel[i].z * dt;
            //printf("X: %f\tY: %f\tZ: %f\tMASS: %f\n", Host_Particle.pos[i].x, Host_Particle.pos[i].y, Host_Particle.pos[i].z, Host_Particle.pos[i].w);
        }

        comp_time = GetTimer() / 1000.0;
        if (frame > 1) { // First iter is warm up
            total_frame_time += comp_time; 
        }
        
        if (writing) {
            StartTimer();
 
            // write out pgm
            for (a = 0; a < num_part; a++) {
                x = (int) (Host_Particle.pos[a].x * 100.0) + (img_dim / 2);
                y = (int) (Host_Particle.pos[a].y * 100.0) + (img_dim / 2);
    
                if (x < 0 || x > img_dim)
                    continue;
    
                loc = x + (img_dim * y);
                //printf("%d\n", loc);
                if (loc >= 0 && loc < img_len) {        
                    out_buffer[loc] = 255;
                }
    
            }
   
            sprintf(frame_name, "img/%d.pgm", frame);
            FILE *file = fopen(frame_name, "w");
            fprintf(file, "P5\n");
            fprintf(file, "%d %d\n", img_dim, img_dim);
            fprintf(file, "%d\n", 255);
            fwrite(out_buffer, sizeof(char), img_len, file);
            fclose(file);

            for (a = 0; a < num_part; a++) {
                x = (int) (Host_Particle.pos[a].x * 100.0) + (img_dim / 2);
                y = (int) (Host_Particle.pos[a].y * 100.0) + (img_dim / 2);
    
                if (x < 0 || x > img_dim)
                    continue;
    
                loc = x + (img_dim * y);
                if (loc >= 0 && loc < img_len) {        
                    out_buffer[loc] = 0;
                }
            }
        
            writing_time = GetTimer() / 1000.0;
        }
        
        printf("Iteration %d:\t%.10f seconds\t%f seconds\n", frame, comp_time, writing_time);
        
    }
    
    avg_time = total_frame_time / (double) (num_iter-1); 

    printf("Total computation time was: %f\t\tAverage frame time was: %f\t\tAverage Particle interations per second were: %f\n", total_frame_time, avg_time, (double) (num_part * num_part) / avg_time);
    
    free(out_buffer);
    free(buf);
    cudaFree(d_buf);
}
