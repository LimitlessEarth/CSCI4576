#!/usr/bin/python

files = ['RossAdam_Proj_nbody_openmp_2_1_2_256320.6668286.comet-16-22.out', 'RossAdam_Proj_nbody_openmp_2_1_2_512160.6668279.comet-10-56.out', 'RossAdam_Proj_nbody_openmp_4_1_4_512160.6668288.comet-16-57.out', 'RossAdam_Proj_nbody_openmpi_2_1_2_512160.6668410.comet-23-18.out', 'RossAdam_Proj_nbody_serial_1_1_1_128160.6668248.comet-07-45.out',
'RossAdam_Proj_nbody_serial_1_1_1_256320.6668250.comet-25-29.out',
'RossAdam_Proj_nbody_serial_1_1_1_512160.6668242.comet-02-63.out'
]

for fil in files:
    data = []
    with open(fil, 'r') as outfile:
        content = outfile.readlines()
        print fil
        #print content
        for line in content:
            print line
            if 'Iteration' in line:
                print line.split('\t')
                data.append(line.split('\t')[1].split(' ')[0])
        #print data
        