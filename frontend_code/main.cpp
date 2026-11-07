/*
 * Front End - command loop
 *
 * Intention:
 *   Runs one day of front-end activity for the game distribution system.
 *   A user logs in, performs transactions (create, delete, list,
 *   listaccounts, search, sell, buy, refund, addcredit), then logs out. Each
 *   accepted transaction is written to the daily transaction file for
 *   the back end to process. list and listaccounts only print to the
 *   terminal; they are not written to the transaction file.
 *
 * Input files (read once at start-up):
 *   - user accounts file:    username, user type, and credit of every account
 *   - available games file:  every game currently for sale, its seller, and price
 *   - game collection file:  every game each user owns
 *
 * Output file:
 *   - daily transaction file: one line per accepted transaction
 *
 * Standard input / output:
 *   Commands and their fields are read from standard input, one line at
 *   a time. Prompts, results, and error messages are printed to standard
 *   output. The program stops when standard input runs out.
 *
 * How to run:
 *   g++ -std=c++17 -o distribution-system *.cpp
 *   ./distribution-system <user_accounts_file> <available_games_file> <game_collection_file> <daily_transaction_file>
 */


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
        accounts.create_user();
    } else if (command == "delete") {
        // Deleting a user also cancels every game they had for sale.
        std::string deleted = accounts.delete_user();
        if (!deleted.empty()) {
            games.remove_seller_games(deleted);
        }
    } else if (command == "list") {
        games.list_games();
    } else if (command == "listaccounts") {
        accounts.list_accounts();
    } else if (command == "search") {
        games.search();
    } else if (command == "sell") {
        games.sell(username);
    } else if (command == "buy") {
        games.buy(username);
    } else if (command == "addcredit") {
        funds.add_credit(username);
    } else if (command == "refund") {
        funds.refund();
    } else {
        std::cout << "ERROR: Unknown command.\n";
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
    Game_Transactions games(session, writer, readers.read_available_games());
    Funds funds(session, writer);

    std::string command;
    while (std::getline(std::cin, command)) {
        if (command == "login") {
            // A second login is rejected before prompting, so the next line is read as a command.
            if (session.is_logged_in()) {
                session.login("");
                continue;
            }
            std::string username = read_line("Enter username:\n");
            session.login(username);
            continue;
        }
        if (!session.is_logged_in()) {
            std::cout << "ERROR: You must log in first.\n";
            continue;
        }
        std::string current_username = session.get_current_user().get_username();
        process_transactions(command, current_username, session, accounts, games, funds);
    }
    return 0;
}
