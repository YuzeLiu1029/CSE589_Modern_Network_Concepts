# -*- coding: utf-8 -*-
"""
Created on Sat Apr  8 15:23:37 2017

@author: yuzeliu
"""

import numpy as np
import math
#import openpyxl as pyxl
import csv
import pylab
import matplotlib.pyplot as plt


abt_l2 = np.zeros(5)
gbn_l2 = np.zeros(5);
sr_l2 = np.zeros(5);

abt_l5 = np.zeros(5)
gbn_l5 = np.zeros(5);
sr_l5 = np.zeros(5);

abt_l8 = np.zeros(5)
gbn_l8 = np.zeros(5);
sr_l8 = np.zeros(5);


sumtemp = 0
count = 0

with open('result_abt_l2.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    abt_l2[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
abt_l2[4] = sumtemp/10



sumtemp = 0
count = 0

with open('result_abt_l5.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    abt_l5[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
abt_l5[4] = sumtemp/10

sumtemp = 0
count = 0

with open('result_abt_l8.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    abt_l8[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
abt_l8[4] = sumtemp/10

sumtemp = 0
count = 0

with open('result_gbn_l2.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    gbn_l2[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
gbn_l2[4] = sumtemp/10



sumtemp = 0
count = 0

with open('result_gbn_l5.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    gbn_l5[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
gbn_l5[4] = sumtemp/10



sumtemp = 0
count = 0

with open('result_gbn_l8.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    gbn_l8[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
gbn_l8[4] = sumtemp/10


sumtemp = 0
count = 0

with open('result_sr2_l2.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    sr_l2[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
sr_l2[4] = sumtemp/10



sumtemp = 0
count = 0

with open('result_sr2_l5.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    sr_l5[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
sr_l5[4] = sumtemp/10






sumtemp = 0
count = 0

with open('result_sr2_l8.csv','rb') as csvfile:
    reader = csv.reader(csvfile, delimiter = '|')
    for row in reader:
        for i in row:
            temp = i.split(',')
            #print temp[10]
            if(temp[10] == 'Throughput'):
                if(count >= 1):
                    sr_l8[count-1] = sumtemp /10
                sumtemp = 0
                count = count + 1
            else :
                sumtemp = sumtemp + float(temp[10])
sr_l8[4] = sumtemp/10






index = np.arange(5)
bar_width = 0.15
opacity = 1
error_config = {'ecolor': '0.3'}

rects1 = plt.bar(index, abt_l2, bar_width,
                 alpha=opacity,
                 color='b',
                 error_kw=error_config,
                 label='abt')
rects2 = plt.bar(index + bar_width, gbn_l2, bar_width,
                 alpha=opacity,
                 color='r',
                 #yerr=std_women,
                 error_kw=error_config,
                 label='gbn_10')
rects2 = plt.bar(index + 2 * bar_width, sr_l2, bar_width,
                 alpha=opacity,
                 color='g',
                 #yerr=std_women,
                 error_kw=error_config,
                 label='sr_10')
plt.xlabel('Loss Possibility')
plt.ylabel('Throughput')
plt.title('Windowsize vs Throughput, loss possibility = 0.2')
plt.xticks(index + 3 * bar_width / 2, ('10', '50', '100', '200', '500'))
plt.legend()
pylab.ylim(0, 0.030)
plt.show()


index = np.arange(5)
bar_width = 0.15
opacity = 1
error_config = {'ecolor': '0.3'}

rects1 = plt.bar(index, abt_l5, bar_width,
                 alpha=opacity,
                 color='b',
                 error_kw=error_config,
                 label='abt')
rects2 = plt.bar(index + bar_width, gbn_l5, bar_width,
                 alpha=opacity,
                 color='r',
                 #yerr=std_women,
                 error_kw=error_config,
                 label='gbn_10')
rects2 = plt.bar(index + 2 * bar_width, sr_l5, bar_width,
                 alpha=opacity,
                 color='g',
                 #yerr=std_women,
                 error_kw=error_config,
                 label='sr_10')
plt.xlabel('Loss Possibility')
plt.ylabel('Throughput')
plt.title('Windowsize vs Throughput, loss possibility = 0.5')
plt.xticks(index + 3 * bar_width / 2, ('10', '50', '100', '200', '500'))
plt.legend()
pylab.ylim(0, 0.030)
plt.show()


index = np.arange(5)
bar_width = 0.15
opacity = 1
error_config = {'ecolor': '0.3'}

rects1 = plt.bar(index, abt_l8, bar_width,
                 alpha=opacity,
                 color='b',
                 error_kw=error_config,
                 label='abt')
rects2 = plt.bar(index + bar_width, gbn_l8, bar_width,
                 alpha=opacity,
                 color='r',
                 #yerr=std_women,
                 error_kw=error_config,
                 label='gbn_10')
rects2 = plt.bar(index + 2 * bar_width, sr_l8, bar_width,
                 alpha=opacity,
                 color='g',
                 #yerr=std_women,
                 error_kw=error_config,
                 label='sr_10')
plt.xlabel('Loss Possibility')
plt.ylabel('Throughput')
plt.title('Windowsize vs Throughput, loss possibility = 0.8')
plt.xticks(index + 3 * bar_width / 2, ('10', '50', '100', '200', '500'))
plt.legend()
pylab.ylim(0, 0.030)
plt.show()









