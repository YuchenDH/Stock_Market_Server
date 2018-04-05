#include <iostream>
#include <boost/asio.hpp>
#include "request_handler.hpp"

int main (int argc, char* argv[]) {
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::endpoint ep( boost::asio::ip::address::from_string("127.0.0.1"), 12345);
  boost::asio::ip::tcp::socket sock(io_service);
  sock.connect(ep);
  
  boost::asio::streambuf b;
  std::ostream os(&b);
  os << "173\n";
  os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  os << "<create>\n";
  os << " <account id=\"123456\" balance=\"1000\"/>\n";
  os << " <symbol sym=\"SPY\">\n";
  os << "  <account id=\"123456\">100000</account>\n";
  os << " </symbol>\n";
  os << "</create>\n\n";

  // try sending some data in input sequence
  size_t n = sock.send(b.data());
  //std::cout << n << " bytes sent\n";
  b.consume(n); // sent data is removed from input sequence
  return 0;
}
