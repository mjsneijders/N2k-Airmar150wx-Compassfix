#include "N2kDataOuterHandler.h"
#include <N2kMessages.h>
#include <math.h>
#include "BoatData.h"
extern tNMEA2000 &NMEA2000;
extern tNMEA2000 &NMEA2000_2;
extern tBoatData Boatdata; 


void tN2kOuterHandler::HandleMsg(const tN2kMsg &N2kMsg) {
  switch (N2kMsg.PGN) {
    case 60928UL: PassPGN(N2kMsg);break;  //ISO address claim
    case 127250UL: HandleHeading(N2kMsg);break; 
    default: FilterPGN(N2kMsg);break;
  }
}


void tN2kOuterHandler::FilterPGN(const tN2kMsg &N2kMsg) {
  //filter this message
}
void tN2kOuterHandler::PassPGN(const tN2kMsg &N2kMsg) {
    //forward this message
    NMEA2000_2.SendMsg(N2kMsg);
}
void tN2kOuterHandler::HandleHeading(const tN2kMsg &N2kMsg) {
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
