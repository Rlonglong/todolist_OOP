#include "daily_list.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>
#include "json.hpp"
using json = nlohmann::json;

Daily_list::Daily_list() {
    tasks = std::vector<Normal_type*>();
    user_mail = new(std::string);
    std::cout << "Please enter your mail." << std::endl << ">> ";
    std::cin >> (*user_mail);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::thread([this]() {
        while (true) {
            auto now = std::chrono::system_clock::now();
            std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
            std::tm* now_tm = std::localtime(&now_time_t);

            if (now_tm->tm_hour == 0 && now_tm->tm_min == 32) {
                refresh_tasks();
                std::this_thread::sleep_for(std::chrono::minutes(1));
            }

            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
    }).detach();
}
Daily_list::~Daily_list() {
    for (size_t i = 0; i < tasks.size(); ++i) {
        delete tasks[i];
    }
    tasks.clear();
    delete user_mail;
}

void Daily_list::refresh_tasks() {
    json j;
    j["tasks"] = json::array();
    for (size_t i = 0; i < tasks.size(); ++i) {
        json task_json;
        task_json["name"] = *tasks[i]->name ;
        task_json["category"] = *tasks[i]->category;
        task_json["completed"] = *tasks[i]->completed;

        j["tasks"].push_back(task_json);
        *(tasks[i]->completed) = false;
    }
    std::ofstream file("todo_list.json");
    file << j.dump(4);
    file.close();
    system(("mail.exe " + (*user_mail)).c_str());
}