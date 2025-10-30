
#include "ft.hpp"
#include "util.hpp"

#include <chrono>
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

void display_tasks(std::vector<task> *tasks) {
  for (int i = 0; i < tasks->size(); i++) {
    std::cout << tasks->at(i).id << " " << tasks->at(i).name << " "
              << std::chrono::duration_cast<std::chrono::seconds>(
                     tasks->at(i).deadline.time_since_epoch())
                     .count()
              << " " << tasks->at(i).priority << '\n';
  }
}

void update_task_list(std::vector<task> *tasks) {
  parse_task_tree(tasks);
  calculate_priorities(tasks);
  sort_by_priorities(tasks);
}

int main(int argc, char **argv) {
  rapidcsv::Document doc(argv[1]);
  std::vector<task> tasks = load_tasks(&doc);
  // std::vector<task> tasks;
  update_task_list(&tasks);
  display_tasks(&tasks);

  std::cout << "add? (y/n) ";
  std::string should_add_s;
  std::cin >> should_add_s;
  if (should_add_s == "y") {
    std::cout << "name: ";
    std::string name;
    std::cin >> name;
    std::cout << "duration(s): ";
    std::string d;
    std::cin >> d;
    task t = task();
    std::vector<int> ids = doc.GetColumn<int>("id");
    t.id = (doc.GetRowCount() > 0)
               ? *std::max_element(ids.begin(), ids.end()) + 1
               : 1;
    t.name = name;
    t.duration = std::chrono::seconds(std::stoi(d));
    tasks.push_back(t);
    update_task_list(&tasks);
    save_tasks(&tasks, &doc, argv[1]);
    display_tasks(&tasks);
  }
}
