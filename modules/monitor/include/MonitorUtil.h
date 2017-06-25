#ifndef MON_UTIL_H
#define MON_UTIL_H

#include <string>

namespace frenchroast {  namespace monitor {

    std::string ntoa(int x,int pad = 0);
    std::string format_millis(long millis);
    const char* get_env_variable(const std::string& var, const std::string& msg = "");
  }};

#endif
