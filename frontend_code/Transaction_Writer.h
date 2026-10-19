#pragma once
#include <string>
#include <vector>

// Builds fixed-width transaction lines and writes them to the output file.
class Transaction_Writer {
private:
    std::vector<std::string> transactions;
    std::string output_file_path;

    // Rewrites the output file with every transaction recorded so far.
    void write_file();

    // Pads (or truncates) text with trailing spaces to exactly `spacing` characters.
    std::string pad_text(const std::string& line, int spacing) const;

    // Formats credit with two decimals, left-padded with zeros to `spacing` characters.
    std::string format_money(double credit, int spacing) const;

public:
    // Initializes the writer with the path of the transaction log file.
    Transaction_Writer(const std::string& output_file_path = "daily_transactions.txt");

    // Records a user-level transaction (login/logout/create/delete/add credit).
    void record_user_transaction(const std::string& code, const std::string& username,
                                 const std::string& user_type, double credit);

    // Records a refund from a seller to a game buyer.
    void record_refund(const std::string& buyer, const std::string& seller, double credit);

    // Records a game being put up for sale by its owner.
    void record_sell(const std::string& owner, const std::string& game, double credit);

    // Records a purchase of a game from a seller.
    void record_buy(const std::string& buyer, const std::string& seller,
                    const std::string& game, double credit);
};
