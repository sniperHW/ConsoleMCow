#include "CPokerHand.h"
#include <iterator>

using namespace std;

bool CPokerHand::issubset(const set<int>& s, const set<int>& father) //子集
{
    set<int> tmp;
    set_difference(s.begin(), s.end(), father.begin(), father.end(), inserter(tmp, tmp.begin()));
    return (tmp.empty()) ? true : false;
}

//有顺子则返回最大的顺子，要求myCards先按降序排序
MyCards CPokerHand::toStraight(const MyCards& myCards)
{
    MyCards ret;
    if (myCards.size() < 5)
        return ret;

    set<myCard> pointS;
    vector<set<myCard>> pointSes; //所有组成顺子的，最后一个为最大的（其实可以冲掉前面的）
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

        if (!elem5) //不满5退出
            break;

        //成功则退出
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

//返回所有形成1张成顺的组合和outs，cards要求降序，cards只能5或6
vector<OnecardToStraight> CPokerHand::getOnecardToStraight(const MyCards& cards)
{
    vector<OnecardToStraight> ret;

    //   if (cards.size() != 5 && cards.size() != 6) {
    //       cout << "cards num not 5 or 6.";
     //      return ret;
     //  }

    set<int> pointS; //升序无重复
    for (auto onecard : cards)
        pointS.insert(onecard.m_point);

    //  if (pointS.size() < 5)
    //      return ret;

    if (*pointS.rbegin() == 14)
        pointS.insert(1);

    vector<int> pointV(pointS.begin(), pointS.end());

    for (int i = 0; i < pointV.size() - 3; i++) {
        int pointDis = pointV[i + 3] - pointV[i];
        if (pointDis == 3 || pointDis == 4) { //单张成顺成立
            OnecardToStraight newCombo;
            for (int j = i; j < i + 4; j++) {
                newCombo.cardPoints.push_back(pointV[j]);
                if (j < i + 3) {
                    int test = pointV[j + 1] - pointV[j];
                    if (pointV[j + 1] - pointV[j] != 1)
                        newCombo.outsPoints.push_back(pointV[j] + 1); //中间是否有隔张，有则是卡顺
                }
            }

            if (newCombo.outsPoints.empty()) { //无隔张填写的，两头顺
                if (pointV[i] != 1)
                    newCombo.outsPoints.push_back(pointV[i] - 1);
                if (pointV[i + 3] != 14)
                    newCombo.outsPoints.push_back(pointV[i + 3] + 1);
            }

            ret.push_back(newCombo);
        }
    }

    //如果某组合的outs是另一个的子集并且是小的那些，则无效；如果outs一样则取大的
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
                    if (p->outsPoints.size() == 1) { //都一个的保留大的
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

//返回公牌与任意两张组合所能形成的最大顺子和outs,outs可能是一张或两张,如果不能组成顺子则draw中的一定是nuts顺
set<int> CPokerHand::getMaxPossibleStraightByBoard(const MyCards& publics, vector<int>& maxStraight)
{
    set<int> ret; //outs

    //   if (publics.size() != 3 && publics.size() != 4) {
           //cout << "cards num not 3 or 4.";
    //       return ret;
    //   }

    set<int> pointS; //升序无重复
    for (auto onecard : publics)
        pointS.insert(onecard.m_point);

    if (pointS.size() < 3)
        return ret;

    if (*pointS.rbegin() == 14)
        pointS.insert(1);

    vector<int> pointV(pointS.begin(), pointS.end());
    int nmaxInstraight = 0;

    //先找出组成顺子的最大张
    for (int i = 0; i < pointV.size() - 2; i++) {
        int pointDis = pointV[i + 2] - pointV[i];
        if (pointDis == 2)
            nmaxInstraight = min(14, pointV[i + 2] + 2);
        else if (pointDis == 3)
            nmaxInstraight = min(14, pointV[i + 2] + 1);
        else if (pointDis == 4)
            nmaxInstraight = pointV[i + 2];
    }

    //填充最大顺子和outs
    if (nmaxInstraight != 0) {
        for (int i = 0; i < 5; i++) {
            maxStraight.push_back(nmaxInstraight - i);
            if (pointS.find(nmaxInstraight - i) == pointS.end())
                ret.insert(nmaxInstraight - i);
        }
    }

    return ret;
}

//返回draw顺的评价，
//nNeedHandsNum = 0,1,2 //0则代表不成立
//nOutsNum = 实际数
//nRank = 0nuts,1中,2小
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

        if (nNeedPrivate > 0) { //如果能一张成顺的牌没有private，则代表出了outs也是公牌成顺，则对应的outs无效
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

        if (find(maxStraight.begin(), maxStraight.end(), 14) != maxStraight.end() && (privatesS.find(14) != privatesS.end() || publicsS.find(14) != publicsS.end())) //公牌或hand有A则不管手牌是什么都是nuts draw
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
    if (nMaxSuitCount == 2) //3张成花时，顺的out要-1*outs(比如两头顺就是8-2=6)
        ret.nOutsNum -= (int)outsS.size();
    ret.nRank = 2 - nHighCardCount;

    return ret;
}

//返回组成后门顺的序列
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
                //检查outs是否导致公牌一张成顺
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

//返回draw花的评价
//nNeedHandsNum = 0,1,2 //0则代表不成立
//nOutsNum = 实际数
//nRank = 更大的有几张 0代表nuts
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
            ret.nNeedHandsNum = possibleflushs.size() == 2 ? 1 : 2;  //private的花色不一样，所以只可能用一张

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

//返回rank(更大的还有几张，0代表nuts), -1代表没有后门花(与后门顺返回类型不一样)
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

//检查是否构成顺或一张成顺,赋值函数配合getHighCardDraw,publicAdd1为4-5个，最大张为假设出的高张
bool CPokerHand::isClosetoStraight(const set<int>& publicAdd1)
{
    if (publicAdd1.size() != 4 && publicAdd1.size() != 5)
        return false;

    vector<int> tmp(publicAdd1.rbegin(), publicAdd1.rend());
    if (publicAdd1.size() == 5) {
        if (tmp[0] - tmp[4] == 4 || tmp[0] == 14 && tmp[1] == 5 || tmp[0] - tmp[3] <= 4) //成顺
            return true;
    }
    else if (publicAdd1.size() == 4) { //是否一张成顺
        if (tmp[0] - tmp[3] <= 4 || tmp[0] == 14 && tmp[1] <= 5)
            return true;
    }

    return false;
}

//检查是否有3张以上同色
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

//不能是成手对，一张成对，一张非对,如果接近顺子或花则无效
bool CPokerHand::getPairDraw(const MyCards& privates, const MyCards& publics)
{
    if (privates[0].m_point == privates[1].m_point) //对子不存在draw高张
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

//out:几张高张,rank:点数相加,nOutsNum 0代表无高张，*系数A、K=3，T-Q=2，其他1
DrawStruct CPokerHand::getHighCardDraw(const MyCards& privates, const MyCards& publics)
{
    DrawStruct ret;
    ret.nOutsNum = 0;
    ret.nRank = 0;

    if (privates[0].m_point == privates[1].m_point) //对子不存在draw高张
        return ret;

    int largestPublicRank = 0;
    for (auto it : publics) {
        if (it.m_point > largestPublicRank)
            largestPublicRank = it.m_point;
    }

    for (auto it : privates) {
        if (it.m_point > largestPublicRank) {
            //检查最大张加入后是否形成一张成顺，或本身有3张同花则不算（直接成顺、花）
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

//公牌的评价
PublicStruct CPokerHand::getPublicStruct(const MyCards& publics)
{
    PublicStruct ps;

    possibleClasses pClasses(publics);
    ps.pokerClass = pClasses.getPokerClass();
    ps.nNeedtoFlush = pClasses.m_needtoFlush;
    ps.nNeedtoStraight = pClasses.m_needtoStraight;
    ps.nPair = (int)pClasses.m_pair.size() / 2;
    ps.blOESDpossible = pClasses.m_blOESDpossible;

    for (auto it : publics) { //记录最大高张
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
                ps.nAppend = 2; //3条比一对大时可以抓咋
        }
    }

    return ps;
}

//draw牌的评价
DrawStruct CPokerHand::getDrawStruct(const MyCards& privates, const MyCards& publics, const PublicStruct& publicStruct)
{
    DrawStruct drawStruct;
    drawStruct.pokerClass = DRAW_NONE;

    if (publics.size() == 5)
        return drawStruct;

    //typedef enum { MAX_HIGH_CARD, MAX_PAIR, MAX_TWO_PAIR, MAX_THREE_OF_A_KIND, MAX_STRAIGHT, MAX_FLUSH, MAX_FULL_HOUSE, MAX_FOUR_OF_A_KIND, MAX_STRAIGHT_FLUSH, DRAW_STRAIGHT, DRAW_FLUSH, DRAW_OTHERS, DRAW_NONE }PokerClass;
    //排除无意义的draw, draw花：2对以上，不包括顺  draw顺：2对以上、两张成花  draw其他：2对以上、两张成花、一张成顺
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
            if (backdoorFlushDraw == 0) //nuts后门花outs算2，其他算1
                nbackdoorDrawOuts += 2;
            else
                nbackdoorDrawOuts += 1;
        }

        backdoorStraightDraw = getBackdoorStraightDraw(privates, publics);
        if (!backdoorStraightDraw.empty())
            nbackdoorDrawOuts += 2; //后门顺outs算2
    }

    //后门花outs系数，nuts花+2，其他花+1，后面顺+2，高张K-A+3,T-Q+2,其他加1，对+5

    if (flushDraw.nNeedHandsNum != 0) { //存在draw同花
        drawStruct.pokerClass = DRAW_FLUSH;
        drawStruct.nNeedHandsNum = flushDraw.nNeedHandsNum;
        drawStruct.nRank = flushDraw.nRank;
        drawStruct.nOutsNum = flushDraw.nOutsNum + straightDraw.nOutsNum;
        drawStruct.nAppendOutsNum = highcardDraw.nOutsNum + nPairDrawOuts;
        if (straightDraw.nOutsNum > 0) //顺花双draw要减2个outs
            drawStruct.nOutsNum -= straightDraw.nOutsNum / 4;
    }
    else if (straightDraw.nNeedHandsNum != 0) { //存在draw顺
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
        drawStruct.nRank = -1; //?怎么评价,或者无需评价
    }

    return drawStruct;
}

//成手牌的评价
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
    case MAX_STRAIGHT_FLUSH: //rank:0:nuts,1:小（不是都排最后的都是 nuts）
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
            madehandStruct.nRank = nPrivateTotalPoint > pClasses.m_straight_flush[4].m_point || nPrivateTotalPoint >= 10 ? 0 : 1; //||A顺
        else if (nNeedHands == 2)
            madehandStruct.nRank = nPrivateTotalPoint > pClasses.m_straight_flush[4].m_point + pClasses.m_straight_flush[3].m_point || nPrivateTotalPoint >= 19 ? 0 : 1;
    }
    break;
    case MAX_FOUR_OF_A_KIND: //rank:0,1,1代表什么都没有,hands=0;nAppend=1代表公牌成4条，rank为离A的距离
    {
        for (auto one_private : privates) {
            if (find(pClasses.m_king.begin(), pClasses.m_king.end(), one_private) != pClasses.m_king.end())
                nNeedHands++;
        }

        int nMaxInPrivate = privates[0].m_point >= privates[1].m_point ? privates[0].m_point : privates[1].m_point;

        if (nNeedHands == 0) {
            madehandStruct.nAppend = 1; //公牌4条
            madehandStruct.nNeedHandsNum = 1;
            madehandStruct.nRank = 14 - nMaxInPrivate;

            //if (privates[0].m_point == 14 || privates[1].m_point == 14) {
            //    madehandStruct.nNeedHandsNum = 1;
            //    madehandStruct.nRank = 0;
           // }
            //else { //代表king和手牌无关
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
    case MAX_FULL_HOUSE: //rank:0,1,2,3; nAppend=1代表公牌有3条,rank代表对大小; nAppend=2代表公牌有3条,但只当对使用
    {
        madehandStruct.nRank = 3; //默认为最小值

        set<int> publicsS;
        map<int, int> point2num;
        for (auto it : publics) {
            publicsS.insert(it.m_point);
            point2num[it.m_point]++;
        }
        vector<int> publicV(publicsS.rbegin(), publicsS.rend()); //从大到小
        vector<int> publicV_exclude3 = publicV; //公牌3张外的集合（flop则为空）
        for (auto p = publicV_exclude3.begin(); p != publicV_exclude3.end(); p++) {
            if (point2num[*p] == 3) {
                publicV_exclude3.erase(p);
                break;
            }
        }

        set<int> privatesS;
        for (auto it : privates)
            privatesS.insert(it.m_point);
        vector<int> privatesV(privatesS.rbegin(), privatesS.rend()); //从大到小

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

        if (nMaxNum == 3 && nPoint_MaxNum == three_Infullhouse) { //可能公牌的3条只当pair用
            madehandStruct.nAppend = 1; //公牌有3条

            if (!publicV_exclude3.empty()) {
                //rank0 公对是超对
                if (pair_Infullhouse > publicV_exclude3[0])
                    if (madehandStruct.nRank == 3) madehandStruct.nRank = 0;

                //rank1 公对是顶对
                if (pair_Infullhouse == publicV_exclude3[0])
                    if (madehandStruct.nRank == 3) madehandStruct.nRank = 1;

                //rank2 公对是中对
                if (publicV_exclude3.size() >= 2) {
                    if (pair_Infullhouse >= publicV_exclude3[1])
                        if (madehandStruct.nRank == 3) madehandStruct.nRank = 2;
                }
            }
            else { //flop只有3张时
                if (pair_Infullhouse == 14) { //AA为rank0
                    if (madehandStruct.nRank == 3) madehandStruct.nRank = 0;
                }
                else if (pair_Infullhouse >= 11) { //JJ-KK为rank1
                    if (madehandStruct.nRank == 3) madehandStruct.nRank = 1;
                }
                else if (pair_Infullhouse >= 8) { //88-TT为rank2
                    if (madehandStruct.nRank == 3) madehandStruct.nRank = 2;
                } 
            }
        }
        else {
            //rank0,代表公牌最大张为对，private中最大张和次大张,为nuts
            if (nMaxNum == 3)
                madehandStruct.nAppend = 2;

            if (privatesV.size() == 2) {
                bool blOnly0pair = false; //只有最高张是对
                if (point2num[publicV[0]] == 2) blOnly0pair = true;
                for (int i = 1; i < publicV.size(); i++)
                    if (point2num[publicV[i]] != 1)  blOnly0pair = false;

                if (blOnly0pair && publicV[0] == privatesV[0] && publicV[1] == privatesV[1])
                    madehandStruct.nRank = 0; //nuts
            }

            //rank1,葫芦中构成3张的有private,并且3张的是最大张
            if (publicV[0] == three_Infullhouse)
                if (madehandStruct.nRank == 3) madehandStruct.nRank = 1;

            //rank2,葫芦中构成3张的有private,并且3张的是次大张
            if (publicV[1] == three_Infullhouse)
                if (madehandStruct.nRank == 3) madehandStruct.nRank = 2;
        }
    }
    break;
    case MAX_FLUSH: //hands:1,2；rank:14-private大张-public大的；append:1代表1张成同花顺
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

        //检查是否有1张成同花顺的
        vector<OnecardToStraight> oneCardToFlushStraight = getOnecardToStraight(publicsWithSameSuit);
        if (!oneCardToFlushStraight.empty())
            madehandStruct.nAppend = 1;
    }
    break;
    case MAX_STRAIGHT: //hands: 1，2；rank:0,1,2(公牌能组成的最大顺的outs含private几张，除非已成顺子是A)   ??公牌已经是nuts顺时
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
    case MAX_THREE_OF_A_KIND: //hands=2:set hands=1:triples; (0代表公牌3张)  rank:set,0顶,1>=次高，2其他；triples，rank代表离A有多少张（减公牌高张）
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
        vector<int> publicsV(publicsS.rbegin(), publicsS.rend()); //从大到小

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
        //纯2对：append=0，hands=2并且不同,rank:0:最大张和次大张，1：最大涨和任意，2：其他；
        //成手对_带公对：append=1，hands=2，rank=0:超对，rank=1:次高队，rank=2其他
        //对_带公对：append=2，hands=1，rank=0:顶对顶踢，1：顶对，2次顶对，3其他
        //公牌2公对：append=3,hands=0
        //?带公对时无法区分超对和顶对，对的大小，因此可能带公对的要比不带的评价高（outs少也有合理性）
        set<int> publicsS;
        for (auto it : publics)
            publicsS.insert(it.m_point);
        vector<int> publicsV(publicsS.rbegin(), publicsS.rend()); //从大到小

        set<int> In2Pair;
        for (auto it : pClasses.m_pair)
            In2Pair.insert(it.m_point);

        int nPrivateNotInpairPoint = 0; //private非成对的点数,用于判断是否是顶踢
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

        if (nPairCountPublic == 0) //纯2对
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
        else if (privates[0].m_point == privates[1].m_point && nPairCountPublic == 1) //成手对_带公对
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
        else if (privates[0].m_point != privates[1].m_point && nPairCountPublic == 1) //对_带公对
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
        //成手对: hands=2: append=1:超对, rank: 代表对大小（到A的差距）
        //成手对: hands=2: append=0:非超对, rank：0=次对，1=次次对
        //顶对：hands=1,append=对的点数(注意这个定义特殊)，rank:踢脚到A的距离
        //非顶对：hands=1，append=0，rank:0=次对，1=次次对
        //公牌对：hands=0
        set<int> publicsS;
        for (auto it : publics)
            publicsS.insert(it.m_point);
        vector<int> publicsV(publicsS.rbegin(), publicsS.rend()); //从大到小

        set<int> privatesS;
        for (auto it : privates)
            privatesS.insert(it.m_point);
        vector<int> privatesV(privatesS.rbegin(), privatesS.rend());

        int nPointInPair = pClasses.m_pair[0].m_point; //只可能有一对
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

        if (nNeedHands == 2) { //成手对
            if (nPointInPair > publicsV[0]) {//超对
                madehandStruct.nAppend = 1;
                madehandStruct.nRank = 14 - nPointInPair;
            }
            else { //非超对
                madehandStruct.nRank = 2;
                if (publicsV.size() >= 3) {
                    if (nPointInPair >= publicsV[2])
                        madehandStruct.nRank = 1; //次次对
                }
                if (publicsV.size() >= 2) {
                    if (nPointInPair >= publicsV[1])
                        madehandStruct.nRank = 0; //次对
                }
            }
        }
        else { //对
            if (nPointInPair == publicsV[0]) { //顶对
                madehandStruct.nAppend = nPointInPair; //这个定义特殊
                madehandStruct.nRank = 14 - nKicker;
                if (nPointInPair > nKicker)
                    madehandStruct.nRank--;
            }
            else {
                madehandStruct.nRank = 2;
                if (publicsV.size() >= 3) {
                    if (nPointInPair == publicsV[2])
                        madehandStruct.nRank = 1; //次次对
                }
                if (publicsV.size() >= 2) {
                    if (nPointInPair == publicsV[1])
                        madehandStruct.nRank = 0; //次对
                }
            }
        }
    }
    break;
    case MAX_HIGH_CARD: //rank为高张到A的距离
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

//评价函数
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

//river时判断是否牌力只是抓咋牌
bool CPokerHand::isRankCatchBluff(const  MadehandStruct& madehandStruct, const DrawStruct& drawStruct, const PublicStruct& publicStruct)
{

    //一张成顺，顺以下，对以上牌型都是
    if (publicStruct.nNeedtoStraight == 1 && Rank_PokerClass[madehandStruct.pokerClass] >= Rank_PokerClass[MAX_PAIR] && Rank_PokerClass[madehandStruct.pokerClass] < Rank_PokerClass[MAX_STRAIGHT]) {
        return true;
    }

    //一张成花，花以下，对以上牌型都是
    if (publicStruct.nNeedtoFlush == 1 && Rank_PokerClass[madehandStruct.pokerClass] >= Rank_PokerClass[MAX_PAIR] && Rank_PokerClass[madehandStruct.pokerClass] < Rank_PokerClass[MAX_FLUSH]) {
        return true;
    }

    //3条或2对，葫芦以下都是
    if ((publicStruct.pokerClass == MAX_THREE_OF_A_KIND || publicStruct.pokerClass == MAX_TWO_PAIR) &&
        Rank_PokerClass[madehandStruct.pokerClass] >= Rank_PokerClass[MAX_PAIR] && Rank_PokerClass[madehandStruct.pokerClass] < Rank_PokerClass[MAX_FULL_HOUSE]) {
        return true;
    }

    //（两张成顺的，按原顺序）
    return false;
}


//成手牌打分
RankGroup CPokerHand::getPokerEvaluate_made(const  MadehandStruct& madehandStruct, const DrawStruct& drawStruct, const PublicStruct& publicStruct, bool blRiver)
{
    RankGroup rankGroup;

    //A0: ev_nuts
        //ev_nuts - evsub_large
        //
        //2_nuts同花顺
    if (madehandStruct.pokerClass == MAX_STRAIGHT_FLUSH && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_large);
        goto CHECK_RIVER;
    }
    //1_nuts同花顺
    if (madehandStruct.pokerClass == MAX_STRAIGHT_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_large);
        goto CHECK_RIVER;
    }
    //2_nuts炸弹
    if (madehandStruct.pokerClass == MAX_FOUR_OF_A_KIND && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_large);
        goto CHECK_RIVER;
    }
    //1_nuts炸弹
    if (madehandStruct.pokerClass == MAX_FOUR_OF_A_KIND && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_large);
        goto CHECK_RIVER;
    }
    //2_nuts葫芦
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_large);
        goto CHECK_RIVER;
    }

    //A1: ev_nuts - evsub_middle
    // 
    //2_小同花顺
    if (madehandStruct.pokerClass == MAX_STRAIGHT_FLUSH && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 1) {
        setRankGroup(rankGroup, ev_nuts, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_大葫芦
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_中葫芦
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_middle);
        goto CHECK_RIVER;
    }

    //A2: ev_nuts - evsub_small
    // 
    //n_nuts花(无一张成同花顺)
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum >= 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_small);
        goto CHECK_RIVER;
    }
    //2_nuts顺
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_small);
        goto CHECK_RIVER;
    }
    //2_顶set
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 == 0 && publicStruct.nNeedtoStraight == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_small);
        goto CHECK_RIVER;
    }

    //B0; ev_sec_nuts
    //ev_sec_nuts - evsub_large
    // 
    //1_nuts公牌炸弹_高张A(控抽水)
    if (madehandStruct.pokerClass == MAX_FOUR_OF_A_KIND && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 1) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        goto CHECK_RIVER;
    }
    //1_nuts顺(控抽水)
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        goto CHECK_RIVER;
    }

    //B1: ev_sec_nuts - evsub_middle
    // 
    //2_大公牌3条葫芦 
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 2) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_超成手对3条葫芦
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 1) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_小葫芦
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 3 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_中顺
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_中set(包括顶set)
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank <= 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_middle);
        goto CHECK_RIVER;
    }

    //B2: ev_sec_nuts - evsub_small
    // 
    //2_大花(rank <=2)
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank <= 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_small);
        goto CHECK_RIVER;
    }
    //2_小set
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 2 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_small);
        goto CHECK_RIVER;
    }
    //2_大2对(中2对)
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank <= 1 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight == 0 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_small);
        goto CHECK_RIVER;
    }

    //C0: ev_large
    //ev_large - evsub_large
    // 
    //1_大葫芦
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        goto CHECK_RIVER;
    }
    //1_大踢3条葫芦
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 1) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        goto CHECK_RIVER;
    }
    //1_大顺
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 1 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        goto CHECK_RIVER;
    }
    //2_nuts花_有公对
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        goto CHECK_RIVER;
    }

    //C1: ev_large - evsub_middle
    // 
    //1_小同花顺
    if (madehandStruct.pokerClass == MAX_STRAIGHT_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1) {
        setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_小花(rank >2)
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank > 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_小顺
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_顺_有公对
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //1_小同花顺
    if (madehandStruct.pokerClass == MAX_STRAIGHT_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1) {
        setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_set_2张成顺
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 2 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight == 2) {
        setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }

    //C2: ev_large - evsub_small
    // 
    //1_3条_小踢（1_3条_大踢）
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_large, evsub_small);
        goto CHECK_RIVER;
    }

    //2_小2对
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight == 0 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_small);
        goto CHECK_RIVER;
    }

    //大超对 C(A-Q)
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank <= 2 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_large, evsub_small);
        goto CHECK_RIVER;
    }

    //D0: ev_sec_large
    //ev_sec_large - evsub_large
    // 
    //1_nuts花_有公对（turn C1,river D0）
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 1) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_large);
        else
            setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_2对_2张成顺（turn C1,river D0）
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight == 2 && publicStruct.nPair == 0) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_large);
        else
            setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //中超对 D
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank <= 5 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_large);
        goto CHECK_RIVER;
    }
    //2_花_有公对（turn C2,river D0）
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 1) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_large);
        else
            setRankGroup(rankGroup, ev_large, evsub_small);
        goto CHECK_RIVER;
    }
    //1_3条_2张成顺（turn C1,river D0）
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
    //2_大2对_(中2对)_有公对
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank <= 1 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1 && publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_顺_2张成花
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush == 2) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //1, 2_nuts花_一张成同花顺
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nRank == 0 && madehandStruct.nAppend == 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //1_大花(<=2)
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //1_小葫芦
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank > 1 && madehandStruct.nAppend == 0) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_2对_2带公对
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1 &&
        publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //超对_带公对
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //顶对顶T D
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend >= 10 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //1_顺_有公对
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank != 2 && madehandStruct.nAppend == 0 &&
        (publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoFlush != 1 || publicStruct.nPair == 1)) {
        setRankGroup(rankGroup, ev_sec_large, evsub_middle);
        goto CHECK_RIVER;
    }

    //D2: ev_sec_large - evsub_small
    // 
    //2_set_1张成顺（turn C2,river D1）
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND &&
        (publicStruct.nNeedtoFlush == 1)) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_small);
        else
            setRankGroup(rankGroup, ev_large, evsub_small);
        goto CHECK_RIVER;
    }
    //2_set_1张成花（turn C2,river D1）
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND &&
        (publicStruct.nNeedtoStraight == 1)) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_small);
        else
            setRankGroup(rankGroup, ev_large, evsub_small);
        goto CHECK_RIVER;
    }
    //A对大踢(>=Q)
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 1 && madehandStruct.nAppend == 14 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_small);
        goto CHECK_RIVER;
    }
    //2_2对_2张成花
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush == 2 && publicStruct.nNeedtoStraight != 1 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_large, evsub_small);
        goto CHECK_RIVER;
    }
    //2_set_2张成花（turn C1,river D1）
    if (madehandStruct.pokerClass == MAX_THREE_OF_A_KIND && madehandStruct.nNeedHandsNum == 2 &&
        publicStruct.nNeedtoFlush == 2 && publicStruct.nNeedtoStraight != 1) {
        if (blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_small);
        else
            setRankGroup(rankGroup, ev_large, evsub_middle);
        goto CHECK_RIVER;
    }
    //1_3条_2张成花（turn C1,river D1）
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
    //1_大花_有公对
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        goto CHECK_RIVER;
    }
    //1_顺_2张成花
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank != 2 && madehandStruct.nAppend == 0 &&
        (publicStruct.nNeedtoFlush == 2)) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        goto CHECK_RIVER;
    }
    //2_成手中对3条葫芦(1_次对3条葫芦)
    if (madehandStruct.pokerClass == MAX_FULL_HOUSE && madehandStruct.nNeedHandsNum > 0 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 1) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        goto CHECK_RIVER;
    }

    //E1: ev_middle - evsub_middle
    // 
    //顶对中T E
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 4 && madehandStruct.nAppend >= 9 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_middle, evsub_middle);
        goto CHECK_RIVER;
    }
    //小超对 E
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank > 5 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_middle, evsub_middle);
        goto CHECK_RIVER;
    }

    //E2: ev_middle - evsub_small
    // 
    //顶对小T E
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank > 4 && madehandStruct.nAppend >= 9 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_middle, evsub_small);
        goto CHECK_RIVER;
    }
    //顶对_有公对(顶对顶踢river前为D2)
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 1 && madehandStruct.nAppend == 2 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_middle, evsub_small);
        if(madehandStruct.nRank == 0 && !blRiver)
            setRankGroup(rankGroup, ev_sec_large, evsub_small);
        goto CHECK_RIVER;
    }
    //小顶对 E
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nAppend < 9 && madehandStruct.nAppend > 4 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_middle, evsub_small);
        goto CHECK_RIVER;
    }

    //F0: ev_small
    //ev_small - evsub_large
    // 
    //1_大花_单张同花顺
    if (madehandStruct.pokerClass == MAX_FLUSH && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank <= 2 && madehandStruct.nAppend == 1) {
        setRankGroup(rankGroup, ev_small, evsub_large);
        goto CHECK_RIVER;
    }
    //超对_2张成花
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush == 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_large);
        goto CHECK_RIVER;
    }
    //顶对_2张成花
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 &&
        publicStruct.nNeedtoFlush == 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_large);
        goto CHECK_RIVER;
    }

    //F1: ev_small - evsub_middle
    // 
    //1_小顺
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_small, evsub_middle);
        goto CHECK_RIVER;
    }
    //2_顺_有3条或2对 ?
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        (publicStruct.pokerClass == MAX_THREE_OF_A_KIND || publicStruct.nPair == 2) && publicStruct.nNeedtoFlush != 1) {
        setRankGroup(rankGroup, ev_small, evsub_middle);
        goto CHECK_RIVER;
    }
    //超对_2张成花_带公对
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush == 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_middle);
        goto CHECK_RIVER;
    }
    //顶对_2张成花_带公对
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 2 &&
        publicStruct.nNeedtoFlush == 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_middle);
        goto CHECK_RIVER;
    }
    //成手次对
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_middle);
        goto CHECK_RIVER;
    }
    //次对
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 0 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_middle);
        goto CHECK_RIVER;
    }

    //F2: ev_small - evsub_small
    // 
    //2_2对_1张成花(2_2对_1张成顺)
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nAppend == 0 &&
        (publicStruct.nNeedtoFlush == 1 || publicStruct.nNeedtoStraight == 1)) {
        setRankGroup(rankGroup, ev_small, evsub_small);
        goto CHECK_RIVER;
    }
    //成手次对_带公对
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_small);
        goto CHECK_RIVER;
    }
    //次对_带公对
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 2 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_small, evsub_small);
        goto CHECK_RIVER;
    }
    //2_小2对_有公对
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1 && publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_small, evsub_small);
        goto CHECK_RIVER;
    }

    //G：ev_catch_bulff
        //ev_catch_bulff - evsub_small
        //
        //成手次次对
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_catch_bulff, evsub_small);
        goto CHECK_RIVER;
    }
    //次次对
    if (madehandStruct.pokerClass == MAX_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 1 && madehandStruct.nAppend == 0 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_catch_bulff, evsub_small);
        goto CHECK_RIVER;
    }
    //成手次次对_带公对
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 2 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 1 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_catch_bulff, evsub_small);
        goto CHECK_RIVER;
    }
    //次次对_带公对
    if (madehandStruct.pokerClass == MAX_TWO_PAIR && madehandStruct.nNeedHandsNum == 1 && madehandStruct.nRank == 2 && madehandStruct.nAppend == 2 &&
        publicStruct.nNeedtoFlush != 1 && publicStruct.nNeedtoFlush != 2 && publicStruct.nNeedtoStraight != 1) {
        setRankGroup(rankGroup, ev_catch_bulff, evsub_small);
        goto CHECK_RIVER;
    }


    CHECK_RIVER:
    //river需要把一些排列重置为抓咋牌
    if (blRiver) {
        if (isRankCatchBluff(madehandStruct, drawStruct, publicStruct)) {
            int evsub = evsub_small; //只简单分大和小
            if (Rank_PokerClass[madehandStruct.pokerClass] > Rank_PokerClass[MAX_PAIR]) evsub = evsub_large;
                setRankGroup(rankGroup, ev_catch_bulff, evsub);
        }

        //river把公牌同花顺、顺 nuts的设为ev_sec_nuts，evsub_large，表示不主动下注，但不fold； river把公牌同花顺、顺、葫芦（对大于3条点数的?）非nuts的设为抓咋牌
        if (publicStruct.nAppend == 1)
            setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        else if (publicStruct.nAppend == 2)
            setRankGroup(rankGroup, ev_catch_bulff, evsub_small);
    }

    return rankGroup;
}

//对draw牌力的打分(只有非river才会调用)
//typedef enum { ev_nuts, ev_sec_nuts, ev_large, ev_sec_large, ev_middle, ev_small, ev_catch_bulff }EvaluateClass;
//typedef enum { evsub_large, evsub_middle, evsub_small }EvaluateClass_sub;
//typedef enum { MAX_HIGH_CARD, MAX_PAIR, MAX_TWO_PAIR, MAX_THREE_OF_A_KIND, MAX_STRAIGHT, MAX_FLUSH, MAX_FULL_HOUSE, MAX_FOUR_OF_A_KIND, MAX_STRAIGHT_FLUSH, DRAW_STRAIGHT, DRAW_FLUSH, DRAW_OTHERS, DRAW_NONE }PokerClass;
RankGroup CPokerHand::getPokerEvaluate_draw(const  MadehandStruct& madehandStruct, const DrawStruct& drawStruct, const PublicStruct& publicStruct)
{
    RankGroup rankGroup;

    //ev_nuts
        //中顺draw nuts花（无公对)
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 &&
        drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 && drawStruct.nRank == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_nuts, evsub_large);
        return rankGroup;
    }

    //ev_sec_nuts
        //中顺draw花
    if (madehandStruct.pokerClass == MAX_STRAIGHT && madehandStruct.nNeedHandsNum == 2 &&
        drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        return rankGroup;
    }
    //2d_nuts花
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 && drawStruct.nRank == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        return rankGroup;
    }
    //2d_花，outs >= 15
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 && drawStruct.nOutsNum >= 15 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        return rankGroup;
    }
    //2d_顺 outs>=8 a_outs>=5
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 2 && drawStruct.nOutsNum >= 8 && drawStruct.nAppendOutsNum >= 5 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        return rankGroup;
    }

    //ev_large
        //1d_nuts花
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 1 && drawStruct.nRank == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        return rankGroup;
    }
    //2d_大花
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 && drawStruct.nRank <= 2 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        return rankGroup;
    }
    //2d_花 a_outs >= 5
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 && drawStruct.nAppendOutsNum >= 5 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_large, evsub_large);
        return rankGroup;
    }
    //2d_顺 outs >= 8
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 2 && drawStruct.nOutsNum >= 8 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_nuts, evsub_large);
        return rankGroup;
    }

    //ev_sec_large
        //2d_花（允许带公对）
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 2 &&
        (publicStruct.nPair == 0 || publicStruct.nPair == 1)) {
        setRankGroup(rankGroup, ev_sec_large, evsub_large);
        return rankGroup;
    }
    //1d nuts花（带公对）
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 1 && drawStruct.nRank == 0 &&
        publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_sec_large, evsub_large);
        return rankGroup;
    }
    //2d_顺 outs >= 6 无公对（大中）
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 2 && drawStruct.nOutsNum >= 6 && drawStruct.nRank <= 1 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_large, evsub_large);
        return rankGroup;
    }
    //1d_大顺 outs >= 6(无公对)
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 1 && drawStruct.nOutsNum >= 6 && drawStruct.nRank == 0 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_sec_large, evsub_large);
        return rankGroup;
    }

    //ev_middle
        //1d_大花（允许有公对）
    if (drawStruct.pokerClass == DRAW_FLUSH && drawStruct.nNeedHandsNum == 1 && drawStruct.nRank <= 2 &&
        (publicStruct.nPair == 0 || publicStruct.nPair == 1)) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        return rankGroup;
    }
    //2d_顺 outs >= 6（有公对）
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 2 && drawStruct.nOutsNum >= 6 &&
        publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        return rankGroup;
    }
    //2d_顺 outs >= 6 无公对（小）
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 2 && drawStruct.nOutsNum >= 6 && drawStruct.nRank == 2 &&
        publicStruct.nPair == 0) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        return rankGroup;
    }
    //1d_大顺 outs >= 6(有公对)
    if (drawStruct.pokerClass == DRAW_STRAIGHT && drawStruct.nNeedHandsNum == 1 && drawStruct.nOutsNum >= 6 && drawStruct.nRank <= 1 &&
        publicStruct.nPair == 1) {
        setRankGroup(rankGroup, ev_middle, evsub_large);
        return rankGroup;
    }

    //ev_small
        //2d_其他 outs >= 7(?6)
    if (drawStruct.pokerClass == DRAW_OTHERS && drawStruct.nNeedHandsNum == 2 && drawStruct.nAppendOutsNum >= 6) {
        setRankGroup(rankGroup, ev_small, evsub_large);
        return rankGroup;
    }
    //2d(1d)_顺(大中) outs = 4 a_outs >= 2
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
