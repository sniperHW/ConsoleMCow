//
// Created by Xuefeng Huang on 2020/1/31.
//

#include "PrivateCards.h"

PrivateCards::PrivateCards() {
}

PrivateCards::PrivateCards(int card1, int card2) {
    this->card1 = card1;
    this->card2 = card2;

    this->card_vec = vector<int>{this->card1,this->card2};
    if (card1 > card2){
        this->hash_code = card1 * 52 + card2;
    }else{
        this->hash_code = card2 * 52 + card1;
    }
    this->board_long = Card::boardInts2long(this->card_vec);
}

PrivateCards::PrivateCards(const string& str)
{
    string sCard1 = str.substr(0, 2);
    string sCard2 = str.substr(2, 2);

    int card1 = Card(sCard1).getCardInt();
    int card2 = Card(sCard2).getCardInt();

    this->card1 = card1;
    this->card2 = card2;

    this->card_vec = vector<int>{ this->card1,this->card2 };
    if (card1 > card2) {
        this->hash_code = card1 * 52 + card2;
    }
    else {
        this->hash_code = card2 * 52 + card1;
    }
    this->board_long = Card::boardInts2long(this->card_vec);
}

uint64_t PrivateCards::toBoardLong() const{
    return this->board_long;
    //return Card::boardInts2long(this->card_vec);
}

int PrivateCards::hashCode() {
    return this->hash_code;
}


string PrivateCards::toString() const{
    if (card1 > card2) {
        return Card::intCard2Str(card1) + Card::intCard2Str(card2);
    }else{
        return Card::intCard2Str(card2) + Card::intCard2Str(card1);
    }
}

const vector<int> & PrivateCards::get_hands() const {
    return this->card_vec;
}
