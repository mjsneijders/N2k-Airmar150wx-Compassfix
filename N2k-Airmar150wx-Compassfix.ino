
#include <Arduino.h>
//#define N2k_CAN_INT_PIN 21
#include "NMEA2000_CAN.h"  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include "NMEA2000_due.h"
#include "N2kMessages.h"
#include "N2kDataInnerHandler.h"
#include "N2kDataOuterHandler.h"
#include "BoardSerialNumber.h"
#include "BoatData.h"
#include <DueFlashStorage.h>
DueFlashStorage dueFlashStorage;
struct Configuration {
  float Variation;
  uint16_t DaysSince1970;
};
Configuration configuration;

#include <Time.h>
#include <TimeLib.h>
#include <wmm.h>

tBoatData Boatdata;

#define DefaultSerialNumber 999999
uint32_t GetSerialNumber() {
  uint32_t Sno=GetBoardSerialNumber();

  return ( Sno!=0?Sno:DefaultSerialNumber );
}



tNMEA2000 &NMEA2000_2=*(new tNMEA2000_due(tNMEA2000_due::CANDevice1));
tN2kInnerHandler N2kInnerHandler(&NMEA2000);
tN2kOuterHandler N2kOuterHandler(&NMEA2000_2);
const unsigned long TransmitMessages[] PROGMEM={126992L,126993L,127258L,129025L,129026L,129029L,129033L,129044L129539L,129540L,130306L,130311L,130312L,130314L,130323L,0};

//  NMEA2000 object,   inner canbus
void setup_can1(){
     NMEA2000.SetN2kCANSendFrameBufSize(150);
     NMEA2000.SetN2kCANReceiveFrameBufSize(150);
    char SnoStr[33];
    uint32_t SerialNumber=GetSerialNumber();
    snprintf(SnoStr,32,"%lu",(long unsigned int)SerialNumber);
    NMEA2000.SetProductInformation(SnoStr, // Manufacturer's Model serial code. 
                                     100, // Manufacturer's product code
                                     "Simple battery monitor",  // Manufacturer's Model ID
                                     "1.0.0.2 (2017-06-13)",  // Manufacturer's Software version code
                                     "1.0.0.0 (2017-06-13)", // Manufacturer's Model version
                                     0xff, // load equivalency - use default
                                     0xffff, // NMEA 2000 version - use default
                                     0xff, // Sertification level - use default
                                     0
                                     );
    // Set device information for temperature monitor
    NMEA2000.SetDeviceInformation(SerialNumber, // Unique number. Use e.g. Serial number.
                                  130, // Device function=Temperature. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                  75, // Device class=Sensor Communication Interface. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                  2040, // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf                               
                                  4, // Marine
                                  0
                                 );
    
    NMEA2000.SetForwardStream(&Serial);
    // NMEA2000.SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.
    NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode,36);
    //NMEA2000.SetDebugMode(tNMEA2000::dm_Actisense); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
    NMEA2000.EnableForward(false); // Disable all msg forwarding to USB (=Serial)
    //NMEA2000.ExtendTransmitMessages(TransmitMessages);
    NMEA2000.AttachMsgHandler(&N2kInnerHandler);
    NMEA2000.Open();
}

// NMEA2000_2 object,  outer canbus
void setup_can2(){
    NMEA2000_2.SetN2kCANSendFrameBufSize(150);
    NMEA2000_2.SetN2kCANReceiveFrameBufSize(150);
    char SnoStr[33];
    uint32_t SerialNumber=GetSerialNumber();
    snprintf(SnoStr,32,"%lu",(long unsigned int)SerialNumber);
    NMEA2000_2.SetProductInformation(SnoStr, // Manufacturer's Model serial code. 
                                   100, // Manufacturer's product code
                                   "Weatherstation WX150-mod",  // Manufacturer's Model ID
                                   "1.0.0.2 (2022-03-09)",  // Manufacturer's Software version code
                                   "1.0.0.0 (2022-03-09)", // Manufacturer's Model version
                                   0xff, // load equivalency - use default
                                   0xffff, // NMEA 2000 version - use default
                                   0xff, // Sertification level - use default
                                   0
                                   );
    // Set device information for temperature monitor
    NMEA2000_2.SetDeviceInformation(SerialNumber, // Unique number. Use e.g. Serial number.
                                  145, // Device function=Temperature. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                  60, // Device class=Sensor Communication Interface. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                  2040, // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf                               
                                  4, // Marine
                                  0
                                 );
    
    NMEA2000_2.SetForwardStream(&Serial);
    NMEA2000_2.SetMode(tNMEA2000::N2km_ListenAndNode,35);
    NMEA2000_2.ExtendTransmitMessages(TransmitMessages);
    NMEA2000.AttachMsgHandler(&N2kOuterHandler);
    NMEA2000_2.EnableForward(false); // Disable all msg forwarding to USB (=Serial)
    NMEA2000_2.Open();
}

void setup_boatdata(){
  Boatdata.Datetime.Lastupdate=0;
  Boatdata.Datetime.Systemtime=0;
  Boatdata.Datetime.Systemdate=0;
  Boatdata.Position.Lastupdate=0;
  Boatdata.Depth.Lastupdate=0;
  Boatdata.Navigation.BearingReference=N2khr_true;
  Boatdata.Navigation.CalculationType=N2kdct_RhumbLine;
  Boatdata.Navigation.XTEMode=N2kxtem_Autonomous; 
  Boatdata.Depth.Range=100; 
  
  
  Boatdata.Position.N2kSource=255;  //35
  Boatdata.Heading.N2kSource=255;  //52 
  Boatdata.Heading.N2kVariationSource=255;  //52 
  Boatdata.Heading.source=N2kmagvar_WMM2020;
  Boatdata.Heading.SOG=N2kDoubleNA; 
  Boatdata.Weather.N2kSource=255;

}

void setup_flash(){
uint8_t codeRunningForTheFirstTime = dueFlashStorage.read(0); // flash bytes will be 255 at first run
  Serial.print("codeRunningForTheFirstTime: ");
  if (codeRunningForTheFirstTime) {
    Serial.println("yes, saving initial data");
    /* OK first time running, set defaults */
    configuration.Variation = 0;  
    StoreVariation(DegToRad(2), 17554);

    // write 0 to address 0 to indicate that it is not the first time running anymore
    dueFlashStorage.write(0, 0); 
  }
  else {
    Serial.println("no, reading data");
    byte* b = dueFlashStorage.readAddress(4); // byte array which is read from flash at adress 4
    memcpy(&configuration, b, sizeof(Configuration)); // copy byte array to temporary struct
    Serial.println((String)"Variation from flash = " + RadToDeg(configuration.Variation));
  }
}
  


void setup() {
    wmm_init(); 
    Serial.begin(115200);
    setup_can1(); 
    setup_can2(); 
    setup_boatdata();
    setup_flash();
}

void loop() {
    NMEA2000.ParseMessages();
    NMEA2000_2.ParseMessages();
    N2kSendVariation();
}

#define VariationUpdatePeriod 1000

void N2kSendVariation() {

  static unsigned long TimeUpdated=millis();
  static unsigned char SID=0; 
  tN2kMsg N2kMsg;
   if  (millis() - TimeUpdated >= VariationUpdatePeriod ){
      TimeUpdated=millis();
    if ( Boatdata.Position.Lastupdate > 0 ){
          SID++; 
          float Variation;
          time_t epoch=(Boatdata.Datetime.DaysSince1970*24*3600) + Boatdata.Datetime.SecondsSinceMidnight;
          uint16_t shortyear=year(epoch); 
          while (shortyear > 100)  shortyear =shortyear - 100; 
          if (shortyear > 25) shortyear = 25;
          if (shortyear < 20) shortyear = 20;
          float wmmdate=wmm_get_date( shortyear,  month(epoch),  day(epoch));
          E0000(Boatdata.Position.Latitude, Boatdata.Position.Longitude, wmmdate, &Variation); 
          int intVariation=(int)(Variation); 
          double floatVariation=(Variation-intVariation) ; 
          floatVariation=(floatVariation/60)*100; 
          Variation=intVariation + floatVariation; 
          Boatdata.Heading.Variation=DegToRad(Variation); 
          StoreVariation(DegToRad(Variation),Boatdata.Datetime.DaysSince1970);
          SetN2kPGN127258(N2kMsg, SID,Boatdata.Heading.source,Boatdata.Datetime.DaysSince1970,DegToRad(Variation));
          NMEA2000_2.SendMsg(N2kMsg);
         
       }else{
          SetN2kPGN127258(N2kMsg, SID,Boatdata.Heading.source,configuration.DaysSince1970,configuration.Variation);
          NMEA2000_2.SendMsg(N2kMsg);
        
       }
   }
}
void StoreVariation(float Variation, uint16_t DaysSince1970){
    if ( Variation != configuration.Variation){
            Serial.println((String)"Storing Variation to flash = " + RadToDeg(Variation));
            configuration.Variation=Variation; 
            configuration.DaysSince1970=DaysSince1970; 
            byte b2[sizeof(Configuration)]; // create byte array to store the struct
            memcpy(b2, &configuration, sizeof(Configuration)); // copy the struct to the byte array
            dueFlashStorage.write(4, b2, sizeof(Configuration)); // write byte array to flash
    }
}  
