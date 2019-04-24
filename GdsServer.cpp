#include <process.h>
#include <ctime>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "GdsServer.h"
#include "Socket.h"
//#include "UrlHelper.h"
//#include "base64.h"

GdsServer::request_func GdsServer::request_func_ = 0;

unsigned GdsServer::Request(void* ptr_s) {
  Socket s = *(reinterpret_cast<Socket*>(ptr_s));

  std::string line = s.ReceiveLine();
  if (line.empty()) {
    return 1;
  }

  http_request req;

  if (line.find("GET") == 0) {
    req.method_ = "GET";
  } else if (line.find("POST") == 0) {
    req.method_ = "POST";
  }

  std::string path;
  std::map<std::string, std::string> params;

  size_t posStartPath = line.find_first_not_of(" ", 3);

  SplitGetReq(line.substr(posStartPath), path, params);

  req.status_ = "202 OK";
  req.s_ = &s;
  req.path_ = path;
  req.params_ = params;

  static const std::string accept = "Accept: ";
  static const std::string accept_language = "Accept-Language: ";
  static const std::string accept_encoding = "Accept-Encoding: ";
  static const std::string user_agent = "User-Agent: ";

  while (1) {
    line = s.ReceiveLine();

    if (line.empty()) break;

    unsigned int pos_cr_lf = line.find_first_of("\x0a\x0d");
    if (pos_cr_lf == 0) break;

    line = line.substr(0, pos_cr_lf);

    if (line.substr(0, accept.size()) == accept) {
      req.accept_ = line.substr(accept.size());
    } else if (line.substr(0, accept_language.size()) == accept_language) {
      req.accept_language_ = line.substr(accept_language.size());
    } else if (line.substr(0, accept_encoding.size()) == accept_encoding) {
      req.accept_encoding_ = line.substr(accept_encoding.size());
    } else if (line.substr(0, user_agent.size()) == user_agent) {
      req.user_agent_ = line.substr(user_agent.size());
    }
  }

  request_func_(&req);

  std::stringstream str_str;
  str_str << req.answer_.size();

  time_t ltime;
  time(&ltime);
  tm* gmt = gmtime(&ltime);

  static std::string const serverName = "Simple GDS Server";

  char* asctime_remove_nl = asctime(gmt);
  asctime_remove_nl[24] = 0;

  s.SendBytes("HTTP/1.1 ");

  s.SendLine(req.status_);

  s.SendLine(std::string("Date: ") + asctime_remove_nl + " GMT");
  s.SendLine(std::string("Server: ") + serverName);
  s.SendLine("Connection: close");
  s.SendLine(std::string("Content-Type: ") + req.content_type_);
  s.SendLine("Content-Length: " + str_str.str() + "\r");
  s.SendLine("\r");
  s.SendLine(req.answer_);

  s.Close();

  return 0;
}

GdsServer::GdsServer(unsigned int port_to_listen, request_func r) {
  SocketServer in(port_to_listen, 5);

  request_func_ = r;

  while (1) {
    Socket* ptr_s = in.Accept();

    unsigned ret;
    _beginthreadex(0, 0, Request, (void*)ptr_s, 0, &ret);
  }
}

void GdsServer::SplitGetReq(std::string get_req, std::string& path,
                 std::map<std::string, std::string>& params) {
  if (get_req[get_req.size() - 1] == '\x0d' ||
      get_req[get_req.size() - 1] == '\x0a')
    get_req = get_req.substr(0, get_req.size() - 1);

  if (get_req[get_req.size() - 1] == '\x0d' ||
      get_req[get_req.size() - 1] == '\x0a')
    get_req = get_req.substr(0, get_req.size() - 1);

  // Remove potential Trailing HTTP/1.x
  if (get_req.size() > 7) {
    if (get_req.substr(get_req.size() - 8, 7) == "HTTP/1.") {
      get_req = get_req.substr(0, get_req.size() - 9);
    }
  }

  std::string::size_type qm = get_req.find("?");
  if (qm != std::string::npos) {
    std::string url_params = get_req.substr(qm + 1);

    path = get_req.substr(0, qm);

    url_params += "&";

    std::string::size_type next_amp = url_params.find("&");

    while (next_amp != std::string::npos) {
      std::string name_value = url_params.substr(0, next_amp);
      url_params = url_params.substr(next_amp + 1);
      next_amp = url_params.find("&");

      std::string::size_type pos_equal = name_value.find("=");

      std::string nam = name_value.substr(0, pos_equal);
      std::string val = name_value.substr(pos_equal + 1);

      std::string::size_type pos_plus;
      while ((pos_plus = val.find("+")) != std::string::npos) {
        val.replace(pos_plus, 1, " ");
      }

      // Replacing %xy notation
      std::string::size_type pos_hex = 0;
      while ((pos_hex = val.find("%", pos_hex)) != std::string::npos) {
        std::stringstream h;
        h << val.substr(pos_hex + 1, 2);
        h << std::hex;

        int i;
        h >> i;

        std::stringstream f;
        f << static_cast<char>(i);
        std::string s;
        f >> s;

        val.replace(pos_hex, 3, s);
        pos_hex++;
      }

      params.insert(std::map<std::string, std::string>::value_type(nam, val));
    }
  } else {
    path = get_req;
  }
}