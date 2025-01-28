#include "nav.hpp"

const std::string to_string(NavType type) {
  switch (type) {
  case toc:
    return "toc";
    break;
  case pagelist:
    return "page-list";
    break;
  case landmarks:
    return "landmarks";
    break;
  default:
    return "toc";
  }
}

Nav::Nav(NavType type, XMLTag *title, OrderedList *list)
    : XMLTag("nav",
             {{"epub:type", ::to_string(type)}, {"id", ::to_string(type)}},
             {title, list}) {};

Anchor::Anchor(std::string href, std::string caption,
               std::map<std::string, std::string> attributes)
    : XMLStringTag("a", attributes, caption) {
  m_attributes.insert({"href", href});
};

ListItem::ListItem(Anchor *anchor, OrderedList *nested_list)
    : XMLTag("li", {anchor}) {
  if (nested_list) {
    m_children.push_back(nested_list);
  }
};

OrderedList::OrderedList(std::map<std::string, std::string> attributes,
                         std::vector<ListItem *> items)
    : XMLTag("ol", attributes, upcast(items)) {};
OrderedList::OrderedList(std::vector<ListItem *> items)
    : XMLTag("ol", upcast(items)) {};

std::vector<XMLTag *> upcast(const std::vector<ListItem *> &items) {
  std::vector<XMLTag *> res;
  for (auto it : items) {
    res.push_back(it);
  }
  return res;
}
