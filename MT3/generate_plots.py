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

    for data_file in glob.glob("new_data/*"):
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
    row_np = []
    row_world_size = []
    row_actaul_speedup = []
    row_ideal_speedup = []
    row_eff = []
    row_ideal_time = []
    row_actual_time = []
    row_sync_type = []
    
    p_row_np = []
    p_row_actual_speedup = []
    p_row_ideal_speedup = []
    p_row_eff = []
    p_row_sync_type = []
    
    
    grid_np = []
    grid_world_size = []
    grid_actual_speedup = []
    grid_ideal_speedup = []
    grid_eff = []
    grid_ideal_time = []
    grid_actual_time = []
    grid_sync_type = []
    
    p_grid_np = []
    p_grid_actual_speedup = []
    p_grid_ideal_speedup = []
    p_grid_eff = []
    p_grid_sync_type = []
    
    
    serial_np =[]
    serial_world_size = []
    serial_ideal_time = []
    serial_actual_time = []
    serial_sync_type = []
    
    p_serial_np =[]
    p_serial_sync_type = []
    
    world_size = []
    
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
    with open('csv/MT3_Data_Stampede.csv', 'r') as csvfile:
        data_reader = csv.reader(csvfile, delimiter=',', quotechar='|')
        for row in data_reader:
            #print row
            if row[0] == "Row":
                row_np.append(int(row[2]))
                row_world_size.append(int(row[3]))
                row_actaul_speedup.append(float(row[8]))
                row_ideal_speedup.append(float(row[6]))
                row_eff.append(float(row[9]))
                row_ideal_time.append(float(row[4]) + float(row[5]))
                row_actual_time.append(float(row[7]))
                row_sync_type.append(row[1])
                if row[3] == '12600':
                    p_row_np.append(int(row[2]))
                    p_row_actual_speedup.append(float(row[8]))
                    p_row_ideal_speedup.append(float(row[6]))
                    p_row_eff.append(float(row[9]))
                    p_row_sync_type.append(row[1])
            elif row[0] == "Block":
                grid_np.append(int(row[2]))
                grid_world_size.append(int(row[3]))
                grid_actual_speedup.append(float(row[8]))
                grid_ideal_speedup.append(float(row[6]))
                grid_eff.append(float(row[9]))
                grid_ideal_time.append(float(row[4]) + float(row[5]))
                grid_actual_time.append(float(row[7]))
                grid_sync_type.append(row[1])
                if row[3] == '12600':
                    p_grid_np.append(int(row[2]))
                    p_grid_actual_speedup.append(float(row[8]))
                    p_grid_ideal_speedup.append(float(row[6]))
                    p_grid_eff.append(float(row[9]))
                    p_grid_sync_type.append(row[1])
            elif row[0] == "Serial":
                serial_np.append(int(row[2]))
                serial_world_size.append(int(row[3]))
                serial_ideal_time.append(float(row[4]) + float(row[5]))
                serial_actual_time.append(float(row[7]))
                serial_sync_type.append(row[1])
                if row[3] == '12600':
                    p_serial_np.append(int(row[2]))
                    p_serial_sync_type.append(row[1])
                
        #
        print p_grid_np
        print p_grid_actual_speedup
        print p_grid_ideal_speedup
        
        fig = plt.figure()
        fig.suptitle('NP vs Speedup Checkerboard', fontsize=20)
        ax = fig.add_subplot(1,1,1)
        
        ax.plot(p_grid_actual_speedup[:2], p_grid_np[:2], label='actual sync')
        ax.plot(p_grid_actual_speedup[2:4], p_grid_np[2:4], label='actual async')
        ax.plot(p_grid_actual_speedup[4:], p_grid_np[4:], label='actual async overlap')
        
        ax.plot(p_grid_ideal_speedup[:2], p_grid_np[:2], label='ideal sync')
        ax.plot(p_grid_ideal_speedup[2:4], p_grid_np[2:4], label='ideal async')
        ax.plot(p_grid_ideal_speedup[4:], p_grid_np[4:], label='ideal async overlap')
        
        #ax.plot(byte_size, timing, "o")
        #ax.set_xscale("log", nonposy='clip')
        #ax.set_yscale("log", nonposy='clip')
        ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%1.1f'))
        legend = ax.legend(loc='lower right', shadow=True)


        plt.xlabel('NP (processes)', fontsize=14)
        plt.ylabel('Efficiency', fontsize=14)
        plt.show()
        
        #####################################################################################
        
        fig = plt.figure()
        fig.suptitle('NP vs Speedup Row-Block', fontsize=20)
        ax = fig.add_subplot(1,1,1)
        
        ax.plot(p_row_actual_speedup[:2], p_row_np[:2], label='actual sync')
        ax.plot(p_row_actual_speedup[2:4], p_row_np[2:4], label='actual async')
        ax.plot(p_row_actual_speedup[4:], p_row_np[4:], label='actual async overlap')
        
        ax.plot(p_row_ideal_speedup[:2], p_row_np[:2], label='ideal sync')
        ax.plot(p_row_ideal_speedup[2:4], p_row_np[2:4], label='ideal async')
        ax.plot(p_row_ideal_speedup[4:], p_row_np[4:], label='ideal async overlap')
        
        #ax.plot(byte_size, timing, "o")
        #ax.set_xscale("log", nonposy='clip')
        #ax.set_yscale("log", nonposy='clip')
        ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%1.1f'))
        legend = ax.legend(loc='lower right', shadow=True)


        plt.xlabel('NP (processes)', fontsize=14)
        plt.ylabel('Efficiency', fontsize=14)
        plt.show()
        
        #####################################################################################
        
        fig = plt.figure()
        fig.suptitle('NP vs Efficiency CheckerBoard', fontsize=20)
        ax = fig.add_subplot(1,1,1)
        
        ax.plot(p_grid_np[:2], p_grid_eff[:2], label='actual sync')
        ax.plot(p_grid_np[2:4], p_grid_eff[2:4], label='actual async')
        ax.plot(p_grid_np[4:], p_grid_eff[4:], label='actual async overlap')
        
        ax.plot(p_grid_np[:2], [1, 1], label='ideal sync')
        ax.plot(p_grid_np[2:4], [1, 1], label='ideal async')
        ax.plot(p_grid_np[4:], [1, 1], label='ideal async overlap')
        
        #ax.plot(byte_size, timing, "o")
        #ax.set_xscale("log", nonposy='clip')
        #ax.set_yscale("log", nonposy='clip')
        ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%1.1f'))
        legend = ax.legend(loc='lower right', shadow=True)


        plt.xlabel('NP (processes)', fontsize=14)
        plt.ylabel('Efficiency', fontsize=14)
        plt.show()
        
        #####################################################################################
        
        fig = plt.figure()
        fig.suptitle('NP vs Efficiency Row-Block', fontsize=20)
        ax = fig.add_subplot(1,1,1)
        
        ax.plot(p_row_np[:2], p_row_eff[:2], label='actual sync')
        ax.plot(p_row_np[2:4], p_row_eff[2:4], label='actual async')
        ax.plot(p_row_np[4:], p_row_eff[4:], label='actual async overlap')
        
        ax.plot(p_row_np[:2], [1, 1], label='ideal sync')
        ax.plot(p_row_np[2:4], [1, 1], label='ideal async')
        ax.plot(p_row_np[4:], [1, 1], label='ideal async overlap')
        
        #ax.plot(byte_size, timing, "o")
        #ax.set_xscale("log", nonposy='clip')
        #ax.set_yscale("log", nonposy='clip')
        ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%1.1f'))
        legend = ax.legend(loc='lower right', shadow=True)


        plt.xlabel('NP (processes)', fontsize=14)
        plt.ylabel('Efficiency', fontsize=14)
        plt.show()
        
        #####################################################################################
        
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
        
        #####################################################################################
        
        fig = plt.figure()
        fig.suptitle('Time vs Efficiency Checkerboard', fontsize=20)
        ax = fig.add_subplot(1,1,1)
        
        plot9_x = []
        plot9_y = []
        plot25_x = []
        plot25_y = []
        
        
        #ax.scatter(grid_actual_time, grid_eff)
        for i in range(0, len(grid_actual_time)):
            if row_np[i] == 9:
                plot9_x.append(grid_actual_time[i])
                plot9_y.append(grid_eff[i])
            elif row_np[i] == 25:
                plot25_x.append(grid_actual_time[i])
                plot25_y.append(grid_eff[i])
                
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
            
        
generate()
        
        
        
        
        
        
    

