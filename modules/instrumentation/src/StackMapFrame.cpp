// copyright (c) 2016 Richard Guadagno
// contact: rrguadagno@gmail.com
//
// This file is part of French-Roast
//
//    French-Roast is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    French-Roast is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
//

#include "StackMapFrame.h"
#include "FrameConst.h"

namespace frenchroast {

  class VerificationType {
  protected:
    BYTE _type;
  public:
    virtual int size_in_bytes() const { return 1; }
    virtual void adjust_offset(int amount) {}
    virtual void load_from_buffer(BYTE* buf) { _type = *buf;} 
    virtual void load_to_buffer(BYTE* buf)  { *buf = _type; }
  };

  class TopVerificationType : public VerificationType {};
  class IntegerVerificationType : public VerificationType {};
  class FloatVerificationType : public VerificationType {};
  class DoubleVerificationType : public VerificationType {};
  class LongVerificationType : public VerificationType {};
  class NullVerificationType : public VerificationType {};
  class UninitializedThisVerificationType : public VerificationType {};
  class ObjectVerificationType : public VerificationType {
    BYTE _constPoolIndex[2];
  public:
    int size_in_bytes() const
    {
      return 3;
    }

    void adjust_offset(int amount)
    {

    }

    void load_from_buffer(BYTE* buf)
    {
      _type = *buf;
      memcpy(_constPoolIndex, buf+1,2);
    }
    
    void load_to_buffer(BYTE* buf)
    {
      *buf = _type;
      memcpy(buf+1,_constPoolIndex,2);
    }
  };
    
  class UninitializedVerificationType : public VerificationType {
    BYTE _offset[2];
  public:
    int size_in_bytes() const
    {
      return 3;
    }

    void adjust_offset(int amount)
    {
      short offset = to_int(_offset,2) + amount;
      write_big_e_bytes(_offset,&offset);
    }

    void load_from_buffer(BYTE* buf)
    {
      _type = *buf;
      memcpy(_offset, buf+1,2);
    } 

    void load_to_buffer(BYTE* buf)
    {
      *buf = _type;
      memcpy(buf+1,_offset,2);
    }
  };


  void load_verification_items(int count, BYTE* buf, std::vector<VerificationType*>& items)
  {
    int startPos=0;
    for(int idx=1;idx<=count;idx++) {
      VerificationType* ptr = nullptr;
      BYTE vtype =  *(buf + startPos);
      if(vtype == 0) ptr = new TopVerificationType();
      if(vtype == 1) ptr = new IntegerVerificationType();
      if(vtype == 2) ptr = new FloatVerificationType();
      if(vtype == 3) ptr = new DoubleVerificationType();
      if(vtype == 4) ptr = new LongVerificationType();
      if(vtype == 5) ptr = new NullVerificationType();
      if(vtype == 6) ptr = new UninitializedThisVerificationType();
      if(vtype == 7) ptr = new ObjectVerificationType();
      if(vtype == 8) ptr = new UninitializedVerificationType();
      if(ptr == nullptr) throw std::invalid_argument("BAD v type");
      ptr->load_from_buffer(buf + startPos);
      startPos += ptr->size_in_bytes();
      items.push_back(ptr);
    }
  }

  void save_verification_items(BYTE* buf, std::vector<VerificationType*>& items)
  {
    int startPos = 0;
    
    for(auto& x : items) {
      x->load_to_buffer(buf + startPos);
      startPos += x->size_in_bytes();
    }
  }

    
  class SameFrame : public StackMapFrame {
  public:

    operator int()
    {
      return stackmapframe::same;
    }
    
    int size_in_bytes() const
    {
      return 1 ;
    }

    bool adjust_offset(short offset)
    {
      if (offset > 63) {
         return false;
      }
      _frameType = offset;
      return true;
    }

    int offset() const
    {
      return _frameType;
    }
    
    void load_from_buffer(BYTE* buf)
    {
      _frameType = *buf;
    }

    void load_to_buffer(BYTE* buf)
    {
      *buf = _frameType;
    }
  };

  SameFrameExtended::SameFrameExtended() 
    {
       _frameType = 251;
    }

    SameFrameExtended::operator int()
    {
      return stackmapframe::same_extended;
    }

    int SameFrameExtended::size_in_bytes() const
    {
      return _size;
    }

    bool SameFrameExtended::adjust_offset(short offset)
    {
      write_big_e_bytes(_offsetDelta,&offset);
      return true;
    }

    int SameFrameExtended::offset() const
    {

      return to_int(_offsetDelta,2);
    }
    
    void SameFrameExtended::load_from_buffer(BYTE* buf)
    {
      _frameType = *buf;
      memcpy(_offsetDelta,buf +1, 2);
      _size = 1 + sizeof(_offsetDelta);
    }

    void SameFrameExtended::load_to_buffer(BYTE* buf)
    {
      memcpy(buf,    &_frameType, 1);
      memcpy(buf +1, _offsetDelta,2);
    }
  
  class SameLocalsOneStackFrame : public StackMapFrame {
    std::vector<VerificationType*> _verificationInfoList;
    int _size;
  public:
    operator int()
    {
      return stackmapframe::same_local_one_stack;
    }

    int size_in_bytes() const
    {
      return _size;
    }

    bool adjust_offset(short offset)
    {
      if (offset > 63) {
        return false;
      }
      _frameType = 64 + offset;
      return true;
    }

    int offset() const
    {
      return _frameType - 64;
    }
    
    void load_from_buffer(BYTE* buf)
    {
      _frameType = *buf;
      load_verification_items(1, buf + 1, _verificationInfoList);
      _size = 1;
      for (auto& x : _verificationInfoList) {
        _size += x->size_in_bytes();
      }
    }

    void load_to_buffer(BYTE* buf)
    {
      *buf = _frameType;
      save_verification_items(buf +  1, _verificationInfoList);
    }
  };

  class AppendFrame : public StackMapFrame {
    BYTE _offsetDelta[2];
    std::vector<VerificationType*> _verificationInfoList;
    int _size;
  public:
    operator int()
    {
      return stackmapframe::append;
    }

    int size_in_bytes() const
    {
      return _size;
    }

    bool adjust_offset(short offset)
    {
      write_big_e_bytes(_offsetDelta,&offset);
      return true;
    }

    int offset() const
    {
      return to_int(_offsetDelta,2);
    }
    
    void load_from_buffer(BYTE* buf)
    {
      _frameType = *buf;
      memcpy(_offsetDelta,buf +1, 2);
      int totalVerificationTypes = _frameType - 251;
      load_verification_items(totalVerificationTypes, buf + 3, _verificationInfoList);
      _size = 3;
      for (auto& x : _verificationInfoList) {
        _size += x->size_in_bytes();
      }
    }

    void load_to_buffer(BYTE* buf)
    {
      memcpy(buf,    &_frameType, 1);
      memcpy(buf +1, _offsetDelta,2);
      save_verification_items(buf +  3, _verificationInfoList);
    }
  };

  class FullFrame : public StackMapFrame {
    BYTE _offsetDelta[2];
    BYTE _localsCount[2];
    BYTE _stackCount[2];
    
    std::vector<VerificationType*> _verificationLocals;
    std::vector<VerificationType*> _verificationStack;
    int _size;
  public:
    operator int()
    {
      return stackmapframe::full;
    }

    int size_in_bytes() const
    {
      return _size;
    }

    bool adjust_offset(short offset)
    {
      write_big_e_bytes(_offsetDelta,&offset);
      return true;
    }

    int offset() const
    {
      return to_int(_offsetDelta,2);
    }
    
    void load_from_buffer(BYTE* buf)
    {
      _frameType = *buf;
      memcpy(_offsetDelta,buf +1, 2);
      memcpy(_localsCount,buf +3, 2);
      int total = to_int(_localsCount, 2);
      load_verification_items(total, buf + 5, _verificationLocals);
      _size = 5;
      for (auto& x : _verificationLocals) {
        _size += x->size_in_bytes();
      }
      
      memcpy(_stackCount,buf + _size, 2);
      _size +=2;
      total = to_int(_stackCount, 2);
      load_verification_items(total, buf + _size, _verificationStack);
      for(auto& x : _verificationStack) {
        _size += x->size_in_bytes();
      }
    }

    void load_to_buffer(BYTE* buf)
    {
      memcpy(buf,    &_frameType, 1);
      memcpy(buf +1, _offsetDelta,2);
      memcpy(buf +3, _localsCount,2);
      save_verification_items(buf +  5, _verificationLocals);
      int size = 5;
      
      for (auto& x : _verificationLocals) {
        size += x->size_in_bytes();
      }
      
      memcpy(buf + size, _stackCount,2);
      save_verification_items(buf +  size + 2, _verificationStack);
    }
  };


  class ChopFrame : public StackMapFrame {
    BYTE _offsetDelta[2];
    int _size;
  public:
    operator int()
    {
      return stackmapframe::chop;
    }

    int size_in_bytes() const
    {
      return _size;
    }

    bool adjust_offset(short offset)
    {
      write_big_e_bytes(_offsetDelta,&offset);
      return true;
    }

    int offset() const
    {
      return to_int(_offsetDelta,2);
    }
    
    void load_from_buffer(BYTE* buf)
    {
      _frameType = *buf;
      memcpy(_offsetDelta,buf +1, 2);
      _size = 1 + sizeof(_offsetDelta);
    }

    void load_to_buffer(BYTE* buf)
    {
      memcpy(buf,    &_frameType, 1);
      memcpy(buf +1, _offsetDelta,2);
    }

  };
  
  std::vector<StackMapFrame*> load_frames_from_buffer(int count,BYTE* buf)
  {
    std::vector<StackMapFrame*> rv;
    int startPos = 0;
    for(int idx = 1; idx <= count; idx++) {
      StackMapFrame* ptr = nullptr;
      BYTE ftype =  *(buf + startPos);

      if (ftype >= 0 && ftype <= 63) {
        ptr = new SameFrame();
      }

      if (ftype >= 64 && ftype <= 127) {
        ptr = new SameLocalsOneStackFrame();
      }

      if (ftype >= 252 && ftype <= 254) {
        ptr = new AppendFrame();
      }

      if (ftype >= 248 && ftype <= 250) {
        ptr = new ChopFrame();
      }

      if (ftype == 251) {
        ptr = new SameFrameExtended();
      }
      
      if (ftype == 255) {
        ptr = new FullFrame();
      }

      if(ptr == nullptr) {
        std::cout << "MISSING STACK FRAME: " << (int)ftype << std::endl;
        exit(0);
      }
    
      ptr->load_from_buffer(buf + startPos);
      startPos += ptr->size_in_bytes();
      rv.push_back(ptr);
    }
    return rv;
  }
    
  void load_frames_to_buffer(std::vector<StackMapFrame*> items, BYTE* buf)
  {
    int startPos=0;
    for(auto& x : items) {
      x->load_to_buffer(buf+startPos);
      startPos+=x->size_in_bytes();
    }
  }
    
}
