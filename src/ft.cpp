
#include "ft.hpp"
#include "util.hpp"

#include <chrono>
#include <format>
#include <iostream>

std::vector<task> load_tasks(rapidcsv::Document *doc) {
  std::vector<task> tasks;
  for (int i = 0; i < doc->GetRowCount(); i++) {
    int id = doc->GetCell<int>("id", i);
    std::string name = doc->GetCell<std::string>("name", i);
    std::string desc = doc->GetCell<std::string>("desc", i);
    std::chrono::time_point<std::chrono::system_clock> deadline(
        std::chrono::seconds(doc->GetCell<int>("deadline", i)));
    std::chrono::seconds duration(doc->GetCell<int>("duration", i));
    bool complete = doc->GetCell<std::string>("complete", i) == "true";
    std::string cids = doc->GetCell<std::string>("child_ids", i);
    tasks.push_back(task(id, name, desc, deadline, duration, complete,
                         util::convert_int(util::split(cids, ","))));
  }
  return tasks;
}

void save_tasks(std::vector<task> *tasks, rapidcsv::Document *doc,
                std::filesystem::path path) {
  for (int i = 0; i < tasks->size(); i++) {
    task t = tasks->at(i);
    doc->SetCell<int>("id", i, t.id);
    doc->SetCell<std::string>("name", i, t.name);
    doc->SetCell<std::string>("desc", i, t.desc);
    doc->SetCell<int>("deadline", i,
                      std::chrono::duration_cast<std::chrono::seconds>(
                          t.deadline.time_since_epoch())
                          .count());
    doc->SetCell<int>("duration", i, t.duration.count());
    doc->SetCell<std::string>("complete", i, t.complete ? "true" : "false");
    doc->SetCell<std::string>(
        "child_ids", i, util::join(util::convert_string(t.child_ids), ","));
  }
  for (int i = tasks->size(); i < doc->GetRowCount(); i++)
    doc->RemoveRow(i);
  doc->Save(path);
}

task *find_task(std::vector<task> *tasks, int id) {
  for (int i = 0; i < tasks->size(); i++) {
    if (tasks->at(i).id == id) {
      return &tasks->at(i);
    }
  }
  return nullptr;
}

void parse_task_tree(std::vector<task> *tasks) {
  for (int i = 0; i < tasks->size(); i++) {
    if (tasks->at(i).child_ids.size() > 0) {
      for (int j : tasks->at(i).child_ids) {
        find_task(tasks, j)->parent_ids.push_back(tasks->at(i).id);
      }
    }
  }
}

double evaluate_chain_priority(task *t, std::vector<task> *tasks) {
  double priority = static_cast<float>(t->duration.count()) /
                    (t->deadline - std::chrono::system_clock::now()).count();
  std::cout << priority << '\n';
  if (t->child_ids.size() > 0) {
    for (int i : t->child_ids) {
      priority += evaluate_chain_priority(find_task(tasks, i), tasks);
    }
  }
  return priority;
}

void calculate_priorities(std::vector<task> *tasks) {
  for (int i = 0; i < tasks->size(); i++) {
    tasks->at(i).priority = evaluate_chain_priority(&tasks->at(i), tasks);
  }
}

void sort_by_priorities(std::vector<task> *tasks) {
  std::sort(tasks->begin(), tasks->end(), [](const task &a, const task &b) {
    return a.priority > b.priority;
  });
}

task *earliest_deadline(std::vector<task> *tasks) {
  task *t = &tasks->at(0);
  for (task ti : *tasks) {
    if (ti.deadline < t->deadline) t = &ti;
  }
  return t;
}

void display_tasks(std::vector<task> *tasks) {
  for (int i = 0; i < tasks->size(); i++) {
    
  }
}

void update_task_list(std::vector<task> *tasks) {
  parse_task_tree(tasks);
  calculate_priorities(tasks);
  sort_by_priorities(tasks);
}

void destroy_tr_instances() {
  std::system("pkill timed_runner");  // would be safer to keep track of pids,
                            // but whats the fun in that
}

void create_tr_instances(task *t) {
  static constexpr char f1[] = "task due in {}m\nid {}: {}\n";
  std::string output = std::format(f1, std::chrono::duration_cast<std::chrono::minutes>(
      t->deadline-std::chrono::system_clock::now()
    ).count(), t->id, util::wrap_paragraph(t->name, 30));
  if (t->desc != "")
    output += "desc: " + util::wrap_paragraph(t->desc, 30);
  int s1 = std::chrono::duration_cast<std::chrono::seconds>(
    t->deadline-std::chrono::minutes(20)-std::chrono::system_clock::now()
  ).count();
  static constexpr char f2[] = "timed_runner {} \"alacritty_displayer '{}' 'a task is due, mister.' 'hellokitty'\" &";
  static constexpr char f3[] = "timed_runner {} \"alacritty_displayer 'RIP {}.\nYou will be missed dearly.' 'pay respects' 'default -d' && rm {}\" &";
  int s2 = std::chrono::duration_cast<std::chrono::seconds>(t->deadline-std::chrono::system_clock::now()).count();
  std::string f = util::select_random_file(HOME + "/dev/cpp/ft/test");
  std::string cmd = std::format(f2, s1, output);
  std::string cmd2 = std::format(f3, s2, f, f);
  std::system(cmd.c_str());
  std::system(cmd2.c_str());
  // std::cout << cmd2.c_str() << '\n';
}

int main(int argc, char **argv) {

  if (argc == 0) {
    
  }

  rapidcsv::Document doc(argv[1]);
  std::vector<task> tasks = load_tasks(&doc);
  update_task_list(&tasks);


  display_tasks(&tasks);


  destroy_tr_instances();
  create_tr_instances(earliest_deadline(&tasks));
}
