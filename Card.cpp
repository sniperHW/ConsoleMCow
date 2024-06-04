
#include "Card.h"

Card::Card(){this->card = "empty";}
Card::Card(string card,int card_num_in_deck){
    this->card = card;
    this->card_int = Card::strCard2int(this->card);
    this->card_number_in_deck = card_num_in_deck;
}

Card::Card(string card){
    this->card = card;
    this->card_int = Card::strCard2int(this->card);
    this->card_number_in_deck = -1;
}

bool Card::empty(){
    if(this->card == "empty")return true;
    else return false;
}

string Card::getCard() {
    return this->card;
}

int Card::getCardInt() {
    return this->card_int;
}

int Card::getNumberInDeckInt(){
    if(this->card_number_in_deck == -1)throw runtime_error("card number in deck cannot be -1");
    return this->card_number_in_deck;
}

int Card::card2int(Card card) {
    return strCard2int(card.getCard());
}

int Card::strCard2int(string card) {
    char rank = card.at(0);
    char suit = card.at(1);
    if(card.length() != 2){
        throw runtime_error( tfm::format("card %s not found",card));
    }
    return (rankToInt(rank) - 2) * 4 + suitToInt(suit);
}

string Card::intCard2Str(int card){
    int rank = card / 4 + 2;
    int suit = card - (rank-2)*4;
    return Card::rankToString(rank) + Card::suitToString(suit);
}

uint64_t Card::boardCards2long(vector<string> cards) {
    vector<Card> cards_objs(cards.size());
    for(int i = 0;i < cards.size();i++){
        cards_objs[i] = Card(cards[i]);
    }
    return Card::boardCards2long(cards_objs);
}

uint64_t Card::boardCards2long(const string& cards) {
    vector<string> v_cards;
    for (int i = 0; i < cards.size(); i+=2) {
        v_cards.push_back(cards.substr(i,2));
    }

    return boardCards2long(v_cards);

}

uint64_t Card::boardCard2long(Card& card){
    return Card::boardInt2long(card.getCardInt());
}

uint64_t Card::boardCards2long(vector<Card>& cards){
    std::vector<int> board_int(cards.size());
    for(int i = 0;i < cards.size();i++){
        board_int[i] = Card::card2int(cards[i]);
    }
    return Card::boardInts2long(board_int);
}



uint64_t Card::boardInt2long(int board){


    if(board < 0 || board >= 52)
        throw runtime_error(tfm::format("Card with id %s not found",board));

    return ((uint64_t)(1) << board);
}

uint64_t Card::boardInts2long(const vector<int>& board){
    if(board.size() < 1 || board.size() > 7){
        throw runtime_error(tfm::format("Card length incorrect: %s",board.size()));
    }
    uint64_t board_long = 0;
    for(int one_card:board){

        if (one_card < 0 || one_card >= 52) 
            throw runtime_error(tfm::format("Card with id %s not found", one_card));
        
        

        board_long += ((uint64_t)(1) << one_card);
    }
    return board_long;
}

/*
long Card::privateHand2long(PrivateCards one_hand){
    return boardInts2long(new int[]{one_hand.card1,one_hand.card2});
}
 */

vector<int> Card::long2board(uint64_t board_long) {
    vector<int> board;
    board.reserve(7);
    for(int i = 0;i < 52;i ++){

        if((board_long & 1) == 1){
            board.push_back(i);
        }
        board_long = board_long >> 1;
    }
    if (board.size() < 1 || board.size() > 7){
        throw runtime_error(tfm::format("board length not correct, board length %s",board.size()));
    }
    return board;
}

vector<Card> Card::long2boardCards(uint64_t board_long){
        vector<int> board = long2board(board_long);
        vector<Card> board_cards(board.size());
        for(int i = 0;i < board.size();i ++){
            int one_board = board[i];
            board_cards[i] = Card(intCard2Str(one_board));
        }
        if (board_cards.size() < 1 || board_cards.size() > 7){
            throw runtime_error(tfm::format("board length not correct, board length %s",board_cards.size()));
        }
        vector<Card> retval(board_cards.size());
        for(int i = 0;i < board_cards.size();i ++){
            retval[i] = board_cards[i];
        }
        return retval;
}

string Card::suitToString(int suit)
{
    switch(suit)
    {
        case 0: return "c";
        case 1: return "d";
        case 2: return "h";
        case 3: return "s";
        default: return "c";
    }
}

string Card::rankToString(int rank)
{
    switch(rank)
    {
        case 2: return "2";
        case 3: return "3";
        case 4: return "4";
        case 5: return "5";
        case 6: return "6";
        case 7: return "7";
        case 8: return "8";
        case 9: return "9";
        case 10: return "T";
        case 11: return "J";
        case 12: return "Q";
        case 13: return "K";
        case 14: return "A";
        default: return "2";
    }
}

int Card::rankToInt(char rank)
{
    switch(rank)
    {
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'T': return 10;
        case 'J': return 11;
        case 'Q': return 12;
        case 'K': return 13;
        case 'A': return 14;
        default: return 2;
    }
}

int Card::suitToInt(char suit)
{
    switch(suit)
    {
        case 'c': return 0; 
        case 'd': return 1; 
        case 'h': return 2; 
        case 's': return 3; 
        default: return 0;
    }
}

vector<string> Card::getSuits(){
    return {"c","d","h","s"};
}

string Card::toString() {
    return this->card;
}

string Card::board2string(uint64_t board_long){
    vector<Card> boardCards = Card::long2boardCards(board_long);
    string sBoard;
    for(auto r:boardCards)
        sBoard += r.toString();
    return sBoard;
}