#pragma once
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

std::vector<std::string*> get_parameters(const std::string& message);
void delete_vec(std::vector<std::string*>& vec);
void runServer();