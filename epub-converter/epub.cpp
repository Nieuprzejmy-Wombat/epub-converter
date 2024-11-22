#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

class File {
protected:
  std::string_view m_filename;
  File(std::string_view filename) : m_filename{filename} {};

public:
  virtual void write() = 0;
};

class XMLTag {
protected:
  std::string_view m_name;
  std::map<std::string_view, std::string_view> m_attributes;
  std::vector<XMLTag> m_children;
  bool m_chidrenless;

public:
  XMLTag(std::string_view name,
         std::map<std::string_view, std::string_view> attributes = {})
      : m_name{name}, m_attributes{attributes}, m_children{{}},
        m_chidrenless{true} {};
  XMLTag(std::string_view name,
         std::map<std::string_view, std::string_view> attributes,
         std::vector<XMLTag> children)
      : m_name{name}, m_attributes{attributes}, m_children{children},
        m_chidrenless{false} {};
  friend std::ostream &operator<<(std::ostream &stream, const XMLTag &tag);
};

std::ostream &operator<<(std::ostream &stream, const XMLTag &tag) {
  stream << "<" << tag.m_name;
  for (auto [key, value] : tag.m_attributes)
    stream << " " << key << "=\"" << value << "\"";
  if (tag.m_chidrenless)
    return stream << "/>\n";
  stream << ">";
  for (auto child : tag.m_children)
    stream << child;
  return stream << "</" << tag.m_name << ">\n";
}

class XMLStringTag : public XMLTag {
  std::string_view m_contents;

public:
  XMLStringTag(std::string_view name,
               std::map<std::string_view, std::string_view> attributes,
               std::string_view contents)
      : XMLTag(name, attributes), m_contents{contents} {};
  friend std::ostream &operator<<(std::ostream &stream,
                                  const XMLStringTag &tag);
};

std::ostream &operator<<(std::ostream &stream, const XMLStringTag &tag) {
  stream << "<" << tag.m_name;
  for (auto [key, value] : tag.m_attributes)
    stream << " " << key << "=\"" << value << "\"";
  return stream << ">" << tag.m_contents << "</" << tag.m_name << ">\n";
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
  friend std::ostream &operator<<(std::ostream &stream, const XMLFile &file);
};

std::ostream &operator<<(std::ostream &stream, const XMLFile &file) {
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
                {XMLTag("rootfiles", {},
                        {XMLTag("rootfile",
                                {{"full-path", "EPUB/package.opf"},
                                 {"media-type",
                                  "application/oebps-package+xml"}})})})){};
};

class Meta : File {
  ContainerFile m_container;

public:
  Meta(ContainerFile container) : File("META-INF"), m_container{container} {};
  void write() {
    system("mkdir META-INF");
    m_container.write();
  };
};

class ManifestItem {
  std::string m_href;
  std::string m_id;
  std::string m_type;
  // properties??
  friend std::ostream &operator<<(std::ostream &stream, ManifestItem &item);
};

std::ostream &operator<<(std::ostream &stream, ManifestItem &item) {
  return stream << XMLTag("item", {{"href", item.m_href},
                                   {"id", item.m_id},
                                   {"media-type", item.m_type}});
}

class SpineItem {
  std::string_view m_id;
  // linear??
  friend std::ostream &operator<<(std::ostream &stream, SpineItem &item);
};

std::ostream &operator<<(std::ostream &stream, SpineItem &item) {
  return stream << XMLTag("itemref", {{"idref", item.m_id}});
}

class PackageFile : public XMLFile {
public:
  PackageFile(std::string_view id, std::string_view title,
              std::string_view creator, std::string_view language,
              std::vector<XMLTag> manifest, std::vector<XMLTag> spine)
      : XMLFile( // dcterms:modified ??
            "EPUB/package.opf",
            XMLTag("package",
                   {{"xmlns", "http://www.idpf.org/2007/opf"},
                    {"version", "3.0"},
                    {"unique-identifier", "uid"}},
                   {XMLTag("metadata",
                           {{"xmlns:dc", "http://purl.org/dc/elements/1.1/"}},
                           {XMLStringTag("dc:identifier", {{"id", "uid"}}, id),
                            XMLStringTag("dc:title", {}, title),
                            XMLStringTag("dc:creator", {}, creator),
                            XMLStringTag("dc:language", {}, language)}),
                    XMLTag("manifest", {}, manifest),
                    XMLTag("spine", {}, spine)})) {}
};

class ContentFile : File {
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

class Content : File {
  PackageFile m_package;
  ContentFile &m_content;

public:
  Content(PackageFile package, ContentFile &content)
      : File("EPUB"), m_package{package}, m_content{content} {};
  void write() {
    system("mkdir EPUB");
    m_package.write();
    m_content.write();
  }
};

class Epub : File {
  Meta m_meta;
  Content m_content;

public:
  Epub(Meta meta, Content content)
      : File("epub"), m_meta{meta}, m_content{content} {};
  void write() {
    m_meta.write();
    m_content.write();
    system("echo application/epub+zip > mimetype");
  };
};

int main() {
  Meta meta{ContainerFile()};
  ContentFile file = ContentFile("EPUB/plik", "<html></html>");
  Content content =
      Content(PackageFile("1", "tytuł", "autor", "polski", {}, {}), file);
  Epub epub = Epub(meta, content);
  epub.write();
  return 0;
}
