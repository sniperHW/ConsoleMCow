#pragma once

#ifndef CPLAYER_H_
#define CPLAYER_H_

#include "globledef.h"
#include "CCombo.h"


class CPlayer
{
public:
	bool m_blIsHero;
	Position m_position;

	Position m_positionByPresent;
	RelativePosition m_positionRelative;

	double m_dEStack;
	CCombo m_hands;
//	Action m_lastAction;

	CPlayer(Position position, const double dEStack);
	CPlayer();
};

#endif