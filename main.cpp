#include "GdsServer.h"
#include "Socket.h"
#include "lib\tinyxml.h"

// добавлены для реализации работы сервера с использованием статических файлов
#include <time.h>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <streambuf>

#ifndef _DEFAULT_HTTP_PORT
#define _DEFAULT_HTTP_PORT 8888
#endif  // !_DEFAULT_HTTP_PORT

void RoutingRequest(GdsServer::http_request* r);
std::string randomAlphaUpperString(size_t length);
std::string randomNumericString(size_t length);
std::string randomSearchResponse(std::string& prt1, std::string& prt2,
                                 std::string& date);
std::string randomPnrResponse();
std::string randomTicketResponse(std::string& pnrParam);
std::string getTimeStamp();

int main() { GdsServer(_DEFAULT_HTTP_PORT, RoutingRequest); }

/*
Текущая реализация обработчика http запросов
в модельной реализации выдает фиксированные ответы на запросы поиска,
бронирования, тикетирования и т.д.
*/
void RoutingRequest(GdsServer::http_request* request) {
  Socket socket = *(request->sPtr);

  std::string title;
  std::string body;

  if (request->_path == "/") {
    std::ifstream response_file("./resources/index.html");
    std::string response_string((std::istreambuf_iterator<char>(response_file)),
                                std::istreambuf_iterator<char>());
    request->_answer = response_string;
    request->_content_type = "text/html; charset=utf-8";
  } else if (request->_path == "/search") {
    std::string prt1 = request->params_["prt1"];
    std::string prt2 = request->params_["prt2"];
    std::string date = request->params_["date"];
    request->_answer = randomSearchResponse(prt1, prt2, date);
    request->_content_type = "text/xml";
  } else if (request->_path == "/hold") {
    std::ifstream response_file("./resources/HoldResponseSirena.xml");
    std::string response_string((std::istreambuf_iterator<char>(response_file)),
                                std::istreambuf_iterator<char>());
    request->_answer = response_string;
    request->_content_type = "text/xml";
  } else if (request->_path == "/book") {
    request->_answer = randomPnrResponse();
    request->_content_type = "text/xml";
  } else if (request->_path == "/ticket") {
    std::string pnrParam = request->params_["pnr"];
    request->_answer = randomTicketResponse(pnrParam);
    request->_content_type = "text/xml";
  } else {
    request->_http_status = "404 Not Found";
    title = "Invalid URL";
    body = "<h1>Invalid URL</h1>";
    request->_content_type = "text/html";
  }
}

std::string getTimeStamp() {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S");
  auto str = oss.str();

  return str;
}

std::string randomAlphaUpperString(size_t length) {
  srand(time(NULL));
  auto randchar = []() -> char {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

std::string randomNumericString(size_t length) {
  auto randchar = []() -> char {
    const char charset[] = "0123456789";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

std::string randomPnrResponse() {
  TiXmlDocument doc;
  const char* filePath = "./resources/BookResponseSirena.xml";
  if (doc.LoadFile(filePath)) {
    TiXmlElement* rootElement = doc.RootElement();
    if (NULL != rootElement) {
      TiXmlElement* answer = rootElement->FirstChildElement("answer");
      if (NULL != answer) {
        answer->SetAttribute("time", getTimeStamp().c_str());

        TiXmlElement* pnr =
            rootElement->FirstChildElement()->FirstChildElement("pnr");
        if (NULL != pnr) {
          pnr->Clear();
          std::string randPnr = randomAlphaUpperString(6).c_str();
          pnr->LinkEndChild(new TiXmlText(randPnr.c_str()));
          pnr->SetAttribute("status", "BOOKED");
        }
      }
    }
  }
  doc.SaveFile(filePath);

  std::ifstream response_file(filePath);
  std::string response_string((std::istreambuf_iterator<char>(response_file)),
                              std::istreambuf_iterator<char>());
  return response_string;
}

std::string randomTicketResponse(std::string& pnrParam) {
  TiXmlDocument doc;
  const char* filePath = "./resources/TicketResponseSirena.xml";
  if (doc.LoadFile(filePath)) {
    TiXmlElement* rootElement = doc.RootElement();
    if (NULL != rootElement) {
      TiXmlElement* answer = rootElement->FirstChildElement("answer");
      if (NULL != answer) {
        answer->SetAttribute("time", getTimeStamp().c_str());

        TiXmlElement* pnr =
            rootElement->FirstChildElement()->FirstChildElement("pnr");
        if (NULL != pnr) {
          pnr->Clear();
          pnr->LinkEndChild(new TiXmlText(pnrParam.c_str()));
          pnr->SetAttribute("status", "BOOKED");
        }

        TiXmlElement* tickets =
            rootElement->FirstChildElement()->FirstChildElement("tickets");
        if (NULL != tickets) {
          tickets->Clear();
          srand(time(NULL));
          int tktNum = rand() % 3 + 1;
          for (int i = 0; i < tktNum; i++) {
            TiXmlElement* ticket = new TiXmlElement("ticket");
            std::string randBso = randomNumericString(10);
            ticket->LinkEndChild(new TiXmlText(randBso.c_str()));
            ticket->SetAttribute("status", "OK");
            tickets->LinkEndChild(ticket);
          }
        }
      }
    }
  }
  doc.SaveFile(filePath);

  std::ifstream response_file(filePath);
  std::string response_string((std::istreambuf_iterator<char>(response_file)),
                              std::istreambuf_iterator<char>());
  return response_string;
}

std::string randomSearchResponse(std::string& prt1, std::string& prt2,
                                 std::string& date) {
  TiXmlDocument doc;
  const char* filePath = "./resources/SearchResponseSirena.xml";
  if (doc.LoadFile(filePath)) {
    TiXmlElement* rootElement = doc.RootElement();
    if (NULL != rootElement) {
      TiXmlElement* answer = rootElement->FirstChildElement("answer");
      if (NULL != answer) {
        answer->SetAttribute("time", getTimeStamp().c_str());

        TiXmlElement* variant =
            answer->FirstChildElement()->FirstChildElement("variant");
        while (NULL != variant) {
          TiXmlElement* flight = variant->FirstChildElement("flight");
          while (NULL != flight) {
            TiXmlElement* origin = flight->FirstChildElement("origin");
            if (NULL != origin) {
              origin->Clear();
              origin->LinkEndChild(new TiXmlText(prt1.c_str()));
              origin->SetAttribute("city", prt1.c_str());
            }
            TiXmlElement* destination = flight->FirstChildElement("destination");
            if (NULL != destination) {
              destination->Clear();
              destination->LinkEndChild(new TiXmlText(prt2.c_str()));
              destination->SetAttribute("city", prt2.c_str());
            }
            flight = flight->NextSiblingElement("flight");
          }
          variant = variant->NextSiblingElement("variant");
        }
      }
    }
  }
  doc.SaveFile(filePath);

  std::ifstream response_file(filePath);
  std::string response_string((std::istreambuf_iterator<char>(response_file)),
                              std::istreambuf_iterator<char>());
  return response_string;
}