#crypto++ should installed into /usr/include/cryptopp as it should be according to http://www.cryptopp.com/wiki/Linux
INC=-L/usr/include/

all:

	g++  $(INC) ATMclient.cpp crypto.cpp sha256.cpp  -lpthread -o ATMclient -g -w
	g++ $(INC) ATMserver.cpp crypto.cpp sha256.cpp -lpthread -o ATMserver -w
	g++ $(INC) ATMproxy.cpp crypto.cpp sha256.cpp -lpthread -o ATMproxy -w


	./makeOTP.sh
