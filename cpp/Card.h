#ifndef CARD_H
#define CARD_H

#include <string>

class Card {
private:
	int rank;
	char suit;

public:
	static const std::string SUITS;

	Card(int r, char s);
	int getRank() const;
	char getSuit() const;
	std::string toString() const;
	static Card parse(const std::string& cardStr);
};
//#include "Card.cpp"
#endif	
