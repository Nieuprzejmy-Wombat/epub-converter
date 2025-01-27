#include "filesystem.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "xml.hpp"

FileSystemResource::FileSystemResource(std::string name) : m_name{name}{};

File::File(std::string name) : FileSystemResource{name}{};

Folder::Folder(std::string name, std::vector<FileSystemResource*> files) : FileSystemResource{name}, m_files{files}{};

void Folder::write() {
    std::cout << "writing " << m_name << "\n";
    system(("mkdir -p " + m_name).c_str());
    for (auto file : m_files) {
      std::cout << "writing " << file->m_name << "\n";
      file->write();
    }
};

XMLFile::XMLFile(std::string name, XMLTag* body) : File{name}, m_body{body}{};

void XMLFile::write() {
    std::ofstream stream;
    stream.open(m_name);
    stream << *this;
    stream.close();
};

std::ostream &operator<<(std::ostream &stream, XMLFile &file) {
  stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";
  stream << file.m_body->to_string();
  return stream;
}
