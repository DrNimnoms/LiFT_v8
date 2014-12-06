
import datetime
import math
from PID import *
import socket
import time
import MySQLdb as mdb
import os
  
debug = False
BMUSocket = ['BMU1', 'BMU2', 'BMU3', 'BMU4']
BMUCommand = ['stop', 'stop', 'stop', 'stop']
chargerSocket = ['charger1', 'charger2']
SQLComm = ['sqlcon', 'sqlcur']
communicationFlags = [0, 0, 0, 0, 0, 0, 0]  # 0-3 BMUs 4-5 chargers 6 database
BMUnum = 4  # number of BMUs
host = "192.168.0.17"
port = 40
chargerHost = "192.168.0.19"
chargerPort = [9221, 9222]
balRec = [0,0]
chargeVol = [0, 0, 0, 0]
minVol = [7,7,7,7]
loopTime = datetime.datetime.now()
loopTime2 = datetime.datetime.now()
waitTime = 0.2
maxVol = 360
cMax = 44.5
cStop = 4.0
pidControl = ['pid1', 'pid2']
Kp = 60
Kd = -150
Ki = 80
if Ki == 0:
    iMax = cMax-2
else:
    iMax = (cMax-2)/Ki
iMin = -1000
pidControl[0] = PID(Kp, Ki, Kd, 0, 0, iMax, iMin)
pidControl[1] = PID(Kp, Ki, Kd, 0, 0, iMax, iMin)
#~ cvStart=4.15
volGoal = [4.2, 4.2]
#~ slop=(cStop-cMax)/(volGoal-cvStart)
chargeDone = [0, 0, 0, 0]
balanceDone = [0, 0, 0, 0]


def connect_to_charger(ii):
    """ connects to a Charger """
    chargerSocket[ii] = setup_communication(chargerHost+str(5+ii), chargerPort[ii])
    if not chargerSocket[ii]:  # is the charger not communicating?
        communicationFlags[ii+4] = 0  # set charger communication flag to false
        if debug:
            print "not connected to charger #%d" % (ii+1)
    else:
        communicationFlags[ii+4] = 1  # set charger communication flag to true
        send_command(chargerSocket[ii], "*IDN? \n")  # sends command to charger
        time.sleep(waitTime)
        reply = get_data(chargerSocket[ii], ii+4)
        if len(reply) < 3:  # if message is too small
            communicationFlags[ii+4] = 0  # its a bad message, failed communication flag set
        if debug:
            print "connected to charger #%d" % (ii+1)
    

def setup_charger(ii):
    send_command(chargerSocket[ii], "*IDN?")
    time.sleep(waitTime)
    reply = get_data(chargerSocket[ii], ii+4)
    send_command(chargerSocket[ii], "outp:stat 1")
    time.sleep(waitTime)
    send_command(chargerSocket[ii], "sour:volt:prot 365")
    time.sleep(waitTime)
    send_command(chargerSocket[ii], "sour:curr 0")
    time.sleep(waitTime)
    send_command(chargerSocket[ii], "sour:volt " + str(maxVol))
    if len(reply) < 3:
        communicationFlags[ii+4] = 0
    
    
def set_charge_current(ii):
    #~ send_command(chargerSocket[ii], "meas:curr? \n")
    #~ time.sleep(waitTime)
    #~ reply = get_data(chargerSocket[ii], ii+4)
    #~ print reply
    top_cell = max(chargeVol[2*ii], chargeVol[2*ii+1])
    cur = 2+pidControl[ii].update(top_cell)
    cur = round(max(min(cur, cMax), 0), 2)
    send_command(chargerSocket[ii], "sour:curr " + str(cur))
    #~ if len(reply) < 2:
        #~ communicationFlags[ii+4] = 0
    #~ if communicationFlags[ii+4]:
        #~ print [cur*2, round(volGoal[ii]-top_cell, 4)] #, top_cell, round(float(reply.strip())*2, 2)
    return cur


def end_charge(ii):
    send_command(chargerSocket[ii], "outp:stat 0")
    time.sleep(waitTime)
    send_command(chargerSocket[ii], "sour:volt 0")
    time.sleep(waitTime)
    send_command(chargerSocket[ii], "sour:curr 0")
    chargerSocket[ii].close()
    communicationFlags[ii+4] = 0


def set_data(bmu_data, b_num):
    """ send BMU ii Data to mysql database """
    data_out = ",".join(str(i) for i in bmu_data)
    time_str = str(datetime.datetime.now())
    #~ print time_str
    query = "INSERT INTO arduino_data%d (arduino_data%d.values,time) VALUES('%s','%s')" % (b_num, b_num, data_out, time_str)
    #print query
    try:
        SQLComm[1].execute(query)
        SQLComm[0].commit()
        communicationFlags[6] = 1
    except:
        communicationFlags[6] = 0


def get_command(n):
    """ gets BMU ii command from mysql database """
    query = "SELECT arduino_command%d.values FROM arduino_command%d ORDER BY id DESC LIMIT 1" % ((n+1), (n+1))
    try:
        SQLComm[1].execute(query)
        result = SQLComm[1].fetchone()
        result = result[0]
    except:
        result = "stop"
        communicationFlags[6] = 0
    return result


def connect_to_mysql():
    """ connects to a mysql """
    try:
        SQLComm[0] = setup_mysql()
        communicationFlags[6] = 1
        SQLComm[1] = SQLComm[0].cursor()
        if debug:
            print "connected to database"
    except:
        communicationFlags[6] = 0
        if debug:
            print "not connected to database"


def connect_to_bmu(ii):
    """ connects to a BMU """
    BMUSocket[ii] = setup_communication(host+str(ii), port+ii)
    if not BMUSocket[ii]:
        communicationFlags[ii] = 0
        if debug:
            print "not connected to BMU #%d" % (ii+1)
    else:
        if debug:
            print "connected to BMU #%d" % (ii+1)
        communicationFlags[ii] = 1


def setup_mysql():
    try:
        result = mdb.connect('localhost', 'userbme', 'Bme135', 'bme')
    except:
        result = False
    return result


def setup_communication(the_host, the_port):
    """ setup communication with the BMU and return server """
    try:
        result = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        result.settimeout(1)
        result.connect((the_host, the_port))
    except:
        #~ print 'Failed to create socket or connect'
        result = False
    return result


def send_command(_s, _c):
    """
    sets the specified data (writes the specified data into database).
    Please note that the data must be string of value separated by comma.
    """
    try:
        _s.send(_c+'\n')
        result = 1
    except:
        result = 0
    return result
  
    #~ def BMUclose(_s):
    #~ """ closes the socket """
    #~ _s.close()


def get_data(_s, ii):
    """ gets the header from BMU and time stamps it """
    condition = 1
    reply = ""
    while condition:
        try:
            c = _s.recv(1000)
            reply += c
            if '\n' in c or c == "":
                condition = 0
        except:
            communicationFlags[ii] = 0
            condition = 0
    return reply


def process_data(v, index):
    """ parses the data in to a dictionary"""
    startInx = 11
    BMEnum = 14
    value = v.rstrip('\n')
    value = value.split(",")
    value[0] = int(value[0],16)
    for i in range(startInx,startInx+BMEnum*10):
        value[i] = int(value[i],16)
    value=[float(i) for i in value]
    #convert dtime from uS to S
    #~ value[1]=value[1]
    #convert cell voltage
    for i in range(startInx,startInx+BMEnum*3):
      value[i]= value[i]*0.0001
    #convert module voltage
    for i in range(startInx+BMEnum*3,startInx+BMEnum*4):
      value[i]= value[i]*0.002
    #convert reference 2 voltage
    for i in range(startInx+BMEnum*4,startInx+BMEnum*5):
      value[i] = value[i]*0.0001
    #convert auxiliary temperature
    Binv = .00023866  # 1/ B-value where B-value is 4190 K
    T0inv = 0.003354  # 1/T_0 where T_0 is 298.15 K
    for i in range(startInx+BMEnum*5, startInx+BMEnum*9):
        value[i] *= 0.0001
        VINidx = ((i-startInx-BMEnum*5) % 4)+startInx+BMEnum*4
        if value[i] <= 0 or value[i] >= value[VINidx]:
            value[i] = -42
        else:
            value[i] /= (value[VINidx]-value[i])
            value[i] = T0inv + Binv*math.log(value[i])
            value[i] = 1.0/value[i]-273
    #convert internal chip temperature
    for i in range(startInx+BMEnum*9, startInx+BMEnum*10):
        value[i] = value[i]/75-263
        value = [round(i, 4) for i in value]
        value[0] = int(value[0])
        value[2] = int(value[2])
    minVol[index] = value[6]    # sets min voltage
    chargeVol[index] = value[7]  # sets max voltage
    bmu_flags = int2binary(value[0])  # sets flags
    if index < 2:
        stringnum = 0
    else:
        stringnum = 1
    stringMin = min(minVol[2*stringnum],minVol[2*stringnum+1])
    stringMax = max(chargeVol[2*stringnum],chargeVol[2*stringnum+1])
    #dead band for bal rec flag
    if (stringMin > 3.9 and (stringMax-stringMin) > .050):
        balRec[stringnum] = 1
    elif (stringMin < 3.89 or (stringMax-stringMin) < .045):
        balRec[stringnum] = 0
    # decide whether bal rec flags are set or not
    if balRec[stringnum]: 
	if not bmu_flags[22]: # set balance recommend if not already set
          value[0] += pow(2,22)
    else:
	if bmu_flags[22]:
          value[0] -= pow(2,22)
    chargeDone[index] = bmu_flags[20]  # sets charge done flag
    balanceDone[index] = bmu_flags[21]  # sets balanced done flag
    return value


def int2binary(num):
    n = num
    digit_lst = [0]*23
    for idx in range(0, 23):
        if n % 2 == 1:
            digit_lst[idx] = 1
        n /= 2
    return digit_lst
