#include "filesystem.hpp"
#include "xml.hpp"
#include <fstream>
#include <string>
#include <vector>

FileSystemResource::FileSystemResource(std::string path) : m_path{path} {};

std::string const FileSystemResource::filename() {
  size_t split_pos = m_path.find_last_of("/", -1);
  return m_path.substr(split_pos + 1);
};

File::File(std::string path) : FileSystemResource{path} {};

void File::write() {
  std::ofstream stream;
  stream.open(m_path);
  stream << contents();
  stream.close();
};

Folder::Folder(std::string path, std::vector<FileSystemResource *> files)
    : FileSystemResource{path}, m_files{files} {};

void Folder::write() {
  system(("mkdir -p " + m_path).c_str());
  for (auto file : m_files) {
    file->write();
  }
};

ContentFile::ContentFile(std::string path, std::string mimetype)
    : File("EPUB/" + path), m_mimetype{mimetype} {};

const std::string &ContentFile::mimetype() { return m_mimetype; };

XMLFile::XMLFile(std::string path, XMLTag *body) : File{path}, m_body{body} {};

std::string XMLFile::contents() {
  return "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" + m_body->to_string();
};

XHTMLFile::XHTMLFile(std::string path, std::vector<std::string> stylesheets,
                     XMLTag *html_body)
    : ContentFile{path},
      m_body{new XMLTag(
          "html",
          {{"xmlns", "http://www.w3.org/1999/xhtml"},
           {"xmlns:epub", "http://www.idpf.org/2007/ops"}},
          {new XMLTag("head", {new XMLTag("meta", {{"charset", "utf-8"}}),
                               new XMLStringTag("title", path)}),
           html_body})} {
  for (auto i : stylesheets) {
    m_body->m_children[0]->m_children.push_back(new XMLTag(
        "link", {{"rel", "stylesheet"}, {"type", "text/css"}, {"href", i}}));
  }
};

std::string XHTMLFile::contents() { return XMLFile(m_path, m_body).contents(); }
