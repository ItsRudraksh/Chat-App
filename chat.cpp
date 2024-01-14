#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <map>
using namespace std;
class Message
{
public:
    string sender;
    string content;
    time_t timestamp;

    Message(const string &sender, const string &content)
        : sender(sender), content(content)
    {
        timestamp = time(nullptr);
    }

    void display() const
    {
        cout << "[" << sender << " - " << ctime(&timestamp) << "] " << content << endl;
    }

    // Modified function to format the message for file storage
    string formatForFile() const
    {
        string formattedMessage = "[" + sender + " - " + ctime(&timestamp) + "] " + content;

        // Replace newline characters with a space
        for (char &c : formattedMessage)
        {
            if (c == '\n' || c == '\r')
            {
                c = ' ';
            }
        }

        return formattedMessage;
    }

    // Function to check if the message contains ASCII art
    bool isAsciiArt() const
    {
        // For simplicity, let's assume any message starting with "/ascii " is considered ASCII art
        return content.substr(0, 7) == "/ascii ";
    }

    // Function to display ASCII art message
    void displayAsciiArt() const
    {
        cout << "[" << sender << " - " << ctime(&timestamp) << "] ASCII Art Message:" << endl;
        // Extract the ASCII art part (excluding "/ascii ")
        cout << content.substr(7) << endl;
    }
};

class User
{
public:
    string username;
    string password; // Added for user authentication
    bool isConnected;

    User(const string &username, const string &password)
        : username(username), password(password), isConnected(false) {}

    void sendMessage(const string &content, vector<Message> &chatHistory) const
    {
        if (isConnected)
        {
            Message message(username, content);
            chatHistory.push_back(message);
        }
        else
        {
            cout << "Error: User " << username << " is not connected." << endl;
        }
    }
};

class ChatRoom
{
public:
    vector<User> users;
    vector<Message> chatHistory;
    map<string, string> userCredentials; // Added for user authentication

    // Added user status tracking
    map<string, bool> userStatus;

    void join(User &user, const string &password)
    {
        if (userCredentials.find(user.username) != userCredentials.end() && userCredentials[user.username] == password)
        {
            user.isConnected = true;
            users.push_back(user);

            // Set user status to online
            userStatus[user.username] = true;

            cout << user.username << " joined the chat (Online)." << endl;
        }
        else
        {
            cout << "Error: Invalid username or password." << endl;
        }
    }

    void leave(User &user)
    {
        user.isConnected = false;

        // Set user status to offline
        userStatus[user.username] = false;

        cout << user.username << " left the chat (Offline)." << endl;
    }

    void displayChatHistory() const
    {
        cout << "Chat History:" << endl;
        for (const auto &message : chatHistory)
        {
            message.display();
        }
        cout << "-------------------------" << endl;
    }

    // Added function to save chat history to a file in append mode
    void saveChatHistoryToFile() const
    {
        ofstream file("chat_history.txt", ios::app); // Open in append mode
        if (file.is_open())
        {
            for (const auto &message : chatHistory)
            {
                file << message.formatForFile() << "\n";
            }
            file.close();
            cout << "Chat history appended to chat_history.txt" << endl;
        }
        else
        {
            cout << "Error: Unable to open chat_history.txt for writing." << endl;
        }
    }

    // Modified function to append user profiles to a file
    void appendUserProfilesToFile(const User &newUser) const
    {
        ofstream file("user_profiles.txt", ios::app); // Open in append mode
        if (file.is_open())
        {
            file << newUser.username << " " << newUser.password << "\n";
            file.close();
            cout << "User profile appended to user_profiles.txt" << endl;
        }
        else
        {
            cout << "Error: Unable to open user_profiles.txt for writing." << endl;
        }
    }

    // Function to create a new user
    void createUser()
    {
        string newUsername, newPassword;

        // Input validation to ensure unique username
        do
        {
            cout << "Enter a new username: ";
            cin >> newUsername;

            if (userCredentials.find(newUsername) != userCredentials.end())
            {
                cout << "Error: Username already exists. Please choose a different username.\n";
            }
        } while (userCredentials.find(newUsername) != userCredentials.end());

        cout << "Enter a password for " << newUsername << ": ";
        cin >> newPassword;

        // Add the new user to userCredentials
        userCredentials[newUsername] = newPassword;

        // Append the new user profile to the user profiles text file
        appendUserProfilesToFile(User(newUsername, newPassword));
        cout << "User " << newUsername << " created successfully.\n";
    }
};

int main()
{
    ChatRoom chat;
    ifstream userProfilesFile("user_profiles.txt");
    if (userProfilesFile.is_open())
    {
        string username, password;
        while (userProfilesFile >> username >> password)
        {
            chat.userCredentials[username] = password;
        }
        userProfilesFile.close();
    }
    int choice;

    do
    {
        cout << "Menu:" << endl;
        cout << "1. Create User (min 2 if none)" << endl;
        cout << "2. Start Chat(any 2 people can chat)" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
        {
            cout << "Enter the number of users to create: ";
            int numUsers;
            cin >> numUsers;

            for (int i = 0; i < numUsers; ++i)
            {
                chat.createUser();
            }
            break;
        }
        case 2:
        {
            // Join the first user
            string username1, password1;
            cout << "User 1, enter your username: ";
            cin >> username1;
            cout << "Enter your password: ";
            cin >> password1;
            User user1(username1, password1);
            chat.join(user1, password1);

            // Join the second user
            string username2, password2;
            cout << "User 2, enter your username: ";
            cin >> username2;
            cout << "Enter your password: ";
            cin >> password2;
            User user2(username2, password2);
            chat.join(user2, password2);

            // Interaction loop
            while (true)
            {
                // User 1 sends a message
                string content1;
                cout << username1 << ", enter your message: ";
                cin.ignore();
                getline(cin, content1);
                user1.sendMessage(content1, chat.chatHistory);

                // User 2 sends a message
                string content2;
                cout << username2 << ", enter your message: ";
                getline(cin, content2);
                user2.sendMessage(content2, chat.chatHistory);
                if (content1 == "bye" || content2 == "bye")
                    break;
            }
            // Display chat history once at the end
            chat.displayChatHistory();

            // Display chat history once at the end
            chat.displayChatHistory();
            cout << "Chat Statistics:" << endl;
            cout << "Total Messages Sent: " << chat.chatHistory.size() << endl;

            // Save chat history and user profiles to files on exiting
            chat.saveChatHistoryToFile();

            // Leave the chat when the interaction loop ends
            chat.leave(user1);
            chat.leave(user2);
            break;
        }
        case 3:
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 3);

    return 0;
}