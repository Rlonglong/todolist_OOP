#include "utils.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <windows.h>

std::string send_to_langchain(const std::string& message) {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in server;
    char buffer[4096] = {0};

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return "WSAStartup failed.";
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return "Socket creation failed.";
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(65432);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (SOCKADDR*)&server, sizeof(server)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        return "Connection failed.";
    }

    send(sock, message.c_str(), message.size(), 0);
    recv(sock, buffer, sizeof(buffer), 0);

    closesocket(sock);
    WSACleanup();

    return std::string(buffer);
}

void trim(std::string* str) {
    if (!str || str->empty()) return;

    size_t start = 0;
    while (start < str->length() && std::isspace((*str)[start])) {
        ++start;
    }

    size_t end = str->length();
    while (end > start && std::isspace((*str)[end - 1])) {
        --end;
    }

    *str = str->substr(start, end - start);
}

std::vector<std::string*> get_parameters(const std::string& message) {
    std::string *response = new(std::string);
    std::string *first_input = new(std::string);
    std::getline(std::cin, *first_input);
    *first_input = message + " @#" + *first_input + "#@";
    *response = send_to_langchain(*first_input);
    trim(response);

    delete first_input;
    while (true) {
        if ((*response).find("@#Rlong#@") == 0) {
            std::regex pattern(R"(@#(.*?)#@)");
            std::sregex_iterator iter((*response).begin(), (*response).end(), pattern);
            std::sregex_iterator end;

            std::vector<std::string*> parameters;
            for (++iter; iter != end; ++iter) {
                std::string *parameter = new(std::string);
                *parameter = (*iter)[1];
                parameters.push_back(parameter);
            }
            delete response;
            return parameters;
        }
        else {
            std::cout << *response << std::endl << ">> ";
            std::string *input = new(std::string); 
            std::getline(std::cin, *input);
            *response = send_to_langchain(*input);
            trim(response);
            delete input;
        }
    }
}

void delete_vec(std::vector<std::string*>& vec) {
    for (auto s : vec) {
        delete s;
    }
    vec.clear();
}


void runServer() {
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!CreateProcessA(
        NULL,
        (LPSTR)"agent.exe",
        NULL,
        NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si,
        &pi))
    {
        std::cerr << "Failed to start server.exe\n";
        return;
    }


    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}