// Game_Transactions.cpp
// Game commands: buy, sell, list and search. Holds the games for sale,
// grouped by seller, plus games listed this session (pending_sales).

#include "Game_Transactions.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <iostream>

// Starts with the games read from the available games file.
Game_Transactions::Game_Transactions(Session& session, Transaction_Writer& transaction,
                                     const std::map<std::string, std::vector<Game>>& games_for_sale)
    : transaction(transaction), session(session), games_for_sale(games_for_sale) {}

// Formats an amount as dollars with two decimals, e.g. "15.00".
static std::string dollars(double amount) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << amount;
    return oss.str();
}

// Buys a game. Sell-standard accounts cannot buy. Checks in order: blank
// game name, blank seller, seller exists, game is for sale by that seller,
// not already owned, enough credit. On success the price moves from buyer
// to seller, the game joins the buyer's collection and a 04 line is written.
void Game_Transactions::buy(const std::string& user) {
    if (session.get_current_user().get_user_type() == "SS") {
        std::cout << "ERROR: Permission denied, sell-standard accounts cannot buy games.\n";
        return;
    }
    std::cout << "Enter game name:\n";
    std::string game;
    std::getline(std::cin, game);
    if (game.empty()) {
        std::cout << "ERROR: Game name cannot be blank.\n";
        return;
    }
    std::cout << "Enter seller username:\n";
    std::string seller;
    std::getline(std::cin, seller);
    if (seller.empty()) {
        std::cout << "ERROR: Seller username cannot be blank.\n";
        return;
    }
    if (session.find_user(seller).get_username().empty()) {
        std::cout << "ERROR: Seller not found.\n";
        return;
    }

    auto it = games_for_sale.find(seller);
    if (it != games_for_sale.end()) {
        for (const Game& listed : it->second) {
            if (listed.get_name() != game) continue;
            User buyer = session.find_user(user);
            if (buyer.owns_game(game)) {
                std::cout << "ERROR: You already own this game.\n";
                return;
            }
            if (buyer.get_total_credit() < listed.get_price()) {
                std::cout << "ERROR: Insufficient credit to complete purchase.\n";
                return;
            }
            session.adjust_credit(user, -listed.get_price());
            session.adjust_credit(seller, listed.get_price());
            session.add_game(user, game);
            transaction.record_buy(user, seller, game, listed.get_price());
            std::cout << "Purchased " + game + " from " + seller + " for $" + dollars(listed.get_price()) + ".\n";
            return;
        }
    }
    std::cout << "ERROR: Game not found.\n";
}

// Lists a game for sale. Buy-standard accounts cannot sell. The name must be
// 1-26 characters and not already listed; the price must be a number no greater
// than 999.99. Each field is checked right after it is entered. The game goes into
// pending_sales, so it cannot be bought until the back end processes it tomorrow.
void Game_Transactions::sell(const std::string& user) {
    if (session.get_current_user().get_user_type() == "BS") {
        std::cout << "ERROR: Permission denied, buy-standard accounts cannot sell games.\n";
        return;
    }
    std::cout << "Enter game name:\n";
    std::string game;
    std::getline(std::cin, game);
    if (game.empty()) {
        std::cout << "ERROR: Game name cannot be blank.\n";
        return;
    }
    if (game.size() > 26) {
        std::cout << "ERROR: Game name must be between 1 and 26 characters.\n";
        return;
    }
    if (is_name_taken(game)) {
        std::cout << "ERROR: Game name already exists.\n";
        return;
    }
    std::cout << "Enter price:\n";
    std::string price_text;
    std::getline(std::cin, price_text);
    double price = 0.0;
    size_t parsed = 0;
    try { price = std::stod(price_text, &parsed); } catch (...) { parsed = 0; }
    if (parsed == 0 || parsed != price_text.size() || price < 0.0) {
        std::cout << "ERROR: Invalid price format.\n";
        return;
    }
    if (price > 999.99 + 1e-9) {
        std::cout << "ERROR: Price cannot exceed $999.99.\n";
        return;
    }
    pending_sales.push_back(Game(game, price, user));
    transaction.record_sell(user, game, price);
    std::cout << "Listed " + game + " for sale at $" + dollars(price) + ".\n";
}

// Prints each game available to buy today as "<name> <price>" (games listed
// this session are not shown, since they cannot be bought until tomorrow).
void Game_Transactions::list_games() const {
    for (const auto& entry : games_for_sale) {
        for (const Game& listed_game : entry.second) {
            std::cout << listed_game.get_name() << " " << dollars(listed_game.get_price()) << "\n";
        }
    }
}

// Cancels every game a seller has for sale (used when their account is deleted).
void Game_Transactions::remove_seller_games(const std::string& user) {
    games_for_sale.erase(user);
}

// True if a game with this name is for sale or was listed this session.
bool Game_Transactions::is_name_taken(const std::string& game) const {
    for (const auto& entry : games_for_sale) {
        for (const Game& g : entry.second) {
            if (g.get_name() == game) return true;
        }
    }
    for (const Game& g : pending_sales) {
        if (g.get_name() == game) return true;
    }
    return false;
}

// Returns true if `user` has a non-empty list of games available to buy.
bool Game_Transactions::is_selling(const std::string& user) const {
    auto it = games_for_sale.find(user);
    return it != games_for_sale.end() && !it->second.empty();
}

// Prompts for a search query, then prints "Game: <name> <seller> <price>" for each
// game for sale and "User: <username> <type>" for each user whose name contains the
// query (case-sensitive). Admins who are not selling are hidden from non-admins.
// Prints "No results found." when nothing matches.
void Game_Transactions::search() {
    std::cout << "Enter search query:\n";
    std::string query;
    std::getline(std::cin, query);
    if (query.empty()) {
        std::cout << "ERROR: Search query cannot be blank.\n";
        return;
    }
    bool found = false;
    for (const auto& entry : games_for_sale) {
        for (const Game& listed_game : entry.second) {
            if (listed_game.get_name().find(query) == std::string::npos) continue;
            std::cout << "Game: " << listed_game.get_name() << " " << entry.first << " "
                      << dollars(listed_game.get_price()) << "\n";
            found = true;
        }
    }
    bool viewer_is_admin = session.is_admin();
    for (const User& account : session.get_all_users()) {
        if (account.get_username().find(query) == std::string::npos) continue;
        if (!viewer_is_admin && account.get_user_type() == "AA" &&
            !is_selling(account.get_username())) continue;
        std::cout << "User: " << account.get_username() << " " << account.get_user_type() << "\n";
        found = true;
    }
    if (!found) std::cout << "No results found.\n";
}
