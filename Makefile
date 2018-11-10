p2p: server client
server: server.cpp 
	g++ server.cpp -std=c++11 -o server -g
client: client.cpp main.cpp
	g++ client.cpp main.cpp  -lm -o client -g
