#include <iostream>
#include "MonitorUtil.h"


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

std::string frenchroast::monitor::format_millis(long millis)
{
  int hours = millis / (3600000);
  millis -= hours * 3600000;
  int min   = millis / 60000;
  millis -= min * 60000;
  int sec   = millis / 1000;
  millis -=  sec * 1000;

  return ntoa(hours,0) + ":" + ntoa(min,2) + ":" + ntoa(sec,2) + ":" + ntoa(millis,3);
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
