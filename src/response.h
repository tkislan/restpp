#ifndef RESPONSE
#define RESPONSE

#include <string>
#include <vector>

namespace restpp {
class Response {
public:

private:
  unsigned int status_;

  std::string method_;
  std::string path_;
  std::vector<std::pair<std::string, std::string>> query_params_;
  std::vector<std::pair<std::string, std::string>> headers_;
};
}

#endif // RESPONSE

