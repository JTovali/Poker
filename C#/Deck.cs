// Deck.cs
using System;
using System.Collections.Generic;

public class Deck {
	
	private List<Card> cards;
	
	// deck constructor
	public Deck() {
		cards = new List<Card>();
		char [] suits = { 'D', 'C', 'H', 'S' };
		
		for (int i = 0; i < suits.Length; i++) {
			char suit = suits[i];
			for (int rank = 2; rank <= 14; rank++) {
				cards.Add(new Card(rank, suit));
			}
		}
	}

	// method to shuffle deck
	public void shuffle() {
		// create a new instance of the Random class to generate random numbers
		Random rng = new Random();
		int n = cards.Count;
		while (n > 1) {
			int k = rng.Next(n--); // generates a random index
			Card value = cards[k];
			cards[k] = cards[n];
			cards[n] = value;
		}
	}

	// method to deal hand
	public List<Card> dealSix() {
		List<Card> hand = new List<Card>(6);
		for (int i = 0; i < 6; i++) 
		{
			Card c = cards[0];
			cards.RemoveAt(0);
			hand.Add(c);
		}
		return hand;
	}

	// method to print deck - when false it will print deck
	public void printDeck(bool singleLine) {
		for (int i = 0; i < cards.Count; i++)
		{
			Console.Write(cards[i]);
			bool endOfRow = !singleLine && ((i + 1) % 13 == 0);
			if (endOfRow)
			{
				Console.WriteLine();
			}
			else if (i + 1 < cards.Count)
			{
				Console.Write(" ");
			}
		}
		Console.WriteLine();
	}
}
