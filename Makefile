#request:request_handler.cpp request_handler.hpp
#	g++ -std=gnu++11 -lpqxx request_handler.cpp request_handler.hpp -o request -lboost_system


#test:	test.cpp test_server.cpp request_handler.cpp
#	g++ test.cpp -std=gnu++11 -o test -lpthread -lboost_system -lpqxx -lpq
#	g++ test_server.cpp -std=gnu++11 -o test_server -lpthread -lboost_system -lboost_thread -lpqxx -lpq

test:	test.cpp test_server.cpp request_handler.cpp create_table.cpp request_handler.hpp
	g++ -O3 test.cpp -std=gnu++11 -o test -lpthread -lboost_system request_handler.cpp create_table.cpp -lpqxx -lpq
	g++ -O3 test_server.cpp -std=gnu++11 -o test_server -lpthread -lboost_system -lboost_thread request_handler.cpp create_table.cpp -lpqxx -lpq


all:	main.cpp request_handler.cpp
	g++ -O3 -std=gnu++11 -o server -lpqxx -lpq

