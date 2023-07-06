import matplotlib.pyplot as plt
import csv

curr = []
avgCurr =[]
currSum = 0
lastXCurrValsAvg = []
lastXCurrValsSum = 0

voltage = []
voltSum = 0
lastXVoltValsAvg = []
lastXVoltValsSum = 0

currVoltRatio = []

time = []
i=0
j=0
k=0

numValues = 50


f = open('CSV_Data/CSM_DATA_69.csv')
f.readline()
for line in f:
    if(i%2 != 0):
        val = float(line)
        time.append((20*j)/1000)
        curr.append(val)
        currSum += val  
        avgCurr.append(  currSum / (j+1))
        if(j > numValues):
            for k in range(j-numValues, j):
                lastXCurrValsSum += curr[k]
            lastXCurrValsAvg.append(lastXCurrValsSum/ numValues)
            lastXCurrValsSum = 0
        else:
            lastXCurrValsAvg.append(currSum / (j+1))
        currVoltRatio.append(lastXCurrValsAvg[j] /lastXVoltValsAvg[j])  
        
            
        j+=1
    if(i%2 == 0):
        val = float(line) *10
        voltage.append(val)
        voltSum += val 
        if(k > numValues):
            for l in range(k-numValues, k):
                lastXVoltValsSum += voltage[l]
            lastXVoltValsAvg.append(lastXVoltValsSum/ numValues)
            lastXVoltValsSum = 0
        else:
            lastXVoltValsAvg.append(voltSum / (k+1))
        k+=1 
    i+=1
    
# with open('CSV_Data/CSM_DATA_31.csv','r') as csvfile:
#     plots = csv.reader(csvfile, delimiter=',')
#     for row in plots:
#         x.append(int(row[0]))
#         y.append(int(row[1]))
f.close
plt.plot(time,curr, 'b',label = "I")
plt.plot(time, lastXCurrValsAvg,  'r', label = "Iavg")
plt.plot(time,avgCurr, 'g',label = "Itotalavg")

#plt.plot(time, voltage,  'g', label = "V")
#plt.plot(time, lastXVoltValsAvg,  'y', label = "Vavg")
#plt.plot(time, currVoltRatio,  'b', label = "I/V")
plt.legend(loc='lower right', borderaxespad=0.)

plt.xlabel('Time [s]')
plt.ylabel('Current [mA]')
plt.title('Current VS Time')
plt.legend()
plt.show()
