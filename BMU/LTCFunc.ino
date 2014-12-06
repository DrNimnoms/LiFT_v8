
  const byte CFGRIni[6]={0x00,0x13,0xE6,0xA7,0x00,0x00};  // set VUV=2.4896 and VOV=4.2992
  byte CFGR[6]={0};


/*------------------------------------------------------------------------------
 * void  LTC_initial()
 *  Initializes all BME's
 *----------------------------------------------------------------------------*/
void LTC_initial() {
  byte comm[2]={0x00,0x01};
  for(int i=0;i<6;i++){
    CFGR[i]=CFGRIni[i];
  }
  sendData(&comm[0],2);
  // send register group
  sendDataEnd(&CFGR[0], 6);

}

/*------------------------------------------------------------------------------
 * void WRCFG(byte addr, byte GPIO, byte DCCAB, byte DCCC)
 *  Write Configuration Register Group for one BME
 *----------------------------------------------------------------------------*/
void WRCFG(BMEdata& _BME)
{
  byte comm[2]={0x00,0x01};
  comm[0]=comm[0]|_BME.addr;
  
  
  CFGR[0]=0x7f | (_BME.GPIO<<3);
  for(int i=1;i<4;i++){
    CFGR[i]=CFGRIni[i];
  }
  CFGR[4]=(byte)_BME.DCC;
  CFGR[5]=(_BME.DCC>>8) & B00001111;
  
  sendData(&comm[0],2);  // send the command writing configuration register group
  
  // send register group
  sendDataEnd(&CFGR[0], 6);
  
}


/*------------------------------------------------------------------------------
 * void  ADCV(byte addr, byte _CH)
 * Start Cell Voltage ADC Conversions and Poll Status for a BME and a given cell
 * if addr=0 then it applies to all BMEs
 *----------------------------------------------------------------------------*/
void  ADCV(byte addr, byte _CH)
{
  byte comm[2]={addr|0x02,0x60};
  comm[0]=comm[0]|(BME_MD>>1)| addr;
  comm[1]=comm[1]|(DCP<<4)|(BME_MD<<7);
  
  sendDataEnd(&comm[0],2); // send the command to start ADC on the cells
}

/*------------------------------------------------------------------------------
 * void  ADAX(byte addr, byte _CHG)
 * Start Cell Voltage ADC Conversions and Poll Status for all cells
 *----------------------------------------------------------------------------*/
void  ADAX(byte addr, byte _CHG)
{
  byte comm[2]={addr|0x04,0x60};
  comm[1]=comm[1] |(BME_MD<<7);
  comm[0]=comm[0] |(BME_MD>>1);
  sendDataEnd(&comm[0],2); // send the command to start ADC on the cells
}

/*------------------------------------------------------------------------------
 * void  ADSTAT(byte addr, byte _CHST)
 * Start Cell Voltage ADC Conversions and Poll Status for all cells
 *----------------------------------------------------------------------------*/
void  ADSTAT(byte addr, byte _CHST)
{
  byte comm[2]={addr|0x04,0x68};
  comm[1]=comm[1] |(BME_MD<<7);
  comm[0]=comm[0] |(BME_MD>>1);
  sendDataEnd(&comm[0],2); // send the command to start ADC on the cells
}

/*------------------------------------------------------------------------------
 * void  RDCFG(BMEdata& _BME, byte& CFGRR)
 *  Read Configuration Register Group
 *----------------------------------------------------------------------------*/
void RDCFG(BMEdata& _BME, byte& CFGRR)
{ 
  byte comm[2]={_BME.addr,0x02};
  sendData(&comm[0],2);
  _BME.dataCheck=_BME.dataCheck | readData(&CFGRR, 6);
  //printHexData(&CFGRR_test[0], 6);
}

/*------------------------------------------------------------------------------
 * void  RDCVA(BMEdata& _BME)
 *  Read Cell Voltages 1-3
 *----------------------------------------------------------------------------*/
void RDCVA(BMEdata& _BME)
{
  byte comm[2]={_BME.addr,0x04};
  byte dataIn[6]={0};
  int tempRev[3]={0};
  boolean readCheck=false;
  
  sendData(&comm[0],2); // send the command read the voltage for cells 1-4
  readCheck=readData(&dataIn[0], 6);  // reads the data in
  if(!readCheck){
    parseData((int*) tempRev, (byte*) dataIn,3); // parses the data in to the cell voltages 
    for(int i=0;i<3;i++) _BME.vol[2-i]=tempRev[i];
  }
  _BME.dataCheck=_BME.dataCheck |readCheck;
}


/*------------------------------------------------------------------------------
 * void  RDAUXA(BMEdata& _BME)
 *  Read Cell Voltages of GPIO 1-3
 *----------------------------------------------------------------------------*/
void RDAUXA(BMEdata& _BME)
{
  byte comm[2]={_BME.addr,0x0C};
  byte dataIn[6]={0};
  int tempRev[3]={0};
  boolean readCheck=false;
  
  sendData(&comm[0],2); // send the command read the voltage for cells 1-4
  readCheck=readData(&dataIn[0], 6);  // reads the data in
  if(!readCheck) {
    parseData((int*) tempRev, (byte*) dataIn,3); // parses the data in to the cell voltages 
    for(int i=0;i<2;i++) _BME.temp[i]=tempRev[i+1];
    _BME.temp[3]=tempRev[0];
  }
  _BME.dataCheck=_BME.dataCheck | readCheck;
}

/*------------------------------------------------------------------------------
 * void   RDAUXB(BMEdata& _BME)
 *  Read Cell Voltages of GPIO 4-5 and ref2
 *----------------------------------------------------------------------------*/
void RDAUXB(BMEdata& _BME)
{
  byte comm[2]={_BME.addr,0x0E};
  byte dataIn[6]={0};
  boolean readCheck=false;
  
  sendData(&comm[0],2); // send the command read the voltage for cells 1-4
  readCheck=readData(&dataIn[0], 6);  // reads the data in
  if(!readCheck){
    _BME.temp[2]= (dataIn[1]<<8) | dataIn[0];
    _BME.vref2=(dataIn[5]<<8) | dataIn[4];
  }
  _BME.dataCheck=_BME.dataCheck | readCheck;
}

/*------------------------------------------------------------------------------
 * void  RDSTATA(byte addr)
 *  Read Cell Voltages of battery modual and LTC temperature
 *----------------------------------------------------------------------------*/
void RDSTATA(BMEdata& _BME)
{
  byte comm[2]={_BME.addr,0x10};
  byte dataIn[6]={0};
  boolean readCheck=false;
  
  sendData(&comm[0],2); // send the command read the voltage for cells 1-4
  readCheck=readData(&dataIn[0], 6);  // reads the data in
  if(!readCheck){
    _BME.vSum= (dataIn[1]<<8) | dataIn[0];
    _BME.iTemp= (dataIn[3]<<8) | dataIn[2];
  }
  _BME.dataCheck=_BME.dataCheck | readCheck;
}

/*------------------------------------------------------------------------------
 * void  RDSTATB(BMEdata& _BME)
 *  Read Cell over/under voltages flags 
 *----------------------------------------------------------------------------*/
void RDSTATB(BMEdata& _BME)
{
  byte comm[2]={_BME.addr,0x12};
  byte dataIn[6]={0};
  boolean readCheck=false;
  
  sendData(&comm[0],2); // send the command read the voltage for cells 1-4
  readCheck=readData(&dataIn[0], 6);  // reads the data in
  if(!readCheck){
    for(int i=0;i<cellNum;i++){
      _BME.uFlag[i]= (dataIn[2]>>(2*i)) & 0x01 ;
      _BME.oFlag[i]= (dataIn[2]>>(2*i+1)) & 0x01;
    }
  }
  _BME.dataCheck=_BME.dataCheck | readCheck;
}

/*------------------------------------------------------------------------------
 * void  CLRCELL(byte addr)
 *  Clears the voltage readings in the BME if addr=0 then it applies to all BMEs
 *----------------------------------------------------------------------------*/
void CLRCELL(byte addr)
{
  byte comm[2]={addr|0x07,0x11};
  sendDataEnd(&comm[0],2); 
}

/*------------------------------------------------------------------------------
 * void  CLRAUX(byte addr)
 *  Clears the GPIO readings in the BME if addr=0 then it applies to all BMEs
 *----------------------------------------------------------------------------*/
void CLRAUX(byte addr)
{
  byte comm[2]={addr|0x07,0x12};
  sendDataEnd(&comm[0],2); 
}

/*------------------------------------------------------------------------------
 * void  CLRSTAT(byte addr)
 *  Clears the status readings in the BME if addr=0 then it applies to all BMEs
 *----------------------------------------------------------------------------*/
void CLRSTAT(byte addr)
{
  byte comm[2]={addr|0x07,0x13};
  sendDataEnd(&comm[0],2); 
}

/*------------------------------------------------------------------------------
 * void  DIAGN(byte addr)
 *  Diagnose on MUX and Poll status
 *----------------------------------------------------------------------------*/
void DIAGN(byte addr)
{
  byte comm[2]={addr|0x07,0x15};
  sendDataEnd(&comm[0],2); 
}


/*------------------------------------------------------------------------------
 * void  CVST(byte addr)
 *  start self-test on digital filter and memory of voltage readings
 *----------------------------------------------------------------------------*/
void CVST(byte addr)
{
  byte comm[2]={addr|0x02,0x27};
  byte dataIn[6]={0};
  int dataCheck[3]={0};
  comm[1]=comm[1] |(BME_MD<<7);
  comm[0]=comm[0] |(BME_MD>>1);
  sendDataEnd(&comm[0],2); 
}

/*------------------------------------------------------------------------------
 * void  AXST(byte addr)
 *  start self-test on digital filter and memory of GPIO readings
 *----------------------------------------------------------------------------*/
void AXST(byte addr)
{
  byte comm[2]={addr|0x04,0x47};
  byte dataIn[6]={0};
  int dataCheck[3]={0};
  comm[1]=comm[1] |(BME_MD<<7);
  comm[0]=comm[0] |(BME_MD>>1);
  sendDataEnd(&comm[0],2); 
}

/*------------------------------------------------------------------------------
 * void  STATST(byte addr)
 *  start self-test on digital filter and memory of SATA readings
 *----------------------------------------------------------------------------*/
void STATST(byte addr)
{
  byte comm[2]={addr|0x04,0x2F};
  byte dataIn[6]={0};
  int dataCheck[3]={0};
  comm[1]=comm[1] |(BME_MD<<7);
  comm[0]=comm[0] |(BME_MD>>1);
  sendDataEnd(&comm[0],2); 
}

/*------------------------------------------------------------------------------
 * void  RDCVST(BMEdata& _BME)
 *  read self-test on digital filter and memory of voltage readings
 *----------------------------------------------------------------------------*/
void RDCVST(BMEdata& _BME)
{
  byte comm[2]={_BME.addr,0x04};
  byte dataIn[6]={0};
  int testVol[3];
  boolean readCheck=false;
  
  sendData(&comm[0],2); // send the command read the voltage for cells 1-4
  readCheck=readData(&dataIn[0], 6);  // reads the data in
  if(!readCheck) {
    parseData((int*) testVol, (byte*) dataIn,3); // parses the data in to the cell voltages 
    for(int i=0;i<3;i++)
    {
      if(testVol[i]!=selfCheckVal1) _BME.volSelfCheck=true;
    }
  }
  // else _BME.volSelfCheck=true;
  _BME.dataCheck=_BME.dataCheck |readCheck;
}

/*------------------------------------------------------------------------------
 * void  RDAXSTA(BMEdata& _BME)
 *  read self-test on digital filter and memory of GPIO 1-3
 *----------------------------------------------------------------------------*/
void RDAXSTA(BMEdata& _BME)
{
  byte comm[2]={_BME.addr,0x0C};
  byte dataIn[6]={0};
  int testVol[3];
  boolean readCheck=false;
  
  sendData(&comm[0],2); // send the command read the voltage for cells 1-4
  readCheck=readData(&dataIn[0], 6);  // reads the data in
  if(!readCheck){
    parseData((int*) testVol, (byte*) dataIn,3); // parses the data in to the cell voltages 
    for(int i=0;i<3;i++)
    {
      if(testVol[i]!=selfCheckVal2) _BME.AuxSelfCheck=true;
    }
  }
  else _BME.AuxSelfCheck=true;
  _BME.dataCheck=_BME.dataCheck | readCheck;
}

/*------------------------------------------------------------------------------
 * void  RDAXSTB(BMEdata& _BME)
 *  read self-test on digital filter and memory of GPIO 4-5 and Vref2
 *----------------------------------------------------------------------------*/
void RDAXSTB(BMEdata& _BME)
{
  byte comm[2]={_BME.addr,0x0E};
  byte dataIn[6]={0};
  int testVol[3];
  boolean readCheck=false;
  
  sendData(&comm[0],2); // send the command read the voltage for cells 1-4
  readCheck=readData(&dataIn[0], 6);  // reads the data in
  if(!readCheck){
    parseData((int*) testVol, (byte*) dataIn,3); // parses the data in to the cell voltages 
    for(int i=0;i<3;i++)
    {
      if(testVol[0]!=selfCheckVal2) _BME.AuxSelfCheck=true;
    }
  }
  //else _BME.AuxSelfCheck=true;
  _BME.dataCheck=_BME.dataCheck | readCheck;
}

/*------------------------------------------------------------------------------
 * void  RDSTATSTA(BMEdata& _BME)
 *  read self-test on digital filter and memory of over and under flags
 *----------------------------------------------------------------------------*/
void RDSTATSTA(BMEdata& _BME)
{
  byte comm[2]={_BME.addr,0x10};
  byte dataIn[6]={0};
  int testVol[3];
  boolean readCheck=false;
  
  sendData(&comm[0],2); // send the command read the voltage for cells 1-4
  readCheck=readData(&dataIn[0], 6);  // reads the data in
  if(!readCheck){
    parseData((int*) testVol, (byte*) dataIn,3); // parses the data in to the cell voltages 
    for(int i=0;i<3;i++)
    {
      if(testVol[i]!=selfCheckVal1) _BME.StatSelfCheck=true;
    }
  }
  //else _BME.StatSelfCheck=true;
  _BME.dataCheck=_BME.dataCheck | readCheck;
}

/*------------------------------------------------------------------------------
 * void  RDSTATSTB(BMEdata& _BME)
 *  read self-test on digital filter and memory of battery modual and LTC temperature
 *----------------------------------------------------------------------------*/
void RDSTATSTB(BMEdata& _BME)
{
  byte comm[2]={_BME.addr,0x12};
  byte dataIn[6]={0};
  int testVol[1];
  boolean readCheck=false;
  
  sendData(&comm[0],2); // send the command read the voltage for cells 1-4
  readCheck=readData(&dataIn[0], 6);  // reads the data in
  if(!readCheck){
    parseData((int*) testVol, (byte*) dataIn,1); // parses the data in to the cell voltages 
    if(testVol[0]!=selfCheckVal1){
      _BME.StatSelfCheck=true;
    }
    _BME.muxCheck=(dataIn[5]>>1) & 0x01;
  }
 // else {
   //  if(uartPrint)Serial.println("readCheck is True inside of LTCFunc:RDSTATSTB");
   // _BME.StatSelfCheck=true;
   // _BME.muxCheck=true;
  //  
//  }
  _BME.dataCheck=_BME.dataCheck | readCheck;
}

//////////////////////////// sub Functions ////////////////////////

/*------------------------------------------------------------------------------
 * void parseData(int* dataOut, byte* dataIn, int n)
 *  parse dataIn into dataOut n is length of dataIn/2 or dataOut/3
 *----------------------------------------------------------------------------*/
void parseData(int* dataOut, byte* dataIn, int n)
{
  for(int i=0;i<n;i++){
    dataOut[i]= (dataIn[2*i+1]<<8) | dataIn[2*i];
  }
  
}

/*------------------------------------------------------------------------------
 * void printHexData(byte* dataPrint, byte n)
 * Prints the data vector that is being sent to it in hex
 *----------------------------------------------------------------------------*/
void printHexData(byte *dataPrint, byte n)
{
  for(int i=0;i<n-1;i++){
    if(uartPrint) Serial.print(dataPrint[i],HEX);
    if(uartPrint) Serial.print(", ");
  }
  if(uartPrint) Serial.print(dataPrint[n-1],HEX);
  if(uartPrint) Serial.print('\n');
} 

/*------------------------------------------------------------------------------
 * boolean readData(byte* inData, byte n)
 *  Reads that data with length n
 *----------------------------------------------------------------------------*/
boolean readData(byte* inData, byte n)
{
  int PEC,address;
  
  PEC=16;//PEC seed
  for(int i=0;i<n;i++){
    inData[i]=SPI.transfer(chipSelectPin,0x00,SPI_CONTINUE);    // reads the data coming in
    address=((PEC>>7)^inData[i])&0xFF;// calculate PEC table address
    PEC= (PEC<<8)^pec15Table[address];
  }
  PEC=(PEC<<1)&0xFFFF;    //the CRC15 has a 0 in the LSB so the final value must be multiplied by 2
  int PECCheck=SPI.transfer(chipSelectPin,0x00,SPI_CONTINUE)<<8;    // gets the PEC1 to check if the data is right 
  PECCheck|=SPI.transfer(chipSelectPin,0x00,SPI_LAST);    // gets the PEC0 to check if the data is right 
  digitalWrite(csBME1, HIGH);
  digitalWrite(csBME2, HIGH);
  if(PEC != PECCheck)return true;
  else return false;

}

/*------------------------------------------------------------------------------
 * void sendDatanEnd(byte* command, byte n)
 * sends dataOut to the BME and the corresponding PEC
 *----------------------------------------------------------------------------*/
void sendDataEnd(byte *dataOut, byte n)
{
  digitalWrite(csBME1, LOW);
  digitalWrite(csBME2, LOW);
//  delayMicroseconds(1000);
  int PEC,address;
  PEC=16;//PEC seed
  for(int i=0;i<n;i++){
    SPI.transfer(chipSelectPin,dataOut[i],SPI_CONTINUE);    // write the data coming in
    address=((PEC>>7)^dataOut[i])&0xFF;// calculate PEC table address
    PEC= (PEC<<8)^pec15Table[address];
  }
  PEC=(PEC<<1);//the CRC15 has a 0 in the LSB so the final value must be multiplied by 2
  byte PEC0=PEC&0xff;
  byte PEC1=PEC>>8&0xff;
  SPI.transfer(chipSelectPin,PEC1,SPI_CONTINUE);
  SPI.transfer(chipSelectPin,PEC0,SPI_LAST);
  digitalWrite(csBME1, HIGH);
  digitalWrite(csBME2, HIGH);
}

/*------------------------------------------------------------------------------
 * void sendDatan(byte* command, byte n)
 * sends dataOut to the BME and the corresponding PEC
 *----------------------------------------------------------------------------*/
void sendData(byte *dataOut, byte n)
{
  digitalWrite(csBME1, LOW);
  digitalWrite(csBME2, LOW);
//  delayMicroseconds(10);
  int PEC,address;
//  SPI.transfer(chipSelectPin,0x88,SPI_CONTINUE);    
//  digitalWrite(chipSelectPin, LOW); 
//  delay(10);
  PEC=16;//PEC seed
  for(int i=0;i<n;i++){
    SPI.transfer(chipSelectPin,dataOut[i],SPI_CONTINUE);    // write the data coming in
    address=((PEC>>7)^dataOut[i])&0xFF;// calculate PEC table address
    PEC= (PEC<<8)^pec15Table[address];
  }
  PEC=(PEC<<1);//the CRC15 has a 0 in the LSB so the final value must be multiplied by 2
  byte PEC0=PEC&0xff;
  byte PEC1=PEC>>8&0xff;
  SPI.transfer(chipSelectPin,PEC1,SPI_CONTINUE);
  SPI.transfer(chipSelectPin,PEC0,SPI_CONTINUE);
}

/*------------------------------------------------------------------------------
 * unsigned int PEC15(byte *dataIn, byte len)
 * updates the PEC with the given dataIn
 *----------------------------------------------------------------------------*/
unsigned int PEC15(byte *dataIn, byte len)
{ 
  int remainder,address;
  
  remainder=16;//PEC seed
  for (int i=0; i<len;i++)
  {
    address=((remainder>>7)^ dataIn[i])&0xFF;// calculate PEC table address
    remainder= (remainder<<8)^pec15Table[address];
  }
  return (remainder*2);//the CRC15 has a 0 in the LSB so the final value must be multiplied by 2
}
