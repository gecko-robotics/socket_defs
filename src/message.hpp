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
// #include <iostream>
#include <ostream>

/*
DEC =    BIN    = HEX
240 = 1111 0000 = F0
170 = 1010 1010 = AA
85  = 0101 0101 = 55
15  = 0000 1111 = 0F
*/

// struct __attribute__((packed)) subscription_t {
//   uint8_t topic;
//   uint8_t status;
// };

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

static
std::string msg2string(const message_t& msg) {
  std::string s;
  if (msg.size() == 0) return s;
  s += "[";
  for (const uint8_t& m: msg) {
    s += std::to_string(int(m));
    s += ",";
  }
  s += "]";
  return s;
}

static
message_t string2msg(const std::string& msg) {
  message_t s;
  for (int i=0; i<msg.size(); i++) {
    // if (msg[i] != '"' && msg[i] != '{' && msg[i] != ' ')
    s.push_back((uint8_t)msg[i]);
  }
  return s;
}

static
std::ostream &operator<<(message_t &msg, const std::string& s) {
  return os << msg2string(msg);
}

static
std::ostream &operator<<(std::ostream &os, message_t const &msg) {
  return os << msg2string(msg);
}

// static
// std::ostream& operator<<(std::ostream& s, const message_t& m) {
//   for (size_t i=0; i<m.size(); ++i) s << (char)m[i];
//   return s;
// }
