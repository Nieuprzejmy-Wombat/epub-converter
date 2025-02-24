#include "epub.hpp"
#include "filesystem.hpp"
#include "nav.hpp"
#include "tag.hpp"
#include "util.hpp"
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>

// Michaił Bachtin

ContainerFile::ContainerFile()
    : XMLFile(
          "META-INF/container.xml",
          std::make_shared<Tag>(
              "container",
              attrs{
                  {"xmlns", "urn:oasis:names:tc:opendocument:xmlns:container"},
                  {"version", "1.0"}},
              children{std::make_shared<Tag>(
                  "rootfiles",
                  children{std::make_shared<Tag>(
                      "rootfile", std::map<std::string, std::string>{
                                      {"full-path", "EPUB/package.opf"},
                                      {"media-type",
                                       "application/oebps-package+xml"}})})})) {
      };

Meta::Meta(std::shared_ptr<ContainerFile> container)
    : Folder{"META-INF", {container}} {};

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
                         std::string language, XHTMLFile &nav,
                         std::vector<std::shared_ptr<ManifestItem>> manifest,
                         std::vector<std::shared_ptr<SpineItem>> spine)
    : XMLFile(
          "EPUB/package.opf",
          std::make_shared<Tag>(
              "package",
              attrs{{"xmlns", "http://www.idpf.org/2007/opf"},
                    {"version", "3.0"},
                    {"unique-identifier", "uid"}},
              children{
                  std::make_shared<Tag>(
                      "metadata",
                      attrs{{"xmlns:dc", "http://purl.org/dc/elements/1.1/"}},
                      children{
                          std::make_shared<Tag>(
                              "dc:identifier",
                              std::map<std::string, std::string>{{"id", "uid"}},
                              children{std::make_shared<Text>(id)}),
                          std::make_shared<Tag>(
                              "dc:title",
                              children{std::make_shared<Text>(title)}),
                          std::make_shared<Tag>(
                              "dc:creator",
                              children{std::make_shared<Text>(creator)}),
                          std::make_shared<Tag>(
                              "dc:language",
                              children{std::make_shared<Text>(language)}),
                          std::make_shared<Tag>(
                              "meta", attrs{{"property", "dcterms:modified"}},
                              children{std::make_shared<Text>(now())})}),
                  std::make_shared<Tag>(
                      "manifest", upcast<std::shared_ptr<ManifestItem>,
                                         std::shared_ptr<AbstractTag>>(add(
                                      manifest, std::make_shared<ManifestItem>(
                                                    "nav", nav, true)))),
                  std::make_shared<Tag>(
                      "spine", upcast<std::shared_ptr<SpineItem>,
                                      std::shared_ptr<AbstractTag>>(spine))})) {
}

Content::Content(std::shared_ptr<PackageFile> package,
                 std::vector<std::shared_ptr<FileSystemResource>> content)
    : Folder{"EPUB",
             add(content,
                 static_cast<std::shared_ptr<FileSystemResource>>(package))} {};

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
  auto file = std::make_shared<XHTMLFile>(
      "plik.xhtml",
      std::make_shared<Tag>(
          "body",
          children{std::make_shared<Tag>(
              "h1", children{std::make_shared<Text>("dummy epub content")})}));
  auto nav = std::make_shared<Nav>(
      toc,
      std::make_shared<Tag>("h1",
                            children{std::make_shared<Text>("spis treści")}),
      std::make_shared<OrderedList>(
          std::vector<std::shared_ptr<ListItem>>{std::make_shared<ListItem>(
              std::make_shared<Anchor>("plik.xhtml", "pliczek"))}));
  auto nav_file = std::make_shared<XHTMLFile>(
      "nav.xhtml", std::make_shared<Tag>("body", children{nav}));
  auto package = std::make_shared<PackageFile>(
      "1", "tytuł", "autor", "pl", *nav_file,
      std::vector<std::shared_ptr<ManifestItem>>{
          std::make_shared<ManifestItem>("ttl", *file)},
      std::vector<std::shared_ptr<SpineItem>>{
          std::make_shared<SpineItem>("ttl")});
  Content content{package, {file, nav_file}};
  Epub epub{"test", meta, content};
  epub.write();
  return 0;
};
