#!/bin/bash

sbatch batch_files/10000/RossAdam_MT1_sync_9_10000.sh
sbatch batch_files/10000/RossAdam_MT1_serial_10000.sh

sbatch batch_files/RossAdam_MT1_sync_4_1000.sh
sbatch batch_files/RossAdam_MT1_sync_9_1000.sh
sbatch batch_files/RossAdam_MT1_sync_25_1000.sh
sbatch batch_files/RossAdam_MT1_sync_36_1000.sh

sbatch batch_files/RossAdam_MT1_async_36_1000.sh
sbatch batch_files/RossAdam_MT1_async_25_1000.sh
sbatch batch_files/RossAdam_MT1_async_9_1000.sh
sbatch batch_files/RossAdam_MT1_async_4_1000.sh
