#pragma once
#include <string>

// Represents a game listed for sale or owned by a user.
class Game {
private:
    std::string name;
    double price;
    std::string seller;

public:
    // Initializes the game's attributes.
    Game(const std::string& name = "", double price = 0.0, const std::string& seller = "");

    // Returns the game's name.
    std::string get_name() const;

    // Returns the game's price.
    double get_price() const;

    // Returns the username of the game's seller.
    std::string get_seller() const;
};
