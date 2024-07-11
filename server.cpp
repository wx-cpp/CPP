#include<iostream>
#include<sys/socket.h>	//linux 头文件<sys\socket.h>
#include<unistd.h>
#include<cstring>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string>


using namespace std;

int main(int argc, char* argv[]) {
	int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == -1) {
		cout << "creat socket error" <<  "\n";
	}
	else cout << "ok" << "\n";

	//绑定 port
	string ip="0.0.0.0";
	int port = 5005;
	struct sockaddr_in sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
	sockaddr.sin_port = htons(port);
	if(::bind(sockfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr))<0){
		cout << "creat socket error" << errno << strerror(errno) << "\n";
		return 1;
	}
	else cout << "ok,ip=" << ip.c_str() << ",port=" << port << "\n";

	//监听socket
	if (::listen(sockfd, 1024) < 0) {
		cout << "creat socket error,"  << "\n";
		return 1;
	}
	else cout << "ok" << "\n";

	while (true) {

		//接收客户端连接
		int connfd = ::accept(sockfd, nullptr, nullptr);
		if (connfd < 0) {
			cout << "creat socket error" << "\n";
			return 1;
		}	
		char buf[1024] = { 0 };

		//接收客户端数据
		size_t len = ::recv(connfd, buf, sizeof(buf), 0);
		cout<< connfd<<buf<<"\n";

		//向客户端发送数据
		::send(connfd, buf, len, 0);
	}

	//关闭socket
	::close(sockfd);

	return 0;
}
