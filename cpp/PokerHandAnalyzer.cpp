// PokerHandAnalyzer.cpp
// Jaime Tovali
// Dr. Pounds
// CSC-330

#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <iomanip>
#include <deque>
#include <string>
#include <set>

#include "Card.h"
#include "Deck.h"

using namespace std;

// enums 
enum HandRank {
        ROYAL_STRAIGHT_FLUSH,
        STRAIGHT_FLUSH,
        FOUR_OF_A_KIND,
        FULL_HOUSE,
        FLUSH,
        STRAIGHT,
        THREE_OF_A_KIND,
        TWO_PAIR,
        PAIR,
        HIGH_CARD
};  

enum Suit {
        DIAMONDS, CLUBS, HEARTS, SPADES
};

// evaluation helper for five cards handset - dataholder
struct ComboResult {
        vector<Card> kept;
        Card dropped;
        HandRank rank;
        vector<int> key;

        ComboResult(const vector<Card>& k, const Card& d, HandRank r, const vector<int>& key_)
                : kept(k), dropped(d), rank(r), key(key_) {}
};

// prototypes
static void clearScreen();
string displayHandrank(HandRank rank);
void printHand(const vector<Card>& handOfSix);
vector<vector<Card>> cardCombination(const vector<Card>& handOfSix);
void printCombination(const vector<Card>& handOfSix);
vector<Card> parseTextFile(const string& line);
void testHandset_File(const string& file);
bool duplicateCardsAndPrint(const vector<Card>& hand);
int suitIdx(char s);
int maxSuitOfRank(const vector<Card>& five, int rank);
int suitOfRank(const vector<Card>& five, int rank);
vector<Card> cardsByRankThenSuitDesc(const vector<Card>& five);
bool isFlush(const vector<Card>& five);
int straightHigh(const vector<Card>& five); 
vector<int> rankCounts(const vector<Card>& five);
Card findCardByRank(const vector<Card>& five, int rank);
vector<Card> ofRankSortedBySuitDesc(const vector<Card>& five, int rank);
vector<Card> singlesSorted(const vector<Card>& five, const vector<int>& cnt);
vector<Card> straightDisplay(const vector<Card>& five);
int droppedFullHouse(char s);
ComboResult evaluateFive(const vector<Card>& five, const Card& dropped);
vector<Card> formatDisplay(const ComboResult& res);
int rankStrength(HandRank r); 
int cmpKey(const vector<int>& a, const vector<int>& b);
int compareCombo(const ComboResult& x, const ComboResult& y);
Card findDropped(const vector<Card>& six, const vector<Card>& five); 
void printHighHandOrder(const vector<Card>& handOfSix);

// main method
int main(int argc, char* argv[]) {
	clearScreen();
	Deck deck;
	deck.shuffleDeck();

	cout << "*** P O K E R  H A N D  A N A L Y Z E R ***\n\n";

	if (argc > 1) { // testfile gamemode
		cout << "*** USING TEST DECK ***\n";
		cout << '\n';
		cout << "*** File: " << argv[1] << '\n';
		testHandset_File(argv[1]);
	} else {
		cout << "*** USING RANDOMIZED DECK OF CARDS ***\n\n";
		cout << "*** Shuffled 52 card deck:\n";
		deck.printDeck(false);

		vector<Card> handOfSix = deck.dealSix(); // extract the first 6 cards
		cout << "\n*** Here is the hand:\n";
		printHand(handOfSix);
		printCombination(handOfSix);
		printHighHandOrder(handOfSix);
	}
	return 0;
}
		
// clear screen
static void clearScreen() {
	system("clear");
}

// convert the hand-rank enum to a string type for displaying
// using switch statement to convert hand-rank easily
string displayHandrank(HandRank rank) {
        switch(rank) {
                case ROYAL_STRAIGHT_FLUSH:      return "Royal straight flush";
                case STRAIGHT_FLUSH:            return "Straight flush";
                case FOUR_OF_A_KIND:            return "Four of a Kind";
                case FULL_HOUSE:                return "Full House";
                case FLUSH:                     return "Flush";
                case STRAIGHT:                  return "Straight";
                case THREE_OF_A_KIND:           return "Three of a Kind";
                case TWO_PAIR:                  return "Two Pair";
                case PAIR:                      return "Pair";
                case HIGH_CARD:                 return "High Card";
                default:                        return "Unknown";
        }
}

// method to print the handset of 6 cards
void printHand(const vector<Card>& handOfSix) {
        for (int i = 0; i < handOfSix.size(); i++) {
                cout << ' ' << left << setw(3) << handOfSix[i].toString();
        }
        cout << '\n';
}

// method to make the 5 combinations possibles from the handOfSix and original
vector<vector<Card>> cardCombination(const vector<Card>& handOfSix) {
	deque<Card> dq(handOfSix.begin(), handOfSix.end()); // copy to rotate
	vector<vector<Card>> combos;
	combos.reserve(6);

	for(int i = 0; i < 6; i++) {
		Card drop = dq.back();
		dq.pop_back();
		combos.emplace_back(dq.begin(), dq.end());	// current 5
		dq.push_front(drop);
	}
	return combos;
}

// print combination table
void printCombination(const vector<Card>& handOfSix) {
	cout << "\n*** Hand Combinations...\n";
	deque<Card> dq(handOfSix.begin(), handOfSix.end());
	
	for( int i = 0; i < 6; i++) {
		Card drop = dq.back();
		dq.pop_back();
		
		for(int j = 0; j < dq.size(); j++) {
			cout << ' ' << left << setw(3) << dq[j].toString();
		}
		cout << " | " << drop.toString() << '\n';
		dq.push_front(drop);	// rotate for the next round
	}
}

// method to parse the text file handset into 6 cards
vector<Card> parseTextFile(const string& line) {
	// replace commas with spaces
	string cleaned = line;
	for(char& ch : cleaned) if (ch == ',') ch = ' ';
	
	//split by whitespaces
	istringstream iss(cleaned);
	vector<string> tokens;
	string tok;
	while (iss >> tok) tokens.push_back(tok);

	if (tokens.size() != 6) return {}; // fail
	
	vector<Card> hand;
	hand.reserve(6);

	try {
		for(const auto& s : tokens) {
			hand.push_back(Card::parse(s)); // build card
		}
	} catch (const exception& e) {
		cout << "Error: " << e.what() << '\n';
		return {};
	}
	return hand;
}

// method to read test file and process each hand
void testHandset_File(const string& filename) {
	ifstream in(filename);
	if (!in) {
		cout << "Error: could not open file\n";
		return;
	}
	
	string line;
	while (getline(in, line)) {
		bool onlyWhiteSpace = true;
		for (char ch : line) {
			if (!isspace(ch)) { onlyWhiteSpace = false; break;} 
		}
		if (onlyWhiteSpace) continue;
	
		auto hand = parseTextFile(line);
		if (!hand.empty()) {
			printHand(hand);
			if (duplicateCardsAndPrint(hand)) return;
	
			printCombination(hand);
			printHighHandOrder(hand);
		}
	}
}	

// check for duplicate cards
bool duplicateCardsAndPrint(const vector<Card>& hand) {
	unordered_set<string> seen;

	for (const auto& c : hand) {
		string key = c.toString();
		if (seen.count(key)) {
			cout << "\n*** ERROR - DUPLICATED CARD FOUND IN DECK ***\n\n";
			cout << "*** DUPLICATE: " << key << " ***\n";
			return true;
		}
		seen.insert(key);
	}
	return false;
}

// suit order: D, C, H, S
int suitIdx(char s) {
	char upper = toupper(s);
	size_t pos = Card::SUITS.find(upper);
	return (pos == string::npos) ? -1 : static_cast<int>(pos);
}

// highest suit index among cards of the given rank
int maxSuitOfRank(const vector<Card>& five, int rank) {
	int best = -1;
	for (const auto& c : five) {
		if (c.getRank() == rank) {
			best = max(best, suitIdx(c.getSuit()));
		}
	}
	return best;
}

// suit index of the single card with this rank in "five"
int suitOfRank(const vector<Card>& five, int rank) {
	for (const auto& c : five) {
		if (c.getRank() == rank) return suitIdx(c.getSuit());
	}
	return -1;
}

// cards sorted by rank descending, then suit descending
vector<Card> cardsByRankThenSuitDesc(const vector<Card>& five) {
	vector<Card> out = five;
	sort(out.begin(), out.end(),
              [](const Card& a, const Card& b) {
                  if (a.getRank() != b.getRank())
                      return a.getRank() > b.getRank(); // rank desc
                  return suitIdx(a.getSuit()) > suitIdx(b.getSuit()); // suit desc
              }
	);
    	return out;
}	
		
// check for flush
bool isFlush(const vector<Card>& five) {
	char c = five[0].getSuit();
	for (int i = 1; i < five.size(); i++) {
		if (five[i].getSuit() != c) return false;
	}
	return true;
}

// to check if straight "A" low
int straightHigh(const vector<Card>& five) {
	// collect unique ranks
	set<int> uniqueRanks;
	for (const auto& card : five) uniqueRanks.insert(card.getRank());
	if (uniqueRanks.size() != 5) return 0;

	// sort ascending
	vector<int> ranks(uniqueRanks.begin(), uniqueRanks.end());
	sort(ranks.begin(), ranks.end());

	// A-low: 2,3,4,5,14
	if (ranks[0] == 2 && ranks[1] == 3 && ranks[2] == 4 && ranks[3] == 5 && ranks[4] == 14) {
		return 5;
	}
	
	// normal consecutives check
	for (int i = 1; i < 5; i++) {
		if (ranks[i] - ranks[i-1] != 1) return 0;
	}
	return ranks[4]; // highest rank in the straight
}

// rank counts
vector<int> rankCounts(const vector<Card>& five) {
	vector<int> count(15, 0);
	for (const auto& card : five) count[card.getRank()]++;
	return count;
}
 
// find a single cards with this rank in "five"
Card findCardByRank(const vector<Card>& five, int rank) {
	for (const auto& card : five) if (card.getRank() == rank) return card;
	return Card(0,'X'); // sentinel value - should not happen if ranks exist
}

// all cards of a rank, suit high -> low (S>H>C>D)
vector<Card> ofRankSortedBySuitDesc(const vector<Card>& five, int rank) {
	vector<Card> result;
	for (const auto& c : five) if (c.getRank() == rank) result.push_back(c);
	sort(result.begin(), result.end(),
		[](const Card& a, const Card& b) {
             		return suitIdx(a.getSuit()) > suitIdx(b.getSuit()); // suit desc
		});
	return result;
}

// singles only; sort rank desc then suit desc (kicker)
vector<Card> singlesSorted(const vector<Card>& five, const vector<int>& cnt) {
	vector<Card> singles;
	for (const auto& c : five) if (cnt[c.getRank()] == 1) singles.push_back(c);
	return cardsByRankThenSuitDesc(singles);
}

// for a straight, include A-low. return cards in top -> low order
vector<Card> straightDisplay(const vector<Card>& five) {
	int top = straightHigh(five);
	vector<int> order;
	order.reserve(5);
	if (top == 5) {
		// A-low straight: 5,4,3,2,A(14)
		order.push_back(5);
		order.push_back(4);
		order.push_back(3);
		order.push_back(2);
		order.push_back(14);
	} else {
		for (int r = top; r >= top - 4; --r) order.push_back(r);
	}
	vector<Card> out;
	out.reserve(5);	
	for (int r : order) {
		out.push_back(findCardByRank(five, r));
	}
	return out;
}

// order full house correctly
int droppedFullHouse(char s) {
	switch (toupper(s)) {
		case 'D': return 3;
		case 'C': return 2;
		case 'H': return 1;
		case 'S': return 0;
		default: return -1;
	}
}
	
// evaluate the 5 cards
ComboResult evaluateFive(const vector<Card>& five, const Card& dropped) {
	bool flush = isFlush(five);
	int straightTop = straightHigh(five);
	vector<int> count = rankCounts(five);

	// collect groups from high -> low
	int quad = 0, trips = 0;
	vector<int> pairs;
	pairs.reserve(2);
	vector<int> singles;
	singles.reserve(5);

	for (int r = 14; r >= 2; r--) {
		if (count[r] == 4) quad = r;
		else if (count[r] == 3) trips = r;
		else if (count[r] == 2) pairs.push_back(r);
		else if (count[r] == 1) singles.push_back(r);
	}

	// royal straight flush / straight flush
	if (flush && straightTop > 0) {
		HandRank rank = (straightTop == 14) ? ROYAL_STRAIGHT_FLUSH : STRAIGHT_FLUSH;
		int flushSuit = suitIdx(five[0].getSuit());
		return ComboResult(five, dropped, rank, vector<int>{straightTop, flushSuit});
	}

	// four of a kind
	if (quad != 0) {
		int kicker = singles[0];
		int kickerSuit = suitOfRank(five, kicker);
		return ComboResult(five, dropped, FOUR_OF_A_KIND, vector<int>{quad, kicker, kickerSuit});
	}
	
	// full house
	if (trips != 0 && !pairs.empty()) {
		int tripSuit = maxSuitOfRank(five, trips);
		int pairSuit = maxSuitOfRank(five, pairs[0]);
		int droppedPref = droppedFullHouse(dropped.getSuit());
		return ComboResult(five, dropped, FULL_HOUSE, vector<int>{trips, pairs[0], tripSuit, pairSuit, droppedPref});
	}

	// flush
	if (flush) {
		vector<Card> sorted = cardsByRankThenSuitDesc(five);
		vector<int> key;
		key.reserve(10);
		for (int i = 0; i < 5; i++) key.push_back(sorted[i].getRank());
		for (int i = 0; i < 5; i++) key.push_back(suitIdx(sorted[i].getSuit()));
		return ComboResult(five, dropped, FLUSH, key);
	}

	// straight
	if (straightTop > 0) {
		int topRank = (straightTop == 5) ? 5 : straightTop;
		int topSuit = suitOfRank(five, topRank);
		return ComboResult(five, dropped, STRAIGHT, vector<int>{straightTop, topSuit});
	}

	// three of a kind
	if (trips != 0) {
		int k1 = singles[0];
		int k2 = singles[1];
		int tripSuit = maxSuitOfRank(five, trips);
		int k1Suit = suitOfRank(five, k1);
		int k2Suit = suitOfRank(five, k2);
		return ComboResult(five, dropped, THREE_OF_A_KIND, vector<int>{trips, k1, k2, tripSuit, k1Suit, k2Suit});
	}
	
	// two pair
	if (pairs.size() >= 2) {
		int hi = pairs[0];
		int lo = pairs[1];
		int k = singles[0];
		int hiSuit = maxSuitOfRank(five, hi);
		int loSuit = maxSuitOfRank(five, lo);
		int kSuit = suitOfRank(five, k);
		return ComboResult(five, dropped, TWO_PAIR, vector<int>{hi, lo, k, hiSuit, loSuit, kSuit});
	}

	// pair
	if (pairs.size() == 1) {
		int p = pairs[0];
		int k1 = singles[0];
		int k2 = singles[1];
		int k3 = singles[2];
		int pSuit = maxSuitOfRank(five, p);
		int k1Suit = suitOfRank(five, k1);
		int k2Suit = suitOfRank(five, k2);
		int k3Suit = suitOfRank(five, k3);
		return ComboResult(five, dropped, PAIR, vector<int>{p, k1, k2, k3, pSuit, k1Suit, k2Suit, k3Suit});
	}

	// high card
	vector<Card> sorted = cardsByRankThenSuitDesc(five);
	vector<int> key;
	key.reserve(10);
	for (int i = 0; i < 5; i++) key.push_back(sorted[i].getRank());
	for (int i = 0; i < 5; i++) key.push_back(suitIdx(sorted[i].getSuit()));
	return ComboResult(five, dropped, HIGH_CARD, key);
}

// reorder the kept five
vector<Card> formatDisplay(const ComboResult& res) {
	const vector<Card>& five = res.kept;
	vector<int> cnt = rankCounts(five);

	int quad = 0, trips = 0;
	vector<int> pairs;
	for (int r = 14; r >= 2; r--) {
		if (cnt[r] == 4) quad = r;
		else if (cnt[r] == 3) trips = r;
		else if (cnt[r] == 2) pairs.push_back(r);
	}	
	vector<Card> out;
	switch (res.rank) {
		case ROYAL_STRAIGHT_FLUSH:
		case STRAIGHT_FLUSH:
		case STRAIGHT:
			return straightDisplay(five);
		case FLUSH:
		case HIGH_CARD:
			return cardsByRankThenSuitDesc(five);
		case FOUR_OF_A_KIND: {
			// four + highest kicker
			vector<Card> singles = singlesSorted(five, cnt);
			vector<Card> quadCards = ofRankSortedBySuitDesc(five, quad);
			out.insert(out.end(), quadCards.begin(), quadCards.end());
			if (!singles.empty()) out.push_back(singles[0]);
			return out;
		}
		case FULL_HOUSE: {
			// trips then pair
			vector<Card> t = ofRankSortedBySuitDesc(five, trips);
			vector<Card> p = ofRankSortedBySuitDesc(five, pairs[0]);
			out.insert(out.end(), t.begin(), t.end());
			out.insert(out.end(), p.begin(), p.end());
			return out;
		}
		case THREE_OF_A_KIND: {
			vector<Card> t = ofRankSortedBySuitDesc(five, trips);
			vector<Card> singles = singlesSorted(five, cnt); //two kickers
			out.insert(out.end(), t.begin(), t.end());
			out.insert(out.end(), singles.begin(), singles.end());
		return out;
		}	
		case TWO_PAIR: {
			vector<Card> hi = ofRankSortedBySuitDesc(five, pairs[0]);
			vector<Card> lo = ofRankSortedBySuitDesc(five, pairs[1]);
			vector<Card> singles = singlesSorted(five, cnt); // one kicker
			out.insert(out.end(), hi.begin(), hi.end());
			out.insert(out.end(), lo.begin(), lo.end());
			if (!singles.empty()) out.push_back(singles[0]);
			return out;
		}
		case PAIR: {
			vector<Card> singles = singlesSorted(five, cnt); //three kickers
			vector<Card> p = ofRankSortedBySuitDesc(five, pairs[0]);
			out.insert(out.end(), singles.begin(), singles.end());
			out.insert(out.end(), p.begin(), p.end());
			return out;
		}
		default:
			return five;
	}
}

// evaluating and printing

// rank strength - HIGH_CARD is weakest
int rankStrength(HandRank r) {
	int ordinalValue = static_cast<int>(r);
	return static_cast<int>(HIGH_CARD) - ordinalValue;
}

// compare 2 tiebreakers key vectors, DESC order
// 1 if b > a, -1 if b < a, 0 if equal
int cmpKey(const vector<int>& a, const vector<int>& b) {
	int n = min(a.size(), b.size());
	for (int i = 0; i < n; i++) {
		if (b[i] != a[i]) return (b[i] > a[i]) ? 1 : -1;
	}
	// if all equal then longer wins
	if (b.size() != a.size()) return (b.size() > a.size()) ? 1 : -1;
	return 0;
}

// compare 2 ComboResults. rank first - desc, then key - desc.
int compareCombo(const ComboResult& x, const ComboResult& y) {
	int rx = rankStrength(x.rank);
	int ry = rankStrength(y.rank);
	if (ry != rx) return (ry > rx) ? 1 : -1;  // descend by strength
	return cmpKey(x.key, y.key);	// descend by key
}

// find the dropped. 6th card
Card findDropped(const vector<Card>& six, const vector<Card>& five) {
	for (const auto& c6 : six) {
		bool found = false;
		for (const auto& c5 : five) {
			if (c6.getRank() == c5.getRank() && c6.getSuit() == c5.getSuit()) {
				found = true;
				break;
			}
		}	
		if (!found) return c6;
	}
	return Card(0, 'X'); // sentinel value - should not happen if inputs are valid
}

// evaluate, sort, and print
void printHighHandOrder(const vector<Card>& handOfSix) {
	// build all 5 cards combo
	vector<vector<Card>> combos = cardCombination(handOfSix);
	//evaluate each combo
	vector<ComboResult> results;
	results.reserve(6);
	for (const auto& five : combos) {
		Card dropped = findDropped(handOfSix, five);
		results.push_back(evaluateFive(five, dropped));
	}
	// sort - strongest first. use comparator; smaller means x is stronger
	sort(results.begin(), results.end(), [](const ComboResult& a, const ComboResult& b) {
		return compareCombo(a, b) < 0;
		});
	// print
	cout << "\n--- HIGH HAND ORDER ---\n"; 	
	for (const auto& r : results) {
		// kept 5 in display order
		vector<Card> display = formatDisplay(r);
		for (const auto& c : display) { 
			cout << ' ' << left << setw(3) << c.toString();
		}
		// separator + dropped + label
		cout << " | " << r.dropped.toString() << " -- " << displayHandrank(r.rank) << '\n';
	}
}
