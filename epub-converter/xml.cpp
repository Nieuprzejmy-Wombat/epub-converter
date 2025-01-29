#include "xml.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

XMLTag::XMLTag(std::string name, std::map<std::string, std::string> attributes)
    : m_name{name}, m_attributes{attributes},
      m_children{std::vector<XMLTag *>{}} {};

XMLTag::XMLTag(std::string name, std::vector<XMLTag *> children)
    : m_name{name}, m_attributes{}, m_children{children} {};

XMLTag::XMLTag(std::string name, std::map<std::string, std::string> attributes,
               std::vector<XMLTag *> children)
    : m_name{name}, m_attributes{attributes}, m_children{children} {};

const std::string XMLTag::to_string() {
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
}

std::ostream &operator<<(std::ostream &stream, XMLTag &tag) {
  return stream << tag.to_string();
}

XMLStringTag::XMLStringTag(std::string name,
                           std::map<std::string, std::string> attributes,
                           std::string contents)
    : XMLTag(name, attributes), m_contents{contents} {};

XMLStringTag::XMLStringTag(std::string name, std::string contents)
    : XMLTag(name), m_contents{contents} {};

const std::string XMLStringTag::to_string() {
  std::string res = "<" + m_name;
  for (auto attr : m_attributes)
    res += " " + attr.first + "=\"" + attr.second + "\"";
  res = res + ">" + m_contents + "</" + m_name + ">\n";
  return res;
};
