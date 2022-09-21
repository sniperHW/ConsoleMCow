//#include "pch.h"
#include "CPlayer.h"

using namespace std;

CPlayer::CPlayer(Position position, const double dEStack) : m_position(position), m_dEStack(dEStack)
{
	m_blIsHero = false;
	m_positionByPresent = m_position;
	m_positionRelative = OOP;
}

CPlayer::CPlayer()
{
	m_blIsHero = false;
}