#!/usr/bin/python

import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
import numpy as np

#output_file_names = np.genfromtxt('output_files',delimiter=" ")
with open('output_files') as f:
    content = f.readlines()
content = [x.strip() for x in content]

i = 0
mean_max = 0.0

byte_size = []
mean = []
std = []
err_marg = []
err_marg_perc = []

for filename in content:
    output = np.genfromtxt(filename, delimiter="\t", unpack=True, skip_header=3)
    byte_size.append([x/8 for x in output[2]])
    mean.append([x * 1000000 for x in output[3]])
    std.append([x * 1000000 for x in output[4]])
    err_marg.append([x * 1000000 for x in output[5]])
    err_marg_perc.append(output[6])

#plt.errorbar()
fig = plt.figure()
fig.suptitle('test title', fontsize=20)
ax = fig.add_subplot(1,1,1)
for it in range(0, len(mean)):
    ax.plot(byte_size[it], mean[it])
    if max(mean[it]) > mean_max:
        mean_max = max(mean[it])
ax.set_yticks(np.arange(0, mean_max+500, 500))
ax.set_xscale("log", nonposy='clip')
#ax.yaxis.set_major_formatter(mtick.FormatStrFormatter('%.2e'))
plt.xlabel('Message Data Size(Bytes)', fontsize=14)
plt.ylabel('Benchmrked time (us)', fontsize=14)
plt.show()