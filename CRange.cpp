//#include "pch.h"
#include "CRange.h"
#include "CStrategy.h"
#include "CDataFrom.h"
#include "util.h"
using namespace std;
#include <regex>
#include "CRangeNodeConfig.h"
#include "CRangeNodeConfigItem.h"
#include "CStrategyTreeConfig.h"

//fileģʽ
bool CRange::Load(GameType gmType, const string& sNodeName, const string& sBoardNext)
{
	//��ȡ�ڵ����ƶ�Ӧ���ļ�·����δ�ҵ��򷵻�false,����offline
	string sPath = CDataFrom::GetRangesFilePath(gmType, sNodeName);
	if (sPath.size() == 0)
		return false;

	//�ӷ�Χ�ļ��������ݣ�OOP��IP��Ӧ

	//��Χ��ȥ���µĹ��ƶ�Ӧ�ı���(��Ϊ0)��sBoardNext�п��������Ż�һ�ţ�����ÿ�Ŷ�Ӧ�Ķ���ȥ��
	RemoveComboByBoard(m_IPRange, sBoardNext);
	RemoveComboByBoard(m_OOPRange, sBoardNext);

	return true;
}

//wizardģʽ
bool CRange::Load(GameType gmType, const string& sActionSquence, const StackByStrategy& stack, const string& sBoardNext, const SuitReplace& suitReplace, const string& sIsoBoard)
{
/*
	Round curRound;
	string sPrefix, actionStr, sNodeName;
	vector<Action> actions;
	string sStrategyFilePath;
	string sActionSquenceTmp;
	RangeData rangeRatio;
	string sLastPlayerPosition;
	RangeData* pRangeRatio = nullptr;

	//��ȡNodeName(ע�ͣ�
	//ȥ��sActionSquence�����һ������(C��X)
	auto idx = sActionSquence.find_last_of('-');
	if (idx != string::npos)
		sActionSquenceTmp = sActionSquence.substr(0, idx);

	parseActionSquence(sActionSquenceTmp, sPrefix, curRound, actions, actionStr);
	if (curRound == preflop)
		sNodeName = sPrefix;
	else if (curRound == flop)
		sNodeName = getNodeName(gmType, stack, actions, sPrefix);

	//flop��Ҫ����ͬ��������ͬ���滻���滻�ڵ������е�board����
	if (curRound == flop) {
		if (suitReplace.blIsNeeded) {
			regex reg("<.*>");
			string sReplace = "<" + sIsoBoard + ">";
			sNodeName = regex_replace(sNodeName, reg, sReplace);
		}
	}

	//�Ӳ��������л�ȡ�滻��special���ã������滻�������滻�����ƣ�(flop��ͨ��ƥ�䷨����)
	auto RangeNodeConfig = g_rangeNodeConfigs[gmType];
	auto pRangeNodeConfigItem = RangeNodeConfig.GetItemByName(sNodeName);
	if (pRangeNodeConfigItem != nullptr) {
		if (!pRangeNodeConfigItem->m_sReplaceNodeName.empty())
			sNodeName = pRangeNodeConfigItem->m_sReplaceNodeName;
	}

	//��ȡ�ڵ����ƶ�Ӧ���ļ�·����δ�ҵ��򷵻�false,����offline

	sStrategyFilePath = g_dataFrom.GetWizardFilePath(gmType, sNodeName, curRound);
	if(sStrategyFilePath.size() == 0)
		return false;
	*/

	//ȡC��X��Ӧ���ԣ�������ж��ߵ�λ��(ע��)
	//clear(); //�����ԭʼrange������Ҫ��ԭʼrange�����㣩
	//solution���ҳ�action.code = "X"��"C"��[n]��rangeRatio = solution.[n].strategy��sLastPlayerPosition = solution.[n].action.position
	// 
	//����ԭʼ��Χ(ע��)
	//���player_info[0].player.relative_postflop_position == "OOP"
		//m_OOPRange = player_info[0].player.range ��m_IPRange = player_info[1].player.range
	 //else m_OOPRange = player_info[1].player.range; m_IPRange = player_info[0].player.range

	//�ҳ�sLastPlayerPosition��Ӧ�����λ�ã�ȷ����Ҫ��Χ�������OOP����IP(ע��)
	//string sRelativePos;
	//���player_info[0].player.position == sLastPlayerPosition
		//sRelativePos = player_info[0].player.relative_postflop_position
	//else 
		//sRelativePos = player_info[1].player.relative_postflop_position
	// sRelativePos == "OOP" ? pRangeRatio = &m_OOPRange : pRangeRatio = &m_IPRange;

	//rangeRatio��pRangeRatioָ��ķ�Χ���㣬���·�Χ

	//ͬ��ת��
	if (suitReplace.blIsNeeded) {
		ConvertIsomorphism(m_OOPRange, suitReplace);
		ConvertIsomorphism(m_IPRange, suitReplace);
	}

	//��Χ��ȥ���µĹ��ƶ�Ӧ�ı���(��Ϊ0)��sBoardNext�п��������Ż�һ�ţ�����ÿ�Ŷ�Ӧ�Ķ���ȥ��
	RemoveComboByBoard(m_IPRange, sBoardNext);
	RemoveComboByBoard(m_OOPRange, sBoardNext);

	return true;
}

//solverģʽ
bool CRange::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const StackByStrategy& stack, const string& sBoardNext, const SuitReplace& suitReplace)
{
	RangeData OOPRangeRatio; //���ڼ���rangeʣ�������OOP�����һ���ж������
	RangeData IPRangeRatio; //���ڼ���rangeʣ�������IP����ڶ����ж������
	RangeData* pRangeRatio = nullptr;


	//����ActionSquence,ȡ���һ��<>�������sCSquence
	vector<Action> actionSquence={};
	string sPrefix=""; 
	Round round;
	string actionStr="";
	if(!CStrategy::parseActionSquence(sActionSquence,sPrefix,round,actionSquence,actionStr)) {
		return false;
	}
	if(actionSquence.size()==0) {
		return false;
	}

	//��actionSquence��ÿһ��������ֻ��X��R��C������Xֻ������ڵ�һ�������һ��һ����C��X��û��A��O��
		//ȷ���ǲ���OOP����IP��������OOP��ż����IP����ֵpRangeRatio��
		//��ǰ�ڵ㣨��һ��Ϊroot����ѡ���Ӧ�Ķ������Ͳ����з���һ������R����Ҫƥ��A
		//�Բ���������ÿ����ϣ���ѡ��������Ӧ�ı���ȥ��дpRangeRatio�ж�Ӧ�����ݣ�����ǵ�һ���ж�����д1*�������ǵ�һ���ж�����д*pRangeRatio[���] * ���������ֵ�һ��Ŀ�����ò����ڵ���ϱ���0��
		//�������һ��������֮ǰƥ��Ķ���ѡ���ӽڵ㣬Ϊ��ǰ�ڵ�

	const Json::Value *node = &root;
	for(auto i = 0;i<int(actionSquence.size());i++){
		if(i%2==0) {
			pRangeRatio = &OOPRangeRatio;
		} else {
			pRangeRatio = &IPRangeRatio;		
		}

		auto a = actionSquence[i];
		auto getActionIdx = [] (const Json::Value *node,const Action& action) -> int {
				int index = -1;

				auto members = (*node)["actions"].getMemberNames();
				if(action.actionType == check) {
					for(int i = 0;i<int(members.size());i++){
						if(members[i] == "CHECK") {
							index = i;
							break;
						}	
					}
				} else if(action.actionType == call) {
					for(int i = 0;i<int(members.size());i++){
						if(members[i] == "CALL") {
							index = i;
							break;
						}	
					}
				} else if (action.actionType == raise) {
					vector<string> names;
					vector<double>  bets;
					for(auto it2 = members.begin();it2 != members.end();++it2){
						if((*it2).find("BET") != string::npos || (*it2).find("RAISE") != string::npos) {
							names.push_back(*it2);
							bets.push_back(getBetByStr(*it2));
						}
					}

					auto j = CStrategy::MatchBetSize(action.fBetSize,bets,0);
					if(j>=0){
						for(int i = 0;i<int(members.size());i++){
							if(names[j]==members[i]){
								index=i;
								break;
							}
						}
					}
				}

				return index;
			};			
		

		int j = getActionIdx(node,a);

		if(j < 0) {
			return false;
		}

		const Json::Value &nodeStrategy = (*node)["strategy"];
		auto members = nodeStrategy["strategy"].getMemberNames();
		for(auto it = members.begin();it != members.end();++it){
			auto name = *it;
			auto value = nodeStrategy["strategy"][name][j].asFloat();

			auto mapIt = pRangeRatio->find(name);
			if(mapIt == pRangeRatio->end()) {
				//��һ��1*value
				(*pRangeRatio)[name] = value;
			} else {
				mapIt->second *= value;
			}
		}

		//���ݶ���ѡ����һ�ڵ�
		node = CStrategy::geActionNode(node,a,0);
		if(node==nullptr){
			return false;
		}
	}


	//ͬ��ת��
	if (suitReplace.blIsNeeded) {
		ConvertIsomorphism(OOPRangeRatio, suitReplace);
		ConvertIsomorphism(IPRangeRatio, suitReplace);
	}

	//��ԭʼ��Χ����,����ԭʼ��Χ��ÿ�����ԭʼ��Χ���� *RangeRatio��Ӧ�ı���, �ܷ�һ��transform������multiplies��

	//��Χ��ȥ���µĹ��ƶ�Ӧ�ı���(��Ϊ0)��sBoardNext�п��������Ż�һ�ţ�����ÿ�Ŷ�Ӧ�Ķ���ȥ��
	RemoveComboByBoard(m_IPRange, sBoardNext);
	RemoveComboByBoard(m_OOPRange, sBoardNext);

	return true;
}

//תΪsolver�����ļ���Ҫ�ĸ�ʽ
string CRange::GetRangeStr()
{
	//����fileģʽ��ĸ�ʽ
	string v;
	return v;
}

//���˫����Χ����Ϊ0
void CRange::Clear()
{

}
//ͬ��ת��
void CRange::ConvertIsomorphism(RangeData& rangeRatio, const SuitReplace& suitReplace)
{
	
}

//�ų�������ص����
void CRange::RemoveComboByBoard(RangeData& rangeRatio, const std::string& sBoardNext)
{

}
