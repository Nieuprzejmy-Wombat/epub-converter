#include "filesystem.hpp"
#include "scrapping.hpp"
#include <iostream>

int main() {
  SingleFile strat{
      {"file", "text", "text/plain"},
      std::make_shared<Default>(Metadata{"1", "title", "author", "en"}),
      std::make_shared<Empty>()};
  auto res = strat();
  if (!res.m_success) {
    std::cout << *res.m_error;
    delete res.m_error;
    return 1;
  }
  res.m_value->write();
  delete res.m_value;
  return 0;
}
