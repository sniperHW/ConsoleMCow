//#include "pch.h"
#include "CRange.h"
#include "CStrategy.h"
#include "CDataFrom.h"
#include "util.h"
#include <regex>
#include "CRangeNodeConfig.h"
#include "CRangeNodeConfigItem.h"
#include "CStrategyTreeConfig.h"
#include "globledef.h"
#include <fstream>

using namespace std;

extern map<GameType, CRangeNodeConfig> g_rangeNodeConfigs;


//�����ļ����������зָ�����ݣ��������հ��У�
void loadFileAsLine(const string& path,vector<string> &lines) {
	std::ifstream ifs;
  	ifs.open(path);
	ifs.seekg (0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg (0, ifs.beg);
    char *buffer = new char[length];
	ifs.read (buffer,length);
	auto l = split(buffer,'\n');
	for(auto it = l.begin();it != l.end();it++) {
		if(*it != "") {
			lines.push_back(*it);
		}
	}
	delete [] buffer;
}


//fileģʽ
bool CRange::Load(GameType gmType, const string& sNodeName, const string& sBoardNext)
{
	//��ȡ�ڵ����ƶ�Ӧ���ļ�·����δ�ҵ��򷵻�false,����offline
	string sPath = CDataFrom::GetRangesFilePath(gmType, sNodeName);
	if (sPath.size() == 0)
		return false;

	//�ӷ�Χ�ļ��������ݣ�OOP��IP��Ӧ
	vector<string> lines;
	loadFileAsLine("./test/LP_#_vs_4cold3bet4bet.txt",lines);

	string *oopStr;
	string *ipStr;

	for(auto i = 0;i < int(lines.size());i++) {
		if(lines[i].find("OOP") != string::npos){
			oopStr = &lines[i+1];
		} else if(lines[i].find("IP") != string::npos){
			ipStr = &lines[i+1];
		}
	}

	if(oopStr == nullptr) {
		return false;
	}

	auto pairsOOP = split(*oopStr,',');
	for(auto it = pairsOOP.begin();it != pairsOOP.end();it++){
		auto v = split(*it,':');
		auto name = v[0];
		auto value = stringToNum<double>(v[1]);
		m_OOPRange[name] = value;
	}

	if(ipStr == nullptr) {
		return false;
	}

	auto pairsIP = split(*ipStr,',');
	for(auto it = pairsIP.begin();it != pairsIP.end();it++){
		auto v = split(*it,':');
		auto name = v[0];
		auto value = stringToNum<double>(v[1]);
		m_IPRange[name] = value;
	}


	//��Χ��ȥ���µĹ��ƶ�Ӧ�ı���(��Ϊ0)��sBoardNext�п��������Ż�һ�ţ�����ÿ�Ŷ�Ӧ�Ķ���ȥ��
	RemoveComboByBoard(m_IPRange, sBoardNext);
	RemoveComboByBoard(m_OOPRange, sBoardNext);

	return true;
}

//wizardģʽ(δ����)
bool CRange::Load(GameType gmType, const string& sActionSquence, const Stacks& stacks, const string& sBoardNext, const SuitReplace& suitReplace, const string& sIsoBoard)
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

	CStrategy::parseActionSquence(sActionSquenceTmp, sPrefix, curRound, actions, actionStr);
	if (curRound == preflop)
		sNodeName = sPrefix;
	else if (curRound == flop)
		sNodeName = CStrategy::getNodeName(gmType, stack, actions, sPrefix);

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
	//g_dataFromδ����
	/*
	sStrategyFilePath = g_dataFrom.GetWizardFilePath(gmType, sNodeName, curRound);
	if(sStrategyFilePath.size() == 0){
		return false;
	}*/
	
	sStrategyFilePath = "./test/wizard_smple.json"; //for test/////////////////////////////////////////////////

	Json::Value root;
	std::ifstream ifs;
	ifs.open(sStrategyFilePath);
	Json::CharReaderBuilder builder;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, ifs, &root, &errs)) {
		std::cout << errs << std::endl;
		return false;
	}

	if(actions.empty()) {
		return false;
	}

	auto lastAction = actions[actions.size()-1];
	if(!(lastAction.actionType == call || lastAction.actionType == check)){
		return false;
	}	

	//ȡC��X��Ӧ���ԣ�������ж��ߵ�λ��(ע��)
	Clear(); //�����ԭʼrange������Ҫ��ԭʼrange�����㣩
	//solution���ҳ�action.code = "X"��"C"��[n]��rangeRatio = solution.[n].strategy��sLastPlayerPosition = solution.[n].action.position
	auto solutions = root["solutions"];
	const Json::Value *nodeStrategy;
	for(auto it = solutions.begin();it != solutions.end();it++){
		if(lastAction.actionType == call && (*it)["action"]["code"] == "C") {
			sLastPlayerPosition = (*it)["action"]["position"].asString();
			nodeStrategy = &(*it)["strategy"];
			break;
		} else if(lastAction.actionType == check && (*it)["action"]["code"] == "X") {
			sLastPlayerPosition = (*it)["action"]["position"].asString();
			nodeStrategy = &(*it)["strategy"];
			break;
		}
	}

	if(sLastPlayerPosition=="") {
		return false;
	}

	for (Json::ArrayIndex i = 0; i < nodeStrategy->size(); i++) {
		auto name = ComboMapping[i];
		auto value = (*nodeStrategy)[i].asDouble();
		rangeRatio[name]=value;
	}

	//����ԭʼ��Χ(ע��)
	//���player_info[0].player.relative_postflop_position == "OOP"
		//m_OOPRange = player_info[0].player.range ��m_IPRange = player_info[1].player.range
	 //else m_OOPRange = player_info[1].player.range; m_IPRange = player_info[0].player.range

	const Json::Value *rangeOOP;
	const Json::Value *rangeIP;

	if(root["player_info"][0]["relative_postflop_position"] == "OOP") {
		rangeOOP = &root["player_info"][0]["range"];
		rangeIP  = &root["player_info"][1]["range"];
	} else {
		rangeOOP = &root["player_info"][1]["range"];
		rangeIP  = &root["player_info"][0]["range"];
	} 


	for (Json::ArrayIndex i = 0; i < rangeOOP->size(); i++) {
		auto name = ComboMapping[i];
		auto value = (*rangeOOP)[i].asDouble();
		m_OOPRange[name]=value;
	}

	for (Json::ArrayIndex i = 0; i < rangeIP->size(); i++) {
		auto name = ComboMapping[i];
		auto value = (*rangeIP)[i].asDouble();
		m_IPRange[name]=value;
	}

	//�ҳ�sLastPlayerPosition��Ӧ�����λ�ã�ȷ����Ҫ��Χ�������OOP����IP(ע��)
	string sRelativePos;
	//���player_info[0].player.position == sLastPlayerPosition
		//sRelativePos = player_info[0].player.relative_postflop_position
	//else 
		//sRelativePos = player_info[1].player.relative_postflop_position
	// sRelativePos == "OOP" ? pRangeRatio = &m_OOPRange : pRangeRatio = &m_IPRange;

	if(root["player_info"][0]["player"]["position"].asString() == sLastPlayerPosition) {
		sRelativePos = root["player_info"][0]["player"]["relative_postflop_position"].asString();
	} else 	if(root["player_info"][1]["player"]["position"].asString() == sLastPlayerPosition) {
		sRelativePos = root["player_info"][1]["player"]["relative_postflop_position"].asString();
	}

	if(sRelativePos=="OOP") {
		pRangeRatio = &m_OOPRange;
	} else if (sRelativePos=="IP") {
		pRangeRatio = &m_IPRange;
	} else {
		return false;
	}

	//rangeRatio��pRangeRatioָ��ķ�Χ���㣬���·�Χ
	for(auto it = rangeRatio.begin();it != rangeRatio.end();it++){
		(*pRangeRatio)[it->first] = (*pRangeRatio)[it->first] * it->second;
	}


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
bool CRange::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const Stacks& stacks, const string& sBoardNext, const SuitReplace& suitReplace)
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
