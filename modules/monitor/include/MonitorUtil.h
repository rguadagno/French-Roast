#ifndef MON_UTIL_H
#define MON_UTIL_H

#include <string>
#include <bitset>

namespace frenchroast {  namespace monitor {

    const std::bitset<4> FORMAT_HOURS{"0001"};
    const std::bitset<4> FORMAT_MINUTES{"0010"};
    const std::bitset<4> FORMAT_SECONDS{"0100"};
    const std::bitset<4> FORMAT_MILLISECONDS{"1000"};

    std::string ntoa(int x,int pad, char = '0');
    std::string format_millis(long millis, std::bitset<4> format = FORMAT_HOURS | FORMAT_MINUTES | FORMAT_SECONDS | FORMAT_MILLISECONDS);
    const char* get_env_variable(const std::string& var, const std::string& msg = "");
    void pad(std::string& str, int amount, char ch = ' ');
    std::string pad(const std::string& str, int amount, char ch = ' ');
    std::string pad_front(const std::string& str, int amount, const std::string point );



  }};

#endif
