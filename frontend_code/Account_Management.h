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

    // Creates a new user account and records it.
    void create_user(const std::string& user);

    // Deletes a user account and records it.
    void delete_user(const std::string& user);
};
