#pragma once

#include <cctype>
#include <iostream>
#include <string>
#include <vector>

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

} // namespace util
