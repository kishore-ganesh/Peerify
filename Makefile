p2p: server client
server: server.cpp networkops.cpp 
	g++ server.cpp networkops.cpp -std=c++11 -o server -g
client: client.cpp main.cpp networkops.cpp
	g++ client.cpp networkops.cpp main.cpp -std=c++11 -pthread -lm -o client -g
