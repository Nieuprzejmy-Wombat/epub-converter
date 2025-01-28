#include "epub.hpp"
#include "filesystem.hpp"
#include "nav.hpp"
#include "util.hpp"
#include "xml.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <string>

// Michaił Bachtin

ContainerFile::ContainerFile()
    : XMLFile("META-INF/container.xml",
              new XMLTag(
                  "container",
                  {{"xmlns", "urn:oasis:names:tc:opendocument:xmlns:container"},
                   {"version", "1.0"}},
                  {new XMLTag(
                      "rootfiles",
                      {new XMLTag("rootfile",
                                  {{"full-path", "EPUB/package.opf"},
                                   {"media-type",
                                    "application/oebps-package+xml"}})})})) {};

Meta::Meta(ContainerFile *container) : Folder{"META-INF", {container}} {};

// properties??
ManifestItem::ManifestItem(std::string href, std::string id, std::string type)
    : XMLTag("item", {{"href", href}, {"id", id}, {"media-type", type}}) {}

// linear??
SpineItem::SpineItem(std::string id) : XMLTag("itemref", {{"idref", id}}) {};

PackageFile::PackageFile(std::string id, std::string title, std::string creator,
                         std::string language, XHTMLAdapter *nav,
                         std::vector<ManifestItem *> manifest,
                         std::vector<SpineItem *> spine)
    : XMLFile(
          "EPUB/package.opf",
          new XMLTag(
              "package",
              {{"xmlns", "http://www.idpf.org/2007/opf"},
               {"version", "3.0"},
               {"unique-identifier", "uid"}},
              {new XMLTag(
                   "metadata",
                   {{"xmlns:dc", "http://purl.org/dc/elements/1.1/"}},
                   {new XMLStringTag("dc:identifier", {{"id", "uid"}}, id),
                    new XMLStringTag("dc:title", title),
                    new XMLStringTag("dc:creator", creator),
                    new XMLStringTag("dc:language", language),
                    new XMLStringTag("meta", {{"property", "dcterms:modified"}},
                                     now())}),
               new XMLTag("manifest", upcast(manifest)),
               new XMLTag("spine", upcast(spine))})) {
  XMLTag *nav_man =
      new ManifestItem(nav->m_name, "nav", "application/xhtml+xml");
  XMLTag *man = m_body->m_children[1];
  man->m_children.push_back(nav_man);
  nav_man->m_attributes.insert({"properties", "nav"});
}

ContentFile::ContentFile(std::string name, std::string content)
    : File("EPUB/" + name), m_content{content} {};

ContentFile::ContentFile(XHTMLAdapter xhtml_file)
    : File("EPUB/" + xhtml_file.m_name), m_content{xhtml_file.to_string()} {};

void ContentFile::write() {
  std::ofstream stream;
  stream.open(m_name);
  stream << m_content;
  stream.close();
};

Content::Content(PackageFile *package,
                 std::vector<FileSystemResource *> content)
    : Folder{"EPUB", content} {
  m_files.push_back(package);
};

Epub::Epub(std::string name, Meta meta, Content content)
    : File{name}, m_meta{meta}, m_content{content} {};

void Epub::write() {
  m_meta.write();
  m_content.write();
  system("echo -n application/epub+zip > mimetype");
  std::string zip = "zip -0 -X " + m_name + ".epub mimetype";
  system(zip.c_str());
  zip =
      "zip -9 -r " + m_name + ".epub " + m_meta.m_name + " " + m_content.m_name;
  system(zip.c_str());
  std::string rm = "rm -r mimetype " + m_meta.m_name + " " + m_content.m_name;
  system(rm.c_str());
};

int main() {
  Meta meta{new ContainerFile()};
  ContentFile *file = new ContentFile{
      "plik.xhtml",
      "<?xml version=\"1.0\" encoding=\"utf-8\"?><html "
      "xmlns=\"http://www.w3.org/1999/xhtml\"><head><meta "
      "charset=\"utf-8\"/><title>EPUB 3.0 Specification</title><link "
      "rel=\"stylesheet\" type=\"text/css\" "
      "href=\"plik.css\"/></head><body><h1 class=\"titlepage\">EPUB 3.0 "
      "Specification</h1>        <p><em>The documents canonically located at "
      "<a href=\"http://idpf.org/epub/30\">http://idpf.org/epub/30</a> "
      "reproduced in EPUB 3 format</em></p>        <div "
      "class=\"legalnotice\"><p>All rights reserved. This work is protected "
      "under Title 17 of the United States Code. Reproduction and "
      "dissemination of this work with changes is prohibited except with the "
      "written permission of the <a href=\"http://www.idpf.org\">International "
      "Digital Publishing Forum (IDPF)</a>. </p><p>EPUB is a registered "
      "trademark of the International Digital Publishing "
      "Forum.</p></div></body></html>"};
  ContentFile *style = new ContentFile{"plik.css", ""};
  Nav *nav = new Nav(
      toc, new XMLStringTag("h1", "spis treści"),
      new OrderedList({new ListItem(new Anchor("plik.xhtml", "pliczek"))}));
  XHTMLAdapter *nav_file = new XHTMLAdapter("nav.xhtml", {}, nav);
  PackageFile *package = new PackageFile(
      "1", "tytuł", "autor", "pl", nav_file,
      {new ManifestItem("plik.xhtml", "ttl", "application/xhtml+xml"),
       new ManifestItem("plik.css", "css", "text/css")},
      {new SpineItem("ttl")});
  Content content{package, {file, style, new ContentFile(*nav_file)}};
  Epub epub{"test", meta, content};
  epub.write();
  return 0;
};
