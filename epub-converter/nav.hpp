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
              std::vector<ListItem *> items = {});
  OrderedList(std::vector<ListItem *> items);
};

class ListItem : public Tag {
public:
  ListItem(Anchor *anchor, OrderedList *nested_list = nullptr);
};

class Nav : public Tag {
public:
  Nav(NavType type, Tag *title, OrderedList *list);
};

#endif
