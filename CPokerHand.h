#pragma once

#ifndef CPOKER_HAND_H_
#define CPOKER_HAND_H_

#include "globledef.h"
#include<algorithm>
//

class possibleClasses {
public:

    //card要排序
    MyCards m_straight_flush;
    MyCards m_flush;
    MyCards m_straight;
    MyCards m_king;
    MyCards m_fullhouse;
    MyCards m_three;
    MyCards m_pair;

    //形成顺花需要的张数，用于公牌评价
    int m_needtoFlush = 0; //1,2,3
    int m_needtoStraight = 0; //1,2

    possibleClasses() {};

    possibleClasses(const MyCards& myCards)
    {
        MyCards myCardsTmp = myCards;
        sort(myCardsTmp.begin(), myCardsTmp.end(), [](myCard v1, myCard v2) {return v1.m_point > v2.m_point; });

        std::map<Suit, MyCards> possibleflushs; //同花的集合
        for (auto onecard : myCardsTmp)
            possibleflushs[onecard.m_suit].push_back(onecard);

        //检查是否有同花顺
        for (auto one : possibleflushs) {
            MyCards straight_flush = toStraight(one.second);
            if (!straight_flush.empty()) {
                m_straight_flush = straight_flush; //同花顺只可能一种花色有,其他都不用填了
                return;
            }
        }

        std::map<int, MyCards> samepoints; //点数的集合
        for (auto onecard : myCardsTmp)
            samepoints[onecard.m_point].push_back(onecard);

        //检查4条
        for (auto one : samepoints) {
            if (one.second.size() == 4) {
                m_king = one.second;
                return;
            }
        }

        //3条和2条抽出并排序
        std::map<int, MyCards> threes;
        for (auto one : samepoints) {
            if (one.second.size() == 3)
                threes[one.first] = one.second;
        }
        std::map<int, MyCards> pairs;
        for (auto one : samepoints) {
            if (one.second.size() == 2) {
                pairs[one.first] = one.second;
            }
        }

        //检查fullhouse
        if (threes.size() == 2) {
            auto Big = threes.rbegin()->second;
            auto Small = threes.begin()->second;
            for (auto onecard : Big)
                m_fullhouse.push_back(onecard);
            for (int i = 0; i < 2; i++)
                m_fullhouse.push_back(Small[i]);
            return;
        }

        if (threes.size() == 1 && !pairs.empty()) {
            auto onethree = threes.begin()->second;
            for (auto onecard : onethree)
                m_fullhouse.push_back(onecard);

            auto largestpair = pairs.rbegin()->second;
            for (auto onepair : largestpair)
                m_fullhouse.push_back(onepair);
            return;
        }

        //检查同花
        //m_needtoFlush 为1-2，0代表无成花可能
        int nMaxSameSuit = 0;
        for (auto one : possibleflushs) {
            if (one.second.size() > nMaxSameSuit)
                nMaxSameSuit = (int)one.second.size();

            if (one.second.size() >= 5) {
                for (int i = 0; i < 5; i++)
                    m_flush.push_back(one.second[i]);
                return;
            }
        }
        int nLack = 5 - nMaxSameSuit;
        if (nLack >= 1 && nLack <= 2)
            m_needtoFlush = nLack;

        //检查顺子
        MyCards straight = toStraight(myCards);
        if (!straight.empty()) {
            m_straight = straight;
            return;
        }

        std::set<int> onlyPointsS;
        for (auto card : myCards) {
            onlyPointsS.insert(card.m_point);
            if (card.m_point == 14)
                onlyPointsS.insert(1);
        }
        //m_needtoStraight = 0代表无成顺可能，因此不能用<=来判断
        std::vector<int>onlyPointsV(onlyPointsS.begin(), onlyPointsS.end());
        for (int i = 3; i < onlyPointsV.size(); i++) {//先检查单张成顺的
            if (onlyPointsV[i] - onlyPointsV[i - 3] <= 4) {
                m_needtoStraight = 1;
                break;
            }
        }
        if (m_needtoStraight != 1) {
            for (int i = 2; i < onlyPointsV.size(); i++) {//先检查单张成顺的
                if (onlyPointsV[i] - onlyPointsV[i - 2] <= 4) {
                    m_needtoStraight = 2;
                    break;
                }
            }
        }

        //检查3条
        if (!threes.empty()) {
            auto onethree = threes.begin()->second;
            for (auto onecard : onethree)
                m_three.push_back(onecard);
            return;
        }

        //检查pair
        if (!pairs.empty()) {
            auto onepair = pairs.rbegin()->second;
            for (auto onecard : onepair)
                m_pair.push_back(onecard);

            auto p = ++pairs.rbegin();
            if (p != pairs.rend()) {
                for (auto onecard : p->second)
                    m_pair.push_back(onecard);
            }
        }

    }

    MyCards toStraight(const MyCards& myCards) {
        MyCards ret;
        if (myCards.size() < 5)
            return ret;

        std::set<myCard> pointS;
        std::vector<std::set<myCard>> pointSes;
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
                pointSes.push_back(std::set<myCard>(pBegin, ++pEnd));
            //else if (pEndStable->m_point == 14 && pEndbefore->m_point - pBegin->m_point == 3 && pBegin->m_point == 2) {
            else if (pEndStable->m_point == 14 && pEnd->m_point - pBegin->m_point == 3 && pBegin->m_point == 2) {
                std::set<myCard> tmp(pBegin, pEnd);
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

    PokerClass getPokerClass()
    {
        if (!m_straight_flush.empty())
            return MAX_STRAIGHT_FLUSH;

        if (!m_king.empty())
            return MAX_FOUR_OF_A_KIND;

        if (!m_fullhouse.empty())
            return MAX_FULL_HOUSE;

        if (!m_flush.empty())
            return MAX_FLUSH;

        if (!m_straight.empty())
            return MAX_STRAIGHT;

        if (!m_three.empty())
            return MAX_THREE_OF_A_KIND;

        if (!m_pair.empty()) {
            if (m_pair.size() == 4)
                return MAX_TWO_PAIR;
            else
                return MAX_PAIR;
        }

        return  MAX_HIGH_CARD;
    }
}; //end of class


class CPokerHand
{
private:
	bool issubset(const std::set<int>& s, const std::set<int>& father); 
    MyCards toStraight(const MyCards& myCards);
    std::vector<OnecardToStraight> getOnecardToStraight(const MyCards& cards);
    std::set<int> getMaxPossibleStraightByBoard(const MyCards& publics, std::vector<int>& maxStraight);
    DrawStruct getStraightDraw(const MyCards& privates, const MyCards& publics);
    std::set<int> getBackdoorStraightDraw(const MyCards& privates, const MyCards& publics);
    DrawStruct getFlushtDraw(const MyCards& privates, const MyCards& publics);
    int getBackdoorFlushDraw(const MyCards& privates, const MyCards& publics);
    bool isClosetoStraight(const std::set<int>& publicAdd1);
    bool isClosetoFlush(const MyCards& publics);
    bool getPairDraw(const MyCards& privates, const MyCards& publics);
    DrawStruct getHighCardDraw(const MyCards& privates, const MyCards& publics);
    PublicStruct  getPublicStruct(const MyCards& publics);
    DrawStruct getDrawStruct(const MyCards& privates, const MyCards& publics, const PublicStruct& publicStruct);
    MadehandStruct getMadehandStruct(const MyCards& privates, const MyCards& publics);
    void setEvaluate(PokerEvaluate& pokerEvaluate, const int nMainGroup, const int nSubGroup, const double score = 0);
    void setRankGroup(RankGroup& rankGroup, const int nMainGroup, const int nSubGroup, const double score = 0);
    bool isRankCatchBluff(const  MadehandStruct& madehandStruct, const DrawStruct& drawStruct, const PublicStruct& publicStruct);
    RankGroup getPokerEvaluate_made(const  MadehandStruct& madehandStruct, const DrawStruct& drawStruct, const PublicStruct& publicStruct, bool blRiver = false);
    RankGroup getPokerEvaluate_draw(const  MadehandStruct& madehandStruct, const DrawStruct& drawStruct, const PublicStruct& publicStruct);
 
    std::map<PokerClass, int> Rank_PokerClass{ {MAX_HIGH_CARD, 0}, { MAX_PAIR,1 }, { MAX_TWO_PAIR,2 }, { MAX_THREE_OF_A_KIND,3 },{ MAX_STRAIGHT,4 },{ MAX_FLUSH,5 },{ MAX_FULL_HOUSE,6 },{ MAX_FOUR_OF_A_KIND,7 },{ MAX_STRAIGHT_FLUSH,8 } };
public:
    PokerEvaluate getPokerEvaluate(const MyCards& privates, const MyCards& publics);

};



#endif
