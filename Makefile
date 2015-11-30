#crypto++ should installed into /usr/include/cryptopp as it should be according to http://www.cryptopp.com/wiki/Linux
INC=-L/usr/include/

all:
	g++  $(INC) ATMclient.cpp -lcryptopp -lpthread -o ATMclient -g
	# g++ -Wall $(INC) ATMproxy.cpp -o ATMproxy -lpthread -g
	g++ $(INC) ATMserver.cpp -lcryptopp -lpthread -o ATMserver
	g++ $(INC) ATMproxy.cpp -lcryptopp -lpthread -o ATMproxy
