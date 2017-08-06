#include "UnitData.h"

using namespace scutil;

UnitData::UnitData() 
	: mineralsLost(0)
	, gasLost(0)
{
	int maxTypeID(0);
	std::set<BWAPI::UnitType>::iterator typeIt;
	std::set<BWAPI::UnitType> types = BWAPI::UnitTypes::allUnitTypes();
	//for (BWAPI::UnitType t : BWAPI::UnitTypes::allUnitTypes())
	for (typeIt = types.begin(); typeIt != types.end(); typeIt++)
	{

		maxTypeID = maxTypeID > (*typeIt).getID() ? maxTypeID : (*typeIt).getID();
	}

	numDeadUnits	    = std::vector<int>(maxTypeID + 1, 0);
	numUnits		    = std::vector<int>(maxTypeID + 1, 0);
}

void UnitData::updateUnit(BWAPI::Unit* unit)
{
	if (!unit) { return; }

    bool firstSeen = false;
    //auto & it = unitMap.find(unit);
	UIMap::iterator it = unitMap.find(unit);
    if (it == unitMap.end())
    {
        firstSeen = true;
        unitMap[unit] = UnitInfo();
    }
    
	UnitInfo & ui   = unitMap[unit];
    ui.unit         = unit;
    ui.player       = unit->getPlayer();
	ui.lastPosition = unit->getPosition();
	ui.lastHealth   = unit->getHitPoints();
    ui.lastShields  = unit->getShields();
	ui.unitID       = unit->getID();
	ui.type         = unit->getType();
    ui.completed    = unit->isCompleted();

    if (firstSeen)
    {
        numUnits[unit->getType().getID()]++;
    }
}

void UnitData::removeUnit(BWAPI::Unit* unit)
{
	if (!unit) { return; }

	mineralsLost += unit->getType().mineralPrice();
	gasLost += unit->getType().gasPrice();
	numUnits[unit->getType().getID()]--;
	numDeadUnits[unit->getType().getID()]++;
		
	unitMap.erase(unit);
}

void UnitData::removeBadUnits()
{
	UIMap::iterator iter;
	for (iter = unitMap.begin(); iter != unitMap.end();)
	{
		if (badUnitInfo(iter->second))
		{
			numUnits[iter->second.type.getID()]--;
			iter = unitMap.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

const bool UnitData::badUnitInfo(const UnitInfo & ui) const
{
    if (!ui.unit)
    {
        return false;
    }

	// Cull away any refineries/assimilators/extractors that were destroyed and reverted to vespene geysers
	if (ui.unit->getType() == BWAPI::UnitTypes::Resource_Vespene_Geyser)
	{ 
		return true;
	}

	// If the unit is a building and we can currently see its position and it is not there
	if (ui.type.isBuilding() && BWAPI::Broodwar->isVisible(ui.lastPosition.x()/32, ui.lastPosition.y()/32) && !ui.unit->isVisible())
	{
		return true;
	}

	return false;
}

int UnitData::getGasLost() const 
{ 
    return gasLost; 
}

int UnitData::getMineralsLost() const 
{ 
    return mineralsLost; 
}

int UnitData::getNumUnits(BWAPI::UnitType t) const 
{ 
    return numUnits[t.getID()]; 
}

int UnitData::getNumDeadUnits(BWAPI::UnitType t) const 
{ 
    return numDeadUnits[t.getID()]; 
}

const UIMap & UnitData::getUnits() const 
{ 
    return unitMap; 
}