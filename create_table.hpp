#ifndef __CREATE_TABLE_HPP__
#define __CREATE_TABLE_HPP__

#include <string>
#include <pqxx/pqxx>
#include <iostream>

int init_db (pqxx::connection *C);

//int db_isEmpty (pqxx::connection * C);
int table_isExist ( pqxx::connection *C, std::string name);

int connect_db (pqxx::connection *C);

void create_account ( pqxx::connection *C );

void create_symbol( pqxx::connection *C, std::string sym_name );

void create_order ( pqxx::connection *C );

void create_sym_buy( pqxx::connection *C, std::string sym_name );

void create_sym_sell( pqxx::connection *C, std::string sym_name );

void create_sym_canceled ( pqxx::connection *C, std::string sym_name );

void create_sym_executed ( pqxx::connection *C, std::string sym_name );


#endif
