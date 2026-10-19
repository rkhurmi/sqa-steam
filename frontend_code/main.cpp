#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "Account_Management.h"
#include "Funds.h"
#include "Game_Transactions.h"
#include "Readers.h"
#include "Session.h"
#include "Transaction_Writer.h"
#include "User.h"

// Prints `message` and returns the next line of standard input.
std::string read_line(const std::string& message) {
    std::cout << message;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

// Runs one command typed by the user. `username` is the logged-in user, if any.
void process_transactions(const std::string& command, const std::string& username, Session& session,
                     Account_Management& accounts, Game_Transactions& games, Funds& funds) {
    if (command == "logout") {
        session.logout(username);
    } else if (command == "create") {
        std::string new_username = read_line("New username: ");
        accounts.create_user(new_username);
    } else if (command == "delete") {
        std::string delete_username = read_line("Username to delete: ");
        accounts.delete_user(delete_username);
    } else if (command == "sell") {
        games.sell(username);
    } else if (command == "buy") {
        games.buy(username);
    } else if (command == "addcredit") {
        funds.add_credit(username);
    } else if (command == "refund") {
        std::string owner = read_line("Buyer username: ");
        std::string seller = read_line("Seller username: ");
        std::string amount_text = read_line("Refund amount: ");
        try {
            funds.refund(owner, seller, std::stod(amount_text));
        } catch (...) {
            std::cout << "Error: invalid amount.\n";
        }
    } else {
        std::cout << "Error: unknown command.\n";
    }
}

// Loads the input files, then processes commands from standard input until it runs out.
int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0]
                  << " <user_accounts_file> <available_games_file> <game_collection_file> <daily_transaction_file>\n";
        return 1;
    }

    Readers readers(argv[1], argv[2], argv[3]);
    Transaction_Writer writer(argv[4]);
    std::vector<User> users = readers.read_user_accounts();
    std::map<std::string, std::vector<Game>> owned_games = readers.read_game_collection();
    for (User& user : users) {
        user.set_game_collection(owned_games[user.get_username()]);
    }

    Session session(writer, users);
    Account_Management accounts(session, writer);
    Game_Transactions games(writer, readers.read_available_games());
    Funds funds(writer);

    std::string command;
    while (std::getline(std::cin, command)) {
        if (command == "login") {
            std::string username = read_line("Username: ");
            session.login(username, "");
            continue;
        }
        if (!session.is_logged_in()) {
            std::cout << "Error: no user is logged in.\n";
            continue;
        }
        std::string current_username = session.get_current_user().get_username();
        process_transactions(command, current_username, session, accounts, games, funds);
    }
    return 0;
}
