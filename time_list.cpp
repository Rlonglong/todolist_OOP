#include "time_list.hpp"
#include <iostream>
#include <string>
#include <set>
#include <string>
#include <sstream>
#include <regex>
#include <iomanip>
#include <chrono>
#include <ctime>

// for type
Time_type::Time_type() {}
Time_type::Time_type(std::string *name, std::string *category, bool *completed, std::string *start_time, std::string *end_time) {
    this->name = new std::string(*name);
    this->category = new std::string(*category);
    this->completed = new bool(*completed);
    this->start_time = new std::string(*start_time);
    this->end_time = new std::string(*end_time);
}
Time_type::~Time_type() {
    delete name;
    delete category;
    delete completed;
    delete start_time;
    delete end_time;
}
std::string Time_type::get_start_time() const {
    return *start_time;
}
std::string Time_type::get_end_time() const {
    return *end_time;
}
void Time_type::set_start_time(std::string *start_time) {
    delete this->start_time;
    this->start_time = new std::string(*start_time);
}
void Time_type::set_end_time(std::string *end_time) {
    delete this->end_time;
    this->end_time = new std::string(*end_time);
}
void Time_type::set_name(std::string *name) {
    delete this->name;
    this->name = new std::string(*name);
}
void Time_type::set_category(std::string *category) {
    delete this->category;
    this->category = new std::string(*category);
}
void Time_type::set_completed(bool *completed) {
    delete this->completed;
    this->completed = new bool(*completed);
}
bool time1smallerthantime2(std::string *time1, std::string *time2) {
    // Assuming time is in YYYY/MM/DD/HH:MM format
    std::istringstream ss1(*time1);
    std::istringstream ss2(*time2);
    
    int year1, month1, day1, hour1, minute1;
    int year2, month2, day2, hour2, minute2;

    char delimiter;
    ss1 >> year1 >> delimiter >> month1 >> delimiter >> day1 >> delimiter >> hour1 >> delimiter >> minute1;

    ss2 >> year2 >> delimiter >> month2 >> delimiter >> day2 >> delimiter >> hour2 >> delimiter >> minute2;

    if (year1 < year2) return true;
    if (year1 > year2) return false;

    if (month1 < month2) return true;
    if (month1 > month2) return false;

    if (day1 < day2) return true;
    if (day1 > day2) return false;

    if (hour1 < hour2) return true;
    if (hour1 > hour2) return false;

    return minute1 < minute2;
}

bool parse_time(const std::string& input, std::tm& tm_out) {
    std::istringstream ss(input);
    ss >> std::get_time(&tm_out, "%Y/%m/%d/%H:%M");
    return !ss.fail();
}

std::chrono::system_clock::time_point tm_to_timepoint(const std::tm& t) {
    return std::chrono::system_clock::from_time_t(std::mktime(const_cast<std::tm*>(&t)));
}

bool TimePtrCompare::operator()(const Time_type* lhs, const Time_type* rhs) const {
    return *lhs < *rhs;
}

bool Time_type::operator<(const Time_type& other) const {
    std::tm parsed_tm_this = {};
    parse_time(*(this->start_time), parsed_tm_this);
    auto time = tm_to_timepoint(parsed_tm_this);
    std::tm parsed_tm_other = {};
    parse_time(*(other.start_time), parsed_tm_other);
    auto other_time = tm_to_timepoint(parsed_tm_other);
    auto now = std::chrono::system_clock::now();
    bool this_started = (time <= now);
    bool other_started = (other_time <= now);

    if (this_started && !other_started) {
        return true;
    }else if (!this_started && other_started) {
        return false;
    }else if (*completed < *other.completed) {
        return true;
    }else if (*completed > *other.completed) {
        return false;
    }
     else {
        return time1smallerthantime2(this->end_time, other.end_time);
    }
}
// for list
Time_list::Time_list() {
    tasks = std::set<Time_type*, TimePtrCompare>();
}
Time_list::~Time_list() {
    for (auto task : tasks) {
        delete task;
    }
    tasks.clear();
}

void Time_list::add_task() {
    std::string *name = new(std::string), *category = new(std::string), *start_time = new(std::string), *end_time = new(std::string);
    bool *completed = new(bool);
    *completed = false; // Initialize completed to false
    std::cout << "Enter task name: " << std::endl << ">> ";
    getline(std::cin, *name);
    std::cout << "Enter task category: " << std::endl << ">> ";
    getline(std::cin, *category);

    std::regex time_format(R"(^\d{4}/\d{2}/\d{2}/\d{2}:\d{2}$)");

    while (true) {
        std::cout << "Enter task start time: " << std::endl
                  << "(With form: yyyy/mm/dd/hh:mm)" << std::endl
                  << ">> ";
        std::cin >> *start_time;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (std::regex_match(*start_time, time_format)) {
            break;
        } else {
            std::cout << "Invalid format! Please follow yyyy/mm/dd/hh:mm format." << std::endl;
        }
    }
    while (true) {
        std::cout << "Enter task end time: " << std::endl
                  << "(With form: yyyy/mm/dd/hh:mm)" << std::endl
                  << ">> ";
        std::cin >> *end_time;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (std::regex_match(*end_time, time_format)) {
            break;
        } else {
            std::cout << "Invalid format! Please follow yyyy/mm/dd/hh:mm format." << std::endl;
        }
    }

    Time_type *new_task = new Time_type(name, category, completed, start_time, end_time);
    tasks.insert(new_task);
}

void Time_list::view_tasks() const {
    int cnt = 0;
    for (auto task : tasks) {
        std::cout << "==========================================" << std::endl;
        std::cout << "Task " << cnt << ":" << std::endl;
        std::cout << " \tTask Name: " << task->get_name() << std::endl;
        std::cout << "\tCategory: " << task->get_category() << std::endl;
        std::cout << "\tStart Time: " << task->get_start_time() << std::endl;
        std::cout << "\tEnd Time: " << task->get_end_time() << std::endl;
        std::cout << "\tCompleted: " << (task->get_completed() ? "Yes" : "No") << std::endl;
        std::cout << "==========================================" << std::endl;
        cnt++;
    }
}

void Time_list::edit_task() {
    std::string *name = new(std::string), *category = new(std::string), *start_time = new(std::string), *end_time = new(std::string);
    bool *completed = new(bool);
    std::cout << "Please use the following index to edit: " << std::endl;
    view_tasks();
    std::cout << "Enter the index of the task to edit: " << std::endl << ">> ";
    int *index = new(int);
    std::cin >> *index;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    auto it = tasks.begin();
    std::advance(it, *index); // Move iterator to the specified index
    if (it != tasks.end() && (*index) >= 0) {
        std::cout << "Editing task: " << (*it)->get_name() << std::endl;
        std::cout << "Enter new task name: " << std::endl << ">> ";
        getline(std::cin, *name);
        std::cout << "Enter new task category: " << std::endl << ">> ";
        getline(std::cin, *category);
        std::regex time_format(R"(^\d{4}/\d{2}/\d{2}/\d{2}:\d{2}$)");

        while (true) {
            std::cout << "Enter new task start time: " << std::endl
                    << "(With form: yyyy/mm/dd/hh:mm)" << std::endl
                    << ">> ";
            std::cin >> *start_time;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (std::regex_match(*start_time, time_format)) {
                break;
            } else {
                std::cout << "Invalid format! Please follow yyyy/mm/dd/hh:mm format." << std::endl;
            }
        }
        while (true) {
            std::cout << "Enter new task end time: " << std::endl
                    << "(With form: yyyy/mm/dd/hh:mm)" << std::endl
                    << ">> ";
            std::cin >> *end_time;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (std::regex_match(*end_time, time_format)) {
                break;
            } else {
                std::cout << "Invalid format! Please follow yyyy/mm/dd/hh:mm format." << std::endl;
            }
        }
        std::cout << "Enter new task completed status (0 for No, 1 for Yes): " << std::endl << ">> ";
        std::cin >> *completed;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        (*it)->set_name(name);
        (*it)->set_category(category);
        (*it)->set_start_time(start_time);
        (*it)->set_end_time(end_time);
        (*it)->set_completed(completed);
        view_tasks();
    } else {
        std::cout << "Invalid index." << std::endl;
    }
}

void Time_list::delete_task() {
    std::string *name = new(std::string), *category = new(std::string), *start_time = new(std::string), *end_time = new(std::string);
    bool *completed = new(bool);
    std::cout << "Please use the following index to delete: " << std::endl;
    view_tasks();
    std::cout << "Enter the index of the task to delete: " << std::endl << ">> ";
    int *index = new(int);
    std::cin >> *index;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    auto it = tasks.begin();
    std::advance(it, *index); // Move iterator to the specified index
    if (it != tasks.end() && (*index) >= 0) {
        delete *it; // Free memory
        tasks.erase(it); // Remove from set
        std::cout << "Task deleted." << std::endl;
        view_tasks();
    } else {
        std::cout << "Task not found." << std::endl;
    }
}