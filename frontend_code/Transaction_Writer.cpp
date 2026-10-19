#include "Transaction_Writer.h"
#include <fstream>
#include <iomanip>
#include <sstream>

void Transaction_Writer::write_file() {
    std::ofstream out(output_file_path);
    for (const std::string& line : transactions) {
        out << line << "\n";
    }
}

std::string Transaction_Writer::pad_text(const std::string& line, int spacing) const {
    std::string result = line.substr(0, static_cast<size_t>(spacing));
    result.append(static_cast<size_t>(spacing) - result.size(), ' ');
    return result;
}

std::string Transaction_Writer::format_money(double credit, int spacing) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << credit;
    std::string text = oss.str();
    if (static_cast<int>(text.size()) < spacing) {
        text.insert(0, static_cast<size_t>(spacing) - text.size(), '0');
    }
    return text;
}

Transaction_Writer::Transaction_Writer(const std::string& output_file_path)
    : output_file_path(output_file_path) {}

void Transaction_Writer::record_user_transaction(const std::string& code, const std::string& username,
                                                 const std::string& user_type, double credit) {
    transactions.push_back(pad_text(code, 2) + " " + pad_text(username, 15) + " " +
                           pad_text(user_type, 2) + " " + format_money(credit, 9));
    write_file();
}

void Transaction_Writer::record_refund(const std::string& buyer, const std::string& seller, double credit) {
    transactions.push_back("05 " + pad_text(buyer, 15) + " " + pad_text(seller, 15) + " " +
                           format_money(credit, 9));
    write_file();
}

void Transaction_Writer::record_sell(const std::string& owner, const std::string& game, double credit) {
    transactions.push_back("03 " + pad_text(game, 19) + " " + pad_text(owner, 15) + " " +
                           format_money(credit, 6));
    write_file();
}

void Transaction_Writer::record_buy(const std::string& buyer, const std::string& seller,
                                    const std::string& game, double credit) {
    transactions.push_back("04 " + pad_text(game, 19) + " " + pad_text(seller, 15) + " " +
                           pad_text(buyer, 15) + " " + format_money(credit, 6));
    write_file();
}
