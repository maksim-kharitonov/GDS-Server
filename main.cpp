#include "GdsServer.h"
#include "Socket.h"

// добавлены для реализации работы сервера с использованием статических файлов
#include <fstream>
#include <streambuf>

#ifndef _DEFAULT_HTTP_PORT
#define _DEFAULT_HTTP_PORT 8888
#endif  // !_DEFAULT_HTTP_PORT

void RoutingRequest(GdsServer::http_request* r);

int main() { GdsServer(_DEFAULT_HTTP_PORT, RoutingRequest); }

/*
Текущая реализация обработчика http запросов
в модельной реализации выдает фиксированные ответы на запросы поиска,
бронирования, тикетирования и т.д.
*/
void RoutingRequest(GdsServer::http_request* r) {
  Socket s = *(r->s_);

  std::string title;
  std::string body;

  if (r->path_ == "/") {
    std::ifstream response_file("./resources/index.html");
    std::string response_string((std::istreambuf_iterator<char>(response_file)),
                                std::istreambuf_iterator<char>());
    r->answer_ = response_string;
    r->content_type_ = "text/html; charset=utf-8";
  } else if (r->path_ == "/search") {
    std::ifstream response_file("./resources/SearchResponseSirena.xml");
    std::string response_string((std::istreambuf_iterator<char>(response_file)),
                                std::istreambuf_iterator<char>());
    r->answer_ = response_string;
    r->content_type_ = "text/xml";
  } else if (r->path_ == "/hold") {
    std::ifstream response_file("./resources/HoldResponseSirena.xml");
    std::string response_string((std::istreambuf_iterator<char>(response_file)),
                                std::istreambuf_iterator<char>());
    r->answer_ = response_string;
    r->content_type_ = "text/xml";
  } else if (r->path_ == "/book") {
    std::ifstream response_file("./resources/BookResponseSirena.xml");
    std::string response_string((std::istreambuf_iterator<char>(response_file)),
                                std::istreambuf_iterator<char>());
    r->answer_ = response_string;
    r->content_type_ = "text/xml";
  } else if (r->path_ == "/ticket") {
    std::ifstream response_file("./resources/TicketResponseSirena.xml");
    std::string response_string((std::istreambuf_iterator<char>(response_file)),
                                std::istreambuf_iterator<char>());
    r->answer_ = response_string;
    r->content_type_ = "text/xml";
  } else {
    r->status_ = "404 Not Found";
    title = "Invalid URL";
    body = "<h1>Invalid URL</h1>";
    r->content_type_ = "text/html";
  }
}
