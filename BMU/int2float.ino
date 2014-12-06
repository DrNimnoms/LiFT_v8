/*------------------------------------------------------------------------------
 * void int2float(BMEdata& _BME)
 * takes data from int to float
 *----------------------------------------------------------------------------*/
 void int2float(BMEdata& _BME){      //_BME is adress of global struct BME
   vint2float((int*) _BME.vol, (float*) _BME.fVol, cellNum);  // passes pointers to struct entities 
   _BME.fVSum=_BME.vSum*0.002;
   _BME.fVref2=_BME.vref2*0.0001;
   tint2float((BMEdata&) _BME);
 }

/*------------------------------------------------------------------------------
 * void tint2float(BMEdata& _BME)
 * takes temperatures from int to float
 *----------------------------------------------------------------------------*/
void tint2float(BMEdata& _BME){
  
  vint2float((int*) _BME.temp, (float*) _BME.fTemp, 4);
  
  for(int i=0;i<4;i++){
    if(_BME.temp[i]==0) _BME.fTemp[i]=-42;
    else{
      _BME.fTemp[i]=_BME.fTemp[i]/(_BME.fVref2-_BME.fTemp[i]); 
      _BME.fTemp[i]=T0inv + Binv*log(_BME.fTemp[i]);
      _BME.fTemp[i]=1.0/_BME.fTemp[i]-273;
    }
  }
    _BME.fiTemp=(_BME.iTemp*IntTempConst)-263;
}

/*------------------------------------------------------------------------------
 * void vint2float(int* vint, float* vfloat, int n)
 * takes voltage from int to float
 *----------------------------------------------------------------------------*/
void vint2float(int* vint, float* vfloat, int n)
{
  for(int i=0;i<n;i++){
    vfloat[i]=vint[i]*0.0001;
  }
}


