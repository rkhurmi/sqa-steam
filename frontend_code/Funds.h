#pragma once
#include <string>
#include "Session.h"
#include "Transaction_Writer.h"

// Handles adding credit and refunds between users.
class Funds {
private:
    // Shared writer, held by reference so every class logs to the same file.
    Transaction_Writer& transaction;
    Session& session;

public:
    // Initializes with the session and the shared writer.
    Funds(Session& session, Transaction_Writer& transaction);

    // Prompts for an amount and records it as credit added to `user`.
    void add_credit(const std::string& user);

    // Admin only. Prompts for the buyer, seller, game and amount, moves that credit
    // from the seller to the buyer, removes the game from the buyer and records it.
    void refund();
};
