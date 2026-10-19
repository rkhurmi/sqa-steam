#include "Readers.h"
#include <fstream>

std::string Readers::trim(const std::string& line) const {
    const char* ws = " \t\r\n";
    size_t start = line.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = line.find_last_not_of(ws);
    return line.substr(start, end - start + 1);
}

double Readers::parse_money(const std::string& amount) const {
    try {
        return std::stod(trim(amount));
    } catch (...) {
        return 0.0;
    }
}

std::string Readers::field(const std::string& line, size_t pos, size_t len) const {
    if (pos >= line.size()) return "";
    return trim(line.substr(pos, len));
}

Readers::Readers(const std::string& user_accounts_file_path,
                 const std::string& available_games_file_path,
                 const std::string& game_collection_file_path)
    : user_accounts_file_path(user_accounts_file_path),
      available_games_file_path(available_games_file_path),
      game_collection_file_path(game_collection_file_path) {}

std::vector<User> Readers::read_user_accounts() {
    std::vector<User> users;
    std::ifstream in(user_accounts_file_path);
    std::string line;
    while (std::getline(in, line)) {
        if (trim(line).empty()) continue;
        std::string name = field(line, 0, 15);
        if (name == "END") continue;
        users.push_back(User(name, field(line, 16, 2), parse_money(field(line, 19, 9))));
    }
    return users;
}

std::map<std::string, std::vector<Game>> Readers::read_available_games() {
    std::map<std::string, std::vector<Game>> games;
    std::ifstream in(available_games_file_path);
    std::string line;
    while (std::getline(in, line)) {
        if (trim(line).empty()) continue;
        std::string name = field(line, 0, 19);
        std::string seller = field(line, 20, 15);
        double price = parse_money(field(line, 36, 6));
        games[seller].push_back(Game(name, price, seller));
    }
    return games;
}

std::map<std::string, std::vector<Game>> Readers::read_game_collection() {
    std::map<std::string, std::vector<Game>> games;
    std::ifstream in(game_collection_file_path);
    std::string line;
    while (std::getline(in, line)) {
        if (trim(line).empty()) continue;
        std::string name = field(line, 0, 19);
        std::string owner = field(line, 20, 15);
        games[owner].push_back(Game(name, 0.0, ""));
    }
    return games;
}
