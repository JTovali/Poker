// Card.cs
using System;

public class Card {
	
	private int rank;
	private char suit;

	private const string SUITS = "DCHS";

	// constructor
	public Card(int rank, char suit) {
		this.rank = rank;
		this.suit = suit;
	}

	// getters
	public int GetRank() {
		return rank;
	}
		
	public char GetSuit() {
		return suit;	
	}
	
	// convert numbers into letters for poker.
	public override string ToString() {
		string rankStr;
		switch (rank) {
			case 14: rankStr = "A"; break;
			case 13: rankStr = "K"; break;
			case 12: rankStr = "Q"; break;
			case 11: rankStr = "J"; break;
			default: rankStr = rank.ToString(); break; // 2-10 uses default
		}
		return rankStr + suit;	
	}

	// Create a Card Object from a string card "10D" - for file gamemode
	public static Card Parse(string cardStr) {
		char suit = cardStr[cardStr.Length - 1];
		string rankPart = cardStr.Substring(0, cardStr.Length - 1);

		int rank;
		switch(rankPart) {
			case "A": rank = 14; break;
			case "K": rank = 13; break;
			case "Q": rank = 12; break;
			case "J": rank = 11; break;
			default: rank = int.Parse(rankPart); break;
		}
		return new Card(rank, suit);
	}
}
		
