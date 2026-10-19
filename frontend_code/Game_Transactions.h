#pragma once
#include <map>
#include <string>
#include <vector>
#include "Game.h"
#include "Session.h"
#include "Transaction_Writer.h"

// Handles buying and selling games and tracks the games currently for sale.
class Game_Transactions {
private:
    // Shared writer, held by reference so every class logs to the same file.
    Transaction_Writer& transaction;
    Session& session;
    std::map<std::string, std::vector<Game>> games_for_sale;

public:
    // Initializes with the session, the shared writer and the games currently for sale.
    Game_Transactions(Session& session, Transaction_Writer& transaction,
                      const std::map<std::string, std::vector<Game>>& games_for_sale = {});

    // Lets `user` buy a game: prompts for the game name and seller, then records it.
    void buy(const std::string& user);

    // Lets `user` put a game up for sale: prompts for the name and price.
    void sell(const std::string& user);

    // Prints every game currently for sale with its seller and price.
    void list_games() const;

    // Removes all games listed for sale by `user` (e.g. when the account is deleted).
    void remove_seller_games(const std::string& user);

    // Returns true if any seller already lists a game with this name.
    bool is_name_taken(const std::string& game) const;
};
