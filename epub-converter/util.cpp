#include "util.hpp"
#include <ctime>
#include <string>

std::string now() {
  time_t now;
  time(&now);
  char buf[sizeof "2011-10-08T07:07:09Z"];
  strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
  return buf;
}
