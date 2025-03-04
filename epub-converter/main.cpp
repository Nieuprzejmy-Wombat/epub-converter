#include "filesystem.hpp"
#include "scrapping.hpp"
#include <iostream>

int main() {
  Default meta_strat{{"1", "title", "author", "en"}};
  Empty nav_strat{};
  SingleFile strat{
      std::make_shared<XHTMLFile>(
          "file", std::make_shared<Tag>(
                      "body", children{std::make_shared<Header>("test")})),
      meta_strat, nav_strat};
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
