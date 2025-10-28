
#include "scheduler.hpp"

namespace ft {

task *scheduler::find_task(int id) {

};

void scheduler::add_task(task t, int project_id) {
  tasks.push_back(&t);
  tasks = reload_ids(tasks).first;
}

std::pair<std::vector<task *>, int> reload_ids(std::vector<task *> v,
                                               int id = 1) {
  for (int i = 0; i < v.size(); i++) {
    v.at(i)->id = id;
    id++;
    if (v.at(i)->subtasks.size() > 0) {
      std::pair<std::vector<task *>, int> inner =
          reload_ids(v.at(i)->subtasks, id);
      v.at(i)->subtasks = inner.first;
      id = inner.second;
    }
  }
  return {v, id};
}

} // namespace ft
