#include "create_table.hpp"

//pqxx::connection *C;

//Add a init_db, which calls connect, isEmpty, and 
//  create all the tables if needed.
int init_db (pqxx::connection *C) {
  try {
    connect_db(C);
    //if (db_isEmpty == 1) {
      create_account(C);
      //create_symbol(C);
      create_order(C);
      //create_sym_buy(C);
      //create_sym_canceled(C);
      //create_sym_executed(C);
      return 0;
      //} else if (db_isEmpty == 0) {
      //return 0;
      //} else {
      //std::cerr << "Error: incompleted legacy data found in db\n";
      //return 1;
      //}
  } catch (std::exception & e) {
    std::cerr << e.what() << std::endl;
    throw;
  }
}

//Add a function to check whether the database is empty 
//  (e.g. created for the first time)
//Note: it should return -1 if part of db is init while
//  some part is missing
//int _isEmpty (pqxx::connection * C) {}
int table_isExist ( pqxx::connection * C , std::string name) {
  std::string sql = "SELECT COUNT(*) FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'schema_name' AND TABLE_NAME = \'" + name + " \';";
  pqxx::work W(*C);
  pqxx::result r = W.exec(sql);
  pqxx::result::const_iterator row = r.begin();
  return row[0].as<int>();
}



int connect_db (pqxx::connection * C) {
  try{
    C = new pqxx::connection("dbname = STOCK_MARKET user = postgres password = passw0rd");
    if (C->is_open()) {
      std::cout << "Opened database successfully" << std::endl;
    }
    else {
      std::cout << " Can not open database" << std::endl;
      return 1;
    }
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    throw;
    //Note: do not catch std::exception and not throw it. Could
    //  cover serious problems
    //return 1;
  }
  return 0;
  //return EXIT_SUCCESS;
}


void create_account ( pqxx::connection * C ){
  std::string sql = "CREATE TABLE ACCOUNT("		\
    "ID          INT PRIMARY KEY   NOT NULL,"		\
    "BALANCE     DOUBLE PRECISION  NOT NULL );";
  pqxx::work W(*C);
  W.exec (sql);
  W.commit();
}


void create_symbol( pqxx::connection *C , std::string sym_name ) {
  std::string sql = "CREATE TABLE " + sym_name + " ("		\
    "ACCOUNT_ID  INT  references ACCOUNT(ID)  NOT NULL,"	\
    "AMOUNT      INT  NOT NULL);";
  pqxx::work W(*C);
  W.exec (sql);
  W.commit();
}


void create_order( pqxx::connection *C){
   std::string sql = "CREATE TABLE ORDER("	\
     "ORDER_ID    INT PRIMARY KEY  NOT NULL,"	\
    "SYMBOL      TEXT  NOT NULL,"		\
    "AMOUNT      INT   NOT NULL,"		\
    "LIMIT       MONEY NOT NULL);";
   pqxx::work W(*C);
   W.exec (sql);
   W.commit();
}


void create_sym_buy( pqxx::connection * C, std::string sym_name ) {
  std::string sql = "CREATE TABLE " + sym_name + "_BUY ("		\
    "BUY_ID  INT    PRIMARY KEY  NOT NULL,"				\
    "ORDERNUM    INT    references ORDER(ORDER_ID) NOT NULL,"		\
    "AMOUNT      INT    NOT NULL,"					\
    "LIMIT       MONEY  NOT NULL );";
   pqxx::work W(*C);
   W.exec (sql);
   W.commit();
}


void create_sym_sell( pqxx::connection * C, std::string sym_name ) {
  std::string sql = "CREATE TABLE " + sym_name + "_SELL ("	   \
    "SELL_ID  INT    PRIMARY KEY  NOT NULL,"		           \
    "ORDERNUM    INT    references ORDER(ORDER_ID) NOT NULL,"      \
    "AMOUNT      INT    NOT NULL,"		                   \
    "LIMIT       MONEY  NOT NULL );";
   pqxx::work W(*C);
   W.exec (sql);
   W.commit();
}


void create_sym_canceled ( pqxx::connection * C, std::string sym_name ) {
  std::string sql = "CREATE TABLE " + sym_name + "_CANCELED("		\
    "CANCELED_ID   INT   PRIMARY KEY NOT NULL,"				\
    "ORDERNUM      INT   references OPENED(OPENED_ID) NOT NULL,"	\
    "AMMOUNT       INT   NOT NULL,"					\
    "PRICE         MONEY NOT NULL,"					\
    "TIME          TIMESTAMP  NOT NULL);";
  pqxx::work W(*C);
  W.exec (sql);
  W.commit();
}

void create_sym_executed ( pqxx::connection * C, std::string sym_name ) {
  std::string sql = "CREATE TABLE " + sym_name + "_EXCUTED("		\
    "EXCUTED_ID   INT  references OPENED(OPENED_ID)  NOT NULL"		\
    "AMOUNT       INT   NOT NULL,"					\
    "PRICE        MONEY      NOT NULL,"					\
    "TIME         TIMESTAMP  NOT NULL);"; 
  pqxx::work W(*C);
  W.exec (sql);
  W.commit();
}

//sql =  "CREATE TABLE OPEN("					       \
  // "OPENED_ID   INT PRIMARY KEY references OPENED(OPENED_ID) NOT NULL;" \
     //"AMOUNT      INT  NOT NULL);";


