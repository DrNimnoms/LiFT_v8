/*------------------------------------------------------------------------------
 * BME.h
 * Created by Nima Ghods
 * Data: 5/21/2012
 * setsup the constants and parameters for BME_XX.ino
 *----------------------------------------------------------------------------*/

// flag limits
  #define tempVCAlarm 60    //virtual cell temperature limit for temperature error 
  #define tempTiAlarm 75    // BME LTC chip temperature limit for temperature error 
  #define tempHSAlarm 120   //heat sink temperature limit for temperature error
  #define tempVCWarn 40    //virtual cell temperature limit for temperature warning 
  #define tempTiWarn 65    //BME LTC chip temperature limit for temperature warning
  #define tempHSWarn 110   //heat sink temperature limit for temperature warning
  
  #define volHighAlarm 4.205  //high virtual cell voltage limit for voltage error 
  #define volLowBalAlarm 3.7   // the voltage at whitch the system will not go in to balancing mode
  #define balRecLimit 3.9      // minimum voltage limit for recommending balancing
  #define volLowWarn 3.2       //low virtual cell voltage limit for voltage warning
  #define volLowAlarm 3.0      //low virtual cell voltage limit for voltage error 
  #define deadBatAlarm 2.5   // the voltage at whitch the system will not go in to charge mode
  #define volMismatch 3.0       //voltage mismatch limit between calculated and measured total voltage of half-string
  #define volModMismatch 0.06   //voltage mismatch limit between calculated and measured total voltage of battery module
 
  #define presHighLimit 5.0    //High pressure limit
  #define presLowLimit 0.5     //Low Pressure limit
  #define presRateHigh 0.3    //High pressure rate limit
  
  #define inCurLimit 2.0      //current in limit durring Drive
  #define highInCur 92.0      //high current in limit during Charging 
  #define highOutCur -2.0      //current out limit during Charging
  #define inOutCur 1.0          // current in or out limit durring stop and balance
  #define doneCur 4.45         //the current at which the charging is called done
  
  #define balRecVol 0.050       // voltage difference at which balancing will be recommended 
  #define volTolerance 0.002   // the max voltage difference that the virtual cells will have at the end of balancing 
  
  #define timeoutLimit 180000     //a timeout limit of 10 hours/looptime==>180000 for charging and balanceing 
  #define overrideTLimit 310        // a time limit for priority flag of 2 or 3 5*62==>62sec

//BMU ADC conversion constants
  #define curConst 0.08587        //80/V*3.3V/4095*4.01ohm/3.01ohm  sensor resolution*adc resolution*voltage divider
  #define volConst 0.0482  //.0439    //(174Kohm+10Kohm)/10Kohm*(10kohm+5.1kohm)/5.1kohm*3.3V/4095
  #define presConst 0.0061034   //1 kpa/5V/0.018*4.7ohm/3.2ohm*3.3V/4095*0.14503 gpsi/kpa
  #define presOffset 0.0058016     //0.04 kpa 0.14503 gpsi/kpa
  #define capConst 0.0000555556    //0.2 sec==>.000055556 hours

// BMU battery prameters
  #define BMENum 14          // number of BMEs attached to the BMU
  #define cellNum 3          // number of virtual cells attached to a BME
  #define BMEConDelay1 2300   // delay for BME ADC conversion commands
  #define BMEConDelay2 2000   // delay for BME ADC conversion commands
  #define BME_MD 2           //Set to:2 for 7kHz, 3 for 26Hz mode(filtered)   

// temperature reading prameters
  #define IntTempConst  0.013333  //0.1mV/7.5mV is the convertion from the reading to K for internal temperature
  #define Binv .00024777  // 1/ B-value where B-value is 4190 K
  #define T0inv 0.003354  // 1/T_0 where T_0 is 298.15 K
//  #define VIN 5.0      // Temperature reference
  
  //time
  #define ONEHOUR 36000000000 //in microseconds
  #define TWOMINUTES 120000000  // in microseconds
  #define ONEMINUTE 60000000  // in microseconds
  #define THIRTYSECONDS 30000000 //in microseconds
  #define TENSECONDS 10000000 //in microseconds
  #define FIVESECONDS 5000000 //in microseconds
  #define THREESECOND 3000000   // in microseconds
  #define ONESECOND 1000000   // in microseconds

// debugging variables
  boolean uartPrint=false;    // print for debugging
  String inputString="";
  boolean fakeVolFlag=false;
  boolean fakePressFlag=false;
  boolean fakeTempFlag=false;
  boolean fakeTotVolFlag=false;
  boolean fakeModVolFlag=false;
  boolean fakeCurFlag=false;
  boolean pseudoDataFlag=false;
  
  enum mode { STOPMODE, DRIVEMODE, CHARGEMODE,BALANCEMODE};
  
  typedef struct{
    mode currentMode;
    boolean selfCheck;
    int hours;
    int minutes;
    unsigned long microseconds;
    unsigned long timeKeepingStamp;
  } modeStuff;
  
  modeStuff modeInfo;
  
  typedef struct  {
  int tempsensor; //1-3 VC, 4 HS, 5 iTemp
  int BME;
  int layer;
  int voltage;
  int pressure; 
  int temperature;
  float totalVoltage;
  float modSum;
  float current;
  } fakeData;
  
  fakeData fakeStuff;
  
  
 // loop timing variables
 const long controlTime=200000;  // loop time in uSec  .2 s loops ==> 5Hz
 const float dt=controlTime/1000000.0;  // control time in sec
 unsigned long timeStamp=0;          // used to keep track of the loop time
 unsigned long balanceTimeStamp=0;  // keeps track of balancing timing
 unsigned long bmcComTimeStamp=0;   // keeps track of time since last communication
 const long balanceRelaxTime= TWOMINUTES; // length of time balance mode must wait before discharging 
 const long balanceCheckTime=THIRTYSECONDS; // 
 long dLoopTime=0;         //actual loop time in usec
 int BMCcommdt=0;          // the time between bmc communications
 unsigned long overrideTimeStamp=0;
 
// Arduino due pins in use 
  const int tVolInPin = A3;  // Analog input pin for the total voltage
  const int presIn1Pin = A1;  // Analog pressure sensor pin
  const int curInPin = A4;  // Analog current sensor pin
  const int cur0InPin=A7;   // current sencor referance pin
  const int relay1 = 47;   // negative side relay or relay 1 pin
  const int relay2 = 49;   // positive side relay or realy 2 pin 
  const int frontWPin = 22;  //front water leak sensor
  const int backWPin = 23;    // back water leak sensor
// BMU varibles
String BMCcommand="stop";      //The command from BMC
int conOnTime=0;          // counter to enable contactor
boolean contactorsOn=false;

// BMU measurement Variables
float totalVoltage =0;        // total half-string voltage read from ADC
float tVolOld =0;          // the last total voltage
float pressure =0;        // pressure sensor reading
float presOld =0;              // last pressure value
float presRate =0;          // filtered pressure rate
int current0 =0;         // current mid point
float current =0;        // value read from the LEM sensor
float currentOffset =0;  // current offset
float curMeas =0;        // measured current
boolean fwLeak =0;         // front leak sensor
boolean bwLeak =0;         // back leak sensor
int fwLeakCount=0;
int bwLeakCount=0;
int mismatchCount=0;

// BMU calculated Variables
float cap=435;            // battery capacity in Amp hours
const float capMax=435;  
float volSum=0.0;          // calculated sum of all virtual cells
float SOC=50;             // percent 0-100 state of charge
float maxTemp=0;          //  max temperature of all virtual cells
float minVol=7.0;        //  minimum voltage of virtual cells?
float maxVol=0.0;          // maximum voltage of virtual cells?
float balance2Vol=4.2;    // voltage to balance to
float balanceMax=4.2;
int balDoneCount=0;
float charge2Vol=3.0;      //voltage to charge to


// BME communication PEC (packet error code) check table
int pec15Table[256];
int CRC15poly=0x4599;
int selfCheckVal1=0x9555;
int selfCheckVal2=0x6AAA;

// BME varibles
typedef struct  {
  byte addr;             //BME address
  int vol[cellNum];      // array of virtual cell voltages
  int vSum;              //measured  BME voltage
  int vref2;              
  int temp[4];           // temperature of: 0-2 = virtual cells, 3 =heatsink
  int iTemp;              // internal temperature of BME LTC84 chip
  float modSum;            //calculated BME voltage (sum of layers)
  float fVol[cellNum];    //float versions of everything
  float fVSum;
  float fVref2; 
  float fTemp[4];
  float fiTemp;
  boolean ignoreT[4];
  boolean ignoreiT;
  boolean uFlag[cellNum];
  boolean oFlag[cellNum];
  boolean balFlag[cellNum];
  boolean balTempCon;
  boolean muxCheck;
  boolean dataCheck;
  boolean volSelfCheck;
  boolean AuxSelfCheck;
  boolean StatSelfCheck;
  int DCC;                  // 3 bit registry, each bit is a flag for balancing a corresponding virtual layer
  byte GPIO;                //5 Auxiliary GPIO ports
} BMEdata;                   //BME data struct 

BMEdata BME[BMENum];

boolean DCP=false;     //Discharge premitted

//Biquad filter struct
typedef struct{
  float gain;//filter gain
  float b0;  //input k coefficient
  float b1;  //input k-1 coefficient
  float b2;  //input k-2 coefficient
  float a1;  //output k-1 coefficient
  float a2;  //output k-2 coefficient
  float x1;  //filter state
  float x2;  //filter state
}BiquadType;

BiquadType biPresrate;

//BMU modes flags
//boolean stopOn=true;
//boolean driveOn=false;
//boolean chargeOn=false;
//boolean balanceOn=false;
boolean stopUntil=false;      // true until the oprator has acknoladged the software stop

//BMU flags
unsigned int flagBMU=0;         //all the BMU flags up to 32 flags
unsigned int flagOverride=0;         //the flags that the BMC wants to override
boolean flagIgnoreTemp=false;
byte flagPriority=0;             //the Priority of the active flag under the current mode

boolean leakFlag =false;         //Leak in front or back of battery bottle

boolean tempAlarmFlag =false;    //Any VC temp > 60 C, Heat Sink temp > 120 C, Internal chip temp>75 C
boolean tempWarnFlag =false;     //Any VC temp > 40 C, Heat Sink temp > 110 C, Internal chip temp>65 C
boolean tempFailFlag =false;     //Any defective/failed temperature sensor reading, when sensor disconnected or shorted out

boolean presRateFlag =false;     //Pressure rate > 1 PSI/sec
boolean presFlag =false;        // Pressure < 1.5 PSI or Pressure  > 2.5 PSI

boolean volHighAlarmFlag =false;      //Any VC voltage > 4.25 V 
boolean volLowBalAlarmFlag=false;    //Any VC voltage < 3.7 V
boolean volLowWarnFlag =false;     //Any VC voltage < 3.2 V
boolean volLowAlarmFlag =false;    //Any VC voltage < 3.0 V
boolean deadBatAlarmFlag=false;    //Any VC voltage < 2.5 V
boolean volFailFlag =false;      //Any VC voltage < .1 V or >6.5 or Vref2<2.978 or>3.020
boolean volMisFlag =false;      /*5V difference between overall half-string voltage and sum of half-string VC voltages or
                                     50mV difference between battery module voltage and sum of its VC voltages*/
boolean selfTestFlag =false;     // self-check failures are sent from BME                                
boolean bmeAlarmFlag =false;     // If any cell over/under voltage failures or self-check failures are sent from BME 
boolean bmeComFlag =false;       // Communication failure occurs between BMU and BME
boolean bmcComFlag =false;       // Communication failure occurs between BMU and BMC

boolean driveCurflag=false;          //Current >20 durring Drive
boolean chargeCurFlag=false;      //Current > 92A or current <2A during Charge
boolean stopCurFlag=false;     //abs(Current)>1A

boolean timeoutFlag=false;      //Charging or balance time > 10 hours

boolean chargeDoneFlag =false;   // charging done flag
boolean balDoneFlag =false;      // balancing done flag
boolean balRecFlag =false;       // balancing recommended flag
boolean balRelaxFlag=false;      // true when system has relaxed
boolean realBalDataFlag=false;

boolean fanOn=false;


//************************ SPI parameters *************************//
// pins used for the connection with the BME and BMC with SPI
const int chipSelectPin = 52;
const int csBME1 =2;
const int csBME2 =3;
const int ClockDivider=100;
const int MISOPin=50;

//************************ Ethernet parameters *************************//
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte BMUNum=0;
byte Add0=5;
byte Add1=6;
byte Add2=7;
//byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE1 };
byte mac[6] = {0};
byte ipadd[]={192,168,0, 170};
// the router's gateway address:
byte gateway[] = { 192, 168, 0, 1 };
// the subnet:
byte subnet[] = { 255, 255, 255, 0 };
int port=40;
//IPAddress ip(ipadd);
EthernetServer server(port);
// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):

//**************************** Voltage lookup tables ************************//

int lookUpSOC[1046]={1000,999,998,997,996,995,994,993,992,991,991,990,989,988,987,986,985,984,983,982,981,980,979,978,977,976,975,974,973,
972,971,970,969,969,968,967,966,965,964,963,962,961,960,959,958,957,956,955,954,953,952,951,950,949,948,947,947,946,945,944,943,942,941,
940,939,938,937,936,935,934,933,932,931,930,929,928,927,926,925,925,924,923,922,921,920,919,918,917,916,915,914,913,912,911,910,909,908,
907,906,905,904,903,903,902,901,900,899,898,897,896,895,894,893,892,891,890,889,888,887,886,885,884,883,882,881,881,880,879,878,877,876,
875,874,873,872,871,870,869,868,867,866,865,864,863,862,861,860,859,859,858,857,856,855,854,853,852,851,850,849,848,847,846,845,844,843,
842,841,840,839,838,837,837,836,835,834,833,832,831,830,829,828,827,826,825,824,823,822,821,820,819,818,817,816,815,815,814,813,812,811,
810,809,808,807,806,805,804,803,802,801,800,799,798,797,796,795,794,793,792,792,791,790,789,788,787,786,785,784,783,782,781,780,779,778,
777,776,775,774,773,772,771,770,770,769,768,767,766,765,764,763,762,761,760,759,758,757,756,755,754,753,752,751,750,749,748,748,747,746,
745,744,743,742,741,740,739,738,737,736,735,734,733,732,731,730,729,728,727,726,726,725,724,723,722,721,720,719,718,717,716,715,714,713,
712,711,710,709,708,707,706,705,704,704,703,702,701,700,699,698,697,696,695,694,693,692,691,690,689,688,687,686,685,684,683,682,682,681,
680,679,678,677,676,675,674,673,672,671,670,669,668,667,666,665,664,663,662,661,660,660,659,658,657,656,655,654,653,652,651,650,649,648,
647,646,645,644,643,642,641,640,639,638,637,637,636,635,634,633,632,631,630,629,628,627,626,625,624,623,622,621,620,619,618,617,616,615,
615,614,613,612,611,610,609,608,607,606,605,604,603,602,601,600,599,598,597,596,595,594,593,593,592,591,590,589,588,587,586,585,584,583,
582,581,580,579,578,577,576,575,574,573,572,571,571,570,569,568,567,566,565,564,563,562,561,560,559,558,557,556,555,554,553,552,551,550,
549,549,548,547,546,545,544,543,542,541,540,539,538,537,536,535,534,533,532,531,530,529,528,527,527,526,525,524,523,522,521,520,519,518,
517,516,515,514,513,512,511,510,509,508,507,506,505,505,504,503,502,501,500,499,498,497,496,495,494,493,492,491,490,489,488,487,486,485,
484,483,483,482,481,480,479,478,477,476,475,474,473,472,471,470,469,468,467,466,465,464,463,462,461,461,460,459,458,457,456,455,454,453,
452,451,450,449,448,447,446,445,444,443,442,441,440,439,438,438,437,436,435,434,433,432,431,430,429,428,427,426,425,424,423,422,421,420,
419,418,417,416,416,415,414,413,412,411,410,409,408,407,406,405,404,403,402,401,400,399,398,397,396,395,394,394,393,392,391,390,389,388,
387,386,385,384,383,382,381,380,379,378,377,376,375,374,373,372,372,371,370,369,368,367,366,365,364,363,362,361,360,359,358,357,356,355,
354,353,352,351,350,350,349,348,347,346,345,344,343,342,341,340,339,338,337,336,335,334,333,332,331,330,329,328,328,327,326,325,324,323,
322,321,320,319,318,317,316,315,314,313,312,311,310,309,308,307,306,306,305,304,303,302,301,300,299,298,297,296,295,294,293,292,291,290,
289,288,287,286,285,284,284,283,282,281,280,279,278,277,276,275,274,273,272,271,270,269,268,267,266,265,264,263,262,261,261,260,259,258,
257,256,255,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240,239,239,238,237,236,235,234,233,232,231,230,229,228,227,226,225,
224,223,222,221,220,219,218,217,217,216,215,214,213,212,211,210,209,208,207,206,205,204,203,202,201,200,199,198,197,196,195,195,194,193,
192,191,190,189,188,187,186,185,184,183,182,181,180,179,178,177,176,175,174,173,173,172,171,170,169,168,167,166,165,164,163,162,161,160,
159,158,157,156,155,154,153,152,151,151,150,149,148,147,146,145,144,143,142,141,140,139,138,137,136,135,134,133,132,131,130,129,129,128,
127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112,111,110,109,108,107,107,106,105,104,103,102,101,100,99,98,97,96,95,94,93,
92,91,90,89,88,87,86,85,84,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,62,61,60,59,58,57,56,55,54,53,52,51,50,
49,48,47,46,45,44,43,42,41,40,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
};

int lookUpVoltage[1046]={4186,4185,4183,4181,4179,4178,4177,4175,4174,4172,4171,4169,4168,4167,4165,4164,4162,4161,4160,4159,4157,4156,
4155,4153,4152,4151,4149,4148,4147,4145,4144,4143,4141,4140,4139,4137,4136,4135,4134,4133,4131,4130,4129,4127,4126,4125,4124,4123,4121,
4120,4119,4117,4116,4115,4114,4113,4111,4110,4109,4107,4106,4105,4104,4103,4101,4100,4099,4098,4097,4095,4094,4093,4092,4091,4089,4088,
4087,4086,4084,4083,4082,4081,4079,4079,4077,4076,4075,4074,4072,4071,4070,4069,4068,4067,4065,4064,4063,4062,4060,4059,4058,4057,4055,
4055,4053,4052,4051,4049,4049,4047,4046,4045,4044,4043,4041,4040,4039,4038,4037,4036,4034,4033,4032,4031,4030,4029,4027,4026,4025,4024,
4023,4022,4021,4019,4018,4017,4016,4015,4014,4013,4011,4011,4009,4008,4007,4006,4005,4003,4003,4001,4000,3999,3998,3997,3996,3995,3993,
3992,3991,3990,3989,3988,3987,3986,3985,3983,3983,3981,3980,3979,3978,3977,3976,3975,3973,3972,3971,3971,3969,3968,3967,3966,3965,3964,
3963,3962,3961,3959,3959,3957,3956,3955,3954,3953,3952,3951,3950,3949,3948,3947,3946,3945,3943,3942,3941,3940,3939,3938,3937,3936,3935,
3934,3933,3932,3931,3930,3929,3928,3927,3925,3925,3923,3922,3921,3920,3919,3918,3917,3916,3915,3914,3913,3912,3911,3910,3909,3908,3907,
3906,3905,3904,3903,3902,3901,3899,3898,3897,3896,3895,3895,3893,3892,3891,3890,3889,3888,3887,3886,3885,3884,3883,3882,3881,3880,3879,
3878,3877,3876,3875,3874,3873,3872,3871,3870,3869,3868,3867,3865,3865,3863,3863,3862,3861,3859,3858,3857,3856,3855,3855,3853,3853,3851,
3851,3849,3849,3847,3846,3845,3844,3844,3843,3841,3841,3839,3839,3837,3836,3835,3835,3833,3833,3832,3831,3829,3829,3827,3827,3826,3825,
3824,3823,3822,3821,3820,3819,3818,3817,3816,3815,3814,3813,3812,3811,3811,3809,3809,3808,3807,3805,3805,3804,3803,3802,3801,3800,3799,
3798,3797,3796,3795,3795,3793,3793,3791,3791,3789,3789,3788,3787,3786,3785,3784,3783,3782,3781,3780,3779,3778,3777,3776,3775,3774,3773,
3772,3771,3770,3769,3768,3766,3765,3764,3763,3762,3761,3759,3758,3757,3755,3754,3753,3752,3750,3749,3747,3746,3744,3743,3741,3739,3738,
3736,3735,3733,3731,3729,3728,3726,3725,3723,3722,3720,3719,3717,3716,3715,3713,3712,3711,3710,3709,3708,3707,3705,3705,3704,3703,3702,
3701,3700,3699,3699,3697,3697,3696,3695,3694,3694,3693,3692,3691,3691,3690,3689,3689,3688,3687,3687,3686,3685,3685,3684,3683,3683,3682,
3682,3681,3680,3680,3679,3679,3678,3677,3677,3676,3676,3675,3675,3674,3673,3673,3672,3672,3671,3671,3670,3670,3669,3669,3668,3668,3667,
3667,3666,3666,3665,3665,3664,3664,3663,3663,3663,3662,3661,3661,3661,3660,3659,3659,3659,3658,3658,3657,3657,3657,3656,3656,3654,3655,
3654,3654,3653,3653,3652,3651,3651,3651,3651,3650,3650,3649,3650,3649,3648,3648,3647,3647,3646,3646,3646,3645,3645,3644,3644,3644,3643,
3643,3643,3642,3642,3642,3641,3641,3641,3640,3640,3640,3639,3639,3639,3638,3638,3638,3637,3637,3636,3636,3636,3636,3635,3635,3634,3634,
3634,3634,3633,3633,3632,3632,3632,3632,3632,3631,3631,3630,3630,3630,3630,3630,3629,3628,3628,3628,3629,3627,3628,3627,3627,3627,3627,
3626,3626,3626,3625,3625,3625,3625,3624,3624,3623,3623,3623,3623,3622,3622,3622,3621,3621,3621,3621,3620,3620,3619,3619,3619,3619,3618,
3618,3618,3617,3617,3617,3617,3617,3616,3616,3615,3615,3615,3615,3614,3614,3614,3613,3613,3613,3613,3613,3612,3612,3612,3611,3611,3611,
3610,3610,3610,3610,3609,3609,3609,3608,3608,3608,3608,3607,3607,3607,3607,3606,3606,3606,3606,3605,3605,3605,3605,3604,3604,3603,3603,
3603,3603,3603,3602,3602,3601,3601,3601,3601,3601,3600,3600,3599,3599,3599,3599,3599,3598,3598,3597,3597,3597,3597,3597,3596,3596,3595,
3595,3595,3595,3594,3594,3593,3593,3593,3593,3592,3592,3591,3591,3591,3590,3590,3590,3589,3589,3589,3588,3588,3587,3587,3587,3586,3586,
3585,3585,3585,3584,3584,3583,3583,3583,3582,3582,3581,3581,3580,3580,3580,3579,3579,3578,3578,3578,3577,3577,3576,3576,3575,3575,3575,
3574,3574,3573,3573,3573,3572,3572,3571,3571,3571,3570,3569,3569,3569,3568,3568,3567,3567,3566,3566,3565,3565,3564,3564,3563,3563,3563,
3562,3561,3561,3560,3559,3559,3558,3558,3557,3557,3556,3555,3555,3554,3553,3553,3552,3552,3551,3550,3550,3549,3548,3548,3547,3547,3546,
3545,3545,3544,3543,3543,3542,3542,3541,3541,3540,3539,3539,3538,3537,3537,3536,3536,3535,3534,3534,3533,3533,3532,3531,3531,3530,3529,
3529,3528,3527,3527,3526,3525,3525,3524,3523,3522,3521,3520,3519,3519,3517,3517,3515,3515,3513,3513,3511,3511,3509,3508,3507,3506,3505,
3504,3503,3502,3501,3500,3499,3498,3497,3496,3495,3494,3493,3492,3491,3490,3489,3488,3487,3485,3485,3483,3482,3481,3479,3478,3477,3475,
3474,3473,3471,3470,3469,3469,3467,3467,3466,3465,3464,3463,3463,3462,3462,3461,3461,3460,3460,3459,3459,3459,3458,3458,3457,3457,3456,
3456,3456,3455,3455,3454,3454,3453,3453,3453,3452,3452,3451,3451,3451,3450,3449,3449,3449,3449,3448,3447,3447,3447,3446,3446,3445,3444,
3444,3443,3443,3442,3442,3442,3441,3440,3440,3439,3439,3438,3438,3437,3437,3437,3436,3436,3435,3434,3435,3433,3432,3432,3432,3431,3430,
3429,3429,3429,3427,3427,3426,3425,3424,3424,3423,3422,3421,3420,3419,3418,3417,3416,3415,3414,3412,3411,3409,3408,3406,3404,3402,3400,
3396,3394,3390,3387,3382,3379,3374,3369,3365,3360,3355,3350,3345,3339,3333,3328,3322,3316,3310,3303,3297,3290,3283,3276,3269,3261,3254,
3246,3237,3229,3220,3211,3201,3192,3182,3171,3160,3148,3136,3124,3110,3096,3081,3065,3048,3029,3009,2987,2963,2935,2899,2829
};

