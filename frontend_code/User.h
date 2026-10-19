#pragma once
#include <string>
#include <vector>
#include "Game.h"

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
         double total_credit = 0.0, double credit_in_session = 0.0);

    // Returns true if this user owns a game with the given name.
    bool owns_game(const std::string& user) const;

    // Adds a game with the given name to this user's collection.
    void add_game(const std::string& game);

    // Accessors needed by other classes.
    std::string get_username() const;
    std::string get_user_type() const;

    double get_total_credit() const;
    void set_total_credit(double credit);

    double get_credit_in_session() const;
    void set_credit_in_session(double credit);

    std::vector<Game> get_game_collection() const;
    void set_game_collection(const std::vector<Game>& games);
};
