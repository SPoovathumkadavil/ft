#pragma once

#define HOME std::filesystem::path(getenv("HOME"))
// #define HOME std::filesystem::path("/Users/sally")
#define DEFAULT_FT_DIR HOME / ".local/library/first.ft"
// #define DEFAULT_FT_DIR HOME / "dev/cpp/ft/first.ft"

#define PRIORITY_MULTIPLIER 10e7f

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
      : id(-1), name(""), desc(""),
        deadline(std::chrono::system_clock::now() + std::chrono::days(1)),
        duration(std::chrono::hours(1)) {}
  task(int id, std::string name, std::string desc,
       std::chrono::time_point<std::chrono::system_clock> deadline,
       std::chrono::seconds duration, bool complete, std::vector<int> child_ids)
      : id(id), name(name), desc(desc), deadline(deadline), duration(duration),
        complete(complete), child_ids(child_ids) {}
};

void create_ft(std::string path);
rapidcsv::Document load_doc(std::string path);

std::vector<task> load_tasks(rapidcsv::Document *doc);
void save_tasks(std::vector<task> *tasks, rapidcsv::Document *doc,
                std::filesystem::path path);

int get_new_id(rapidcsv::Document *doc);
task *find_task(std::vector<task> *tasks, int id);
void parse_task_tree(std::vector<task> *tasks);
double evaluate_chain_priority(task *t, std::vector<task> *tasks);
void calculate_priorities(std::vector<task> *tasks);
void sort_by_priorities(std::vector<task> *tasks);
task *earliest_deadline(std::vector<task> *tasks);
void update_task_list(std::vector<task> *tasks);

static int initial_tr_wait = 0;
void destroy_tr_instances();
void create_tr_instances(task *t);

void display_tasks(std::vector<task> *tasks, bool minimal = false);

int main(int argc, char **argv);
