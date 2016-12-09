#!/usr/bin/python

# Adam Ross - tstc.py
# 
# A helper function to aggregate and plot the data from Stampede and Comet

import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
import numpy as np
import glob
import csv


def old():
    name_map = {
        "new_data/comet_inter_data" : "Comet Inter Node Communication",
        "new_data/comet_intra_data" : "Comet Intra Node Communication",
        "new_data/stampede_inter_data" : "Stampede Inter Node Communication",
        "new_data/stampede_intra_data" : "Stampede Intra Node Communication",
        "new_data/small_message" : "Small message Data"
    }

    for data_file in glob.glob("data/*"):
        byte_size = []
        timing = []

        with open(data_file) as f:
            content = f.readlines()
        content = [x.strip() for x in content]


        for point in content:
            data = point.split("\t")
            byte_size.append(float(data[0]))
            timing.append(float(data[1]))
    
        # Ts is the time to send a single byte which is the first element in our data
        ts = timing[0]
        
        byte_size = byte_size[12:]
        timing = timing[12:]

        # Least square fit data
        n = len(byte_size)
        stdevx = np.std(byte_size)
        stdevy = np.std(timing)
        sumx = sum(byte_size)
        sumy = sum(timing)
        sumxy = sum([byte_size[i] * timing[i] for i in range(n)])
        sumx2 = sum([x ** 2 for x in byte_size])
        # Tc is the slope of our lease square
        tc = ((n*sumxy) - (sumx*sumy)) / ((n*sumx2) - (sumx**2))
        b = (sumy - (tc*sumx)) / n
    
        print "file ", data_file, "     ts: ", '{0:.15f}'.format(ts), "      tc: ", '{0:.15f}'.format(tc)
    
        fig = plt.figure()
        fig.suptitle(name_map[data_file], fontsize=20)
        ax = fig.add_subplot(1,1,1)
    
        x = [2**x for x in range(len(byte_size))]
        y = [(b + tc*a) for a in x ]
    
        # For our normal data nromalize the x and y axis
        if data_file != "new_data/small_message":
            #x = [a/1000000.0 for a in x]
            #byte_size = [a/1000000.0 for a in byte_size]
            ax.plot(x, y)
            ax.plot(byte_size, timing, "o")
            ax.set_xscale("log", nonposy='clip')
            ax.set_yscale("log", nonposy='clip')
            ax.xaxis.set_major_formatter(mtick.FormatStrFormatter('%1.1f'))
        else:
            #ax.plot(x, y)
            ax.plot(byte_size, timing, "o")
            #ax.set_xticks(np.arange(0, 2048, 256))
            ax.xaxis.set_major_formatter(mtick.FormatStrFormatter('%1.1f'))
        
        

        legend = ax.legend(loc='lower center', shadow=True)


        plt.xlabel('Message Data Size(MB)', fontsize=14)
        plt.ylabel('Benchmrked time (us)', fontsize=14)
        plt.show()
        
def generate():

    data_map = {'serial' : [], 'openmp' : [], 'openmpi' : [], 'openmp_mpi' : [], 'openmp_block' : [], 'cuda' : []}
    
    # NP vs speed up
        # row, grid
        
        # X axis is NP
        # Y is speed up
        # Need speed up for estimated and actual on 12600?
        
    # Efficiency vs NP
        # Ideal vs actual for row and grid for all np
        
        # X is Efficiency
        # Y is NP
        # Need ideal and actual row and grid for all measured np
        
    # Time vs efficiency
        # For all measured np, using individual plots for block-row and checkerboard. 
        # Is the "knee" obvious? Where is the best balance between resource use and overall time to completion, if visible?
        
        # X is Time
        # Y is Efficiency
        
        # Need np, efficiency for grid and row
    with open('data.sum', 'r') as csvfile:
        data_reader = csv.reader(csvfile, delimiter=',', quotechar='|')
        for row in data_reader:
            new = row[0].split(':')
            
            full_name = new[0].strip().split('_')
            total_time = new[2].split('\t')[0].strip()
            avg_time = new[3].split('\t')[0].strip()
            
            #print full_name, ' ', len(full_name)
            
            np_per_node = '-1'
            threads_per_task = '-1'
            
            if len(full_name) > 6:
                total_processes = full_name[5]
                source = full_name[3]
                nodes = full_name[6]
                tasks = full_name[7]
            if len(full_name) == 9:
                source = full_name[3] + '_' + full_name[4]
                size = full_name[8].split('.')[0]
            elif len(full_name) == 11:
                source = full_name[3] + '_' + full_name[4]
                size = full_name[8]
                np_per_node = full_name[9]
                threads_per_task = full_name[10].split('.')[0]
            elif len(full_name) == 4:
                print "cuda!"
                source = 'cuda'
                nodes = '1'
                tasks = '2560'
                total_processes = '2560'
                size = full_name[-1].split('.')[0]
            else:
                total_processes = full_name[4]
                nodes = full_name[5]
                tasks = full_name[6]
                size = full_name[7].split('.')[0]
            
            print source, total_time, avg_time, total_processes, nodes, tasks, np_per_node, threads_per_task, size
            
            
            try:
                if int(total_processes) == 48 and int(np_per_node) in [1, 4] and int(threads_per_task) in [24, 6]:
                    pass
                else:
                    data_map[str(source)].append([float(total_time), float(avg_time), int(total_processes), int(nodes), int(tasks), int(np_per_node), int(threads_per_task), int(size)])
            except Exception as e:
                #print source, total_time, avg_time, total_processes, nodes, tasks, np_per_node, threads_per_task, size
                
                data_map['openmp_block'].append([float(total_time), float(avg_time), int(total_processes), int(nodes), int(tasks), int(np_per_node), int(threads_per_task), int(size), int(source[6:])])
              
        ############################################################################################
          
        data_map['serial'].sort(key=lambda x: x[7])
        
            
        fig = plt.figure()
        fig.suptitle('Serial Average frame time per Particle Count', fontsize=20)
        ax = fig.add_subplot(1,1,1)
        
        ax.plot([x[7] for x in data_map['serial']], [x[1] for x in data_map['serial']], label='serial')

        ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%1.1f'))
        legend = ax.legend(loc='lower right', shadow=True)


        plt.xlabel('Particle count', fontsize=14)
        plt.ylabel('Average fram time', fontsize=14)
        #plt.show()
        plt.savefig('Serial Average frame time per Particle Count.png'.replace(' ', '_'))
        fig.clf()
        
        ############################################################################################
        
        for src in data_map:
            data_map[src].sort(key=lambda x: x[7])
        
        for src in data_map:
            print src
            if src != 'serial' and src != 'openmp_block':
                data = {}
                for dat in data_map[src]:
                    if str(dat[2]) not in data:
                        data[str(dat[2])] = []
                    data[str(dat[2])].append([dat[7], dat[1]])
                
                #print data
                #print sorted([int(x) for x in data.keys()])
                
                fig = plt.figure()
                title = src + ' Average frame time per Particle Count'
                fig.suptitle(title, fontsize=20)
                ax = fig.add_subplot(1,1,1)
                
                for st in sorted([int(x) for x in data.keys()]):
                    ax.plot([x[0] for x in data[str(st)]], [x[1] for x in data[str(st)]], label=str(st) + " proc")

                ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%1.1f'))
                legend = ax.legend(loc='lower right', shadow=True)


                plt.xlabel('Particle count', fontsize=14)
                plt.ylabel('Average frame calc time', fontsize=14)
                #plt.show()
                plt.savefig((title + '.png').replace(' ', '_'))
                fig.clf()
                
                
                
                fig = plt.figure()
                title = src + ' Speedup'
                fig.suptitle(title, fontsize=20)
                ax = fig.add_subplot(1,1,1)
                spup = {}
                for st in sorted([int(x) for x in data.keys()]):
                    spup[str(st)] = []
                    for indx in range(len(data_map['serial'])):
                         spup[str(st)].append([x[1] for x in data_map['serial']][indx] / [x[1] for x in data[str(st)]][indx])
                    print st, spup[str(st)]
                    ax.plot([x[7] for x in data_map['serial']], spup[str(st)], label=str(st) + " proc")

                ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%1.1f'))
                legend = ax.legend(loc='lower right', shadow=True)


                plt.xlabel('Particle count', fontsize=14)
                plt.ylabel('Speedup', fontsize=14)
                #plt.show()
                plt.savefig((title + '.png').replace(' ', '_'))
                fig.clf()
                


                fig = plt.figure()
                title = src + ' Efficiency'
                fig.suptitle(title, fontsize=20)
                ax = fig.add_subplot(1,1,1)
                for st in sorted([int(x) for x in data.keys()]):
                    eff = [x/st for x in spup[str(st)]]
                    ax.plot([x[7] for x in data_map['serial']], eff, label=str(st) + " proc")

                ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%1.1f'))
                legend = ax.legend(loc='lower right', shadow=True)


                plt.xlabel('Particle count', fontsize=14)
                plt.ylabel('Efficiency', fontsize=14)
                #plt.show()
                plt.savefig((title + '.png').replace(' ', '_'))
                fig.clf()
            
            elif src == 'openmp_block':
                data_map[src].sort(key=lambda x: x[8])
                print data_map[src]
                
                fig = plt.figure()
                title = src + ' for Block timing'
                fig.suptitle(title, fontsize=20)
                ax = fig.add_subplot(1,1,1)
                ax.plot([x[8] for x in data_map[src]], [x[1] for x in data_map[src]], label= "12 proc")

                ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%1.1f'))
                legend = ax.legend(loc='lower right', shadow=True)


                plt.xlabel('Block size', fontsize=14)
                plt.ylabel('Frame time (sec)', fontsize=14)
                #plt.show()
                plt.savefig((title + '.png').replace(' ', '_'))
                fig.clf()
                
                  

        
        """
        fig = plt.figure()
        fig.suptitle('Serial Average frame time per Particle Count', fontsize=20)
        ax = fig.add_subplot(1,1,1)
        
        ax.plot([x[7] for x in data_map['serial']], [x[1] for x in data_map['serial']], label='serial')
        #ax.plot(, , label='actual async')
        #ax.plot(, , label='actual async overlap')
        
        #ax.plot(, , label='ideal sync')
        #ax.plot(, , label='ideal async')
        #ax.plot(, , label='ideal async overlap')
        
        #ax.plot(byte_size, timing, "o")
        #ax.set_xscale("log", nonposy='clip')
        #ax.set_yscale("log", nonposy='clip')
        ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%1.1f'))
        legend = ax.legend(loc='lower right', shadow=True)


        plt.xlabel('Particle count', fontsize=14)
        plt.ylabel('Average fram time', fontsize=14)
        plt.show()
        
        ############################################################################################
        
        fig = plt.figure()
        fig.suptitle('Time vs Efficiency Row-Block', fontsize=20)
        ax = fig.add_subplot(1,1,1)
        
        plot9_x = []
        plot9_y = []
        plot25_x = []
        plot25_y = []
        
        
        #ax.scatter(grid_actual_time, grid_eff)
        for i in range(0, len(row_actual_time)):
            if row_np[i] == 9:
                plot9_x.append(row_actual_time[i])
                plot9_y.append(row_eff[i])
            elif row_np[i] == 25:
                plot25_x.append(row_actual_time[i])
                plot25_y.append(row_eff[i])
                
        ax.plot(plot9_x[0:9:2], plot9_y[0:9:2], label='NP=9 Sync')
        ax.plot(plot25_x[0:9:2], plot25_y[0:9:2], label='NP=25 Sync')
        
        ax.plot(plot9_x[1:10:2], plot9_y[1:10:2], label='NP=9 Async')
        ax.plot(plot25_x[1:10:2], plot25_y[1:10:2], label='NP=25 Async')
        
        ax.plot(plot9_x[10:], plot9_y[10:], label='NP=9 Async Overlap')        
        ax.plot(plot25_x[10:], plot25_y[10:], label='NP=25 Async Overlap')
        
        
        #ax.plot(byte_size, timing, "o")
        #ax.set_xscale("log", nonposy='clip')
        #ax.set_yscale("log", nonposy='clip')
        ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%1.1f'))
        legend = ax.legend(loc='lower right', shadow=True)


        plt.xlabel('Timing (s)', fontsize=14)
        plt.ylabel('Efficiency', fontsize=14)
        plt.show()
        
        """
            
        
generate()
        
        
        
        
        
        
    

