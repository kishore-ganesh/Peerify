p2p: server client
server: server.cpp networkops.cpp 
	g++ server.cpp networkops.cpp -std=c++11 -o server -g
client: client.cpp fileops.cpp networkops.cpp util.cpp
	g++ client.cpp networkops.cpp fileops.cpp util.cpp -std=c++11 -pthread -lm -o client -g
