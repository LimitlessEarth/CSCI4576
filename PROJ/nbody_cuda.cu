#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

#define BLOCK_SIZE 256
#define SOFTENING 1e-9f


typedef struct { float4 *pos, *vel; } Particle;

void initialize_particles(float *data, int n) {
    for (int i = 0; i < n; i++) {
        data[i] = 2.0f * (rand() / (float)RAND_MAX) - 1.0f;
    }
}

__global__
void body_force(float4 *p, float4 *v, float dt, int n) {
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i < n) {
        float Fx = 0.0f; float Fy = 0.0f; float Fz = 0.0f;

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

                Fx += dx * inv_dist3; 
                Fy += dy * inv_dist3; 
                Fz += dz * inv_dist3;
            }
            __syncthreads();
        }

        v[i].x += Fx * dt; 
        v[i].y += Fy * dt; 
        v[i].z += Fz * dt;
    }
}

int main(const int argc, const char** argv) {
    
    int                 num_part            = 500000;
    int                 bytes               = 2 * num_part * sizeof(float4);
    const int           num_iter            = 100;    // simulation iterations   
    const float         dt                  = 0.01f;    // time step
    int                 nBlocks, frame, i;
    float               *buf, *d_buf;
    double              total_time, avg_time;
    
    buf = (float*) malloc(bytes);
    Particle Host_Particle = { (float4*)buf, ((float4*)buf) + num_part };

    initialize_particles(buf, 8 *num_part); // Init pos / vel data

    cudaMalloc(&d_buf, bytes);
    Particle Device_Particle = { (float4*) d_buf, ((float4*)d_buf) + num_part };

    nBlocks = (num_part + BLOCK_SIZE - 1) / BLOCK_SIZE;
    total_time = 0.0; 

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

        const double time_elapsed = GetTimer() / 1000.0;
        if (frame > 1) { // First iter is warm up
            total_time += time_elapsed; 
        }
        printf("Iteration %d: %.20f seconds\n", iter, time_elapsed);
    }
    avg_time = total_time / (double) (num_iter-1); 

    printf("%d, %0.3f\n", num_part, 1e-9 * num_part * num_part / avg_time);
    //printf("Average rate for iterations 2 through %d: %.3f +- %.3f steps per second.\n", num_iter, rate);
    printf("%d Bodies: average %0.3f Billion Interactions / second\n", num_part, 1e-9 * num_part * num_part / avg_time);
    
    free(buf);
    cudaFree(d_buf);
}
