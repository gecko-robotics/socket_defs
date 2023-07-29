
#pragma once


// #include <errno.h>
#include <string>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_sockaddr
#include <ostream>
#include <regex>
#include <sys/un.h>     // UDS
#include <stdexcept>


// extern int errno; // don't like this global value

// Globals new types --------------------------------------------
constexpr int SOCKET_ERR = -1;
constexpr int SOCKET_TIMEOUT = -1;
constexpr int SOCKET_OK = 0;
constexpr uint32_t AF_ERROR = 100;

struct group_t {
  std::string addr;
  uint16_t port;
};

using sockaddr_t = struct sockaddr;
using inetaddr_t = struct sockaddr_in; // udp or tcp
using unixaddr_t = struct sockaddr_un; // uds
using socket_fd_t = int; // socket descriptor


static
const inetaddr_t inet_sockaddr(const std::string &addr, uint16_t port) {
  inetaddr_t inet{0};
  inet.sin_family      = AF_INET;
  inet.sin_addr.s_addr = inet_addr(addr.c_str());
  inet.sin_port        = htons(port);
  // return std::move(inet);
  return inet;
}

inline
const inetaddr_t inet_sockaddr(const group_t grp) {
  return inet_sockaddr(grp.addr, grp.port);
}

static
const inetaddr_t inet6_sockaddr(const std::string &addr, uint16_t port) {
  inetaddr_t inet{0};
  // return std::move(inet);
  return inet;
}

static
std::string inet2string(const inetaddr_t &addr) {
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

static
std::ostream& operator<<(std::ostream &os, inetaddr_t const &s) {
  os << inet2string(s);
  return os;
}

static
std::string unix2string(const unixaddr_t &addr) {
  if (addr.sun_family == AF_UNIX) return std::string(addr.sun_path);
  return "not unix";
}

static
std::ostream &operator<<(std::ostream &os, unixaddr_t const &s) {
  os << "File: " << unix2string(s);
  return os;
}


template<typename T>
static
const T filter(const std::string& address);

template<>
const inetaddr_t filter(const std::string& address) {
  // std::regex proto("(udp|tcp)\\:\\/\\/([a-z,A-Z,\\d,\\/,.,*,_,-,:]+)");
  // std::regex proto("(udp|tcp):\\/\\/([a-zA-Z\\d.*_-]+):(\\d{1,5})");
  std::regex re("(udp|tcp):\\/\\/([a-zA-Z\\d.*]+):(\\d{1,5})");
  std::smatch m;
  uint16_t port;
  uint32_t ip;

  // find [original, protocol, path|ip:port]
  regex_search(address, m, re);
  inetaddr_t ans{0};
  ans.sin_family = AF_ERROR;

  if (m.size() != 4) throw std::invalid_argument("invalid inet protocol");
  if (m[1] == "tcp") throw std::invalid_argument("not handling tcp right now");
  if (m[1] != "udp") throw std::invalid_argument("invalid inet protocol");
  // std::cerr << m[1] << m.size() << std::endl;

  // uint16_t port;
  // uint32_t ip;
  // std::string ss = m[2];
  // std::regex ipport("([a-z,A-Z,\\d,\\/,.,*]+):([*,\\d]+)");
  // std::smatch mm;

  // find [original, ip, port]
  // regex_search(ss, mm, ipport);
  // if (mm.size() != 3) return ans; //return std::move(ans);

  else if (m[2] == "*") ip = INADDR_ANY;
  else if (m[2] == "bc") ip = INADDR_BROADCAST;
  else ip = inet_addr(m[2].str().c_str());

  if (m[3] == "*") port = 0;
  else port = stoi(m[3]);

  ans.sin_family      = AF_INET;
  ans.sin_addr.s_addr = ip;
  ans.sin_port        = htons(port);
  // return std::move(ans);
  return ans;
}


template<>
const unixaddr_t filter(const std::string& address) {
  std::regex fmt("(unix)\\:\\/\\/([a-z,A-Z\\d\\/.*_-:]+)");
  std::smatch m;

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

const inetaddr_t inet_sockaddr(const std::string &path) {
  // return std::move(filter<inetaddr_t>(path));
  return filter<inetaddr_t>(path);
}

const unixaddr_t unix_sockaddr(const std::string &path) {
  // return std::move(filter<unixaddr_t>(path));
  return filter<unixaddr_t>(path);
}
