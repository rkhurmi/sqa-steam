#include "Account_Management.h"
#include <iostream>

Account_Management::Account_Management(Session& session, Transaction_Writer& transaction)
    : session(session), transaction(transaction) {}

void Account_Management::create_user() {
    if (!session.is_admin()) {
        std::cout << "ERROR: Permission denied, create requires admin privileges.\n";
        return;
    }
    std::cout << "Enter new username:\n";
    std::string user;
    std::getline(std::cin, user);
    if (user.empty() || user.size() > 15) {
        std::cout << "ERROR: Username must be between 1 and 15 characters.\n";
        return;
    }
    if (!session.find_user(user).get_username().empty()) {
        std::cout << "ERROR: Username already exists.\n";
        return;
    }
    std::cout << "Enter user type (admin, full-standard, buy-standard, sell-standard):\n";
    std::string type_text;
    std::getline(std::cin, type_text);
    std::string code = User::code_from_type_name(type_text);
    if (code.empty()) {
        std::cout << "ERROR: Invalid user type.\n";
        return;
    }
    session.get_all_users().push_back(User(user, code, 0.0));
    transaction.record_user_transaction("01", user, code, 0.0);
    std::cout << "User " + user + " created.\n";
}

std::string Account_Management::delete_user() {
    if (!session.is_admin()) {
        std::cout << "ERROR: Permission denied, delete requires admin privileges.\n";
        return "";
    }
    std::cout << "Enter username to delete:\n";
    std::string user;
    std::getline(std::cin, user);
    if (user.empty()) {
        std::cout << "ERROR: Username cannot be blank.\n";
        return "";
    }
    if (session.get_current_user().get_username() == user) {
        std::cout << "ERROR: You cannot delete the currently logged-in user.\n";
        return "";
    }
    std::vector<User>& users = session.get_all_users();
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].get_username() == user) {
            transaction.record_user_transaction("02", user, users[i].get_user_type(),
                                                users[i].get_total_credit());
            users.erase(users.begin() + static_cast<long>(i));
            std::cout << "User " + user + " deleted.\n";
            return user;
        }
    }
    std::cout << "ERROR: Username not found.\n";
    return "";
}

void Account_Management::list_accounts() {
    if (!session.is_admin()) {
        std::cout << "ERROR: Permission denied, listaccounts requires admin privileges.\n";
        return;
    }
    for (const User& account : session.get_all_users()) {
        std::cout << account.get_username() << " " << account.get_user_type() << " "
                  << account.get_total_credit() << "\n";
    }
}
