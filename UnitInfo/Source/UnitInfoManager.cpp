#include "UnitInfoManager.h"
#include <BWAPI.h>
#include <set>

using namespace std;
using namespace BWAPI;
using namespace scutil;

UnitInfoManager& UnitInfoManager::getInstance() {
	static UnitInfoManager instance;
	return instance;
}

UnitInfoManager::UnitInfoManager() {
	//set<BWAPI::Unit*> enemyUnits = Broodwar->enemy()->getUnits();
	Unitset enemyUnits = Broodwar->enemy()->getUnits();
	//set<BWAPI::Unit*>::iterator it;

	//for (BWAPI::Unit* unit : Broodwar->enemy()->getUnits()){
	//for (it = enemyUnits.begin(); it != enemyUnits.end(); it++) {
	for (auto player: Broodwar->getPlayers()) {
		if (player->isNeutral()) continue;

		for (auto unit : player->getUnits()) {
			//updates only if I can see unit or CompleteMapInformation is enabled
			if (unit->isVisible() || Broodwar->isFlagEnabled(BWAPI::Flag::CompleteMapInformation)) {
				//code below works even if item is not present at the map (a default one is created)
				_unitData[player].updateUnit(unit);
			}

		}
	}
}


UnitInfoManager::~UnitInfoManager() {
}

const scutil::UnitData & UnitInfoManager::getUnitDataOfPlayer(BWAPI::Player player) {
	return _unitData[player];
}

const std::map<BWAPI::Player, scutil::UnitData>& UnitInfoManager::getAllUnitData() const {
	return _unitData;
}

const	UIMap & UnitInfoManager::getUnitInfoMapOfPlayer(BWAPI::Player player) {
	return getUnitDataOfPlayer(player).getUnits();
}

void UnitInfoManager::onFrame() {

	for (auto player : Broodwar->getPlayers()) {
		if (player->isNeutral()) continue;
		//for (uit = enemyUnits.begin(); uit != enemyUnits.end(); uit++) {
		for (auto unit : player->getUnits()){
			//updates only if I can see unit or CompleteMapInformation is enabled
			if (unit->isVisible() || Broodwar->isFlagEnabled(Flag::CompleteMapInformation)) {
				_unitData[player].updateUnit(unit);
			}
		}

		//after updating, draw
		for (auto unitInfoOfPlayer : _unitData) {
			for (auto unitInfo : unitInfoOfPlayer.second.getUnits()) {
				drawUnit(unitInfo.second);
			}
		}
	}
}

void UnitInfoManager::drawUnit(UnitInfo& u) {
	//don't draw visible units
	if (u.unit->isVisible()) return;

	UnitType type = u.type;

	int x1 = u.lastPosition.x - type.dimensionLeft();
	int y1 = u.lastPosition.y - type.dimensionUp();
	int x2 = u.lastPosition.x + type.dimensionRight();
	int y2 = u.lastPosition.y + type.dimensionDown();


	BWAPI::Broodwar->drawBoxMap(x1, y1, x2, y2, Colors::Red, false);
	BWAPI::Broodwar->drawTextMap(
		x1, u.lastPosition.y,
		type.c_str()
	);
}

void UnitInfoManager::onUnitDiscover(Unit u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);	
}

void UnitInfoManager::onUnitShow(Unit u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}

void UnitInfoManager::onUnitCreate(Unit u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}

void UnitInfoManager::onUnitEvade(Unit u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}

void UnitInfoManager::onUnitHide(Unit u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}

void UnitInfoManager::onUnitComplete(Unit u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}


void UnitInfoManager::onUnitDestroy(Unit u) {
	_unitData[u->getPlayer()].removeUnit(u);
}

void UnitInfoManager::onUnitMorph(Unit u) {
	_unitData[u->getPlayer()].updateUnit(u);
}

void UnitInfoManager::onUnitRenegade(Unit u) {
	//removes unit from previous owner
	for (auto unitInfoOfPlayer : _unitData) {
		if (unitInfoOfPlayer.second.getUnits().find(u) != unitInfoOfPlayer.second.getUnits().end()) {
			//found! remove:
			unitInfoOfPlayer.second.removeUnit(u);
		}
	}

	//add unit to new owner
	_unitData[u->getPlayer()].updateUnit(u);
}


