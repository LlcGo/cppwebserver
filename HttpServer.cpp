#include "HttpServer.h"
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/http_struct.h>
#include <opencv2/opencv.hpp>

#include <json/json.h>
#include <json/value.h>
#include "Utils/Log.h"
#include "Utils/Common.h"
#include "Utils/Base64.h"
#pragma comment(lib, "ws2_32.lib")

#define RECV_BUF_MAX_SIZE 1024*1024*1
char    tempBuf[RECV_BUF_MAX_SIZE];

HttpServer::HttpServer() {

    WSADATA wdSockMsg;
    int s = WSAStartup(MAKEWORD(2, 2), &wdSockMsg);

    if (0 != s)
    {
        switch (s)
        {
        case WSASYSNOTREADY: printf("重启电脑，或者检查网络库");   break;
        case WSAVERNOTSUPPORTED: printf("请更新网络库");  break;
        case WSAEINPROGRESS: printf("请重新启动");  break;
        case WSAEPROCLIM:  printf("请关闭不必要的软件，以确保有足够的网络资源"); break;
        }
    }

    if (2 != HIBYTE(wdSockMsg.wVersion) || 2 != LOBYTE(wdSockMsg.wVersion))
    {
        LOGE("网络库版本错误");
        return;
    }

}
HttpServer::~HttpServer() {
    LOGE("");
    WSACleanup();
}

void HttpServer::start(const char* ip, int port) {

    event_config* evt_config = event_config_new();
    struct event_base* base = event_base_new_with_config(evt_config);
    struct evhttp* http = evhttp_new(base);
    evhttp_set_default_content_type(http, "text/html; charset=utf-8");

    evhttp_set_timeout(http, 30);
    // 设置路由
    evhttp_set_cb(http, "/", api_index, this);
    evhttp_set_cb(http, "/api/health", api_health, this);
    evhttp_set_cb(http, "/api/data", api_data, this);
    evhttp_set_cb(http, "/api/upload_image", api_upload_image, this);

    evhttp_bind_socket(http, ip, port);
    event_base_dispatch(base);

    event_base_free(base);
    evhttp_free(http);
    event_config_free(evt_config);

}

void api_index(struct evhttp_request* req, void* arg) {
    LOGI("");
    Json::Value result_urls;
    result_urls["/api"] = "api";
    result_urls["/api/health"] = "check health";
    result_urls["/api/data"] = "data";
    result_urls["/api/upload_image"] = "upload_image";
    
    
    Json::Value result;
    result["urls"] = result_urls;

    struct evbuffer* buff = evbuffer_new();
    evbuffer_add_printf(buff, "%s", result.toStyledString().c_str());
    evhttp_send_reply(req, HTTP_OK, nullptr, buff);
    evbuffer_free(buff);

}
void api_health(struct evhttp_request* req, void* arg) {
    int result_code = 0;
    std::string result_msg = "error";

    // 健康检测
    result_code = 1000;
    result_msg = "current service health";


    Json::Value result;
    result["msg"] = result_msg;
    result["code"] = result_code;

    struct evbuffer* buff = evbuffer_new();
    evbuffer_add_printf(buff, "%s", result.toStyledString().c_str());
    evhttp_send_reply(req, HTTP_OK, nullptr, buff);
    evbuffer_free(buff);

}
void api_data(struct evhttp_request* req, void* arg) {
    LOGI("");

    HttpServer* server = (HttpServer*)arg;

    parse_post(req, tempBuf);

    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    Json::Value root;
    JSONCPP_STRING errs;


    Json::Value result_data;
    Json::Value result_data_item;
    int result_code = 0;
    std::string result_msg = "error";
    Json::Value result;

    if (reader->parse(tempBuf, tempBuf + std::strlen(tempBuf), &root, &errs) && errs.empty()) {

        for (int i = 0; i < 100; i++)
        {
            result_data_item["name"] = "name-"+std::to_string(i);
            result_data_item["seq"] = i;
            result_data_item["CurTimestamp"] = getCurTimestamp();

            result_data.append(result_data_item);
        }
        result["data"] = result_data;
        result_code = 1000;
        result_msg = "success";


    }
    else {
        result_msg = "invalid request parameter";
    }
    result["msg"] = result_msg;
    result["code"] = result_code;

    //LOGI("\n \t request:%s \n \t response:%s", root.toStyledString().data(), result.toStyledString().data());


    struct evbuffer* buff = evbuffer_new();
    evbuffer_add_printf(buff, "%s", result.toStyledString().c_str());
    evhttp_send_reply(req, HTTP_OK, nullptr, buff);
    evbuffer_free(buff);

}
void api_upload_image(struct evhttp_request* req, void* arg) {
    LOGI("");

    parse_post(req, tempBuf);

    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    Json::Value root;
    JSONCPP_STRING errs;

    int result_code = 0;
    std::string result_msg = "error";


    if (reader->parse(tempBuf, tempBuf + std::strlen(tempBuf), &root, &errs) && errs.empty()) {

        if (root["code"].isString()) {
            std::string code = root["code"].asCString();

            LOGI("code=%s",code.data());
        }
        if (root["image"].isString()) {

            std::string filename = "image-" + std::to_string(getCurTime())+".jpg";
            LOGI("image=%s", filename.data());

            // opencv::Mat -> imageBase64
            //cv::Mat image = cv::imread("D:\\Project\\bxc\\BXC_CppCallPython\\x64\\Release\\test.jpg");
            //std::string imageBase64;
            //std::vector<int> quality = { 100 };
            //std::vector<uchar> jpeg_data;
            //cv::imencode(".jpg", image, jpeg_data, quality);
            //BXC::Base64Encode(jpeg_data.data(), jpeg_data.size(), imageBase64);

            // imageBase64 -> opencv::Mat
            std::string imageBase64 = root["image"].asString();
            std::string imageStr;
            BXC::Base64Decode(imageBase64, imageStr);
            std::vector<char> imageBuf(imageStr.begin(), imageStr.end());
            cv::Mat image = cv::imdecode(imageBuf, CV_LOAD_IMAGE_COLOR);

            cv::imwrite(filename, image);

        }

        result_code = 1000;
        result_msg = "success";
    }
    else {
        result_msg = "invalid request parameter";
    }

    Json::Value result;
    result["msg"] = result_msg;
    result["code"] = result_code;


    struct evbuffer* buff = evbuffer_new();
    evbuffer_add_printf(buff, "%s", result.toStyledString().c_str());
    evhttp_send_reply(req, HTTP_OK, nullptr, buff);
    evbuffer_free(buff);


}


void parse_get(struct evhttp_request* req, struct evkeyvalq* params) {
    if (req == nullptr) {
        return;
    }
    const char* url = evhttp_request_get_uri(req);
    if (url == nullptr) {
        return;
    }
    struct evhttp_uri* decoded = evhttp_uri_parse(url);
    if (!decoded) {
        return;
    }
    const char* path = evhttp_uri_get_path(decoded);
    if (path == nullptr) {
        path = "/";
    }
    char* query = (char*)evhttp_uri_get_query(decoded);
    if (query == nullptr) {
        return;
    }
    evhttp_parse_query_str(query, params);
}
void parse_post(struct evhttp_request* req, char* buf) {
    size_t post_size = 0;

    post_size = evbuffer_get_length(req->input_buffer);
    if (post_size <= 0) {
        //        printf("====line:%d,post msg is empty!\n",__LINE__);
        return;

    }
    else {
        size_t copy_len = post_size > RECV_BUF_MAX_SIZE ? RECV_BUF_MAX_SIZE : post_size;
        //        printf("====line:%d,post len:%d, copy_len:%d\n",__LINE__,post_size,copy_len);
        memcpy(buf, evbuffer_pullup(req->input_buffer, -1), copy_len);
        buf[post_size] = '\0';
        //        printf("====line:%d,post msg:%s\n",__LINE__,buf);
    }

}