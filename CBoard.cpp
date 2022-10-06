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

    ConvetIsomorphism();	//����ͬ��
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

    if (csCards[0] == csCards[2] && csCards[0] == csCards[4]) // �����ƴ�С��ͬ����ɫһ����ͬ
    {
    }
    else if (csCards[0] == csCards[2]) //hdh,hdc ��1��2 �����ƴ�С��ͬ
    {
        if (csCards[3] == csCards[5]) //2,3��ɫ��ͬ����Ϊ1��3��ɫ��ͬ��1��2������ɫ����Ϊ���ǵ�ͬ��ֻ�� hdh �� ��ʱ�����1��2  ������
        {
            temp = csCards[1];
            csCards[1] = csCards[3];
            csCards[3] = temp;
        }
    }
    else if (csCards[2] == csCards[4]) //hhd �� ���2��3�����ƴ�С��ͬ��2��3��ɫ��ͬ���������һ�����һ�Ż�ɫ��ͬ��������ڵڶ��ŵ�λ�ã��Ϳ�����ͬ��hhd 
    {
        if (csCards[1] == csCards[5]) //1,3��ɫ��ͬ,��Ϊ1��2��ɫ��ͬ��2��3������ɫ
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

/*�������Ƕ�������������������ͬ�Ļ�ɫ����ͨ��ת����
�ư��մ�С���������ת��Ϊ�ֱ�ת��Ϊhdc���ֻ�ɫ����������û�е��Ǹ���ɫת��Ϊs
����cFlopReplaceӦ��Ϊhdc����Ϊͬ����������ͬ��ɫ���Ǿ�ֻ����hdc*/
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
    switch (iIndex) //������ɫ�ֱ�ͬ������h,d,c��û�г��ֵ��Ǹ���ɫ��Ӧ��s
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

/*�������Ƕ�������������ֻ����������ͬ�Ļ�ɫ����ͨ��ת����
�ư��մ�С���������ת��Ϊ�ֱ�ת��Ϊhdh,hhd ��dhh������ʽ��ȥƥ��ͬ��1755���е�һ��*/
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

    //ֻ����������ɫ��ת����cFlopReplace��ֻ��h��d������ɫ
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
    //  iSuit�ĵ�0,1,2,3�ֱ�����ˣ�h,d,c,s���ֻ�ɫ
    //�����ֵ��ֵΪ1��˵��ԭʼ���г����˸û�ɫ���Ѿ�ת������h����D��
    //ʣ�µĻ�ɫת����c��sת��Ϊc��s�е��κ�һ����ɫ�����ԣ���Ϊʣ���������ɫӦ��ʱ�ԳƵ�,
    for (int i = 0; i < 4; i++)
    {
        if (0 == iSuit[i])
        {
            switch (i)
            {
            case 0://hδת��
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
            case 1: //dδת��
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
//�������ã�����ͬ��ת��ʱ��m_suitReplace��ŵ��ǵ�ǰ��ɫת��1755�е�һ��ͬ���Ļ�ɫӳ���ϵ
//����ʵ����ʹ��ʱ�����ƽ���ͬ��ʱ��ȡ������1755�е�һ�֣�Ҫת���ɵ�ǰ��ɫ��ͬ����
//�����Ƶ�ͬ����ͬ���������������ͬ��һ�������ͬ��ת�������Զ�m_suitReplace��������ת��

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

    /*�����ƴ�С��ͬ��һ����3����ɫ��ͬ����hdc������ɫ*/
    if (cCardOrgin[0] == cCardOrgin[2] && cCardOrgin[0] == cCardOrgin[4])
    {
        cRePlace[0] = cCardOrgin[0];
        cRePlace[1] = 'h';
        cRePlace[2] = cCardOrgin[2];
        cRePlace[3] = 'd';
        cRePlace[4] = cCardOrgin[4];
        cRePlace[5] = 'c';

        //�����������û��sɫ��˵����hdc������ɫ������Ϊ�ƴ�С��ͬ�����Բ�����ת��
        if (cCardSuit[0] != 's' && cCardSuit[1] != 's' && cCardSuit[2] != 's')
        {
            cCardSuit[0] = 'h';
            cCardSuit[1] = 'd';
            cCardSuit[2] = 'c';
        }

        ConvertCardSuit3(cCardSuit);
    }
    else if (cCardOrgin[0] == cCardOrgin[2])//ǰ�������ƴ�С��ȣ���ɫһ����ͬ
    {
        if (cCardOrgin[1] == cCardOrgin[5])//��1,3�������ƻ�ɫ��ͬ��ͬ����hdh
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
            //��ͬ��ʱ���ǲ������ƴ�С���������򣬻�ɫ��ͬʱҲ��������λ�ô���
            //���Բ�Ӧ�ó��ֱ����

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
    else //�����ƴ�С��ͬ
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