#include <iostream>
#include "Game.h"
#include "User.h"
#include "Session.h"
#include "Readers.h"
#include "Game_Transactions.h"
#include "Transaction_Writer.h"
#include "Account_Management.h"
#include "Funds.h"

// Small demo wiring all classes around one shared Transaction_Writer.
int main() {
    Transaction_Writer writer("daily_transactions.txt");

    Readers readers("users.txt", "available_games.txt", "game_collection.txt");
    std::vector<User> users = readers.read_user_accounts();
    if (users.empty()) users.push_back(User("admin", "AA", 1000.0));  // fallback demo data

    Session session(writer, users);
    Game_Transactions games(writer, readers.read_available_games());
    Account_Management accounts(session, writer);
    Funds funds(writer);

    session.login("admin", "unused");
    session.process_transactions();
    funds.refund("alice", "bob", 12.50);
    std::cout << "Name taken? " << games.is_name_taken("Chess") << "\n";
    session.logout("admin");
    (void)accounts;

    std::cout << "Done; see daily_transactions.txt\n";
    return 0;
}
