/**
MIT License

Copyright (c) 2003 Kevin Walchko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

// #include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>
#include "message.hpp"

// Dictionary ----------------------------------------------
using dict = std::map<std::string,std::string>;

static
std::ostream& operator<<(std::ostream&s, const dict& d) {
  for (const auto& [key, value] : d) {
    s << key << ": " << value << std::endl;
  }
  return s;
}

class Ascii {
  public:
  message_t encode(const std::string& msg) {
    message_t ret;
    for (const char& c: msg) {
      if (c  == ' ') ret.push_back('|');
      else ret.push_back((char) c);
    }
    return ret;
  }
  std::string decode(const message_t& msg) {
    std::string ret;
    for (const uint8_t& c: msg) {
      if (c  == '|') ret.push_back(' ');
      else ret.push_back((char) c);
    }
    return ret;
  }

  inline
  message_t pack(const std::string& a) { return encode(a); }
  inline
  std::string unpack(const message_t& m) { return decode(m); }
};

class Raw {
  public:
  message_t encode(const std::string& msg) {
    message_t ret;
    // memcpy(ret.data(), msg.data(), msg.size()); // segfault linux
    for (const char& c: msg) ret.push_back((uint8_t) c);
    // return std::move(ret);
    return ret;
  }
  std::string decode(const message_t& msg) {
    std::string ret;
    // memcpy(ret.data(), msg.data(), msg.size()); // sefault linux
    for (const uint8_t& c: msg) ret.push_back((char) c);
    // return std::move(ret);
    return ret;
  }

  inline
  message_t pack(const std::string& s) { return encode(s); }
  inline
  std::string unpack(const message_t& m) { return decode(m); }
};


class Json {
  public:
  std::string eat(const message_t& s) {
    std::string ret;
    for (int i=0; i < s.size(); ++i) if (s[i] != '"' && s[i] != '{' && s[i] != ' ' && s[i] != '}') ret.push_back((char)s[i]);
    return ret;
  }

  message_t encode(const dict& d) {
    message_t ret;
    ret.push_back((uint8_t)'{');
    for (const auto& [key, value] : d) {
      ret.push_back('"');
      for (const char& c: key) ret.push_back((uint8_t)c);
      ret.push_back('"');
      ret.push_back(':');

      ret.push_back('"');
      for (const char& c: value) ret.push_back((uint8_t)c);
      ret.push_back('"');
      ret.push_back(',');
    }
    ret.pop_back(); // remove trailing comma
    ret.push_back('}');
    return ret;
  }

  dict decode(const message_t& s) {
    dict d;

    message_t line;
    size_t start=0, end=0, colon=0;
    size_t i=0;
    size_t loc=0;
    while (i<s.size()) {
      // clean
      if (s[i] == '"' || s[i] == '{' || s[i] == ' ') {
        i++;
        continue;
      }

      if (s[i] == ':') {
        colon = loc;
        while (i<s.size()) {
          // clean
          if (s[i] == '"' || s[i] == '{' || s[i] == ' ') {
            i++;
            continue;
          }

          if (s[i] == ',' || s[i] == '}') {
            end = loc;
            std::string key(colon-start,0);
            std::string value(end-colon-1,0);
            memcpy(key.data(), &line.data()[start], colon-start);
            memcpy(value.data(), &line.data()[colon+1], end-colon-1);
            // cout << line << " " << start << " " << colon << " " << end << endl;
            d[key] = value;
            line.clear();
            loc=0;
            i++;
            break;
          }
          line.push_back(s[i]);
          i++;loc++;
        }
      }
      // need else because of break from while loop
      else {
        line.push_back(s[i]);
        i++;loc++;
      }
    }
    return d;
  }

  inline
  message_t pack(const dict& d) { return encode(d); }
  inline
  dict unpack(const message_t& m) { return decode(m); }
};

  // dict decode(const message_t& msg) {
  //   dict d;
  //   std::string s;

  //   std::string m = eat(msg); // remove?
  //   std::stringstream ss(m);

  //   while (getline(ss, s, ',')) {
  //     std::string k,v;
  //     std::stringstream sss(s);
  //     std::getline(sss,k,':');

  //     for (int i=k.size()+1; i< s.size(); ++i) if (s[i] != ' ') v.push_back(s[i]);
  //     // std::cerr << k << ": " << v << std::endl;
  //     d[k] = v;
  //   }

    // std::string s;
    // std::string k;
    // char c;

    // int i = 0;
    // while (i < msg.size()) {
    //   c = msg[i];
    //   if (c == '{' || c == ' '); // ignore
    //   else if (c == '"') { // start or end of string
    //     c = msg[++i];
    //     while ( c != '"') {
    //       s.push_back(c);
    //       c = msg[++i];
    //     }
    //   }
    //   else if (c == ':') { // end of key
    //     k = s;
    //     s.clear();
    //   }
    //   else if (c == ',' || c == '}') { // end of key/value pair
    //     d[k] = s;
    //     // cout << k << ": " << s << endl;
    //     s.clear();
    //   }
    //   else s.push_back(c); // int or something else
    //   i++;
    // }

  //   return d;
  // }
// };

// static
// dict decode(const message_t& s) {
//   dict d;

//   message_t line;
//   size_t start=0, end=0, colon=0;
//   size_t i=0;
//   size_t loc=0;
//   while (i<s.size()) {
//     // clean
//     if (s[i] == '"' || s[i] == '{' || s[i] == ' ') {
//       i++;
//       continue;
//     }

//     if (s[i] == ':') {
//       colon = loc;
//       while (i<s.size()) {
//         // clean
//         if (s[i] == '"' || s[i] == '{' || s[i] == ' ') {
//           i++;
//           continue;
//         }

//         if (s[i] == ',' || s[i] == '}') {
//           end = loc;
//           std::string key(colon-start,0);
//           std::string value(end-colon-1,0);
//           memcpy(key.data(), &line.data()[start], colon-start);
//           memcpy(value.data(), &line.data()[colon+1], end-colon-1);
//           // cout << line << " " << start << " " << colon << " " << end << endl;
//           d[key] = value;
//           line.clear();
//           loc=0;
//           i++;
//           break;
//         }
//         line.push_back(s[i]);
//         i++;loc++;
//       }
//     }
//     // need else because of break from while loop
//     else {
//       line.push_back(s[i]);
//       i++;loc++;
//     }
//   }
//   return d;
// }

// dict decode(const message_t& msg) {
//   dict d;

//   // filter ... prefer to not do this loop
//   message_t s;
//   for (int i=0; i<msg.size(); i++) {
//     if (msg[i] != '"' && msg[i] != '{' && msg[i] != ' ') s.push_back((uint8_t)msg[i]);
//   }

//   size_t start=0, end=0, colon=0;
//   size_t i=0;
//   while (i<s.size()) {
//     if (s[i] == ':') {
//       colon = i;
//       i++;
//       while (i<s.size()) {
//         if (s[i] == ',' || s[i] == '}') {
//           end = i;
//           std::string key(colon-start,0);
//           std::string value(end-colon-1,0);
//           memcpy(key.data(), &s.data()[start], colon-start);
//           memcpy(value.data(), &s.data()[colon+1], end-colon-1);
//           d[key] = value;
//           start = end+1; // skip ,
//           break;
//         }
//         i++;
//       }
//     }
//     i++;
//   }
//   return d;
// }
