#ifndef HTTPSERVER_HTTPSERVER_H
#define HTTPSERVER_HTTPSERVER_H

class HttpServer
{
public:
    explicit HttpServer();
    ~HttpServer();

public:
    void start(const char* ip, int port);

};

void api_index(struct evhttp_request* req, void* arg);
void api_health(struct evhttp_request* req, void* arg);
void api_data(struct evhttp_request* req, void* arg);
void api_upload_image(struct evhttp_request* req, void* arg);

void parse_get(struct evhttp_request* req, struct evkeyvalq* params);
void parse_post(struct evhttp_request* req, char* buff);

#endif //HTTPSERVER_HTTPSERVER_H

