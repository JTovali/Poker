// Card.java 
public class Card {
	
	private int rank;
	private char suit;

	// System Ranking  Diamonds -> Clubs -> Hearts -> Spades. low -> high
	private static final String SUITS = "DCHS";

	// Constructors
	public Card (int rank, char suit) {
		
		this.rank = rank;
		this.suit = suit;
	}

	// getters
	public int getRank() {
		return rank;
	}
	
	public char getSuit() {
		return suit;
	}

	// Convert to the poker rank. Numbers into letters - For gamemode 1
	public String toString() {
		String rankStr;
		switch (rank) {
			case 14: rankStr = "A"; break;
			case 13: rankStr = "K"; break;
			case 12: rankStr = "Q"; break;
			case 11: rankStr = "J"; break;
			default: rankStr = String.valueOf(rank); break; // 2-10 use default
		}
		return rankStr + suit;
	}

	// Create a Card Object from a string card "10D" - For file gamemode
	public static Card parse(String cardStr) {
		char suit = cardStr.charAt(cardStr.length() - 1);
		String rankPart = cardStr.substring(0, cardStr.length() - 1);
		
		int rank;
		switch(rankPart) {
			case "A": rank = 14; break;
			case "K": rank = 13; break;
			case "Q": rank = 12; break;
			case "J": rank = 11; break;
			default: rank = Integer.parseInt(rankPart); break;
		}
		return new Card(rank, suit);
	}
}
