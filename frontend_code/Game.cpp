// Game.cpp
// A game for sale or in a user's collection: its name, price and seller.

#include "Game.h"

// Creates a game. Games in a collection have no price or seller.
Game::Game(const std::string& name, double price, const std::string& seller)
    : name(name), price(price), seller(seller) {}

// Getters (a game cannot be changed once created).
std::string Game::get_name() const { return name; }

double Game::get_price() const { return price; }

std::string Game::get_seller() const { return seller; }
