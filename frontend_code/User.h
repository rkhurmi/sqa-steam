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
    bool owns_game(const std::string& game) const;

    // Adds a game with the given name to this user's collection.
    void add_game(const std::string& game);

    // Removes a game with the given name from this user's collection (used by refund).
    void remove_game(const std::string& game);

    // The most credit any account may hold.
    static constexpr double MAX_CREDIT = 999999.00;

    // Getters for the username and type code.
    std::string get_username() const;
    std::string get_user_type() const;

    // Returns the readable type name, e.g. "admin" for "AA".
    std::string get_type_name() const;

    // Turns a type name ("admin", ...) into its two-letter code, or "" if unknown.
    static std::string code_from_type_name(const std::string& name);

    // The account's credit balance.
    double get_total_credit() const;
    void set_total_credit(double credit);

    // Credit added this session, checked against the $1000 addcredit limit.
    double get_credit_in_session() const;
    void set_credit_in_session(double credit);

    // The games this user owns.
    std::vector<Game> get_game_collection() const;
    void set_game_collection(const std::vector<Game>& games);
};
