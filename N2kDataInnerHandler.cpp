#include "N2kDataInnerHandler.h"
#include <N2kMessages.h>
#include <math.h>
#include "BoatData.h"
extern tNMEA2000 &NMEA2000;
extern tNMEA2000 &NMEA2000_2;
extern tBoatData Boatdata; 


void tN2kInnerHandler::HandleMsg(const tN2kMsg &N2kMsg) {
  switch (N2kMsg.PGN) {
    case 60928UL: FilterPGN(N2kMsg);break;        //ISO address claim
    case 126992UL: PassPGN(N2kMsg);break;         //System Time
    case 126993UL: PassPGN(N2kMsg);break;         //Heartbeat 
    case 127250UL: FilterPGN(N2kMsg);break;       //Heading
    case 127257UL: FilterPGN(N2kMsg);break;       //Attitude
    case 127258UL: FilterPGN(N2kMsg); break;      //Variation
    case 129025UL: PassPGN(N2kMsg);HandlePosition(N2kMsg); break; //GPS position, Rapid Update
    case 129026UL: PassPGN(N2kMsg);HandleCOGSOG(N2kMsg);break;    //GPS COG&SOG, Rapid Update
    case 129029UL: PassPGN(N2kMsg);HandleGNSS(N2kMsg);break;      //GPS GNSS Position data
    case 129033UL: PassPGN(N2kMsg);break;         //Local Time offset
    case 129044UL: PassPGN(N2kMsg);break;         //Datum
    case 129539UL: PassPGN(N2kMsg);break;         //GNSS DOPs
    case 129540UL: PassPGN(N2kMsg);break;         //GNSS Sats in view
    case 130306UL: HandleWind(N2kMsg);break;      //Wind data
    case 130311UL: PassPGN(N2kMsg);break;         //Environmental parameters
    case 130312UL: PassPGN(N2kMsg);break;          //Temperature
    case 130314UL: PassPGN(N2kMsg);break;         //Actual Pressure
    case 130323UL: PassPGN(N2kMsg);break;        //Meteorological Station Data
    default: PassPGN(N2kMsg);break;
  }
}

void tN2kInnerHandler::HandleHeading(const tN2kMsg &N2kMsg) {
  unsigned char SID;
  tN2kHeadingReference ref;
  double _Deviation = 0;
  double _Variation;
  double _Heading; 

  if ( ParseN2kHeading(N2kMsg, SID, _Heading, _Deviation, _Variation, ref) ) {
        if ( ref == N2khr_magnetic ) {
                Boatdata.Heading.LastMagneticHeadingTime = millis();
                Boatdata.Heading.MagneticHeading=_Heading;
               
        }
        if ( ref == N2khr_true ) {
                Boatdata.Heading.LastTrueHeadingTime = millis();
                Boatdata.Heading.TrueHeading=_Heading;  
        }    
  }
}
void tN2kInnerHandler::FilterPGN(const tN2kMsg &N2kMsg) {
  //filter this message
}
void tN2kInnerHandler::HandleCOGSOG(const tN2kMsg &N2kMsg) {
    if ( N2kMsg.Source == Boatdata.Position.N2kSource || Boatdata.Position.N2kSource == 255 ){ 
        unsigned char SID;
        tN2kHeadingReference HeadingReference;
          if ( ParseN2kCOGSOGRapid(N2kMsg,
                                   SID,
                                   HeadingReference,
                                   Boatdata.Heading.COG,
                                   Boatdata.Heading.SOG
             )) {
                 
                    Boatdata.Heading.LastCOGSOGTime=millis();
                    if ( HeadingReference==N2khr_magnetic ) {
                      if ( !N2kIsNA(Boatdata.Heading.Variation) ) Boatdata.Heading.COG+=Boatdata.Heading.Variation;
                    }
        
          }
    }
}
void tN2kInnerHandler::HandleWind(const tN2kMsg &N2kMsg) {
// pass through appearant wind
// filter out Theoretical wind. 
// add our ouwn calculation for theoretical wind
  tN2kMsg N2kMsg_1;
  unsigned char SID;
  tN2kWindReference WindReference;
  double Heading; 
  double WindSpeed, WindAngle; 
  double TWS, TWA, TWD;
  double SOG, COG; 

  if ( ParseN2kWindSpeed(N2kMsg, SID, WindSpeed, WindAngle, WindReference) ) {
    if ( WindReference == N2kWind_Apparent ){
         //passthroug AWA
          NMEA2000_2.SendMsg(N2kMsg); //passthroug appearant wind
          Boatdata.Weather.AppWind.Lastupdate=millis();
          Boatdata.Weather.AppWind.Speed=WindSpeed; 
          Boatdata.Weather.AppWind.Angle=WindAngle;
          
          //Serial.println ((String)"Variation = " + RadToDeg(Boatdata.Heading.Variation)); 
        
          if (millis() - Boatdata.Heading.LastTrueHeadingTime< 1500 && Boatdata.Heading.LastTrueHeadingTime > 0 ){
            Heading=Boatdata.Heading.TrueHeading; 
          }else if(millis() - Boatdata.Heading.LastMagneticHeadingTime< 1500 && Boatdata.Heading.LastMagneticHeadingTime > 0 ){
            Heading=Boatdata.Heading.MagneticHeading + Boatdata.Heading.Variation; 
          }else{
            Heading=Boatdata.Heading.COG;
          }
          
          if (millis() - Boatdata.Heading.LastCOGSOGTime< 3000 && Boatdata.Heading.LastCOGSOGTime > 0){
              SOG=Boatdata.Heading.SOG;
              COG=Boatdata.Heading.COG;
 
              CalculateTruewind(Heading, 
                                SOG, 
                                COG, 
                                WindSpeed, 
                                WindAngle, 
                                TWS, 
                                TWD, 
                                TWA);
              //*//TWD
              SetN2kWindSpeed (N2kMsg_1, 1, TWS, TWD, N2kWind_True_North); 
              NMEA2000_2.SendMsg(N2kMsg_1); 
              //*
              //*//TWA
              SetN2kWindSpeed (N2kMsg_1, 1, TWS, TWA, N2kWind_True_boat);                           
              NMEA2000_2.SendMsg(N2kMsg_1);
              /*/
              Serial.println ((String)"SOG = " +  msToKnots(SOG) + " COG = " +  RadToDeg(COG)); 
              Serial.println ((String)"AWS = " + msToKnots(WindSpeed)  + ", AWA = " + RadToDeg(WindAngle)); 
              Serial.println ((String)"Boatheading = " +  RadToDeg(Heading)); 
              Serial.print ((String)"TWS = " + msToKnots(TWS)  + ",TWD = " + RadToDeg(TWD)); 
              Serial.println ((String)", TWA = " + RadToDeg(TWA) );
              //*/
          }
     }
  }
}
void tN2kInnerHandler::HandlePosition(const tN2kMsg &N2kMsg) {
    if ( N2kMsg.Source == Boatdata.Position.N2kSource || Boatdata.Position.N2kSource == 255 ){ 
          if ( ParseN2kPGN129025(N2kMsg,
                                 Boatdata.Position.Latitude, 
                                 Boatdata.Position.Longitude
              )) {
                  Boatdata.Position.Lastupdate=millis();
              }
     }
}
void tN2kInnerHandler::HandleGNSS(const tN2kMsg &N2kMsg) {
       if ( N2kMsg.Source == Boatdata.Position.N2kSource || Boatdata.Position.N2kSource == 255 ){ 
            unsigned char SID;
              if ( ParseN2kGNSS(N2kMsg,
                                SID,
                                Boatdata.Datetime.DaysSince1970,
                                Boatdata.Datetime.SecondsSinceMidnight,
                                Boatdata.Position.Latitude,
                                Boatdata.Position.Longitude,
                                Boatdata.Position.Altitude,
                                Boatdata.Position.GNSStype,
                                Boatdata.Position.GNSSmethod,
                                Boatdata.Position.nSatellites,
                                Boatdata.Position.HDOP,
                                Boatdata.Position.PDOP,
                                Boatdata.Position.GeoidalSeparation,
                                Boatdata.Position.nReferenceStations,
                                Boatdata.Position.ReferenceStationType,
                                Boatdata.Position.ReferenceSationID,
                                Boatdata.Position.AgeOfCorrection    
                  )) {
                if ( Boatdata.Datetime.DaysSince1970 > 17553){
                    Boatdata.Position.Lastupdate=millis();
                }
              }
       }
}
void tN2kInnerHandler::PassPGN(const tN2kMsg &N2kMsg) {
    //forward this message
    NMEA2000_2.SendMsg(N2kMsg);
}

void tN2kInnerHandler::CalculateTruewind(double Heading, double SOG, double COG, double AWS, double AWA, double &TWS, double &TWD, double &TWA){
/*/
input ONLY m/s for speeds and ONLY radials for angles. 
function takes 228 microSeconds on arduino Due. 

Include math.h for M_PI, sin(), cos() and atan2() functions

Input Variables: 
Heading = Compass heading ( prefereably TRUE,  but Magnetic should be okay if the display also uses Magnetic)
SOG = Speed over Ground from GPS (m/s)
COG = Course over Ground from GPS (radials)
AWS = Appearant Wind Speed from Wind device (m/s).
AWA = Appearant Wind Angle from wind device, relative to ship centerline. (radials)
Output Variables: 
TWS = Theoretical Wind Speed (m/s)
TWD = Theoretical Wind Direction, relative to north (radials)
TWA = Theoretical Wind Angle, relative to ship centerline (radials)

//*/

//force AWA into +/-pi radials  not 0-2pi radials.
while (AWA > M_PI) AWA -=2*  M_PI; 

//create AWD from Heading,  and fix into 0-2pi radials.
double AWD; 
AWD=Heading + AWA; 
while (AWD < 0 ) AWD += 2* M_PI; 
while (AWD > 2*M_PI ) AWD -= 2* M_PI;


//create boat vector(x,y) from SOG and COG.
double Xboat, Yboat; 
Xboat = sin(COG) * SOG; 
Yboat = cos(COG) * SOG; 

//create wind vector(x,y) from AWD and AWS.
double Xwind, Ywind;
Xwind=sin(AWD) * AWS; 
Ywind=cos(AWD) * AWS; 

//substract boat vector from wind vector to create Truewind Vector. 
double Xtw, Ytw; 
Xtw=Xwind-Xboat;
Ytw=Ywind-Yboat; 

//Calculate lenght of Truewindvector for Truewind Speed.
TWS = sqrt ((Xtw*Xtw) + (Ytw*Ytw)); 

//Calculate Angle of Truewindvector for Truewind direction. 
TWD = atan2 ( Xtw, Ytw); 

//make sure result is 0-2pi radials
while(TWD > (2* M_PI)) TWD -= 2* M_PI; 
while(TWD < 0) TWD += 2* M_PI;

//substract Heading for TrueWind Angle.
TWA = TWD - Heading; 

//for TWA  force 0-2pi radials
while(TWA > (2* M_PI)) TWA -= 2* M_PI; 
while (TWA < 0) TWA += 2*M_PI; 

}
