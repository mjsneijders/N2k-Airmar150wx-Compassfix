#include <NMEA2000.h>


class tN2kOuterHandler : public tNMEA2000::tMsgHandler {


protected:
  void FilterPGN(const tN2kMsg &N2kMsg); 
  void HandleHeading(const tN2kMsg &N2kMsg);//127250
  void PassPGN(const tN2kMsg &N2kMsg);

public:
  tN2kOuterHandler(tNMEA2000 *_pNMEA2000) : tNMEA2000::tMsgHandler(0,_pNMEA2000) {
  }
  void HandleMsg(const tN2kMsg &N2kMsg);
};
