#ifndef RRPT_H
#define RRPT_H

namespace frenchroast { namespace monitoring {

    class Reporter {
    public:
      void init(const std::string& initinfo);
      void hook(const std::string& tag);
    };
    
  }
}

#endif
