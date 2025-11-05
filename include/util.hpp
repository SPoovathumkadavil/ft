#pragma once

#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>

namespace util {

inline std::vector<std::string> split(const std::string &s,
                                      const std::string &delimiter) {
  std::vector<std::string> tokens;
  size_t pos_start = 0, pos_end;
  size_t delim_len = delimiter.length();
  std::string token;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    tokens.push_back(token);
  }
  tokens.push_back(s.substr(pos_start)); // Add the last token
  return tokens;
}

inline std::string join(std::vector<std::string> v, std::string delimiter) {
  std::string s = "";
  for (int i = 0; i < v.size(); i++) {
    s += v.at(i);
    if (i == v.size() - 1)
      s += delimiter;
  }
  return s;
}

inline std::vector<int> convert_int(std::vector<std::string> a) {
  std::vector<int> v;
  for (std::string i : a) {
    if (i != "")
      v.push_back(std::stoi(i.c_str()));
  }
  return v;
}

inline std::vector<std::string> convert_string(std::vector<int> a) {
  std::vector<std::string> v;
  for (int i : a) {
    v.push_back(std::to_string(i));
  }
  return v;
}

inline std::string wrap_paragraph(const std::string& text, int maxWidth) {
    std::stringstream ss(text);
    std::string word;
    std::string wrappedText;
    std::string currentLine;

    while (ss >> word) {
        if (currentLine.empty()) {
            currentLine = word;
        } else if (currentLine.length() + 1 + word.length() <= maxWidth) {
            currentLine += " " + word;
        } else {
            wrappedText += currentLine + "\n";
            currentLine = word;
        }
    }
    wrappedText += currentLine;

    return wrappedText;
}

std::string select_random_file(const std::string& directoryPath) {
    std::vector<std::string> files;

    // Iterate through the directory and collect file paths
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (std::filesystem::is_regular_file(entry.status())) {
            files.push_back(entry.path().string());
        }
    }

    if (files.empty()) {
        return ""; // No files found
    }

    // Seed the random number generator
    unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);

    // Create a distribution for indices within the vector bounds
    std::uniform_int_distribution<int> distribution(0, files.size() - 1);

    // Get a random index
    int randomIndex = distribution(generator);

    // Return the randomly selected file path
    return files[randomIndex];
}

} // namespace util
