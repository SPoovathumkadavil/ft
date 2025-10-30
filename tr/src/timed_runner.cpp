
#include "timed_runner.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cout << "usage: timed_runner seconds_till_run command" << '\n';
    return 1;
  }
  std::chrono::time_point<std::chrono::system_clock> t =
      std::chrono::system_clock::now() +
      std::chrono::seconds(std::stoi(argv[1]));
  std::string command = argv[2];
  std::this_thread::sleep_until(t);
  std::system(command.c_str());
}
