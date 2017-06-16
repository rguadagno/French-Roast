#ifndef TESTING_UTILS_H
#define TESTING_UTILS_H

#include <string>
#include <memory>

namespace fr_test_util
{
  std::unique_ptr<const unsigned char> get_class_buf(const std::string& fileName);
  void put_class_buf(const std::string& fileName, unsigned char * buf, size_t size);
}

#endif
