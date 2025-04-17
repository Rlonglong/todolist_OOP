use std::env;
use std::fs;
use serde::Deserialize;
use lettre::message::{Message, SinglePart};
use lettre::transport::smtp::authentication::Credentials;
use lettre::{SmtpTransport, Transport};
use dotenv::dotenv;

#[derive(Debug, Deserialize)]
struct Task {
    name: Box<String>,
    category: Box<String>,
    completed: bool,
}

#[derive(Debug, Deserialize)]
struct TaskList {
    tasks: Vec<Box<Task>>,
}

fn load_tasks_from_file(file_path: &str) -> Result<Box<String>, Box<dyn std::error::Error>> {
    let file_content = fs::read_to_string(file_path)?;
    let raw_task_list: TaskList = serde_json::from_str(&file_content)?;

    let task_list = Box::new(TaskList {
        tasks: raw_task_list
            .tasks
            .into_iter()
            .map(|task| {
                Box::new(Task {
                    name: task.name,
                    category: task.category,
                    completed: task.completed,
                })
            })
            .collect(),
    });

    let mut plain_content = Box::new(String::from("Here's your To-Do List for today:\n\n"));

    for (i, task) in task_list.tasks.iter().enumerate() {
        let status = if task.completed { "✅" } else { "❎" };
        plain_content.push_str(&format!(
            "{:>3}. {:<3}| {:<10} |  {}\n",
            i,
            status,
            task.category,
            task.name
        ));
    }

    plain_content.push_str("\nUpdate again at 24:00.\n© 2025 RL. All rights reserved.");
    Ok(plain_content)
}

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
            // println!("Email sent successfully!");
            Ok(())
        }
        Err(e) => {
            // eprintln!("Could not send email: {:?}", e);
            Err(Box::new(e))
        }
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <receiver_email>", args[0]);
        return;
    }

    let receiver_email = Box::new(args[1].clone());
    let _ = send_email(receiver_email);
}
