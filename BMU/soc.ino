/*------------------------------------------------------------------------------
 * void socCal(void)
 * calculates state of charge of the battery
 *----------------------------------------------------------------------------*/
void socCal(){
  
  if(maxVol>=4.2 && current<=4.5 && modeInfo.currentMode==CHARGEMODE) cap=capMax;
  else if(abs(current)>.3) cap+=current*capConst;
  if(cap>capMax) cap=capMax;
  if(cap<0) cap=0; 
  SOC=cap/capMax*100;
  
}


/*------------------------------------------------------------------------------
 * void initalizeSoc(void)
 * calculates state of charge of the battery
 *----------------------------------------------------------------------------*/
void initalizeSoc(){
//  Serial.println(int(minVol*1000)); 
  SOC=findIntSoc(int(minVol*1000))/10.0;
  if(SOC>100) SOC=100;
  if(SOC<0) SOC=0; 
  cap=SOC*capMax*0.01;
//  Serial.println(SOC);
}
/*------------------------------------------------------------------------------
 * int findIntSoc(int lVol
 * Finds the SOC given inital voltage
 *----------------------------------------------------------------------------*/
int findIntSoc(int lVol){
 int lo=-1,hi=1019, mid=0; 
 while(hi-lo>1){
   mid=(lo+hi)/2;
   if(lookUpVoltage[mid] >= lVol) lo=mid;
   else hi=mid;
 }
 if(lookUpVoltage[lo] == lVol) hi=lo;
 return lookUpSOC[hi];
}

