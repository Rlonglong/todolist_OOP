#pragma once
#include "normal_list.hpp"

class Daily_list : public Normal_list {
    public:
        Daily_list();
        ~Daily_list();

    private:
        void refresh_tasks();
        std::string *user_mail;
};