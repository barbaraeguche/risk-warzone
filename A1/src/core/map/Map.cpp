#include "Map.h"

#include <iostream>
#include <unordered_set>

// ==================== Territory Class Implementation ====================

Territory::Territory() :
  territoryName(new std::string("")),
  territoryId(new int(0)),
  ownerPlayer(new std::string("")),
  armies(new int(0)),
  continent(nullptr) {}

Territory::Territory(const std::string& name, int id) :
  territoryName(new std::string(name)),
  territoryId(new int(id)),
  ownerPlayer(new std::string("")),
  armies(new int(0)),
  continent(nullptr) {}

Territory::Territory(const Territory& other) :
  territoryName(new std::string(*other.territoryName)),
  territoryId(new int(*other.territoryId)),
  ownerPlayer(new std::string(*other.ownerPlayer)),
  armies(new int(*other.armies)),
  adjTerritories(other.adjTerritories),
  continent(other.continent) {}

Territory& Territory::operator=(const Territory& other) {
  if (this != &other) {
    setName(*other.territoryName);
    setId(*other.territoryId);
    setOwner(*other.ownerPlayer);
    setArmies(*other.armies);

    adjTerritories = other.adjTerritories;
    setContinent(other.continent);
  }
  return *this;
}

Territory::~Territory() {
  delete territoryName;
  delete territoryId;
  delete ownerPlayer;
  delete armies;
}

// --- MANAGEMENT ---
bool Territory::isAdjacentTo(Territory* territory) const {
  return std::ranges::find(adjTerritories, territory) != adjTerritories.end();
}

void Territory::addAdjacentTerritory(Territory* territory) {
  if (territory && territory != this && !isAdjacentTo(territory)) {
    adjTerritories.push_back(territory);
  }
}

void Territory::removeAdjacentTerritory(Territory* territory) {
  std::erase(adjTerritories, territory);
}

// --- UTILITY ---
void Territory::displayInfo() const {
  std::cout << "Territory: " << (territoryName ? *territoryName : "None")
            << " (ID: " << (territoryId ? *territoryId : -1) << ")" << std::endl;

  std::cout << "  Owner: " << (ownerPlayer && !ownerPlayer->empty() ? *ownerPlayer : "None")
            << ", Armies: " << (armies ? *armies : -0 ) << std::endl;

  std::cout << "  Continent: " << (continent ? continent->getName() : "None") << std::endl;

	std::cout << "  Adjacent to: ";
	if (adjTerritories.empty()) {
		std::cout << "None";
	} else {
		for (size_t i = 0; i < adjTerritories.size(); i++) {
			std::cout << (adjTerritories[i] ? adjTerritories[i]->getName() : "None");
			if (i < adjTerritories.size() - 1) {
				std::cout << ", ";
			}
		}
	}

	std::cout << std::endl;
}

bool Territory::operator==(const Territory& other) const {
	return territoryId == other.territoryId && territoryName == other.territoryName;
}


// ==================== Continent Class Implementation ====================

Continent::Continent() :
  continentName(new std::string("")),
  continentId(new int(0)) {}

Continent::Continent(const std::string& name, int id) :
  continentName(new std::string(name)),
  continentId(new int(id)) {}

Continent::Continent(const Continent& other) :
  continentName(new std::string(*other.continentName)),
  continentId(new int(*other.continentId)),
  territories(other.territories) {}

Continent& Continent::operator=(const Continent& other) {
  if (this != &other) {
    setName(*other.continentName);
    setId(*other.continentId);

    territories = other.territories;
  }
  return *this;
}

Continent::~Continent() {
  delete continentName;
  delete continentId;

  for (Territory* territory : territories) {
    delete territory;
  }
  territories.clear();
}

// --- MANAGEMENT ---
bool Continent::containsTerritory(Territory* territory) const {
  return std::ranges::find(territories, territory) != territories.end();
}

void Continent::addTerritory(Territory* territory) {
  if (territory && !containsTerritory(territory)) {
    territories.push_back(territory);
    territory->setContinent(this); // assign the territory to the current continent
  }
}

void Continent::removeTerritory(Territory* territory) {
  std::erase(territories, territory);
  if (territory) {
    territory->setContinent(nullptr);
  }
}

// --- VALIDATION ---
bool Continent::isConnected() const {
  if (territories.empty() || territories.size() == 1) {
    return true;
  }

  // use bfs to check if all territories in a continent are connected
  std::unordered_set<Territory*> visited;
  std::queue<Territory*> queue;

  queue.push(territories[0]);
  visited.insert(territories[0]);

  while (!queue.empty()) {
    Territory* current = queue.front();
    queue.pop();

    for (Territory* adj : current->getAdjTerritories()) {
      // only check territories that belong to the current continent and haven't previously traversed
      if (adj->getContinent() == this && !visited.contains(adj)) {
        visited.insert(adj);
        queue.push(adj);
      }
    }
  }

  return visited.size() == territories.size();
}

// --- UTILITY ---
void Continent::displayInfo() const {
	std::cout << "Continent: "
            << (continentName ? *continentName : "None")
            << " (ID: " << (continentId ? *continentId : -1) << ")" << std::endl;

  std::cout << "  Territories (" << territories.size() << "): ";
	if (territories.empty()) {
		std::cout << "None";
	} else {
		for (size_t i = 0; i < territories.size(); i++) {
		  std::cout << (territories[i] ? territories[i]->getName() : "None");
			if (i < territories.size() - 1) {
				std::cout << ", ";
			}
		}
	}

	std::cout << std::endl;
}

bool Continent::operator==(const Continent& other) const {
	return continentId == other.continentId && continentName == other.continentName;
}




















// // ==================== Map Class Implementation ====================
//
// Map::Map() : mapName("Hatsune Miku") {}
// Map::Map(const std::string& name) : mapName(name) {}
//
// void Map::displayMap() const {
// 	std::cout << "\n========== Map: " << mapName << " ==========" << std::endl;
// 	std::cout << "Total Continents: " << continents.size() << std::endl;
// 	std::cout << "Total Territories: " << territories.size() << std::endl;
//
// 	std::cout << "\n--- Continents ---" << std::endl;
// 	for (const auto& continent : continents) {
// 		continent->displayInfo();
// 	}
//
// 	std::cout << "\n--- Territories ---" << std::endl;
// 	for (const auto& territory : territories) {
// 		territory->displayInfo();
// 	}
//
// 	std::cout << "\n--- Validation Results ---" << std::endl;
// 	std::cout << "========================================\n" << std::endl;
// }
//
// void Map::clear() {
// 	territories.clear();
// 	continents.clear();
// 	territoryNameMap.clear();
// 	territoryIdMap.clear();
// 	continentNameMap.clear();
// }
//
//

















// // ==================== MapLoader Class Implementation ====================
//
// MapLoader::MapLoader() {}
//
//
//
//
//
//
