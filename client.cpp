

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;
int main(int argc, char *argv[])
{
	int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		cout << "creat socket error" << "\n";
		return 1;
	}
	else
		cout << "ok" << "\n";

	string ip = "0.0.0.0";
	int port = 5005;
	struct sockaddr_in sockaddr;
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
	sockaddr.sin_port = htons(port);
	if (::connect(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
	{
		cout << "error" << strerror(errno) << "\n";
		return 1;
	}

	string data = "abjshjkas";
	::send(sockfd, data.c_str(), 1024, 0);
	char buf[1024] = {0};
	::recv(sockfd, buf, sizeof(buf), 0);

	cout << buf << "\n";
	::close(sockfd);
	return 0;
}
