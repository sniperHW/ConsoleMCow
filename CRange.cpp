//#include "pch.h"
#include "CRange.h"
#include "CStrategy.h"
#include "CDataFrom.h"
#include "util.h"
#include <regex>
#include "CRangeNodeConfig.h"
#include "CStrategyTreeConfig.h"
#include "globledef.h"
#include <fstream>
#include "CActionLine.h"

using namespace std;

extern map<GameType, CRangeNodeConfig> g_rangeNodeConfigs;


//�����ļ����������зָ�����ݣ��������հ��У�
void loadFileAsLine(const string& path,vector<string> &lines) {
	std::ifstream ifs;
  	ifs.open(path);
	if(ifs.is_open()){
		ifs.seekg (0, ifs.end);
		int length = ifs.tellg();
		ifs.seekg (0, ifs.beg);
		char *buffer = new char[length+1];
		ifs.read (buffer,length);
		buffer[length] = 0;
		auto l = split(buffer,'\n');
		for(auto it = l.begin();it != l.end();it++) {
			if(*it != "") {
				lines.push_back(*it);
			}
		}
		delete [] buffer;
		ifs.close();
	}
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

	string *oopStr = nullptr;
	string *ipStr = nullptr;

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
bool CRange::Load(GameType gmType, const string& sNodeName, const string& sBoardNext, const SuitReplace& suitReplace, const string& sIsoBoard)
{
	string sNodeNameTmp = sNodeName;
	string sStrategyFilePath;
	string sActionSquenceTmp;
	RangeData rangeRatio;
	string sLastPlayerPosition;
	RangeData* pRangeRatio = nullptr;

	//flop��Ҫ����ͬ��������ͬ���滻���滻�ڵ������е�board����
	if (suitReplace.blIsNeeded) {
		regex reg("<.*>");
		string sReplace = "<" + sIsoBoard + ">";
		sNodeNameTmp = regex_replace(sNodeName, reg, sReplace);
	}

	//�Ӳ��������л�ȡ�滻��replace���ã������滻�������滻������
	string sReplace = g_rangeNodeConfigs[gmType].GetReplace(sNodeNameTmp);
	if (!sReplace.empty())
		sNodeNameTmp = sReplace;

	//��ȡ�ڵ����ƶ�Ӧ���ļ�·����δ�ҵ��򷵻�false,����offline
	//sStrategyFilePath = g_dataFrom.GetWizardFilePath(gmType, sNodeNameTmp, curRound);
	//if(sStrategyFilePath.size() == 0){
	//	return false;
	//}

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

	//ȡC��X��Ӧ���ԣ�������ж��ߵ�λ��(ע��)
	Clear(); //�����ԭʼrange������Ҫ��ԭʼrange�����㣩
	//solution���ҳ�action.code = "X"��"C"��[n]��rangeRatio = solution.[n].strategy��sLastPlayerPosition = solution.[n].action.position
	auto solutions = root["solutions"];
	const Json::Value* nodeStrategy = nullptr;
	for (auto it = solutions.begin(); it != solutions.end(); it++) {
		if ((*it)["action"]["code"] == "C" || (*it)["action"]["code"] == "X") {
			sLastPlayerPosition = (*it)["action"]["position"].asString();
			nodeStrategy = &(*it)["strategy"];
			break;
		}
	}

	if (sLastPlayerPosition == "") {
		return false;
	}

	for (Json::ArrayIndex i = 0; i < nodeStrategy->size(); i++) {
		auto name = ComboMapping[i];
		auto value = (*nodeStrategy)[i].asDouble();
		rangeRatio[name] = value;
	}

	//����ԭʼ��Χ(ע��)
	//���player_info[0].player.relative_postflop_position == "OOP"
		//m_OOPRange = player_info[0].player.range ��m_IPRange = player_info[1].player.range
	 //else m_OOPRange = player_info[1].player.range; m_IPRange = player_info[0].player.range

	const Json::Value* rangeOOP;
	const Json::Value* rangeIP;

	if (root["player_info"][0]["relative_postflop_position"] == "OOP") {
		rangeOOP = &root["player_info"][0]["range"];
		rangeIP = &root["player_info"][1]["range"];
	}
	else {
		rangeOOP = &root["player_info"][1]["range"];
		rangeIP = &root["player_info"][0]["range"];
	}


	for (Json::ArrayIndex i = 0; i < rangeOOP->size(); i++) {
		auto name = ComboMapping[i];
		auto value = (*rangeOOP)[i].asDouble();
		m_OOPRange[name] = value;
	}

	for (Json::ArrayIndex i = 0; i < rangeIP->size(); i++) {
		auto name = ComboMapping[i];
		auto value = (*rangeIP)[i].asDouble();
		m_IPRange[name] = value;
	}

	//�ҳ�sLastPlayerPosition��Ӧ�����λ�ã�ȷ����Ҫ��Χ�������OOP����IP(ע��)
	string sRelativePos;
	//���player_info[0].player.position == sLastPlayerPosition
		//sRelativePos = player_info[0].player.relative_postflop_position
	//else 
		//sRelativePos = player_info[1].player.relative_postflop_position
	// sRelativePos == "OOP" ? pRangeRatio = &m_OOPRange : pRangeRatio = &m_IPRange;

	if (root["player_info"][0]["player"]["position"].asString() == sLastPlayerPosition) {
		sRelativePos = root["player_info"][0]["player"]["relative_postflop_position"].asString();
	}
	else 	if (root["player_info"][1]["player"]["position"].asString() == sLastPlayerPosition) {
		sRelativePos = root["player_info"][1]["player"]["relative_postflop_position"].asString();
	}

	if (sRelativePos == "OOP") {
		pRangeRatio = &m_OOPRange;
	}
	else if (sRelativePos == "IP") {
		pRangeRatio = &m_IPRange;
	}
	else {
		return false;
	}

	//rangeRatio��pRangeRatioָ��ķ�Χ���㣬���·�Χ
	for (auto it = rangeRatio.begin(); it != rangeRatio.end(); it++) {
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
bool CRange::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const Stacks& stacks, const Stacks& stacksByStrategy ,const string& sBoardNext, const SuitReplace& suitReplace)
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
	vector<Action> actions={};
	const Json::Value *node = &root;
	for(auto i = 0;i<int(actionSquence.size());i++){
		if(i%2==0) {
			pRangeRatio = &OOPRangeRatio;
		} else {
			pRangeRatio = &IPRangeRatio;		
		}
		double sstack = 0;
		if(i==actionSquence.size()-1) {
			sstack = stacks.dEStack;
		}

		auto a = actionSquence[i];
		auto getActionIdx = [actions] (const Json::Value *node,const Action& action,double sstack) -> int {
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
							auto v = CStrategy::CalcBetRatio(getBetByStr(*it2),actions,int(actions.size()),sstack);	
							bets.push_back(v);
						}
					}

					if(bets.size()>1){
						//�ҵ����ֵ
						double max = 0;
						double maxIdx = -1;
						for(int i = 0;i < int(bets.size());i++){
							if(bets[i]>max){
								maxIdx = i;
							}
						}
						//�����һ��Ԫ�ؽ���λ��
						swap(bets[maxIdx],bets[bets.size()-1]);
						swap(names[maxIdx],names[names.size()-1]);
						//�����ֵ����
						bets.pop_back();
						names.pop_back();	
					}


					auto j = CStrategy::MatchBetRatio(action.fBetSize,bets);
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
		

		int j = getActionIdx(node,a,sstack);

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
		node = CStrategy::geActionNode(node,a,actions,0);
		if(node==nullptr){
			return false;
		}else {
			actions.push_back(a);
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
	std::unordered_map<std::string, double>::iterator  it_map;
	std::unordered_map<std::string, double> mapTemp;

	for (it_map = rangeRatio.begin(); it_map != rangeRatio.end(); it_map++)
	{
		string key = it_map->first;
		string strKeyIsomor = CStrategy::ConvertOneHandcard(key, suitReplace);
		mapTemp.insert({ strKeyIsomor, it_map->second });
	}
	rangeRatio.clear();
	rangeRatio.insert(mapTemp.begin(), mapTemp.end());
}

static bool matchBoardNext(const string &range,const string& sBoardNext) {
	for(int i = 0;i<int(range.size());i+=2) {
		for(int j=0;j<int(sBoardNext.size());j+=2) {
			if(range[i] == sBoardNext[j] && range[i+1] == sBoardNext[j+1]) {
				return true;
			}
		}
	}
	return false;
}

//�ų�������ص���ϣ�sBoardNext��ʽ��KsQh7s,3����1����
void CRange::RemoveComboByBoard(RangeData& rangeRatio, const std::string& sBoardNext)
{
	for(auto it = rangeRatio.begin();it != rangeRatio.end();) {
		if(matchBoardNext(it->first,sBoardNext)){
			it = rangeRatio.erase(it);
		} else {
			it++;
		}
	}
}


void CRange::DumpRange(const string& sPath, const RangeData& range) {
	std::ofstream ofs;
	ofs.open(sPath,std::ofstream::out);
	if(ofs.is_open()) {
		for(auto it = range.begin();it != range.end();it++) {
			if(it != range.begin()){
				ofs << "\n";
			}
			ofs << it->first << "," << it->second;
		}
	}
	ofs.close();
}

extern void loadFileAsLine(const string& path,vector<string> &lines);

void CRange::ReadRange(const string& sPath, RangeData& range) {
	vector<string> lines;
	loadFileAsLine(sPath,lines);
	for(auto l:lines) {
		auto r = split(l,',');
		range[r[0]] = stringToNum<double>(r[1]);
	}
}
