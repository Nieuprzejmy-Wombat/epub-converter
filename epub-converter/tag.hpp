#ifndef TAG_HPP
#define TAG_HPP

#include <map>
#include <string>
#include <vector>

class AbstractTag {
public:
  virtual std::string to_string() const = 0;
  friend std::ostream &operator<<(std::ostream &stream, const AbstractTag &tag);
};

std::ostream &operator<<(std::ostream &stream, const AbstractTag &tag);

class Tag : public AbstractTag {
protected:
  const std::string m_name; // dangling references
  std::map<std::string, std::string> m_attributes;
  std::vector<AbstractTag *> m_children;

public:
  Tag(const std::string name, std::map<std::string, std::string> attributes,
      std::vector<AbstractTag *> children);
  Tag(const std::string name, std::vector<AbstractTag *> children);
  Tag(const std::string name,
      std::map<std::string, std::string> attributes = {});
  virtual std::string to_string() const override;
};

class Text : public AbstractTag {
protected:
  const std::string m_contents;

public:
  Text(const std::string name);
  virtual std::string to_string() const override;
};

#endif
