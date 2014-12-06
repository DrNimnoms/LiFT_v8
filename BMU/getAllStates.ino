
/*------------------------------------------------------------------------------
 * void measCalAllstates(void)
 * Measures and calculates all states of half string
 *-----------------------------------------------------------------------------*/
 void measCalAllstates(void){
  if(pseudoDataFlag) 
    PseudoData();
  else 
    getBMEData();               // gets data from all BMEs 
  calStateBME();              // calculates state of BME's
 
  if(!pseudoDataFlag) 
    getBMUData();               //gets data for the half-string
  calStateBMU();              // calculates the state of the half-string
 }

 /*------------------------------------------------------------------------------
 * void getBMUData(void)
 * Measures and calculates data on a half string level 
 *-----------------------------------------------------------------------------*/
 void getBMUData(void){
  fwLeak=!digitalRead(frontWPin);
  bwLeak=!digitalRead(backWPin);
  totalVoltage=avgADC(tVolInPin,4)*volConst;
  if(fakeTotVolFlag) totalVoltage=fakeStuff.totalVoltage;  //
  pressure=avgADC(presIn1Pin,5)*presConst-presOffset;    //get pressure
  
  if (fakePressFlag) fakePressureData();
  current0=avgADC(cur0InPin,3);//analogRead(cur0InPin);
  curMeas=(avgADC(curInPin,3)-(float)current0)*curConst;
  current=curMeas-currentOffset;
  if(fakeCurFlag) current=fakeStuff.current;
//  if (abs(current)<=.4) current=0;

 }
 
 /*------------------------------------------------------------------------------
 * void calStateBMU(void)
 * uses the measured data
 * to calculate the state of the battery and check errors
 *-----------------------------------------------------------------------------*/
 
 void calStateBMU(void){
  
  socCal();                              //calculates the state of charge
  presRate= biquadFilter(biPresrate, pressure);                // filtered pressure rate
//  presRate=rateCal(pressure,presOld);    // calculates pressure rate
//  presOld=pressure;                      //set the old pressure value to the new one
 }
 
 /*------------------------------------------------------------------------------
 * void getBMEData(void)
 * Gets the measured data
 * Collects voltage, flag and temperature readings of the BME's.
 *-----------------------------------------------------------------------------*/
 void getBMEData(void){
  int i;
   
  for(i=0;i<BMENum;i++){ 
    BME[i].dataCheck=false;
  }
  
  CLRCELL(0);          // wake the bme 
  
  ADCV(0,0);          //  broadcast to all channels
  delayMicroseconds(BMEConDelay1);
  
  for(int i=0;i<BMENum;i++){ 
    RDCVA((BMEdata&) BME[i]);
  }
  
  // get cell temperatures
  ADAX(0,0);
  delayMicroseconds(BMEConDelay1);
  
  for(int i=0;i<BMENum;i++){
    RDAUXA((BMEdata&) BME[i]);
    RDAUXB((BMEdata&) BME[i]);  
  }
  
  // get chip temperatures, sum of battery module 
  ADSTAT(0,0);
  delayMicroseconds(BMEConDelay2);
  for(int i=0;i<BMENum;i++){
    RDSTATA((BMEdata&) BME[i]);
    RDSTATB((BMEdata&) BME[i]);
  }
//  BME[8].DCC=6;
  for(i=0;i<BMENum;i++){               // Cycles through BME's
    BME[i].GPIO=0x0f|((!fanOn)<<4);          // Sets the GPIO to 0 or 1 for the multiplexer
    WRCFG((BMEdata&) BME[i]);          // Sends out the GPIO command
  }
  

  if (modeInfo.currentMode==BALANCEMODE && balRelaxFlag) saturateBalanceVoltage();
  if (fakeTempFlag) fakeTemperatureData();
  if (fakeVolFlag) fakeVoltageData();
  for(int i=0;i<BMENum;i++){
    int2float((BMEdata&) BME[i]); // passes pointer to BME[i]
  }
}

 /*------------------------------------------------------------------------------
 * void calStateBME(void)
 * uses the measured data
 * to calculate the state of the battery and check errors
 *-----------------------------------------------------------------------------*/
 
 void calStateBME(void){
  
  if(modeInfo.currentMode!=BALANCEMODE){
    minVol=findMinV();            //updates min cell voltage and total battery-string voltage
    maxVol=findMaxV();            //updates max cell voltage and total battery-string voltage
  }
  else { // update during every relax period of balance mode
    realBalDataFlag=areWeThereYet(balanceTimeStamp,balanceCheckTime+6*controlTime);
    if(realBalDataFlag || !balRelaxFlag){
      minVol=findMinV();            //updates min cell voltage and total battery-string voltage
      maxVol=findMaxV();            //updates max cell voltage and total battery-string voltage
    }
  }
  maxTemp=findMaxT();                  // updates the max temperature reading
  volSumCal();                       // sums all the virtual cell voltages into modules and half-strin voltage
  if(fakeModVolFlag) BME[fakeStuff.BME].modSum=fakeStuff.modSum;
 }

 /*------------------------------------------------------------------------------
 * void BMESelfTest(void)
 * clears the ADC registers and performs a self-test on each BME
 * checks the digital filter of each BME
 *-----------------------------------------------------------------------------*/
 void BMESelfTest(void){
  int i;
  modeInfo.selfCheck=true;
  selfTestFlag =false;
  for(i=0;i<BMENum;i++){           //sets flags to false
    BME[i].dataCheck=false;
    BME[i].muxCheck=false;
    BME[i].volSelfCheck=false;
    BME[i].AuxSelfCheck=false;
    BME[i].StatSelfCheck=false;
  }
  
  CLRCELL(0);          // wake the bme
  CLRCELL(0);          // clears all battery voltage data
  CLRAUX(0);           // clear all battery module temperature readings
  CLRSTAT(0);          // clear all battery module status register

  
  CVST(0);                // start testing voltage sensor's digital filter
  delayMicroseconds(BMEConDelay1);
  for(int i=0;i<BMENum;i++){ 
    RDCVST((BMEdata&) BME[i]); //check results of self test
  }
  
  AXST(0);                // start testing auxiliary sensor's digital filter
  delayMicroseconds(BMEConDelay1);
  for(int i=0;i<BMENum;i++){ 
    RDAXSTA((BMEdata&) BME[i]);  //check results of self test
  }
  
  STATST(0);              // start testing Status sensor's digital filter
  delayMicroseconds(BMEConDelay2);
  DIAGN(0);
  delayMicroseconds(BMEConDelay2);
  for(i=0;i<BMENum;i++){ 
    RDSTATSTA((BMEdata&) BME[i]);  //check results of self test
    RDSTATSTB((BMEdata&) BME[i]);  //check results of self test
  }
  
  for(i=0;i<BMENum;i++){                         // goes through all BMEs
     if(!BME[i].dataCheck){                           // check if BME is communicating       
       if(BME[i].volSelfCheck || BME[i].AuxSelfCheck || BME[i].StatSelfCheck){ //BME[i].muxCheck ||
         selfTestFlag =true;
         if(uartPrint)Serial.print("BME ");
         if(uartPrint)Serial.print(i+1);
         if(uartPrint)Serial.print(": mux ");
         if(uartPrint)Serial.print(BME[i].muxCheck);
         if(uartPrint)Serial.print(", volself ");
         if(uartPrint)Serial.print(BME[i].volSelfCheck);
         if(uartPrint)Serial.print(",auxself ");
         if(uartPrint)Serial.print(BME[i].AuxSelfCheck);
         if(uartPrint)Serial.print(",statself ");
         if(uartPrint)Serial.println(BME[i].StatSelfCheck);
       }
     }  
   }
  // reset cell data
  getBMEData();
  
//  ADAX(0,0);
//  delayMicroseconds(BMEConDelay1);
//  
//  for(int i=0;i<BMENum;i++){
//    RDAUXA((BMEdata&) BME[i]);
//    RDAUXB((BMEdata&) BME[i]);  
//  }
  
 }
 
//////////////////////////// sub Functions //////////////////////// 
 
 /*------------------------------------------------------------------------------
 *  float rateCal(float value, float valueLast)
 * takes in the current and last data point
 * returns the rate
 *-----------------------------------------------------------------------------*/
 
 float rateCal(float value, float valueLast){
   float valueRate=0;
   valueRate=(value-valueLast)/dt;
   
   return valueRate;
 }
 
 /*------------------------------------------------------------------------------
 * float findMinV(void)
 * finds the lowest voltage cell
 *----------------------------------------------------------------------------*/
 float findMinV(void){
   int i,j;
   float _mVol=7.0;
   for(j=0;j<BMENum;j++){
     if(!BME[j].dataCheck){
       for(i=0;i<cellNum;i++){
         if(BME[j].fVol[i] < _mVol){
           _mVol=BME[j].fVol[i];
         }
       }
     }
   }
   return _mVol;
 }

/*------------------------------------------------------------------------------
 * float findMaxV(void)
 * finds the lowest voltage cell
 *----------------------------------------------------------------------------*/
 float findMaxV(void){
   int i,j;
   float _mVol=0;
   for(j=0;j<BMENum;j++){
     if(!BME[j].dataCheck){
       for(i=0;i<cellNum;i++){
         if(BME[j].fVol[i] > _mVol){
           _mVol=BME[j].fVol[i];
         }
       }
     }
   }
   return _mVol;
 }
 
 /*------------------------------------------------------------------------------
 * float findMaxT(void)
 * returns the highest cell temperature
 *----------------------------------------------------------------------------*/
 float findMaxT(void){
   int i,j;
   float _mTemp=-300;
   for(j=0;j<BMENum;j++){
     if(!BME[j].dataCheck){
       for(i=0;i<3;i++){
         if(BME[j].fTemp[i] > _mTemp){
           _mTemp=BME[j].fTemp[i];
         }
       }
     }
   }
   return _mTemp;
 }
 
 /*------------------------------------------------------------------------------
 * void volSumCal(void)
 * returns the sums all the virtual cell voltages
 *----------------------------------------------------------------------------*/
 void volSumCal(void){
   int i,j;
   volSum=0.0;
   for(j=0;j<BMENum;j++){
     BME[j].modSum=0.0;
     for(i=0;i<cellNum;i++){  
       BME[j].modSum+=BME[j].fVol[i];
     }
//     Serial.println(BME[j].modSum);
     volSum+=BME[j].modSum;
   }
 }
 
 
 /*------------------------------------------------------------------------------
 *void saturateBalanceVoltage(void)
 * saturates voltages if balance mode is on
 *----------------------------------------------------------------------------*/
 void saturateBalanceVoltage(void){ 
   int i,j;
   for(j=0;j<BMENum;j++){
     for(i=0;i<cellNum;i++){  
       if(BME[j].vol[i]> (int)(maxVol*10000)){
         BME[j].vol[i]=(int)(maxVol*10000);
//         if(uartPrint)Serial.print("upper saturation is occurring for bme");
//         if(uartPrint)Serial.print(j);
//         if(uartPrint)Serial.print("and layer");
//         if(uartPrint)Serial.println(i);
       }
       else if(BME[j].vol[i]< (int)((minVol-.001)*10000)){
         BME[j].vol[i]=(int)((minVol-.001)*10000);
//         if(uartPrint)Serial.print("lower saturation is occurring for bme");
//         if(uartPrint)Serial.print(j);
//         if(uartPrint)Serial.print(" layer");
//         if(uartPrint)Serial.println(i);
       }

     }
   }
 }
 
 /*------------------------------------------------------------------------------
 * int biquadFilter(int adcPin,int n)
 * applies Biquad filter to the input
 * returns the average
 *-----------------------------------------------------------------------------*/
 float biquadFilter(BiquadType biPram, float input){
    // Apply Filter
    float x = biPram.gain*input - biPram.a1*biPram.x1 - biPram.a2*biPram.x2;
    float out = biPram.b0*x + biPram.b1*biPram.x1 + biPram.b2*biPram.x2;
    // Update states
    biPram.x2 = biPram.x1;
    biPram.x1 = x;
    return out;
 }
 
 /*------------------------------------------------------------------------------
 * int avgADC(int adcPin,int n)
 * average the ADC measurement 2^n times
 * returns the average
 *-----------------------------------------------------------------------------*/
 int avgADC(int adcPin,int n){
  long tempo=0;
  for(int i=0;i<(1<<n);i++){
    tempo+=analogRead(adcPin);
    delay(1);
  }
  tempo=tempo>>n;
  return (int)tempo;
 }
 
 /*------------------------------------------------------------------------------
 * void PseudoData(void)
 * virtual data collector
 *-----------------------------------------------------------------------------*/
 void PseudoData(void){
  int i,j;
  fwLeak=false;
  bwLeak=false;
  totalVoltage=168;
  if(fakeTotVolFlag) totalVoltage=fakeStuff.totalVoltage;
  pressure=avgADC(presIn1Pin,5)*presConst-presOffset;
  if (fakePressFlag) fakePressureData();
  current0=0;
  curMeas=0;
  current=0;
  if(fakeCurFlag) current=fakeStuff.current;
    
  for(int j=0;j<BMENum;j++){ 
    for(int i=0;i<3;i++) {
      
      BME[j].vol[2-i]=40000+random(-20, 20);; // 4V*10000
      BME[j].temp[2-i]=20000; //?
      BME[j].uFlag[i]= false;
      BME[j].oFlag[i]= false;
    }
    BME[j].dataCheck=false;
    BME[j].temp[3] =20000; //? 
    BME[j].vref2 = 30000;//? 3v*10000
    BME[j].vSum = 6000;//? 12V *500
    BME[j].iTemp = 22500;//?  
    BME[j].GPIO = 0x0f|((false)<<4); //? fans?
  }
  
  if (fakeTempFlag) fakeTemperatureData();
  if (fakeVolFlag) fakeVoltageData();  
  for(int i=0;i<BMENum;i++){
    int2float((BMEdata&) BME[i]); // passes pointer to BME[i]
  }

 }
