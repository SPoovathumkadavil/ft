#pragma once

#include <filesystem>
#include <stdexcept>

namespace ft {

inline std::filesystem::path get_home_path() {
  const char *homeDirCStr = std::getenv("HOME");
  if (homeDirCStr)
    std::string homeDir = homeDirCStr;
  else
    throw std::invalid_argument("Error: HOME environment variable not set.");
  return std::filesystem::path(homeDirCStr);
}

inline std::vector<std::filesystem::path>
find_files_in_dir(const std::filesystem::path &directory_path,
                  const std::string &extension = "~") {
  std::vector<std::filesystem::path> found_files;
  if (!std::filesystem::is_directory(directory_path))
    throw std::invalid_argument("Error: Provided path is not a directory.");
  for (const auto &entry :
       std::filesystem::directory_iterator(directory_path)) {
    if (std::filesystem::is_regular_file(entry.path())) {
      if (extension != "~" && entry.path().extension() == extension) {
        found_files.push_back(entry.path());
      }
    }
  }
  return found_files;
}

const std::filesystem::path library_dir = get_home_path() / ".local/library/ft";

} // namespace ft
