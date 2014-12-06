/*------------------------------------------------------------------------------
 * void pinInital(void)
 * initializes the pins as inputs and outputs and sets up serial communication
 *----------------------------------------------------------------------------*/

void pinInital(void){
  int i=0;
  
  Serial.begin(9600);           // set up Serial library at 9600 bps 
  
  //************************ SPI setup *************************//
  
  pinMode(csBME1, OUTPUT);           // set pin to output
  pinMode(csBME2, OUTPUT);           // set pin to output
  digitalWrite(csBME1, HIGH);
  digitalWrite(csBME2, HIGH);
  SPI.begin(chipSelectPin);
  SPI.setClockDivider(chipSelectPin,ClockDivider);
  SPI.setBitOrder(chipSelectPin,MSBFIRST);
  SPI.setDataMode(chipSelectPin,SPI_MODE0);
  pinMode(MISOPin, INPUT);           // set pin to input
  digitalWrite(MISOPin, HIGH);       // turn on pullup resistors
  
  
  //************************ BME setup *************************// 
  intit_PEC15_table(); //make PEC table
  LTC_initial();
  delay(2);
  for(i=0;i<BMENum;i++){
    BME[i].addr=0x80+(i<<3);
    BME[i].DCC=0;
    BME[i].GPIO=0;
  }
  BMESelfTest();      // self-check all BMEs
  while(minVol==7){
    if(pseudoDataFlag){ 
      PseudoData();
    }
    else{
      getBMEData();     // gets the data from all BMEs
    } 
    minVol=findMinV();
  }
  initalizeSoc();
  //************************ BMU setup *************************// 
  modeInfo.currentMode=STOPMODE;
  modeReset();
  pinMode(relay1, OUTPUT); //pin selected to control
  digitalWrite(relay1, LOW);
  pinMode(relay2, OUTPUT); //pin selected to control
  digitalWrite(relay2, LOW);    
  
  pinMode(frontWPin, INPUT);      // set pin to input
  digitalWrite(frontWPin, HIGH);  // turn on pullup resistors
  pinMode(backWPin, INPUT);       // set pin to input
  digitalWrite(backWPin, HIGH);  // turn on pullup resistors
  
  analogReadResolution(12);
  
  if(pseudoDataFlag){ 
    PseudoData();
  }
  else{
    getBMUData();    // gets/calculates data for the half string
  }
  presOld=pressure;
  intitBiquadFil();
  //************************ Ethernet setup *************************// 
  // start the Ethernet connection and the server:
  
  
  pinMode(Add0, INPUT);       // set pin to input
  digitalWrite(Add0, HIGH);  // turn on pullup resistors
  pinMode(Add1, INPUT);       // set pin to input
  digitalWrite(Add1, HIGH);  // turn on pullup resistors
  pinMode(Add2, INPUT);       // set pin to input
  digitalWrite(Add2, HIGH);  // turn on pullup resistors
  
  BMUNum = !digitalRead(Add0);
  BMUNum |= !digitalRead(Add1)<<1;
  BMUNum |= !digitalRead(Add2)<<2;
  ipadd[3]=170+BMUNum;
  port = 40+BMUNum;
  byte macTempo0[6] = { 0x90, 0xA2, 0xDA, 0x0E, 0xCE, 0x7B };
  byte macTempo1[6] = { 0x90, 0x2A, 0xDA, 0x0E, 0xCE, 0x78 };
  byte macTempo2[6] = { 0x90, 0x2A, 0xDA, 0x0E, 0xCD, 0x9F };
  byte macTempo3[6] = { 0x90, 0x2A, 0xDA, 0x0E, 0xCD, 0x96 };
//  if(uartPrint) Serial.println(BMUNum);
//  if(uartPrint) Serial.println(ipadd[3]);
  
  for(i=0;i<6;i++){
    if(BMUNum==0) mac[i] =  macTempo0[i];
    if(BMUNum==0) mac[i] =  macTempo1[i];
    if(BMUNum==0) mac[i] =  macTempo2[i];
    if(BMUNum==0) mac[i] =  macTempo3[i];
  }
  
  
  IPAddress ip(ipadd);
  server = EthernetServer(port);
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  delay(5); // waits 5 us for configuration
}

/*------------------------------------------------------------------------------
 * void intitBiquadFil(void)
 * Make a PEC table for checking data
 * needed to run during initialization
 *-----------------------------------------------------------------------------*/

void intitBiquadFil()
{
  biPresrate.gain=1;//filter gain
  biPresrate.b0 = 1.0;              //input k coefficient
  biPresrate.b1 = -1.0;              //input k-1 coefficient
  biPresrate.b2 = 0;                //input k-2 coefficient
  biPresrate.a1 = 0;                //output k-1 coefficient
  biPresrate.a2 = 0;                //output k-2 coefficient
  biPresrate.x1 = pressure-presOld;  //filter state
  biPresrate.x2 = biPresrate.x1;    //filter state
}

/*------------------------------------------------------------------------------
 * void intit_PEC15_table(void)
 * Make a PEC table for checking data
 * needed to run during initialization
 *-----------------------------------------------------------------------------*/

void intit_PEC15_table()
{
  int remainder=0;
  for (int i=0;i<256;i++)
  {
    remainder =i<<7;
    for (int b=8; b>0; b--)
    {
      if(remainder & 0x4000)
      {
        remainder=(remainder<<1);
        remainder=(remainder ^ CRC15poly);
      }
      else
      {
        remainder=(remainder<<1);
      }
    }
    pec15Table[i]=remainder & 0xFFFF;
  }
}
    
