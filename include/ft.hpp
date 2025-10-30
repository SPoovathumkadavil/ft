#pragma once

// read default.ft
// parse csv into task objects
//   - parent-child tree nodes
// display (virtual)
// add tasks
// complete tasks

#include "rapidcsv.hpp"

#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

struct task {
  int id;
  std::string name;
  std::string desc;
  std::chrono::time_point<std::chrono::system_clock> deadline;
  std::chrono::seconds duration;
  bool complete = 0;
  std::vector<int> child_ids = {};
  std::vector<int> parent_ids = {};
  double priority = -1;
  task()
      : id(-1), name("task"), desc("this is a task."),
        deadline(std::chrono::system_clock::now() + std::chrono::days(1)),
        duration(std::chrono::hours(1)) {}
  task(int id, std::string name, std::string desc,
       std::chrono::time_point<std::chrono::system_clock> deadline,
       std::chrono::seconds duration, bool complete, std::vector<int> child_ids)
      : id(id), name(name), desc(desc), deadline(deadline), duration(duration),
        complete(complete), child_ids(child_ids) {}
};

std::vector<task> load_tasks(rapidcsv::Document *doc);
void save_tasks(std::vector<task> *tasks, rapidcsv::Document *doc,
                std::filesystem::path path);

task *find_task(std::vector<task> *tasks, int id);
void parse_task_tree(std::vector<task> *tasks);
void calculate_priorities(std::vector<task> *tasks);
void sort_by_priorities(std::vector<task> *tasks);
void update_task_list(std::vector<task> *tasks);

static int initial_tr_wait = 0;
void destroy_tr_instances();
void create_tr_instance(task *t);

void display_tasks(std::vector<task> *tasks);

int main(int argc, char **argv);
