#include "request_handler.hpp"
#include "create_table.hpp"

namespace pt = boost::property_tree;



void request_handler(boost::shared_ptr<boost::asio::basic_stream_socket<boost::asio::ip::tcp> > sock,\
		     pqxx::basic_connection<pqxx::connect_direct>* C) {
  //read into buffer
  boost::asio::streambuf b;

  // reserve 512 bytes in output sequence
  boost::asio::streambuf::mutable_buffers_type bufs = b.prepare(512);

  size_t len = sock->receive(bufs);
  std::cout << "received bytes: " << len << std::endl;
  if (len == 0) {
    std::cerr << "Error: Socket is not readable, returning now...\n";
    return;
  }

  // received data is "committed" from output sequence to input sequence
  b.commit(len);

  std::istream is(&b);
  std::size_t xml_len;
  is >> xml_len;
  try{
    while ( len < xml_len ) {
      std::size_t newlen = sock->receive(bufs);
      if (newlen == 0) {
	std::cerr << "Error: Socket error before complete xml received\n";
	return;
      }
      len += newlen;
    }
  } catch (boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> > &e) {
    return;
  }

  std::istream rest(&b);
  
  //consume the \n left by first line.
  b.consume(1);
  
  //std::cout << rest.rdbuf() << std::endl;
  //std::stringstream ss;
  //ss << rest.rdbuf();
  //std::string xml_str = ss.str();
  //std::cout << "xml_str in string\n" << xml_str;
  //parse into xml
  pt::ptree xml_tree;
  try {
    std::cout << "begin to parse" << std::endl;
    pt::xml_parser::read_xml(rest, xml_tree);
    std::cout << "parsed in tree" << std::endl;
    pt::write_xml(std::cout,xml_tree);
 } catch (pt::xml_parser::xml_parser_error &e) {
    std::cerr << e.what() << std::endl;
    return;
  }
  
  //is it illegal?
  //return (and log it)
  
  //is it create?
  std::cout << "then call create\n" << std::endl;
  BOOST_FOREACH( pt::ptree::value_type const & xml_child, xml_tree.get_child("")) {
    //for ( pt::ptree::const_iterator it = xml_tree.begin(); it != xml_tree.end(); ++it) {
    std::string xml_child_name = xml_child.first;
    std::cout << xml_child_name << std::endl;
    if ( xml_child_name == "create" ) {
      pt::write_xml(std::cout,xml_child.second);
      create_xml_handler(xml_child.second,C);
      //std::cout <<"go to create_handler" << std::endl;
    }
    //give it to create_handler();

  //is it transaction?
    else if ( xml_child_name == "transactions" ) {
      std::string account_trans = xml_child.second.get<std::string>("<xmlattr>.id");
      //checl if account_trans is valid

      //if valid,give it to transaction_handler();
      transaction_handler(xml_child.second,C,account_trans);
    }
  }
  return;
}

/*
bool account_isExist ( std::set<std::string> account_set, std::string account_id ) {
  std::pair<std::set<std::string>::iterator,bool> ret;
  ret = account_set.insert( account_id );
  //if exist, it return false.
  return ret.second;
}
*/



boost::shared_ptr<pt::ptree> create_xml_handler(pt::ptree xml, pqxx::connection *C) {
  //split commands
  std::cout << "\n in the create_handler" << std::endl;
  boost::shared_ptr<pt::ptree> results_tree(new pt::ptree);
  BOOST_FOREACH( pt::ptree::value_type const & child_tree, xml.get_child( "" )) {
    std::string child_tree_name = child_tree.first;
    std::cout << child_tree_name << std::endl;
    if ( child_tree_name == "account" ) {
      try{
	std::string account_id = child_tree.second.get<std::string>("<xmlattr>.id");
	std::cout << "accout_id: " << account_id << std::endl;
	std::string balance = child_tree.second.get<std::string>("<xmlattr>.balance");
	std::cout << "balance " << balance << std::endl;
	/*
	std::string sql = " INSERT INTO ACCOUNT ( ID, BALANCE )"	\
	  "VALUES ( " + account_id + ", " + balance + " )"		\
	  "ON CONFLICT (ID) DO NOTHING";
	pqxx::work W(*C);
	W.exec( sql );
	W.commit();
	
	//try to find whether the account is already exists
	//if ( !account_isExist( account_set,account_id ) ) {
	  pt::ptree error;
	  error.put( "error.<xmlattr>.id", account_id );
	  error.put( "error", "Account alreadu exists" );
	  results_tree->add_child( "results.error",error );
	  }*/
	//add child to the results
	pt::ptree created;
	created.put( "created.<xmlattr>.id",account_id );
	results_tree->add_child( "resultes.created",created );
      }
      catch ( std::exception & e) {
	std::cerr << e.what() << std::endl;
	return results_tree;
      }
      //return with results
    }
    else if ( child_tree_name == "symbol" ) {
      BOOST_FOREACH( pt::ptree::value_type const & child_sym, xml.get_child( "symbol")) {
	std::string child_sym_name = child_sym.first;
	if ( child_sym_name == "account" ) {
	  //get_optional could not throw exception
	  try{
	    std::string account_id = child_sym.second.get<std::string>("<xmlattr>.id");
	    std::cout << "symbol account: " << account_id << std::endl;
	    std::string sym_num = child_sym.second.get<std::string>("");
	    std::cout << "symbol num: " << sym_num << std::endl;
	    //check whether this symbol already exist
	    /*
	    if ( !table_isExist(C,child_sym_name)) {
	      create_symbol(C,child_sym_name);
	    }
	    std::string sql = " INSERT INTO " + child_sym_name + " ( ID, AMOUNT )" \
	      "VALUES ( " + account_id + ", " + sym_num + " );";
	    pqxx::work W(*C);
	    W.exec( sql );
	    W.commit();
	    */
	    // return with results
	    pt::ptree created;
	    created.put( "created.<xmlattr>.symbol",child_sym_name );
	    results_tree->add_child( "resultes.created",created );
	  }
	  catch ( std::exception & e) {
	    std::cerr << e.what() << std::endl;
	    return results_tree;
	  }
	}
      }//symbol
    }//for each child in sym
  }//for each child in create
  //pt::write_xml(std::cout,*results_tree);
  return results_tree;
}

bool order_isValid ( std::string account, std::string amount, std::string limit, pqxx::connection *C) {
  return true;
}

order fetch_max(std::string table_name, std::string column_name, int max) {
  return order({"1",1,1});
}

void order_handler(pt::ptree xml, pqxx::connection *C,std::string account){
  std::string order_sym = xml.get<std::string>("<xmlattr>.sym");
  std::string order_amount = xml.get<std::string>("<xmlattr>.amount");
  std::string order_limit = xml.get<std::string>("<xmlattr>.limit");
  //check whether the order is valid
  //1. amount <0 (sell): sym_amount >= amount
  //2. amount >0 (buy): account_balance >= amount * limit
  int orderID;
  //padded for test compile
  orderID = 0;
  if ( order_isValid (account, order_amount, order_limit, C) ) {
    //insert into database order
    
  }
  
  //if order is not valid:
  pt::ptree error;
  error.put("error.<xmlattr>.sym", order_sym);
  error.put("error.<xmlattr>.amount", order_amount);
  error.put("error.<xmlattr>.limit", order_limit);
  //put error massage according to the situation;

  
  //if order valid:
  //find the match:make the transaction

  //save to excuted database

  
  //not match, save to the database
  std::string sql = " INSERT INTO ORDER ( ORDER_ID, SYMBOL, AMOUNT, LIMIT )"		\
    "VALUES ( DEFAULT " + order_sym + ", " + order_amount + ", " + order_limit + " ) "\
    "RETURNING OREDER_ID;";
  pqxx::work W(*C);
  //pqxx::results r = W.exec( sql );
  W.commit();
  
  pt::ptree opened;
  opened.put("opened.<xmlattr>.id", orderID);
  opened.put("opened.<xmlattr>.sym", order_sym);
  opened.put("opened.<xmlattr>.amount", order_amount);
  opened.put("opened.<xmlattr>.limit", order_limit);

  return;
}

/*
void cancel_handler(std::string id, pqxx:connection *C, std::string account) {
  std::string order_sym;
  std::string order_amount;
  std::string order_limit;
  std::string sql = "SELECT * FROM OPENED WHERE OPENED_ID = " + id " ;";
  pqxx::work W(*C);
  pqxx::result R(W.exec( sql ));
  for ( pqxx::result::const_iterator c = R.begin(); c!= R.end(); ++c) {
    order_sym = c[1].as<std:string>();
    order_amount = c[2].as<std:string>();
    order_limit = c[3].as<std:string>();
  }

  //do the transaction

  //save to database ??? time
  std::string sql = " INSERT INTO CANCELED ( OPENED_ID, SYMBOL, AMOUNT)" \
    "VALUES ( " + id  + ", " + order_sym + ", " + order_amount + "); ";
  pqxx::work W(*C);
  W.exec( sql );
  W.commit();
  
  //reply
  pt::ptree canceled;
  canceld.put("canceled.<xmlattr>.amount", order_amount);
  canceled.put("canceled.<xmlattr>.time", time);

  return;

}


void query_handler(std::string id, pqxx::connection *C) {
  pt::ptree status;
  status.put("status.<xmlattr>.id",id);
 
  //for open
  std::string sql = "SELECT AMOUNT FROM OPENED WHERE OPENED_ID = " + id " ;";
  pqxx::work W(*C);
  pqxx::result r = W.exec( sql );
  std::string opend_shares = r.begin()[0].as<std::string>();
  pt::ptree open;
  open.put("open.<xmlattr>.shares", open_shares);
  status.add_child("status.open",open);
  
  //for cancel
  std::string canceled_shares;
  std::string canceled_time;
  std::string sql = "SELECT AMOUNT,TIME, FROM CANCELED WHERE CANCELED_ID = " + id " ;";
  pqxx::work W(*C);
  pqxx::result r = W.exec( sql );
  for ( pqxx::result::const_iterator c = R.begin(); c!= R.end(); ++c) {
    canceled_shares = c[0].as<std:string>();
    canceled_time = c[1].as<std:string>();
  }
  pt::ptree canceled;
  canceled.put("canceled.<xmlattr>.shares", canceled_shares); 
  canceled.put("canceled.<xmlattr>.time", canceled_time); 
  status.add_child("status.canceled",canceled);

  //for excuted
  std::string excuted_shares;
  std::string excuted_time;
  std::string excuted_price;
  std::string sql = "SELECT AMOUNT,PRICE, TIME, FROM EXCUTED WHERE EXCUTED_ID = " + id " ;";
  pqxx::work W(*C);
  pqxx::result r = W.exec( sql );
  for ( pqxx::result::const_iterator c = R.begin(); c!= R.end(); ++c) {
    excuted_shares = c[0].as<std:string>();
    excuted_price = c[1].as<std:string>();
    excuted_time = c[2].as<std:string>();
    
  }
  pt::ptree canceled;
  excuted.put("excuted.<xmlattr>.shares", excuted_shares);
  excuted.put("excuted.<xmlattr>.price", excuted_price); 
  excuted.put("excuted.<xmlattr>.time", excuted_time); 
  status.add_child("status.excuted",excuted);

  return;
}
*/

void transaction_handler(pt::ptree xml, pqxx::connection *C, std::string account) {
  std::cout << "------------in the transaction_handler-------" << std::endl;
  BOOST_FOREACH ( pt::ptree::value_type const & child_tree, xml.get_child("")) {
    std::string child_tree_name = child_tree.first;
    std::cout << child_tree_name << std::endl;
    if ( child_tree_name == "order" ) {
      //std::string order_sym = child_tree.second.get<std::string>("<xmlattr>.sym");
      //std::string amount = child_tree.second.get<std::string>("<xmlattr>.amount");
      //std::string limit = child_tree.second.get<std::string>("<xmlattr>.limit");
      //give it to order_hander
      order_handler(child_tree.second,C,account);
    }
    /*
    else if ( child_tree_name = "cancel" ) {
      std::string order_id = child_tree.second.get<std::string>("<xmlsttr>".id);
      //if id is valid
      cancel_handler(order_id,C,account);
      //if not handle this
      
    }
    else if ( child_tree_name = "query" ) {
      std::string order_id = child_tree.second.get<std::string>("<xmlsttr>".id);
      //if id is valid
      query_handler(order_id,C);
      //if not handle this
      
    }
    else {
      //invalid child name,handle this
      
    }
    */
  return;
  }
}



  
void transaction_worker(pt::ptree xml, pqxx::connection *C) {
  return;
}
