#pragma once

#include <map>
#include <BWAPI.h>
#include "UnitData.h"

class UnitInfo {
	std::map<BWAPI::Player, UnitData> _unitData;
public:
	UnitInfo();
	~UnitInfo();
};

