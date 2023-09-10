// https://google.github.io/googletest/primer.html
#include <gtest/gtest.h>
#include <socket_defs.hpp>
#include <string>
#include <iostream>
#include <cstdio> // remove
// #include <fmt/format.h>
// #include <format>

using namespace std;

struct data_t { int a; };

// struct Unixaddr_t: sockaddr_un {
//   public:
//   ~Unixaddr_t() {
//     if (strlen(sun_path) > 0)
//       remove(sun_path);
//   }
// };

// TEST(socket, dummy) {

// }


TEST(socket, message_t) {
  data_t d{5};
  message_t m = pack<data_t>(d);
  data_t e = unpack<data_t>(m);
  EXPECT_EQ(d.a, e.a);
  EXPECT_EQ(sizeof(d), sizeof(e));

  // these don't work since string is converted to
  // hex and they don't match!!!
  // "alice ..." != "0x68,0x59, ..."
  string s = "alice and bob";
  // m = string2msg(s);
  m << s;
  // string ss = to_string(m);
  // cout << s << " " << ss << endl;
  // cout << s << " " << format("{:#x}", m[0]);
  // EXPECT_TRUE(s == ss);

  string sss = to_string(m);
  // cout << s << " " << sss << endl;
  // EXPECT_TRUE(s == sss);

  m.clear();
  m << s;
  // cout << "s: " << s << endl;
  // cout << "m: ";
  // for (const uint8_t& c: m) cout << (char)c;
  // cout << endl;
  EXPECT_EQ(m.size(), s.size());
}

// TEST(socket, ascii) {
//   Ascii a;
//   string orig{"bob|tom|alice|george"};
//   // ascii_t msg = a.unpack(orig);
//   // EXPECT_TRUE("bob" == msg[0]);
//   // EXPECT_TRUE("tom" == msg[1]);
//   // EXPECT_TRUE("alice" == msg[2]);
//   // EXPECT_TRUE("george" == msg[3]);
//   // string redo = a.pack(msg);
//   // EXPECT_TRUE(redo == orig);
//   // // cerr << "ascii" << endl;
// }

TEST(socket, ascii) {
  Ascii a;
  string s{"hello 9 1234 1.2.3.4"};
  message_t m = a.encode(s);
  string ss = a.decode(m);

  EXPECT_TRUE(ss == s);
}

TEST(socket, raw) {
  Raw a;
  string s{"hello 9 1234 1.2.3.4"};
  message_t m = a.encode(s);
  string ss = a.decode(m);

  EXPECT_TRUE(ss == s);
}

TEST(socket, json) {
  Json j;
  dict d;
  d["host"] = "uri://1.2.3.4:1234";
  d["format"] = "msg_t";
  d["topic"] = "navigation:vision"; // colons OK
  d["size"] = "125";
  message_t m = j.encode(d);

  dict dd = j.decode(m);
  // cout << dd << endl;

  EXPECT_TRUE(d == dd);
}

TEST(socket, json_fail) {
  Json j;
  dict d;
  d["host"] = "uri://1.2.3.4:1234";
  d["format"] = "msg\"_t"; // bad, but survives
  d["topic"] = "navigation:vision,"; // comma
  d["size"] = "1,25"; // comma
  message_t m = j.encode(d);

  dict dd = j.decode(m);
  // cout << dd << endl;
  EXPECT_FALSE(d == dd);

  d.clear();
  m.clear();
  dd.clear();
  d["host"] = "uri://1.2.3.4:1234";
  d["format"] = "msg\"_t"; // bad
  d["topic"] = "navigation:vision";
  d["size"] = "1\"25"; // bad
  m = j.encode(d);

  dd = j.decode(m);
  // cout << dd << endl;

  EXPECT_FALSE(d == dd);
}

TEST(socket, unix_valid) {
  vector<string> unix{
    "unix:///path/to/file.uds",
    "unix://hello-there.uds",
    "unix://./unix.ps.uds"};

  for (const string& uds: unix) {
    EXPECT_NO_THROW(
      unixaddr_t u = unix_sockaddr(uds)
    );
  }
}

TEST(socket, unix_random) {
  EXPECT_NO_THROW(
    unixaddr_t u = unix_sockaddr()
  );
}

TEST(socket, unix_invalide) {
  string path = "unix://alice*"; // value?
  unixaddr_t u = unix_sockaddr(path);
  string s = to_string(u);
  // cout << path << " " << s << endl;
  EXPECT_FALSE(path == ("unix://" + s));
}

TEST(socket, inet_valid) {
  int port = 9999;
  string host = "1.2.3.4";
  string path = "udp://"+host+":"+to_string(port);
  inetaddr_t addr = inet_sockaddr(path);
  string s = to_string(addr);
  EXPECT_TRUE(path == "udp://" + s);
  EXPECT_EQ(addr.sin_addr.s_addr, inet_addr(host.c_str()));
  EXPECT_EQ(addr.sin_port, htons(port));


  vector<string> udp{
    "udp://1.2.3.4:12345",
    "udp://www.google.com:2345",
    "udp://computer.local:54321"
  };

  for (const string& inet: udp) {
    EXPECT_NO_THROW(inet_sockaddr(inet));
  }
}

TEST(socket, inet_broadcast) {
  int port = 99;
  string address = "udp://bc:"+to_string(port);
  inetaddr_t addr = inet_sockaddr(address);
  // cerr << address << " " << host << endl;
  EXPECT_EQ(addr.sin_addr.s_addr, inet_addr("255.255.255.255"));
  EXPECT_EQ(addr.sin_port, htons(port));
}

TEST(socket, inet_invalid) {
  EXPECT_THROW(inet_sockaddr("tcp://1.2.3.4:1234"), std::invalid_argument);
  EXPECT_THROW(inet_sockaddr("udp://1.2.3.4"), std::invalid_argument);
}




