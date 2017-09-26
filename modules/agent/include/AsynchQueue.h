// copyright (c) 2017 Richard Guadagno
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
//    along with French-Roast.  If not, see <http://www.gnu.org/licenses/>.
//

#include <queue>
#include <mutex>
#include <condition_variable>


namespace frenchroast {
  
template
<typename T>
class AsynchQueue {

  std::queue<T> _q;
  std::mutex   _lock;
  std::condition_variable _proceed;
  bool is_empty;
 public:
  void push(T item)
  {
    std::unique_lock<std::mutex> lck(_lock);
    is_empty = _q.empty();
    _q.push(item);
    //    if(_q.size() > 10) std::cout << "-------- " << _q.size() << std::endl;
    if(is_empty)
      _proceed.notify_one();
  }
  
  void get(T& item)
  {
    std::unique_lock<std::mutex> lck(_lock);
    while(_q.empty()) _proceed.wait(lck);
    item = std::move(_q.front());
    _q.pop();
  }
  
  };

}
