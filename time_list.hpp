#include "normal_list.hpp"
#include <set>

class Time_type : public Normal_type {
    public:
        ~Time_type();
        void add_task() override {}
        void view_tasks() const override {}
        void edit_task() override {}
        void delete_task() override {}
        bool operator<(const Time_type& other) const;

    protected:
        Time_type();
        Time_type(std::string *name, std::string *category, bool *completed, std::string *start_time, std::string *end_time);
        std::string get_start_time() const;
        std::string get_end_time() const;
        void set_start_time(std::string *start_time);
        void set_end_time(std::string *end_time);
        void set_name(std::string *name);
        void set_category(std::string *category);
        void set_completed(bool *completed);


        std::string *start_time;
        std::string *end_time;
    friend class Time_list;
};

struct TimePtrCompare {
    bool operator()(const Time_type* lhs, const Time_type* rhs) const;
};

class Time_list : public Time_type {
    public:
        Time_list();
        ~Time_list();
        void add_task() override;
        void view_tasks() const override;
        void edit_task() override;
        void delete_task() override;

    private:
        std::set<Time_type*, TimePtrCompare> tasks;
};