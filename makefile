all:client server server_epoll client_epoll

client:client.cpp
	g++ -std=c++11 -o client client.cpp
server:server.cpp
	g++ -std=c++11 -o server server.cpp
server_epoll:server_epoll.cpp
	g++ -std=c++11 -o server_epoll server_epoll.cpp 
client_epoll:client_epoll.cpp
	g++ -std=c++11 -o client_epoll client_epoll.cpp 

clean:
	rm -f client server server_epoll client_epoll

