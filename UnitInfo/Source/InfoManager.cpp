#include "InfoManager.h"
#include <BWAPI.h>
#include <set>

using namespace std;
using namespace BWAPI;

InfoManager& InfoManager::getInstance() {
	static InfoManager instance;
	return instance;
}

InfoManager::InfoManager() {
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
				//code below works even if SpottedObject is not present at the map (a default one is created)

				_unitData[player].updateUnit(unit);
				//persistentEnemyObjects[(*it)->getID()].update(*it); //this one is updated in GameStateManager::onFrame
			}

		}
	}
}


InfoManager::~InfoManager() {
}

void InfoManager::onFrame() {
	/*//updates spottedEnemies with currently seen enemies:
	map<int, SpottedObject>& spottedEnemies = GameState::getSpottedEnemyUnits();
	//adds all enemy units as spotted objects
	set<BWAPI::Unit*> enemyUnits = BWAPI::Broodwar->enemy()->getUnits();
	set<BWAPI::Unit*>::iterator uit;
	*/

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

	

	//draws information of observed units

	//Logging::getInstance()->log("[%d] I spotted %d enemies", BWAPI::Broodwar->getFrameCount(), spottedEnemies.size());

}

void InfoManager::drawUnit(UnitInfo& u) {
	//don't draw visible units
	if (u.unit->isVisible()) return;

	UnitType type = u.type;
	/*
	Logging::getInstance()->log(
	"[%d] Drawing %s at %d,%d / tile: (%d,%d)",
	BWAPI::Broodwar->getFrameCount(),
	myType.getName().c_str(),
	x, y, x / 32, y / 32
	);
	*/
	int x1 = u.lastPosition.x - type.dimensionLeft();
	int y1 = u.lastPosition.y - type.dimensionUp();
	int x2 = u.lastPosition.x + type.dimensionRight();// - myType.dimensionLeft();
	int y2 = u.lastPosition.y + type.dimensionDown();// -  myType.dimensionUp();


	/*
	Logging::getInstance()->log(
	"Rect coords: (%d,%d), (%d,%d)", x1, y1, x2, y2
	);
	*/

	BWAPI::Broodwar->drawBoxMap(x1, y1, x2, y2, Colors::Red, false);
	BWAPI::Broodwar->drawTextMap(
		x1, u.lastPosition.y,
		type.c_str()
	);
}

void InfoManager::onUnitDiscover(Unit u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);	
}

void InfoManager::onUnitShow(Unit u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}

void InfoManager::onUnitCreate(Unit u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}

void InfoManager::onUnitEvade(Unit u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}

void InfoManager::onUnitHide(Unit u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}

void InfoManager::onUnitComplete(Unit u) {
	//creates or updates information about enemy unit
	_unitData[u->getPlayer()].updateUnit(u);
}


void InfoManager::onUnitDestroy(Unit u) {
	_unitData[u->getPlayer()].removeUnit(u);
}

void InfoManager::onUnitMorph(Unit u) {
	_unitData[u->getPlayer()].updateUnit(u);
}

void InfoManager::onUnitRenegade(Unit u) {
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


