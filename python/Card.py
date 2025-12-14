# Card.py

class Card:

    # System Ranking  Diamonds -> Clubs -> Hearts -> Spades. low -> high
    SUITS = "DCHS"

    # constructor
    def __init__(self, rank: int, suit: str):
        self.rank = rank
        self.suit = suit    # one character string

    # getters
    def get_rank(self) -> int:  # "->" is a return type hint for user to know
        return self.rank

    def get_suit(self) -> str:
        return self.suit

    # convert to the poker rank. Numbers into letters - for random gamemode
    def __str__(self) -> str:
        if self.rank == 14:
            rank_str = "A"
        elif self.rank == 13:
            rank_str = "K"
        elif self.rank == 12:
            rank_str = "Q"
        elif self.rank == 11:
            rank_str = "J"
        else:
            rank_str = str(self.rank)
        return f"{rank_str}{self.suit}"

    # create a card object from a string card "10D" - for file gamemode
    @staticmethod
    def parse(card_str: str) -> "Card":
        suit = card_str[-1].upper()
        rank_part = card_str[:-1]
        if rank_part == "A":
            rank = 14
        elif rank_part == "K":
            rank = 13
        elif rank_part == "Q":
            rank = 12
        elif rank_part == "J":
            rank = 11
        else:
            rank = int(rank_part)
        return Card(rank, suit)
