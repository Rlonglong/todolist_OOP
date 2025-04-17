#include <iostream>
#include <string>
#include <thread>
#include "main.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
using namespace std;


int main() {
    std::thread([]() {
        runServer();
    }).detach();
    std::this_thread::sleep_for(std::chrono::seconds(10));

    cout << "RL to_do_list!!! How is going on today?" << endl << ">> ";
    Normal_type *todolist;
    vector<std::string*> parameters;
    parameters = get_parameters("@#Rlong#@ choosing_list");
    int num = std::stoi(*(parameters[0]));
    switch (num) {
        case 1:
            todolist = new Normal_list();
            break;
        case 2:
            todolist = new Daily_list();
            break;
        case 3:
            todolist = new Categoried_list();
            break;
        case 4:
            todolist = new Time_list();
            break;
        default:
            todolist = new Normal_list();
    }
    delete_vec(parameters);
    
    while(1) {
        int *choice = new(int);
        cout << "1. Add Task" << endl;
        cout << "2. View Tasks" << endl;
        cout << "3. Edit Task" << endl;
        cout << "4. Delete Task" << endl;
        cout << "5. Clear" << endl;
        cout << "6. Exit" << endl;
        cout << "!! Please select an option(1-6) or directly say what you want to do:" << endl;
        cout << ">> ";
        parameters = get_parameters("@#Rlong#@ reasoning");
        // for (int i = 0; i < parameters.size(); i++) {
        //     cout << "[" << *(parameters[i]) << "]" << endl;
        // }
        for (int i = 0; i < parameters.size(); i++) {
            *choice = std::stoi(*(parameters[i]));
            switch (*choice) {
                case 1:
                    todolist->add_task();
                    break;
                case 2:
                    todolist->view_tasks();
                    break;
                case 3:
                    todolist->edit_task();
                    break;
                case 4:
                    todolist->delete_task();
                    break;
                case 5:
                    system("cls"); 
                    // or use system("clear") on Unix/Linux systems
                    cout << "1. Add Task" << endl;
                    cout << "2. View Tasks" << endl;
                    cout << "3. Edit Task" << endl;
                    cout << "4. Delete Task" << endl;
                    cout << "5. Clear" << endl;
                    cout << "6. Exit" << endl;
                    break;
                case 6:
                    delete todolist; // Free memory
                    return 0; // Exit the program
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        }
        delete_vec(parameters);
        delete choice;
    }
}
