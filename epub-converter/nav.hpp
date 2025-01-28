#ifndef NAV_HPP
#define NAV_HPP

#include "xml.hpp"
#include <string>

enum NavType {toc, pagelist, landmarks};

const std::string to_string(NavType type);

class Anchor : public XMLStringTag {
public:
  Anchor(std::string href, std::string caption, std::map<std::string, std::string> attributes = {});
};

class ListItem;

class OrderedList : public XMLTag {
public:
  OrderedList(std::map<std::string, std::string> attributes = {}, std::vector<ListItem*> items = {});
  OrderedList(std::vector<ListItem*> items);
};

class ListItem : public XMLTag {
public:
  ListItem(Anchor * anchor, OrderedList * nested_list = nullptr);
};

class Nav : public XMLTag {
public:
  Nav(NavType type, XMLTag * title, OrderedList * list);
};

std::vector<XMLTag*> upcast(const std::vector<ListItem*>& items);

#endif
