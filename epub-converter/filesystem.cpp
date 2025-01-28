#include "filesystem.hpp"
#include "xml.hpp"
#include <fstream>
#include <ostream>
#include <string>
#include <vector>

FileSystemResource::FileSystemResource(std::string name) : m_name{name} {};

File::File(std::string name) : FileSystemResource{name} {};

Folder::Folder(std::string name, std::vector<FileSystemResource *> files)
    : FileSystemResource{name}, m_files{files} {};

void Folder::write() {
  system(("mkdir -p " + m_name).c_str());
  for (auto file : m_files) {
    file->write();
  }
};

XMLFile::XMLFile(std::string name, XMLTag *body) : File{name}, m_body{body} {};

void XMLFile::write() {
  std::ofstream stream;
  stream.open(m_name);
  stream << *this;
  stream.close();
};

std::string XMLFile::to_string() {
  return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" + m_body->to_string();
};

std::ostream &operator<<(std::ostream &stream, XMLFile &file) {
  return stream << file.to_string();
}

XHTMLAdapter::XHTMLAdapter(std::string title,
                           std::vector<std::string> stylesheets,
                           XMLTag *html_body)
    : XMLFile(
          title,
          new XMLTag(
              "html",
              {{"xmlns", "http://www.w3.org/1999/xhtml"},
               {"xmlns:epub", "http://www.idpf.org/2007/ops"}},
              {new XMLTag("head", {new XMLTag("meta", {{"charset", "utf-8"}}),
                                   new XMLStringTag("title", title)}),
               new XMLTag("body", {html_body})})) {
  for (auto i : stylesheets) {
    m_body->m_children[0]->m_children.push_back(new XMLTag(
        "link", {{"rel", "stylesheet"}, {"type", "text/css"}, {"href", i}}));
  }
};
