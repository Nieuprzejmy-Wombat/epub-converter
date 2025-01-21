#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include "xml.cpp"
#include "util.cpp"

// Michaił Bachtin

class FileSystemResource {
protected:
  FileSystemResource(std::string name) : m_name{name}{};
public:
  std::string m_name;
  virtual void write() = 0;
};

class File : public FileSystemResource {
protected: 
  File(std::string name) : FileSystemResource{name}{};
};

class Folder : public FileSystemResource {
protected:
  std::vector<FileSystemResource*> m_files;
  Folder(std::string name, std::vector<FileSystemResource*> files = {}) : FileSystemResource{name}, m_files{files}{};
public:
  virtual void write() override {
    std::cout << "writing " << m_name << "\n";
    system(("mkdir -p " + m_name).c_str());
    for (auto file : m_files) {
      std::cout << "writing " << file->m_name << "\n";
      file->write();
    }
  }
};

class XMLFile : public File {
protected:
  XMLTag* m_body;

public:
  XMLFile(std::string name, XMLTag* body)
      : File{name}, m_body{body} {};
  void write() {
    std::ofstream stream;
    stream.open(m_name);
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

class Meta : public Folder {
public:
  Meta(ContainerFile* container) : Folder{"META-INF", {container}} {};
};

class ManifestItem : public XMLTag{
  // properties??
  public:
    ManifestItem(std::string href, std::string id, std::string type) : 
      XMLTag("item", {{"href", href}, {"id", id}, {"media-type", type}}){}
};

class SpineItem : public XMLTag{
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
                            new XMLStringTag("dc:language", {}, language),
                            new XMLStringTag("meta", {{"property", "dcterms:modified"}}, now())}),
                    new XMLTag("manifest", {}, manifest),
                    new XMLTag("spine", {}, spine)})) {}
};

class ContentFile : public File {
  std::string m_content;

public:
  ContentFile(std::string name, std::string content)
      : File("EPUB/" + name), m_content{content} {};
  void write() {
    std::ofstream stream;
    stream.open(m_name);
    stream << m_content;
    stream.close();
  };
};

class Content : public Folder {
public:
  Content(PackageFile * package, std::vector<FileSystemResource*> content)
      : Folder{"EPUB", content} {
        m_files.push_back(package);
      };
};

class Epub : File {
  Meta m_meta;
  Content m_content;

public:
  Epub(std::string name, Meta meta, Content content) : File{name}, m_meta{meta}, m_content{content} {};
  void write() override {
    m_meta.write();
    m_content.write();
    system("echo application/epub+zip > mimetype");
    std::string zip = "zip " + m_name + ".epub -r " + m_meta.m_name + " " + m_content.m_name + " mimetype";
    system(zip.c_str());
    std::string rm = "rm -r mimetype " + m_meta.m_name + " " + m_content.m_name;
    system(rm.c_str());
  };
};

int main() {
  Meta meta{new ContainerFile()};
  ContentFile * file = new ContentFile{"plik.xhtml", "<?xml version=\"1.0\" encoding=\"utf-8\"?><html xmlns=\"http://www.w3.org/1999/xhtml\"><head><meta charset=\"utf-8\"/><title>EPUB 3.0 Specification</title><link rel=\"stylesheet\" type=\"text/css\" href=\"../css/epub-spec.css\"/></head><body><h1 class=\"titlepage\">EPUB 3.0 Specification</h1>        <p><em>The documents canonically located at <a href=\"http://idpf.org/epub/30\">http://idpf.org/epub/30</a> reproduced in EPUB 3 format</em></p>        <div class=\"legalnotice\"><p>All rights reserved. This work is protected under Title 17 of the United States Code. Reproduction and dissemination of this work with changes is prohibited except with the written permission of the <a href=\"http://www.idpf.org\">International Digital Publishing Forum (IDPF)</a>. </p><p>EPUB is a registered trademark of the International Digital Publishing Forum.</p></div></body></html>"};
  Content content{new PackageFile("1", "tytuł", "autor", "pl",
            {new ManifestItem("plik.xhtml", "ttl", "application/xhtml+xml")}, 
            {new SpineItem( "ttl")}), {file}};
  Epub epub{"test", meta, content};
  epub.write(); 
  return 0;
}
