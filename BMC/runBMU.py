
"""socketTest_x_x.py
script that sets up communication with the Arduino though ethernet
"""

__author__ = "Nima Ghods (nima.ghods@ga.com)"
__version__ = "$Revision: 4.2 $"
__date__ = "$Date: 2014/05/10$"
__copyright__ = "Copyright (c) 2012 General Atomics"
__license__ = "Python"

from subFunc import *

if __name__ == '__main__':
    while True:
        timeStamp = datetime.datetime.now()  # get time
        if(debug):
			print loopTime
        if communicationFlags[6] == 0:  # are we not connected database?
            connect_to_mysql()  # connect to db
        else:
            for ii in range(0, BMUnum):  # for all BMUs
                BMUCommand[ii] = get_command(ii) # get command from database
                if communicationFlags[ii] == 0:  # if not connected to BMU[ii]
                    connect_to_bmu(ii)  # connect to BMU[ii], also sets global BMUSocket[ii]
                communicationFlags[ii] = send_command(BMUSocket[ii], BMUCommand[ii])  # sends command to BMU[ii]
                reply = get_data(BMUSocket[ii], ii)  # gets data from BMUs
                if len(reply) >= 3 and communicationFlags[ii] == 1:  # if connected and non-empty message
                    reply = process_data(reply, ii)  # parses and processes data, assigns some variables too
                    #~ print reply
                    set_data(reply, ii+1)  # puts reply info in database
            for ii in range(0, 2):
                if 'cha' in BMUCommand[2*ii] and 'cha' in BMUCommand[2*ii+1] and not\
                        chargeDone[2*ii] and not chargeDone[2*ii+1]:
                    if communicationFlags[ii+4] == 0:  # is the charger not connected?
                        pidControl[ii].setPoint(volGoal[ii])  # PID Goal for ii'th full string if not connected
                        pidControl[ii].setDerivator(volGoal[ii]-max(chargeVol[2*ii], chargeVol[2*ii+1]))
                        connect_to_charger(ii)  # connect ii'th full string to charger, sets communicationFlags
                        if communicationFlags[ii+4] == 1:
                            setup_charger(ii)  # set up the charger, also verifies communication
                    else:
                        commandTemp = BMUCommand[2*ii].split('_')  # value after charge_<xxx>
                        if volGoal[ii] != float(commandTemp[1])/10000:  # if goal is not the command
                            volGoal[ii] = float(commandTemp[1])/10000  # set goal to the command
                            pidControl[ii].setPoint(volGoal[ii])  # update the PID with the new goal
                        set_charge_current(ii)  # calls PID, sets charger to the current from PID
                elif communicationFlags[ii+4] == 1:  # if you're not asking for charge or done charging, and still communicating
                    end_charge(ii)  # end charging, disconnects chargers
        loopTime = datetime.datetime.now()-timeStamp
        while loopTime.seconds < 1:  # do while the loop time is under 1 second
            loopTime = datetime.datetime.now()-timeStamp  # stalls till 1 second by repeatedly calculating loopTime
