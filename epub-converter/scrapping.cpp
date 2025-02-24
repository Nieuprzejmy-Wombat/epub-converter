#include "scrapping.hpp"
#include "epub.hpp"
#include "nav.hpp"
#include <memory>

Default::Default(Metadata metadata) : m_metadata{metadata} {};

Either<std::string, Metadata> Default::operator()() {
  return new Metadata{m_metadata};
};

SingleFile::SingleFile(TextFile file,
                       std::shared_ptr<ScrappingStrat<Metadata>> metadata_strat,
                       std::shared_ptr<ScrappingStrat<Nav>> nav_strat)
    : m_file{file}, m_metadata_strat{metadata_strat}, m_nav_strat{nav_strat} {};

Either<std::string, Epub> SingleFile::operator()() {
  auto file_manifest =
      std::make_shared<ManifestItem>(m_file.filename(), m_file);

  auto nav = (*m_nav_strat)();
  if (!nav.m_success) {
    return new std::string{"nav strategy failed with message: " + *nav.m_error};
  }
  auto nav_file = std::make_shared<XHTMLFile>(
      "nav.xhtml", std::make_shared<Tag>(
                       "body", children{std::shared_ptr<Nav>{nav.m_value}}));

  auto metadata = (*m_metadata_strat)();
  if (!metadata.m_success) {
    return new std::string{"metadata strategy failed with message: " +
                           *metadata.m_error};
  }

  auto package = std::make_shared<PackageFile>(
      std::shared_ptr<Metadata>(metadata.m_value), *nav_file,
      std::vector<std::shared_ptr<ManifestItem>>{file_manifest},
      std::vector<std::shared_ptr<SpineItem>>{
          std::make_shared<SpineItem>(*file_manifest)});

  Content content{package, {std::make_shared<TextFile>(m_file), nav_file}};
  return new Epub{m_file.filename(), {}, content};
};

Either<std::string, Nav> Empty::operator()() {
  return new Nav{toc, std::make_shared<Header>("Table of contents"),
                 std::make_shared<OrderedList>()};
};
