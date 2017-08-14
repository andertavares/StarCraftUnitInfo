# StarCraftUnitInfo
A module to store StarCraft Information of units behind the fog-of-war. Many bot developers must spend some time to develop such unit tracking modules, and the idea here is to offer this as a library. For example, [UAlbertaBot](https://github.com/davechurchill/ualbertabot) has this module nicely implemented there, but it is not trivial to extract and use it in your own bot. So this module is basically an extraction of UAlbertaBot's code, regarding unit tracking. 

The module stores information (position, HP, shields, etc.) concerning the last time a unit was seen. It also has functions for drawing boxes and unit names on such positions to aid visual inspecting and debugging.

The master branch contains code for BWAPI 4.1.2. If you're looking for information and instructions for the version compatible with BWAPI 3.7.5, please take a look here: https://github.com/andertavares/StarCraftUnitInfo/tree/bwapi3

# Preparing the library
- Download the version you need in Releases. Each release has a library (`UnitInfoLib`) and an example AI module (`UnitInfoExampleAI`). For these steps, select the library.
- Extract the contents inside your AI module. There's an `include` and a `lib` directory.
- Add `include` directory to your project in both Debug and Release configurations (right click on your AI Module's project -> Properties -> C/C++ -> Additional Include Directories)
- Add `lib/UnitInfo.lib` to your project under Release configuration and `lib/UnitInfo.lib` to your Debug configuration. In either case, the steps are: right click on your AI Module's project -> Properties -> Linker -> Input -> Additional dependencies.
- Open your AI Module's source file and add `#include "UnitInfoManager.h"` in your preamble.
- Add `scutil::UnitInfoManager::getInstance();` to your `onStart()` to initialize the module.
- Add the callbacks to `scutil::UnitInfoManager` at the following callbacks of your AI Module: `onFrame`, `onUnitDiscover`, `onUnitEvade`, `onUnitShow`, `onUnitHide`, `onUnitCreate`, `onUnitDestroy`, `onUnitMorph`, `onUnitRenegade`, `onUnitComplete`. The callbacks are on the form: `scutil::UnitInfoManager::getInstance().on*`, where you replace the `*` with the corresponding name, passing the same parameters received by the callback of your AI Module.

# Using the library
## Data Structures
The structures and classes concerned with unit information are: 
- `UnitInfoManager`: has the callbacks and the methods to access the other structures
- `UnitData`: takes care of consistency of unit data
- `UIMap`: maps a `BWAPI::Unit` to a `UnitInfo`, which actually stores the information.
- `UnitInfo`: allows you to retrieve unitID, lastHealth, lastShields, player, unit, lastPosition, type, completed of a unit you cannot see anymore.

## Example of use
To retrieve information of units, please take a look at the following piece of code:

```
// Example of use of enemy unit information:
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
 ```
First, you get the UIMap with units of the enemy, then you traverse it querying data of each unit that has been seen at least once, and is still alive.

Also, in our releases, there is a `UnitInfoExampleAI`, which is the basic `ExampleAIModule` of BWAPI, with the project configuration and the code to use the module.

# Contact
If you found some error, please open an Issue. For general questions, feel free to contact the author: andersonrochatavares .at. gmail.com

# Credits
Most of this code was extracted from [UAlbertaBot](https://github.com/davechurchill/ualbertabot). Some code and inspiration came from [OpprimoBot](https://github.com/jhagelback/OpprimoBot).
