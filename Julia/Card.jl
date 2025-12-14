# Card.jl

# define global constants for suit
const SUITS = ['D', 'C', 'H', 'S']

# Card structure
mutable struct Card
    rank::Int
    suit::Char
end

# convert card value into string
function cardToString(card::Card)
    rankStr = ""
    if card.rank == 14
        rankStr = "A"
    elseif card.rank == 13
	rankStr = "K"
    elseif card.rank == 12
	rankStr = "Q"
    elseif card.rank == 11
	rankStr = "J"
    else
        rankStr = string(card.rank)
    end
    return rankStr * string(card.suit)
end

# parse a card from a textfile
function parseCard(cardStr::AbstractString)::Card
    cardStr = strip(uppercase(cardStr))
    suit = cardStr[end]
    rankPart = strip(cardStr[1:end-1])
    rank = 0

    if rankPart == "A"
        rank = 14
    elseif rankPart == "K"
	rank = 13
    elseif rankPart == "Q"
 	rank = 12
    elseif rankPart ==  "J"
	rank = 11
    else
      	rank = parse(Int, rankPart)
    end
    return Card(rank, suit)
end	
