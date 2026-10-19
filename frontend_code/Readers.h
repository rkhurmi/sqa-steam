#pragma once
#include <map>
#include <string>
#include <vector>
#include "Game.h"
#include "User.h"

// Reads users, games for sale, and owned game collections from their files.
class Readers {
private:
    std::string user_accounts_file_path;
    std::string available_games_file_path;
    std::string game_collection_file_path;

    // Removes leading and trailing whitespace from a line.
    std::string trim(const std::string& line) const;

    // Converts a money string to a double; returns 0.0 if it is not a number.
    double parse_money(const std::string& amount) const;

    // Returns a trimmed fixed-width field, or "" if the line is too short.
    std::string field(const std::string& line, size_t pos, size_t len) const;

public:
    // Stores the paths of the three input files.
    Readers(const std::string& user_accounts_file_path,
            const std::string& available_games_file_path,
            const std::string& game_collection_file_path);

    // Reads every user account from the user accounts file.
    std::vector<User> read_user_accounts();

    // Reads games for sale, grouped by seller username.
    std::map<std::string, std::vector<Game>> read_available_games();

    // Reads owned games, grouped by buyer username.
    std::map<std::string, std::vector<Game>> read_game_collection();
};
