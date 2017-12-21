#ifndef MON_UTIL_H
#define MON_UTIL_H

#include <string>
#include <bitset>
#include <vector>
#include <unordered_map>
#include "StackTrace.h"
#include "ClassDetail.h"
#include "MethodStats.h"
#include "JammedReport.h"
#include "Connector.h"
#include "MarkerField.h"

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

    std::vector<std::string> parse_type_tokens(const std::string& tstr);
    std::string              translate_descriptor(const std::string& name, int* = nullptr);
    std::vector<StackTrace>  construct_traffic(const std::string& msg, std::unordered_map<std::string, MethodStats>& counters);
    JammedReport&            process_jammed(const std::string& monitor, const std::string& waiter, const std::string& owner, std::unordered_map<std::string, JammedReport>& jcount);
    std::vector<ClassDetail> construct_class_details(const std::string& msg);
    void transmit_lines(const std::string& fileName, const std::string& ipport, frenchroast::network::Connector<>&);
    void transmit_lines(const std::vector<std::string>&, const std::string& ipport, frenchroast::network::Connector<>&);
    MarkerField build_marker( std::string str);
    std::vector<std::string> build_instance_headers(const std::string& subkey);

  }};

#endif
