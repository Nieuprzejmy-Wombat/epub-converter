#include <map>
#include <vector>
#include <ostream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "xml.hpp"

XMLTag::XMLTag(std::string name,
         std::map<std::string, std::string> attributes)
      : m_name{name}, m_attributes{attributes}, m_children{std::vector<XMLTag*>{}} {};

XMLTag::XMLTag(std::string name,
         std::map<std::string, std::string> attributes,
         std::vector<XMLTag*> children)
      : m_name{name}, m_attributes{attributes}, m_children{children} {};

const std::string XMLTag::to_string() {
      std::string res = "<" + m_name;
      for (auto [key, value] : m_attributes)
        res += " " + key + "=\"" + value + "\"";
      if (m_children.empty())
        return res + "/>\n";
      res += ">";
      for (auto it : m_children)
        res += it->to_string();
      return res + "</" + m_name + ">\n";
}

std::ostream &operator<<(std::ostream &stream, XMLTag &tag) {
  return stream << tag.to_string();
}

XMLStringTag::XMLStringTag(std::string name,
               std::map<std::string, std::string> attributes,
               std::string contents)
      : XMLTag(name, attributes), m_contents{contents} {};

const std::string XMLStringTag::to_string() {
      std::string res = "<" + m_name;
      for (auto [key, value] : m_attributes)
        res += " " + key + "=\"" + value + "\"";
      return res + ">" + m_contents + "</" + m_name + ">\n";
}
