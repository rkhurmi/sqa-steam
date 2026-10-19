#include "Game.h"

Game::Game(const std::string& name, double price, const std::string& seller)
    : name(name), price(price), seller(seller) {}

std::string Game::get_name() const { return name; }

double Game::get_price() const { return price; }

std::string Game::get_seller() const { return seller; }
