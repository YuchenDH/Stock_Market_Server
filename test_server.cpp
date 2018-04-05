#include <iostream>
#include <exception>
#include <pqxx/pqxx>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "request_handler.hpp"

#define PORT 12345

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;

int main(int argc, char* argv[]) {
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::acceptor acceptor(io_service);
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), PORT);
  pqxx::connection * C = NULL;
  try {
    acceptor.open(endpoint.protocol());
    acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.bind(endpoint);
    acceptor.listen();
  } catch (std::exception &e) {
    std::cerr << "Error: Create local socket error!\n";
    std::cerr << e.what() << std::endl;
    return 1;
  } //try

  try {
    while (true) {
      socket_ptr sock(new boost::asio::ip::tcp::socket(io_service));
      acceptor.accept(*sock);
      boost::thread(boost::bind(request_handler, sock, C));
    }
  } catch (std::exception &e) {
    std::cerr << "Error: Accept or Thread error\n";
    std::cerr << e.what() << std::endl;
    return 1;
  } //try
  
  return 0;
}


    
  
  
