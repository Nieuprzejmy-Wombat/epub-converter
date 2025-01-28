#ifndef XML_HPP
#define XML_HPP

#include <map>
#include <string>
#include <vector>

class XMLTag {
public:
  std::string m_name;
  std::map<std::string, std::string> m_attributes;
  std::vector<XMLTag *> m_children;

  XMLTag(std::string name, std::map<std::string, std::string> attributes = {});
  XMLTag(std::string name, std::vector<XMLTag *> children);
  XMLTag(std::string name, std::map<std::string, std::string> attributes,
         std::vector<XMLTag *> children);
  virtual const std::string to_string();
  friend std::ostream &operator<<(std::ostream &stream, XMLTag &tag);
};

std::ostream &operator<<(std::ostream &stream, XMLTag &tag);

class XMLStringTag : public XMLTag {
  std::string m_contents;

public:
  XMLStringTag(std::string name, std::map<std::string, std::string> attributes,
               std::string contents);
  XMLStringTag(std::string name, std::string contents);
  const std::string to_string() override;
};

#endif
