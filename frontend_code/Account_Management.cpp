#include "Account_Management.h"
#include <iostream>

Account_Management::Account_Management(Session& session, Transaction_Writer& transaction)
    : session(session), transaction(transaction) {}

void Account_Management::create_user(const std::string& user) {
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

void Account_Management::delete_user(const std::string& user) {
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

void Account_Management::list_accounts() {
    if (!session.is_logged_in() || session.get_current_user().get_user_type() != "AA") {
        std::cout << "Error: admin privileges required.\n";
        return;
    }
    for (const User& account : session.get_all_users()) {
        std::cout << account.get_username() << " " << account.get_user_type() << " " << account.get_total_credit() << "\n";
    }
}
