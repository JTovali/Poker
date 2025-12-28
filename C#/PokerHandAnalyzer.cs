//////////////////////////////////////////////////////////////////////////////////////////////
//											   //
//			Six-Card Stud Poker Hand Analyzer			          //
//			          Dr. Pounds					         //
//			      Jaime Tovali Moralez				        //
//			            CSC-330				               //
//			    							      //
///////////////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.IO;

public class PokerHandAnalyzer {
	
	// main method
	public static void Main(string [] args) {
		clearScreen();
		Deck deck = new Deck();
		deck.shuffle();

		Console.WriteLine("*** P O K E R  H A N D  A N A L Y Z E R ***\n");
		Console.WriteLine();
		
		if (args.Length > 0) {		// test-file mode
			Console.WriteLine("*** USING TEST DECK ***\n");
			Console.WriteLine("*** File: " + args[0]);
			testHandset_File(args[0]); 
		} else {
			Console.WriteLine("*** USING RANDOMIZED DECK OF CARDS ***\n");
			Console.WriteLine("*** Shuffled 52 card deck:");
			deck.printDeck(false);
			List<Card> handOfSix = deck.dealSix(); // extract the 6 first cards
			Console.WriteLine("\n*** Here is the hand:");
			printHand(handOfSix);
			printCombination(handOfSix);
			printHighHandOrder(handOfSix);
		}
	}

	// clear screen method
	public static void clearScreen() {
		Console.Write("\u001b[H\u001b[2J");
		Console.Out.Flush();
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
	private class ComboResult {
		public List<Card> kept; 
		public Card dropped;
		public HandRank rank;
		public int[] key;

		public ComboResult(List<Card> kept, Card dropped, HandRank rank, int[] key) { // constructor
			this.kept = kept;
			this.dropped = dropped;
			this.rank = rank;
			this.key = key;
		}
	}

	// convert the handrank enum to a string type for displaying
	private static string display_Handrank(HandRank rank) {
		switch(rank) {
			case HandRank.ROYAL_STRAIGHT_FLUSH: 	return "Royal straight flush";
			case HandRank.STRAIGHT_FLUSH: 		return "Straight flush";
			case HandRank.FOUR_OF_A_KIND: 		return "Four of a Kind";
			case HandRank.FULL_HOUSE: 		return "Full House";
			case HandRank.FLUSH:			return "Flush";
			case HandRank.STRAIGHT:			return "Straight";
			case HandRank.THREE_OF_A_KIND:		return "Three of a Kind";
			case HandRank.TWO_PAIR:			return "Two Pair";
			case HandRank.PAIR:			return "Pair";
			case HandRank.HIGH_CARD:		return "High Card";
			default:				return "Unknown";
		}
	}

	// method to print the handset of 6 cards
	private static void printHand(List<Card> handOfSix) {
		for (int i = 0; i < handOfSix.Count; i++) {
			Console.Write("{0,-4}", " " + handOfSix[i]);
		}
		Console.WriteLine();
	}

	// method to make the 5 combinations possibles from the handOfSix 
	private static List<List<Card>> cardCombination(List <Card> handOfSix) {
		var dq = new LinkedList<Card>(handOfSix);	// make copy of the six card to deque them
		var combos = new List<List<Card>>(6);		// hold six handset combination
		
		for (int i = 0; i < 6; i++) {
			var drop = dq.Last.Value;
			dq.RemoveLast();
			combos.Add(new List<Card>(dq)); 
			dq.AddFirst(drop);	
		}
		return combos;
	}
	
	// method to print combination table
	private static void printCombination(List <Card> handOfSix) {
		Console.WriteLine("\n*** Hand Combinations...");
		var dq = new LinkedList<Card>(handOfSix); // copy of the  handOfSix		

		for (int i = 0; i < 6; i++) {
			var drop = dq.Last.Value;
			dq.RemoveLast();
			foreach (var j in dq) {
				Console.Write("{0,-4}", " " + j);
			}
			Console.Write(" | ");
			Console.WriteLine(drop);
			dq.AddFirst(drop); 		// rotate for the next round
		}
	}

	// method to parse the text-file handset into the 6 cards
	private static List<Card> parseTextFile(string line) {
		string [] tokens = line.Replace(",", " ").Trim().Split(new[] {' ', '\t'}, StringSplitOptions.RemoveEmptyEntries);
		if (tokens.Length != 6) return null;

		var  hand = new List<Card>(6);
		try {
			foreach (var s in tokens) {
				hand.Add(Card.Parse(s));	// builds cards
			}
		} catch (Exception e) {
			Console.WriteLine("Error: " + e.Message);
			return null;
		}
		return hand;
	}

	// read the test-file and process each hand
	private static void testHandset_File(string file) {
		try {
			using(var reader = new StreamReader(file)) {
				string line;
				while ((line = reader.ReadLine()) != null) {
					line = line.Trim(); 
					if (line.Length == 0) continue;
		
					var hand = parseTextFile(line);
					if (hand != null) {
						printHand(hand);
						if (duplicateCardsAndPrint(hand)) {
							return;
						}
						printCombination(hand);
						printHighHandOrder(hand);
					}	
				}
			}
		} catch (IOException e) {
			 Console.WriteLine("Error: " + e.Message);
		}
	}

	// method to check for duplicate cards in file
	private static bool duplicateCardsAndPrint(List<Card> hand) {
		var seen = new HashSet<string>();

		foreach (var c in hand) {
			string key = c.ToString();
			if (!seen.Add(key)) {
				Console.WriteLine("\n*** ERROR - DUPLICATED CARD FOUND IN DECK ***\n");
				Console.WriteLine();
				Console.WriteLine("*** DUPLICATE: " + key + " ***");
				return true;
			}
		}
		return false;
	}

	// suit order: D, C ,H, S
	private static int suitIdx(char s) { 
		return "DCHS".IndexOf(char.ToUpper(s));
	}
	 
	// highest suit index among cards of the given rank
	private static int maxSuitOfRank(List<Card> five, int rank) {
		int best = -1;
		foreach (var c in five) {
			if (c.GetRank() == rank) {
				best = Math.Max(best, suitIdx(c.GetSuit()));
			}
		}
		return best; //returns after scanning al matches
	}

	// suit index of the singles card with this rank in "five"
	private static int suitOfRank(List<Card> five, int rank) {
		foreach (var c in five) if (c.GetRank() == rank) return suitIdx(c.GetSuit());
		return -1; // this should not happen if the ranks exits
	}

	// cards sorted by rank in descending order, then suit descend
	private static List<Card> cardsByRankThenSuitDesc(List<Card> five) {
		var outList = new List<Card>(five);
		outList.Sort((a,b) => { //lambda - function inline. Sort by rank high→low; if tied, sort by suit high→low
			int rc = b.GetRank().CompareTo(a.GetRank()); 
			if (rc != 0) return rc;
			return suitIdx(b.GetSuit()).CompareTo(suitIdx(a.GetSuit()));
		});
		return outList;
	}

	// method to check for flush
	private static bool isFlush(List<Card> five) {
		char c = five[0].GetSuit();
		for (int i = 1; i < five.Count; i++) {		
			if (five[i].GetSuit() != c) return false;
		}		
		return true;
	}

	// method to check if straight. "A" low = 5
	private static int straightHigh(List<Card> five) {
		// collect unique ranks
		var uniqueRanks = new HashSet<int>();
		foreach (var card in five) uniqueRanks.Add(card.GetRank());
		if (uniqueRanks.Count != 5) return 0;	
		
		var ranks = new List<int>(uniqueRanks);
		ranks.Sort();	//ascending order
	
		// "A" low
		if (ranks[0] == 2 && ranks[1] == 3 && ranks[2] == 4 && ranks[3] == 5 && ranks[4] == 14) {
			return 5;
		}
	
		// normal consecutive
		for (int i = 1; i < 5; i++) {
			if (ranks[i] - ranks[i - 1] != 1) return 0;
		}
		return ranks[4]; // highest rank in the straight
	}

	// method for rank counts
	private static int[] rankCounts(List<Card> five) {
		var count = new int[15];
		foreach (var c in five) count[c.GetRank()]++;
		return count;
	}

	// method for single card with this rank in "five"
	private static Card findCardByRank(List<Card> five, int rank) {
		foreach (var c in five) if (c.GetRank() == rank) return c;
		return null;
	}

	// all cards of a rank, suit high->low S>H>C>D
	private static List<Card> ofRankSortedBySuitDesc(List<Card> five, int rank) { 
		var list = new List<Card>();
		foreach (var c in five) if (c.GetRank() == rank) list.Add(c);
		list.Sort((a, b) => suitIdx(b.GetSuit()).CompareTo(suitIdx(a.GetSuit())));
		return list;
	}

	// sort rank high>low, the suit high>>low - this is important for kickers
	private static List<Card> singlesSorted(List<Card> five, int[] cnt) {
		var singles = new List<Card>();
		foreach (var c in five) if (cnt[c.GetRank()] == 1) singles.Add(c);
		return cardsByRankThenSuitDesc(singles);
	}

	// for straight. Include A-low. return top->low straight order
	private static List<Card> straightDisplayOrder(List<Card> five) {
		int top = straightHigh(five);
		var order = new List<int>(5);
		if (top == 5) {
			order.Add(5); order.Add(4); order.Add(3); order.Add(2); order.Add(14);
		} else {
			for (int r = top; r >= top - 4; --r) order.Add(r);
		}
		var outList = new List<Card>(5);
		foreach (var r in order) outList.Add(findCardByRank(five, r));
		return outList;
	}
	
	// Order Full House correctly
	private static int droppedFullHouse(char s) {
		switch (char.ToUpper(s)) {
			case 'D': return 3;
			case 'C': return 2;
			case 'H': return 1;
			case 'S': return 0;
			default: return -1;
		}
	} 

	// method to evaluate the 5 card in hand
	private static ComboResult evaluateFive(List<Card> five, Card dropped) {
        	bool flush = isFlush(five);
                int straightTop = straightHigh(five);
                int[] count = rankCounts(five);

		//collect the groups from high->low so list ends up nicely
		int quad = 0, trips = 0;
               	var pairs = new List<int>(2);
                var singles = new List<int>(5);

                for (int r = 14; r >= 2; r--) {
                        if (count[r] == 4) quad = r;
                        else if (count[r] == 3) trips = r;
                        else if (count[r] == 2) pairs.Add(r);
                        else if (count[r] == 1) singles.Add(r);
                }
		
		// Straight Flush / Royal Straight Flush
		if (flush && straightTop > 0) {
			HandRank rank = (straightTop == 14) ? HandRank.ROYAL_STRAIGHT_FLUSH : HandRank.STRAIGHT_FLUSH;
			int flushSuit = suitIdx(five[0].GetSuit());
			return new ComboResult(five, dropped, rank, new int[] { straightTop, flushSuit });
		}
		
		// Four of a Kind
		if (quad != 0) {
			int kicker = singles[0];
			int kickerSuit = suitOfRank(five, kicker);
			return new ComboResult(five, dropped, HandRank.FOUR_OF_A_KIND, new int[] { quad, kicker, kickerSuit });
		}
	
		// Full House
		if (trips != 0 && pairs.Count > 0) {
			int tripSuit = maxSuitOfRank(five, trips);
			int pairSuit = maxSuitOfRank(five, pairs[0]);
			int droppedPref = droppedFullHouse(dropped.GetSuit());
			return new ComboResult(five, dropped, HandRank.FULL_HOUSE, new int[] { trips, pairs[0], tripSuit, pairSuit, droppedPref });
		}

		// Flush
		if (flush) {
			var sortedFlush = cardsByRankThenSuitDesc(five);

			var keyFlush = new int[10];
			for (int i = 0; i < 5; i++) keyFlush[i] = sortedFlush[i].GetRank();
			for (int i = 0; i < 5; i++) keyFlush[5 + i] = suitIdx(sortedFlush[i].GetSuit());
			return new ComboResult(five, dropped, HandRank.FLUSH, keyFlush );
		}

		// Straight
		if (straightTop > 0) {
			int topRank = (straightTop == 5) ? 5 : straightTop;
			int topSuit = suitOfRank(five, topRank);
			return new ComboResult(five, dropped, HandRank.STRAIGHT, new int[] { straightTop, topSuit });
		}

		// Three of a Kind
		if (trips != 0) {
			int k1 = singles[0], k2 = singles[1];
			int tripSuit = maxSuitOfRank(five, trips);
			int k1Suit = suitOfRank(five, k1);
			int k2Suit = suitOfRank(five, k2);
			return new ComboResult( five, dropped, HandRank.THREE_OF_A_KIND, new int[] { trips, k1, k2, tripSuit, k1Suit, k2Suit });
		}

		// Two Pair
		if (pairs.Count >= 2) {
			int hi = pairs[0], lo = pairs[1], k = singles[0];
			int hiSuit = maxSuitOfRank(five, hi);
			int loSuit = maxSuitOfRank(five, lo);
			int kSuit = suitOfRank(five, k);
			return new ComboResult( five, dropped, HandRank.TWO_PAIR, new int[] { hi, lo, k, hiSuit, loSuit, kSuit });
		}

		// Pair
		if (pairs.Count == 1) {
			int p = pairs[0], k1 = singles[0], k2 = singles[1], k3 = singles[2];
			int pSuit = maxSuitOfRank(five, p);
			int k1Suit = suitOfRank(five, k1);
			int k2Suit = suitOfRank(five, k2);
			int k3Suit = suitOfRank(five, k3);
			return new ComboResult( five, dropped, HandRank.PAIR, new int[] { p, k1, k2, k3, pSuit, k1Suit, k2Suit, k3Suit });
		}

		//  High-Card
   		var sortedHigh = cardsByRankThenSuitDesc(five);
		var keyHigh = new int[10];
		for (int i = 0; i < 5; i++) keyHigh[i] = sortedHigh[i].GetRank();
		for (int i = 0; i < 5; i++) keyHigh[5 + i] = suitIdx(sortedHigh[i].GetSuit());
		return new ComboResult( five, dropped, HandRank.HIGH_CARD, keyHigh);
		
	}

	// reorder the kept 5 in hand
	private static List<Card> formatDisplay(ComboResult res) {
		var five = res.kept;

		// recounts and group ranks from the actual kept cards
		int [] cnt = rankCounts(five);
		
		int quad = 0, trips = 0; 
		var pairs = new List<int>(2);
		for (int r = 14; r >= 2; r--) {
			if (cnt[r] == 4) quad = r;
			else if (cnt[r] == 3) trips = r;
			else if (cnt[r] == 2) pairs.Add(r);
		}
		var outList = new List<Card>(5);
		switch (res.rank) {
	
			case HandRank.ROYAL_STRAIGHT_FLUSH:
			case HandRank.STRAIGHT_FLUSH:
			case HandRank.STRAIGHT:
				return straightDisplayOrder(five);
	
			case HandRank.FLUSH:
			case HandRank.HIGH_CARD:
				return cardsByRankThenSuitDesc(five);
	
			case HandRank.FOUR_OF_A_KIND: {
				outList.AddRange(ofRankSortedBySuitDesc(five, quad));  		// quads
				var singlesLocal = singlesSorted(five, cnt);
				outList.Add(singlesLocal[0]);					// kicker
				return outList;
			}
		
			case HandRank.FULL_HOUSE: {
				outList.AddRange(ofRankSortedBySuitDesc(five, trips));		// trips
				outList.AddRange(ofRankSortedBySuitDesc(five, pairs[0]));	// pair
				return outList;
			}
			
			case HandRank.THREE_OF_A_KIND: {
				outList.AddRange(ofRankSortedBySuitDesc(five, trips));		// trips
				outList.AddRange(singlesSorted(five, cnt));			// two kickers
				return outList;
			}
			
			case HandRank.TWO_PAIR: {
				outList.AddRange(ofRankSortedBySuitDesc(five, pairs[0]));	// high pair
				outList.AddRange(ofRankSortedBySuitDesc(five, pairs[1]));	// low pair
				var singlesLocal = singlesSorted(five, cnt);
				outList.Add(singlesLocal[0]);					// kicker	
				return outList;
			}

			case HandRank.PAIR: {
				outList.AddRange(singlesSorted(five, cnt));                     // 3 kickers
				outList.AddRange(ofRankSortedBySuitDesc(five, pairs[0]));	// pair
				return outList;
			}
			default:
				return five;
		}
	}

	//////////// evaluating, sort, and printing ////////////
	
	// method for rank strength method
	private static int rankStrength(HandRank r) {
		return Enum.GetValues(typeof(HandRank)).Length - 1 - (int)r;
	}

	// method to compare tiebreaker keys, all descending
	private static int cmpKey(int[] a, int[] b) {
		int n = Math.Min(a.Length, b.Length);
		for (int i = 0; i < n; i++) {
			int c = b[i].CompareTo(a[i]); //descending
			if (c != 0) return c;
		}
		return b.Length.CompareTo(a.Length);
	}

	// compare 2 ComboResults, rank first then key
	private static int compareCombo(ComboResult x, ComboResult y) {
		int rx = rankStrength(x.rank), ry = rankStrength(y.rank);
		int c = ry.CompareTo(rx); // descend by strength
		if (c != 0) return c;
		return cmpKey(x.key, y.key); // descend by key
	}

	// finding the dropped card - 6th card that is dropped
	private static Card findDropped(List<Card> six, List<Card> five) {
		foreach (var c in six) {
			if (!five.Contains(c)) {
				return c;
			}	
		}
		return null;
	}

	// evaluate, sort, and print
	private static void printHighHandOrder(List<Card> handOfSix) {
		var combos = cardCombination(handOfSix);
		var results = new List<ComboResult>(6);
		foreach (var five in combos) {
			Card dropped = findDropped(handOfSix, five);
			results.Add(evaluateFive(five, dropped));
		}
		results.Sort(compareCombo); // sort by rank strength, then by tiebreaker key "desc"

		Console.WriteLine("\n--- HIGH HAND ORDER ---");
		foreach (var r in results) {
			var display = formatDisplay(r);
			// kept five formatted
			foreach (var c in display) {
				Console.Write("{0,-4}", " " + c);
			}
			Console.Write(" | ");
			// seperator + dropped + ranked label
			Console.Write(r.dropped);
			Console.WriteLine(" -- " + display_Handrank(r.rank));
		}
	} 
}
