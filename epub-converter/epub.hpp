#ifndef EPUB_HPP
#define EPUB_HPP

#include "filesystem.hpp"

class ContainerFile : public XMLFile {
public:
  ContainerFile();
};

class Meta : public Folder {
public:
  Meta(ContainerFile* container);
};

class ManifestItem : public XMLTag{
  public:
    ManifestItem(std::string href, std::string id, std::string type);
};

class SpineItem : public XMLTag{
  public:
    SpineItem(std::string id);
};

class PackageFile : public XMLFile {
public:
  PackageFile(std::string id, std::string title,
              std::string creator, std::string language, XHTMLAdapter * nav,
              std::vector<XMLTag*> manifest, std::vector<XMLTag*> spine);
};

class ContentFile : public File {
  std::string m_content;

public:
  ContentFile(std::string name, std::string content);
  ContentFile(XHTMLAdapter xhtml_file);
  void write();
};

class Content : public Folder {
public:
  Content(PackageFile * package, std::vector<FileSystemResource*> content);
};

class Epub : File {
  Meta m_meta;
  Content m_content;

public:
  Epub(std::string name, Meta meta, Content content);
  void write() override;
};

#endif
