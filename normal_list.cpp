#include "normal_list.hpp"
#include <iostream>
#include <string>
#include <limits>
#include <regex>

// for type
Normal_type::Normal_type() {}
Normal_type::Normal_type(std::string *name, std::string *category, bool *completed) {
    this->name = new std::string(*name);
    this->category = new std::string(*category);
    this->completed = new bool(*completed);
}
Normal_type::~Normal_type() {
    delete name;
    delete category;
    delete completed;
}
std::string Normal_type::get_name() const {
    return *name;
}
std::string Normal_type::get_category() const {
    return *category;
}
bool Normal_type::get_completed() const {
    return *completed;
}
void Normal_type::set_name(std::string *name) {
    delete this->name;
    this->name = new std::string(*name);
}
void Normal_type::set_category(std::string *category) {
    delete this->category;
    this->category = new std::string(*category);
}
void Normal_type::set_completed(bool *completed) {
    delete this->completed;
    this->completed = new bool(*completed);
}

// for list
Normal_list::Normal_list() {
    tasks = std::vector<Normal_type*>();
}
Normal_list::~Normal_list() {
    for (size_t i = 0; i < tasks.size(); ++i) {
        delete tasks[i];
    }
    tasks.clear();
}
void Normal_list::add_task(){
    std::string *name = new(std::string), *category = new(std::string);
    bool *completed = new(bool);
    *completed = false; // Initialize completed to false
    std::cout << "Enter task name: " << std::endl << ">> ";
    getline(std::cin, *name);
    std::cout << "Enter task category: " << std::endl << ">> ";
    getline(std::cin, *category);

    Normal_type *new_task = new Normal_type(name, category, completed);
    tasks.push_back(new_task);
}
void Normal_list::view_tasks() const{
    for (size_t i = 0; i < tasks.size(); ++i) {
        std::cout << "Task " << i << ": " << *(tasks[i]->name) << ", Category: " << *(tasks[i]->category) << ", Completed: " << (*(tasks[i]->completed) ? "Yes" : "No") << std::endl;
    }
}
void Normal_list::edit_task(){
    std::cout << "Please use the following index to edit: " << std::endl;
    view_tasks();
    int *index = new(int);
    std::string *name = new(std::string), *category = new(std::string);
    bool *completed = new(bool);
    std:: cout << "Enter the index of the task to edit: " << std::endl << ">> ";
    std::cin >> *index;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (*index < 0 || *index >= tasks.size()) {
        std::cout << "Invalid index." << std::endl;
        return; // Invalid index
    }
    std::cout << "Enter new name: " << std::endl << ">> ";
    getline(std::cin, *name);
    std::cout << "Enter new category: " << std::endl << ">> ";
    getline(std::cin, *category);
    std::cout << "Enter new completed status (1 for true, 0 for false): " << std::endl << ">> ";
    std::cin >> *completed;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    tasks[*index]->set_name(name);
    tasks[*index]->set_category(category);
    tasks[*index]->set_completed(completed);
    view_tasks();
}
void Normal_list::delete_task(){
    std:: cout << "Please use the following index to delete: " << std::endl;
    view_tasks();
    std:: cout << "Enter the index of the task to delete: " << std::endl << ">> ";
    int *index = new(int);
    std::cin >> *index;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (*index < 0 || *index >= tasks.size()) {
        std::cout << "Invalid index." << std::endl;
        return; // Invalid index
    }
    delete tasks[*index];
    tasks.erase(tasks.begin() + *index);
    view_tasks();
}