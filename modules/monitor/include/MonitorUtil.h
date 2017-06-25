#ifndef MON_UTIL_H
#define MON_UTIL_H

#include <string>

namespace frenchroast {  namespace monitor {

    std::string ntoa(int x,int pad, char = '0');
    std::string format_millis(long millis);
    const char* get_env_variable(const std::string& var, const std::string& msg = "");
    void pad(std::string& str, int amount, char ch = ' ');
    std::string pad(const std::string& str, int amount, char ch = ' ');
  }};

#endif
