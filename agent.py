from langchain_google_genai import ChatGoogleGenerativeAI
from langchain.memory import ConversationBufferMemory
from langchain.agents import initialize_agent
from langchain.schema import SystemMessage
import socket
from dotenv import load_dotenv
import os
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

load_dotenv()
google_api_key = os.getenv("GOOGLE_API_KEY")
os.environ["GOOGLE_API_KEY"] = google_api_key

llm = ChatGoogleGenerativeAI(model="gemini-2.0-flash-thinking-exp", temperature=0)
memory = ConversationBufferMemory(
    memory_key="chat_history",
    return_messages=True
)

initial_message = """
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

    This is the first step and is used to determine which task list the user wants to use. Return the appropriate list number in the format:

        @#Rlong#@ @#1#@   ← for normal_list  
        @#Rlong#@ @#2#@   ← for daily_list  
        @#Rlong#@ @#3#@   ← for categoried_list  
        @#Rlong#@ @#4#@   ← for time_list

    Notes:
    - This is the beginning of the entire process. Always start with a welcome message like "Welcome to the RL To-Do List" and briefly explain the available lists.
    - The value of @#the_first_input#@ is the user's response to "How's your day going?" You can use that to respond warmly and naturally.
    - The user might not respond with numbers. For example, if the user says "I want the normal one", you should still return @#Rlong#@ @#1#@ based on meaning.

    Example:
        Input:
            I’d like the regular one.
        Output:
            @#Rlong#@ @#1#@

    ---

Important:
- Always return the structured format exactly as shown.
- When a boolean is required, return 1 (true) or 0 (false) inside @#Rlong#@.
- When speaking to the user, format your response for readability using clean typography and line breaks.
- Unless the user explicitly provides open-ended input that clearly indicates multiple items. By default, assume only one item unless the user clearly specifies otherwise.
More important:
- When the I specifies a current stage for you, you should just asking the question in this stage without asking further questions or introducing unrelated content. Only the relevant task information for the specified stage should be returned or asked. For emaplem when you are "@#Rlong#@ reasoning", you don't need to ask what the name or category of the task. Just focus on the parameters you need to retrun in this stage!!!
- Do not use any \\(backslash) in your answer or the program will fuck up.
More more important:
- Definetly do not use any \\(backslash) in your answer or the program will fuck up.
"""

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


# import google.generativeai as genai

# genai.configure(api_key="你的API_KEY")

# models = genai.list_models()

# for model in models:
#     print(f"Model name: {model.name}")
#     print(f"Generation support: {'generateContent' in model.supported_generation_methods}\n")