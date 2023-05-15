#include "CPokerHand.h"
#include <iterator>

using namespace std;

bool CPokerHand::issubset(const set<int>& s, const set<int>& father) //�Ӽ�
{
    set<int> tmp;
    set_difference(s.begin(), s.end(), father.begin(), father.end(), inserter(tmp, tmp.begin()));
    return (tmp.empty()) ? true : false;
}

//��˳���򷵻�����˳�ӣ�Ҫ��myCards�Ȱ���������
MyCards CPokerHand::toStraight(const MyCards& myCards)
{
    MyCards ret;
    if (myCards.size() < 5)
        return ret;

    set<myCard> pointS;
    vector<set<myCard>> pointSes; //�������˳�ӵģ����һ��Ϊ���ģ���ʵ���Գ��ǰ��ģ�
    for (auto onecard : myCards)
        pointS.insert(onecard);

    if (pointS.rbegin()->m_point == 14)
        pointS.insert(myCard(pointS.rbegin()->m_suit, 1));


    if (pointS.size() < 5)
        return ret;


    auto pBegin = pointS.begin();
    auto pEndStable = pointS.end();
    pEndStable--;
    auto pEnd = pBegin;
    for (int i = 0; i < 4; i++) {

        pEnd = pBegin;
        bool elem5 = true;
        for (int j = 0; j < 4; j++) {
            pEnd++;
            if (pEnd == pointS.end()) {
                elem5 = false;
                break;
            }
        }

        if (!elem5) //����5�˳�
            break;

        //�ɹ����˳�
        auto pEndbefore = pEnd;
        pEndbefore--;

        if (pEnd->m_point - pBegin->m_point == 4)
            pointSes.push_back(set<myCard>(pBegin, ++pEnd));
        //else if (pEndStable->m_point == 14 && pEndbefore->m_point - pBegin->m_point == 3 && pBegin->m_point == 2) {
        else if (pEndStable->m_point == 14 && pEnd->m_point - pBegin->m_point == 3 && pBegin->m_point == 2) {
            set<myCard> tmp(pBegin, pEnd);
            tmp.insert(*pEndStable);
            pointSes.push_back(tmp);
        }

        pBegin++;
    }

    if (!pointSes.empty()) {
        for (auto onecard : pointSes[pointSes.size() - 1])
            ret.push_back(onecard);
    }

    sort(ret.begin(), ret.end(), [](myCard v1, myCard v2) {return v1.m_point > v2.m_point; });
    return ret;

}

//���������γ�1�ų�˳����Ϻ�outs��cardsҪ����cardsֻ��5��6
vector<OnecardToStraight> CPokerHand::getOnecardToStraight(const MyCards& cards)
{
    vector<OnecardToStraight> ret;

    //   if (cards.size() != 5 && cards.size() != 6) {
    //       cout << "cards num not 5 or 6.";
     //      return ret;
     //  }

    set<int> pointS; //�������ظ�
    for (auto onecard : cards)
        pointS.insert(onecard.m_point);

    //  if (pointS.size() < 5)
    //      return ret;

    if (*pointS.rbegin() == 14)
        pointS.insert(1);

    vector<int> pointV(pointS.begin(), pointS.end());

    for (int i = 0; i < pointV.size() - 3; i++) {
        int pointDis = pointV[i + 3] - pointV[i];
        if (pointDis == 3 || pointDis == 4) { //���ų�˳����
            OnecardToStraight newCombo;
            for (int j = i; j < i + 4; j++) {
                newCombo.cardPoints.push_back(pointV[j]);
                if (j < i + 3) {
                    int test = pointV[j + 1] - pointV[j];
                    if (pointV[j + 1] - pointV[j] != 1)
                        newCombo.outsPoints.push_back(pointV[j] + 1); //�м��Ƿ��и��ţ������ǿ�˳
                }
            }

            if (newCombo.outsPoints.empty()) { //�޸�����д�ģ���ͷ˳
                if (pointV[i] != 1)
                    newCombo.outsPoints.push_back(pointV[i] - 1);
                if (pointV[i + 3] != 14)
                    newCombo.outsPoints.push_back(pointV[i + 3] + 1);
            }

            ret.push_back(newCombo);
        }
    }

    //���ĳ��ϵ�outs����һ�����Ӽ�������С����Щ������Ч�����outsһ����ȡ���
     //issubset

    vector<OnecardToStraight> rettmp2outs;
    vector<OnecardToStraight> rettmp1outs;

    for (auto one : ret) {
        if (one.outsPoints.size() == 1)
            rettmp1outs.push_back(one);
        else if (one.outsPoints.size() == 2)
            rettmp2outs.push_back(one);
    }

    for (auto one : rettmp1outs) {
        bool valid = true;
        for (auto p = rettmp2outs.begin(); p != rettmp2outs.end(); p++) {
            set<int> oneS(one.outsPoints.begin(), one.outsPoints.end());
            set<int> pS(p->outsPoints.begin(), p->outsPoints.end());
            if (issubset(oneS, pS)) {
                if (one.cardPoints.back() > p->cardPoints.back()) {
                    if (p->outsPoints.size() == 1) { //��һ���ı������
                        p->cardPoints = one.cardPoints;
                        p->outsPoints = one.outsPoints;
                        valid = false;
                    }
                }
                else
                    valid = false;
            }
        }
        if (valid)
            rettmp2outs.push_back(one);
    }

    return rettmp2outs;
}

//���ع���������������������γɵ����˳�Ӻ�outs,outs������һ�Ż�����,����������˳����draw�е�һ����nuts˳
set<int> CPokerHand::getMaxPossibleStraightByBoard(const MyCards& publics, vector<int>& maxStraight)
{
    set<int> ret; //outs

    //   if (publics.size() != 3 && publics.size() != 4) {
           //cout << "cards num not 3 or 4.";
    //       return ret;
    //   }

    set<int> pointS; //�������ظ�
    for (auto onecard : publics)
        pointS.insert(onecard.m_point);

    if (pointS.size() < 3)
        return ret;

    if (*pointS.rbegin() == 14)
        pointS.insert(1);

    vector<int> pointV(pointS.begin(), pointS.end());
    int nmaxInstraight = 0;

    //���ҳ����˳�ӵ������
    for (int i = 0; i < pointV.size() - 2; i++) {
        int pointDis = pointV[i + 2] - pointV[i];
        if (pointDis == 2)
            nmaxInstraight = min(14, pointV[i + 2] + 2);
        else if (pointDis == 3)
            nmaxInstraight = min(14, pointV[i + 2] + 1);
        else if (pointDis == 4)
            nmaxInstraight = pointV[i + 2];
    }

    //������˳�Ӻ�outs
    if (nmaxInstraight != 0) {
        for (int i = 0; i < 5; i++) {
            maxStraight.push_back(nmaxInstraight - i);
            if (pointS.find(nmaxInstraight - i) == pointS.end())
                ret.insert(nmaxInstraight - i);
        }
    }

    return ret;
}

//����draw˳�����ۣ�
//nNeedHandsNum = 0,1,2 //0���������
//nOutsNum = ʵ����
//nRank = 0nuts,1��,2С
DrawStruct CPokerHand::getStraightDraw(const MyCards& privates, const MyCards& publics)
{
    DrawStruct ret;

    MyCards cards;
    for (auto it : privates)
        cards.push_back(it);
    for (auto it : publics)
        cards.push_back(it);

    set<int> privatesS;
    privatesS.insert(privates[0].m_point);
    privatesS.insert(privates[1].m_point);
    if (privatesS.find(14) != privatesS.end())
        privatesS.insert(1);

    set<int> publicsS;
    for (auto it : publics)
        publicsS.insert(it.m_point);
    if (publicsS.find(14) != publicsS.end())
        publicsS.insert(1);

    map<Suit, int> suitCount;
    for (auto it : publics)
        suitCount[it.m_suit]++;
    int nMaxSuitCount = 0;
    for (auto it : suitCount)
        if (it.second > nMaxSuitCount)
            nMaxSuitCount = it.second;


    set<int> outsS;
    set<int> needHandsS;
    vector<OnecardToStraight> onecardToStraight = getOnecardToStraight(cards);
    for (auto it : onecardToStraight) {
        //for (auto oneouts : it.outsPoints)
        //    outsS.insert(oneouts);
        int nNeedPrivate = 0;
        for (auto onecard : it.cardPoints) {
            if (publicsS.find(onecard) == publicsS.end() && privatesS.find(onecard) != privatesS.end()) {
                needHandsS.insert(onecard);
                nNeedPrivate++;
            }
        }

        if (nNeedPrivate > 0) { //�����һ�ų�˳����û��private����������outsҲ�ǹ��Ƴ�˳�����Ӧ��outs��Ч
            for (auto oneouts : it.outsPoints)
                outsS.insert(oneouts);
        }
    }

    vector<int> maxStraight;
    int nHighCardCount = 0;
    set<int> maxStraightOuts = getMaxPossibleStraightByBoard(publics, maxStraight);
    if (!maxStraightOuts.empty()) {
        for (auto it : needHandsS) {
            if (it >= *maxStraightOuts.begin())
                nHighCardCount++;
        }

        if (find(maxStraight.begin(), maxStraight.end(), 14) != maxStraight.end() && (privatesS.find(14) != privatesS.end() || publicsS.find(14) != publicsS.end())) //���ƻ�hand��A�򲻹�������ʲô����nuts draw
            nHighCardCount = 2;

    }
    else {
        if (!needHandsS.empty())
            nHighCardCount = 2;
        else
            nHighCardCount = 0;
    }

    ret.nNeedHandsNum = (int)needHandsS.size();
    ret.nOutsNum = (int)outsS.size() * 4;
    if (nMaxSuitCount == 2) //3�ųɻ�ʱ��˳��outҪ-1*outs(������ͷ˳����8-2=6)
        ret.nOutsNum -= (int)outsS.size();
    ret.nRank = 2 - nHighCardCount;

    return ret;
}

//������ɺ���˳������
set<int> CPokerHand::getBackdoorStraightDraw(const MyCards& privates, const MyCards& publics)
{
    set<int> ret;

    if (publics.size() >= 4)
        return ret;

    set<int> privatesS;
    privatesS.insert(privates[0].m_point);
    privatesS.insert(privates[1].m_point);
    if (privatesS.size() < 2)
        return ret;
    vector<int> privatesV(privatesS.begin(), privatesS.end());

    if (privatesV[1] == 14)
        return ret;

    for (auto one_public : publics) {
        set<int> threeCards = privatesS;
        if (one_public.m_point == 14)
            continue;
        threeCards.insert(one_public.m_point);
        vector<int> threeCardsV(threeCards.begin(), threeCards.end());
        if (threeCardsV.size() == 3) {
            if (threeCardsV[2] - threeCardsV[0] == 2) {
                //���outs�Ƿ��¹���һ�ų�˳
                vector<int> outs{ threeCardsV[0] - 1,threeCardsV[2] + 1 };
                set<int> publicsS;
                bool blOne2Straight = false;
                for (auto one_public : publics)
                    publicsS.insert(one_public.m_point);
                for (auto one_outs : outs) {
                    set<int> publics_add1S = publicsS;
                    vector<int> publics_add1V(publics_add1S.begin(), publics_add1S.end());
                    if (publics_add1V.size() == 4 && publics_add1V[3] - publics_add1V[0] <= 4) {
                        blOne2Straight = true;
                        break;
                    }
                }

                if (!blOne2Straight) {
                    for (auto card : threeCardsV)
                        ret.insert(card);
                }
            }
        }
    }

    return ret;
}

//����draw��������
//nNeedHandsNum = 0,1,2 //0���������
//nOutsNum = ʵ����
//nRank = ������м��� 0����nuts
DrawStruct CPokerHand::getFlushtDraw(const MyCards& privates, const MyCards& publics)
{
    DrawStruct ret;
    map<Suit, vector<int>> possibleflushs;

    for (auto one_private : privates)
        possibleflushs[one_private.m_suit].push_back(one_private.m_point);

    for (auto p = possibleflushs.begin(); p != possibleflushs.end(); p++)
        sort(p->second.begin(), p->second.end());

    for (auto one_public : publics) {
        if (possibleflushs.find(one_public.m_suit) != possibleflushs.end())
            possibleflushs[one_public.m_suit].push_back(one_public.m_point);
    }

    for (auto it : possibleflushs) {
        if (it.second.size() == 4) {
            ret.nNeedHandsNum = possibleflushs.size() == 2 ? 1 : 2;  //private�Ļ�ɫ��һ��������ֻ������һ��

            int largePrivate = ret.nNeedHandsNum == 2 ? it.second[1] : it.second[0];

            ret.nRank = 14 - largePrivate;
            for (int i = 1; i < 4; i++) {
                if (it.second[i] > largePrivate)
                    ret.nRank--;
            }
        }
        //else if (it.second.size() > 4)
            //cout << "error: already flush" << endl;
    }

    ret.nOutsNum = 9;
    return ret;
}

//����rank(����Ļ��м��ţ�0����nuts), -1����û�к��Ż�(�����˳�������Ͳ�һ��)
int CPokerHand::getBackdoorFlushDraw(const MyCards& privates, const MyCards& publics)
{
    int rank = -1;

    if (publics.size() >= 4)
        return -1;

    map<Suit, vector<int>> possibleflushs;
    for (auto one_private : privates)
        possibleflushs[one_private.m_suit].push_back(one_private.m_point);

    if (possibleflushs.size() == 2)
        return -1;

    auto p = possibleflushs.begin();
    sort(p->second.begin(), p->second.end());
    int largePrivate = p->second.back();
    Suit suit = p->first;

    for (auto one_public : publics) {
        if (one_public.m_suit == suit) {
            rank = 14 - largePrivate;
            if (one_public.m_point > largePrivate)
                rank--;
            break;
        }
    }

    return rank;
}

//����Ƿ񹹳�˳��һ�ų�˳,��ֵ�������getHighCardDraw,publicAdd1Ϊ4-5���������Ϊ������ĸ���
bool CPokerHand::isClosetoStraight(const set<int>& publicAdd1)
{
    if (publicAdd1.size() != 4 && publicAdd1.size() != 5)
        return false;

    vector<int> tmp(publicAdd1.rbegin(), publicAdd1.rend());
    if (publicAdd1.size() == 5) {
        if (tmp[0] - tmp[4] == 4 || tmp[0] == 14 && tmp[1] == 5 || tmp[0] - tmp[3] <= 4) //��˳
            return true;
    }
    else if (publicAdd1.size() == 4) { //�Ƿ�һ�ų�˳
        if (tmp[0] - tmp[3] <= 4 || tmp[0] == 14 && tmp[1] <= 5)
            return true;
    }

    return false;
}

//����Ƿ���3������ͬɫ
bool CPokerHand::isClosetoFlush(const MyCards& publics)
{
    map<Suit, int> suitCounts;
    for (auto it : publics)
        suitCounts[it.m_suit]++;

    int nMaxCount = 0;
    for (auto it : suitCounts)
        if (it.second > nMaxCount)
            nMaxCount = it.second;

    return (nMaxCount >= 3);
}

//�����ǳ��ֶԣ�һ�ųɶԣ�һ�ŷǶ�,����ӽ�˳�ӻ�����Ч
bool CPokerHand::getPairDraw(const MyCards& privates, const MyCards& publics)
{
    if (privates[0].m_point == privates[1].m_point) //���Ӳ�����draw����
        return false;

    map<int, int> publicPairs;
    for (auto card : publics)
        publicPairs[card.m_point]++;

    int Pair = 0;
    int nPointSingle = 0;

    for (auto it : privates) {
        auto p = publicPairs.find(it.m_point);
        if (p != publicPairs.end()) {
            if (p->second == 1) {
                Pair++;
                nPointSingle = p->first == privates[0].m_point ? privates[1].m_point : privates[0].m_point;
            }
        }
    }

    set<int> publicAdd1;
    for (auto one_public : publics)
        publicAdd1.insert(one_public.m_point);
    publicAdd1.insert(nPointSingle);
    if (isClosetoStraight(publicAdd1) || isClosetoFlush(publics))
        return false;

    return Pair == 1 ? true : false;

}

//out:���Ÿ���,rank:�������,nOutsNum 0�����޸��ţ�*ϵ��A��K=3��T-Q=2������1
DrawStruct CPokerHand::getHighCardDraw(const MyCards& privates, const MyCards& publics)
{
    DrawStruct ret;
    ret.nOutsNum = 0;
    ret.nRank = 0;

    if (privates[0].m_point == privates[1].m_point) //���Ӳ�����draw����
        return ret;

    int largestPublicRank = 0;
    for (auto it : publics) {
        if (it.m_point > largestPublicRank)
            largestPublicRank = it.m_point;
    }

    for (auto it : privates) {
        if (it.m_point > largestPublicRank) {
            //�������ż�����Ƿ��γ�һ�ų�˳��������3��ͬ�����㣨ֱ�ӳ�˳������
            set<int> publicAdd1;
            for (auto one_public : publics)
                publicAdd1.insert(one_public.m_point);
            publicAdd1.insert(it.m_point);

            if (!isClosetoStraight(publicAdd1) && !isClosetoFlush(publics) && !getPairDraw(privates, publics)) {
                if (it.m_point >= 13)
                    ret.nOutsNum += 3;
                else if (it.m_point >= 10)
                    ret.nOutsNum += 2;
                else
                    ret.nOutsNum += 1;

                ret.nRank += it.m_point;
            }
        }
    }

    return ret;
}

//���Ƶ�����
PublicStruct CPokerHand::getPublicStruct(const MyCards& publics)
{
    PublicStruct ps;

    possibleClasses pClasses(publics);
    ps.pokerClass = pClasses.getPokerClass();
    ps.nNeedtoFlush = pClasses.m_needtoFlush;
    ps.nNeedtoStraight = pClasses.m_needtoStraight;
    ps.nPair = (int)pClasses.m_pair.size() / 2;
    ps.blOESDpossible = pClasses.m_blOESDpossible;

    for (auto it : publics) { //��¼������
        if (it.m_point > ps.nMaxRank)
            ps.nMaxRank = it.m_point;

        if (it.m_point >= high_card_point)
            ps.nHighCardCount++;
    }

    if (publics.size() == 5) { //river

        int nMaxPoint = 0;
        for (auto it : publics) {
            if (it.m_point > nMaxPoint)
                nMaxPoint = it.m_point;
        }

        if (ps.pokerClass == MAX_STRAIGHT_FLUSH || ps.pokerClass == MAX_STRAIGHT) {
            if (nMaxPoint == 14)
                ps.nAppend = 1; //nuts
            else
                ps.nAppend = 2;
        }
        else if (ps.pokerClass == MAX_FULL_HOUSE) {
            map<int, int> pointInFullhouse;
            int point2 = 0;
            int point3 = 0;
            for (auto it : publics)
                pointInFullhouse[it.m_point]++;
            for (auto it : pointInFullhouse) {
                if (it.second == 2)
                    point2 = it.first;
                else if (it.second == 3)
                    point3 = it.first;
            }
            if (point3 > point2)
                ps.nAppend = 2; //3����һ�Դ�ʱ����ץզ
        }
    }

    return ps;
}

//draw�Ƶ�����
DrawStruct CPokerHand::getDrawStruct(const MyCards& privates, const MyCards& publics, const PublicStruct& publicStruct)
{
    DrawStruct drawStruct;
    drawStruct.pokerClass = DRAW_NONE;

    if (publics.size() == 5)
        return drawStruct;

    //typedef enum { MAX_HIGH_CARD, MAX_PAIR, MAX_TWO_PAIR, MAX_THREE_OF_A_KIND, MAX_STRAIGHT, MAX_FLUSH, MAX_FULL_HOUSE, MAX_FOUR_OF_A_KIND, MAX_STRAIGHT_FLUSH, DRAW_STRAIGHT, DRAW_FLUSH, DRAW_OTHERS, DRAW_NONE }PokerClass;
    //�ų��������draw, draw����2�����ϣ�������˳  draw˳��2�����ϡ����ųɻ�  draw������2�����ϡ����ųɻ���һ�ų�˳
    bool isDrawFlushValid = true;
    bool isDrawStraightValid = true;
    bool isDrawOtherValid = true;
    bool isDrawBackdoor = true;

    if (publicStruct.pokerClass == MAX_TWO_PAIR ||
        publicStruct.pokerClass == MAX_THREE_OF_A_KIND ||
        publicStruct.pokerClass == MAX_FLUSH ||
        publicStruct.pokerClass == MAX_FULL_HOUSE ||
        publicStruct.pokerClass == MAX_FOUR_OF_A_KIND ||
        publicStruct.pokerClass == MAX_STRAIGHT_FLUSH) {
        isDrawFlushValid = false;
    }

    if (publicStruct.pokerClass == MAX_TWO_PAIR ||
        publicStruct.pokerClass == MAX_THREE_OF_A_KIND ||
        publicStruct.pokerClass == MAX_FLUSH ||
        publicStruct.pokerClass == MAX_FULL_HOUSE ||
        publicStruct.pokerClass == MAX_FOUR_OF_A_KIND ||
        publicStruct.pokerClass == MAX_STRAIGHT_FLUSH ||
        publicStruct.pokerClass == MAX_STRAIGHT ||
        publicStruct.nNeedtoFlush == 2 || publicStruct.nNeedtoFlush == 1) {
        isDrawStraightValid = false;
    }

    if (publicStruct.pokerClass == MAX_TWO_PAIR ||
        publicStruct.pokerClass == MAX_THREE_OF_A_KIND ||
        publicStruct.pokerClass == MAX_FLUSH ||
        publicStruct.pokerClass == MAX_FULL_HOUSE ||
        publicStruct.pokerClass == MAX_FOUR_OF_A_KIND ||
        publicStruct.pokerClass == MAX_STRAIGHT_FLUSH ||
        publicStruct.pokerClass == MAX_STRAIGHT ||
        publicStruct.pokerClass == MAX_PAIR ||
        publicStruct.nNeedtoFlush == 2 || publicStruct.nNeedtoFlush == 1 ||
        publicStruct.nNeedtoStraight == 1) {
        isDrawOtherValid = false;
    }

    if (publicStruct.pokerClass == MAX_TWO_PAIR ||
        publicStruct.pokerClass == MAX_THREE_OF_A_KIND ||
        publicStruct.pokerClass == MAX_FLUSH ||
        publicStruct.pokerClass == MAX_FULL_HOUSE ||
        publicStruct.pokerClass == MAX_FOUR_OF_A_KIND ||
        publicStruct.pokerClass == MAX_STRAIGHT_FLUSH ||
        publicStruct.pokerClass == MAX_STRAIGHT ||
        publicStruct.pokerClass == MAX_PAIR ||
        publicStruct.nNeedtoFlush == 2 || publicStruct.nNeedtoFlush == 1 ||
        publicStruct.nNeedtoStraight == 1) {
        isDrawBackdoor = false;
    }

    DrawStruct straightDraw;
    set<int> backdoorStraightDraw;

    DrawStruct flushDraw;
    int backdoorFlushDraw = -1;

    DrawStruct highcardDraw;
    bool pairDraw = false;

    int nPairDrawOuts = 0;

    if (isDrawFlushValid)
        flushDraw = getFlushtDraw(privates, publics);

    if (isDrawStraightValid)
        straightDraw = getStraightDraw(privates, publics);

    if (isDrawOtherValid) {
        highcardDraw = getHighCardDraw(privates, publics);
        pairDraw = getPairDraw(privates, publics);
        if (pairDraw)
            nPairDrawOuts = 5;
    }

    int nbackdoorDrawOuts = 0;
    if (isDrawBackdoor) {
        backdoorFlushDraw = getBackdoorFlushDraw(privates, publics);
        if (backdoorFlushDraw != -1) {
            if (backdoorFlushDraw == 0) //nuts���Ż�outs��2��������1
                nbackdoorDrawOuts += 2;
            else
                nbackdoorDrawOuts += 1;
        }

        backdoorStraightDraw = getBackdoorStraightDraw(privates, publics);
        if (!backdoorStraightDraw.empty())
            nbackdoorDrawOuts += 2; //����˳outs��2
    }

    //���Ż�outsϵ����nuts��+2��������+1������˳+2������K-A+3,T-Q+2,������1����+5

    if (flushDraw.nNeedHandsNum != 0) { //����drawͬ��
        drawStruct.pokerClass = DRAW_FLUSH;
        drawStruct.nNeedHandsNum = flushDraw.nNeedHandsNum;
        drawStruct.nRank = flushDraw.nRank;
        drawStruct.nOutsNum = flushDraw.nOutsNum + straightDraw.nOutsNum;
        drawStruct.nAppendOutsNum = highcardDraw.nOutsNum + nPairDrawOuts;
        if (straightDraw.nOutsNum > 0) //˳��˫drawҪ��2��outs
            drawStruct.nOutsNum -= straightDraw.nOutsNum / 4;
    }
    else if (straightDraw.nNeedHandsNum != 0) { //����draw˳
        drawStruct.pokerClass = DRAW_STRAIGHT;
        drawStruct.nNeedHandsNum = straightDraw.nNeedHandsNum;
        drawStruct.nRank = straightDraw.nRank;
        drawStruct.nOutsNum = straightDraw.nOutsNum;
        drawStruct.nAppendOutsNum = highcardDraw.nOutsNum + nPairDrawOuts;
    }
    else {
        drawStruct.pokerClass = DRAW_OTHERS;
        drawStruct.nNeedHandsNum = 2;
        drawStruct.nOutsNum = 0;
        drawStruct.nAppendOutsNum = highcardDraw.nOutsNum + nPairDrawOuts + nbackdoorDrawOuts;
        drawStruct.nRank = -1; //?��ô����,������������
    }

    return drawStruct;
}

//�����Ƶ�����
MadehandStruct CPokerHand::getMadehandStruct(const MyCards& privates, const MyCards& publics)
{
    MadehandStruct madehandStruct;

    MyCards cards;
    for (auto card : privates)
        cards.push_back(card);
    for (auto card : publics)
        cards.push_back(card);

    possibleClasses pClasses(cards);
    PokerClass ps = pClasses.getPokerClass();
    madehandStruct.pokerClass = ps;

    //typedef enum { MAX_HIGH_CARD, MAX_PAIR, MAX_TWO_PAIR, MAX_THREE_OF_A_KIND, MAX_STRAIGHT, MAX_FLUSH, MAX_FULL_HOUSE, MAX_FOUR_OF_A_KIND, MAX_STRAIGHT_FLUSH }PokerClass;
    int nNeedHands = 0;

    switch (ps) {
    case MAX_STRAIGHT_FLUSH: //rank:0:nuts,1:С�����Ƕ������Ķ��� nuts��
    {
        int nPrivateTotalPoint = 0;
        for (auto one_private : privates) {
            if (find(pClasses.m_straight_flush.begin(), pClasses.m_straight_flush.end(), one_private) != pClasses.m_straight_flush.end()) {
                nNeedHands++;
                nPrivateTotalPoint += one_private.m_point;
            }
        }

        madehandStruct.nNeedHandsNum = nNeedHands;

        if (nNeedHands == 1)
            madehandStruct.nRank = nPrivateTotalPoint > pClasses.m_straight_flush[4].m_point || nPrivateTotalPoint >= 10 ? 0 : 1; //||A˳
        else if (nNeedHands == 2)
            madehandStruct.nRank = nPrivateTotalPoint > pClasses.m_straight_flush[4].m_point + pClasses.m_straight_flush[3].m_point || nPrivateTotalPoint >= 19 ? 0 : 1;
    }
    break;
    case MAX_FOUR_OF_A_KIND: //rank:0,1,1����ʲô��û��,hands=0;nAppend=1�����Ƴ�4����rankΪ��A�ľ���
    {
        for (auto one_private : privates) {
            if (find(pClasses.m_king.begin(), pClasses.m_king.end(), one_private) != pClasses.m_king.end())
                nNeedHands++;
        }

        int nMaxInPrivate = privates[0].m_point >= privates[1].m_point ? privates[0].m_point : privates[1].m_point;

        if (nNeedHands == 0) {
            madehandStruct.nAppend = 1; //����4��
            madehandStruct.nNeedHandsNum = 1;
            madehandStruct.nRank = 14 - nMaxInPrivate;

            //if (privates[0].m_point == 14 || privates[1].m_point == 14) {
            //    madehandStruct.nNeedHandsNum = 1;
            //    madehandStruct.nRank = 0;
           // }
            //else { //����king�������޹�
            //    madehandStruct.nNeedHandsNum = 0;
            //    madehandStruct.nRank = 1;
           // }
        }
        else {
            madehandStruct.nNeedHandsNum = nNeedHands;
            madehandStruct.nRank = 0;
        }
    }
    break;
    case MAX_FULL_HOUSE: //rank:0,1,2,3; nAppend=1��������3��,rank����Դ�С; nAppend=2��������3��,��ֻ����ʹ��
    {
        madehandStruct.nRank = 3; //Ĭ��Ϊ��Сֵ

        set<int> publicsS;
        map<int, int> point2num;
        for (auto it : publics) {
            publicsS.insert(it.m_point);
            point2num[it.m_point]++;
        }
        vector<int> publicV(publicsS.rbegin(), publicsS.rend()); //�Ӵ�С
        vector<int> publicV_exclude3 = publicV; //����3����ļ��ϣ�flop��Ϊ�գ�
        for (auto p = publicV_exclude3.begin(); p != publicV_exclude3.end(); p++) {
            if (point2num[*p] == 3) {
                publicV_exclude3.erase(p);
                break;
            }
        }

        set<int> privatesS;
        for (auto it : privates)
            privatesS.insert(it.m_point);
        vector<int> privatesV(privatesS.rbegin(), privatesS.rend()); //�Ӵ�С

        map<int, int>point2num_Infullhouse;
        for (auto it : pClasses.m_fullhouse)
            point2num_Infullhouse[it.m_point]++;
        int three_Infullhouse, pair_Infullhouse;
        for (auto it : point2num_Infullhouse) {
            if (it.second == 2)
                pair_Infullhouse = it.first;
            else if (it.second == 3)
                three_Infullhouse = it.first;
        }


        for (auto one_private : privates) {
            if (find(pClasses.m_fullhouse.begin(), pClasses.m_fullhouse.end(), one_private) != pClasses.m_fullhouse.end())
                nNeedHands++;
        }
        madehandStruct.nNeedHandsNum = nNeedHands;

        int nMaxNum = 0;
        int nPoint_MaxNum = 0;
        for (auto num : point2num) {
            if (num.second > nMaxNum) {
                nMaxNum = num.second;
                nPoint_MaxNum = num.first;
            }
        }

        if (nMaxNum == 3 && nPoint_MaxNum == three_Infullhouse) { //���ܹ��Ƶ�3��ֻ��pair��
            madehandStruct.nAppend = 1; //������3��

            if (!publicV_exclude3.empty()) {
                //rank0 �����ǳ���
                if (pair_Infullhouse > publicV_exclude3[0])
                    if (madehandStruct.nRank == 3) madehandStruct.nRank = 0;

                //rank1 �����Ƕ���
                if (pair_Infullhouse == publicV_exclude3[0])
                    if (madehandStruct.nRank == 3) madehandStruct.nRank = 1;

                //rank2 �������ж�
                if (publicV_exclude3.size() >= 2) {
                    if (pair_Infullhouse >= publicV_exclude3[1])
                        if (madehandStruct.nRank == 3) madehandStruct.nRank = 2;
                }
            }
            else { //flopֻ��3��ʱ
                if (pair_Infullhouse == 14) { //AAΪrank0
                    if (madehandStruct.nRank == 3) madehandStruct.nRank = 0;
                }
                else if (pair_Infullhouse >= 11) { //JJ-KKΪrank1
                    if (madehandStruct.nRank == 3) madehandStruct.nRank = 1;
                }
                else if (pair_Infullhouse >= 8) { //88-TTΪrank2
                    if (madehandStruct.nRank == 3) madehandStruct.nRank = 2;
                } 
            }
        }
        else {
            //rank0,�����������Ϊ�ԣ�private������źʹδ���,Ϊnuts
            if (nMaxNum == 3)
                madehandStruct.nAppend = 2;

            if (privatesV.size() == 2) {
                bool blOnly0pair = false; //ֻ��������Ƕ�
                if (point2num[publicV[0]] == 2) blOnly0pair = true;
                for (int i = 1; i < publicV.size(); i++)
                    if (point2num[publicV[i]] != 1)  blOnly0pair = false;

                if (blOnly0pair && publicV[0] == privatesV[0] && publicV[1] == privatesV[1])
                    madehandStruct.nRank = 0; //nuts
            }

            //rank1,��«�й���3�ŵ���private,����3�ŵ��������
            if (publicV[0] == three_Infullhouse)
                if (madehandStruct.nRank == 3) madehandStruct.nRank = 1;

            //rank2,��«�й���3�ŵ���private,����3�ŵ��Ǵδ���
            if (publicV[1] == three_Infullhouse)
                if (madehandStruct.nRank == 3) madehandStruct.nRank = 2;
        }
    }
    break;
    case MAX_FLUSH: //hands:1,2��rank:14-private����-public��ģ�append:1����1�ų�ͬ��˳
    {
        int nMaxPointInPrivate = 0;

        for (auto one_private : privates) {
            if (find(pClasses.m_flush.begin(), pClasses.m_flush.end(), one_private) != pClasses.m_flush.end()) {
                if (one_private.m_point > nMaxPointInPrivate)
                    nMaxPointInPrivate = one_private.m_point;
                nNeedHands++;
            }
        }

        Suit suitInFlush = pClasses.m_flush[0].m_suit;
        int nLargePublicNum = 0;
        MyCards publicsWithSameSuit;
        for (auto one_public : publics) {
            if (one_public.m_suit == suitInFlush) {
                publicsWithSameSuit.push_back(one_public);
                if (one_public.m_point > nMaxPointInPrivate)
                    nLargePublicNum++;
            }
        }

        madehandStruct.nRank = 14 - nMaxPointInPrivate - nLargePublicNum;
        //madehandStruct.nNeedHandsNum = 5 - (int)publicsWithSameSuit.size();
        madehandStruct.nNeedHandsNum = nNeedHands;

        //����Ƿ���1�ų�ͬ��˳��
        vector<OnecardToStraight> oneCardToFlushStraight = getOnecardToStraight(publicsWithSameSuit);
        if (!oneCardToFlushStraight.empty())
            madehandStruct.nAppend = 1;
    }
    break;
    case MAX_STRAIGHT: //hands: 1��2��rank:0,1,2(��������ɵ����˳��outs��private���ţ������ѳ�˳����A)   ??�����Ѿ���nuts˳ʱ
    {
        set<int> privatesS;
        for (auto it : privates)
            privatesS.insert(it.m_point);
        set<int> publicsS;
        for (auto it : publics)
            publicsS.insert(it.m_point);

        set<int> InStraight;
        for (auto it : pClasses.m_straight)
            InStraight.insert(it.m_point);

        vector<int> maxStraight;
        set<int> maxStraightOuts = getMaxPossibleStraightByBoard(publics, maxStraight);

        for (auto p : privatesS) {
            if (publicsS.find(p) == publicsS.end() && InStraight.find(p) != InStraight.end())
                nNeedHands++;
        }
        madehandStruct.nNeedHandsNum = nNeedHands;

        int nNeedInMaxStraight = 0;
        for (auto p : privatesS) {
            if (maxStraightOuts.find(p) != maxStraightOuts.end())
                nNeedInMaxStraight++;
        }
        madehandStruct.nRank = 2 - nNeedInMaxStraight;
        if (*InStraight.rbegin() == 14)
            madehandStruct.nRank = 0;
    }
    break;
    case MAX_THREE_OF_A_KIND: //hands=2:set hands=1:triples; (0������3��)  rank:set,0��,1>=�θߣ�2������triples��rank������A�ж����ţ������Ƹ��ţ�
    {
        int nPointInThree = pClasses.m_three[0].m_point;
        int nPrivatePointInThree, nPrivatePointOutThree;

        for (auto it : privates) {
            if (it.m_point == nPointInThree) {
                nPrivatePointInThree = it.m_point;
                nNeedHands++;
            }
            else
                nPrivatePointOutThree = it.m_point;
        }
        madehandStruct.nNeedHandsNum = nNeedHands;

        set<int> publicsS;
        for (auto it : publics)
            publicsS.insert(it.m_point);
        vector<int> publicsV(publicsS.rbegin(), publicsS.rend()); //�Ӵ�С

        if (nNeedHands == 2) {//set
            if (nPrivatePointInThree == publicsV[0])
                madehandStruct.nRank = 0;
            else if (nPrivatePointInThree >= publicsV[1])
                madehandStruct.nRank = 1;
            else
                madehandStruct.nRank = 2;
        }
        else if (nNeedHands == 1) { //triples
            madehandStruct.nRank = 14 - nPrivatePointOutThree;
            for (auto it : publicsV)
                if (it > nPrivatePointOutThree && it != nPrivatePointInThree)
                    madehandStruct.nRank--;
        }
    }
    break;
    case MAX_TWO_PAIR:
    {
        //��2�ԣ�append=0��hands=2���Ҳ�ͬ,rank:0:����źʹδ��ţ�1������Ǻ����⣬2��������
        //���ֶ�_�����ԣ�append=1��hands=2��rank=0:���ԣ�rank=1:�θ߶ӣ�rank=2����
        //��_�����ԣ�append=2��hands=1��rank=0:���Զ��ߣ�1�����ԣ�2�ζ��ԣ�3����
        //����2���ԣ�append=3,hands=0
        //?������ʱ�޷����ֳ��ԺͶ��ԣ��ԵĴ�С����˿��ܴ����Ե�Ҫ�Ȳ��������۸ߣ�outs��Ҳ�к����ԣ�
        set<int> publicsS;
        for (auto it : publics)
            publicsS.insert(it.m_point);
        vector<int> publicsV(publicsS.rbegin(), publicsS.rend()); //�Ӵ�С

        set<int> In2Pair;
        for (auto it : pClasses.m_pair)
            In2Pair.insert(it.m_point);

        int nPrivateNotInpairPoint = 0; //private�ǳɶԵĵ���,�����ж��Ƿ��Ƕ���
        bool blIsMaxTick = false;
        for (auto one_private : privates) {
            if (In2Pair.find(one_private.m_point) == In2Pair.end()) {
                nPrivateNotInpairPoint = one_private.m_point;
                int nDis = 14 - nPrivateNotInpairPoint;
                for (auto one_pairpoint : In2Pair) {
                    if (one_pairpoint > nPrivateNotInpairPoint)
                        nDis--;
                }
                if (nDis == 0)
                    blIsMaxTick = true;
                break;
            }
        }

        map<int, int> pairInPublic;
        for (auto it : publics)
            pairInPublic[it.m_point]++;
        int nPairCountPublic = 0;
        for (auto it : pairInPublic) {
            if (it.second == 2 && In2Pair.find(it.first) != In2Pair.end())
                nPairCountPublic++;
        }

        if (nPairCountPublic == 0) //��2��
        {
            madehandStruct.nAppend = 0;
            //if (In2Pair.find(privates[0].m_point) != In2Pair.end() && In2Pair.find(privates[1].m_point) != In2Pair.end() && privates[0].m_point != privates[1].m_point)
            //    madehandStruct.nNeedHandsNum = 2;
            //else
            //    madehandStruct.nNeedHandsNum = 0;
            madehandStruct.nNeedHandsNum = 2;

            if (In2Pair.find(publicsV[0]) != In2Pair.end() && In2Pair.find(publicsV[1]) != In2Pair.end())
                madehandStruct.nRank = 0;
            else if (In2Pair.find(publicsV[0]) != In2Pair.end())
                madehandStruct.nRank = 1;
            else
                madehandStruct.nRank = 2;
        }
        else if (privates[0].m_point == privates[1].m_point && nPairCountPublic == 1) //���ֶ�_������
        {
            madehandStruct.nAppend = 1;
            madehandStruct.nNeedHandsNum = 2;
            int nMadePairPoint = privates[0].m_point;
            madehandStruct.nRank = 2;

            if (nMadePairPoint > publicsV[0])
                madehandStruct.nRank = 0;
            else {
                if (publicsV.size() >= 2)
                    if (nMadePairPoint > publicsV[1])
                        madehandStruct.nRank = 1;
            }
        }
        else if (privates[0].m_point != privates[1].m_point && nPairCountPublic == 1) //��_������
        {
            madehandStruct.nAppend = 2;
            madehandStruct.nNeedHandsNum = 1;
            madehandStruct.nRank = 3;

            if (privates[0].m_point == publicsV[0] || privates[1].m_point == publicsV[0]) {
                if (blIsMaxTick)
                    madehandStruct.nRank = 0;
                else
                    madehandStruct.nRank = 1;
            }
            else if (privates[0].m_point == publicsV[1] || privates[1].m_point == publicsV[1])
                madehandStruct.nRank = 2;
        }
        else if (nPairCountPublic == 2)
        {
            madehandStruct.nAppend = 3;
            madehandStruct.nNeedHandsNum = 0;
        }
    }
    break;
    case MAX_PAIR:
    {
        //���ֶ�: hands=2: append=1:����, rank: ����Դ�С����A�Ĳ�ࣩ
        //���ֶ�: hands=2: append=0:�ǳ���, rank��0=�ζԣ�1=�δζ�
        //���ԣ�hands=1,append=�Եĵ���(ע�������������)��rank:�߽ŵ�A�ľ���
        //�Ƕ��ԣ�hands=1��append=0��rank:0=�ζԣ�1=�δζ�
        //���ƶԣ�hands=0
        set<int> publicsS;
        for (auto it : publics)
            publicsS.insert(it.m_point);
        vector<int> publicsV(publicsS.rbegin(), publicsS.rend()); //�Ӵ�С

        set<int> privatesS;
        for (auto it : privates)
            privatesS.insert(it.m_point);
        vector<int> privatesV(privatesS.rbegin(), privatesS.rend());

        int nPointInPair = pClasses.m_pair[0].m_point; //ֻ������һ��
        int nKicker = 0;
        if (privatesV.size() == 2)
            nKicker = privatesV[0] == nPointInPair ? privatesV[1] : privatesV[0];

        if (privatesV.size() == 1)
            nNeedHands = 2;
        else {
            if (privatesV[0] == nPointInPair || privatesV[1] == nPointInPair)
                nNeedHands = 1;
            else
                nNeedHands = 0;
        }
        madehandStruct.nNeedHandsNum = nNeedHands;

        if (nNeedHands == 2) { //���ֶ�
            if (nPointInPair > publicsV[0]) {//����
                madehandStruct.nAppend = 1;
                madehandStruct.nRank = 14 - nPointInPair;
            }
            else { //�ǳ���
                madehandStruct.nRank = 2;
                if (publicsV.size() >= 3) {
                    if (nPointInPair >= publicsV[2])
                        madehandStruct.nRank = 1; //�δζ�
                }
                if (publicsV.size() >= 2) {
                    if (nPointInPair >= publicsV[1])
                        madehandStruct.nRank = 0; //�ζ�
                }
            }
        }
        else { //��
            if (nPointInPair == publicsV[0]) { //����
                madehandStruct.nAppend = nPointInPair; //�����������
                madehandStruct.nRank = 14 - nKicker;
                if (nPointInPair > nKicker)
                    madehandStruct.nRank--;
            }
            else {
                madehandStruct.nRank = 2;
                if (publicsV.size() >= 3) {
                    if (nPointInPair == publicsV[2])
                        madehandStruct.nRank = 1; //�δζ�
                }
                if (publicsV.size() >= 2) {
                    if (nPointInPair == publicsV[1])
                        madehandStruct.nRank = 0; //�ζ�
                }
            }
        }
    }
    break;
    case MAX_HIGH_CARD: //rankΪ���ŵ�A�ľ���
    {
        madehandStruct.nNeedHandsNum = 2;

        set<int> privatesS;
        for (auto it : publics)
            privatesS.insert(it.m_point);
        vector<int> privatesV(privatesS.rbegin(), privatesS.rend());

        madehandStruct.nRank = 14 - privatesV[0];
    }
    break;
    } //end of switch

    return madehandStruct;
}

//���ۺ���
//typedef enum { ev_nuts, ev_sec_nuts, ev_large, ev_sec_large, ev_middle, ev_small, ev_catch_bulff }EvaluateClass;
//typedef enum { evsub_large, evsub_middle, evsub_small }EvaluateClass_sub;
//typedef enum { MAX_HIGH_CARD, MAX_PAIR, MAX_TWO_PAIR, MAX_THREE_OF_A_KIND, MAX_STRAIGHT, MAX_FLUSH, MAX_FULL_HOUSE, MAX_FOUR_OF_A_KIND, MAX_STRAIGHT_FLUSH, DRAW_STRAIGHT, DRAW_FLUSH, DRAW_OTHERS, DRAW_NONE }PokerClass;
void CPokerHand::setEvaluate(PokerEvaluate& pokerEvaluate, const int nMainGroup, const int nSubGroup, const double score)
{
    pokerEvaluate.rankGroup.nMainGroup = nMainGroup;
    pokerEvaluate.rankGroup.nSubGroup = nSubGroup;
    pokerEvaluate.rankGroup.dlScore = score;
}

void CPokerHand::setRankGroup(RankGroup& rankGroup, const int nMainGroup, const int nSubGroup, const double score)
{
    rankGroup.nMainGroup = nMainGroup;
    rankGroup.nSubGroup = nSubGroup;
    rankGroup.dlScore = score;
}

//riverʱ�ж��Ƿ�����ֻ��ץզ��
bool CPokerHand::isRankCatchBluff(const  MadehandStruct& madehandStruct, const DrawStruct& drawStruct, const PublicStruct& publicStruct)
{

    //һ�ų�˳��˳���£����������Ͷ���
    if (publicStruct.nNeedtoStraight == 1 && Rank_PokerClass[madehandStruct.pokerClass] >= Rank_PokerClass[MAX_PAIR] && Rank_PokerClass[madehandStruct.pokerClass] < Rank_PokerClass[MAX_STRAIGHT]) {
        return true;
    }

    //һ�ųɻ��������£����������Ͷ���
    if (publicStruct.nNeedtoFlush == 1 && Rank_PokerClass[madehandStruct.pokerClass] >= Rank_PokerClass[MAX_PAIR] && Rank_PokerClass[madehandStruct.pokerClass] < Rank_PokerClass[MAX_FLUSH]) {
        return true;
    }

    //3����2�ԣ���«���¶���
    if ((publicStruct.pokerClass == MAX_THREE_OF_A_KIND || publicStruct.pokerClass == MAX_TWO_PAIR) &&
        Rank_PokerClass[madehandStruct.pokerClass] >= Rank_PokerClass[MAX_PAIR] && Rank_PokerClass[madehandStruct.pokerClass] < Rank_PokerClass[MAX_FULL_HOUSE]) {
        return true;
    }

    //�����ų�˳�ģ���ԭ˳��
    return false;
}


//�����ƴ��
RankGroup CPokerHand::getPokerEvaluate_made(const  MadehandStruct& madehandStruct, const DrawStruct& drawStruct, const PublicStruct& publicStruct, bool blRiver)
{
    RankGroup rankGroup;

    //A0: ev_nuts
        //ev_nuts - evsub_large
        //
        //2_nutsͬ��˳
    if (madehandStruct.pokerClass == MAX_STRAIGHT_FLUSH && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_large);
        goto CHECK_RIVER;
    }
    //1_nutsͬ��˳
    if (madehandStruct.pokerClass == MAX_STRAIGHT_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_large);
        goto CHECK_RIVER;
    }
    //2_nutsը��
    if (madehandStruct.pokerClass == MAX_FOUR_OF_A_KIND && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_large);
        goto CHECK_RIVER;
    }
    //1_nutsը��
    if (madehandStruct.pokerClass == MAX_FOUR_OF_A_KIND && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_large);
        goto CHECK_RIVER;
    }
    //2_nuts��«
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_large);
        goto CHECK_RIVER;
    }

    //A1: ev_nuts - evsub_middle
    // 
    //2_Сͬ��˳
    if (madehandStruct.pokerClass == MAX_STRAIGHT_FLUSH && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 1) {
        setRankGroup(rankGroup, ev_nuts, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_���«
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_�к�«
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_middle);
        goto CHECK_RIVER;
    }

    //A2: ev_nuts - evsub_small
    // 
    //n_nuts��(��һ�ų�ͬ��˳)
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum >= 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_small);
        goto CHECK_RIVER;
    }
    //2_nuts˳
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_small);
        goto CHECK_RIVER;
    }
    //2_��set
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 == 0 && publicStruct.nNeedtoStraight == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_small);
        goto CHECK_RIVER;
    }

    //B0; ev_sec_nuts
    //ev_sec_nuts - evsub_large
    // 
    //1_nuts����ը��_����A(�س�ˮ)
    if (madehandStruct.pokerClass == MAX_FOUR_OF_A_KIND && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 1) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        goto CHECK_RIVER;
    }
    //1_nuts˳(�س�ˮ)
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        goto CHECK_RIVER;
    }

    //B1: ev_sec_nuts - evsub_middle
    // 
    //2_����3����« 
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 2) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_�����ֶ�3����«
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 1) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_С��«
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 3 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_��˳
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_��set(������set)
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank <= 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_middle);
        goto CHECK_RIVER;
    }

    //B2: ev_sec_nuts - evsub_small
    // 
    //2_��(rank <=2)
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank <= 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_small);
        goto CHECK_RIVER;
    }
    //2_Сset
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 2 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_small);
        goto CHECK_RIVER;
    }
    //2_��2��(��2��)
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank <= 1 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight == 0 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_small);
        goto CHECK_RIVER;
    }

    //C0: ev_large
    //ev_large - evsub_large
    // 
    //1_���«
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        goto CHECK_RIVER;
    }
    //1_����3����«
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 1) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        goto CHECK_RIVER;
    }
    //1_��˳
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 1 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        goto CHECK_RIVER;
    }
    //2_nuts��_�й���
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        goto CHECK_RIVER;
    }

    //C1: ev_large - evsub_middle
    // 
    //1_Сͬ��˳
    if (madehandStruct.pokerClass == MAX_STRAIGHT_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1) {
        setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_С��(rank >2)
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank > 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_С˳
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_˳_�й���
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //1_Сͬ��˳
    if (madehandStruct.pokerClass == MAX_STRAIGHT_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1) {
        setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_set_2�ų�˳
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 2 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight == 2) {
        setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }

    //C2: ev_large - evsub_small
    // 
    //1_3��_С�ߣ�1_3��_���ߣ�
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_large, evsub_small);
        goto CHECK_RIVER;
    }

    //2_С2��
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight == 0 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_small);
        goto CHECK_RIVER;
    }

    //�󳬶� C(A-Q)
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank <= 2 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_large, evsub_small);
        goto CHECK_RIVER;
    }

    //D0: ev_sec_large
    //ev_sec_large - evsub_large
    // 
    //1_nuts��_�й��ԣ�turn C1,river D0��
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 1) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_large);
        else
            setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_2��_2�ų�˳��turn C1,river D0��
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight == 2 && publicStruct.nPair == 0) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_large);
        else
            setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //�г��� D
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank <= 5 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_large);
        goto CHECK_RIVER;
    }
    //2_��_�й��ԣ�turn C2,river D0��
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 1) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_large);
        else
            setRankGroup(rankGroup, ev_large, evsub_small);
        goto CHECK_RIVER;
    }
    //1_3��_2�ų�˳��turn C1,river D0��
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 1 &&
        (publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoStraight == 2)) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_large);
        else
            setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }

    //D1: ev_sec_large - evsub_middle
    // 
    //2_��2��_(��2��)_�й���
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank <= 1 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1 && publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_˳_2�ųɻ�
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush == 2) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //1, 2_nuts��_һ�ų�ͬ��˳
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nRank == 0 && madehandStruct.nAppend == 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //1_��(<=2)
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //1_С��«
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank > 1 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_2��_2������
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1 &&
        publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //����_������
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //���Զ�T D
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend >= 10 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //1_˳_�й���
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank != 2 && madehandStruct.nAppend == 0 &&
        (publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoFlush != 1 || publicStruct.nPair == 1)) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }

    //D2: ev_sec_large - evsub_small
    // 
    //2_set_1�ų�˳��turn C2,river D1��
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND &&
        (publicStruct.nNeedtoFlush == 1)) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_small);
        else
            setRankGroup(rankGroup, ev_large, evsub_small);
        goto CHECK_RIVER;
    }
    //2_set_1�ųɻ���turn C2,river D1��
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND &&
        (publicStruct.nNeedtoStraight == 1)) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_small);
        else
            setRankGroup(rankGroup, ev_large, evsub_small);
        goto CHECK_RIVER;
    }
    //A�Դ���(>=Q)
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 1 && madehandStruct.nAppend == 14 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_small);
        goto CHECK_RIVER;
    }
    //2_2��_2�ųɻ�
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush == 2 && publicStruct.nNeedtoStraight != 1 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_large, evsub_small);
        goto CHECK_RIVER;
    }
    //2_set_2�ųɻ���turn C1,river D1��
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 2 &&
        publicStruct.nNeedtoFlush == 2 && publicStruct.nNeedtoStraight != 1) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_small);
        else
            setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //1_3��_2�ųɻ���turn C1,river D1��
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 1 &&
        (publicStruct.nNeedtoFlush == 2 && publicStruct.nNeedtoStraight != 1)) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_small);
        else
            setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }

    //ev_middle
    //E0: ev_middle - evsub_large
    // 
    //1_��_�й���
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        goto CHECK_RIVER;
    }
    //1_˳_2�ųɻ�
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank != 2 && madehandStruct.nAppend == 0 &&
        (publicStruct.nNeedtoFlush == 2)) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        goto CHECK_RIVER;
    }
    //2_�����ж�3����«(1_�ζ�3����«)
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum > 0 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 1) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        goto CHECK_RIVER;
    }

    //E1: ev_middle - evsub_middle
    // 
    //������T E
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 4 && madehandStruct.nAppend >= 9 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_middle, evsub_middle);
        goto CHECK_RIVER;
    }
    //С���� E
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank > 5 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_middle, evsub_middle);
        goto CHECK_RIVER;
    }

    //E2: ev_middle - evsub_small
    // 
    //����СT E
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank > 4 && madehandStruct.nAppend >= 9 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_middle, evsub_small);
        goto CHECK_RIVER;
    }
    //����_�й���(���Զ���riverǰΪD2)
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 1 && madehandStruct.nAppend == 2 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_middle, evsub_small);
        if(madehandStruct.nRank == 0 && !blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_small);
        goto CHECK_RIVER;
    }
    //С���� E
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nAppend < 9 && madehandStruct.nAppend > 4 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_middle, evsub_small);
        goto CHECK_RIVER;
    }

    //F0: ev_small
    //ev_small - evsub_large
    // 
    //1_��_����ͬ��˳
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 2 && madehandStruct.nAppend == 1) {
        setRankGroup(rankGroup, ev_small, evsub_large);
        goto CHECK_RIVER;
    }
    //����_2�ųɻ�
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush == 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_large);
        goto CHECK_RIVER;
    }
    //����_2�ųɻ�
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 &&
        publicStruct.nNeedtoFlush == 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_large);
        goto CHECK_RIVER;
    }

    //F1: ev_small - evsub_middle
    // 
    //1_С˳
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_small, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_˳_��3����2�� ?
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        (publicStruct.pokerClass == MAX_THREE_OF_A_KIND || publicStruct.nPair == 2) && publicStruct.nNeedtoFlush != 1) {
        setRankGroup(rankGroup, ev_small, evsub_middle);
        goto CHECK_RIVER;
    }
    //����_2�ųɻ�_������
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush == 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_middle);
        goto CHECK_RIVER;
    }
    //����_2�ųɻ�_������
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 2 &&
        publicStruct.nNeedtoFlush == 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_middle);
        goto CHECK_RIVER;
    }
    //���ִζ�
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_middle);
        goto CHECK_RIVER;
    }
    //�ζ�
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_middle);
        goto CHECK_RIVER;
    }

    //F2: ev_small - evsub_small
    // 
    //2_2��_1�ųɻ�(2_2��_1�ų�˳)
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        (publicStruct.nNeedtoFlush == 1 || publicStruct.nNeedtoStraight == 1)) {
        setRankGroup(rankGroup, ev_small, evsub_small);
        goto CHECK_RIVER;
    }
    //���ִζ�_������
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_small);
        goto CHECK_RIVER;
    }
    //�ζ�_������
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 2 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_small);
        goto CHECK_RIVER;
    }
    //2_С2��_�й���
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1 && publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_small, evsub_small);
        goto CHECK_RIVER;
    }

    //G��ev_catch_bulff
        //ev_catch_bulff - evsub_small
        //
        //���ִδζ�
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_catch_bulff, evsub_small);
        goto CHECK_RIVER;
    }
    //�δζ�
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_catch_bulff, evsub_small);
        goto CHECK_RIVER;
    }
    //���ִδζ�_������
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_catch_bulff, evsub_small);
        goto CHECK_RIVER;
    }
    //�δζ�_������
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 2 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_catch_bulff, evsub_small);
        goto CHECK_RIVER;
    }


    CHECK_RIVER:
    //river��Ҫ��һЩ��������Ϊץզ��
    if (blRiver) {
        if (isRankCatchBluff(madehandStruct, drawStruct, publicStruct)) {
            int evsub = evsub_small; //ֻ�򵥷ִ��С
            if (Rank_PokerClass[madehandStruct.pokerClass] > Rank_PokerClass[MAX_PAIR]) evsub = evsub_large;
                setRankGroup(rankGroup, ev_catch_bulff, evsub);
        }

        //river�ѹ���ͬ��˳��˳ nuts����Ϊev_sec_nuts��evsub_large����ʾ��������ע������fold�� river�ѹ���ͬ��˳��˳����«���Դ���3��������?����nuts����Ϊץզ��
        if (publicStruct.nAppend == 1)
            setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        else if (publicStruct.nAppend == 2)
            setRankGroup(rankGroup, ev_catch_bulff, evsub_small);
    }

    return rankGroup;
}

//��draw�����Ĵ��(ֻ�з�river�Ż����)
//typedef enum { ev_nuts, ev_sec_nuts, ev_large, ev_sec_large, ev_middle, ev_small, ev_catch_bulff }EvaluateClass;
//typedef enum { evsub_large, evsub_middle, evsub_small }EvaluateClass_sub;
//typedef enum { MAX_HIGH_CARD, MAX_PAIR, MAX_TWO_PAIR, MAX_THREE_OF_A_KIND, MAX_STRAIGHT, MAX_FLUSH, MAX_FULL_HOUSE, MAX_FOUR_OF_A_KIND, MAX_STRAIGHT_FLUSH, DRAW_STRAIGHT, DRAW_FLUSH, DRAW_OTHERS, DRAW_NONE }PokerClass;
RankGroup CPokerHand::getPokerEvaluate_draw(const  MadehandStruct& madehandStruct, const DrawStruct& drawStruct, const PublicStruct& publicStruct)
{
    RankGroup rankGroup;

    //ev_nuts
        //��˳draw nuts�����޹���)
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 &&
        drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 && drawStruct.nRank == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_large);
        return rankGroup;
    }

    //ev_sec_nuts
        //��˳draw��
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 &&
        drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        return rankGroup;
    }
    //2d_nuts��
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 && drawStruct.nRank == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        return rankGroup;
    }
    //2d_����outs >= 15
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 && drawStruct.nOutsNum >= 15 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        return rankGroup;
    }
    //2d_˳ outs>=8 a_outs>=5
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 2 && drawStruct.nOutsNum >= 8 && drawStruct.nAppendOutsNum >= 5 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        return rankGroup;
    }

    //ev_large
        //1d_nuts��
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 1 && drawStruct.nRank == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        return rankGroup;
    }
    //2d_��
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 && drawStruct.nRank <= 2 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        return rankGroup;
    }
    //2d_�� a_outs >= 5
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 && drawStruct.nAppendOutsNum >= 5 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        return rankGroup;
    }
    //2d_˳ outs >= 8
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 2 && drawStruct.nOutsNum >= 8 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        return rankGroup;
    }

    //ev_sec_large
        //2d_������������ԣ�
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 &&
        (publicStruct.nPair == 0 || publicStruct.nPair == 1)) {
        setRankGroup(rankGroup, ev_sec_large, evsub_large);
        return rankGroup;
    }
    //1d nuts���������ԣ�
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 1 && drawStruct.nRank == 0 &&
        publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_large);
        return rankGroup;
    }
    //2d_˳ outs >= 6 �޹��ԣ����У�
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 2 && drawStruct.nOutsNum >= 6 && drawStruct.nRank <= 1 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_large, evsub_large);
        return rankGroup;
    }
    //1d_��˳ outs >= 6(�޹���)
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 1 && drawStruct.nOutsNum >= 6 && drawStruct.nRank == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_large, evsub_large);
        return rankGroup;
    }

    //ev_middle
        //1d_�󻨣������й��ԣ�
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 1 && drawStruct.nRank <= 2 &&
        (publicStruct.nPair == 0 || publicStruct.nPair == 1)) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        return rankGroup;
    }
    //2d_˳ outs >= 6���й��ԣ�
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 2 && drawStruct.nOutsNum >= 6 &&
        publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        return rankGroup;
    }
    //2d_˳ outs >= 6 �޹��ԣ�С��
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 2 && drawStruct.nOutsNum >= 6 && drawStruct.nRank == 2 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        return rankGroup;
    }
    //1d_��˳ outs >= 6(�й���)
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 1 && drawStruct.nOutsNum >= 6 && drawStruct.nRank <= 1 &&
        publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        return rankGroup;
    }

    //ev_small
        //2d_���� outs >= 7(?6)
    if (drawStruct.pokerClass == DRAW_OTHERS && drawStruct.nNeedHandsNum == 2 && drawStruct.nAppendOutsNum >= 6) {
        setRankGroup(rankGroup, ev_small, evsub_large);
        return rankGroup;
    }
    //2d(1d)_˳(����) outs = 4 a_outs >= 2
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum >= 1 && drawStruct.nOutsNum >= 3 && drawStruct.nRank <= 1 && drawStruct.nAppendOutsNum >= 2) {
        setRankGroup(rankGroup, ev_small, evsub_large);
        return rankGroup;
    }

    return rankGroup;
}

PokerEvaluate CPokerHand::getPokerEvaluate(const MyCards& privates, const MyCards& publics)
{
    bool blRiver = false;
    if (publics.size() == 5)
        blRiver = true;

    PokerEvaluate pokerEvaluate;
    pokerEvaluate.publicStruct = getPublicStruct(publics);
    pokerEvaluate.madehandStruct = getMadehandStruct(privates, publics);
    pokerEvaluate.drawStruct = getDrawStruct(privates, publics, pokerEvaluate.publicStruct);

    MadehandStruct& madehandStruct = pokerEvaluate.madehandStruct;
    DrawStruct& drawStruct = pokerEvaluate.drawStruct;
    PublicStruct& publicStruct = pokerEvaluate.publicStruct;

    RankGroup rankGroup_made, rankGroup_draw;
    rankGroup_made = getPokerEvaluate_made(madehandStruct, drawStruct, publicStruct, blRiver);
    rankGroup_draw = getPokerEvaluate_draw(madehandStruct, drawStruct, publicStruct);

    pokerEvaluate.rankGroup = rankGroup_made > rankGroup_draw ? rankGroup_made : rankGroup_draw;

    if (pokerEvaluate.rankGroup.nMainGroup == -1) {
        pokerEvaluate.rankGroup.nMainGroup = ev_nothing;
        pokerEvaluate.rankGroup.nSubGroup = 0;
    }


    //for test
    //cout << "made: " << rankGroup_made.nMainGroup << "," << rankGroup_made.nSubGroup << "\t";
    //cout << "draw: " << rankGroup_draw.nMainGroup << "," << rankGroup_draw.nSubGroup << endl;

    return pokerEvaluate;
}
