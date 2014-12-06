
 /*------------------------------------------------------------------------------
 * void printout(void)
 * prints information out on the screen for all BME's
 *----------------------------------------------------------------------------*/
 void serialEvent() {
  inputString = "";
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
//    Serial.println(inputString);
  }
  debugCommand(inputString);
 }

/*------------------------------------------------------------------------------
 * void BMEcheck(void)
 * checks to see if BMEs are comunicating 
 *----------------------------------------------------------------------------*/
 void BMEcheck(){
   
   for(int i=0;i<BMENum;i++){
     if(BME[i].dataCheck){
       Serial.print("ERROR Communicating with BME ");
       Serial.println((i+1));
     }
     else{
       Serial.print("Found BME ");
       Serial.println((i+1));
     }
   } 
   Serial.println("--------------------------------------------------");
 }
 
 /*------------------------------------------------------------------------------
 * void printout(void)
 * prints information out on the screen for all BME's
 *----------------------------------------------------------------------------*/
 void printOut(){
   
   for(int i=0;i<BMENum;i++){
     if(BME[i].dataCheck){
       Serial.print("ERROR Communicating with BME ");
       Serial.println((i+1));
     }
     else{
       printOutBME((BMEdata&)BME[i]);
     }
   } 
 }
 
 /*------------------------------------------------------------------------------
 * void printout(void)
 * prints information out on the screen for one BME
 *----------------------------------------------------------------------------*/
void printOutBME(BMEdata& _BME){

  int i=0;
  
  Serial.print("-------------------------------------------------------------------------------------\n");
  if(_BME.dataCheck){
     Serial.print("ERROR Communicating with BME ");
     Serial.println(((_BME.addr-0x80)>>3)+1);
  }
  else{
     Serial.print("BME ");
     Serial.print(((_BME.addr-0x80)>>3)+1);
     
    Serial.print("  Voltages \n");
    Serial.print("Cell 1   \t\t   Cell 2 \t\t   Cell 3 \t\t Cell Sum \t\t V_ref2 \n");
    for(i=0;i<cellNum;i++){
      Serial.print(_BME.fVol[i],4);
      Serial.print(" V\t\t");
    }
    Serial.print(_BME.fVSum,3);
    Serial.print(" V\t\t");
    Serial.print(_BME.fVref2,4);
    Serial.println(" V");
  }
}


/*------------------------------------------------------------------------------
 * void testBMU(void)
 * prints information out on the screen for one BME
 *----------------------------------------------------------------------------*/
 void testBMU(){
  Serial.println("-------------------------------------------------------------------------------------");
  Serial.println("BMU sensors"); 
  Serial.print("Current:"); 
  if (current < 0){
    Serial.print('-');
    Serial.print(-1*current,2);
  }
  else{
    Serial.print(current,2);
  }
  Serial.print(" A\t Voltage:");
  Serial.print(totalVoltage,2);
  Serial.print(" V\t layerSum:");
  Serial.print(volSum,2);
  Serial.print(" V\t Pressure:");
  Serial.print(pressure,1);
  Serial.print(" PSIG\t waterleak: ");
  if(!fwLeak & !bwLeak) Serial.print("no leak");
  else if(!fwLeak & bwLeak) Serial.print("back leak");
  else if(fwLeak & !bwLeak) Serial.print("front leak");
  else if(fwLeak & bwLeak) Serial.print("both leak");
  Serial.print(" \nFlag: ");
  Serial.print(flagBMU,HEX);
  Serial.print(" \t SOC: ");
  Serial.print(SOC,2);
  Serial.print(" %\t Vsum: ");
  Serial.print(volSum,2);
  Serial.print("V \t minVol: ");
  Serial.print(minVol,4);
  Serial.print("V \t maxVol: ");
  Serial.print(maxVol,4);
  Serial.print("V \t maxTemp: ");
  Serial.print(maxTemp,2);
  Serial.println(" C");
  Serial.print("Current Mode:\t");
  switch (modeInfo.currentMode+1) {
    case 1:
      Serial.println("STOP");
      break;
    case 2:
      Serial.println("DRIVE");
      break;
    case 3:
      Serial.println("CHARGE");
      break;
    case 4:
      Serial.println("BALANCE");
      break;
    default:
      Serial.println("Somethings Wrong");
      break;
  }
//  Serial.print(presRate,2);
 }
 
/*------------------------------------------------------------------------------
 * void testBME(void)
 * prints information out on the screen for one BME
 *----------------------------------------------------------------------------*/
void testBME(BMEdata& _BME){

  int i=0;
  
//  int2float((BMEdata&) _BME);
  Serial.print("-------------------------------------------------------------------------------------\n");
  if(BME[i].dataCheck){
     Serial.print("ERROR Communicating with BME ");
     Serial.println(((_BME.addr-0x80)>>3)+1);
  }
  else{
     Serial.print("BME ");
     Serial.println(((_BME.addr-0x80)>>3)+1);
  }
  Serial.print(" Voltages \n");
  Serial.print("Cell 1   \t\t   Cell 2 \t\t   Cell 3 \t\t Cell Sum \t\t V_ref2 \n");
  for(i=0;i<cellNum;i++){
    Serial.print(_BME.fVol[i],4);
    Serial.print(" V\t\t");
  }
  Serial.print(_BME.fVSum,3);
  Serial.print(" V\t\t");
  Serial.print(_BME.fVref2,4);
  Serial.println(" V");
  Serial.print("BME ");
  Serial.print(((_BME.addr-0x80)>>3)+1);
  Serial.print(" Temperatures \n");
  for(i=0;i<4;i++){
    Serial.print(_BME.fTemp[i],1);
    Serial.print(" C\t\t");
  }
  Serial.print(_BME.fiTemp,1);
  Serial.print(" C\n");
  Serial.print("Over Voltage Flags C1 C2 C3\n");
  for(i=0;i<cellNum;i++){
    Serial.print(_BME.oFlag[i]);
    Serial.print("\t");
  }
  Serial.print("\nUnder Voltage Flags C1 C2 C3\n");
  for(i=0;i<cellNum;i++){
    Serial.print(_BME.uFlag[i]);
    Serial.print("\t");
  }
  Serial.print("\nSelf-tests \n");
  Serial.print("DataCheck \t MUX Decoder   \t   Volt Mem \t   GPIO Mem \t Status Mem \n");
  if(_BME.dataCheck) Serial.print("Fail\t\t");
  else Serial.print("Pass\t\t");
  if(_BME.muxCheck) Serial.print(" Fail\t\t   ");
  else Serial.print(" Pass\t\t   ");
  if(_BME.volSelfCheck) Serial.print("Fail\t\t   ");
  else Serial.print("Pass\t\t   ");
  if(_BME.AuxSelfCheck) Serial.print("Fail\t\t ");
  else Serial.print("Pass\t\t ");
  if(_BME.StatSelfCheck) Serial.println("Fail\t\t");
  else Serial.println("Pass\t\t");
}

/*------------------------------------------------------------------------------
 * void debugCommand(void)
 * does the command that is sent from the GUI
 *----------------------------------------------------------------------------*/
void debugCommand(String input){
  char debugStrings[8][8];
  int sVal=0;
  int temp=0;
  int counter=0;
  int eVal=input.indexOf('\n',sVal+1); //find the end of the command
  String aString="";
  sVal=input.indexOf("_",temp); // find next underscore after temp

 do{
  aString=input.substring(temp,sVal); //grabe string bewteen temp and sVal
  temp=sVal+1; // set temp to the next position after sVal
  sVal=input.indexOf("_",temp); // find next underscore after temp
  aString.toCharArray(debugStrings[counter], 8);
  counter++;
 }while(sVal>0);
aString=input.substring(temp,eVal); //grab final string
aString.toCharArray(debugStrings[counter], 8);
aString=debugStrings[0];  

  if(aString.indexOf("pri") >=0){
    if (counter=1){
      aString = debugStrings[1];
      int i = aString.toInt();
      if(i>0 && i<15)testBME((BMEdata&)BME[i-1]);
      else testBMU();
    }
    else{
      testBMU();
      for(int j=0;j<14;j++){
        printOutBME((BMEdata&)BME[j]);
      }
    }
  }
  else if(aString.indexOf("fon") >=0){
    fanOn=true; 
    if(uartPrint)Serial.println("Fan On ");
  }
  else if(aString.indexOf("fof") >=0){
    fanOn=false; 
    if(uartPrint)Serial.println("Fan Off ");
  }
  else if(aString.indexOf("don") >=0) {
    dischargeTest();
    if(uartPrint)Serial.println("Resistors On ");
  }
  else if(aString.indexOf("dof") >=0) {
    stopBal();
    BMCcommand="";
    if(uartPrint)Serial.println("Resistors Off ");
  }
  else if(input.indexOf("help") >=0) {
    if(uartPrint){
      Serial.println("stop");
      Serial.println("drive");
      Serial.println("charge");
      Serial.println("discharge test: drt_module_layers: eg. drt_5_12");
      Serial.println("balance_bal2int: balance_38000");
      Serial.println("override_overflagInt_");
      Serial.println("ignore");
      Serial.println("clear or reset");
      Serial.println("print_x to read data from x bme: print_11");
      Serial.println("fon to turn fan on");
      Serial.println("fof to turn fan off");
      Serial.println("don to turn resistors on");
      Serial.println("dof to turn resistors off");
      Serial.println("recbal_0 or recbal_1 to toggle recommend balance flag");
    }
  }
   
  else if(aString.indexOf("drt") >=0){
    if(uartPrint)Serial.println("Discharge Test");
    int sVal=input.indexOf("_"); //find the first underscore
    int eVal=input.indexOf('\n',sVal+1); //find the end of the command
    String drtString=input.substring(sVal+1,eVal); //grab command after "drt_"
    drtString=String("deb_1_" + drtString);
    BMCcommand=drtString;
  }
  else if(aString.indexOf("flag") >=0){
    aString=debugStrings[1];
    temp = aString.toInt();
    flagDebugTest(temp);
  }
  else if(aString.indexOf("fake") >=0){ 
   aString=debugStrings[1];
    if(aString=="v"){
      fakeVolFlag= true;
      aString=debugStrings[2];
      temp= aString.toInt(); 
      fakeStuff.BME=temp-1;
      aString=debugStrings[3];
      temp= aString.toInt(); 
      fakeStuff.layer=temp-1;
      aString=debugStrings[4];
      temp= aString.toInt(); 
      fakeStuff.voltage=temp;
    }
    else if(aString=="t"){
      fakeTempFlag= true;
      aString=debugStrings[2];
      temp= aString.toInt(); 
      fakeStuff.BME=temp-1;
      aString=debugStrings[3];
      temp= aString.toInt(); 
      fakeStuff.tempsensor=temp-1;
      aString=debugStrings[4];
      temp= aString.toInt(); 
      fakeStuff.temperature=temp;
    }
    else if(aString=="p"){
      fakePressFlag= true;
      aString=debugStrings[2];
      temp= aString.toInt(); 
      fakeStuff.pressure=temp;
    }
    else if(aString=="c"){
      fakeCurFlag= true;
      aString=debugStrings[2];
      temp= aString.toInt(); 
      fakeStuff.current=temp*.0001;
    }
    else{
      fakeTempFlag=false;
      fakeVolFlag=false;
      fakePressFlag=false;
      fakeCurFlag=false;
    }
  }
  else if(aString=="totalmm") {
    fakeTotVolFlag=true;
    aString=debugStrings[1];
    temp= aString.toInt();
    fakeStuff.totalVoltage=temp*.0001;   
  }
  else if(aString=="modmm") {
      fakeModVolFlag=true;
      aString=debugStrings[1];
      temp= aString.toInt();
      fakeStuff.BME=temp-1;
      aString=debugStrings[2];
      temp= aString.toInt();
      fakeStuff.modSum=temp*.0001;
  }
  else if(aString=="clear") {
    clearFlags();  
  }
  else if(aString=="uart") {
    aString=debugStrings[1];
    temp= aString.toInt();
    if (temp==1){
      uartPrint=true;
      Serial.println("Serial printing enabled.");
      Serial.println("\n \t\t BMU 7.6\n");    // print the code version
      Serial.println("Enter Command (help for a list of commands)");
    }
    else{
      uartPrint=false;
      Serial.println("Serial printing disabled.");
    }     
  }
  else if(aString=="pseudo") {
    aString=debugStrings[1];
    temp= aString.toInt();
    if (temp==1){
      pseudoDataFlag=true;
      Serial.println("Pseudo data enabled.");
    }
    else{
      pseudoDataFlag=false;
      Serial.println("Pseudo data disabled.");
    }     
  }
  else {
    BMCcommand=input; //regular mode commands like charge, balance, stop, clear,
  }
    
}

 /*------------------------------------------------------------------------------
 *  void dischargeTest(void)
 * discharges all the virtual cells of a battery
 *----------------------------------------------------------------------------*/
 
 void dischargeTest(void){
   int j=0;
   for(j=0;j<BMENum;j++){           // for all BMEs            
     BME[j].DCC= 7;                // set 3 bit DCC registry to "111" 
   } 
 }
 
 
 /*------------------------------------------------------------------------------
 *  void dischargeResistorTest(int module, int layer)
 * enables the <module>'s <resistor> so discharging occcurs
 * prints the voltage of each layer in the whole module
 *----------------------------------------------------------------------------*/
 
 void dischargeResistorTest(int module,int layer){
   if (0<module<=BMENum) {  // make sure the command is Module 1-14
      BME[module-1].balFlag[layer-1]= 1;
      if (1) { //(BME[module-1].fVol[layer-1] > 0) { //make sure voltage isnt too low volLowWarn
        if (layer >122) {
          BME[module-1].DCC=7;    // turn  on all layers
        }
        else if (layer >22) {
          BME[module-1].DCC=3;    // turn  on all layers
        }
        else if (layer >12) {
          BME[module-1].DCC=5;    // turn  on all laye
        }
        else if (layer >11) {
          BME[module-1].DCC=6;    // turn  on all laye
        }
        else if (layer >0) {
          BME[module-1].DCC= BME[module-1].DCC | (1<<(3-layer));    // turn on the moudle layer's resistor
        }
        int j=0;
        for(j=0;j<cellNum;j++){           // print voltage of each cell with tab spacers "vol1 \t vol2 \tb vol3 \n"            
          Serial.print(BME[module-1].fVol[j],4); // four decimal precision on voltages
          Serial.print("\t");
        }
        Serial.println("");      // prints carriage return
      }
      else {  // if voltage is too low, stop the discharge resistor test
        BME[module-1].DCC= 0;   // stop balancing by disabling the bit flag corresponding to the i-th virtual layer
        Serial.print("Voltage too low. Try another cell or recharge.");  
        BMCcommand = "stop";
      }
   } 
 }
 
  /*------------------------------------------------------------------------------
 *  void flagDebugTest(void)
 * discharges all the virtual cells of a battery
 *----------------------------------------------------------------------------*/
 
 void flagDebugTest(const int &flag){
   if (0<flag && flag <33){
     flagBMU= flagBMU | (1<<(flag-1)); 
     if(uartPrint)Serial.print("Flag ");
     switch (flag) {
    case 1:
      Serial.println("1: Water Leak set.");
      break;
    case 2:
      Serial.println("2: High Temperature Warning set.");
      break;
    case 3:
      Serial.println("3: High Temperature Alarm set.");
      break;
    case 4:
      Serial.println("4: Temperature Sensor Failure set.");
      break;
    case 5:
      Serial.println("5: High Pressure Rate set.");
      break;
    case 6:
      Serial.println("6: Pressure Out of Range set.");
      break;
    case 7:
      Serial.println("7: High Voltage Alarm set.");
      break;
    case 8:
      Serial.println("8: Low Balancing Voltage Alarm set.");
      break;
    case 9:
      Serial.println("9: Low Voltage Warning set.");
      break;
    case 10:
      Serial.println("10: Low Voltage Alarm set.");
      break;
    case 11:
      Serial.println("11: Dead Battery Alarm set.");
      break;
    case 12:
      Serial.println("12: Voltage Sensor Failure set.");
      break;
    case 13:
      Serial.println("13: Voltage Mismatch set.");
      break;
    case 14:
      Serial.println("14: BME Flag Alarm set.");
      break;
    case 15:
      Serial.println("15: BMU to BME Com Error set.");
      break;
    case 16:
      Serial.println("16: BMU to BMC Com Error set.");
      break;
    case 17:
      Serial.println("17: Drive Current Out of Range set.");
      break;
    case 18:
      Serial.println("18: Charge Current Out of Range set.");
      break;
    case 19:
      Serial.println("19:  Stop Current Out of Range  set.");
      break;
    case 20:
      Serial.println("20: Charging/Balancing Timeout set.");
      break;
    case 21:
      Serial.println("21: Charging Done   set.");
      break;
    case 22:
      Serial.println("22: Balancing Done set.");
      break;
    case 23:
      Serial.println("23: Balance Recommended  set.");
      break;
  }

   }
   else if(flag==0){
     flagBMU=0;
     if(uartPrint)Serial.println("Flags Cleared ");
   }    
 }
 
 
 /*------------------------------------------------------------------------------
 *  void fakeVoltageData(void)
 * discharges all the virtual cells of a battery
 *----------------------------------------------------------------------------*/
 
 void fakeVoltageData(){  
   BME[fakeStuff.BME].vol[fakeStuff.layer]=fakeStuff.voltage; 
 }
 
 /*------------------------------------------------------------------------------
 *  void fakeTemperatureData(void)
 * 
 *----------------------------------------------------------------------------*/
 
 void fakeTemperatureData(){  
   int anInteger=fakeStuff.tempsensor;
   if (anInteger >=0 && anInteger<=3)
     BME[fakeStuff.BME].temp[anInteger]=fakeStuff.temperature; 
   else if (anInteger==4)
        BME[fakeStuff.BME].iTemp=fakeStuff.temperature; 
 }
 /*------------------------------------------------------------------------------
 *  void fakePressureData(void)
 * 
 *----------------------------------------------------------------------------*/
 
 void fakePressureData(){  
   pressure=((float)fakeStuff.pressure)*.0001; 
 }
 

