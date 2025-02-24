#include "nav.hpp"
#include "tag.hpp"
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

Nav::Nav(NavType type, std::shared_ptr<Header> title,
         std::shared_ptr<OrderedList> list)
    : Tag("nav", {{"epub:type", ::to_string(type)}, {"id", ::to_string(type)}},
          {title, list}) {};

Anchor::Anchor(std::string href, std::string caption,
               std::map<std::string, std::string> attributes)
    : Tag("a", attributes, {std::make_shared<Text>(caption)}) {
  m_attributes.insert({"href", href});
};

ListItem::ListItem(std::shared_ptr<Anchor> anchor,
                   std::shared_ptr<OrderedList> nested_list)
    : Tag("li", {anchor, nested_list}) {};

ListItem::ListItem(std::shared_ptr<Anchor> anchor) : Tag("li", {anchor}) {};

OrderedList::OrderedList(std::map<std::string, std::string> attributes,
                         std::vector<std::shared_ptr<ListItem>> items)
    : Tag("ol", attributes,
          upcast<std::shared_ptr<ListItem>, std::shared_ptr<AbstractTag>>(
              items)) {};
OrderedList::OrderedList(std::vector<std::shared_ptr<ListItem>> items)
    : Tag("ol", upcast<std::shared_ptr<ListItem>, std::shared_ptr<AbstractTag>>(
                    items)) {};

Header::Header(short level, std::string contents)
    : Tag("h" + std::to_string(level), {std::make_shared<Text>(contents)}) {};
Header::Header(std::string contents) : Header(1, contents) {};
