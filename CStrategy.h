#pragma once

#ifndef CSTRATEGY_H_
#define CSTRATEGY_H_

#include "globledef.h"
#include "json/json.h"
#include "CGame.h"

class CStrategyItem
{
public:
	Action m_action;
	StrategyData m_strategyData;
	EVData m_evData;
};

class CCommForSpecialProcessing
{
public:
	std::string m_sCommand;
	std::vector<std::string> m_sActions1;
	std::vector<std::string> m_sActions2;
	std::vector<std::string> m_range;
	bool m_blRangeExclude;
	std::vector<std::string> m_conditions;
};


struct Condition {
	std::string sObj; //目前只有EV,""代表无条件
	bool	blLess;
	bool	blEqueal;
	double	dlValue;
};


// exploi

typedef struct ExploiEVCondition_tag{
	Action action;
	double dEvLowBound;
	double dEvUpBound;
	double dAdjustRatio;
}ExploiEVCondition;




class CStrategy
{
public:
	std::vector<std::shared_ptr<CStrategyItem>> m_strategy;

	bool Load(GameType gmType, const std::string& sActionSquence); //从自定义策略文件读取
	bool Load(GameType gmType, const std::string & sActionSquence, const Stacks& stacks, const OOPX oopx, const SuitReplace & suitReplace, const std::string & sIsoBoard); //从wizard读取
	bool Load(const Json::Value & root, const std::string & sActionSquence, const Stacks& stacks, const Stacks& stacksByStrategy, const SuitReplace & suitReplace, const bool converToAllin = false); //从solver读取

	void AlignmentByBetsize();
	void AlignmentByStackDepth();
	void AlignmentByexploit(const RangeData& heroCurRange, const std::string& sBoard, const std::string& sActionSquenceRatio, const Position posHero, const Position posRival, const GameType gmType);

	void SpecialProcessing(const GameType gmType, const std::string& sCommand); //按spcial配置处理m_strategy
	void SpecialProcessing(const std::string& sCommand);

	static int MatchBetRatio(double dActuallyRatio, const std::vector<double>&candidateRatios); 
	static bool parseActionSquence(const std::string& sActionSquence, std::string& sPrefix, Round &round,std::vector<Action>& actions,std::string &actionStr);
	static const Json::Value *geActionNode(const Json::Value *node,const Action& action,const std::vector<Action>& actions, std::vector<Action>& actionsByStrategy, const Stacks& stacks, const Stacks& stacksByStrategy);//double stack);//const StackByStrategy& stack,bool last);
	static Action getActionByStr(const std::string &str); 
	static double CalcBetRatio(const double dPot, const std::vector<Action>& actions, int iLastIdx, const double dEstack = 0); //只有allin才填写dEstack

	//多人对抗函数(目前仅供CActionLine调用)
	static std::vector<std::pair<Position, Action>> parseMultiActionSquence(const std::string& sOriActionInfo);
	static double CalcMultiBetRatio(const double dPot, const std::vector<std::pair<Position, Action>>& posActions); //pot为换街初始pot
	static double CalcBetSize(const double dPot, const std::vector<std::pair<Position, Action>>& posActions, const Position heroPosition, const double dSizeByPot); //2人，多人调用同一个,最后一个为对手行动

	void LoadMacro(std::string path);

	static std::string ConvertOneHandcard(std::string& sCard, const SuitReplace& suitReplace);

	void DumpStrategy(const std::string& sComment,  const std::vector<std::shared_ptr<CStrategyItem>>* pStrategy = NULL);
	bool ReadStrategy(const std::string& sPath,  std::vector<std::shared_ptr<CStrategyItem>>& strategy);

	//for simple solve
	bool GetAbnormalSizes(const double dPot, const std::string& sActionSquence, std::vector<double>& raiseSizes);
	const std::vector<std::shared_ptr<CStrategyItem>> GetStrategy() const;

	//for exploi 辅助函数
	static std::string getCommandTypeExploi(const std::string& sCommand);
	static std::string parasParamExploi(const std::string& sParam, std::string& sParamDetail); //返回参数类型，改写具体参数
	static bool matchActionExploi(const Action& srcAction, const Action& desAction);

	//for AlignmentByBetsize
	static std::vector<std::shared_ptr<CStrategyItem>> getStrategyByNode(const Json::Value* node, const Stacks& stacksByStrategy, const std::vector<Action>& actionsByStrategy);
	static std::vector<std::shared_ptr<CStrategyItem>> getAverageStrategyByBetsize(const std::vector<std::shared_ptr<CStrategyItem>>& strategyLow, const std::vector<std::shared_ptr<CStrategyItem>>& strategyUp, const double dSegmentRatio, const int nTemplateBy); //dSegmentRatio为low_bound到基准的占比
	static std::pair<const Json::Value*, const Json::Value*> getActionNodeTwoBound(const Json::Value* node, const Action& action, const std::vector<Action>& actions, std::vector<Action>& actionsByStrategyLow, std::vector<Action>& actionsByStrategyUp, const Stacks& stacks, const Stacks& stacksByStrategy, double& dSegmentRatio, int& templateBy); //如果只有一个有效则放first
	static std::pair<int, int> MatchBetRatioTwoBound(const double dActuallyRatio, const std::vector<double>& candidateRatios, double& dSegmentRatio); //-1代表不匹配
	static double getSegmentRatio(const double lowBound, const double base, const double UpBound);

	//for AlignmentByStackDepth
	static std::vector<std::shared_ptr<CStrategyItem>> getAverageStrategyByStackDepth(const std::vector<std::shared_ptr<CStrategyItem>>& strategyLow, const std::vector<std::shared_ptr<CStrategyItem>>& strategyUp, const double dSegmentRatio, const int nTemplate, const bool blIgnoreBetsize = false); //blIgnoreBetsize 为true用于preflop,只比对actionType


private:

	void ConvertIsomorphism(const SuitReplace & suitReplace); //将m_strategy进行同构转换

	std::vector<CCommForSpecialProcessing> GetCommands(const std::string& sCommands);
	void DiscardOne(std::shared_ptr<CStrategyItem>& strategyItem, std::shared_ptr<CStrategyItem>& foldItem, const std::unordered_map<std::string, bool>& rangeMap, const Condition& condition);
	void ReplaceOne(std::shared_ptr<CStrategyItem>& srcStrategy, std::shared_ptr<CStrategyItem>& desStrategy, std::shared_ptr<CStrategyItem>& allinStrategy, const std::unordered_map<std::string, bool>& rangeMap, const Condition& condition, const bool force);

	void Assign(const std::string& action, const std::unordered_map<std::string, bool>& rangeMap);//std::vector<std::string> &range);
	void Discard(const std::string& action, const std::unordered_map<std::string, bool>& rangeMap, const std::vector<Condition>& conditions);
	void Replace(const std::string& action1, const std::string& action2, const std::unordered_map<std::string, bool>& rangeMap, const std::vector<Condition>& conditions, const bool force = false);

	void ConvertIsomorphismOneMap(std::unordered_map<std::string, double>* pMap, const SuitReplace& suitReplace);

	//for exploi
	void ExecuteCommandExploi(const std::string& sCommand, const RangeData& heroCurRange, const std::string& sBoard);
	void AdjustFrequencyExploi(const RangeData& heroCurRange, const std::string& sBoard, const Action& actionToMatch, const Action& actionToReplace, const OP_obj opObj, const RankGroup& rgLowestValue, const adjust_methord adjustMechord, const double adjustRatio, const ratio_type ratioType, double& actuallyRatio); //adjustFrequency, adjustRatio统一为符合调整条件部分的比例
	void AdjustFrequencyExploi(const RangeData& heroCurRange, const std::string& sBoard, const Action& actionToMatch, const Action& actionToReplace, const OP_obj opObj, const RankGroup& rgLowestValue, const double adjustRatio, double& actuallyRatio); //adjustStrength, opObj为加强部分，ratioType总是byAction, adjustMethord总是avarage
	void AdjustFrequencyExploi(const RangeData& heroCurRange, const std::vector<ExploiEVCondition>& evConditions, double& actuallyRatio); //adjustFrequencyByEV, 只用于flop,符合EV区间的和fold互转
	void ParaseCommandExploi(const std::string& sCommand, Action& actionToMatch, Action& actionToReplace, OP_obj& opObj, RankGroup& rgLowestValue, adjust_methord& adjustMethord, double& adjustRatio, ratio_type& ratioType); //AdjustFrequency
	void ParaseCommandExploi(const std::string& sCommand, Action& actionToMatch, Action& actionToReplace, OP_obj& opObj, RankGroup& rgLowestValue, double& adjustRatio); //AdjustStrength
	void ParaseCommandExploi(const std::string& sCommand, std::vector<ExploiEVCondition>& evConditions); //AdjustFrequencyByEV


};

#endif


/*
//AdjustFrequencyExploi
// -------------------------------
//const RangeData& heroCurRange, 
//const string& sBoard, 
const Action& actionToMatch,
const Action& actionToReplace,
const OP_obj opObj, //op_value, op_bluff, op_all
const RankGroup& rgLowestValue, //{main,sub}
const adjust_methord adjustMechord, //adjust_average, adjust_ascend, adjust_descend
const double adjustRatio,
const ratio_type ratioType, //by_whole, by_action, by_valid
//double& actuallyRatio


//AdjustStrengthExploi
// -------------------------------
//const RangeData& heroCurRange, 
//onst string& sBoard, 
const Action& actionToMatch,
const Action& actionToReplace,
const OP_obj opObj,
const RankGroup& rgLowestValue,
const double adjustRatio,
//double& actuallyRatio

//AdjustFrequencyExploi
//------------------------------------
//const RangeData& heroCurRange, 
const vector<ExploiEVCondition>& evConditions, //{ action, dEvLowBound, dEvUpBound, dAdjustRatio}
//double& actuallyRatio)
*/