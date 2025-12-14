import java.util.*;

public class Deck {

	private List <Card> cards; //all cards in the deck
	
	// Deck constructor - inorder
	public Deck() {
		cards = new ArrayList<>();
		char[] suits = {'D','C','H','S'};
		
		for (int i = 0; i < suits.length; i++) {
			char suit = suits[i];
			for (int rank = 2; rank <= 14; rank++) {
				cards.add(new Card(rank,suit));
			}
		}
	}
	
	//method to shuffle deck
	public void shuffle() {
		Collections.shuffle(cards, new Random());
	}

	// method to deal hand
	public List<Card> dealSix() {
		List<Card> hand = new ArrayList<>(6);
		for (int i = 0; i < 6; i++) {
			hand.add(cards.remove(0));
		}
		return hand;
	}

	// Method for printing deck - when false it will print deck
	public void printDeck(boolean singleLine) {
		for (int i = 0; i < cards.size(); i++) {
			System.out.print(cards.get(i));
			boolean endOfRow = !singleLine && ((i+1) % 13 == 0);
		
			if (endOfRow) {
				System.out.println();
			} else if (i + 1 < cards.size()) {
				System.out.print(" ");
			}
		}
		System.out.println();
	}
}			



