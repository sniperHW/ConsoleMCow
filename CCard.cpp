//#include "pch.h"
#include "CCard.h"

using namespace std;

static char __DEBUG_FILE__[] = "card";

CCard::CCard()
{
    m_cardSuit = n;
}

CCard::CCard(const string& sSymbol)
{
    m_sSymbol = sSymbol;
    m_cardSuit = n;
    GetCardDetail(m_nCardPoint, m_cardSuit, sSymbol);
}

void CCard::setCard(const std::string& sSymbol)
{
    m_sSymbol = sSymbol;
    GetCardDetail(m_nCardPoint, m_cardSuit, sSymbol);
}

void CCard::GetCardDetail(int& nCardPoint, Suit& cardSuit, const string& sSymbol)
{
    switch (sSymbol[0])
    {
    case '2':
        nCardPoint = 2;
        break;
    case '3':
        nCardPoint = 3;
        break;
    case '4':
        nCardPoint = 4;
        break;
    case '5':
        nCardPoint = 5;
        break;
    case '6':
        nCardPoint = 6;
        break;
    case '7':
        nCardPoint = 7;
        break;
    case '8':
        nCardPoint = 8;
        break;
    case '9':
        nCardPoint = 9;
        break;
    case 'T':
        nCardPoint = 10;
        break;
    case 'J':
        nCardPoint = 11;
        break;
    case 'Q':
        nCardPoint = 12;
        break;
    case 'K':
        nCardPoint = 13;
        break;
    case 'A':
        nCardPoint = 14;
        break;
    }

    switch (sSymbol[1])
    {
    case 'h':
        cardSuit = h;
        break;
    case 'd':
        cardSuit = d;
        break;
    case 'c':
        cardSuit = c;
        break;
    case 's':
        cardSuit = s;
        break;
    }
}
char  CCard::CharFromPoint(int iCard)
{
    char cCard = 'n';
    switch (iCard)
    {
    case 2:
        cCard = '2';
        break;
    case 3:
        cCard = '3';
        break;
    case 4:
        cCard = '4';
        break;
    case 5:
        cCard = '5';
        break;
    case 6:
        cCard = '6';
        break;
    case 7:
        cCard = '7';
        break;
    case 8:
        cCard = '8';
        break;
    case 9:
        cCard = '9';
        break;
    case 10:
        cCard = 'T';
        break;
    case 11:
        cCard = 'J';
        break;
    case 12:
        cCard = 'Q';
        break;
    case 13:
        cCard = 'K';
        break;
    case 14:
        cCard = 'A';
        break;
    default:
        cCard = 'n';
    }
    return cCard;
}
char CCard::PointFromChar(char cCard)
{
    char iCard = -1;
    switch (cCard)
    {
    case '2':
        iCard = (char)2;
        break;
    case '3':
        iCard = (char)3;
        break;
    case '4':
        iCard = (char)4;
        break;
    case '5':
        iCard = (char)5;
        break;
    case '6':
        iCard = (char)6;
        break;
    case '7':
        iCard = (char)7;
        break;
    case '8':
        iCard = (char)8;
        break;
    case '9':
        iCard = (char)9;
        break;
    case 'T':
    case 't':
        iCard = (char)10;
        break;
    case 'J':
    case 'j':
        iCard = (char)11;
        break;
    case 'Q':
    case 'q':
        iCard = (char)12;
        break;
    case 'K':
    case 'k':
        iCard = (char)13;
        break;
    case 'A':
    case 'a':
        iCard = (char)14;
        break;
    default:
        iCard = -1;
    }
    return iCard;
}

Suit CCard::SuitFromChar(char cCardSuit)
{
    Suit  eSuit = n;
    switch (cCardSuit)
    {
    case 'H':
    case 'h':
        eSuit = h;
        break;
    case 'D':
    case 'd':
        eSuit = d;
        break;
    case 'C':
    case 'c':
        eSuit = c;
        break;
    case 'S':
    case 's':
        eSuit = s;
        break;
    default:
        break;
    }
    return eSuit;
}

char CCard::CharacterFromsuit(Suit nCardSuit)
{
    char  cRet = 'n';
    switch (nCardSuit)
    {
    case h:
        cRet = 'h';
        break;
    case d:
        cRet = 'd';
        break;
    case c:
        cRet = 'c';
        break;
    case s:
        cRet = 's';
        break;
    default:
        break;
    }
    return cRet;
}