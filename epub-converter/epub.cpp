#include "epub.hpp"
#include "filesystem.hpp"
#include "nav.hpp"
#include "util.hpp"
#include "xml.hpp"
#include <cstdio>
#include <cstdlib>
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
ManifestItem::ManifestItem(std::string id, ContentFile &file)
    : XMLTag("item", {{"href", file.filename()},
                      {"id", id},
                      {"media-type", file.mimetype()}}) {};

// linear??
SpineItem::SpineItem(std::string id) : XMLTag("itemref", {{"idref", id}}) {};

PackageFile::PackageFile(std::string id, std::string title, std::string creator,
                         std::string language, XHTMLFile *nav,
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
  XMLTag *nav_man = new ManifestItem("nav", *nav);
  XMLTag *man = m_body->m_children[1];
  man->m_children.push_back(nav_man);
  nav_man->m_attributes.insert({"properties", "nav"});
}

Content::Content(PackageFile *package,
                 std::vector<FileSystemResource *> content)
    : Folder{"EPUB", content} {
  m_files.push_back(package);
};

Epub::Epub(std::string path, Meta meta, Content content)
    : File{path}, m_meta{meta}, m_content{content} {};

void Epub::write() {
  m_meta.write();
  m_content.write();
  system("echo -n application/epub+zip > mimetype");
  std::string zip = "zip -0 -X " + m_path + ".epub mimetype";
  system(zip.c_str());
  zip =
      "zip -9 -r " + m_path + ".epub " + m_meta.m_path + " " + m_content.m_path;
  system(zip.c_str());
  std::string rm = "rm -r mimetype " + m_meta.m_path + " " + m_content.m_path;
  system(rm.c_str());
};

// a dummy method so Epub won't be an abstract class
std::string Epub::contents() { return ""; }

int main() {
  Meta meta{};
  XHTMLFile *file = new XHTMLFile{
      "plik.xhtml",
      {},
      new XMLTag("body", {new XMLStringTag("h1", "dummy epub content")})};
  Nav *nav = new Nav(
      toc, new XMLStringTag("h1", "spis treści"),
      new OrderedList({new ListItem(new Anchor("plik.xhtml", "pliczek"))}));
  XHTMLFile *nav_file =
      new XHTMLFile("nav.xhtml", {}, new XMLTag("body", {nav}));
  PackageFile *package =
      new PackageFile("1", "tytuł", "autor", "pl", nav_file,
                      {new ManifestItem("ttl", *file)}, {new SpineItem("ttl")});
  Content content{package, {file, nav_file}};
  Epub epub{"test", meta, content};
  epub.write();
  return 0;
};
