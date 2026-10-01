#include "HttpServer.h"
#include "Utils/Log.h"
#include <httplib.h>
#include <iostream>

int main(int argc, char** argv)
{
	httplib::Server server;

	server.Get("/hi", [](const httplib::Request& req, httplib::Response &resp) {
		resp.set_content(R"({"message":"hello","code":0})", "application/json");
	});

	server.listen("localhost",1234);
}