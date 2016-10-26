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
        
        

        legend = ax.legend(loc='upper center', shadow=True)


        plt.xlabel('Message Data Size(MB)', fontsize=14)
        plt.ylabel('Benchmrked time (us)', fontsize=14)
        plt.show()
        
def generate():
    np = []
    row_speedup = []
    row_eff = []
    row_ideal_time = []
    row_actual_time = []
    grid_speedup = []
    grid_eff = []
    grid_ideal_time = []
    grid_actual_time = []
    
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
            print row
            
            
        
generate()
        
        
        
        
        
        
    

