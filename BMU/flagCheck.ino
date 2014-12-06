/*------------------------------------------------------------------------------
 * void checkFlags(void)
 * Checks and sets all BMU flags burritos
 *----------------------------------------------------------------------------*/
void checkFlags(void){
  leakCheck();      //checks the front and back leak sensors
  bmeCommCheck();   //checks the communication of all BME's
  if(modeInfo.currentMode!=BALANCEMODE){
    fanOn = false;
    volCheck();      //checks the voltage for over and under voltage
    bmeFlagCheck();  //checks the BME's self-checks flags
  }
  else if (!balRelaxFlag || realBalDataFlag){ // check during every relax period of balance mode
    volCheck();      //checks the voltage for over and under voltage
    bmeFlagCheck();  //checks the BME's self-checks flags
  }
  tempCheck();     //checks the temperature for over temperature
  currentCheck();  //checks the current 
  pressurCheck();      //checks additional sensors
  timeoutCheck();
  setFlag();       // sets the flag to be sent out 
}

 /*------------------------------------------------------------------------------
 * void leakCheck(void)
 * checks to the if there is a leak in front or back of the half-string
 *-----------------------------------------------------------------------------*/
 void leakCheck(){
  static int fwLeakCounter=0;
  static int bwLeakCounter=0;
  leakFlag=false;
  if(fwLeak) fwLeakCounter++;
  else fwLeakCounter=0;
  if(bwLeak) bwLeakCounter++;
  else bwLeakCounter=0;
  if((bwLeakCounter > 1) || (fwLeakCounter>1)) leakFlag=true;  //checks the leak sensors
 }
/*------------------------------------------------------------------------------
 * void timeoutCheck()
 * time out after 10 hours of chargeing or balancing
 *----------------------------------------------------------------------------*/
 void timeoutCheck(){
   unsigned long timeSince= timeElapsed(modeInfo.timeKeepingStamp);  // update time in the mode
   modeInfo.timeKeepingStamp=micros();
   modeInfo.microseconds+=timeSince;   
   if (modeInfo.microseconds>ONEMINUTE){
       modeInfo.microseconds-=ONEMINUTE;
       modeInfo.minutes++;
   }
   if (modeInfo.minutes>60){
       modeInfo.minutes-=60;
       modeInfo.hours++;
   }
   timeoutFlag=false;      // Check if charging or balance time > 8 hours
   if(modeInfo.currentMode==CHARGEMODE || modeInfo.currentMode==BALANCEMODE){
     if (modeInfo.hours>=8) timeoutFlag= true;  // set Timeout flag
   }
//   if(uartPrint)Serial.print("Mode(");
//   if(uartPrint)Serial.print(modeInfo.currentMode);
//   if(uartPrint)Serial.print("), Hours: ");
//   if(uartPrint)Serial.print(modeInfo.hours);
//   if(uartPrint)Serial.print(",Minutes: ");
//   if(uartPrint)Serial.print(modeInfo.minutes);
//   if(uartPrint)Serial.print(",uSecs: ");
//   if(uartPrint)Serial.println(modeInfo.microseconds);

 }

/*------------------------------------------------------------------------------
 * void currentCheck(void)
 * checks the current sensor reading
 *----------------------------------------------------------------------------*/
 void currentCheck(){
   driveCurflag=false;          //Current >2A durring Drive
   chargeCurFlag=false;      //Current > 92A or current <2A during Charge
   stopCurFlag=false;     //abs(Current)>1A

   if(modeInfo.currentMode==DRIVEMODE && current >= inCurLimit) driveCurflag= true;
   if(modeInfo.currentMode==CHARGEMODE && (current >= highInCur || current <= highOutCur)) chargeCurFlag= true;
   if((modeInfo.currentMode==STOPMODE || modeInfo.currentMode==BALANCEMODE) && abs(current) >= inOutCur) stopCurFlag= true;
 }

/*------------------------------------------------------------------------------
 * void pressurCheck(void)
 * checks the pressure sensor and the calculated pressure rate
 *----------------------------------------------------------------------------*/
 void pressurCheck(){
   presRateFlag= false;    //Pressure rate > .25 PSI/sec
   presFlag =false;      // Pressure < 0.5 PSI or Pressure  > 5.0 PSI
   if(abs(presRate)>=presRateHigh) presRateFlag= true;
   if((pressure>=presHighLimit) || (pressure<=presLowLimit)) presFlag= true;
 }

/*------------------------------------------------------------------------------
 * void bmeCommCheck(void)
 * checks the communication of all BME's
 *----------------------------------------------------------------------------*/
 void bmeCommCheck(void){
   static int dataCheckCounter[BMENum]={0};
   bmeComFlag=false;  // Communication failure occurs between BMU and BME
   
   for(int j=0;j<BMENum;j++){                         // goes through all BMEs
     if(BME[j].dataCheck) dataCheckCounter[j]++;
     else dataCheckCounter[j]=0;
     if(dataCheckCounter[j]>1){
       bmeComFlag=true;          // set communication error flag
       if(uartPrint)Serial.print("dataCheck error on bME ");
       if(uartPrint)Serial.println(j);
     }
   }     
 }

/*------------------------------------------------------------------------------
 * void bmeFlagCheck(void)
 * checks the BME's self-checks flags
 *----------------------------------------------------------------------------*/
 void bmeFlagCheck(void){
   bmeAlarmFlag=false; // If any cell over/under voltage failures or self-check failures are sent from BME 
   
   for(int j=0;j<BMENum;j++){                         // goes through all BMEs
     if(!BME[j].dataCheck){                           // check if BME is communicating
       for (int i=0;i<cellNum;i++){                   // goes through all virtual cells in a BME
         if(BME[j].uFlag[i] || BME[j].oFlag[i]){
           bmeAlarmFlag=true;
           if(uartPrint)Serial.print("BME ");
           if(uartPrint)Serial.print(j+1);
           if(uartPrint)Serial.print(" layer ");
           if(uartPrint)Serial.print(i+1);
           if(uartPrint)Serial.print(" bmeAlarmflag ");
         }      
       }
     }  
   }
   if(selfTestFlag) bmeAlarmFlag=true;   
 }

/*------------------------------------------------------------------------------
 * void tempCheck(void)
 * temperature check on all cells, heat sinks, and chip temperatures for 
 * temperature warnings and temperature errors
 *----------------------------------------------------------------------------*/
void tempCheck(void){
 // set flags to false before starting the temperature check
 tempAlarmFlag=false;
 tempWarnFlag=false;
 tempFailFlag=false;
 
 //
 for(int j=0;j<BMENum;j++){                         // goes through all BMEs
   if(!BME[j].dataCheck){                           // check if BME is communicating
     for (int i=0;i<cellNum;i++){                   // goes through all virtual cells in a BME
       if(!BME[j].ignoreT[i]){
         if(BME[j].fTemp[i] <-2 || BME[j].fTemp[i]== 'nan'){  //check and set temperature sensor failure flag
           if(flagIgnoreTemp) BME[j].ignoreT[i]=true;
           else tempFailFlag= true;
         }
         else if(BME[j].fTemp[i] >= tempVCWarn){           // check virtual cell temperature for temperature warning
           if(BME[j].fTemp[i] >= tempVCAlarm){        // check virtual cell temperature for temperature error
             if(flagIgnoreTemp) BME[j].ignoreT[i]=true;
             else tempAlarmFlag=true;                          // set temperature alarm flag
           }
           else {
             if(flagIgnoreTemp) BME[j].ignoreT[i]=true;
             else tempWarnFlag=true;                             // set temperature warnning flag
           }
         }
       }
     }
     if(!BME[j].ignoreT[3]){
       if( BME[j].fTemp[3] <-2 || BME[j].fTemp[3]== 'nan' ){ //check and set temperature sensor failure flag
         if(flagIgnoreTemp) BME[j].ignoreT[3]=true;
         else tempFailFlag= true; 
       } 
       else if( BME[j].fTemp[3] >= tempHSWarn){        // check heat sink and chip temperature for temperature warning
         if(BME[j].fTemp[3] >= tempHSAlarm){    // check heat sink and chip temperature for temperature warning
             if(flagIgnoreTemp) BME[j].ignoreT[3]=true;
             else tempAlarmFlag=true;                          // set temperature alarm flag
         }
         else{
           if(flagIgnoreTemp) BME[j].ignoreT[3]=true;
           else tempWarnFlag=true;                             // set temperature warnning flag
         }
       }
     }
     if(!BME[j].ignoreiT){
       if(BME[j].fiTemp >= tempTiWarn){
         if(BME[j].fiTemp >= tempTiAlarm){
            if(flagIgnoreTemp) BME[j].ignoreiT=true;
             else tempAlarmFlag=true;                          // set temperature alarm flag
         }
         else{
           tempWarnFlag=true;                             // set temperature warnning flag
         }
       }
     }
   }
 } 
 flagIgnoreTemp=false;
}

/*------------------------------------------------------------------------------
 * void volCheck(void)
 * Checks the Voltage for all the batteries and sets a flag if it is outside 
 * the required range also checks balance recommendation
 *----------------------------------------------------------------------------*/
void volCheck(void){
  // set flags to false before starting the voltage check
  volHighAlarmFlag =false;      //Any VC voltage > 4.205 V 
  balRecFlag=false;             //Any VC voltage < 3.9 V
  volLowWarnFlag =false;     //Any VC voltage < 3.2 V
  volLowBalAlarmFlag = false;
  volLowAlarmFlag =false;    //Any VC voltage < 3.0 V
  deadBatAlarmFlag=false;    //Any VC voltage < 2.5 V
  volFailFlag =false;      //Any VC voltage < .1 V or >6.5 or Vref2<2.978 or>3.020
  boolean misTempo=false;
  volMisFlag =false;      /*5V difference between overall half-string voltage and sum of half-string VC voltages or
                                     50mV difference between battery module voltage and sum of its VC voltages*/
                                     
  for(int j=0;j<BMENum;j++){                // goes through all BMEs
   if(!BME[j].dataCheck){                   // check if BME is communicating
     if ( (BME[j].fVref2 > 3.020) || (BME[j].fVref2 < 2.978)){
       volFailFlag = true;             // set voltage failure flag
     } 
     if(abs(BME[j].modSum-BME[j].fVSum)>=volModMismatch){
       misTempo =true;
       Serial.print("BME ");
       Serial.print(j+1);
       Serial.print(": modSum ");
       Serial.print(BME[j].modSum);
       Serial.print(" and fVsum ");
       Serial.print(BME[j].fVSum);
       Serial.println(" are mismatched");
     }

   }
  }
  if(abs(totalVoltage-volSum)>=volMismatch){
    misTempo = true;
    if(uartPrint)Serial.print("MISMATCH! totalVoltage:");
    if(uartPrint)Serial.print(totalVoltage);
    if(uartPrint)Serial.print(" and volsum:");
    if(uartPrint)Serial.print(volSum);
    if(uartPrint)Serial.println("are mismatched");
  }
  if(maxVol >= 6.5 ){     // check virtual cell voltage sensor for failure 
       volFailFlag = true;             // set voltage failure flag
  } 
  else if((maxVol >= volHighAlarm) || (modeInfo.currentMode==CHARGEMODE && maxVol>=(charge2Vol+0.01))){  // check virtual cell voltage for high voltage flag
    volHighAlarmFlag  = true;          // set high voltage error flag
    if(uartPrint) Serial.print("high voltage alarm: ");
    if(uartPrint) Serial.println(maxVol,4);
  }  
  
  if(minVol <= 0.0) volFailFlag = true;             // set voltage failure flag
  else if(minVol <= deadBatAlarm && modeInfo.currentMode!=DRIVEMODE){
    deadBatAlarmFlag=true;  // set dead battery alarm
    if(uartPrint) Serial.print("dead battery alarm: ");
    if(uartPrint) Serial.println(minVol,4);
  }
  else if(minVol <= volLowAlarm && modeInfo.currentMode!=CHARGEMODE){
    volLowAlarmFlag=true;    // low voltage alarm
    if(uartPrint) Serial.print("low voltage alarm: ");
    if(uartPrint) Serial.println(minVol,4);
  }
  else if(minVol <= volLowWarn  && modeInfo.currentMode!=CHARGEMODE){
    volLowWarnFlag = true;    //  low voltage warning  
    if(uartPrint) Serial.print("low battery warning: ");
    if(uartPrint) Serial.println(minVol,4);
  }    
  else if((balance2Vol <= volLowBalAlarm || minVol <= volLowBalAlarm) && (BMCcommand.indexOf("bal") >=0||modeInfo.currentMode==BALANCEMODE)){ 
    volLowBalAlarmFlag = true;    //  low voltage warning
    if(uartPrint) Serial.print("low balancing alarm: ");
    if(uartPrint) Serial.println(minVol,4);
  }
  if((maxVol-minVol)>=balRecVol && modeInfo.currentMode!=BALANCEMODE && minVol>=balRecLimit ){
    balRecFlag=true;    // set balance recomanded flag
  }   
  
  if(misTempo) mismatchCount++;
  else mismatchCount=0;
  if(mismatchCount>1) volMisFlag =true;
}

/*------------------------------------------------------------------------------
 * void setFlag(void)
 * Sets the flag
 *----------------------------------------------------------------------------*/
 void setFlag(void){
   if(leakFlag) flagBMU=flagBMU | 1;    // fwleak and bwleak get set in getBMUdata
   if(tempWarnFlag) flagBMU=flagBMU | (1<<1);
   if(tempAlarmFlag) flagBMU=flagBMU | (1<<2);
   if(tempFailFlag) flagBMU=flagBMU | (1<<3);
   if(presRateFlag) flagBMU=flagBMU | (1<<4);
   if(presFlag) flagBMU=flagBMU | (1<<5); 
   if(volHighAlarmFlag) flagBMU=flagBMU | (1<<6);
   if(volLowBalAlarmFlag) flagBMU=flagBMU | (1<<7);
   if(volLowWarnFlag) flagBMU=flagBMU | (1<<8);
   if(volLowAlarmFlag) flagBMU=flagBMU | (1<<9);
   if(deadBatAlarmFlag) flagBMU=flagBMU | (1<<10);
   if(volFailFlag) flagBMU=flagBMU | (1<<11);
   if(volMisFlag) flagBMU=flagBMU | (1<<12);
   if(bmeAlarmFlag) flagBMU=flagBMU | (1<<13);
   if(bmeComFlag) flagBMU=flagBMU | (1<<14);
   if(bmcComFlag) flagBMU=flagBMU | (1<<15);
   if(driveCurflag) flagBMU=flagBMU | (1<<16);
   if(chargeCurFlag) flagBMU=flagBMU | (1<<17);
   if(stopCurFlag) flagBMU=flagBMU | (1<<18);
   if(timeoutFlag) flagBMU=flagBMU | (1<<19);
   if(chargeDoneFlag) flagBMU=flagBMU | (1<<20);      
   if(balDoneFlag) flagBMU=flagBMU | (1<<21);
   if(balRecFlag) flagBMU=flagBMU | (1<<22); 
   
   flagBMU= flagBMU & ~flagOverride;
 }
 
 /*------------------------------------------------------------------------------
 * void setPriority(void)
 * Sets the flag
 *----------------------------------------------------------------------------*/
 void setPriority(void){
   
   flagPriority=0;
   
   if(modeInfo.currentMode==DRIVEMODE){
     if((flagBMU & 0x400000) !=0) flagPriority=5;   //  B10000000000000000000000
     if((flagBMU & 0x00110A) !=0) flagPriority=4;  //  
     if((flagBMU & 0x008000) !=0) flagPriority=3;
     if((flagBMU & 0x016A75) !=0) flagPriority=2;    
   }
   else if(modeInfo.currentMode==STOPMODE){
     if((flagBMU & 0x400000) !=0) flagPriority=5;   //  B10000000000000000000000
     if((flagBMU & 0x00192A) !=0) flagPriority=4;  //  
     if((flagBMU & 0x04E655) !=0) flagPriority=3;  // 
   }  
   else if(modeInfo.currentMode==BALANCEMODE){
     if((flagBMU & 0x200000) !=0) flagPriority=6;   //  B10100000000000000000000
     if((flagBMU & 0x00102A) !=0) flagPriority=2;  //  B00000000000000000101010
     if((flagBMU & 0x0CE8D5) !=0) flagPriority=1;  //  
   }
   else if(modeInfo.currentMode==CHARGEMODE){
     if((flagBMU & 0x100000) !=0) flagPriority=6;
     if((flagBMU & 0x400000) !=0) flagPriority=5;   //  B01000000000000000000000
     if((flagBMU & 0x00002A) !=0) flagPriority=2;   // 
     if((flagBMU & 0x0AFC55) !=0) flagPriority=1;  //  
   }  
 }
 
 /*------------------------------------------------------------------------------
 * void clearFlags(void)
 * clears all flags
 *----------------------------------------------------------------------------*/
 void clearFlags(void){
   if(uartPrint)Serial.println("inside of clearFlags()");
   flagBMU=0;
   flagPriority=0;
   stopUntil=false; 
   fanOn= false;
   flagOverride=0;
   chargeDoneFlag=false;
   balDoneFlag=false;
   leakFlag=false;
   //debug stuff
   fakeVolFlag=false;
   fakePressFlag=false;
   fakeTempFlag=false;
   fakeTotVolFlag=false;
   fakeModVolFlag=false;
   fakeCurFlag=false;
   selfTestFlag =false;
   for(int j;j<BMENum;j++){                // goes through all BMEs
     BME[j].ignoreiT=false;
     for (int i=0;i<4;i++){
       BME[j].ignoreT[i]=false;
       BME[j].uFlag[i]=false;
       BME[j].oFlag[i]=false;
     }
   }
 }
