#ifndef DECK_H
#define DECK_H

#include <vector>
#include "Card.h"

using namespace std;

class Deck{
private:
	vector<Card> cards;

public:
	Deck();
	void shuffleDeck();
	vector<Card> dealSix();
	void printDeck(bool singleLine);
};

//#include "Deck.cpp"
#endif	
