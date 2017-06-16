#include "testing_utils.h"
#include <iostream>
#include <fstream>

std::unique_ptr<const unsigned char> fr_test_util::get_class_buf(const std::string& fileName)
{
  std::ifstream klass{fileName, std::ios::binary};
  std::filebuf* buf = klass.rdbuf();
  std::size_t length = buf->pubseekoff(0,klass.end,klass.in);
  buf->pubseekpos(0,klass.in);
  std::unique_ptr<unsigned char> kbuf(new unsigned char[length]);
  buf->sgetn(reinterpret_cast<char*>(kbuf.get()),length);
  klass.close();
  return kbuf;
}

void fr_test_util::put_class_buf(const std::string& fileName, unsigned char * buf, size_t size)
{
  std::ofstream klass{fileName, std::ios::binary};
  if(klass) {
    std::filebuf* rdbuf = klass.rdbuf();
    rdbuf->sputn(reinterpret_cast<char*>(buf), size);
    klass.close();
  }
  else {
    throw std::ios_base::failure("cannot open " + fileName + " for write"  );
  }
}
