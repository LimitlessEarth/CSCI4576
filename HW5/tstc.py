#!/usr/bin/python

import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
import numpy as np
import glob

for data_file in glob.glob("data/*"):
    byte_size = []
    timing = []

    with open(data_file) as f:
        content = f.readlines()
    content = [x.strip() for x in content]


    for point in content:
        data = point.split(" ")
        byte_size.append(float(data[0]))
        timing.append(float(data[1]))
        
    """
    fig = plt.figure()
    fig.suptitle('AllReduce Benchmarks 16 processes', fontsize=20)
    ax = fig.add_subplot(1,1,1)
    ax.plot(byte_size, timing)
    legend = ax.legend(loc='upper center', shadow=True)
    #ax.set_yticks(np.arange(0, mean_max+500, 500))
    ax.set_xscale("log", nonposy='clip')
    ax.set_yscale("log", nonposy='clip')
    #ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%.2e'))
    plt.xlabel('Message Data Size(Bytes)', fontsize=14)
    plt.ylabel('Benchmrked time (us)', fontsize=14)
    plt.show()
    """

    n = len(byte_size)
    meanx = np.mean(byte_size)
    meany = np.mean(timing)
    stdevx = np.std(byte_size)
    stdevy = np.std(timing)
    sumx = sum(byte_size)
    sumy = sum(timing)
    sumxy = sum([byte_size[i] * timing[i] for i in range(n)])
    sumx2 = sum([x ** 2 for x in byte_size])
    tc = (n*sumxy - sumx*sumy) / (n*sumx2 - sumx**2)
    print sumy
    print tc*sumx
    ts = (sumy - tc*sumx) / n
    print "file ", data_file, "     ts: ", '{0:.15f}'.format(ts), "      tc: ", '{0:.15f}'.format(tc)
