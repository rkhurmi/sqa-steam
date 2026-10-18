#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "Game.cpp"
#include "Transaction_Writer.cpp"

// Handles buying and selling games and tracks the games currently for sale.
class Game_Transactions {
private:
    // Shared writer, held by reference so every class logs to the same file.
    Transaction_Writer& transaction;
    std::map<std::string, std::vector<Game>> games_for_sale;

public:
    // Initializes with the shared writer and the games currently for sale.
    Game_Transactions(Transaction_Writer& transaction,
                      const std::map<std::string, std::vector<Game>>& games_for_sale = {})
        : transaction(transaction), games_for_sale(games_for_sale) {}

    // Lets `user` buy a game: prompts for the game name and seller, then records it.
    void buy(const std::string& user) {
        std::string game, seller;
        std::cout << "Game name: ";
        std::getline(std::cin, game);
        std::cout << "Seller: ";
        std::getline(std::cin, seller);

        auto it = games_for_sale.find(seller);
        if (it == games_for_sale.end()) {
            std::cout << "Error: seller has no games for sale.\n";
            return;
        }
        std::vector<Game>& list = it->second;
        for (size_t i = 0; i < list.size(); ++i) {
            if (list[i].get_name() == game) {
                transaction.record_buy(user, seller, game, list[i].get_price());
                list.erase(list.begin() + static_cast<long>(i));
                return;
            }
        }
        std::cout << "Error: game not found for that seller.\n";
    }

    // Lets `user` put a game up for sale: prompts for the name and price.
    void sell(const std::string& user) {
        std::string game, price_text;
        std::cout << "Game name: ";
        std::getline(std::cin, game);
        if (is_name_taken(game)) {
            std::cout << "Error: game name is already taken.\n";
            return;
        }
        std::cout << "Price: ";
        std::getline(std::cin, price_text);
        double price = 0.0;
        try { price = std::stod(price_text); } catch (...) {
            std::cout << "Error: invalid price.\n";
            return;
        }
        games_for_sale[user].push_back(Game(game, price, user));
        transaction.record_sell(user, game, price);
    }

    // Removes all games listed for sale by `user` (e.g. when the account is deleted).
    void remove_seller_games(const std::string& user) {
        games_for_sale.erase(user);
    }

    // Returns true if any seller already lists a game with this name.
    bool is_name_taken(const std::string& game) const {
        for (const auto& entry : games_for_sale) {
            for (const Game& g : entry.second) {
                if (g.get_name() == game) return true;
            }
        }
        return false;
    }
};
