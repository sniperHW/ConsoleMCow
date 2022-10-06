//#include "pch.h"
#include "CBoard.h"
#include "CCard.h"
#include <string>
#include <cstring>

using namespace std;

static char __DEBUG_FILE__[] = "board";

void CBoard::SetFlop(const string& sSymbol)
{
    //m_flopCards.push_back
    char cCard[7] = { 0 };

    memcpy(cCard, sSymbol.c_str(), 6);
    cCard[6] = '\0';
    SortThreeCard(cCard);
    string sFlopCard(cCard);
    //sSymbol = sTemp;

    CCard card;
    card.setCard(sFlopCard.substr(0, 2));
    m_flopCards.push_back(card);

    card.setCard(sFlopCard.substr(2, 2));
    m_flopCards.push_back(card);

    card.setCard(sFlopCard.substr(4, 2));
    m_flopCards.push_back(card);

    ConvetIsomorphism();	//设置同构
}

void CBoard::SetTurn(const string& sSymbol)
{
    CCard card;
    card.setCard(sSymbol);
    m_flopCards.push_back(card);
}

void CBoard::SetRiver(const string& sSymbol)
{
    CCard card;
    card.setCard(sSymbol);
    m_flopCards.push_back(card);
}

string CBoard::GetBoardSymbol()
{
    string s;
    for (auto it : m_flopCards)
        s += it.m_sSymbol;
    return s;
}


void  CBoard::SortThreeCard(char* csCards)
{
    CompareAndSwapCard(csCards, csCards + 2);
    CompareAndSwapCard(csCards + 2, csCards + 4);
    CompareAndSwapCard(csCards, csCards + 2);
    char temp;

    if (csCards[0] == csCards[2] && csCards[0] == csCards[4]) // 三张牌大小相同，花色一定不同
    {
    }
    else if (csCards[0] == csCards[2]) //hdh,hdc ，1，2 两张牌大小相同
    {
        if (csCards[3] == csCards[5]) //2,3花色相同，改为1，3花色相同，1，2调换花色，因为我们的同构只有 hdh ， 这时候调换1，2  两张牌
        {
            temp = csCards[1];
            csCards[1] = csCards[3];
            csCards[3] = temp;
        }
    }
    else if (csCards[2] == csCards[4]) //hhd ， 如果2，3两张牌大小相同，2，3花色不同，如果其中一张与第一张花色相同，必须放在第二张的位置，就可以用同构hhd 
    {
        if (csCards[1] == csCards[5]) //1,3花色相同,改为1，2花色相同，2，3调换花色
        {
            temp = csCards[3];
            csCards[3] = csCards[5];
            csCards[5] = temp;
        }
    }
}

void CBoard::ConvetIsomorphism()
{
    char cCard[7] = { 0 };
    string sSymbol = GetBoardSymbol();

    memcpy(cCard, sSymbol.c_str(), 6);
    cCard[6] = '\0';
    GetIsomorphismNodeName(&cCard[0]);
}

const string& CBoard::GetIsomorphismSymbol()
{
    return m_sIsomorphismBoard;
}

const SuitReplace& CBoard::GetIsomorphismSuitReplace()
{
    return m_suitReplace;
}

void CBoard::ClearSuitReplace()
{
    m_suitReplace.blIsNeeded = false;
}


void CBoard::CompareAndSwapCard(char* cCard1, char* cCard2)
{
    if (CCard::PointFromChar(cCard1[0]) > CCard::PointFromChar(cCard2[0]))
    {
        char TempCard[2] = { 0,0 };
        TempCard[0] = cCard1[0];
        TempCard[1] = cCard1[1];

        cCard1[0] = cCard2[0];
        cCard1[1] = cCard2[1];

        cCard2[0] = TempCard[0];
        cCard2[1] = TempCard[1];
    }
}

void CBoard::CompareAndSwapCard(CCard& cCard1, CCard& cCard2)
{
    if (cCard1.m_nCardPoint > cCard2.m_nCardPoint)
    {
        CCard TempCard = { 0 };
        TempCard = cCard1;
        cCard1 = cCard2;
        cCard2 = TempCard;
    }
}

/*本函数是对桌面上三张牌三个不同的花色进行通过转换，
牌按照从小到大排序后转换为分别转换为hdc三种花色，三张牌里没有的那个花色转换为s
其中cFlopReplace应该为hdc，因为同构里三个不同花色我们就只用了hdc*/
void CBoard::ConvertCardSuit3(const char* cFlopSuitOri)
{
    int iSuit[4] = { 0,0,0,0 };
    int iTheFourthSuit = -1;
    int iIndex = 0;
    char cFlopReplace[3] = { 'h','d','c' };

    if (cFlopSuitOri[0] == cFlopReplace[0] && cFlopSuitOri[1] == cFlopReplace[1]
        && cFlopSuitOri[2] == cFlopReplace[2])
    {
        m_suitReplace.blIsNeeded = false;
        m_suitReplace.h2 = h;
        m_suitReplace.c2 = c;
        m_suitReplace.d2 = d;
        m_suitReplace.s2 = s;
        return;
    }

    m_suitReplace.blIsNeeded = true;

    for (int i = 0; i < 3; i++)
    {
        switch (cFlopSuitOri[i])
        {
        case 'H':
        case 'h':
            iSuit[0] = 1;
            m_suitReplace.h2 = CCard::SuitFromChar(cFlopReplace[i]);
            break;
        case 'D':
        case 'd':
            iSuit[1] = 1;
            m_suitReplace.d2 = CCard::SuitFromChar(cFlopReplace[i]);
            break;
        case 'C':
        case 'c':
            iSuit[2] = 1;
            m_suitReplace.c2 = CCard::SuitFromChar(cFlopReplace[i]);
            break;
        case 'S':
        case 's':
            iSuit[3] = 1;
            m_suitReplace.s2 = CCard::SuitFromChar(cFlopReplace[i]);
            break;
        }
    }
    for (iIndex = 0; iIndex < 4; iIndex++)
    {
        if (0 == iSuit[iIndex])
        {
            break;
        }
    }
    switch (iIndex) //三个花色分别同构到了h,d,c，没有出现的那个花色对应到s
    {
    case 0:
        m_suitReplace.h2 = s;
        break;
    case 1:
        m_suitReplace.d2 = s;
        break;
    case 2:
        m_suitReplace.c2 = s;
        break;
    case 3:
        m_suitReplace.s2 = s;
        break;
    default:
        break;
    }
    return;
}

/*本函数是对桌面上三张牌只有两个个不同的花色进行通过转换，
牌按照从小到大排序后转换为分别转换为hdh,hhd 和dhh三种形式，去匹配同构1755种中的一种*/
void CBoard::ConvertCardSuit2(const char* cFlopOri, const char* cFlopReplace)
{
    int iSuit[4] = { 0,0,0,0 };
    int iTwoCardStartIndex;

    if (cFlopOri[0] == cFlopReplace[0] && cFlopOri[1] == cFlopReplace[1]
        && cFlopOri[2] == cFlopReplace[2])
    {
        m_suitReplace.blIsNeeded = false;
        m_suitReplace.h2 = h;
        m_suitReplace.c2 = c;
        m_suitReplace.d2 = d;
        m_suitReplace.s2 = s;
        return;
    }
    m_suitReplace.blIsNeeded = true;
    if (cFlopOri[0] == cFlopOri[2])//hdh
        iTwoCardStartIndex = 0;
    else if (cFlopOri[0] == cFlopOri[1])//hhd
        iTwoCardStartIndex = 1;
    else if (cFlopOri[1] == cFlopOri[2])//dhh
        iTwoCardStartIndex = 0;

    //只进行两个花色的转换，cFlopReplace中只有h和d两个花色
    for (int i = iTwoCardStartIndex; i <= iTwoCardStartIndex + 1; i++)
    {
        switch (cFlopOri[i])
        {
        case 'H':
        case 'h':
            iSuit[0] = 1;
            m_suitReplace.h2 = CCard::SuitFromChar(cFlopReplace[i]);
            break;
        case 'D':
        case 'd':
            iSuit[1] = 1;
            m_suitReplace.d2 = CCard::SuitFromChar(cFlopReplace[i]);
            break;
        case 'C':
        case 'c':
            iSuit[2] = 1;
            m_suitReplace.c2 = CCard::SuitFromChar(cFlopReplace[i]);
            break;
        case 'S':
        case 's':
            iSuit[3] = 1;
            m_suitReplace.s2 = CCard::SuitFromChar(cFlopReplace[i]);
            break;
        default:
            break;
        }
    }
    int iConvertedCard = 2;
    //  iSuit的第0,1,2,3分别代表了，h,d,c,s四种花色
    //如果该值被值为1，说明原始牌中出现了该花色，已经转换到了h或者D，
    //剩下的花色转换到c和s转换为c和s中的任何一个花色都可以，因为剩余的两个花色应该时对称的,
    for (int i = 0; i < 4; i++)
    {
        if (0 == iSuit[i])
        {
            switch (i)
            {
            case 0://h未转换
                if (2 == iConvertedCard)
                {
                    m_suitReplace.h2 = c;
                    iConvertedCard++;
                }
                else if (3 == iConvertedCard)
                {
                    m_suitReplace.h2 = s;
                    iConvertedCard++;
                }
                break;
            case 1: //d未转换
                if (2 == iConvertedCard)
                {
                    m_suitReplace.d2 = c;
                    iConvertedCard++;
                }
                else if (3 == iConvertedCard)
                {
                    m_suitReplace.d2 = s;
                    iConvertedCard++;
                }
                break;
            case 2: //c
                if (2 == iConvertedCard)
                {
                    m_suitReplace.c2 = c;
                    iConvertedCard++;
                }
                else if (3 == iConvertedCard)
                {
                    m_suitReplace.c2 = s;
                    iConvertedCard++;
                }
                break;
            case 3: //s
                if (2 == iConvertedCard)
                {
                    m_suitReplace.s2 = c;
                    iConvertedCard++;
                }
                else if (3 == iConvertedCard)
                {
                    m_suitReplace.s2 = s;
                    iConvertedCard++;
                }
                break;
            }
        }
    }
    return;
}

void CBoard::ConvertCardSuit1(char cFlopOri)
{
    m_suitReplace.blIsNeeded = true;
    switch (cFlopOri)
    {
    case 'H':
    case 'h':
        m_suitReplace.h2 = h;
        m_suitReplace.d2 = d;
        m_suitReplace.c2 = c;
        m_suitReplace.s2 = s;
        m_suitReplace.blIsNeeded = false;
        break;
    case 'D':
    case 'd':
        m_suitReplace.h2 = s;
        m_suitReplace.d2 = h;
        m_suitReplace.c2 = d;
        m_suitReplace.s2 = c;
        break;
    case 'C':
    case 'c':
        m_suitReplace.h2 = c;
        m_suitReplace.d2 = s;
        m_suitReplace.c2 = h;
        m_suitReplace.s2 = d;
        break;
    case 'S':
    case 's':
        m_suitReplace.h2 = d;
        m_suitReplace.d2 = c;
        m_suitReplace.c2 = s;
        m_suitReplace.s2 = h;
        break;
    }
    return;
}
//函数作用，在做同构转换时，m_suitReplace里放的是当前花色转到1755中的一种同构的花色映射关系
//但是实际上使用时对手牌进行同构时获取到的是1755中的一种，要转换成当前花色的同构，
//与桌牌的同构不同，这是相对于桌牌同构一个逆向的同构转换，所以对m_suitReplace进行逆向转换

bool CBoard::RevertSuitReplace()
{
    SuitReplace suitTemp;
    if (false == m_suitReplace.blIsNeeded)
    {
        return true;
    }
    suitTemp.blIsNeeded = true;
    suitTemp.h2 = n;
    suitTemp.c2 = n;
    suitTemp.d2 = n;
    suitTemp.s2 = n;
    switch (m_suitReplace.h2)
    {
    case h:
        suitTemp.h2 = h;
        break;
    case d:
        suitTemp.d2 = h;
        break;
    case c:
        suitTemp.c2 = h;
        break;
    case s:
        suitTemp.s2 = h;
        break;
    default:
        break;
    }

    switch (m_suitReplace.d2)
    {
    case h:
        suitTemp.h2 = d;
        break;
    case d:
        suitTemp.d2 = d;
        break;
    case c:
        suitTemp.c2 = d;
        break;
    case s:
        suitTemp.s2 = d;
        break;
    default:
        break;
    }

    switch (m_suitReplace.c2)
    {
    case h:
        suitTemp.h2 = c;
        break;
    case d:
        suitTemp.d2 = c;
        break;
    case c:
        suitTemp.c2 = c;
        break;
    case s:
        suitTemp.s2 = c;
        break;
    default:
        break;
    }

    switch (m_suitReplace.s2)
    {
    case h:
        suitTemp.h2 = s;
        break;
    case d:
        suitTemp.d2 = s;
        break;
    case c:
        suitTemp.c2 = s;
        break;
    case s:
        suitTemp.s2 = s;
        break;
    default:
        break;
    }

    m_suitReplace = suitTemp;

    return true;
}
bool CBoard::GetIsomorphismNodeName(const char* csFlopOri)
{
    char cRePlace[7] = { 0 };
    char cCardOrgin[7] = { 0 };
    char cCardSuit[4] = { 0 };
    char cSuitReplace[4] = { 0 };

    cRePlace[6] = '\0';
    cCardOrgin[6] = '\0';
    cCardSuit[3] = '\0';
    cSuitReplace[3] = '\0';

    memcpy(cCardOrgin, csFlopOri, 6);

    cCardSuit[0] = cCardOrgin[1];
    cCardSuit[1] = cCardOrgin[3];
    cCardSuit[2] = cCardOrgin[5];

    /*三张牌大小相同，一定是3个花色，同构到hdc三个花色*/
    if (cCardOrgin[0] == cCardOrgin[2] && cCardOrgin[0] == cCardOrgin[4])
    {
        cRePlace[0] = cCardOrgin[0];
        cRePlace[1] = 'h';
        cRePlace[2] = cCardOrgin[2];
        cRePlace[3] = 'd';
        cRePlace[4] = cCardOrgin[4];
        cRePlace[5] = 'c';

        //如果三张牌里没有s色，说明是hdc三个花色，又因为牌大小相同，所以不进行转换
        if (cCardSuit[0] != 's' && cCardSuit[1] != 's' && cCardSuit[2] != 's')
        {
            cCardSuit[0] = 'h';
            cCardSuit[1] = 'd';
            cCardSuit[2] = 'c';
        }

        ConvertCardSuit3(cCardSuit);
    }
    else if (cCardOrgin[0] == cCardOrgin[2])//前面两张牌大小相等，花色一定不同
    {
        if (cCardOrgin[1] == cCardOrgin[5])//，1,3两张牌牌花色相同，同构到hdh
        {
            cRePlace[0] = cCardOrgin[0];
            cRePlace[1] = 'h';
            cRePlace[2] = cCardOrgin[2];
            cRePlace[3] = 'd';
            cRePlace[4] = cCardOrgin[4];
            cRePlace[5] = 'h';

            cSuitReplace[0] = 'h';
            cSuitReplace[1] = 'd';
            cSuitReplace[2] = 'h';
            ConvertCardSuit2(cCardSuit, cSuitReplace);
        }
        else if (cCardOrgin[3] == cCardOrgin[5])
        {
            //在同构时我们不仅对牌大小进行了排序，花色相同时也进行了牌位置处理，
            //所以不应该出现本情况

        }
        else //strTemp.Format("%sh%sd%sc,", sI, sJ, sK);
        {
            cRePlace[0] = cCardOrgin[0];
            cRePlace[1] = 'h';
            cRePlace[2] = cCardOrgin[2];
            cRePlace[3] = 'd';
            cRePlace[4] = cCardOrgin[4];
            cRePlace[5] = 'c';


            ConvertCardSuit3(cCardSuit);
        }
    }
    else if (cCardOrgin[2] == cCardOrgin[4])
    {
        if (cCardOrgin[1] == cCardOrgin[3])//strTemp.Format("%sh%sd%sh,", sI, sJ, sK);
        {
            cRePlace[0] = cCardOrgin[0];
            cRePlace[1] = 'h';
            cRePlace[2] = cCardOrgin[2];
            cRePlace[3] = 'h';
            cRePlace[4] = cCardOrgin[4];
            cRePlace[5] = 'd';

            cSuitReplace[0] = 'h';
            cSuitReplace[1] = 'h';
            cSuitReplace[2] = 'd';
            ConvertCardSuit2(cCardSuit, cSuitReplace);
        }
        else if (cCardOrgin[1] == cCardOrgin[5])
        {
        }
        else //strTemp.Format("%sh%sd%sc,", sI, sJ, sK);
        {
            cRePlace[0] = cCardOrgin[0];
            cRePlace[1] = 'h';
            cRePlace[2] = cCardOrgin[2];
            cRePlace[3] = 'd';
            cRePlace[4] = cCardOrgin[4];
            cRePlace[5] = 'c';
            ConvertCardSuit3(cCardSuit);
        }

    }
    else //三张牌大小不同
    {
        if (cCardOrgin[1] == cCardOrgin[3] && cCardOrgin[1] == cCardOrgin[5]) //hhh
        {
            cRePlace[0] = cCardOrgin[0];
            cRePlace[1] = 'h';
            cRePlace[2] = cCardOrgin[2];
            cRePlace[3] = 'h';
            cRePlace[4] = cCardOrgin[4];
            cRePlace[5] = 'h';

            ConvertCardSuit1(cCardOrgin[1]);
        }
        else if (cCardOrgin[1] == cCardOrgin[3]) //hhd
        {
            cRePlace[0] = cCardOrgin[0];
            cRePlace[1] = 'h';
            cRePlace[2] = cCardOrgin[2];
            cRePlace[3] = 'h';
            cRePlace[4] = cCardOrgin[4];
            cRePlace[5] = 'd';

            cSuitReplace[0] = 'h';
            cSuitReplace[1] = 'h';
            cSuitReplace[2] = 'd';
            ConvertCardSuit2(cCardSuit, cSuitReplace);
        }
        else if (cCardOrgin[1] == cCardOrgin[5]) //hdh
        {
            cRePlace[0] = cCardOrgin[0];
            cRePlace[1] = 'h';
            cRePlace[2] = cCardOrgin[2];
            cRePlace[3] = 'd';
            cRePlace[4] = cCardOrgin[4];
            cRePlace[5] = 'h';

            cSuitReplace[0] = 'h';
            cSuitReplace[1] = 'd';
            cSuitReplace[2] = 'h';
            ConvertCardSuit2(cCardSuit, cSuitReplace);
        }
        else if (cCardOrgin[3] == cCardOrgin[5]) //dhh
        {
            cRePlace[0] = cCardOrgin[0];
            cRePlace[1] = 'd';
            cRePlace[2] = cCardOrgin[2];
            cRePlace[3] = 'h';
            cRePlace[4] = cCardOrgin[4];
            cRePlace[5] = 'h';

            cSuitReplace[0] = 'd';
            cSuitReplace[1] = 'h';
            cSuitReplace[2] = 'h';
            ConvertCardSuit2(cCardSuit, cSuitReplace);
        }
        else //hdc
        {
            cRePlace[0] = cCardOrgin[0];
            cRePlace[1] = 'h';
            cRePlace[2] = cCardOrgin[2];
            cRePlace[3] = 'd';
            cRePlace[4] = cCardOrgin[4];
            cRePlace[5] = 'c';

            ConvertCardSuit3(cCardSuit);
        }
    }

    string csFlopReplace(cRePlace);
    m_sIsomorphismBoard = csFlopReplace;
    RevertSuitReplace();
    return true;
}