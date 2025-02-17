#include "nav.hpp"
#include "util.hpp"

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

Nav::Nav(NavType type, Tag *title, OrderedList *list)
    : Tag("nav", {{"epub:type", ::to_string(type)}, {"id", ::to_string(type)}},
          {title, list}) {};

Anchor::Anchor(std::string href, std::string caption,
               std::map<std::string, std::string> attributes)
    : Tag("a", attributes, std::vector<AbstractTag *>{new Text(caption)}) {
  m_attributes.insert({"href", href});
};

ListItem::ListItem(Anchor *anchor, OrderedList *nested_list)
    : Tag("li", {anchor}) {
  if (nested_list) {
    m_children.push_back(nested_list);
  }
};

OrderedList::OrderedList(std::map<std::string, std::string> attributes,
                         std::vector<ListItem *> items)
    : Tag("ol", attributes, upcast(items)) {};
OrderedList::OrderedList(std::vector<ListItem *> items)
    : Tag("ol", upcast(items)) {};
