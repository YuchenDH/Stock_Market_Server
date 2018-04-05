#ifndef __REQUEST_HANDLER__
#define __REQUEST_HANDLER__

#include <iostream>
#include <sstream>
#include <pqxx/pqxx>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <string>
#include <set>
#include <unordered_map>
#include <exception>
#include <boost/shared_ptr.hpp>

#define MAX 1;
#define MIN 0;

//#include "create_table.hpp"

namespace pt = boost::property_tree;

//std::unordered_map<std::string, std::mutex> mutex_map();


//std::set<std::string> account_set;

struct _reference_order {
  std::string id;
  int amount;
  float price;
};

typedef struct _reference_order order;

void request_handler(boost::shared_ptr<boost::asio::basic_stream_socket<boost::asio::ip::tcp> > sock, pqxx::basic_connection<pqxx::connect_direct>* C);

boost::shared_ptr<pt::ptree> create_xml_handler(pt::ptree xml, pqxx::connection *C);

void create_worker(pt::ptree command, pqxx::connection *C);

bool order_isValid ( std::string account, std::string amount, std::string limit, pqxx::connection *C);

order fetch_max(std::string table_name, std::string column_name, int max = 1);

void transaction_handler(pt::ptree xml, pqxx::connection *C, std::string account);

void transaction_worker(pt::ptree xml, pqxx::connection *C);

bool account_isExist(std::set<std::string> acccount_set, std::string account_id);

void order_handler(pt::ptree xml, pqxx::connection *C, std::string account);

void cancel_handler(std::string id, pqxx::connection *C, std::string account);

void query_handler(std::string id, pqxx::connection *C);

#endif /*__REQUEST_HANDLER__*/
