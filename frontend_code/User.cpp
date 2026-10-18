#pragma once
#include <string>
#include <vector>
#include "Game.cpp"

// Represents a user account and the games that user owns.
class User {
private:
    std::string username;
    std::string user_type;
    double total_credit;
    double credit_in_session;
    std::vector<Game> game_collection;

public:
    // Initializes the user's attributes; the game collection starts empty.
    User(const std::string& username = "", const std::string& user_type = "",
         double total_credit = 0.0, double credit_in_session = 0.0)
        : username(username), user_type(user_type),
          total_credit(total_credit), credit_in_session(credit_in_session) {}

    // Returns true if this user owns a game with the given name.
    bool owns_game(const std::string& user) const {
        for (const Game& g : game_collection) {
            if (g.get_name() == user) return true;
        }
        return false;
    }

    // Adds a game with the given name to this user's collection.
    void add_game(const std::string& game) {
        game_collection.push_back(Game(game, 0.0, ""));
    }

    // Accessors needed by other classes.
    std::string get_username() const { return username; }
    std::string get_user_type() const { return user_type; }
    
    double get_total_credit() const { return total_credit; }
    void set_total_credit(double credit) { total_credit = credit; }
    
    double get_credit_in_session() const { return credit_in_session; }
    void set_credit_in_session(double credit) { credit_in_session = credit; }
    
    std::vector<Game> get_game_collection() const { return game_collection; }
    void set_game_collection(const std::vector<Game>& games) { game_collection = games; }
};
