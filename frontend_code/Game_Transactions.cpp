#include "Game_Transactions.h"
#include <iostream>

Game_Transactions::Game_Transactions(Transaction_Writer& transaction,
                                     const std::map<std::string, std::vector<Game>>& games_for_sale)
    : transaction(transaction), games_for_sale(games_for_sale) {}

void Game_Transactions::buy(const std::string& user) {
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

void Game_Transactions::sell(const std::string& user) {
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

void Game_Transactions::remove_seller_games(const std::string& user) {
    games_for_sale.erase(user);
}

bool Game_Transactions::is_name_taken(const std::string& game) const {
    for (const auto& entry : games_for_sale) {
        for (const Game& g : entry.second) {
            if (g.get_name() == game) return true;
        }
    }
    return false;
}
