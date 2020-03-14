# -*- coding: utf-8 -*-
"""
Created on Fri Apr  7 16:27:35 2017

@author: yuzeliu
"""

import numpy as np
import math
#import openpyxl as pyxl
import csv
import pylab
import matplotlib.pyplot as plt


abt = np.zeros(5)
gbn = np.zeros(5);
sr = np.zeros(5);
gbn50 = np.zeros(5);
sr50 = np.zeros(5);

sumtemp = 0
count = 0

with open('result_abt.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    abt[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
abt[4] = sumtemp/10

sumtemp = 0
count = 0
with open('result_gbn_10.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    gbn[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
gbn[4] = sumtemp/10

sumtemp = 0
count = 0
with open('result_sr_10.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    sr[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
sr[4] = sumtemp/10

#print abt
#print gbn
#print sr

x = [0.1, 0.2, 0.4, 0.6, 0.8]
#abt1 = [abt[0],abt[1],abt[2],abt[3],abt[4]]
pylab.plot(x, abt, '-o', color = 'blue', label='abt')
pylab.plot(x, gbn, '-o',color = 'red', label='gbn_10')
pylab.plot(x, sr, '-o', color = 'green',label='sr_10')
pylab.legend(loc='upper right')
pylab.title('Loss Probability vs Throughput, windowsize = 10')
pylab.xlabel('Loss Possibility')
pylab.ylabel('Throughputs')
#pylab.ylim(-1.5, 2.0)
pylab.show()




index = np.arange(5)
bar_width = 0.15
opacity = 1
error_config = {'ecolor': '0.3'}

rects1 = plt.bar(index, abt, bar_width,
                 alpha=opacity,
                 color='b',
                 error_kw=error_config,
                 label='abt')
rects2 = plt.bar(index + bar_width, gbn, bar_width,
                 alpha=opacity,
                 color='r',
                 #yerr=std_women,
                 error_kw=error_config,
                 label='gbn_10')
rects2 = plt.bar(index + 2 * bar_width, sr, bar_width,
                 alpha=opacity,
                 color='g',
                 #yerr=std_women,
                 error_kw=error_config,
                 label='sr_10')
plt.xlabel('Loss Possibility')
plt.ylabel('Throughput')
plt.title('Loss Possibility vs Throughput, windowsize = 10')
plt.xticks(index + 3 * bar_width / 2, ('0.1', '0.2', '0.4', '0.6', '0.8'))
plt.legend()
pylab.ylim(0, 0.030)
plt.show()








sumtemp = 0
count = 0
with open('result_gbn_50.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    gbn50[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
gbn50[4] = sumtemp/10

sumtemp = 0
count = 0
with open('result_sr_50.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    sr50[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
sr50[4] = sumtemp/10



pylab.plot(x, abt, '-o', color = 'blue', label='abt')
#pylab.plot(x, sr, '-g', label='sr_10')
pylab.plot(x, gbn50, '-o',color = 'red', label='gbn_50')
pylab.plot(x, sr50, '-o', color = 'green', label='sr_50')
pylab.legend(loc='lower left')
pylab.title('Loss Probability vs Throughput, windowsize = 50')
pylab.xlabel('loss possibility')
pylab.ylabel('throughputs')
pylab.xlim(0, 0.9)
pylab.show()

index = np.arange(5)
bar_width = 0.15
opacity = 1
error_config = {'ecolor': '0.3'}

rects1 = plt.bar(index, abt, bar_width,
                 alpha=opacity,
                 color='b',
                 error_kw=error_config,
                 label='abt')
rects2 = plt.bar(index + bar_width, gbn50, bar_width,
                 alpha=opacity,
                 color='r',
                 #yerr=std_women,
                 error_kw=error_config,
                 label='gbn_50')
rects2 = plt.bar(index + 2 * bar_width, sr50, bar_width,
                 alpha=opacity,
                 color='g',
                 #yerr=std_women,
                 error_kw=error_config,
                 label='sr_50')
plt.xlabel('Loss Possibility')
plt.ylabel('Throughput')
plt.title('Loss Possibility vs Throughput windowsize = 50')
plt.xticks(index + 3 * bar_width / 2, ('0.1', '0.2', '0.4', '0.6', '0.8'))
plt.legend()
pylab.ylim(0, 0.030)
plt.show()



'''
#pylab.plot(x, abt, '-b', label='abt')
pylab.plot(x, gbn, '-o',color = 'red' label='gbn_10')
pylab.plot(x, sr, '-o', color = 'green',label='sr_10')
pylab.plot(x, gbn50, '-o', label='gbn_50')
pylab.plot(x, sr50, '-o', label='sr_50')
pylab.legend(loc='lower left')
pylab.title('Loss probability vs Throughput')
pylab.xlabel('loss possibility')
pylab.ylabel('throughputs')
pylab.show()
'''

                
                    
            
        
        








