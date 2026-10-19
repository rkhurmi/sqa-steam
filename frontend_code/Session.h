#pragma once
#include <string>
#include <vector>
#include "Transaction_Writer.h"
#include "User.h"

// Tracks the logged-in user and all known users for one session.
class Session {
private:
    User current_user;
    std::vector<User> all_users;
    Transaction_Writer& transaction;
    bool logged_in;

public:
    Session(Transaction_Writer& transaction, const std::vector<User>& all_users = {});

    // Logs in the named user if they exist.
    void login(const std::string& user, const std::string& pw);

    // Processes the commands of a logged-in session.
    void process_transactions();

    // Ends the session for the named user and records the logout.
    void logout(const std::string& user);

    // Returns a copy of the named user, or a User with an empty username if not found.
    User find_user(const std::string& user);

    // Accessors needed by Account_Management.
    bool is_logged_in() const;
    User get_current_user() const;
    std::vector<User>& get_all_users();
};
