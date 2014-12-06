/*------------------------------------------------------------------------------
 * void BMCcomm(void)
 * Sends Battery information and receives commands over ethernet
 * data out: dt, SOC, total voltage, BME voltages, and BME temperatures
 * data in: start, stop, balance, fan, data
 *-----------------------------------------------------------------------------*/

void BMCcomm()
{  
  //if(loopCount%bmcComTime==0){      //execute once a secloop areWeThereYet(BMCcommdt,960000)
  if(areWeThereYet(bmcComTimeStamp,TENSECONDS)) bmcComFlag=true;

   // listen for incoming clients
    EthernetClient client = server.available();
    if (client) {
      BMCcommdt=(int)(timeElapsed(bmcComTimeStamp)/1000); //milliseconds since last communication 
      bmcComTimeStamp=micros();
      if(bmcComFlag) {
        flagBMU=flagBMU & ~(0x008000);
        bmcComFlag=false;
      }
      BMCcommand="";//clearing string for next read
      while (client.available()) {
        char c = client.read();
        if (BMCcommand.length() < 50) {
          BMCcommand += c; //store characters to string 
        }    
      }
      if(BMCcommand.indexOf("cle") >=0) {
        clearFlags();            //clear flags
      }
      else if(BMCcommand.indexOf("ign") >=0)//checks for ignore
      { 
        if(uartPrint)Serial.println("Ignore temperature");
        flagBMU=~(~flagBMU | (0xE));
        flagOverride=~(~flagOverride | (0xE));
        flagIgnoreTemp=true;
      }
      sendData((EthernetClient&) client);
    }
}


/*------------------------------------------------------------------------------
 * void sendData(void)
 * send flag, dt, SOC, bottle voltage, current, cell voltage and temperaturs
 * information out to a client
 *----------------------------------------------------------------------------*/
void sendData(EthernetClient& _Client){
  int i,j;
  
  _Client.print(flagBMU,HEX);
  _Client.print(',');
  _Client.print(BMCcommdt);
  _Client.print(',');
 // _Client.print(BMUNum+1);
  _Client.print(modeInfo.currentMode+1);
  _Client.print(',');
  _Client.print(SOC,2);
  _Client.print(',');
  _Client.print(totalVoltage,2);
//  _Client.print(volSum,2);
//  _Client.print(154.14);
  _Client.print(',');
  _Client.print(current,3);
  _Client.print(',');
  _Client.print(minVol,4);
  _Client.print(',');
  _Client.print(maxVol,4);
  _Client.print(',');
  _Client.print(maxTemp,2);
  _Client.print(',');
  _Client.print(pressure,2);
  _Client.print(',');
  _Client.print(presRate,2);
  
//  int testnum=36700+random(-85,85);
  
  for(j=0;j<BMENum;j++){
    for(i=0;i<cellNum;i++){
      _Client.print(',');
      _Client.print(BME[j].vol[i],HEX);
//      _Client.print(testnum,HEX);
    }
  }
  for(j=0;j<BMENum;j++){
    _Client.print(',');
    _Client.print(BME[j].vSum,HEX);
//    _Client.print(testnum,HEX);
  }
  for(j=0;j<BMENum;j++){
    _Client.print(',');
    _Client.print(BME[j].vref2,HEX);
//    _Client.print(testnum,HEX);
  }
  for(j=0;j<BMENum;j++){
    for(i=0;i<4;i++){
      _Client.print(',');
      _Client.print(BME[j].temp[i],HEX);
//      _Client.print(testnum>>1,HEX);
    }
  }
  for(j=0;j<BMENum;j++){
    _Client.print(',');
    _Client.print(BME[j].iTemp,HEX);
//    _Client.print(22000,HEX);
  }
  _Client.print('\n');
}
