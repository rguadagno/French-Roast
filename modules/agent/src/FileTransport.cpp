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
#include "FileTransport.h"


namespace frenchroast { namespace agent {

    FileTransport::FileTransport(const std::string& filename)
    {
      try {
        _outfile.exceptions(std::ifstream::failbit);
        _outfile.open(filename);
      }
      catch(std::ifstream::failure&) {
        if(!_outfile.eof())
          throw std::ifstream::failure("cannot open file: " + filename);
      }
    }  

    void FileTransport::out(const std::string& tag)
    {
      _outfile << tag << std::endl;
    }
    
    /*void ReporterFile::close()
    {
      _outfile.close();
      }*/

  }
}
