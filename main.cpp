#include "HttpServer.h"
#include "Utils/Log.h"
#include <httplib.h>
#include <iostream>

int tcpClient()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cout << "WSAStartup 失败，错误码: " << result << std::endl;
        return -1;
    }
    std::cout << "✅ Winsock 初始化成功" << std::endl;

    int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    // unsigned long ul = 1;
    // int ret = ioctlsocket(sockfd, FIONBIO, (unsigned long*)&ul);


    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.4.1", &addr.sin_addr);
    //    addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8080);
    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(sockaddr_in)) == -1)
    {
        std::cout << "connet error" << std::endl;
        return -1;
    }

    while (1)
    {
        char* message = (char*)malloc(100);
        std::cout << "输入数据: ";
        std::cin >> message;
        int len = strlen(message);
        std::cout << "您输入的数据是:" << message << "  数据长度为:" << strlen(message) << std::endl;

        message[len] = '\r';;
        message[len + 1] = '\n';
        message[len + 2] = '\0';
        send(sockfd, message, len + 2, 0);

        free(message);
    }

}

int main(int argc, char** argv)
{
	httplib::Server server;

	server.Get("/hi", [](const httplib::Request& req, httplib::Response &resp) {
		resp.set_content(R"({"message":"hello","code":0})", "application/json");
	});

	server.listen("localhost",1234);
}