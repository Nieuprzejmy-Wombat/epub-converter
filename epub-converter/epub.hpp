#ifndef EPUB_HPP
#define EPUB_HPP

#include "filesystem.hpp"

class ContainerFile : public XMLFile {
public:
  ContainerFile();
};

class Meta : public Folder {
public:
  Meta(ContainerFile *container);
};

class ManifestItem : public XMLTag {
public:
  ManifestItem(std::string id, ContentFile &file);
};

class SpineItem : public XMLTag {
public:
  SpineItem(std::string id);
};

class PackageFile : public XMLFile {
public:
  PackageFile(std::string id, std::string title, std::string creator,
              std::string language, XHTMLFile *nav,
              std::vector<ManifestItem *> manifest,
              std::vector<SpineItem *> spine);
};

class Content : public Folder {
public:
  Content(PackageFile *package, std::vector<FileSystemResource *> content);
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
