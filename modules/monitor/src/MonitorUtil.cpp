
#include "MonitorUtil.h"


std::string frenchroast::monitor::ntoa(int x, int pad )
{
  std::string rv = std::to_string(x);
  if (rv.length() < pad) {
    std::string pstr = "00000";
    rv = pstr.substr(0, pad - rv.length()) + rv;
  }
  return rv;
}


std::string frenchroast::monitor::format_millis(long millis)
{
  int hours = millis / (3600000);
  millis -= hours * 3600000;
  int min   = millis / 60000;
  millis -= min * 60000;
  int sec   = millis / 1000;
  millis -=  sec * 1000;

  return ntoa(hours) + ":" + ntoa(min,2) + ":" + ntoa(sec,2) + ":" + ntoa(millis,3);
}
