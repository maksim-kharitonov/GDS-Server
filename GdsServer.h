#include <map>
#include <string>

class Socket;

class GdsServer {
 public:
  struct http_request {
    http_request() {}

    Socket* s_;
    std::string method_;
    std::string path_;
    std::map<std::string, std::string> params_;

    std::string accept_;
    std::string accept_language_;
    std::string accept_encoding_;
    std::string user_agent_;

    std::string content_type_;
    std::string status_;
    std::string answer_;
  };

  typedef void (*request_func)(http_request*);
  GdsServer(unsigned int port_to_listen, request_func);

 private:
  static unsigned __stdcall Request(void*);
  static request_func request_func_;
  static void SplitGetReq(std::string get_req, std::string& path,
                          std::map<std::string, std::string>& params);
};
