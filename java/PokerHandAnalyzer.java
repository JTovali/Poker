//////////////////////////////////////////////////////////////////////////////////////////
//										       //
//			    Six-Card Stud Poker Hand Analyzer			      //
//			              Dr. Pounds				     //	
//			          Jaime Tovali Moralez				    //
//			          	CSC-330					   //	
//  										  //
///////////////////////////////////////////////////////////////////////////////////		 

import java.util.*;
import java.io.*;

public class PokerHandAnalyzer {
	
	// main method
	public static void main (String [] args) {
		
		clearScreen();
		Deck deck = new Deck();
		deck.shuffle();
		
		System.out.println("*** P O K E R  H A N D  A N A L Y Z E R ***\n");
		System.out.println();
		
		if (args.length > 0) {	// TEST-FILE GAMEMODE
			System.out.println("*** USING TEST DECK ***\n");
			System.out.println("*** File: " + args[0]);
			testHandset_File(args[0]); 
		} else { 		// RANDOM GAMEMODE
			System.out.println("*** USING RANDOMIZED DECK OF CARDS ***\n");
			System.out.println("*** Shuffled 52 card deck:");
			deck.printDeck(false);
			List<Card> handOfSix = deck.dealSix(); //extract the 6 first cards
			System.out.println("\n*** Here is the hand:");
			printHand(handOfSix);
			List <List<Card>> handCombination = cardCombination(handOfSix); //Store each combination hand here
			printCombination(handOfSix); 
			printHighHandOrder(handOfSix);
		}	
	}

	// clear screen method
	public static void clearScreen() {
		System.out.print("\033[H\033[2J");
		System.out.flush();
	}

	// enumeration for hand ranks
	public enum HandRank {
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
	}	
	
	// enumeration for suits
	public enum Suit {
		DIAMONDS, CLUBS, HEARTS, SPADES
	}

	// evaluation helpers for five cards handset - dataholder
	private static class ComboResult {
		final List<Card> kept; 
		final Card dropped;
		final HandRank rank;
		final int[] key;
		
		ComboResult(List<Card> kept, Card dropped, HandRank rank, int[] key) { // constructor
			this.kept = kept;
			this.dropped = dropped;
			this.rank = rank;
			this.key = key;
		}
	}
	
	// convert the hand-rank enum to a string type for displaying
	// using swutch statement to convert hand-rank easily
	private static String display_Handrank(HandRank rank) {
		switch(rank) {
			case ROYAL_STRAIGHT_FLUSH: 	return "Royal straight flush";
			case STRAIGHT_FLUSH: 		return "Straight flush";
			case FOUR_OF_A_KIND: 		return "Four of a Kind";
			case FULL_HOUSE: 		return "Full House";
			case FLUSH:			return "Flush";
			case STRAIGHT:			return "Straight";
			case THREE_OF_A_KIND:		return "Three of a Kind";
			case TWO_PAIR:			return "Two Pair";
			case PAIR:			return "Pair";
			case HIGH_CARD:			return "High Card";
			default:			return "Unknown";
		}
	}
	
	// method to print the handset of 6 cards
	private static void printHand(List<Card> handOfSix) {
		for (int i = 0; i < handOfSix.size(); i++) {
			System.out.printf("%-4s", " " + handOfSix.get(i));
		}
		System.out.println();
	}

	// method to make the 5 combinations possibles from the handOfSix and original
	private static List<List<Card>> cardCombination(List <Card> handOfSix) {
		Deque <Card> dq = new ArrayDeque<>(handOfSix);	// make copy of the six card to deque them
		List <List<Card>> combos = new ArrayList<>(6);	// hold six handset combination
		
		for (int i = 0; i < 6; i++) {
			Card drop = dq.removeLast();
			combos.add(new ArrayList<>(dq)); 
			dq.addFirst(drop);
		}
		return combos;
	}
	
	// method to print combination table
	private static void printCombination(List <Card> handOfSix) {
		System.out.println("\n*** Hand Combinations...");
		Deque <Card> dq = new ArrayDeque<>(handOfSix); // copy of the  handOfSix		

		for (int i = 0; i < 6; i++) {
			Card drop = dq.removeLast();
			for (Card j : dq) {
				System.out.printf("%-4s", " " + j);
			}
			System.out.print(" | ");
			System.out.println(drop);
			dq.addFirst(drop); 		// rotate for the next round
		}
	}
	
	// method to pase the text-file handset into the 6 cards
	private static List<Card> parseTextFile(String line) {
		String [] tokens = line.replace(",", " ").trim().split("\\s+");
		if (tokens.length != 6) return null;

		List<Card> hand = new ArrayList<>(6);
		try {
			for (String s : tokens) {
				hand.add(Card.parse(s));	// builds cards
			}
		} catch (Exception e) {
			System.out.println("Error: " + e.getMessage());
			return null;
		}
		return hand;
	}

	// method to read test-file and load the hands
	private static void testHandset_File(String file) {
		try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
			String line;
			while ((line = reader.readLine()) != null) {
				line = line.trim(); 
				if (line.isEmpty()) continue;
		
				List<Card> hand = parseTextFile(line);
				if (hand != null) {
					printHand(hand);
					if (duplicateCardsAndPrint(hand)) {
						return;
					}
					List<List<Card>> combos = cardCombination(hand);
					printCombination(hand);
					printHighHandOrder(hand);
				}
			}
		} catch (IOException e) {
			 System.out.println("Error: " + e.getMessage());
		
		}
	}

	// method to check for duplicate in file - //hashSet to store unique elements
	private static boolean duplicateCardsAndPrint(List<Card> hand) {
		Set<String> seen = new HashSet<>();

		for (Card c : hand) {
			String key = c.toString();
			if (!seen.add(key)) {
				System.out.println("\n*** ERROR - DUPLICATED CARD FOUND IN DECK ***\n");
				System.out.println();
				System.out.println("*** DUPLICATE: " + key + " ***");
				return true;
			}
		}
		return false;
	}

	// suit order: D, C ,H, S
	private static int suitIdx(char s) { return "DCHS".indexOf(Character.toUpperCase(s)); }
   
	// highest suit index among cards of the given rank
	private static int maxSuitOfRank(List<Card> five, int rank) {
		int best = -1;
		for (Card c : five) {
			if (c.getRank() == rank) {
				best = Math.max(best, suitIdx(c.getSuit()));
			}
		}
		return best; //returns after scanning al matches
	}
	
	// suit index of the singles card with this rank in "five"
	private static int suitOfRank(List<Card> five, int rank) {
		for (Card c : five) if (c.getRank() == rank) return suitIdx(c.getSuit());
		return -1; // this should not happen if the ranks exits
	}

	// cards sorted by rank in descending order, then suit descend
	private static List<Card> cardsByRankThenSuitDesc(List<Card> five) {
		List<Card> out = new ArrayList<>(five);
		out.sort((a,b) -> { //lambda - function inline. Sort by rank high→low; if tied, sort by suit high→low
			int rc = Integer.compare(b.getRank(), a.getRank());
			if (rc != 0) return rc;
			return Integer.compare(suitIdx(b.getSuit()), suitIdx(a.getSuit()));
		});
		return out;
	}

	// method to check for flush
	private static boolean isFlush(List<Card> five) {
		char c = five.get(0).getSuit();
		for (int i = 1; i < five.size(); i++) {		
			if (five.get(i).getSuit() != c) return false;
		}		
		return true;
	}

	// method to check if straight. "A" low = 5
	private static int straightHigh(List<Card> five) {
		//using Set to  collect unique ranks
		Set<Integer> uniqueRanks = new HashSet<>(5);
		for (Card c : five) uniqueRanks.add(c.getRank());
		if (uniqueRanks.size() != 5) return 0;	
		
		List<Integer> ranks = new ArrayList<>(uniqueRanks);
		Collections.sort(ranks); //ascending order
		
		// "A" low
		if (ranks.get(0) == 2 && ranks.get(1) == 3 && ranks.get(2) == 4 && ranks.get(3) == 5 && ranks.get(4) == 14) {
			return 5;
		}

		// normal consecutive
		for (int i = 1; i < 5; i++) {
			if (ranks.get(i) - ranks.get(i - 1) != 1) return 0;
		}
		return ranks.get(4); // highest rank in the straight
	}
	
	// method for rank counts
	private static int [] rankCounts(List<Card> five) {
		int[] count = new int[15];
		for (Card c : five) count[c.getRank()]++;
		return count;
	}

	// method for single card with this rank in "five"
	private static Card findCardByRank(List<Card> five, int rank) {
		for (Card c : five) if (c.getRank() == rank) return c;
		return null;
	}

	// all cards of a rank, suit high->low S>H>C>D
	private static List<Card> ofRankSortedBySuitDesc(List<Card> five, int rank) { 
		List<Card> list = new ArrayList<>();
		for (Card c : five) if (c.getRank() == rank) list.add(c);
		list.sort((a, b) -> Integer.compare(suitIdx(b.getSuit()), suitIdx(a.getSuit())));
		return list;
		}
	
	// sort rank high>low, the suit high>>low - this is important for kickers
	private static List<Card> singlesSorted(List<Card> five, int[] cnt) {
		List<Card> singles = new ArrayList<>();
		for (Card c : five) if (cnt[c.getRank()] == 1) singles.add(c);
		return cardsByRankThenSuitDesc(singles);
	}
	
	// for straight. Include A-low. return top->low straight order
	private static List<Card> straightDisplayOrder(List<Card> five) {
		int top = straightHigh(five);
		List<Integer> order = new ArrayList<>(5);
		if (top == 5) {
			order.add(5); order.add(4); order.add(3); order.add(2); order.add(14);
		} else {
			for (int r = top; r >= top - 4; --r) order.add(r);
		}
		List<Card> out = new ArrayList<>(5);
		for (int r : order) out.add(findCardByRank(five, r));
		return out;
	}	
	
	//Order Full House correctly
	private static int droppedFullHouse(char s) {
		switch (Character.toUpperCase(s)) {
			case 'D': return 3;
			case 'C': return 2;
			case 'H': return 1;
			case 'S': return 0;
			default: return -1;
		}
	} 

	// method to evaluate the 5 card in hand
	private static ComboResult evaluateFive(List<Card> five, Card dropped) {
        	boolean flush = isFlush(five);
                int straightTop = straightHigh(five);
                int[] count = rankCounts(five);
	
		//collect the groups from high->low so list ends up nicely
              	int quad = 0, trips = 0;
                List<Integer> pairs = new ArrayList<>(2);
                List<Integer> singles = new ArrayList<>(5);

                for (int r = 14; r >= 2; r--) {
                        if (count[r] == 4) quad = r;
                        else if (count[r] == 3) trips = r;
                        else if (count[r] == 2) pairs.add(r);
                        else if (count[r] == 1) singles.add(r);
                }

		// Straigh Flush / Royal Straight Flush
		if (flush && straightTop > 0) {
			HandRank rank = (straightTop == 14) ? HandRank.ROYAL_STRAIGHT_FLUSH : HandRank.STRAIGHT_FLUSH;
			int flushSuit = suitIdx(five.get(0).getSuit());
			return new ComboResult(five, dropped, rank, new int[] { straightTop, flushSuit });
		}

		// Four of a Kind
		if (quad != 0) {
			int kicker = singles.get(0);
			int kickerSuit = suitOfRank(five, kicker);
			return new ComboResult(five, dropped, HandRank.FOUR_OF_A_KIND, new int[] { quad, kicker, kickerSuit });
		}

		// Full House
		if (trips != 0 && !pairs.isEmpty()) {
			int tripSuit = maxSuitOfRank(five, trips);
			int pairSuit = maxSuitOfRank(five, pairs.get(0));
			int droppedPref = droppedFullHouse(dropped.getSuit());
			return new ComboResult(five, dropped, HandRank.FULL_HOUSE, new int[] { trips, pairs.get(0), tripSuit, pairSuit, droppedPref });
		} 
		
		// Flush
		if (flush) {
			List<Card> sorted = cardsByRankThenSuitDesc(five);

			int[] key = new int[10];
			for (int i = 0; i < 5; i++) key[i] = sorted.get(i).getRank();
			for (int i = 0; i < 5; i++) key[5 + i] = suitIdx(sorted.get(i).getSuit());
			return new ComboResult(five, dropped, HandRank.FLUSH, key );
		}

		// Straight
		if (straightTop > 0) {
			int topRank = (straightTop == 5) ? 5 : straightTop;
			int topSuit = suitOfRank(five, topRank);
			return new ComboResult(five, dropped, HandRank.STRAIGHT, new int[] { straightTop, topSuit });
		}
		
		// Three of a Kind
		if (trips != 0) {
			int k1 = singles.get(0), k2 = singles.get(1);
			int tripSuit = maxSuitOfRank(five, trips);
			int k1Suit = suitOfRank(five, k1);
			int k2Suit = suitOfRank(five, k2);
			return new ComboResult( five, dropped, HandRank.THREE_OF_A_KIND, new int[] { trips, k1, k2, tripSuit, k1Suit, k2Suit });
		}

		// Two Pair
		if (pairs.size() >= 2) {
			int hi = pairs.get(0), lo = pairs.get(1), k = singles.get(0);
			int hiSuit = maxSuitOfRank(five, hi);
			int loSuit = maxSuitOfRank(five, lo);
			int kSuit = suitOfRank(five, k);
			return new ComboResult( five, dropped, HandRank.TWO_PAIR, new int[] { hi, lo, k, hiSuit, loSuit, kSuit });
		}

		// Pair
		if (pairs.size() == 1) {
			int p = pairs.get(0), k1 = singles.get(0), k2 = singles.get(1), k3 = singles.get(2);
			int pSuit = maxSuitOfRank(five, p);
			int k1Suit = suitOfRank(five, k1);
			int k2Suit = suitOfRank(five, k2);
			int k3Suit = suitOfRank(five, k3);
			return new ComboResult( five, dropped, HandRank.PAIR, new int[] { p, k1, k2, k3, pSuit, k1Suit, k2Suit, k3Suit });
		}

		// High Card
		List<Card> sorted = cardsByRankThenSuitDesc(five);
		int[] key = new int[10];
		for (int i = 0; i < 5; i++) key[i] = sorted.get(i).getRank();
		for (int i = 0; i < 5; i++) key[5 + i] = suitIdx(sorted.get(i).getSuit());
		return new ComboResult( five, dropped, HandRank.HIGH_CARD, key);
		
	}

	// reorder the kept five
	private static List<Card> formatDisplay(ComboResult res) {
		List<Card> five = res.kept;
		int [] cnt = rankCounts(five);
		
		int quad = 0, trips = 0; 
		List<Integer> pairs = new ArrayList<>(2);
		for (int r = 14; r >= 2; r--) {
			if (cnt[r] == 4) quad = r;
			else if (cnt[r] == 3) trips = r;
			else if (cnt[r] == 2) pairs.add(r);
		}
		List<Card> out = new ArrayList<>(5);
		switch (res.rank) {
			case ROYAL_STRAIGHT_FLUSH:
			case STRAIGHT_FLUSH:
			case STRAIGHT:
				return straightDisplayOrder(five);
			case FLUSH:
			case HIGH_CARD:
				return cardsByRankThenSuitDesc(five);
			case FOUR_OF_A_KIND:
				out.addAll(ofRankSortedBySuitDesc(five, quad));
				out.addAll(singlesSorted(five, cnt).subList(0, 1));
				return out;
			case FULL_HOUSE:
				out.addAll(ofRankSortedBySuitDesc(five, trips));
				out.addAll(ofRankSortedBySuitDesc(five, pairs.get(0)));
				return out;
			case THREE_OF_A_KIND:
				out.addAll(ofRankSortedBySuitDesc(five, trips));
				out.addAll(singlesSorted(five, cnt));
				return out;
			case TWO_PAIR:
				out.addAll(ofRankSortedBySuitDesc(five, pairs.get(0)));
				out.addAll(ofRankSortedBySuitDesc(five, pairs.get(1)));
				out.addAll(singlesSorted(five, cnt).subList(0, 1));
				return out;
			case PAIR:
				out.addAll(singlesSorted(five, cnt));
				out.addAll(ofRankSortedBySuitDesc(five, pairs.get(0)));
				return out;
			default:
				return five;
		}
	}

	/////////// evaluating and printing /////////////
	
	// method for rank strength method
	private static int rankStrength(HandRank r) {
		return HandRank.values().length - 1 - r.ordinal();
	}

	// method to compare of tiebreaker keys, all descending
	private static int cmpKey(int[] a, int[] b) {
		int n = Math.min(a.length, b.length);
		for (int i = 0; i < n; i++) {
			int c = Integer.compare(b[i], a[i]); //descending
			if (c != 0) return c;
		}
		return Integer.compare(b.length, a.length);
	}

	// compare 2 ComboResults, rank first then key	
	private static int compareCombo(ComboResult x, ComboResult y) {
		int rx = rankStrength(x.rank), ry = rankStrength(y.rank);
		int c = Integer.compare(ry,rx); // descend by strength
		if (c != 0) return c;
		return cmpKey(x.key, y.key); // descend by key
	}
	
	// finding the dropped card - 6th card that is dropped
	private static Card findDropped(List<Card> six, List<Card> five) {
		for (Card c : six) {
			if (!five.contains(c)) {
				return c;
			}	
		}
		return null;
	}

	// evaluate, sort, and print
	private static void printHighHandOrder(List<Card> handOfSix) {
		List<List<Card>> combos = cardCombination(handOfSix);
		List<ComboResult> results = new ArrayList<>(6);
		for (List<Card> five : combos) {
			Card dropped = findDropped(handOfSix, five);
			results.add(evaluateFive(five, dropped));
		}
		results.sort(PokerHandAnalyzer::compareCombo); // method reference to our static comparator

		System.out.println("\n--- HIGH HAND ORDER ---");
		for (ComboResult r : results) {
			List<Card> display = formatDisplay(r);
			// kept five
			for (Card c : display) {
				System.out.printf("%-4s", " " + c);
			}
			System.out.print(" | ");
			//dropped
			System.out.print(r.dropped);
			System.out.println(" -- " + display_Handrank(r.rank));
		}
	}
}
