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
	set<BWAPI::Unit*> enemyUnits = Broodwar->enemy()->getUnits();

	set<Player*>::iterator playerIt;
	set<Player*> players = Broodwar->getPlayers();
	for (playerIt = players.begin(); playerIt != players.end(); playerIt++) {
		Player* player = *playerIt;
		if (player->isNeutral()) continue;

		set<Unit*>::iterator uIt;
		set<Unit*> units = player->getUnits();
		for (uIt = units.begin(); uIt != units.end(); uIt++){
			Unit* unit = *uIt;
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

const scutil::UnitData & UnitInfoManager::getUnitDataOfPlayer(BWAPI::Player* player) {
	return _unitData[player];
}

const std::map<BWAPI::Player*, scutil::UnitData>& UnitInfoManager::getAllUnitData() const {
	return _unitData;
}

const	UIMap & UnitInfoManager::getUnitInfoMapOfPlayer(BWAPI::Player* player) {
	return getUnitDataOfPlayer(player).getUnits();
}

void UnitInfoManager::onFrame() {

	set<Player*>::iterator playerIt;
	set<Player*> players = Broodwar->getPlayers();
	for (playerIt = players.begin(); playerIt != players.end(); playerIt++) {
	//for (const Player* player : Broodwar->getPlayers()) {
		Player* player = *playerIt;
		if (player->isNeutral()) continue;

		set<BWAPI::Unit*>::iterator uit;
		set<BWAPI::Unit*> units = player->getUnits();
		for (uit = units.begin(); uit != units.end(); uit++) {
		//for (auto unit : player->getUnits()){
			Unit* unit = *uit;
			//updates only if I can see unit or CompleteMapInformation is enabled
			if (unit->isVisible() || Broodwar->isFlagEnabled(Flag::CompleteMapInformation)) {
				_unitData[player].updateUnit(unit);
			}
		}

		//after updating, draw
		std::map<BWAPI::Player*, scutil::UnitData>::iterator it;

		//for (auto unitInfoOfPlayer : _unitData) {
		for (it = _unitData.begin(); it != _unitData.end(); it++) {
		//for (auto unitInfoOfPlayer : _unitData) {
			std::pair<Player*, scutil::UnitData> unitInfoOfPlayer = *it;

			UIMap::iterator it;
			UIMap unitInfos = unitInfoOfPlayer.second.getUnits();
			//for (auto unitInfo : unitInfoOfPlayer.second.getUnits()) {
			for (it = unitInfos.begin(); it != unitInfos.end(); it++){
				pair<Unit*, UnitInfo> unitInfo = *it;
				drawUnit(unitInfo.second);
			}
		}
	}
}

void UnitInfoManager::drawUnit(UnitInfo& u) {
	//don't draw visible units
	if (u.unit->isVisible()) return;

	UnitType type = u.type;

	int x1 = u.lastPosition.x() - type.dimensionLeft();
	int y1 = u.lastPosition.y() - type.dimensionUp();
	int x2 = u.lastPosition.x() + type.dimensionRight();
	int y2 = u.lastPosition.y() + type.dimensionDown();
	

	BWAPI::Broodwar->drawBoxMap(x1, y1, x2, y2, Colors::Red, false);
	BWAPI::Broodwar->drawTextMap(
		x1, u.lastPosition.y(),
		type.c_str()
	);
}

void UnitInfoManager::onUnitDiscover(Unit* u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);	
}

void UnitInfoManager::onUnitShow(Unit* u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}

void UnitInfoManager::onUnitCreate(Unit* u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}

void UnitInfoManager::onUnitEvade(Unit* u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}

void UnitInfoManager::onUnitHide(Unit* u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}

void UnitInfoManager::onUnitComplete(Unit* u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}


void UnitInfoManager::onUnitDestroy(Unit* u) {
	_unitData[u->getPlayer()].removeUnit(u);
}

void UnitInfoManager::onUnitMorph(Unit* u) {
	_unitData[u->getPlayer()].updateUnit(u);
}

void UnitInfoManager::onUnitRenegade(Unit* u) {
	//removes unit from previous owner
	map<BWAPI::Player*, scutil::UnitData>::iterator it;

	//for (auto unitInfoOfPlayer : _unitData) {
	for (it = _unitData.begin(); it != _unitData.end(); it++){
		pair<Player*, scutil::UnitData> unitInfoOfPlayer = *it;
		if (unitInfoOfPlayer.second.getUnits().find(u) != unitInfoOfPlayer.second.getUnits().end()) {
			//found! remove:
			unitInfoOfPlayer.second.removeUnit(u);
		}
	}

	//add unit to new owner
	_unitData[u->getPlayer()].updateUnit(u);
}
