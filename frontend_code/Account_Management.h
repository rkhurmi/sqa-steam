#pragma once
#include <string>
#include "Session.h"
#include "Transaction_Writer.h"

// Creates and deletes user accounts on behalf of the current session.
class Account_Management {
private:
    Session& session;
    // Shared writer, held by reference so every class logs to the same file.
    Transaction_Writer& transaction;

public:
    // Initializes with the live session and the shared writer.
    Account_Management(Session& session, Transaction_Writer& transaction);

    // Prompts for and creates a new user account (admin only) and records it.
    void create_user();

    // Prompts for and deletes a user account (admin only) and records it.
    // Returns the deleted username, or an empty string if nothing was deleted.
    std::string delete_user();

    // Prints every active account and its type and credit (admin only).
    void list_accounts();
};
