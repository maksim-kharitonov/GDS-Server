#include <map>
#include <string>

class Socket;

class GdsServer {
 public:
  struct http_request {
    http_request() {}

    Socket* sPtr;
    std::string _method;
    std::string _path;
    std::map<std::string, std::string> params_;

    std::string accept_;
    std::string _accept_language;
    std::string _accept_encoding;
    std::string _user_agent;

    std::string _content_type;
    std::string _http_status;
    std::string _answer;
  };

  typedef void (*request_func)(http_request*);
  GdsServer(unsigned int port, request_func);

 private:
  static unsigned __stdcall Request(void*);
  static request_func request_func_;
  static void SplitGetReq(std::string get_req, std::string& path,
                          std::map<std::string, std::string>& params);
};
