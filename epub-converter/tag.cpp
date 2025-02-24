#include "tag.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

std::ostream &operator<<(std::ostream &stream, const AbstractTag &tag) {
  return stream << tag.to_string();
}

Tag::Tag(std::string name, std::map<std::string, std::string> attributes,
         std::vector<std::shared_ptr<AbstractTag>> children)
    : m_name{name}, m_attributes{attributes}, m_children{children} {};

Tag::Tag(std::string name, std::vector<std::shared_ptr<AbstractTag>> children)
    : m_name{name}, m_attributes{}, m_children{children} {};

Tag::Tag(std::string name, std::map<std::string, std::string> attributes)
    : m_name{name}, m_attributes{attributes}, m_children{} {};

std::string Tag::to_string() const {
  std::string res = "<" + m_name;
  for (auto attr : m_attributes)
    res += " " + attr.first + "=\"" + attr.second + "\"";
  if (m_children.empty())
    return res + "/>\n";
  res += ">";
  for (auto it : m_children) {
    res += it->to_string();
  }
  return res + "</" + m_name + ">\n";
};

Text::Text(const std::string contents) : m_contents{contents} {};
std::string Text::to_string() const { return m_contents; };
