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

class ContentFile;

class ManifestItem : public XMLTag {
public:
  ManifestItem(std::string href, std::string id,
               std::string mimetype = "application/xhtml+xml");
  ManifestItem(std::string id, ContentFile &file);
};

class SpineItem : public XMLTag {
public:
  SpineItem(std::string id);
};

class PackageFile : public XMLFile {
public:
  PackageFile(std::string id, std::string title, std::string creator,
              std::string language, XHTMLAdapter *nav,
              std::vector<ManifestItem *> manifest,
              std::vector<SpineItem *> spine);
};

class ContentFile : public File {
  std::string m_content;
  std::string m_mimetype;

public:
  ContentFile(std::string name, std::string content,
              std::string mimetype = "application/xhtml+xml");
  ContentFile(XHTMLAdapter xhtml_file);
  const std::string &mimetype();
  void write();
};

class Content : public Folder {
public:
  Content(PackageFile *package, std::vector<FileSystemResource *> content);
};

class Epub : File {
  Meta m_meta;
  Content m_content;

public:
  Epub(std::string name, Meta meta, Content content);
  void write() override;
};

#endif
