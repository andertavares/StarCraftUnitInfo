#pragma once

#include <map>
#include <BWAPI.h>
#include "UnitData.h"

class UnitInfoManager {
	/**
	 * Effectively stores the unit data
	 */
	std::map<BWAPI::Player*, ::UnitData> _unitData;
	void drawUnit(UnitInfo& unitInfo);
	UnitInfoManager();

public:
	/**
	 * Returns the singleton instance of this class 
	 */
	static UnitInfoManager& getInstance();
	~UnitInfoManager();

	void onStart();
	void onEnd(bool isWinner);
	void onFrame();
	
	void onPlayerLeft(BWAPI::Player* player);
	
	void onUnitDiscover(BWAPI::Unit* unit);
	void onUnitEvade(BWAPI::Unit* unit);
	void onUnitShow(BWAPI::Unit* unit);
	void onUnitHide(BWAPI::Unit* unit);
	void onUnitCreate(BWAPI::Unit* unit);
	void onUnitDestroy(BWAPI::Unit* unit);
	void onUnitMorph(BWAPI::Unit* unit);
	void onUnitRenegade(BWAPI::Unit* unit);
	void onUnitComplete(BWAPI::Unit* unit);
};

