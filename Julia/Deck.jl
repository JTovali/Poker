# Deck.jl

using Random

mutable struct Deck
    cards::Vector{Card}
end

# constructor that creates a 52 card deck
function Deck()
    cards = Card[]
    suits = ['D', 'C', 'H', 'S']
    for i in suits
        for rank in 2:14
    	    push!(cards, Card(rank, i))
	end
    end
    return Deck(cards)
end

# random function
function shuffleDeck(deck::Deck)
    shuffle!(deck.cards)
end

# deal six card for handset
function dealSix(deck::Deck)
    hand = deck.cards[1:6]
    deleteat!(deck.cards, 1:6)
    return hand
end

# print deck
function printDeck(deck::Deck, singleLine::Bool)
    for i in 1:length(deck.cards)
        print(cardToString(deck.cards[i]))
	endOfRow = (!singleLine) && (i % 13 == 0)
	if endOfRow
	    println()
	elseif i < length(deck.cards)
	    print(" ")
	end
    end
    println()
end
