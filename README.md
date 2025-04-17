# Overview
This project is implemented using Object-Oriented Programming (OOP), and I use a Gemini AI agent to help convert natural language into structured parameters for tasks. The class inheritance and structure are designed to be extendable and modular, so more features and agent capabilities can be added in the future.:
![class_Inheritance](https://github.com/user-attachments/assets/e6e73d07-675a-4c96-b0ff-f0c13f760f3b)


# How to use?
It's just a To-Do List, so I thought it would be easy to understand what you need to do.  
My code will print prompts to guide your input, or you can directly ask the AI agent what it can do.
## Prior Work
In this project, I send emails and use the Gemini AI agent. 
Therefore, before running the project, you need to add the required API keys to the `.env` file.  
There are three things you need to include:
1. SENDER_EMAIL=YOUR_EMAIL
2. SENDER_PASSWARD=MAIL_API_PASSWORD
3. GOOGLE_API_KEY=YOUR_GOOGLE_GEMINI_API

> Notice that you cannot add any blank around the equal sign.
## Demo Viedo
You can see the example here:
https://github.com/user-attachments/assets/97370f8e-c8ca-479a-b4af-651c16b53f95





# How I implement these functions?
This implementation focuses on OOP, so I did not focus on developing application features, but rather on how to use inheritance and implement overloaded functions.   Therefore, all lists focus on the same four functions: `add_task`, `view_tasks`, `edit_task`, and `delete_task`
## Noraml list
This list is just a normal one, which only save three parameters `name`, `category`, `completed`. And the starcture would be
```cpp=
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
```
* ```add_task()``` : It just ```push_back``` the new input into the vector.
* ```view_tasks()``` : Just have a traversal of the vector and print it out.
* ```edit_task()``` : I would show all of tasks existing and let the user to edit the task by choosing index. (I would print error and stop this operation if the index is invaild)
* ```delete_task()``` : Similarly to ```edit_task()```, show and then let user to choose which index to delete.


## Daily List
It's nearly the same as the normal list. However, this one is used for daily tasks, which will refresh their status at 24:00 and send an email to the user to remind them of the day's completion.  
Since most of the functionality is inherited from the normal list, the composition is very simple:
```cpp=
class Daily_list : public Normal_list {
    public:
        Daily_list();
        ~Daily_list();

    private:
        void refresh_tasks();
        std::string *user_mail;
};
```
The four functions are the same as in the normal list, but this list has a new function called `refresh_tasks()` and a new parameter `user_mail` to record the user's email, which is updated in the constructor.  
In order to achieve automatic refreshing at 24:00, I check whether `tm_hour == 0` and `tm_min == 0` every minute. To avoid blocking the thread with sleep, I perform this in another thread, like:
```cpp=
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

            if (now_tm->tm_hour == 0 && now_tm->tm_min == 0) {
                refresh_tasks();
                std::this_thread::sleep_for(std::chrono::minutes(1));
            }

            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
    }).detach();
}
```
And in the ```refresh_tasks()```, I record the status of every item's completed into a .json file so that I can perform the executable file to send email according this.
```cpp=
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
```
And the executable file ```mail.exe``` would work like:
```rust=
fn send_email(recipient_email: Box<String>) -> Result<(), Box<dyn std::error::Error>> {
    dotenv().ok();

    let sender_email = Box::new(env::var("SENDER_EMAIL")?);
    let sender_password = Box::new(env::var("SENDER_PASSWARD")?);

    let plain_content = load_tasks_from_file("todo_list.json")?;
    let text_part = SinglePart::plain(*plain_content.clone());

    let email = Message::builder()
        .from(sender_email.parse()?)
        .to(recipient_email.parse()?)
        .subject("Daily list!!!")
        .singlepart(text_part)?;

    let creds = Credentials::new(sender_email.to_string(), sender_password.to_string());
    let mailer = SmtpTransport::relay("smtp.gmail.com")?
        .credentials(creds)
        .build();

    match mailer.send(&email) {
        Ok(_) => {
            println!("Email sent successfully!");
            Ok(())
        }
        Err(e) => {
            eprintln!("Could not send email: {:?}", e);
            Err(Box::new(e))
        }
    }
}
```

## Time List
In this list, what I want to achieve is a list that automatically sorts tasks by the time they are added.   To do this, I inevitably had to introduce the parameters `start_time`, `end_time`, and override the `operator<` to compare two objects of this type.  
The comparison follows this order:
1. Whether the task has started  
2. Whether the task is completed  
3. The end time  

In order to sort automatically, I use set and design the cmp by muself. The ```Time_list``` would be looked like:
```cpp=
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

bool TimePtrCompare::operator()(const Time_type* lhs, const Time_type* rhs) const {
    return *lhs < *rhs;
}

bool Time_type::operator<(const Time_type& other) const {
    // ...(There are sth are omitted)

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
```

In the four function, four functions share the same key idea - the biggest difference is adding ```start_time``` and ```end_time```. The way to format these two parameters is using ```regex```, as below:
```cpp=
// Assuming time is in YYYY/MM/DD/HH:MM format
std::regex time_format(R"(^\d{4}/\d{2}/\d{2}/\d{2}:\d{2}$)");
std::regex_match(*start_time, time_format)
```

## Categorzied List
In this list, I want the category to dominate everything. It will display tasks with category classification and allow adding/saving tasks using a template category.  
To achieve this more easily, I changed the container to `unordered_map`. By the way, I used `unordered_map` because I don't believe the order should be sorted alphabetically. Instead, I want to maintain the original order based on user input.
```cpp=
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
```

Since the container changed, there are little bit different:
1. ```add_task()``` : Users can choose to add a task either by saving a template or by inputting the task manually, just like in normal mode. In template mode, I display all available categorized templates by reading a `.json` file and let the user choose the index of the category template they wish to add. In normal mode, it's simply a regular input: the key is the input category, and the value is the input task name and its completion status.
2. ```view_task()``` : Displays all tasks organized by category. Users also have the option to save a category as a template, which will be saved into a `.json` file and later used by `add_task()`.
3. ```edit_task()``` : Provides the index of categories and the index of tasks within each category, allowing the user to choose which task they want to edit.
4. ```delete_task()``` : Offers the index of categories and the index of tasks within each category, allowing the user to choose which task they want to delete.

It's a part of reading. And saving part would be similar so I ignore it.
```cpp=
json j;
infile >> j;
infile.close();

if (j.empty()) {
    std::cout << "No templates found." << std::endl;
    return;
}

std::cout << "Available template categories:" << std::endl;
for (auto it = j.begin(); it != j.end(); ++it) {
    std::cout << "- " << it.key() << std::endl;
}

std::cout << "Choose a category to add tasks: " << std::endl << ">> ";
std::string *category = new(std::string);
getline(std::cin, *category);
if (!j.contains(*category)) {
    std::cout << "No such category in template." << std::endl;
    return;
}
for (const auto& task : j[*category]) {
    std::string *name = new(std::string), *new_category = new(std::string);
    bool *completed = new(bool);
    *completed = false; // Initialize completed to false
    *name = task["name"];
    *new_category = *category;
    Normal_type *new_task = new Normal_type(name, new_category, completed);
    tasks[*category].push_back(new_task);
}
```

## AI agent
I use lang-chain here to convert natural language into the paramters I need. So user can easily operate this without a limited input. What I use is Gemini's model, so you have to do the [prior work](https://hackmd.io/JIawdVuKRlm1bhdTgSn2hA#Prior-Work) before you use it.

### Prompt to the agent
I use the agent in the lang-chain, so that I can have some prompt to it before user using it. And there is a part of them:
```
You are a specialized virtual assistant designed to help users convert natural language into structured parameters for a To-Do List application.

Your primary role is to:
- Understand user input
- Extract required parameters
- Return values in a precise, structured format

This To-Do List application allows users to manage different types of task lists. Each task or operation may require specific input parameters.

Once you have gathered enough valid information, return the result in this exact format:
    @#Rlong#@ @#parameter_value1#@ @#parameter_value2#@ ...

⚠️ Important Rules:
- Only use `@#Rlong#@` when you are returning parameter values.
- Do NOT include `@#Rlong#@` in explanations, greetings, or instructions.
- Do NOT add any space inside the tag (e.g., `@#parameter_value#@`) or around them.
- Do NOT use extra or misplaced spaces in the output string.

Input format:
- The `{input}` will either be a user reply or a current stage prompt (which starts with `@#Rlong#@`) and includes `@#the_first_input#@` to represent the user's initial message for that stage.

Your job is to respond accordingly based on that stage prompt.

Summary:
✅ Use `@#Rlong#@` only for returning structured parameter values  
✅ Ask clear questions when input is missing or incorrect  
✅ Never add extra spaces inside the return format  
✅ Guide the user smoothly through each step  

current stage prompt:
    1. current stage prompt: '@#Rlong#@ reasoning @#the_first_input#@'
    There 6 following operations:
        (1) add task
        (2) view task
        (3) edit task
        (4) delete task
        (5) clear
        (6) exit
    This stage is used to understand the user's intent. Your goal is to identify which of the 6 operations (numbered 1–6) the user wants to perform. You should return the results in the following format:

        @#Rlong#@ @#1#@ @#2#@ ...

    Return multiple operation codes if the input involves more than one operation. Also, remember the parameters provided by the user for each operation. When required, you will return them to me along with the operation code.

    Note:
    - Users may describe their intentions using natural language instead of numbers. You should still determine the corresponding operation codes.
    - If the user says they want to perform an operation multiple times (e.g., "I want to add 3 tasks"), you should ask for the necessary parameters first (e.g., "What are the tasks?"). Once you get the parameters, return the full result with repetitions like:

        @#Rlong#@ @#3#@ @#1#@ @#1#@ @#1#@

    (Meaning: 3 additions and 1 deletion.)

    Example:
        Input:
            @#Rlong#@ reasoning @#I want to delete the second task. And I also want to add some new tasks.#@
        You should ask(Because he/she says some tasks and you cannot know how many tasks he/she needs. If he says "two tasks", then you don't need to ask this question.):
            How many tasks would you like to add?
        The user might input like:
            three tasks please
        Then you should return:
            @#Rlong#@ @#3#@ @#3#@ @#1#@ @#1#@ @#1#@

    ---

    2. current stage prompt: '@#Rlong#@ choosing_list @#the_first_input#@'
    ...(The following is omitted)
```
Since there are two main parts of the response—one is returning the parameters I requested, and the other is the response to the user, which is meant to guide them by providing the required parameters—I ask the AI agent to return `#@Rlong@#` when it's the parameters for me.

### AI Agent Communication
In order for the AI agent to remember the conversation with the user, so it doesn't ask the same questions the user has already answered, I want the AI agent's execution to continue running without stopping. Therefore, I use a separate thread to run it and connect to it via a socket, sending messages with localhost in my main function.
```python=
# AI part
memory.chat_memory.add_message(SystemMessage(content=initial_message))

agent = initialize_agent(
    tools=[],
    llm=llm,
    agent="chat-conversational-react-description",
    memory=memory,
    verbose=True
)


server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(('localhost', 65432))
server_socket.listen(1)
print("LangChain Agent Listening...")

while True:
    conn, addr = server_socket.accept()
    with conn:
        print(f"Connected by {addr}")
        while True:
            data = conn.recv(4096)
            if not data:
                break
            message = data.decode()
            response = agent.run(message)
            conn.sendall(response.encode())
```

```cpp=
string send_to_langchain(const string& message) {
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

    return string(buffer);
}

int main() {
    thread([]() {
        runServer();
    }).detach();
    this_thread::sleep_for(chrono::seconds(10));
    // ...(The following is omitted)
    
}
```

