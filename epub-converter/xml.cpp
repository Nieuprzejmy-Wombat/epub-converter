#include <map>
#include <memory>
#include <vector>
#include <ostream>
#include <iostream>
#include <cstdio>
#include <cstdlib>

class XMLTag {
public:
  std::string m_name;
  std::map<std::string, std::string> m_attributes;
  std::vector<XMLTag*> m_children;

  XMLTag(std::string name,
         std::map<std::string, std::string> attributes = {})
      : m_name{name}, m_attributes{attributes}, m_children{std::vector<XMLTag*>{}} {};
  XMLTag(std::string name,
         std::map<std::string, std::string> attributes,
         std::vector<XMLTag*> children)
      : m_name{name}, m_attributes{attributes}, m_children{} {
        for (auto it = children.begin(); it != children.end(); it++) {
          m_children.push_back(*it);
        }
      };
  virtual const std::string to_string() {
      std::string res = "<" + m_name;
      for (auto [key, value] : m_attributes)
        res += " " + key + "=\"" + value + "\"";
      if (m_children.empty())
        return res + "/>\n";
      res += ">";
      for (auto it = m_children.begin(); it != m_children.end(); it++)
        res += (*it)->to_string();
      return res + "</" + m_name + ">\n";
  }
  friend std::ostream &operator<<(std::ostream &stream, XMLTag &tag);
};

std::ostream &operator<<(std::ostream &stream, XMLTag &tag) {
  return stream << tag.to_string();
}

class XMLStringTag : public XMLTag {
  std::string m_contents;

public:
  XMLStringTag(std::string name,
               std::map<std::string, std::string> attributes,
               std::string contents)
      : XMLTag(name, attributes), m_contents{contents} {};
  const std::string to_string() override {
      std::string res = "<" + m_name;
      for (auto [key, value] : m_attributes)
        res += " " + key + "=\"" + value + "\"";
      return res + ">" + m_contents + "</" + m_name + ">\n";
  }
  friend std::ostream &operator<<(std::ostream &stream,
                                   XMLStringTag &tag);
};

std::ostream &operator<<(std::ostream &stream, XMLStringTag &tag) {
  return stream << tag.to_string();
}

