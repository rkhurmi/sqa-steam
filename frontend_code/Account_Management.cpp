#pragma once
#include <iostream>
#include <string>
#include "Session.cpp"
#include "Transaction_Writer.cpp"

// Creates and deletes user accounts on behalf of the current session.
class Account_Management {
private:
    Session& session;
    // Shared writer, held by reference so every class logs to the same file.
    Transaction_Writer& transaction;

public:
    // Initializes with the live session and the shared writer.
    Account_Management(Session& session, Transaction_Writer& transaction)
        : session(session), transaction(transaction) {}

    // Creates a new user account and records it.
    void create_user(const std::string& user) {
        if (!session.is_logged_in() || session.get_current_user().get_user_type() != "AA") {
            std::cout << "Error: admin privileges required.\n";
            return;
        }
        if (!session.find_user(user).get_username().empty()) {
            std::cout << "Error: user already exists.\n";
            return;
        }
        std::string type;
        std::cout << "User type: ";
        std::getline(std::cin, type);
        session.get_all_users().push_back(User(user, type, 0.0));
        transaction.record_user_transaction("01", user, type, 0.0);
    }

    // Deletes a user account and records it.
    void delete_user(const std::string& user) {
        if (!session.is_logged_in() || session.get_current_user().get_user_type() != "AA") {
            std::cout << "Error: admin privileges required.\n";
            return;
        }
        std::vector<User>& users = session.get_all_users();
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].get_username() == user) {
                transaction.record_user_transaction("02", user, users[i].get_user_type(),
                                                    users[i].get_total_credit());
                users.erase(users.begin() + static_cast<long>(i));
                return;
            }
        }
        std::cout << "Error: user not found.\n";
    }
};
