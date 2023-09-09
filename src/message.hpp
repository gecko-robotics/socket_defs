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

#include <vector>
#include <ostream>
#include <stdio.h> // printing hex, sprintf
#include <numeric> // std::accumulate

using message_t = std::vector<uint8_t>;
// constexpr uint8_t SUBSCRIBE = 0xAA;
// constexpr uint8_t UNSUBSCRIBE = 0x55;

template<typename T>
message_t pack(const T& d) {
  message_t msg(sizeof(d));
  memcpy(static_cast<uint8_t*>(msg.data()), &d, sizeof(d));
  return std::move(msg);
}

template<typename T>
T unpack(const message_t& m) {
  T d;
  memcpy(&d, m.data(), m.size());
  return std::move(d);
}

namespace HIDDEN {
static
std::string conv_u8(const uint8_t u8) {
  char ss[4];
  snprintf(ss, 4, "0x%X", u8);
  return std::string(ss);
}
}

// https://www.techiedelight.com/convert-a-vector-to-a-string-in-cpp/
static
std::string to_string(const message_t& msg) {
  std::string s;
  if (msg.size() == 0) return s;
  return std::accumulate(msg.begin()+1, msg.end(), HIDDEN::conv_u8(msg[0]),
    [](const std::string& a, uint8_t b) {
      return a + "," + HIDDEN::conv_u8(b);
    }
  );
}


static
message_t &operator<<(message_t &msg, const std::string& s) {
  for (int i=0; i<s.size(); ++i) msg.push_back((uint8_t)s[i]);
  return msg;
}

static
std::ostream &operator<<(std::ostream &os, message_t const &msg) {
  return os << to_string(msg);
}

// [[deprecated("use std::string << message_t")]]
// static
// message_t string2msg(const std::string& msg) {
//   message_t s;
//   for (int i=0; i<msg.size(); i++) {
//     // if (msg[i] != '"' && msg[i] != '{' && msg[i] != ' ')
//     s.push_back((uint8_t)msg[i]);
//   }
//   return s;
// }

// // https://www.techiedelight.com/convert-a-vector-to-a-string-in-cpp/
// [[deprecated("use std::string = to_string(message_t)")]]
// static
// std::string msg2string(const message_t& msg) {
//   std::string s;
//   if (msg.size() == 0) return s;
//   // s += "[";
//   // for (const uint8_t& m: msg) {
//   //   // char ss[4]; //std::to_string(int(m));
//   //   // sprintf(ss, "0x%X", m);
//   //   // s += std::string(ss);
//   //   s += conv(m);
//   //   s += ",";
//   // }
//   // s += "]";
//   return std::accumulate(msg.begin()+1, msg.end(), HIDDEN::conv_u8(msg[0]),
//     [](const std::string& a, uint8_t b) {
//       return a + "," + HIDDEN::conv_u8(b);
//     }
//   );
// }