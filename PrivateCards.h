//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_PRIVATECARDS_H
#define TEXASSOLVER_PRIVATECARDS_H
#include "Card.h"

class PrivateCards {
public:

    int card1{};
    int card2{};
    PrivateCards();
    PrivateCards(int card1, int card2);
    PrivateCards(const string& str);
    uint64_t toBoardLong() const; //xzy
    int hashCode();
    string toString() const;

    const vector<int> & get_hands() const; //xzy

    bool operator==(const PrivateCards& other) const {
        return this->toBoardLong() == other.toBoardLong();
    }

private:
    vector<int> card_vec;
    int hash_code{};
    uint64_t board_long;
};


#endif //TEXASSOLVER_PRIVATECARDS_H
