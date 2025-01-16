#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include "xml.cpp"

// Michaił Bachtin

class FileSystemResource {
public:
  virtual void write() = 0;
};

class File : public FileSystemResource {
protected:
  std::string_view m_filename;
  File(std::string_view filename) : m_filename{filename} {};
public:
  virtual void write() = 0;
};

class XMLFile : public File {
protected:
  XMLTag* m_body;

public:
  XMLFile(std::string_view filename, XMLTag* body)
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
  stream << file.m_body->to_string();
  return stream;
}

class ContainerFile : public XMLFile {
public:
  ContainerFile()
      : XMLFile(
            "META-INF/container.xml",
            new XMLTag(
                "container",
                {{"xmlns", "urn:oasis:names:tc:opendocument:xmlns:container"},
                 {"version", "1.0"}},
                 {
                  new XMLTag("rootfiles", {}, 
                    {
                      new XMLTag("rootfile", {{"full-path", "EPUB/package.opf"}, {"media-type", "application/oebps-package+xml"}})})
                  })){
};};

class Meta : FileSystemResource {
  ContainerFile m_container;

public:
  Meta(ContainerFile container) : m_container{container} {};
  void write() {
    system("mkdir -p META-INF");
    m_container.write();
  };
};

class ManifestItem : XMLTag{
  // properties??
  public:
    ManifestItem(std::string href, std::string id, std::string type) : 
      XMLTag("item", {{"href", href}, {"id", id}, {"media-type", type}}){}
};

class SpineItem : XMLTag{
  // linear??
  public:
    SpineItem(std::string id) : XMLTag("itemref", {{"idref", id}}){};
};

class PackageFile : public XMLFile {
public:
  PackageFile(std::string id, std::string title,
              std::string creator, std::string language,
              std::vector<XMLTag*> manifest, std::vector<XMLTag*> spine)
      : XMLFile( // dcterms:modified ??
            "EPUB/package.opf",
            new XMLTag("package",
                   {{"xmlns", "http://www.idpf.org/2007/opf"},
                    {"version", "3.0"},
                    {"unique-identifier", "uid"}},
                   {new XMLTag("metadata",
                           {{"xmlns:dc", "http://purl.org/dc/elements/1.1/"}},
                           {new XMLStringTag("dc:identifier", {{"id", "uid"}}, id),
                            new XMLStringTag("dc:title", {}, title),
                            new XMLStringTag("dc:creator", {}, creator),
                            new XMLStringTag("dc:language", {}, language)}),
                    new XMLTag("manifest", {}, manifest),
                    new XMLTag("spine", {}, spine)})) {}
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
