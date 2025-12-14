# PokerHandAnalyzer.py
# JaimeT
# Dr. Pounds
# CSC-330

import sys
import os
from enum import IntEnum
from collections import deque
from Card import Card
from Deck import Deck
from typing import List
from functools import cmp_to_key

# enumeration for suits
class Suit(IntEnum):
    DIAMONDS = 0
    CLUBS = 1
    HEARTS = 2
    SPADES = 3

# enumeration for handranks
class HandRank(IntEnum):
    ROYAL_STRAIGHT_FLUSH = 0
    STRAIGHT_FLUSH = 1
    FOUR_OF_A_KIND = 2
    FULL_HOUSE = 3
    FLUSH = 4
    STRAIGHT = 5
    THREE_OF_A_KIND = 6
    TWO_PAIR = 7
    PAIR = 8
    HIGH_CARD = 9

# evaluation helpers for five cards handset - dataholder
class ComboResult: # name: type
    def __init__(self, kept, dropped, rank, key):
        self.kept = kept
        self.dropped = dropped
        self.rank = rank
        self.key = key

# convert the hand rank enum into a display string
def display_handrank(rank : HandRank) -> str:
    if rank == HandRank.ROYAL_STRAIGHT_FLUSH:   return "Royal straight flush"
    if rank == HandRank.STRAIGHT_FLUSH:         return "Straight flush"
    if rank == HandRank.FOUR_OF_A_KIND:         return "Four of a Kind"
    if rank == HandRank.FULL_HOUSE:             return "Full House"
    if rank == HandRank.FLUSH:                  return "Flush"
    if rank == HandRank.STRAIGHT:               return "Straight"
    if rank == HandRank.THREE_OF_A_KIND:        return "Three of a Kind"
    if rank == HandRank.TWO_PAIR:               return "Two Pair"
    if rank == HandRank.PAIR:                   return "Pair"
    if rank == HandRank.HIGH_CARD:              return "High Card"
    return "Unknown"

# helper methods

# suit order: D C H S
def suit_idx(s: str) -> int:
    return "DCHS".find(s.upper())

# highest suit index among cards of the given rank
def max_suit_of_rank(five: List[Card], rank: int) -> int:
    best = -1
    for c in five:
        if c.get_rank() == rank:
            best = max(best, suit_idx(c.get_suit()))
    return best

# suit index of the single card with this rank in "five"
def suit_of_rank(five: List[Card], rank: int) -> int:
    for c in five:
        if c.get_rank() == rank:
            return suit_idx(c.get_suit())
    return -1

# cards sorted by rank in descending order, then suit descending
def cards_by_rank_then_suit_desc(five: List[Card]) -> List[Card]:
    return sorted(
        five, key=lambda card: (card.get_rank(), suit_idx(card.get_suit())), reverse=True)

# check for flush
def is_flush(five: List[Card]) -> bool: 
    c = five[0].get_suit()
    for i in range(1, len(five)):
        if five[i].get_suit() != c:
            return False
    return True

# check if straight, A can be low = 5
def straight_high(five: List[Card]) -> int:
    unique_ranks = {card.get_rank() for card in five}
    if len(unique_ranks) != 5:
        return 0

    ranks = sorted(unique_ranks) # ascending
    # "A" low
    if ranks == [2, 3, 4, 5, 14]:
        return 5

    # normal consecutive
    for i in range(1, 5):
        if ranks[i] - ranks[i - 1] != 1:
            return 0
    return ranks[4] #highest rank in the straight

# rank counts
def rank_counts(five: List[Card]):
    count = [0] * 15 # 15 slots
    for c in five:
        count[c.get_rank()] += 1
    return count

# find a single card with this rank in "five"
def find_card_by_rank(five: List[Card], rank: int):
    for c in five:
        if c.get_rank() == rank:
            return c
    return None

# all cards of a rank, suit high -> low 
def of_rank_sorted_by_suit_desc(five: List[Card], rank: int):
    cards: List[Card] = []
    for c in five:
        if c.get_rank() == rank:
            cards.append(c)
    cards.sort( key=lambda card: suit_idx(card.get_suit()), reverse=True)
    return cards

# singles only. sorted rank desc then suit desc. "kickers"
def singles_sorted(five: List[Card], cnt: List[int]):
    singles: List[Card] = [] # var: hint of what type tp be and creates an empty list
    for c in five:
        if cnt[c.get_rank()] == 1:
            singles.append(c)
    return cards_by_rank_then_suit_desc(singles)

# for a straight. return cards in top -> low. "A" low handled as 5,4,3,2,A
def straight_display_order(five: List[Card]):
    top = straight_high(five)
    order: List[int] = []
    if top == 5:
        order = [5, 4, 3, 2, 14]
    else:
        for r in range(top, top - 5, -1):
            order.append(r)

    out: List[Card] = []
    for r in order:
        out.append(find_card_by_rank(five, r))
    return out
            
# order full house correclty - tiebreaker, ordering convention
def dropped_fullhouse(s: str) -> int:
    s = s.upper()
    if s == 'D': return 3
    if s == 'C': return 2
    if s == 'H': return 1
    if s == 'S': return 0
    return -1

# evaluate the 5 card hand
def evaluate_five(five: List[Card], dropped: Card) -> ComboResult:
    flush = is_flush(five)
    straight_top = straight_high(five)
    count = rank_counts(five)

    quad = 0
    trips = 0
    pairs: List[int] = [] 
    singles: List[int] = []

    for r in range(14, 1, -1):
        if count[r] == 4:
            quad = r
        elif count[r] == 3:
            trips = r
        elif count[r] == 2:
            pairs.append(r)
        elif count[r] == 1:
            singles.append(r)

    
    # straight flush / royal straight flush
    if flush and straight_top > 0:
        rank = HandRank.ROYAL_STRAIGHT_FLUSH if straight_top == 14 else HandRank.STRAIGHT_FLUSH
        flush_suit = suit_idx(five[0].get_suit())
        return ComboResult(five, dropped, rank, [straight_top, flush_suit])

    # four of a kind
    if quad != 0:
        kicker = singles[0]
        kicker_suit = suit_of_rank(five, kicker)
        return ComboResult(five, dropped, HandRank.FOUR_OF_A_KIND, [ quad, kicker, kicker_suit])

    # full house
    if trips != 0 and len(pairs) > 0:
        trip_suit = max_suit_of_rank(five, trips)
        pair_suit = max_suit_of_rank(five, pairs[0])
        dropped_pref = dropped_fullhouse(dropped.get_suit())
        return ComboResult(five, dropped, HandRank.FULL_HOUSE, [trips, pairs[0], trip_suit, pair_suit, dropped_pref]) 

    # flush
    if flush:
        sorted_five = cards_by_rank_then_suit_desc(five)
        rank_part = [c.get_rank() for c in sorted_five] 
        suits_part = [suit_idx(c.get_suit()) for c in sorted_five]
        key = rank_part + suits_part
        return ComboResult(five, dropped, HandRank.FLUSH, key)        
    
    # straight
    if straight_top > 0:
        top_rank = 5 if straight_top == 5 else straight_top
        top_suit = suit_of_rank(five, top_rank)
        return ComboResult(five, dropped, HandRank.STRAIGHT, [straight_top, top_suit])

    # three of a kind
    if trips != 0:
        k1, k2 = singles[0], singles[1]
        trip_suit = max_suit_of_rank(five, trips)
        k1_suit = suit_of_rank(five, k1)
        k2_suit = suit_of_rank(five, k2)
        return ComboResult(five, dropped, HandRank.THREE_OF_A_KIND, [trips, k1, k2, trip_suit, k1_suit, k2_suit])

    # two pair
    if len(pairs) >= 2:
        hi, lo = pairs[0], pairs[1]
        k = singles[0]
        hi_suit = max_suit_of_rank(five, hi)
        lo_suit = max_suit_of_rank(five, lo)
        k_suit = suit_of_rank(five, k)
        return ComboResult(five, dropped, HandRank.TWO_PAIR, [hi, lo, k, hi_suit, lo_suit, k_suit])

    # pair
    if len(pairs) == 1:
        p = pairs[0]
        k1, k2, k3 = singles[0], singles[1], singles[2]
        p_suit = max_suit_of_rank(five, p)
        k1_suit = suit_of_rank(five, k1) 
        k2_suit = suit_of_rank(five, k2)
        k3_suit = suit_of_rank(five, k3)
        return ComboResult(five, dropped, HandRank.PAIR, [p, k1, k2, k3, p_suit, k1_suit, k2_suit, k3_suit])

    # high card
    sorted_five = cards_by_rank_then_suit_desc(five)
    rank_part = [c.get_rank() for c in sorted_five]
    suit_part = [suit_idx(c.get_suit()) for c in sorted_five]
    key = rank_part + suit_part
    return ComboResult(five, dropped, HandRank.HIGH_CARD, key)

# reorder the kept five
def format_display(res: ComboResult) -> List[Card]:
    five = res.kept
    cnt = rank_counts(five)
    
    quad = 0
    trips = 0
    
    pairs: List[int] = []
    for r in range(14, 1, -1):
        if cnt[r] == 4:
            quad = r
        elif cnt[r] == 3:
            trips = r
        elif cnt[r] == 2:
            pairs.append(r)

    out: List[Card] = []
    
    if res.rank in (HandRank.ROYAL_STRAIGHT_FLUSH, HandRank.STRAIGHT_FLUSH, HandRank.STRAIGHT):
        return straight_display_order(five)

    if res.rank in  (HandRank.FLUSH, HandRank.HIGH_CARD):
        return cards_by_rank_then_suit_desc(five)

    if res.rank == HandRank.FOUR_OF_A_KIND:
        out.extend(of_rank_sorted_by_suit_desc(five, quad))
        out.extend(singles_sorted(five, cnt)[:1])    # kicker
        return out
    
    if res.rank == HandRank.FULL_HOUSE:
        out.extend(of_rank_sorted_by_suit_desc(five, trips))    # trips
        out.extend(of_rank_sorted_by_suit_desc(five, pairs[0])) # pair
        return out

    if res.rank == HandRank.THREE_OF_A_KIND:
        out.extend(of_rank_sorted_by_suit_desc(five, trips))    # trips
        out.extend(singles_sorted(five, cnt))                   # 2 kickers
        return out

    if res.rank == HandRank.TWO_PAIR:
        out.extend(of_rank_sorted_by_suit_desc(five, pairs[0])) # high pair
        out.extend(of_rank_sorted_by_suit_desc(five, pairs[1])) # low pair
        out.extend(singles_sorted(five, cnt)[:1])               # kicker
        return out  

    if res.rank == HandRank.PAIR:
        out.extend(singles_sorted(five, cnt))                   # 3 kickers
        out.extend(of_rank_sorted_by_suit_desc(five, pairs[0])) # pair
        return out

    return five
    
# evaluating and printing

# method for rank strength
def rank_strength(r: HandRank) -> int:
    return (len(HandRank) - 1) - int(r)

# method to compare tiebreakers key arrays, DESC
def cmp_key(a: List[int], b: List[int]) -> int:
    n = min(len(a), len(b))
    for i in range(n):
        if b[i] != a[i]:
            return 1 if b[i] > a[i] else -1

    # if all equal up to n, longer wins
    if len(b) != len(a):
        return 1 if len(b) > len(a) else -1
    return 0

# compare two ComboResult: rank first (DESC), then key (DESC)
def compare_combo(x: ComboResult, y: ComboResult) -> int:
    rx = rank_strength(x.rank)
    ry = rank_strength(y.rank)
    if ry != rx:
        return 1 if ry > rx else -1
    return cmp_key(x.key, y.key)

#find the dropped card - the 6th that is not in the kept five
def find_dropped( six: List[Card], five: List[Card]):
    for c in six:
        if c not in five:
            return c
    return None

# formatting/printing helpers

# print the handset of 6 cards
def print_hand(hand_of_six: List[Card]):
    for i in range(len(hand_of_six)):
        card = hand_of_six[i]
        print(f"{(' ' + str(card)):<4}", end="")
    print()

# build the 5 card combination by dropping each of the six cards once
def card_combination(hand_of_six: List[Card]) -> List[List[Card]]:
    dq = deque(hand_of_six)
    combos: List[List[Card]] = [] # will hold 6 five card combos
    for i in range(6):
        drop = dq.pop()
        combos.append(list(dq))
        dq.appendleft(drop)
    return combos

# print combination table
def print_combination(hand_of_six: List[Card]):
    print("\n*** Hand Combinations...")
    dq = deque(hand_of_six)

    for i in range(6):
        drop = dq.pop()
        for j in list(dq):
            print(f"{(' ' + str(j)):<4}", end="")
        print(" | ", end="")
        print(drop)
        dq.appendleft(drop)

# print high hand order
def print_high_hand_order(hand_of_six: List[Card]):
    combos = card_combination(hand_of_six)
    results: List[ComboResult] = []
    
    for five in combos:
        dropped = find_dropped(hand_of_six, five)
        results.append(evaluate_five(five, dropped))

    # sort by rank strength, then by tiebreaker key. both descend
    results.sort(key = cmp_to_key(compare_combo))

    print("\n--- HIGH HAND ORDER ---")
    for r in results:
        display = format_display(r)
        # kept five
        for c in display:
            print(f"{(' ' + str(c)):<4}", end="")
        # separator + dropped + ranked label
        print(" | ", end="")
        print(f"{r.dropped} -- {display_handrank(r.rank)}")

# text file

# method to parse one line from the text file
def parse_text_file(line : str):
    tokens = line.replace(",", " ").strip().split()
    if (len(tokens) != 6):
        return None

    hand = [] # create empty list
    try:
        for s in tokens:
            hand.append(Card.parse(s))  # build Card object
    except Exception as e:
        print("Error: ", e)
        return None
    return hand

# read the test file and process each hand
def test_handset_file(file_name: str):
    try:
        with open(file_name, "r") as reader:
            for line in reader:
                line = line.strip()
                if not line:
                    continue

                hand = parse_text_file(line)
                if hand is not None:
                    print_hand(hand)
                    if duplicate_cards_and_print(hand):
                        return
                    combos = card_combination(hand)
                    print_combination(hand)
                    print_high_hand_order(hand)
    except OSError as e:
        print("Error: ", e)

# check for duplicates in the hand of six
def duplicate_cards_and_print(hand: List[Card]) ->bool:
    seen = set()
    for c in hand:
        key = str(c)
        if key in seen:
            print("\n*** ERROR - DUPLICATED CARD FOUND IN DECK ***\n")
            print()
            print(f"*** DUPLICATE: {key} ***")
            return True
        seen.add(key)
    return False

# method to clear screen
def clear_screen():
    os.system("clear")
    sys.stdout.flush()

# main method
def main(args):
    clear_screen()
    deck = Deck()
    deck.shuffle()

    print("*** P O K E R  H A N D  A N A L Y Z E R ***\n")
    print()

    if (len(args) > 0):     # test file gamemode           
        print("*** USING TEST DECK ***\n")
        print(f"*** File: {args[0]}")
        test_handset_file(args[0])
    else:                   # random gamemode
        print("*** USING RANDOMIZED DECK OF CARDS ***\n")
        print("*** Shuffled 52 card deck:")
        deck.print_deck(False)
        hand_of_six = deck.deal_six()   # extract the six first cards
        print("\n*** Here is the hand:")
        print_hand(hand_of_six)
        hand_combination = card_combination(hand_of_six)
        print_combination(hand_of_six)
        print_high_hand_order(hand_of_six)
            
if __name__ == "__main__":
        main(sys.argv[1:]) 
