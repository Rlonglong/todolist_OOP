#pragma once
#include <unordered_map>
#include "normal_list.hpp"

class Categoried_list : public Normal_type {
    public:
        Categoried_list();
        ~Categoried_list();
        void add_task() override;
        void view_tasks() const override;
        void edit_task() override;
        void delete_task() override;

    private:
        std::unordered_map<std::string, std::vector<Normal_type*>> tasks;
};