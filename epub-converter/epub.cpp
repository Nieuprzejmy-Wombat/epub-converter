#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

class FileSystemResource {
public:
  virtual void write() = 0;
};

class File : public FileSystemResource {
protected:
  std::string_view m_filename;
  File(std::string_view filename) : m_filename{filename} {};
};

class XMLTag {
protected:
  std::string_view m_name;
  std::map<std::string_view, std::string_view> m_attributes;
  std::vector<std::unique_ptr<XMLTag>> m_children;

public:
  XMLTag(std::string_view name,
         std::map<std::string_view, std::string_view> attributes,
         std::vector<std::unique_ptr<XMLTag>> children = {})
      : m_name{name}, m_attributes{attributes}, m_children{children} {};
  XMLTag(std::string_view name,
         std::vector<std::unique_ptr<XMLTag>> children = {})
      : m_name{name}, m_attributes{{}}, m_children{children} {};

  friend std::ostream &operator<<(std::ostream &stream, const XMLTag &tag);
  friend const std::string toString(const XMLTag &tag);
};

const std::string toString(const XMLTag &tag) {
  std::string res = "<" + std::string{tag.m_name};
  for (auto [key, value] : tag.m_attributes)
    res += " " + std::string{key} + "=\"" + std::string{value} + "\"";
  if (tag.m_children.empty())
    return res + " />\n";
  res += ">";
  for (auto i{tag.m_children.begin()}; i != tag.m_children.end(); i++)
    res += toString(**i);
  return res + "</" + std::string{tag.m_name} + ">\n";
}

std::ostream &operator<<(std::ostream &stream, const XMLTag &tag) {
  return stream << toString(tag);
}

class XMLStringTag : public XMLTag {
  std::string_view m_contents;

public:
  XMLStringTag(std::string_view name,
               std::map<std::string_view, std::string_view> attributes,
               std::string_view contents)
      : XMLTag{name, attributes}, m_contents{contents} {};
  XMLStringTag(std::string_view name, std::string_view contents)
      : XMLTag{name}, m_contents{contents} {};
  friend const std::string toString(const XMLStringTag &tag);
};

const std::string toString(const XMLStringTag &tag) {
  std::string res = "<" + std::string{tag.m_name};
  for (auto [key, value] : tag.m_attributes)
    res += " " + std::string{key} + "=\"" + std::string{value} + "\"";
  return res + ">" + std::string{tag.m_contents} + "</" +
         std::string{tag.m_name} + ">\n";
}

class XMLFile : public File {
  XMLTag m_body;

public:
  XMLFile(std::string_view filename, XMLTag body)
      : File(filename), m_body{body} {};
  void write() {
    std::ofstream stream;
    stream.open(std::string{m_filename});
    stream << *this;
    stream.close();
  }
  friend std::ostream &operator<<(std::ostream &stream, XMLFile &file);
};

std::ostream &operator<<(std::ostream &stream, XMLFile &file) {
  stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";
  stream << file.m_body;
  return stream;
}

class ContainerFile : public XMLFile {
public:
  ContainerFile()
      : XMLFile(
            "META-INF/container.xml",
            XMLTag(
                "container",
                {{"xmlns", "urn:oasis:names:tc:opendocument:xmlns:container"},
                 {"version", "1.0"}},
                {std::make_unique<XMLTag>(
                    "rootfiles",
                    std::vector<std::unique_ptr<XMLTag>>{
                        std::make_unique<XMLTag>(
                            "rootfile",
                            std::map<std::string_view, std::string_view>{
                                {"full-path", "EPUB/package.opf"},
                                {"media-type",
                                 "application/oebps-package+xml"}})})})) {

        };
};

class Meta : FileSystemResource {
  ContainerFile m_container;

public:
  Meta(ContainerFile container) : m_container{container} {};
  void write() {
    system("mkdir -p META-INF");
    m_container.write();
  };
};

class ManifestItem : XMLTag {
  // properties??
  ManifestItem(std::string href, std::string id, std::string type)
      : XMLTag("item", {{"href", href}, {"id", id}, {"media-type", type}}) {};
};

class SpineItem : XMLTag {
  // linear??
  SpineItem(std::string id) : XMLTag{"itemref", {{"idref", id}}} {};
};

class PackageFile : public XMLFile {
public:
  PackageFile(std::string_view id, std::string_view title,
              std::string_view creator, std::string_view language,
              std::vector<std::unique_ptr<XMLTag>> manifest,
              std::vector<std::unique_ptr<XMLTag>> spine)
      : XMLFile( // dcterms:modified ??
            "EPUB/package.opf",
            XMLTag(
                "package",
                {{"xmlns", "http://www.idpf.org/2007/opf"},
                 {"version", "3.0"},
                 {"unique-identifier", "uid"}},
                {std::make_unique<XMLTag>(
                     "metadata",
                     std::map<std::string_view, std::string_view>{
                         {"xmlns:dc", "http://purl.org/dc/elements/1.1/"}},
                     std::vector<std::unique_ptr<XMLTag>>{
                         std::make_unique<XMLStringTag>(
                             "dc:identifier",
                             std::map<std::string_view, std::string_view>{
                                 {"id", "uid"}},
                             id),
                         std::make_unique<XMLStringTag>("dc:title", title),
                         std::make_unique<XMLStringTag>("dc:creator", creator),
                         std::make_unique<XMLStringTag>("dc:language",
                                                        language)}),
                 std::make_unique<XMLTag>("manifest", manifest),
                 std::make_unique<XMLTag>("spine", spine)})) {}
};

class ContentFile : public File {
  std::string_view m_content;

public:
  ContentFile(std::string_view filename, std::string_view content)
      : File(filename), m_content{content} {};
  void write() {
    std::ofstream stream;
    stream.open(std::string{m_filename});
    stream << m_content;
    stream.close();
  };
};

class Content : FileSystemResource {
  PackageFile m_package;
  FileSystemResource &m_content;

public:
  Content(PackageFile package, FileSystemResource &content)
      : m_package{package}, m_content{content} {};
  void write() {
    system("mkdir -p EPUB");
    m_package.write();
    m_content.write();
  }
};

class Epub : FileSystemResource {
  Meta m_meta;
  Content m_content;

public:
  Epub(Meta meta, Content content) : m_meta{meta}, m_content{content} {};
  void write() {
    m_meta.write();
    m_content.write();
    system("echo application/epub+zip > mimetype");
  };
};

int main() {
  Meta meta{ContainerFile{}};
  ContentFile file = ContentFile{"EPUB/plik", "<html></html>"};
  Content content =
      Content(PackageFile("1", "tytuł", "autor", "pl", {}, {}), file);
  Epub epub = Epub(meta, content);
  epub.write();
  return 0;
}
