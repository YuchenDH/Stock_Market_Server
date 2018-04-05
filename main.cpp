#include <iostream>
#include <exception>
#include <pqxx/pqxx>
#include <boost/asio.hpp>

#include "request_handler.hpp"

#define PORT 12345

using boost::asio::ip::tcp

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

int main(int argc, char* argv[]) {
  pqxx::connection *C;
  try {
    C = new pqxx::connection("dbname = testdb user = postgres password = passw0rd \
      hostaddr = 127.0.0.1 port = 5432");
    if (C.is_open()) {
      std::cerr << "Opened database successfully: " << C.dbname() << endl;
    } else {
      std::cerr << "Can't open database" << std::endl;
      return 1;
    }
    C.disconnect();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  boost::asio::io_service io_service;
  boost::asio::ip::tcp::acceptor acceptor(io_service);
  boost::asio::iup::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), PORT);

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
      socket_ptr sock(new ip::tcp::socket(service));
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


    
  
  
