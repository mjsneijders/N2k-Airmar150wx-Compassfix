#ifndef _BoatData_H_
#define _BoatData_H_

#include <N2kMessages.h>
struct tBoatData {
  struct tBDdatetime {
    unsigned long Lastupdate; 
    uint16_t Systemdate;
    double Systemtime;
    tN2kTimeSource Source; 
    uint16_t DaysSince1970;
    double SecondsSinceMidnight;
  };

  struct tBDposition {
    unsigned long Lastupdate; 
    unsigned long LastCOGSOGTime;
    double Latitude;
    double Longitude; 
    double Altitude;
    tN2kGNSStype GNSStype; 
    tN2kGNSSmethod GNSSmethod;
    unsigned char nSatellites; 
    double HDOP; 
    double PDOP; 
    double GeoidalSeparation;
    unsigned char nReferenceStations; 
    tN2kGNSStype ReferenceStationType; 
    uint16_t ReferenceSationID;
    double AgeOfCorrection;
    unsigned char  N2kSource; 
  };
  
  struct tBDHeading {
    unsigned long LastTrueHeadingTime;
    unsigned long LastMagneticHeadingTime;
    unsigned long LastCOGSOGTime; 
    tN2kHeadingReference ref;
    tN2kMagneticVariation source; 
    double Deviation;
    double Variation;
    double MagneticHeading;
    double TrueHeading;
    double COG;
    double SOG;
    unsigned char N2kSource; 
    unsigned char N2kVariationSource; 
  };
  struct tBDWind{
      unsigned long Lastupdate;
      double Speed; 
      double Angle; 
    };
  struct tBDWeather{
    unsigned char  N2kSource; 
    tBDWind TrueWind;
    tBDWind AppWind;
  };
  struct tBDDepth {
    unsigned long Lastupdate; 
    double DepthBelowTransducer;
    double Offset;
    double Range;

    
  };
  struct tBDnavigation {
    unsigned long Lastupdate;
    double DistanceToWaypoint;
    tN2kHeadingReference BearingReference;
    bool PerpendicularCrossed;
    bool ArrivalCircleEntered;
    tN2kDistanceCalculationType CalculationType;
    double ETATime;
    int16_t ETADate;
    double BearingOriginToDestinationWaypoint; 
    double BearingPositionToDestinationWaypoint;
    uint8_t OriginWaypointNumber;
    uint8_t DestinationWaypointNumber;
    double DestinationLatitude;
    double DestinationLongitude;
    double WaypointClosingVelocity;
    tN2kXTEMode XTEMode;
    bool NavigationTerminated;
    double XTE;
    unsigned char  N2kSource; 
  };
  struct tBDAccu{
    unsigned long Lastupdate;
    double Voltage; 
    double Current; 
    double Temperature; 
    uint8_t StateOfCharge;
    double TimeRemaining ;  
    unsigned char  N2kSource;   
  };
  struct tBDPower {
    tBDAccu Accu1; 
    tBDAccu Accu2;
    tBDAccu Accu3; 
  };
  struct tBDLevel {
    unsigned long Lastupdate;
    double Level; 
    double Capacity; 
    tN2kFluidType FluidType; 
    unsigned char  N2kSource; 
  };
  struct tBDTank {
    tBDLevel Diesel; 
    tBDLevel Water; 
    tBDLevel Bilge; 
  };
  unsigned long Lastupdate;
  tBDposition Position; 
  tBDdatetime Datetime; 
  tBDnavigation Navigation;
  tBDHeading Heading; 
  tBDDepth Depth; 
  tBDPower Power; 
  tBDWeather Weather; 
  tBDTank Tank; 

};

extern tBoatData Boatdata;
#endif // _BoatData_H_
