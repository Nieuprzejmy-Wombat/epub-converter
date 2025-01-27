#include <ctime>
#include <string>
#include "util.hpp"

std::string now(){
  time_t now;
  time(&now);
  char buf[sizeof "2011-10-08T07:07:09Z"];
  strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
  return buf;
}
