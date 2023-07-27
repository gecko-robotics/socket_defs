// https://google.github.io/googletest/primer.html
#include <gtest/gtest.h>
#include <socket.hpp>
#include <string>
#include <iostream>

using namespace std;

struct data_t { int a; };

TEST(socket, debug) {
  EXPECT_TRUE(1);
}

TEST(socket, ascii) {
  Ascii a;
  string s{"hello|9|1234|1.2.3.4"};
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

TEST(socket, unix) {
  string path = "unix://path/to/file.udp";
  unixaddr_t u = unix_sockaddr(path);
  string s = unix2string(u);
  // cout << path << " " << s << endl;
  EXPECT_TRUE(path == ("unix://" + s));

  EXPECT_THROW({
    path = "udp://1.2.3.4:1234";
    u = unix_sockaddr(path);
    s = unix2string(u);
    // cout << path << " " << s << endl;
    EXPECT_TRUE(path == ("unix://" + s));
    },
  std::invalid_argument);
}

TEST(socket, inet) {
  string path = "udp://1.2.3.4:12345";
  inetaddr_t i = inet_sockaddr(path);
  string s = inet2string(i);
  EXPECT_TRUE(path == "udp://" + s);

  EXPECT_THROW({
    path = "tcp://1.2.3.4:1234";
    i = inet_sockaddr(path);
    s = inet2string(i);
    },
  std::invalid_argument);

  // EXPECT_THROW({
  //   path = "udp://1.2.3.4:123456";
  //   i = inet_sockaddr(path);
  //   s = inet2string(i);

  //   cout << path << " " << s << endl;
  //   },
  // // std::invalid_argument);
  // std::regex_error);

  EXPECT_THROW({
    path = "udp://1.2.3.4";
    i = inet_sockaddr(path);
    s = inet2string(i);
    },
  std::invalid_argument);

  // path = "udp://computer.local:1234";
  // i = inet_sockaddr(path);
  // s = inet2string(i);
  // cout << path << " " << s << endl;
  // EXPECT_TRUE(path == "udp://" + s);

  // EXPECT_THROW({
  //   path = "udp://computer.local:1234";
  //   i = inet_sockaddr(path);
  //   s = inet2string(i);
  //   },
  // std::invalid_argument);

  int port = 9999;
  string host = "1.2.3.4";
  string address = "udp://"+host+":"+to_string(port);
  inetaddr_t addr = inet_sockaddr(address);
  EXPECT_EQ(addr.sin_addr.s_addr, inet_addr(host.c_str()));
  EXPECT_EQ(addr.sin_port, htons(port));

  port = 99;
  host = "bc";
  address = "udp://"+host+":"+to_string(port);
  addr = inet_sockaddr(address);
  // cerr << address << " " << host << endl;
  EXPECT_EQ(addr.sin_addr.s_addr, inet_addr("255.255.255.255"));
  EXPECT_EQ(addr.sin_port, htons(port));
}

// struct data_t { int a; };

// data_t test_data[LOOP]{{1},{2},{3},{4},{5}};

// TEST(socket, addresses) {
//   int port = 9999;
//   string host = "1.2.3.4";
//   string address = "udp://"+host+":"+to_string(port);
//   inetaddr_t addr = inet_sockaddr(address);
//   EXPECT_EQ(addr.sin_addr.s_addr, inet_addr(host.c_str()));
//   EXPECT_EQ(addr.sin_port, htons(port));

//   port = 99;
//   host = "bc";
//   address = "udp://"+host+":"+to_string(port);
//   addr = inet_sockaddr(address);
//   // cerr << address << " " << host << endl;
//   EXPECT_EQ(addr.sin_addr.s_addr, inet_addr("255.255.255.255"));
//   EXPECT_EQ(addr.sin_port, htons(port));
// }

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

TEST(socket, message_t) {
  data_t d{5};
  message_t m = pack<data_t>(d);
  data_t e = unpack<data_t>(m);
  EXPECT_EQ(d.a, e.a);
  EXPECT_EQ(sizeof(d), sizeof(e));

  // string s = "alice and bob";
  // m = string2msg(s);
  // string ss = msg2string(m);
  // cout << s << " " << ss << endl;
  // EXPECT_TRUE(s == ss);
}