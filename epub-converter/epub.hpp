#ifndef EPUB_HPP
#define EPUB_HPP

#include "filesystem.hpp"
#include "util.hpp"
#include <memory>

class ContainerFile : public XMLFile {
public:
  ContainerFile();
};

class Meta : public Folder {
public:
  Meta(std::shared_ptr<ContainerFile> container =
           std::make_shared<ContainerFile>());
};

class ManifestItem : public Tag {
public:
  ManifestItem(std::string id, const ContentFile &file,
               std::string properties = "");
};

class SpineItem : public Tag {
public:
  SpineItem(const ManifestItem &item, bool linear = true);
};

class Metadata : public Tag {
public:
  Metadata(std::string id, std::string title, std::string creator,
           std::string language, std::string time_modified = now());
};

class PackageFile : public XMLFile {
public:
  PackageFile(std::shared_ptr<Metadata> metadata, XHTMLFile &nav,
              std::vector<std::shared_ptr<ManifestItem>> manifest,
              std::vector<std::shared_ptr<SpineItem>> spine);
};

class Content : public Folder {
public:
  Content(std::shared_ptr<PackageFile> package,
          std::vector<std::shared_ptr<FileSystemResource>> content);
};

class Epub : File {
  Meta m_meta;
  Content m_content;

public:
  Epub(std::string path, Meta meta, Content content);
  void write() override;
  std::string contents() override;
};

#endif
