// Session.cpp
// Tracks who is logged in and holds every user account for the run.
// all_users is the master list; current_user is a copy of the logged-in user,
// so every change to a user updates both.

#include "Session.h"
#include <iostream>

// Human-readable name for a two-letter user type code.
static std::string type_name(const std::string& code) {
    if (code == "AA") return "admin";
    if (code == "FS") return "full-standard";
    if (code == "BS") return "buy-standard";
    if (code == "SS") return "sell-standard";
    return code;
}

// Starts with nobody logged in.
Session::Session(Transaction_Writer& transaction, const std::vector<User>& all_users)
    : current_user(), all_users(all_users), transaction(transaction), logged_in(false) {}

// Logs a user in. Rejects a second login, a blank username and an unknown username.
void Session::login(const std::string& user) {
    if (logged_in) {
        std::cout << "ERROR: Already logged in, log out first.\n";
        return;
    }
    if (user.empty()) {
        std::cout << "ERROR: Username cannot be blank.\n";
        return;
    }
    User found = find_user(user);
    if (found.get_username().empty()) {
        std::cout << "ERROR: Username not found.\n";
        return;
    }
    current_user = found;
    logged_in = true;
    std::cout << "Login successful, welcome " << found.get_username()
              << " (" << type_name(found.get_user_type()) << ").\n";
}

// Logs the current user out and writes the 00 end-of-session line
// with the user's credit as it stands now.
void Session::logout(const std::string& user) {
    if (!logged_in || current_user.get_username() != user) {
        std::cout << "ERROR: You must log in first.\n";
        return;
    }
    transaction.record_user_transaction("00", current_user.get_username(),
                                        current_user.get_user_type(),
                                        current_user.get_total_credit());
    logged_in = false;
    current_user = User();
    std::cout << "Logout successful.\n";
}

// Returns a copy of the named user, or an empty User if not found
// (callers check get_username().empty()).
User Session::find_user(const std::string& user) {
    for (const User& u : all_users) {
        if (u.get_username() == user) return u;
    }
    return User();
}

// Getters. is_admin is true only while an admin is logged in.
bool Session::is_logged_in() const { return logged_in; }
bool Session::is_admin() const { return logged_in && current_user.get_user_type() == "AA"; }
User Session::get_current_user() const { return current_user; }
std::vector<User>& Session::get_all_users() { return all_users; }

// addcredit: adds to the user's credit and to their credit added this
// session (used for the $1000-per-session limit).
void Session::add_credit(const std::string& user, double amount) {
    for (User& u : all_users) {
        if (u.get_username() == user) {
            u.set_total_credit(u.get_total_credit() + amount);
            u.set_credit_in_session(u.get_credit_in_session() + amount);
            if (logged_in && current_user.get_username() == user) current_user = u;
            return;
        }
    }
}

// buy and refund: changes credit by `amount` (negative to take it away)
// without counting toward the addcredit limit.
void Session::adjust_credit(const std::string& user, double amount) {
    for (User& u : all_users) {
        if (u.get_username() == user) {
            u.set_total_credit(u.get_total_credit() + amount);
            if (logged_in && current_user.get_username() == user) current_user = u;
            return;
        }
    }
}

// buy: adds a game to the user's collection.
void Session::add_game(const std::string& user, const std::string& game) {
    for (User& u : all_users) {
        if (u.get_username() == user) {
            u.add_game(game);
            if (logged_in && current_user.get_username() == user) current_user = u;
            return;
        }
    }
}
