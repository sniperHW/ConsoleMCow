testnet:
	g++ -c ../CNet.cpp
	g++ -o server server.cpp CNet.o -lws2_32 -std=c++0x
	g++ -o client client.cpp CNet.o -lws2_32 -std=c++0x