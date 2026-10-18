#pragma once
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

// Builds fixed-width transaction lines and writes them to the output file.
class Transaction_Writer {
private:
    std::vector<std::string> transactions;
    std::string output_file_path;

    // Rewrites the output file with every transaction recorded so far.
    void write_file() {
        std::ofstream out(output_file_path);
        for (const std::string& line : transactions) {
            out << line << "\n";
        }
    }

    // Pads (or truncates) text with trailing spaces to exactly `spacing` characters.
    std::string pad_text(const std::string& line, int spacing) const {
        std::string result = line.substr(0, static_cast<size_t>(spacing));
        result.append(static_cast<size_t>(spacing) - result.size(), ' ');
        return result;
    }

    // Formats credit with two decimals, left-padded with zeros to `spacing` characters.
    std::string format_money(double credit, int spacing) const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << credit;
        std::string text = oss.str();
        if (static_cast<int>(text.size()) < spacing) {
            text.insert(0, static_cast<size_t>(spacing) - text.size(), '0');
        }
        return text;
    }

public:
    // Initializes the writer with the path of the transaction log file.
    Transaction_Writer(const std::string& output_file_path = "daily_transactions.txt")
        : output_file_path(output_file_path) {}

    // Records a user-level transaction (login/logout/create/delete/add credit).
    void record_user_transaction(const std::string& code, const std::string& username,
                                 const std::string& user_type, double credit) {
        transactions.push_back(pad_text(code, 2) + " " + pad_text(username, 15) + " " +
                               pad_text(user_type, 2) + " " + format_money(credit, 9));
        write_file();
    }

    // Records a refund from a seller to a game owner.
    void record_refund(const std::string& owner, const std::string& seller, double credit) {
        transactions.push_back("05 " + pad_text(owner, 15) + " " + pad_text(seller, 15) + " " +
                               format_money(credit, 9));
        write_file();
    }

    // Records a game being put up for sale by its owner.
    void record_sell(const std::string& owner, const std::string& game, double credit) {
        transactions.push_back("03 " + pad_text(game, 19) + " " + pad_text(owner, 15) + " " +
                               format_money(credit, 6));
        write_file();
    }

    // Records a purchase of a game from a seller.
    void record_buy(const std::string& owner, const std::string& seller,
                    const std::string& game, double credit) {
        transactions.push_back("04 " + pad_text(game, 19) + " " + pad_text(seller, 15) + " " +
                               pad_text(owner, 15) + " " + format_money(credit, 6));
        write_file();
    }
};
