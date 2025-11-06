
#include "ft.hpp"
#include "util.hpp"

#include <chrono>
#include <format>
#include <iostream>
#include <tabulate/table.hpp>

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
  double priority = PRIORITY_MULTIPLIER * (static_cast<float>(t->duration.count()) /
                    (t->deadline - std::chrono::system_clock::now()).count());
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
  std::stable_partition(tasks->begin(), tasks->end(), [&](const task& val) { return !val.complete; });
}

task *earliest_deadline(std::vector<task> *tasks) {
  task *t = nullptr;
  for (task ti : *tasks) {
    if (t == nullptr && !ti.complete) t = &ti;
    if (!ti.complete && ti.deadline < t->deadline) t = &ti;
  }
  return t;
}

void display_tasks(std::vector<task> *tasks, bool minimal) {
  tabulate::Table table;
  if (minimal) {
    table.add_row({"id", "name", "completed"});
  } else {
    table.add_row({"id", "name", "desc", "deadline", "duration", "priority", "completed"});
    for (task t : *tasks) {
      std::time_t tt = std::chrono::system_clock::to_time_t(t.deadline);
      std::tm local_tm;
      std::ostringstream oss;
      localtime_r(&tt, &local_tm);
      oss << std::put_time(&local_tm, "%m/%d/%y %H:%M:%S");
      table.add_row({
        std::to_string(t.id),
        util::wrap_paragraph(t.name, 10),
        util::wrap_paragraph(t.desc, 25),
        oss.str(),
        std::format("{:%H:%M:%S}", t.duration),
        std::to_string(t.priority),
        t.complete ? "yes" : "no"
      });
    }
  }
  std::cout << table << '\n';
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
  if (t == nullptr) return;
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
  std::string f = util::select_random_file(HOME / "dev/cpp/ft/test");
  std::string cmd = std::format(f2, s1, output);
  std::string cmd2 = std::format(f3, s2, f, f);
  std::system(cmd.c_str());
  std::system(cmd2.c_str());
  // std::cout << cmd2.c_str() << '\n';
}

int get_new_id(rapidcsv::Document *doc) {
  std::vector<int> ids = doc->GetColumn<int>("id");
  return (doc->GetRowCount() > 0)
              ? *std::max_element(ids.begin(), ids.end()) + 1
              : 1;
}

void create_ft(std::string path) {
  std::filesystem::create_directories(std::filesystem::path(path).parent_path());
  std::ofstream output(path);

  if (output.is_open()) {
      output << "id,name,desc,deadline,duration,complete,child_ids" << '\n';
      output.close();
  } else {
      std::cerr << "Error: Unable to open file " << path << '\n';
  }
}

rapidcsv::Document load_doc(std::string path) {
  if (!std::filesystem::exists(path)) {
    create_ft(path);
  }
  return rapidcsv::Document(path);
}

int main(int argc, char **argv) {

  rapidcsv::Document doc = load_doc(DEFAULT_FT_DIR);
  std::vector<task> tasks = load_tasks(&doc);
  update_task_list(&tasks);

  bool update_tr = false;

  if (argc < 2) {
    display_tasks(&tasks);
  } else {
    std::string action = argv[1];
    if (action == "list" || action == "ls") {
      display_tasks(&tasks);
    } else if (action == "add" || action == "a") {
      if (argc > 2) {
        task t = task();
        t.id = get_new_id(&doc);
        for (int i = 2; i < argc; i++) {
          std::string key = argv[i];
          std::string val = key.substr(key.find_first_of(":")+1);
          key = key.substr(0, key.find_first_of(":"));
          if (key == "name" || key == "n") {
            t.name = val;
          } else if (key == "desc") {
            t.desc = val;
          } else if (key == "deadline" || key == "dead") {
            t.deadline = util::parse_date_time(val);
          } else if (key == "duration" || key == "dur") {
            t.duration = util::parse_duration(val);
          } else if (key == "child") {
            t.child_ids = util::convert_int(util::split(val, ","));
          }
        }
        task *oet = earliest_deadline(&tasks);
        tasks.push_back(t);
        if (earliest_deadline(&tasks) != oet) update_tr = true;
      } else {
        std::cout << "name: ";
        std::string name;
        std::getline(std::cin, name);

        std::cout << "desc: ";
        std::string desc;
        std::getline(std::cin, desc);

        std::cout << "deadline [m/d/y (h:m:s)]: ";
        std::string deadline;
        std::getline(std::cin, deadline);

        std::cout << "duration [?h ?m ?s]: ";
        std::string duration;
        std::getline(std::cin, duration);

        std::cout << "children [id1,id2,...]: ";
        std::string child_ids;
        std::getline(std::cin, child_ids);

        task t = task();
        t.id = get_new_id(&doc);
        if (name != "") t.name = name;
        if (desc != "") t.desc = desc;
        if (deadline != "") t.deadline = util::parse_date_time(deadline);
        if (duration != "") t.duration = util::parse_duration(duration);
        if (child_ids != "") t.child_ids = util::convert_int(util::split(child_ids, ","));
        task *oet = earliest_deadline(&tasks);
        tasks.push_back(t);
        if (earliest_deadline(&tasks) != oet) update_tr = true;
      }
    } else if (action == "complete" || action == "c") {
      if (argc > 2) {
        int id = std::stoi(argv[2]);
        task *t = find_task(&tasks, id);
        task *oet = earliest_deadline(&tasks);
        t->complete = !t->complete;
        if (earliest_deadline(&tasks) != oet) update_tr = true;
      } else {
        std::cout << "id: ";
        std::string id;
        std::cin >> id;
        task *t = find_task(&tasks, std::stoi(id));
        task *oet = earliest_deadline(&tasks);
        t->complete = !t->complete;
        if (earliest_deadline(&tasks) != oet) update_tr = true;
      }
    } else if (action == "remove" || action == "rm") {
      if (argc > 2) {
        int id = std::stoi(argv[2]);
        task *t = find_task(&tasks, id);
        if (t == nullptr) {
          std::cout << "id not found." << '\n';
          return 1;
        }
        task *oet = earliest_deadline(&tasks);
        for (int i = 0; i < tasks.size(); i++) {
          if (tasks.at(i).id == id) tasks.erase(tasks.begin()+i);
        }
        if (earliest_deadline(&tasks) != oet) update_tr = true;
      } else {
        std::cout << "id: ";
        std::string id;
        std::getline(std::cin, id);
        if (id == "") {
          std::cout << "please input id." << '\n';
          return 1;
        }
        task *t = find_task(&tasks, std::stoi(id));
        if (t == nullptr) {
          std::cout << "id not found." << '\n';
          return 1;
        }
        task *oet = earliest_deadline(&tasks);
        for (int i = 0; i < tasks.size(); i++) {
          if (tasks.at(i).id == std::stoi(id)) tasks.erase(tasks.begin()+i);
        }
        if (earliest_deadline(&tasks) != oet) update_tr = true;
      }
    }
  }

  save_tasks(&tasks, &doc, DEFAULT_FT_DIR);

  if (update_tr) {
    destroy_tr_instances();
    create_tr_instances(earliest_deadline(&tasks));
  }
}
