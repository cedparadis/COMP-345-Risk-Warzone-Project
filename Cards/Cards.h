#pragma once

#include <vector>
#include <iostream>

#define DECK_SIZE 50
#define HAND_SIZE 5

class Hand;
class Deck;

class Card {
    public:
        enum CardType {
            Airlift,
            Blockade,
            Bomb,
            Diplomacy,
            Reinforcement,
            LAST = 5
        };        

        Card(CardType);
        Card(const Card&);
        ~Card();

        CardType getCardType() const;
        std::string cardTypeToString() const;

        void play(std::vector<Card*> &, Deck*, Hand*);

        Card& operator=(const Card&);
        friend std::ostream& operator<<(std::ostream&, const Card&);
    
    private:
        CardType* type;
};

class Hand {
    public:  
        std::vector<Card*> cards;      
        Hand();
        Hand(const Hand&);
        ~Hand();

        int getHandSize();
       
        void addCard(Card*);      

        Hand& operator=(const Hand&);
        friend std::ostream& operator<<(std::ostream&, const Hand&);
        
    private: 
};

class Deck {
    public:
        Deck();
        Deck(const Deck&);
        ~Deck();

        Card* draw();
        void addCard(Card*);

        Deck& operator=(const Deck&);
        friend std::ostream& operator<<(std::ostream&, const Deck&);

    private:
        std::vector<Card*> cards;
};
