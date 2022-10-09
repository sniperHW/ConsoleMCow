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


//加载文件，返回以行分割的内容（不包括空白行）
void loadFileAsLine(const string& path,vector<string> &lines) {
	std::ifstream ifs;
  	ifs.open(path);
	if(ifs.is_open()){
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
		ifs.close();
	}
}


//file模式
bool CRange::Load(GameType gmType, const string& sNodeName, const string& sBoardNext)
{
	//获取节点名称对应的文件路径，未找到则返回false,代表offline
	string sPath = CDataFrom::GetRangesFilePath(gmType, sNodeName);
	if (sPath.size() == 0)
		return false;

	//从范围文件加载数据，OOP和IP对应
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


	//范围中去除新的公牌对应的比例(改为0)，sBoardNext中可能是三张或一张，三张每张对应的都有去除
	RemoveComboByBoard(m_IPRange, sBoardNext);
	RemoveComboByBoard(m_OOPRange, sBoardNext);

	return true;
}

//wizard模式(未测试)
bool CRange::Load(GameType gmType, const string& sNodeName, const string& sBoardNext, const SuitReplace& suitReplace, const string& sIsoBoard)
{
	string sNodeNameTmp = sNodeName;
	string sStrategyFilePath;
	string sActionSquenceTmp;
	RangeData rangeRatio;
	string sLastPlayerPosition;
	RangeData* pRangeRatio = nullptr;

	//flop需要处理同构，存在同构替换则替换节点名称中的board部分
	if (suitReplace.blIsNeeded) {
		regex reg("<.*>");
		string sReplace = "<" + sIsoBoard + ">";
		sNodeNameTmp = regex_replace(sNodeName, reg, sReplace);
	}

	//从策略配置中获取替换和replace设置，存在替换则启用替换的名称
	string sReplace = g_rangeNodeConfigs[gmType].GetReplace(sNodeNameTmp);
	if (!sReplace.empty())
		sNodeNameTmp = sReplace;

	//获取节点名称对应的文件路径，未找到则返回false,代表offline
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

	//取C或X对应策略，和最后行动者的位置(注释)
	Clear(); //先清空原始range（不需要按原始range来计算）
	//solution下找出action.code = "X"或"C"的[n]，rangeRatio = solution.[n].strategy；sLastPlayerPosition = solution.[n].action.position
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

	//加载原始范围(注释)
	//如果player_info[0].player.relative_postflop_position == "OOP"
		//m_OOPRange = player_info[0].player.range ；m_IPRange = player_info[1].player.range
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

	//找出sLastPlayerPosition对应的相对位置，确定需要范围运算的是OOP还是IP(注释)
	string sRelativePos;
	//如果player_info[0].player.position == sLastPlayerPosition
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

	//rangeRatio和pRangeRatio指向的范围运算，更新范围
	for (auto it = rangeRatio.begin(); it != rangeRatio.end(); it++) {
		(*pRangeRatio)[it->first] = (*pRangeRatio)[it->first] * it->second;
	}


	//同构转换
	if (suitReplace.blIsNeeded) {
		ConvertIsomorphism(m_OOPRange, suitReplace);
		ConvertIsomorphism(m_IPRange, suitReplace);
	}

	//范围中去除新的公牌对应的比例(改为0)，sBoardNext中可能是三张或一张，三张每张对应的都有去除
	RemoveComboByBoard(m_IPRange, sBoardNext);
	RemoveComboByBoard(m_OOPRange, sBoardNext);

	return true;
}

//solver模式
bool CRange::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const Stacks& stacks, const Stacks& stacksByStrategy ,const string& sBoardNext, const SuitReplace& suitReplace)
{
	RangeData OOPRangeRatio; //用于计算range剩余比例，OOP代表第一个行动的玩家
	RangeData IPRangeRatio; //用于计算range剩余比例，IP代表第二个行动的玩家
	RangeData* pRangeRatio = nullptr;


	//解析ActionSquence,取最后一个<>后的序列sCSquence
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

	//对actionSquence中每一个动作（只有X，R，C，其中X只会出现在第一个，最后一个一定是C或X，没有A和O）
		//确定是操作OOP还是IP（奇数是OOP，偶数是IP，赋值pRangeRatio）
		//当前节点（第一个为root）下选择对应的动作，和策略中方法一样，但R不需要匹配A
		//对策略数据中每个组合，用选定动作对应的比例去改写pRangeRatio中对应的数据（如果是第一次行动则填写1*比例，非第一次行动则填写*pRangeRatio[组合] * 比例，区分第一次目的是让不存在的组合保持0）
		//不是最后一个动作则按之前匹配的动作选择子节点，为当前节点
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
						//找到最大值
						double max = 0;
						double maxIdx = -1;
						for(int i = 0;i < int(bets.size());i++){
							if(bets[i]>max){
								maxIdx = i;
							}
						}
						//跟最后一个元素交换位置
						swap(bets[maxIdx],bets[bets.size()-1]);
						swap(names[maxIdx],names[names.size()-1]);
						//将最大值丢弃
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
				//第一次1*value
				(*pRangeRatio)[name] = value;
			} else {
				mapIt->second *= value;
			}
		}

		//根据动作选择下一节点
		node = CStrategy::geActionNode(node,a,actions,0);
		if(node==nullptr){
			return false;
		}else {
			actions.push_back(a);
		}
	}


	//同构转换
	if (suitReplace.blIsNeeded) {
		ConvertIsomorphism(OOPRangeRatio, suitReplace);
		ConvertIsomorphism(IPRangeRatio, suitReplace);
	}

	//和原始范围运算,更新原始范围（每个组合原始范围数据 *RangeRatio对应的比例, 能否一个transform函数加multiplies）

	//范围中去除新的公牌对应的比例(改为0)，sBoardNext中可能是三张或一张，三张每张对应的都有去除
	RemoveComboByBoard(m_IPRange, sBoardNext);
	RemoveComboByBoard(m_OOPRange, sBoardNext);

	return true;
}

//转为solver配置文件需要的格式
string CRange::GetRangeStr()
{
	//参照file模式里的格式
	string v;
	return v;
}

//清空双方范围，置为0
void CRange::Clear()
{

}
//同构转换
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

//排除公牌相关的组合（sBoardNext格式：KsQh7s,3个或1个）
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
