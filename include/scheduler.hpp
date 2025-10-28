#pragma once

#define SUBTASK_DEFAULT_PRIORITY_INCREMENT 0.15

#include <chrono>
#include <string>
#include <vector>

namespace ft {

struct task {
  int id;
  std::string name;
  std::string desc;
  std::chrono::duration<double> duration;
  std::chrono::system_clock::time_point deadline;
  double priority;
  bool completed = false;
  std::vector<task *> subtasks;
  virtual ~task() = default;
  task()
      : id(-1), name("task"), desc(""), duration(std::chrono::hours(1)),
        deadline(std::chrono::system_clock::now() + std::chrono::hours(24)) {};
  task(std::string name, std::string desc,
       std::chrono::duration<double> duration,
       std::chrono::system_clock::time_point deadline,
       std::vector<task *> subtasks = std::vector<task *>())
      : id(-1), name(name), desc(desc), duration(duration), deadline(deadline),
        subtasks(subtasks) {};
};

class scheduler {
public:
  void add_task(task t, int project_id = -1);
  void complete_task(int i);

  task *find_task(int id);
  std::vector<task *> get_tasks() { return tasks; };

private:
  void calculate_priority();
  void reorder();
  std::pair<std::vector<task *>, int> reload_ids(std::vector<task *> v,
                                                 int last_id = 1);
  std::vector<task *> tasks;
};

} // namespace ft
