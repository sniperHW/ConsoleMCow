#pragma once

#ifndef CPLAYER_H_
#define CPLAYER_H_

#include "globledef.h"
#include "CCombo.h"

class CPlayer
{
public:
	std::string m_sPlayName;
	Position m_position;
	float m_fInitStack;

	Position m_positionByPresent;
	RelativePosition m_positionRelative;

	float m_fEffectiveStack;
	CCombo m_hand;
	Action m_lastAction;

	CPlayer(std::string sPlayName, Position position, float fInitStack);
};

#endif