#include "User.h"

User::User(const std::string& username, const std::string& user_type,
           double total_credit, double credit_in_session)
    : username(username), user_type(user_type),
      total_credit(total_credit), credit_in_session(credit_in_session) {}

bool User::owns_game(const std::string& user) const {
    for (const Game& g : game_collection) {
        if (g.get_name() == user) return true;
    }
    return false;
}

void User::add_game(const std::string& game) {
    game_collection.push_back(Game(game, 0.0, ""));
}

std::string User::get_username() const { return username; }
std::string User::get_user_type() const { return user_type; }

double User::get_total_credit() const { return total_credit; }
void User::set_total_credit(double credit) { total_credit = credit; }

double User::get_credit_in_session() const { return credit_in_session; }
void User::set_credit_in_session(double credit) { credit_in_session = credit; }

std::vector<Game> User::get_game_collection() const { return game_collection; }
void User::set_game_collection(const std::vector<Game>& games) { game_collection = games; }
