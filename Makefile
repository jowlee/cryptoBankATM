#crypto++ should installed into /usr/include/cryptopp as it should be according to http://www.cryptopp.com/wiki/Linux
INC=-L/usr/include/

all:

	# g++ -Wall $(INC) ATMproxy.cpp -o ATMproxy -lpthread -g
	#g++ $(INC) crypto.cpp -lcryptopp -lpthread -c

	g++  $(INC) ATMclient.cpp crypto.cpp sha256.cpp -lcryptopp -lpthread -o ATMclient -g
	g++ $(INC) ATMserver.cpp crypto.cpp sha256.cpp -lcryptopp -lpthread -o ATMserver
	g++ $(INC) ATMproxy.cpp crypto.cpp sha256.cpp -lcryptopp -lpthread -o ATMproxy

	#g++ $(INC) crypto.cpp sha256.cpp -lcryptopp -lpthread -o cryptoTest

	./makeOTP.sh
