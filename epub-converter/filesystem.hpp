#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include "xml.hpp"
#include <string>
#include <vector>

class FileSystemResource {
protected:
  FileSystemResource(std::string name);

public:
  std::string m_name;
  virtual void write() = 0;
};

class File : public FileSystemResource {
protected:
  File(std::string name);
};

class Folder : public FileSystemResource {
protected:
  std::vector<FileSystemResource *> m_files;
  Folder(std::string name, std::vector<FileSystemResource *> files = {});

public:
  virtual void write() override;
};

class XMLFile : public File {
public:
  XMLTag *m_body;
  XMLFile(std::string name, XMLTag *body);
  void write();
  std::string to_string();
  friend std::ostream &operator<<(std::ostream &stream, XMLFile &file);
};

std::ostream &operator<<(std::ostream &stream, XMLFile &file);

class XHTMLAdapter : public XMLFile {
public:
  XHTMLAdapter(std::string title, std::vector<std::string> stylesheets,
               XMLTag *html_body);
};

#endif
