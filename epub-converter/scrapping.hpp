#ifndef SCRAPPING_HPP
#define SCRAPPING_HPP

#include "epub.hpp"
#include "filesystem.hpp"
#include "nav.hpp"
#include <memory>
#include <vector>

template <typename T, typename U> class Either {
public:
  bool m_success;
  union {
    T *m_error;
    U *m_value;
  };
  Either(T* error) : m_success{false}, m_error{error} {};
  Either(U* value) : m_success{true}, m_value{value} {};
};

template <typename T> class ScrappingStrat {
public:
  virtual Either<std::string, T> operator()() = 0;
};

template <typename T> class StratChain : public ScrappingStrat<T> {
  std::vector<std::shared_ptr<ScrappingStrat<T>>> m_strats;

public:
  StratChain(std::vector<std::shared_ptr<ScrappingStrat<T>>> strats)
      : m_strats{strats} {};
  StratChain() : m_strats{} {};
  virtual Either<std::string, T> operator()() override {
    for (auto strat : m_strats) {
      auto res = (*strat)();
      if (res.m_success) {
        return res;
      }
    }
    return new std::string{"none of strategies in chain were successful"};
  };
};

class Default : public ScrappingStrat<Metadata> {
public:
  Metadata m_metadata;
  Default(Metadata metadata);
  virtual Either<std::string, Metadata> operator()() override;
};

class SingleFile : public ScrappingStrat<Epub> {
  std::shared_ptr<ContentFile> m_file;
  ScrappingStrat<Metadata> &m_metadata_strat;
  ScrappingStrat<Nav> &m_nav_strat;

public:
  SingleFile(std::shared_ptr<ContentFile> file,
             ScrappingStrat<Metadata> &metadata_strat,
             ScrappingStrat<Nav> &nav_strat);
  virtual Either<std::string, Epub> operator()() override;
};

class Empty : public ScrappingStrat<Nav> {
public:
  virtual Either<std::string, Nav> operator()() override;
};

#endif
