// User.cpp
// One user account: username, type code (AA, FS, BS, SS), credit,
// and the games the user owns.

#include "User.h"

// Creates an account. credit_in_session tracks credit added this session,
// for the $1000-per-session addcredit limit.
User::User(const std::string& username, const std::string& user_type,
           double total_credit, double credit_in_session)
    : username(username), user_type(user_type),
      total_credit(total_credit), credit_in_session(credit_in_session) {}

// Returns true if the user already owns a game with this name.
bool User::owns_game(const std::string& game) const {
    for (const Game& owned_game : game_collection) {
        if (owned_game.get_name() == game) return true;
    }
    return false;
}

// Adds a bought game to the user's collection.
void User::add_game(const std::string& game) {
    game_collection.push_back(Game(game, 0.0, ""));
}

// Removes the first game with this name from the user's collection, if owned.
void User::remove_game(const std::string& game) {
    for (size_t i = 0; i < game_collection.size(); ++i) {
        if (game_collection[i].get_name() == game) {
            game_collection.erase(game_collection.begin() + static_cast<long>(i));
            return;
        }
    }
}

// Getters and setters.
std::string User::get_username() const { return username; }
std::string User::get_user_type() const { return user_type; }

double User::get_total_credit() const { return total_credit; }
void User::set_total_credit(double credit) { total_credit = credit; }

double User::get_credit_in_session() const { return credit_in_session; }
void User::set_credit_in_session(double credit) { credit_in_session = credit; }

std::vector<Game> User::get_game_collection() const { return game_collection; }
void User::set_game_collection(const std::vector<Game>& games) { game_collection = games; }

// Human-readable name for the user's two-letter type code.
std::string User::get_type_name() const {
    if (user_type == "AA") return "admin";
    if (user_type == "FS") return "full-standard";
    if (user_type == "BS") return "buy-standard";
    if (user_type == "SS") return "sell-standard";
    return user_type;
}

// Converts a type name typed at the create prompt ("admin", ...) to its
// two-letter code; returns "" for anything else.
std::string User::code_from_type_name(const std::string& name) {
    if (name == "admin") return "AA";
    if (name == "full-standard") return "FS";
    if (name == "buy-standard") return "BS";
    if (name == "sell-standard") return "SS";
    return "";
}
