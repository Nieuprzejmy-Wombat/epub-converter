#include "epub.hpp"
#include "filesystem.hpp"
#include "nav.hpp"
#include "util.hpp"
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>

// Michaił Bachtin

ContainerFile::ContainerFile()
    : XMLFile(
          "META-INF/container.xml",
          new Tag("container",
                  {{"xmlns", "urn:oasis:names:tc:opendocument:xmlns:container"},
                   {"version", "1.0"}},
                  {new Tag("rootfiles",
                           {new Tag("rootfile",
                                    {{"full-path", "EPUB/package.opf"},
                                     {"media-type",
                                      "application/oebps-package+xml"}})})})) {
      };

Meta::Meta(ContainerFile *container) : Folder{"META-INF", {container}} {};

// properties??
ManifestItem::ManifestItem(std::string id, ContentFile &file, bool is_nav)
    : Tag("item", {{"href", file.filename()},
                   {"id", id},
                   {"media-type", file.mimetype()}}) {
  if (is_nav) {
    m_attributes.insert({{"properties", "nav"}});
  }
};

// linear??
SpineItem::SpineItem(std::string id) : Tag("itemref", {{"idref", id}}) {};

PackageFile::PackageFile(std::string id, std::string title, std::string creator,
                         std::string language, XHTMLFile *nav,
                         std::vector<ManifestItem *> manifest,
                         std::vector<SpineItem *> spine)
    : XMLFile(
          "EPUB/package.opf",
          new Tag("package",
                  {{"xmlns", "http://www.idpf.org/2007/opf"},
                   {"version", "3.0"},
                   {"unique-identifier", "uid"}},
                  {new Tag("metadata",
                           {{"xmlns:dc", "http://purl.org/dc/elements/1.1/"}},
                           {new Tag("dc:identifier", {{"id", "uid"}},
                                    {new Text(id)}),
                            new Tag("dc:title", {new Text(title)}),
                            new Tag("dc:creator", {new Text(creator)}),
                            new Tag("dc:language", {new Text(language)}),
                            new Tag("meta", {{"property", "dcterms:modified"}},
                                    {new Text(now())})}),
                   new Tag("manifest",
                           upcast(add(manifest,
                                      new ManifestItem("nav", *nav, true)))),
                   new Tag("spine", upcast(spine))})) {}

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
      new Tag("body", {new Tag("h1", {new Text("dummy epub content")})})};
  Nav *nav = new Nav(
      toc, new Tag("h1", {new Text("spis treści")}),
      new OrderedList({new ListItem(new Anchor("plik.xhtml", "pliczek"))}));
  XHTMLFile *nav_file = new XHTMLFile("nav.xhtml", new Tag("body", {nav}));
  PackageFile *package =
      new PackageFile("1", "tytuł", "autor", "pl", nav_file,
                      {new ManifestItem("ttl", *file)}, {new SpineItem("ttl")});
  Content content{package, {file, nav_file}};
  Epub epub{"test", meta, content};
  epub.write();
  return 0;
};
