#-------------------------------------------------------
#                    Poker Hand Analyzer	       -
#                       Jaime Tovali                   -
#                        Dr. Pounds                    -
#-------------------------------------------------------

include("Card.jl")
include("Deck.jl") 

# enumeration for suits
@enum Suit DIAMONDS=0 CLUBS=1 HEARTS=2 SPADES=3

# enumeration for hand ranks
@enum HandRank ROYAL_STRAIGHT_FLUSH=0 STRAIGHT_FLUSH=1 FOUR_OF_A_KIND=2 FULL_HOUSE=3 FLUSH=4 STRAIGHT=5 THREE_OF_A_KIND=6 TWO_PAIR=7 PAIR=8 HIGH_CARD=9

# data holder for evaluated  5 card hand
mutable struct ComboResult
    kept::Vector{Card}   # 5 cards kept
    dropped::Card	 # last card dropped
    rank::HandRank       # hand ranking enum
    key::Vector{Int}     # tiebreaker key
end

# convert hand rank enum into a display string
function displayHandRank(rank::HandRank)::String
    if rank == ROYAL_STRAIGHT_FLUSH
        return "Royal Straight Flush"
    elseif rank == STRAIGHT_FLUSH
	return "Straight Flush"
    elseif rank == FOUR_OF_A_KIND
	return "Four of a Kind"
    elseif rank == FULL_HOUSE
	return "Full House"
    elseif rank == FLUSH
	return "Flush"
    elseif rank == STRAIGHT
	return "Straight"
    elseif rank == THREE_OF_A_KIND
	return "Three of a Kind"
    elseif rank == TWO_PAIR
	return "Two Pair"
    elseif rank == PAIR
	return "Pair"
    elseif rank == HIGH_CARD
	return "High Card"
    else
	return "Unknown"
    end
end

# clear screen
function clearScreen()
    run(`clear`)
    flush(stdout)
end

# print the handset of 6 cards
function printHand(handOfSix::Vector{Card})
    for i in 1:length(handOfSix)
        print(" ", cardToString(handOfSix[i]), " ")
    end
    println()
end

# build the 5 card combination by dropping each of the 6 cards once
function cardCombination(handOfSix::Vector{Card})::Vector{Vector{Card}}
    combos = Vector{Vector{Card}}()   # will hold 6 five card combos
    for i in 1:6
        # create a copy of the hand and remove the i card
	combo = copy(handOfSix)
        deleteat!(combo, i)
	push!(combos, combo)
    end
    return combos
end 

# print combination table
function printCombination(handOfSix::Vector{Card})
    print("\n*** Hand Combinations...\n")
    handToShift = copy(handOfSix)

    for i in 1:6
	droppedCard = handToShift[end]
	# printing the remaining five
	for j in 1:5
	    print(rpad(cardToString(handToShift[j]), 4))
	end
	print(" | ")
        println(lpad(cardToString(droppedCard), 4))
	# moves last element to the first position
	handToShift = circshift(handToShift, 1)
    end
end

# parse one line from the text file
function parseTextFile(line::AbstractString)
    tokens = split(strip(replace(line, "," => " ")))
    if length(tokens) != 6
	return nothing
    end

    hand = Card[] 
    try
    	for s in tokens
	    push!(hand, parseCard(s))
        end
    catch e
	println("Error: ", e)
	return nothing
    end
    return hand
end

# check for duplicates in the hand of six
function duplicateCardsAndPrint(hand)
    seen = Set{String}()
    for c in hand
	key = cardToString(c)
	if key in seen
	    println("\n*** ERROR - DUPLICATED CARD FOUND IN DECK ***\n")
	    println("*** DUPLICATE: $key ***")
	    return true
	end
	push!(seen, key)
    end
    return false
end 

# read the test file and process each hand
function testHandsetFile(fileName::String)
    try
	for line in eachline(fileName)
	    s = strip(line)
	   # isempty(s) && continue
	
	    hand = parseTextFile(s)
	    if hand !== nothing
	        printHand(hand)
		if duplicateCardsAndPrint(hand)
		    return
		end
	#	combos = cardCombination(hand)
		printCombination(hand)
		printHighHandOrder(hand)
	    end
	end
    catch e
        println("Error:", e)
    end
end

#suit order: D C H S
function suitIdx(s::Char)::Int
    return findfirst(==(uppercase(s)), ['D', 'C', 'H', 'S'])-1
end

# highest suit index among cards of the given rank
function maxSuitOfRank(five::Vector{Card}, rank::Int)::Int
    best = -1
    for c in five
	if c.rank == rank
	    best = max(best, suitIdx(c.suit))
	end
    end
    return best
end

# suit index of the single card with this rank in "five"
function suitOfRank(five::Vector{Card}, rank::Int)::Int
    for c in five
	if c.rank == rank
	    return suitIdx(c.suit)
	end
    end
    return -1
end

# cards sorted by rank in descending order, then suit descending
function cardsByRankThenSuitDesc(five::Vector{Card})::Vector{Card}
    copyFive = copy(five)
    sort!(copyFive, by = c -> (c.rank, suitIdx(c.suit)), rev = true)
    return copyFive
end

# check if its flush
function isFlush(five::Vector{Card})::Bool
    suit = five[1].suit
    for i in 2:length(five)
	if five[i].suit != suit
	    return false
	end
    end
    return true
end

# check if straight, A can be low = 5
function straightHigh(five::Vector{Card})::Int
    ranksList = [c.rank for c in five]  # extract ranks
    uniqueRanks = unique(ranksList)
    if length(uniqueRanks) != 5         # must have 5 unique ranks
	return 0
    end
    
    # sort ascending
    ranks = sort(uniqueRanks)
    
    # if A-low straight
    if ranks == [2, 3, 4, 5, 14]
	return 5
    end

    # normal consecutive
    for i in 2:5
	if ranks[i] - ranks[i - 1] != 1
	    return 0
	end
    end
    return ranks[end]  # highest rank in the straight
end

# count the ranks in five
function rankCounts(five::Vector{Card})
    count = fill(0, 15) # 15 slots
    for c in five
	count[c.rank] += 1
    end
    return count
end

# find a single card with this rank in "five"
function findCardByRank(five::Vector{Card}, rank::Int)
    for c in five
	if c.rank == rank
	    return c
	end
    end
    return nothing
end

# all cards of a given rank, sorted by suit high -> low
function ofRankSortedBySuitDesc(five::Vector{Card}, rank::Int)
    cards = Card[]
    for c in five
	if c.rank == rank
	    push!(cards, c)
	end
    end
    sort!(cards, by = c -> suitIdx(c.suit), rev = true)
    return cards
end

# singles only, sorted rank desc then suit desc - "kickers"
function singlesSorted(five::Vector{Card}, cnt)
    singles = Card[]
    for c in five
	if cnt[c.rank] ==  1
 	    push!(singles, c)
	end
    end
    return cardsByRankThenSuitDesc(singles)
end

# for a straight. return cards in top -> low. "A" low is 5, 4, 3, 2, A
function straightDisplayOrder(five::Vector{Card})
    top = straightHigh(five)
    order = Int[]

    # build the target rank order
    if top == 5
    	order = [5, 4, 3, 2, 14]
    else
    	# normal straight
	for r in top:-1:(top - 4)
	    push!(order, r)
	end
    end

    # map ranks to the actual Card objects in that order
    out = Card[]
    for r in order
	c = findCardByRank(five, r)
	if c !== nothing
	    push!(out, c)
	end
    end
    return out
end

# order full house correctly - tiebreaker, ordering convention
function droppedFullhouse(s::Char)::Int
    s = uppercase(s)
    if s == 'C'
    	return 3
    elseif s == 'D'
	return 2
    elseif s == 'H'
	return 1
    elseif s == 'S'
	return 0
    else
	return -1
    end
end

# evaluate the 5 card hand
function evaluateFive(five::Vector{Card}, dropped::Card)::ComboResult
    flush = isFlush(five)
    straightTop = straightHigh(five)
    count = rankCounts(five)

    quad = 0
    trips = 0
    pairs = Int[]
    singles = Int[]

    for r in 14:-1:2
    	if count[r] == 4
   	    quad = r
     	elseif count[r] == 3
	    trips = r
	elseif count[r] == 2
	    push!(pairs, r)
	elseif count[r] == 1
 	    push!(singles, r)
	end
    end

   # straight flush / royal straight flush
   if flush && straightTop > 0
	rank = (straightTop == 14) ? ROYAL_STRAIGHT_FLUSH : STRAIGHT_FLUSH
	flushSuit = suitIdx(five[1].suit)  # all same suit, take the first
        return ComboResult(five, dropped, rank, [straightTop, flushSuit])
   end 

   # four of a kind
   if quad != 0
	kicker = singles[1]
 	kickerSuit = suitOfRank(five, kicker)
	return ComboResult(five, dropped, FOUR_OF_A_KIND, [quad, kicker, kickerSuit])
   end

   # full house
   if trips != 0 && length(pairs) > 0
   	tripSuit = maxSuitOfRank(five, trips)
	pairSuit = maxSuitOfRank(five, pairs[1])
        droppedPref = droppedFullhouse(dropped.suit)
	return ComboResult(five, dropped, FULL_HOUSE, [trips, pairs[1], tripSuit, pairSuit, droppedPref])
   end

   # flush  
   if flush
	sortedFive = cardsByRankThenSuitDesc(five)
	rankPart = [c.rank for c in sortedFive]
	suitsPart = [suitIdx(c.suit) for c in sortedFive] 
  	key = [rankPart; suitsPart]
	return ComboResult(five, dropped, FLUSH, key)
   end

   # straight
   if straightTop > 0
	topRank = (straightTop == 5) ? 5 : straightTop
	topSuit = suitOfRank(five, topRank)
	return ComboResult(five, dropped, STRAIGHT, [straightTop, topSuit])
   end

   # three of a kind
   if trips != 0
	k1 = singles[1]
	k2 = singles[2]
	tripSuit = maxSuitOfRank(five, trips)
	k1Suit = suitOfRank(five, k1)
	k2Suit = suitOfRank(five, k2)
	return ComboResult(five, dropped, THREE_OF_A_KIND, [trips, k1, k2, tripSuit, k1Suit, k2Suit])
   end	 	 

   # two pair
   if length(pairs) >= 2
	hi = pairs[1]
 	lo = pairs[2]
   	k = singles[1]
	hiSuit = maxSuitOfRank(five, hi)
	loSuit = maxSuitOfRank(five, lo)
	kSuit = suitOfRank(five, k)
	return ComboResult(five, dropped, TWO_PAIR, [hi, lo, k, hiSuit, loSuit, kSuit])
   end 

   # pair
   if length(pairs) == 1
	p = pairs[1]
	k1 = singles[1]
	k2 = singles[2]
	k3 = singles[3]
	pSuit = maxSuitOfRank(five, p)
	k1Suit = suitOfRank(five, k1)
	k2Suit = suitOfRank(five, k2)
	k3Suit = suitOfRank(five, k3)
	return ComboResult(five, dropped, PAIR, [p, k1, k2, k3, pSuit, k1Suit, k2Suit, k3Suit])
   end

   # high card
   sortedFive = cardsByRankThenSuitDesc(five)
   rankPart = [c.rank for c in sortedFive]
   suitPart = [suitIdx(c.suit) for c in sortedFive]
   key = [rankPart; suitPart]
   return ComboResult(five, dropped, HIGH_CARD, key)
end

# reorder the kept five for display
function formatDisplay(res::ComboResult)::Vector{Card}
   five = res.kept
   cnt = rankCounts(five)
   quad = 0
   trips = 0
   pairs = Int[]

   # collect quad/trips/pairs from high -> low
   for r in 14:-1:2
	if cnt[r] == 4
	   quad = r
	elseif cnt[r] == 3
	   trips = r
	elseif cnt[r] == 2
	   push!(pairs, r)
	end
   end

   # straights - flush/royal
   if res.rank in (ROYAL_STRAIGHT_FLUSH, STRAIGHT_FLUSH, STRAIGHT)
	return straightDisplayOrder(five)
   end

   # flush or high card
   if res.rank in (FLUSH, HIGH_CARD)
	return cardsByRankThenSuitDesc(five)
   end

   # four of a kind and 1 kicker
   if res.rank == FOUR_OF_A_KIND
	out = ofRankSortedBySuitDesc(five, quad)
	kickers = singlesSorted(five, cnt)
        push!(out, kickers[1]) 	# take the top kicker
	return out
   end

   # full house: trips then pair
   if res.rank == FULL_HOUSE
	out = ofRankSortedBySuitDesc(five, trips)
	pairsToUse = ofRankSortedBySuitDesc(five, pairs[1])
	append!(out, pairsToUse)
	return out
   end

   # three of a kind: trips then both kickers
   if res.rank == THREE_OF_A_KIND
	out = ofRankSortedBySuitDesc(five, trips)
        singlesToUse = singlesSorted(five, cnt)	# 2 kickers
	append!(out, singlesToUse)
	return out
   end

   # two pairs: high pair, low pair, and kicker
   if res.rank == TWO_PAIR
	out = ofRankSortedBySuitDesc(five, pairs[1])
	lowPair = ofRankSortedBySuitDesc(five, pairs[2])
	append!(out, lowPair) 
	kickers = singlesSorted(five, cnt)
	push!(out, kickers[1])	# top kicker
	return out
   end

   # one pair: 3 kickers then pairs - high -> low
   if res.rank == PAIR
	out = singlesSorted(five, cnt)	# 3 kickers
	append!(out, ofRankSortedBySuitDesc(five, pairs[1])) # combine pair
        return out
   end

   return five
end

# rank strength
const HANDRANK_MAX = 9
function rankStrength(r::HandRank)::Int
   return HANDRANK_MAX - Int(r)
end

# compare tiebreakers key arrays, desc
function cmpKey(a::Vector{Int}, b::Vector{Int})::Int
   n = min(length(a), length(b))
   for i in 1:n
	if b[i] != a[i]
	   return (b[i] > a[i]) ? 1 : -1
	end
   end

   # if all equal up to n, longer wins
   if length(b) != length(a)
	return (length(b) > length(a)) ? 1 : -1
   end
   return 0
end

# compare two ComboResult - rank first (desc), then key (desc)
function compareCombo(x::ComboResult, y::ComboResult)::Int
   rx = rankStrength(x.rank)
   ry = rankStrength(y.rank)
   if ry != rx
	return (ry > rx) ? 1 : -1  # descend by strength y > x
   end
   return cmpKey(x.key, y.key)  # descend by key
end

# find the dropped card - the 6th that is not in kept five
function findDropped(six::Vector{Card}, five::Vector{Card})
   for c6 in six
	found = any(c6.rank == c5.rank && c6.suit == c5.suit for c5 in five)
	if !found
	   return c6
	end
   end
   return nothing
end

# print high hand order
function printHighHandOrder(handOfSix::Vector{Card})
   results = ComboResult[]
   handToShift = copy(handOfSix)

   # evaluate each 5 card combo
   for i in 1:6
	five = handToShift[1:5]
	dropped = handToShift[end]
    	push!(results, evaluateFive(five, dropped))
	handToShift = circshift(handToShift, 1)
   end

   # sort by rank strength - desc, then key - desc
   lt = (a, b) -> compareCombo(a, b) == -1
   sort!(results; lt=lt) # stable sort

   println("\n--- HIGH HAND ORDER ---")
   for r in results
	display = formatDisplay(r)
        # kept five
      	for c in display
	   print(rpad(cardToString(c), 4))
	end
     	
	# separator + dropped + rankedLabel
	print(" | ")
	println(lpad(cardToString(r.dropped), 4), " -- ", displayHandRank(r.rank))
   end
end
# main function
function main(args)
    clearScreen()
    deck = Deck()
    shuffleDeck(deck)

    println("*** P O K E R  H A N D  A N A L Y Z E R ***\n")

    if length(args) > 0
       println("*** USING TEST DECK ***\n")
       println("*** File: $(args[1])")
       testHandsetFile(args[1])
    else
    print("*** USING RANDOMIZED DECK OF CARDS ***\n")
    print("\n*** Shuffled 52 card deck:\n")
    printDeck(deck, false)

    handOfSix = dealSix(deck)
    print("\n*** Here is the hand:\n")
    printHand(handOfSix)

    printCombination(handOfSix)
    printHighHandOrder(handOfSix)
    end
end

# this is to run code
if abspath(PROGRAM_FILE) == @__FILE__
    main(ARGS)
end
