#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include "tag.hpp"
#include <string>
#include <vector>

class FileSystemResource {
protected:
  FileSystemResource(std::string path);

public:
  std::string m_path;
  virtual void write() = 0;
  std::string const filename();
};

class File : public FileSystemResource {
public:
  virtual std::string contents() = 0;
  virtual void write() override;

protected:
  File(std::string path);
};

class Folder : public FileSystemResource {
protected:
  std::vector<FileSystemResource *> m_files;
  Folder(std::string path, std::vector<FileSystemResource *> files = {});

public:
  virtual void write() override;
};

class ContentFile : public File {
  std::string m_mimetype;

public:
  ContentFile(std::string path, std::string mimetype = "application/xhtml+xml");
  const std::string &mimetype();
};

class XMLFile : public File {
public:
  AbstractTag *m_body;
  XMLFile(std::string path, AbstractTag *body);
  std::string contents() override;
};

class Link : public Tag {
public:
  Link(std::string href);
};

class XHTMLFile : public ContentFile {
public:
  AbstractTag *m_body;
  XHTMLFile(std::string path, AbstractTag *html_body);
  std::string contents() override;
};

#endif
