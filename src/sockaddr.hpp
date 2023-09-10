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

#include <string>
#include <string_view>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_sockaddr
#include <ostream>
#include <regex>
#include <sys/un.h>    // UDS
#include <unistd.h>  // unlink


// Globals new types --------------------------------------------
constexpr int SOCKET_ERR = -1;
constexpr int SOCKET_TIMEOUT = -1;
constexpr int SOCKET_OK = 0;
constexpr uint32_t AF_ERROR = 100;

struct group_t {
  std::string addr;
  uint16_t port;
};

using sockaddr_t = struct sockaddr; // value?
using inetaddr_t = struct sockaddr_in; // udp or tcp
using unixaddr_t = struct sockaddr_un; // uds
using socket_fd_t = int; // socket descriptor


static
const inetaddr_t inet_sockaddr(const std::string &addr, uint16_t port) {
  inetaddr_t inet{0};
  inet.sin_family      = AF_INET;
  inet.sin_addr.s_addr = inet_addr(addr.c_str());
  inet.sin_port        = htons(port);
  return inet;
}

inline
const inetaddr_t inet_sockaddr(const group_t grp) {
  return inet_sockaddr(grp.addr, grp.port);
}

static
const inetaddr_t inet6_sockaddr(const std::string &addr, uint16_t port) {
  inetaddr_t inet{0};
  return inet;
}

// to_string ]------------------------------------------------------
inline
std::string to_string(const inetaddr_t &addr) {
  char ip[32]{0};

  if (addr.sin_family == AF_INET) {
    ::inet_ntop(AF_INET, &(addr.sin_addr),ip,sizeof(ip));
    std::string host(ip,strlen(ip));
    host += ":" + std::to_string(ntohs(addr.sin_port));
    return host;
  }
  else if (addr.sin_family == AF_INET6) { return "ipv6 unsupported"; }
  return "not inet";
}

inline
std::string to_string(const unixaddr_t &addr) {
  if (addr.sun_family != AF_UNIX) return "";
  return std::string(addr.sun_path);
}

// Operators ]-----------------------------------------------------
static
std::ostream& operator<<(std::ostream &os, inetaddr_t const &addr) {
  os << to_string(addr);
  return os;
}

static
std::ostream &operator<<(std::ostream &os, unixaddr_t const &addr) {
  os << "unix://" << to_string(addr);
  return os;
}

// Filtering URI Paths ]------------------------------------------------
template<typename T>
static
const T filter(const std::string& address);

template<>
const inetaddr_t filter(const std::string& address) {
  std::regex re("(udp|tcp):\\/\\/([a-zA-Z\\d.*]+):(\\d{1,5})");
  std::smatch m;
  uint16_t port;
  uint32_t ip;

  try {
    // find [original, protocol, ip, port]
    regex_search(address, m, re);
    inetaddr_t ans{0};
    ans.sin_family = AF_ERROR;

    if (m.size() != 4) throw std::invalid_argument("invalid inet protocol");
    if (m[1] == "tcp") throw std::invalid_argument("not handling tcp right now");
    if (m[1] != "udp") throw std::invalid_argument("invalid inet protocol");

    else if (m[2] == "*") ip = INADDR_ANY;
    else if (m[2] == "bc") ip = INADDR_BROADCAST;
    else ip = inet_addr(m[2].str().c_str());

    if (m[3] == "*") port = 0; // first open port
    else port = stoi(m[3]);

    ans.sin_family      = AF_INET;
    ans.sin_addr.s_addr = ip;
    ans.sin_port        = htons(port);
    return ans;
  }
  catch (std::regex_error e) {
    throw std::invalid_argument(e.what());
  }
}


template<>
const unixaddr_t filter(const std::string& address) {
  std::regex fmt("(unix)\\:\\/\\/([a-zA-Z\\d\\/._\\-:]+)");
  std::smatch m;
  try {
    // find [original, unix, path]
    regex_search(address, m, fmt);

    if (m.size() != 3) throw std::invalid_argument("invalid format");
    if (m.str(1) != "unix") throw std::invalid_argument("invalid unix protocol");
    std::string path = m.str(2);

    unixaddr_t ans{0};
    ans.sun_family = AF_UNIX;
    strncpy(ans.sun_path, path.c_str(), path.size());

    return ans;
  }
  catch (std::regex_error e) {
    throw std::invalid_argument(e.what());
  }
}

// Converters ]------------------------------------------------
inline
const inetaddr_t inet_sockaddr(const std::string &path) {
  return filter<inetaddr_t>(path);
}

inline
const unixaddr_t unix_sockaddr(const std::string &path) {
  return filter<unixaddr_t>(path);
}

static
const unixaddr_t unix_sockaddr() {
  uint64_t msec = 0;
  using namespace std::chrono;
  msec = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
  std::string file = "/tmp/" + std::to_string(msec);
  unlink(file.c_str());
  return unix_sockaddr("unix://" + file);
}
