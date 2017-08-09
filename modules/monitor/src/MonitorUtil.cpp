#include <iostream>
#include "MonitorUtil.h"
#include "Util.h"


std::string frenchroast::monitor::ntoa(int x, int amount, char ch )
{
  std::string rv = std::to_string(x);
  return std::string(  rv.length() < amount ? std::string(amount - rv.length(), ch) : "").append(rv);
}

void frenchroast::monitor::pad(std::string& str, int amount, char ch)
{
  str.append(std::string(  str.length() < amount ? std::string(amount - str.length(), ch) : ""));
}

std::string  frenchroast::monitor::pad(const std::string& str, int amount, char ch)
{
  std::string rv{str};
  pad(rv, amount, ch);
  return rv;
}

std::string frenchroast::monitor::pad_front(const std::string& str, int amount, const std::string point )
{
  const std::string lstr =  frenchroast::split(str, point)[0];
  return std::string(  lstr.length() < amount ? std::string(amount - lstr.length(), ' ') : "") + lstr + point + frenchroast::split(str, point)[1];
}


std::string frenchroast::monitor::format_millis(long millis, std::bitset<4> format)
{
  int hours = millis / (3600000);
  millis -= hours * 3600000;
  int min   = millis / 60000;
  millis -= min * 60000;
  int sec   = millis / 1000;
  millis -=  sec * 1000;

  std::string rv = "";
  if((format & FORMAT_HOURS) == FORMAT_HOURS) {
    rv.append(ntoa(hours,0));
  }

  if((format & FORMAT_MINUTES) == FORMAT_MINUTES) {
    if(rv != "") rv.append(":");
    rv.append(ntoa(min,2));
  }

  if((format & FORMAT_SECONDS) == FORMAT_SECONDS) {
    if(rv != "") rv.append(":");
    rv.append(ntoa(sec,2));
  }

  if((format & FORMAT_MILLISECONDS) == FORMAT_MILLISECONDS) {
    if(rv != "") rv.append(":");
    rv.append(ntoa(millis,3));
  }

  return rv;
}

const char* frenchroast::monitor::get_env_variable(const std::string& var, const std::string& msg) 
{
  const char* rv = std::getenv(var.c_str());
  if(rv == nullptr) {
    std::cout << "environment variable: " + var + " is not set: " << msg << std::endl;
    exit(0);
  }
  
  return rv;
}
