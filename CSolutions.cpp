#include "CSolutions.h"

using namespace std;

shared_ptr<CSolution> CSolutions::NewSolution(const string& sGameID)
{
	shared_ptr<CSolution> pNewSolution = make_shared<CSolution>();
	m_Solutions[sGameID] = pNewSolution;

	return pNewSolution;

}

shared_ptr<CSolution> CSolutions::GetSolutionByGameID(const string& sGameID)
{
	auto p = m_Solutions.find(sGameID);
	if (p != m_Solutions.end())
		return p->second;
	else
		return nullptr;
}

//?是否已经释放
void CSolutions::FinishSolution(const string& sGameID)
{
	m_Solutions.erase(sGameID);
}
