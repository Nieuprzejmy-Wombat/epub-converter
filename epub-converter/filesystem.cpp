#include "filesystem.hpp"
#include "tag.hpp"
#include <fstream>
#include <memory>
#include <string>
#include <vector>

FileSystemResource::FileSystemResource(std::string path) : m_path{path} {};

std::string const FileSystemResource::filename() {
  size_t split_pos = m_path.find_last_of("/");
  return m_path.substr(split_pos + 1);
};

File::File(std::string path) : FileSystemResource{path} {};

void File::write() {
  std::ofstream stream;
  stream.open(m_path);
  stream << contents();
  stream.close();
};

Folder::Folder(std::string path,
               std::vector<std::shared_ptr<FileSystemResource>> files)
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

XMLFile::XMLFile(std::string path, std::shared_ptr<AbstractTag> body)
    : File{path}, m_body{body} {};

std::string XMLFile::contents() {
  return "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" + m_body->to_string();
};

Link::Link(std::string href)
    : Tag("link",
          {{"rel", "stylesheet"}, {"type", "text/css"}, {"href", href}}) {};

// std::vector<Link *> stylesheets
XHTMLFile::XHTMLFile(std::string path, std::shared_ptr<AbstractTag> html_body)
    : ContentFile{path},
      m_body{new Tag(
          "html",
          {{"xmlns", "http://www.w3.org/1999/xhtml"},
           {"xmlns:epub", "http://www.idpf.org/2007/ops"}},
          children{std::make_shared<Tag>(
                       "head",
                       children{std::make_shared<Tag>(
                                    "meta", attrs{{"charset", "utf-8"}}),
                                std::make_shared<Tag>(
                                    "title",
                                    children{std::make_shared<Text>(path)})}),
                   html_body})}

{};

std::string XHTMLFile::contents() { return XMLFile(m_path, m_body).contents(); }
