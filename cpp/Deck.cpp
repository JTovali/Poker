// Deck.cpp

#include <algorithm>
#include <random>
#include <iostream>

#include "Deck.h"

using namespace std;
	
// deck constructor
Deck::Deck() {
	const char suits[4] = {'D','C','H','S'};
	for (int i = 0; i < 4; i++) {
		char suit = suits[i];
		for (int rank = 2; rank <= 14; rank++) {
			cards.push_back(Card(rank, suit));
		}
	}
}

// method to shuffle deck
void Deck::shuffleDeck() {
	random_device rd; // initialize engine
	mt19937 gen(rd()); // seed generator for random numbers
	shuffle(cards.begin(), cards.end(), gen);
}	

// deal six cards
vector<Card> Deck::dealSix() {
	vector<Card> hand;
	hand.reserve(6);
	for (int i = 0; i < 6; i++) {
		hand.push_back(cards.front());
		cards.erase(cards.begin());
	}
	return hand;
}
	
void Deck::printDeck(bool singleLine) {
	for (int i = 0; i < cards.size(); i++) {
		cout << cards[i].toString();
		bool endOfRow = !singleLine && ((i + 1) % 13 == 0);
			
		if (endOfRow) {
			cout << "\n";
		} else if (i + 1 < cards.size()) {
			cout << " ";
		}
	}
	cout << "\n";
} 	
