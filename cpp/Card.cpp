// Card.cpp

#include "Card.h"
#include <string>

		
const std::string Card::SUITS = "DCHS";


// constructor
Card::Card(int r, char s) {
	rank = r;
	suit = s;
}

// getters
int Card::getRank() const {
	return rank;
}
	
char Card::getSuit() const {
	return suit;
}

// convert to the poker rank. Number into the assign ranks - for gamemode 1 "AD","10S"
std::string Card::toString() const {
	std::string rankStr;
		
	switch (rank) {
		case 14: rankStr = "A"; break;
		case 13: rankStr = "K"; break;
		case 12: rankStr = "Q"; break;
		case 11: rankStr = "J"; break;
		default: rankStr = std::to_string(rank); break; // covert 2-10 to a string	
	}
	return rankStr + suit;
}
// create a Card Object from a string card "10D" for file gamemode
Card Card::parse(const std::string& cardStr) {
	char suit = cardStr[cardStr.size() - 1]; // get the last char suit
	std::string rankPart = cardStr.substr(0, cardStr.size() - 1);

	int rank;
		
	if (rankPart == "A") {
		rank = 14;
	} else if (rankPart == "K") {
		rank = 13;
	} else if (rankPart == "Q")  {
		rank = 12;
	} else if (rankPart == "J") {
		rank = 11;
	} else {
		// converts string to integer	
		rank = std::stoi(rankPart);
	}	
// return a new Card Object
return Card(rank, suit);
}
