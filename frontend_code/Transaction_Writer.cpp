// Transaction_Writer.cpp
// Builds the fixed-width lines of the daily transaction file. Every accepted
// transaction is added to the list and the whole file is rewritten, so the file
// is always up to date, even if input ends without a logout.

#include "Transaction_Writer.h"
#include <fstream>
#include <iomanip>
#include <sstream>

// Rewrites the output file with every transaction recorded so far.
void Transaction_Writer::write_file() {
    std::ofstream out(output_file_path);
    for (const std::string& line : transactions) {
        out << line << "\n";
    }
}

// Left-justifies text in a field of `spacing` characters:
// pads with spaces, or cuts it short if it is too long.
std::string Transaction_Writer::pad_text(const std::string& line, int spacing) const {
    std::string result = line.substr(0, static_cast<size_t>(spacing));
    result.append(static_cast<size_t>(spacing) - result.size(), ' ');
    return result;
}

// Formats a money value with 2 decimals, padded with leading zeros
// to `spacing` characters (e.g. 15.5 with spacing 9 -> "000015.50").
std::string Transaction_Writer::format_money(double credit, int spacing) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << credit;
    std::string text = oss.str();
    if (static_cast<int>(text.size()) < spacing) {
        text.insert(0, static_cast<size_t>(spacing) - text.size(), '0');
    }
    return text;
}

// Stores the path of the daily transaction file.
Transaction_Writer::Transaction_Writer(const std::string& output_file_path)
    : output_file_path(output_file_path) {}

// Records a create (01), delete (02), addcredit (06) or logout (00) line:
// CC UUUUUUUUUUUUUUU TT CCCCCCCCC
void Transaction_Writer::record_user_transaction(const std::string& code, const std::string& username,
                                                 const std::string& user_type, double credit) {
    transactions.push_back(pad_text(code, 2) + " " + pad_text(username, 15) + " " +
                           pad_text(user_type, 2) + " " + format_money(credit, 9));
    write_file();
}

// Records a refund (05) line:
// 05 BUYER__________ SELLER_________ CCCCCCCCC
void Transaction_Writer::record_refund(const std::string& buyer, const std::string& seller, double credit) {
    transactions.push_back("05 " + pad_text(buyer, 15) + " " + pad_text(seller, 15) + " " +
                           format_money(credit, 9));
    write_file();
}

// Records a sell (03) line. The game name field is 19 characters.
// 03 GAME_______________ SELLER_________ PPPPPP
void Transaction_Writer::record_sell(const std::string& owner, const std::string& game, double credit) {
    transactions.push_back("03 " + pad_text(game, 19) + " " + pad_text(owner, 15) + " " +
                           format_money(credit, 6));
    write_file();
}

// Records a buy (04) line. The game name field is 19 characters.
// 04 GAME_______________ SELLER_________ BUYER__________ PPPPPP
void Transaction_Writer::record_buy(const std::string& buyer, const std::string& seller,
                                    const std::string& game, double credit) {
    transactions.push_back("04 " + pad_text(game, 19) + " " + pad_text(seller, 15) + " " +
                           pad_text(buyer, 15) + " " + format_money(credit, 6));
    write_file();
}
