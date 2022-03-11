#include <NMEA2000.h>


class tN2kInnerHandler : public tNMEA2000::tMsgHandler {


protected:
  void FilterPGN(const tN2kMsg &N2kMsg); 
  void HandleWind(const tN2kMsg &N2kMsg); // 130306
  void HandlePosition(const tN2kMsg &N2kMsg);//129025
  void HandleCOGSOG(const tN2kMsg &N2kMsg); //129026
  void HandleGNSS(const tN2kMsg &N2kMsg); //129029
  void PassPGN(const tN2kMsg &N2kMsg);
  void HandleHeading(const tN2kMsg &N2kMsg); 
  void CalculateTruewind(double Heading, double SOG, double COG, double AWS, double AWA, double &TWS, double &TWD, double &TWA);
public:
  tN2kInnerHandler(tNMEA2000 *_pNMEA2000) : tNMEA2000::tMsgHandler(0,_pNMEA2000) {
  }
  void HandleMsg(const tN2kMsg &N2kMsg);
};
