#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "timer.h"

#define SOFTENING 1e-9f
#define BLOCK_SIZE 256

typedef struct { float4 *pos, *vel; } Particle;

void print_usage() {
    printf("Usage: -d image dimensions\n-t time step\n-i number of iterations\n-w writing on off\n-p number of particles\n");
}

void initialize_particles(float *data, int n) {
    for (int i = 0; i < n; i++) {
        data[i] = 2.0f * (rand() / (float)RAND_MAX) - 1.0f;
    }
}

__global__
void body_force(float4 *p, float4 *v, float dt, int n) {
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i < n) {
        float ax = 0.0f; float ay = 0.0f; float az = 0.0f;

        for (int tile = 0; tile < gridDim.x; tile++) {
            __shared__ float3 spos[BLOCK_SIZE];
            float4 tpos = p[tile * blockDim.x + threadIdx.x];
            spos[threadIdx.x] = make_float3(tpos.x, tpos.y, tpos.z);
            __syncthreads();

            #pragma unroll
            for (int j = 0; j < BLOCK_SIZE; j++) {
                float dx = spos[j].x - p[i].x;
                float dy = spos[j].y - p[i].y;
                float dz = spos[j].z - p[i].z;
                float dist_sqr = dx*dx + dy*dy + dz*dz + SOFTENING;
                float inv_dist = rsqrtf(dist_sqr);
                float inv_dist3 = inv_dist * inv_dist * inv_dist;

                ax += dx * inv_dist3; 
                ay += dy * inv_dist3; 
                az += dz * inv_dist3;
            }
            __syncthreads();
        }

        v[i].x += ax * dt; 
        v[i].y += ay * dt; 
        v[i].z += az * dt;
    }
}

int main(int argc, char* argv[]) {
    
    int                 num_part            = 1000000;
    int                 bytes               = 2 * num_part * sizeof(float4);
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
    Particle Host_Particle = { (float4*)buf, ((float4*)buf) + num_part };

    initialize_particles(buf, 8 *num_part); // Init pos / vel data
    
    out_buffer = (char *) calloc(img_dim * img_dim, sizeof(char));

    cudaMalloc(&d_buf, bytes);
    Particle Device_Particle = { (float4*) d_buf, ((float4*)d_buf) + num_part };

    nBlocks = (num_part + BLOCK_SIZE - 1) / BLOCK_SIZE;
    total_frame_time = 0.0; 

    for (frame = 1; frame <= num_iter; frame++) {
        StartTimer();

        cudaMemcpy(d_buf, buf, bytes, cudaMemcpyHostToDevice);
        body_force<<<nBlocks, BLOCK_SIZE>>>(Device_Particle.pos, Device_Particle.vel, dt, num_part);
        cudaMemcpy(buf, d_buf, bytes, cudaMemcpyDeviceToHost);

        for (i = 0 ; i < num_part; i++) { // integrate position
            Host_Particle.pos[i].x += Host_Particle.vel[i].x * dt;
            Host_Particle.pos[i].y += Host_Particle.vel[i].y * dt;
            Host_Particle.pos[i].z += Host_Particle.vel[i].z * dt;
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
