#ifndef TAG_HPP
#define TAG_HPP

#include <map>
#include <memory>
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
  std::string m_name; // dangling references
  std::map<std::string, std::string> m_attributes;
  std::vector<std::shared_ptr<AbstractTag>> m_children;

public:
  Tag(std::string name, std::map<std::string, std::string> attributes,
      std::vector<std::shared_ptr<AbstractTag>> children);
  Tag(std::string name, std::vector<std::shared_ptr<AbstractTag>> children);
  Tag(std::string name, std::map<std::string, std::string> attributes = {});
  virtual std::string to_string() const override;
  const std::map<std::string, std::string> &attributes() const;
};

class Text : public AbstractTag {
protected:
  const std::string m_contents;

public:
  Text(const std::string name);
  virtual std::string to_string() const override;
};

using attrs = std::map<std::string, std::string>;

using children = std::vector<std::shared_ptr<AbstractTag>>;

#endif
