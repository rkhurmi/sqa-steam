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
    // Starts with nobody logged in; all_users comes from the accounts file.
    Session(Transaction_Writer& transaction, const std::vector<User>& all_users = {});

    // Logs in the named user if they exist.
    void login(const std::string& user);

    // Ends the session for the named user and records the logout.
    void logout(const std::string& user);

    // Returns a copy of the named user, or a User with an empty username if not found.
    User find_user(const std::string& user);

    // Returns true while a user is logged in.
    bool is_logged_in() const;

    // Returns true if a user is logged in and is an admin ("AA").
    bool is_admin() const;

    // Returns a copy of the logged-in user (an empty User when nobody is logged in).
    User get_current_user() const;

    // Returns the master list of accounts, so create and delete can change it.
    std::vector<User>& get_all_users();

    // Adds `amount` to the named user's total credit and session credit (also updates the logged-in copy).
    void add_credit(const std::string& user, double amount);

    // Changes the named user's total credit only (purchases and refunds); session credit is untouched.
    void adjust_credit(const std::string& user, double amount);

    // Adds a game to the named user's collection (also updates the logged-in copy).
    void add_game(const std::string& user, const std::string& game);
};
