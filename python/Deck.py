 # Deck.py

import random
from Card import Card
from typing import List

class Deck:
    
    # deck constructor
    def __init__(self):
        self.cards = []
        suits = "DCHS"
        for s in suits:
            for rank in range(2, 15): # 2-14
                self.cards.append(Card(rank, s))

    # method to shuffle deck
    def shuffle(self):
        random.shuffle(self.cards)

    # method to deal hand of six
    def deal_six(self) -> List[Card]:
        hand = self.cards[:6]
        del self.cards[:6]
        return hand

    # method for printing deck - when false
    def print_deck(self, single_line: bool):
        for i in range(len(self.cards)):
            card = self.cards[i]
            print(card, end="")
            end_of_row = (not single_line) and ((i + 1) % 13 == 0)
            if end_of_row:
                print()
            elif i + 1 < len(self.cards):
                print(" ", end="")
        print()    
