/**
 * This is just like the ExampleAIModule provided with BWAPI.
 * We only added the code needed to instantiate and use UnitInfoManager
 * A line of code was added at onStart
 * A line of code was added at onFrame
 * Callback codes were replaced with calls to UnitInfoManager methods
 */

#include "ExampleAIModule.h"
#include "UnitInfoManager.h"
#include <iostream>

using namespace BWAPI;
using namespace scutil;
using namespace Filter;

void ExampleAIModule::onStart() {
	// Hello World!
	Broodwar->printf("Hello world!");

	// Print the map name.
	// BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
	Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	// and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	UnitInfoManager::getInstance(); //initializes the UnitInfoManager

}

void ExampleAIModule::onEnd(bool isWinner) {
	// Called when the game ends
}

void ExampleAIModule::onFrame() {
	// Called once every game frame

	// Display the game frame rate as text in the upper left area of the screen
	Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());

	// Return if the game is a replay or is paused
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	// activates the onFrame of UnitInfoManager
	UnitInfoManager::getInstance().onFrame();

	// Example of use of enemy unit information:
 	// traverses every enemy unit in my knowledge and show their information
	UIMap enemyUnitInfo = UnitInfoManager::getInstance().getUnitInfoMapOfPlayer(Broodwar->enemy());
	
	for (auto iterator : enemyUnitInfo) {
	  Unit u = iterator.first;
	  UnitInfo uInfo = iterator.second;
	  
	  Broodwar->printf(
		  "%s at (%d, %d)", uInfo.type.c_str(),
		  uInfo.lastPosition.x, uInfo.lastPosition.y
	  );
	  // see UnitData.h for all attributes of UnitInfo struct ;)
	}

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;



	// Iterate through all the units that we own
	for (auto &u : Broodwar->self()->getUnits()) {
		// Ignore the unit if it no longer exists
		// Make sure to include this block when handling any Unit pointer!
		if (!u->exists())
			continue;

		// Ignore the unit if it has one of the following status ailments
		if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
			continue;

		// Ignore the unit if it is in one of the following states
		if (u->isLoaded() || !u->isPowered() || u->isStuck())
			continue;

		// Ignore the unit if it is incomplete or busy constructing
		if (!u->isCompleted() || u->isConstructing())
			continue;


		// Finally make the unit do some stuff!


		// If the unit is a worker unit
		if (u->getType().isWorker()) {
			// if our worker is idle
			if (u->isIdle()) {
				// Order workers carrying a resource to return them to the center,
				// otherwise find a mineral patch to harvest.
				if (u->isCarryingGas() || u->isCarryingMinerals()) {
					u->returnCargo();
				}
				else if (!u->getPowerUp()) { // The worker cannot harvest anything if it
					// is carrying a powerup such as a flag
					// Harvest from the nearest mineral patch or gas refinery
					if (!u->gather(u->getClosestUnit(IsMineralField || IsRefinery))) {
						// If the call fails, then print the last error message
						Broodwar << Broodwar->getLastError() << std::endl;
					}

				} // closure: has no powerup
			} // closure: if idle

		}
		else if (u->getType().isResourceDepot()) { // A resource depot is a Command Center, Nexus, or Hatchery


			// Order the depot to construct more workers! But only when it is idle.
			if (u->isIdle() && !u->train(u->getType().getRace().getWorker())) {
				// If that fails, draw the error at the location so that you can visibly see what went wrong!
				// However, drawing the error once will only appear for a single frame
				// so create an event that keeps it on the screen for some frames
				Position pos = u->getPosition();
				Error lastErr = Broodwar->getLastError();
				Broodwar->registerEvent([pos, lastErr](Game*) { Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
					nullptr,    // condition
					Broodwar->getLatencyFrames());  // frames to run

				// Retrieve the supply provider type in the case that we have run out of supplies
				UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
				static int lastChecked = 0;

				// If we are supply blocked and haven't tried constructing more recently
				if (lastErr == Errors::Insufficient_Supply &&
					lastChecked + 400 < Broodwar->getFrameCount() &&
					Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0) {
					lastChecked = Broodwar->getFrameCount();

					// Retrieve a unit that is capable of constructing the supply needed
					Unit supplyBuilder = u->getClosestUnit(GetType == supplyProviderType.whatBuilds().first &&
						(IsIdle || IsGatheringMinerals) &&
						IsOwned);
					// If a unit was found
					if (supplyBuilder) {
						if (supplyProviderType.isBuilding()) {
							TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
							if (targetBuildLocation) {
								// Register an event that draws the target build location
								Broodwar->registerEvent([targetBuildLocation, supplyProviderType](Game*) {
									Broodwar->drawBoxMap(Position(targetBuildLocation),
										Position(targetBuildLocation + supplyProviderType.tileSize()),
										Colors::Blue);
								},
									nullptr,  // condition
									supplyProviderType.buildTime() + 100);  // frames to run

								// Order the builder to construct the supply structure
								supplyBuilder->build(supplyProviderType, targetBuildLocation);
							}
						}
						else {
							// Train the supply provider (Overlord) if the provider is not a structure
							supplyBuilder->train(supplyProviderType);
						}
					} // closure: supplyBuilder is valid
				} // closure: insufficient supply
			} // closure: failed to train idle unit

		}

	} // closure: unit iterator
}

void ExampleAIModule::onSendText(std::string text) {

	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());


	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!

}

void ExampleAIModule::onReceiveText(BWAPI::Player player, std::string text) {
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player player) {
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target) {

	// Check if the target is a valid position
	if (target) {
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else {
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void ExampleAIModule::onUnitDiscover(BWAPI::Unit unit) {
	UnitInfoManager::getInstance().onUnitDiscover(unit);
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit unit) {
	UnitInfoManager::getInstance().onUnitEvade(unit);
}

void ExampleAIModule::onUnitShow(BWAPI::Unit unit) {
	UnitInfoManager::getInstance().onUnitShow(unit);
}

void ExampleAIModule::onUnitHide(BWAPI::Unit unit) {
	UnitInfoManager::getInstance().onUnitHide(unit);
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit unit) {
	UnitInfoManager::getInstance().onUnitCreate(unit);
}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit) {
	UnitInfoManager::getInstance().onUnitDestroy(unit);
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit) {
	UnitInfoManager::getInstance().onUnitMorph(unit);
}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit) {
	UnitInfoManager::getInstance().onUnitRenegade(unit);
}

void ExampleAIModule::onSaveGame(std::string gameName) {
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit) {
	UnitInfoManager::getInstance().onUnitComplete(unit);
}
