#pragma once
#include <string>
#include <vector>
#include "basic_task.hpp"
#include "utils.hpp"


class Normal_type : protected Basic_task {
    public:
        virtual void add_task(){};
        virtual void view_tasks() const{};
        virtual void edit_task(){};
        virtual void delete_task(){};
        ~Normal_type();
    protected:
        Normal_type();
        Normal_type(std::string *name, std::string *category, bool *completed);
        std::string get_name() const;
        std::string get_category() const;
        bool get_completed() const;
        void set_name(std::string *name);
        void set_category(std::string *category);
        void set_completed(bool *completed);

    friend class Normal_list;
    friend class Daily_list;
    friend class Categoried_list;
};

class Normal_list : public Normal_type {
    public:
        Normal_list();
        ~Normal_list();
        void add_task() override;
        void view_tasks() const override;
        void edit_task() override;
        void delete_task() override;

    private:
        std::vector<Normal_type*> tasks;
    friend class Daily_list;
};