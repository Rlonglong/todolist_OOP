#include "categoried_list.hpp"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <limits>
#include <regex>

namespace fs = std::filesystem;
using json = nlohmann::json;


Categoried_list::Categoried_list() {
    tasks = std::unordered_map<std::string, std::vector<Normal_type*>>();
    std::cout << "Do you want to delete the template.json? (0 for No, 1 for Yes): " << std::endl << ">> ";
    int *delete_template = new int();
    std::cin >> *delete_template;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if(!*delete_template) {
        return;
    }
    if (fs::exists("template.json")) {
        fs::remove("template.json");
    }
}
Categoried_list::~Categoried_list() {
    for (auto& pair : tasks) {
        for (size_t i = 0; i < pair.second.size(); ++i) {
            delete pair.second[i];
        }
    }
    tasks.clear();
}
void Categoried_list::add_task() {
    std::cout << "Do you want to add tasks with prior saving template? (0 for No, 1 for Yes): " << std::endl << ">> ";
    int *save = new int();
    std::cin >> *save;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if(*save) {
        std::ifstream infile("template.json");
        if (!infile.is_open()) {
            std::cerr << "Failed to open template.json" << std::endl;
            return;
        }

        json j;
        infile >> j;
        infile.close();

        if (j.empty()) {
            std::cout << "No templates found." << std::endl;
            return;
        }

        std::cout << "Available template categories:" << std::endl;
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::cout << "- " << it.key() << std::endl;
        }

        std::cout << "Choose a category to add tasks: " << std::endl << ">> ";
        std::string *category = new(std::string);
        getline(std::cin, *category);
        if (!j.contains(*category)) {
            std::cout << "No such category in template." << std::endl;
            return;
        }
        for (const auto& task : j[*category]) {
            std::string *name = new(std::string), *new_category = new(std::string);
            bool *completed = new(bool);
            *completed = false; // Initialize completed to false
            *name = task["name"];
            *new_category = *category;
            Normal_type *new_task = new Normal_type(name, new_category, completed);
            tasks[*category].push_back(new_task);
        }
        return;
    }

    std::string *name = new(std::string), *category = new(std::string);
    bool *completed = new(bool);
    *completed = false; // Initialize completed to false
    std::cout << "Enter task name: " << std::endl << ">> ";
    getline(std::cin, *name);
    std::cout << "Enter task category: " << std::endl << ">> ";
    getline(std::cin, *category);

    Normal_type *new_task = new Normal_type(name, category, completed);
    tasks[*category].push_back(new_task);
}
void Categoried_list::view_tasks() const {
    for (const auto& pair : tasks) {
        std::cout << "Category: " << pair.first << std::endl;
        for (size_t i = 0; i < pair.second.size(); ++i) {
            std::cout << "Task " << i << ": " << pair.second[i]->get_name() << ", completed :" << pair.second[i]->get_completed() << std::endl;
        }
    }
    std::cout << "==========================================" << std::endl;
    std::cout << "Do you want need to save template? (0 for No, 1 for Yes): " << std::endl << ">> ";
    int *save = new int();
    std::cin >> *save;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (*save) {
        std::cout << "Choose a category to save: " << std::endl;
        for (const auto& pair : tasks) {
            std::cout << pair.first << std::endl;
        }
        std::cout << ">> ";
        std::string *category = new(std::string);
        getline(std::cin, *category);
        if (tasks.find(*category) == tasks.end()) {
            std::cout << "No such category in template." << std::endl;
            return;
        }
        json j;
        std::ifstream infile("template.json");
        if (infile.is_open()) {
            infile >> j;
            infile.close();
        }
        j[*category] = json::array();
        for (const auto& task : tasks.at(*category)) {
            json task_json;
            task_json["name"] = *task->name;
            j[*category].push_back(task_json);
        }

        std::ofstream outfile("template.json");
        outfile << j.dump(4);
        outfile.close();
    }
    std::cout << "==========================================" << std::endl;
}
void Categoried_list::edit_task() {
    std::cout << "Please use the following index to edit: " << std::endl;
    int catg = 0;
    for (const auto& pair : tasks) {
        std::cout << "Category" << catg << ": " << pair.first << std::endl;
        for (size_t i = 0; i < pair.second.size(); ++i) {
            std::cout << "Task " << i << ": " << pair.second[i]->get_name() << ", completed :" << pair.second[i]->get_completed() << std::endl;
        }
        catg++;
    }
    int *catg_index = new(int);
    int *index = new(int);
    std::cout << "Enter the index of the category to edit: " << std::endl << ">> ";
    std::cin >> *catg_index;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter the index of the task to edit: " << std::endl << ">> ";
    std::cin >> *index;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    auto it = tasks.begin();
    std::advance(it, *catg_index); // Move iterator to the specified index
    if (it != tasks.end() && (*catg_index) >= 0) {
        if (*index < 0 || *index >= it->second.size()) {
            std::cout << "Invalid index." << std::endl;
            return;
        }
        auto task = it->second[*index];
        std::cout << "Editing task: " << task->get_name() << std::endl;
        std::cout << "Enter new task name: " << std::endl << ">> ";
        std::string *name = new(std::string);
        bool *completed = new(bool);
        getline(std::cin, *name);
        std::cout << "Enter new task completed status (0 for No, 1 for Yes): " << std::endl << ">> ";
        std::cin >> *completed;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        task->set_name(name);
        task->set_completed(completed);
        view_tasks();
    } else {
        std::cout << "Invalid index." << std::endl;
    }
}
void Categoried_list::delete_task() {
    std::cout << "Please use the following index to delete: " << std::endl;
    int catg = 0;
    for (const auto& pair : tasks) {
        std::cout << "Category" << catg << ": " << pair.first << std::endl;
        for (size_t i = 0; i < pair.second.size(); ++i) {
            std::cout << "Task " << i << ": " << pair.second[i]->get_name() << ", completed :" << pair.second[i]->get_completed() << std::endl;
        }
        catg++;
    }
    int *catg_index = new(int);
    int *index = new(int);
    std::cout << "Enter the index of the category to delete: " << std::endl << ">> ";
    std::cin >> *catg_index;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter the index of the task to delete: " << std::endl << ">> ";
    std::cin >> *index;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    auto it = tasks.begin();
    std::advance(it, *catg_index); // Move iterator to the specified index
    if (it != tasks.end() && (*catg_index) >= 0) {
        if (*index < 0 || *index >= it->second.size()) {
            std::cout << "Invalid index." << std::endl;
            return;
        }
        delete it->second[*index];
        it->second.erase(it->second.begin() + *index);
        if (it->second.empty()) {
            tasks.erase(it);
        }
        view_tasks();
    } else {
        std::cout << "Invalid index." << std::endl;
    }
}