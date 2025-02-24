#ifndef NAV_HPP
#define NAV_HPP

#include "tag.hpp"
#include <string>

enum NavType { toc, pagelist, landmarks };

const std::string to_string(NavType type);

class Anchor : public Tag {
public:
  Anchor(std::string href, std::string caption,
         std::map<std::string, std::string> attributes = {});
};

class ListItem;

class OrderedList : public Tag {
public:
  OrderedList(std::map<std::string, std::string> attributes = {},
              std::vector<std::shared_ptr<ListItem>> items = {});
  OrderedList(std::vector<std::shared_ptr<ListItem>> items);
};

class ListItem : public Tag {
public:
  ListItem(std::shared_ptr<Anchor> anchor,
           std::shared_ptr<OrderedList> nested_list);
  ListItem(std::shared_ptr<Anchor> anchor);
};

class Nav : public Tag {
public:
  Nav(NavType type, std::shared_ptr<Tag> title,
      std::shared_ptr<OrderedList> list);
};

#endif
