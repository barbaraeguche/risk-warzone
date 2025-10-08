#include "Map.h"
#include "Player.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <unordered_set>


// ==================== Territory Class Implementation ====================

Territory::Territory() : territoryName(new std::string("")),
                         territoryId(new int(0)),
                         ownerPlayer(nullptr),
                         armies(new int(0)),
                         adjTerritories(new std::vector<Territory *>()),
                         continent(nullptr) {}

Territory::Territory(const std::string& name, int id) : territoryName(new std::string(name)),
                                                        territoryId(new int(id)),
                                                        ownerPlayer(nullptr),
                                                        armies(new int(0)),
                                                        adjTerritories(new std::vector<Territory *>()),
                                                        continent(nullptr) {}

// don't copy `adjTerritories` (map will rebuild), don't copy continent pointer
Territory::Territory(const Territory& other) : territoryName(new std::string(*other.territoryName)),
                                               territoryId(new int(*other.territoryId)),
                                               ownerPlayer(nullptr),
                                               armies(new int(*other.armies)),
                                               adjTerritories(new std::vector<Territory *>()),
                                               continent(nullptr) {}

Territory& Territory::operator=(const Territory& other) {
  if (this != &other) {
    // clean up existing
    delete territoryName;
    delete territoryId;
    delete armies;
    delete adjTerritories;

    // deep copy of primitives and containers
    territoryName = new std::string(*other.territoryName);
    territoryId = new int(*other.territoryId);
    armies = new int(*other.armies);
    adjTerritories = new std::vector<Territory*>(*other.adjTerritories);

    // shallow copy for shared references
    ownerPlayer = other.ownerPlayer;
    continent = other.continent;
  }
  return *this;
}

Territory::~Territory() {
  delete territoryName;
  delete territoryId;
  delete armies;
  delete adjTerritories;
}

// --- GETTERS ---
std::string Territory::getName() const {
  return *territoryName;
}

int Territory::getId() const {
  return *territoryId;
}

Player* Territory::getOwner() const {
  return ownerPlayer;
}

int Territory::getArmies() const {
  return *armies;
}

const std::vector<Territory *>& Territory::getAdjTerritories() const {
  return *adjTerritories;
}

Continent* Territory::getContinent() const {
  return continent;
}

// --- SETTERS ---
void Territory::setName(const std::string& name) const {
  *territoryName = name;
}

void Territory::setId(int id) const {
  *territoryId = id;
}

void Territory::setOwner(Player* owner) {
  if (!owner || ownerPlayer == owner) return;

  Player* oldOwner = ownerPlayer;
  ownerPlayer = owner;

  owner->addTerritory(this);
  if (oldOwner) {
    oldOwner->removeTerritory(this);
  }
}

void Territory::setArmies(int armyCount) const {
  *armies = armyCount;
}

void Territory::setContinent(Continent* mainContinent) {
  continent = mainContinent;
}

// --- MANAGEMENT ---
bool Territory::isAdjacentTo(Territory* territory) const {
  return std::ranges::find(*adjTerritories, territory) != adjTerritories->end();
}

void Territory::addAdjacentTerritory(Territory* territory) {
  if (territory && territory != this && !isAdjacentTo(territory)) {
    adjTerritories->push_back(territory);
  }
}

void Territory::removeAdjacentTerritory(Territory* territory) {
  std::erase(*adjTerritories, territory);
}

// --- UTILITY ---
void Territory::displayInfo() const {
  std::cout << "Territory: " << (territoryName ? *territoryName : "None")
      << " (ID: " << (territoryId ? *territoryId : -1) << ")" << std::endl;

  std::cout << "  Owner: " << (ownerPlayer ? ownerPlayer->getName() : "None")
      << ", Armies: " << (armies ? *armies : 0) << std::endl;

  std::cout << "  Continent: " << (continent ? continent->getName() : "None") << std::endl;

  std::cout << "  Adjacent to: ";
  if (adjTerritories->empty()) {
    std::cout << "None";
  } else {
    for (size_t i = 0; i < adjTerritories->size(); i++) {
      std::cout << ((*adjTerritories)[i] ? (*adjTerritories)[i]->getName() : "None");
      if (i < adjTerritories->size() - 1) {
        std::cout << ", ";
      }
    }
  }

  std::cout << std::endl;
}

bool Territory::operator==(const Territory& other) const {
  return territoryId == other.territoryId && territoryName == other.territoryName;
}

// --- STREAM INSERTION ---
std::ostream& operator<<(std::ostream& os, const Territory& territory) {
  os << "Territory["
      << (territory.territoryName ? *territory.territoryName : "<Unnamed>")
      << " (ID:" << (territory.territoryId ? *territory.territoryId : -1) << "), ";

  os << "Owner:" << (territory.ownerPlayer ? territory.ownerPlayer->getName() : "None") << ", ";
  os << "Armies:" << (territory.armies ? *territory.armies : 0) << ", ";

  os << "Continent:" << (territory.continent ? territory.continent->getName() : "None") << ", ";
  os << "Adjacent:" << territory.adjTerritories->size() << "]";
  return os;
}


// ==================== Continent Class Implementation ====================

Continent::Continent() : continentName(new std::string("")),
                         continentId(new int(0)),
                         controlValue(new int(0)),
                         territories(new std::vector<Territory *>()) {}

Continent::Continent(const std::string& name, int id, int bonus) : continentName(new std::string(name)),
                                                        continentId(new int(id)),
                                                        controlValue(new int(bonus)),
                                                        territories(new std::vector<Territory *>()) {}

// don't copy `territories` (map will rebuild)
Continent::Continent(const Continent& other) : continentName(new std::string(*other.continentName)),
                                               continentId(new int(*other.continentId)),
                                               controlValue(new int(*other.controlValue)),
                                               territories(new std::vector<Territory *>()) {}

Continent& Continent::operator=(const Continent& other) {
  if (this != &other) {
    // clean up old resources
    delete continentName;
    delete continentId;
    delete controlValue;
    delete territories;

    // deep copy primitives
    continentName = new std::string(*other.continentName);
    continentId = new int(*other.continentId);
    controlValue = new int(*other.controlValue);

    // deep copy vector (shallow copy of pointers)
    territories = new std::vector<Territory*>(*other.territories);
  }
  return *this;
}

Continent::~Continent() {
  delete continentName;
  delete continentId;
  delete controlValue;
  delete territories;
}

// --- GETTERS ---
std::string Continent::getName() const {
  return *continentName;
}

int Continent::getId() const {
  return *continentId;
}

int Continent::getControlValue() const {
  return *controlValue;
}

const std::vector<Territory *>& Continent::getTerritories() const {
  return *territories;
}

// --- SETTERS ---
void Continent::setName(const std::string& name) const {
  *continentName = name;
}

void Continent::setId(int id) const {
  *continentId = id;
}

void Continent::setControlValue(int bonus) const {
  *controlValue = bonus;
}

// --- MANAGEMENT ---
bool Continent::containsTerritory(Territory* territory) const {
  return std::ranges::find(*territories, territory) != territories->end();
}

void Continent::addTerritory(Territory* territory) {
  if (territory && !containsTerritory(territory)) {
    territories->push_back(territory);
    territory->setContinent(this); // assign the territory to the current continent
  }
}

void Continent::removeTerritory(Territory* territory) {
  std::erase(*territories, territory);
  if (territory) {
    territory->setContinent(nullptr);
  }
}

// --- VALIDATION ---
bool Continent::isConnected() const {
  if (territories->empty() || territories->size() == 1) {
    return true;
  }

  // use bfs to check if all territories in a continent are connected
  std::unordered_set<Territory *> visited;
  std::queue<Territory *> queue;

  queue.push((*territories)[0]);
  visited.insert((*territories)[0]);

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

  return visited.size() == territories->size();
}

// --- UTILITY ---
void Continent::displayInfo() const {
  std::cout << "Continent: "
      << (continentName ? *continentName : "None")
      << " (ID: " << (continentId ? *continentId : -1)
      << ", Bonus: " << (controlValue ? *controlValue : 0) << ")"
      << std::endl;

  std::cout << "  Territories (" << territories->size() << "): ";
  if (territories->empty()) {
    std::cout << "None";
  } else {
    for (size_t i = 0; i < territories->size(); i++) {
      std::cout << ((*territories)[i] ? (*territories)[i]->getName() : "None");
      if (i < territories->size() - 1) {
        std::cout << ", ";
      }
    }
  }

  std::cout << std::endl;
}

bool Continent::operator==(const Continent& other) const {
  return continentId == other.continentId && continentName == other.continentName;
}

// --- STREAM INSERTION ---
std::ostream& operator<<(std::ostream& os, const Continent& continent) {
  os << "Continent["
      << (continent.continentName ? *continent.continentName : "<Unnamed>")
      << " (ID:" << (continent.continentId ? *continent.continentId : -1) << "), "
      << "Territories:" << continent.territories->size() << "]";
  return os;
}


// ==================== Map Class Implementation ====================

Map::Map() : mapName(new std::string("Hatsune Miku")),
             territories(new std::vector<std::unique_ptr<Territory> >()),
             continents(new std::vector<std::unique_ptr<Continent> >()),
             territoryNameMap(new std::unordered_map<std::string, Territory *>()),
             territoryIdMap(new std::unordered_map<int, Territory *>()),
             continentNameMap(new std::unordered_map<std::string, Continent *>()) {}

Map::Map(const std::string& name) : mapName(new std::string(name)),
                                    territories(new std::vector<std::unique_ptr<Territory> >()),
                                    continents(new std::vector<std::unique_ptr<Continent> >()),
                                    territoryNameMap(new std::unordered_map<std::string, Territory *>()),
                                    territoryIdMap(new std::unordered_map<int, Territory *>()),
                                    continentNameMap(new std::unordered_map<std::string, Continent *>()) {}

Map::Map(const Map& other) : mapName(new std::string(*other.mapName)),
                             territories(new std::vector<std::unique_ptr<Territory> >()),
                             continents(new std::vector<std::unique_ptr<Continent> >()),
                             territoryNameMap(new std::unordered_map<std::string, Territory *>()),
                             territoryIdMap(new std::unordered_map<int, Territory *>()),
                             continentNameMap(new std::unordered_map<std::string, Continent *>()) {
  // deep copy territories
  for (const auto& territory : *other.territories) {
    auto newTerritory = std::make_unique<Territory>(*territory);
    territories->push_back(std::move(newTerritory));
  }

  // deep copy continents
  for (const auto& continent : *other.continents) {
    auto newContinent = std::make_unique<Continent>(*continent);
    continents->push_back(std::move(newContinent));
  }

  // rebuild lookup maps
  rebuildMaps();

  // now rebuild all relationships using the new pointers
  for (size_t i = 0; i < territories->size(); i++) {
    Territory* newTerritory = (*territories)[i].get();
    const Territory* oldTerritory = (*other.territories)[i].get();

    // rebuild continent assignment
    if (oldTerritory->getContinent()) {
      Continent* newContinent = (*continentNameMap)[oldTerritory->getContinent()->getName()];
      newContinent->addTerritory(newTerritory);
    }

    // rebuild adjacencies
    for (const Territory* oldAdj : oldTerritory->getAdjTerritories()) {
      Territory* newAdj = (*territoryNameMap)[oldAdj->getName()];
      if (newAdj && !newTerritory->isAdjacentTo(newAdj)) {
        newTerritory->addAdjacentTerritory(newAdj);
      }
    }
  }
}

Map& Map::operator=(const Map& other) {
  if (this != &other) {
    delete mapName;
    delete territories;
    delete continents;
    delete territoryNameMap;
    delete territoryIdMap;
    delete continentNameMap;

    mapName = new std::string(*other.mapName);
    territories = new std::vector<std::unique_ptr<Territory>>();
    for (const auto& t : *other.territories) {
      territories->push_back(std::make_unique<Territory>(*t));
    }

    continents = new std::vector<std::unique_ptr<Continent>>();
    for (const auto& c : *other.continents) {
      continents->push_back(std::make_unique<Continent>(*c));
    }

    territoryNameMap = new std::unordered_map<std::string, Territory*>(*other.territoryNameMap);
    territoryIdMap = new std::unordered_map<int, Territory*>(*other.territoryIdMap);
    continentNameMap = new std::unordered_map<std::string, Continent*>(*other.continentNameMap);
  }
  return *this;
}

Map::~Map() {
  delete mapName;
  delete territories;
  delete continents;
  delete territoryNameMap;
  delete territoryIdMap;
  delete continentNameMap;
}

// --- GETTERS ---
std::string Map::getMapName() const {
  return *mapName;
}

const std::vector<std::unique_ptr<Territory> >& Map::getTerritories() const {
  return *territories;
}

const std::vector<std::unique_ptr<Continent> >& Map::getContinents() const {
  return *continents;
}

// --- SETTERS ---
void Map::setMapName(const std::string& name) const {
  *mapName = name;
}

// --- TERRITORY MANAGEMENT ---
Territory* Map::addTerritory(const std::string& name, int id) {
  auto territory = std::make_unique<Territory>(name, id);
  Territory* territoryPtr = territory.get();

  territories->push_back(std::move(territory));
  (*territoryNameMap)[name] = territoryPtr;
  (*territoryIdMap)[id] = territoryPtr;

  return territoryPtr;
}

Territory* Map::getTerritory(const std::string& name) const {
  const auto it = territoryNameMap->find(name);
  return (it != territoryNameMap->end()) ? it->second : nullptr;
}

Territory* Map::getTerritory(int id) const {
  const auto it = territoryIdMap->find(id);
  return (it != territoryIdMap->end()) ? it->second : nullptr;
}

bool Map::removeTerritory(const std::string& name) {
  Territory* territory = getTerritory(name);
  if (!territory) {
    return false;
  }

  // remove the continent
  if (territory->getContinent()) {
    territory->getContinent()->removeTerritory(territory);
  }

  // remove adjacencies
  for (Territory* adj : territory->getAdjTerritories()) {
    adj->removeAdjacentTerritory(territory);
  }

  // remove from maps
  territoryNameMap->erase(name);
  territoryIdMap->erase(territory->getId());

  // remove from vector
  std::erase_if(
    *territories,
    [territory](const std::unique_ptr<Territory>& t) {
      return t.get() == territory;
    });

  return true;
}

// --- CONTINENT MANAGEMENT ---
Continent* Map::addContinent(const std::string& name, int id, int bonus) {
  auto continent = std::make_unique<Continent>(name, id, bonus);
  Continent* continentPtr = continent.get();

  continents->push_back(std::move(continent));
  (*continentNameMap)[name] = continentPtr;

  return continentPtr;
}

Continent* Map::getContinent(const std::string& name) const {
  const auto it = continentNameMap->find(name);
  return (it != continentNameMap->end()) ? it->second : nullptr;
}

bool Map::removeContinent(const std::string& name) {
  Continent* continent = getContinent(name);
  if (!continent) {
    return false;
  }

  // remove continent reference from all territories
  for (Territory* territory : continent->getTerritories()) {
    territory->setContinent(nullptr);
  }

  // remove from map
  continentNameMap->erase(name);

  // remove from vector
  std::erase_if(
    *continents,
    [continent](const std::unique_ptr<Continent>& c) {
      return c.get() == continent;
    });

  return true;
}

// --- GRAPH OPERATIONS ---
void Map::addAdjacency(const std::string& territory1, const std::string& territory2) {
  Territory* t1 = getTerritory(territory1);
  Territory* t2 = getTerritory(territory2);

  if (t1 && t2) {
    t1->addAdjacentTerritory(t2);
    t2->addAdjacentTerritory(t1);
  }
}

void Map::addAdjacency(Territory* territory1, Territory* territory2) {
  if (territory1 && territory2) {
    territory1->addAdjacentTerritory(territory2);
    territory2->addAdjacentTerritory(territory1);
  }
}

// --- VALIDATION ---
bool Map::validate() const {
  return isConnectedGraph() && areContinentsConnected() && eachTerritoryBelongsToOneContinent();
}

bool Map::isConnectedGraph() const {
  if (territories->empty()) {
    return true;
  }

  std::unordered_set<Territory *> visited;
  dfsVisit((*territories)[0].get(), visited);

  return visited.size() == territories->size();
}

bool Map::areContinentsConnected() const {
  for (const auto& continent : *continents) {
    if (!continent->isConnected()) {
      return false;
    }
  }
  return true;
}

bool Map::eachTerritoryBelongsToOneContinent() const {
  for (const auto& territory : *territories) {
    if (!territory->getContinent()) {
      return false; // territory belongs to no continent
    }
  }

  return true; // no territory belongs to multiple continents
}

// --- UTILITY ---
void Map::displayMap() const {
  std::cout << "\n========== Map: " << *mapName << " ==========" << std::endl;
  std::cout << "Total Continents: " << continents->size() << std::endl;
  std::cout << "Total Territories: " << territories->size() << std::endl;

  std::cout << "\n--- Continents ---" << std::endl;
  for (const auto& continent : *continents) {
    continent->displayInfo();
  }

  std::cout << "\n--- Territories ---" << std::endl;
  for (const auto& territory : *territories) {
    territory->displayInfo();
  }

  std::cout << "\n--- Validation Results ---" << std::endl;
  std::cout << "  Connected Graph: " << (isConnectedGraph() ? "YES" : "NO") << std::endl;
  std::cout << "  Continents Connected: " << (areContinentsConnected() ? "YES" : "NO") << std::endl;
  std::cout << "  Each Territory in One Continent: " << (eachTerritoryBelongsToOneContinent() ? "YES" : "NO") <<
      std::endl;
  std::cout << "  Overall Valid: " << (validate() ? "YES" : "NO") << std::endl;
  std::cout << "========================================\n" << std::endl;
}

void Map::clear() {
  territories->clear();
  continents->clear();
  territoryNameMap->clear();
  territoryIdMap->clear();
  continentNameMap->clear();
}

int Map::getNumberOfTerritories() const {
  return static_cast<int>(territories->size());
}

int Map::getNumberOfContinents() const {
  return static_cast<int>(continents->size());
}

// --- HELPERS ---
void Map::dfsVisit(Territory* territory, std::unordered_set<Territory *>& visited) {
  visited.insert(territory);

  for (Territory* adj : territory->getAdjTerritories()) {
    if (!visited.contains(adj)) {
      dfsVisit(adj, visited);
    }
  }
}

void Map::rebuildMaps() {
  territoryNameMap->clear();
  territoryIdMap->clear();
  continentNameMap->clear();

  for (const auto& territory : *territories) {
    (*territoryNameMap)[territory->getName()] = territory.get();
    (*territoryIdMap)[territory->getId()] = territory.get();
  }
  for (const auto& continent : *continents) {
    (*continentNameMap)[continent->getName()] = continent.get();
  }
}

// --- STREAM INSERTION ---
std::ostream& operator<<(std::ostream& os, const Map& map) {
  os << "Map["
      << (map.mapName ? *map.mapName : "<Unnamed>") << ", "
      << "Territories:" << map.territories->size() << ", "
      << "Continents:" << map.continents->size() << ", "
      << "Valid:" << (map.validate() ? "YES" : "NO") << "]";
  return os;
}


// ==================== MapLoader Class Implementation ====================

MapLoader::MapLoader() : currentState(ParseState::NONE),
                         territoryAdjacencies(new std::unordered_map<std::string, std::vector<std::string> >()) {}

MapLoader::MapLoader(const MapLoader& other) : currentState(other.currentState),
                                               territoryAdjacencies(nullptr) {}

MapLoader& MapLoader::operator=(const MapLoader& other) {
  if (this != &other) {
    currentState = other.currentState;
  }
  return *this;
}

MapLoader::~MapLoader() {
  delete territoryAdjacencies;
};

// --- MAP LOADING ---
std::unique_ptr<Map> MapLoader::loadMap(const std::string& filename) {
  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cout << "Error: Cannot open file " << filename << std::endl;
    return nullptr;
  }

  auto map = std::make_unique<Map>(filename);
  currentState = ParseState::NONE;
  territoryAdjacencies->clear();

  std::string line;
  bool hasValidContent = false;
  bool hasContinents = false;
  bool hasTerritories = false;

  while (std::getline(file, line)) {
    line = trim(line);
    if (line.empty() || line[0] == ';') {
      continue; // skip empty lines and comments
    }

    // check for section headers
    if (line == "[Map]" || line == "[map]") {
      currentState = ParseState::MAP_INFO;
      hasValidContent = true;
      continue;
    } else if (line == "[Continents]" || line == "[continents]") {
      currentState = ParseState::CONTINENTS;
      hasValidContent = true;
      hasContinents = true;
      continue;
    } else if (line == "[Territories]" || line == "[territories]") {
      currentState = ParseState::TERRITORIES;
      hasValidContent = true;
      hasTerritories = true;
      continue;
    }

    // parse content based on current state
    bool parseSuccess = false;
    switch (currentState) {
      case ParseState::MAP_INFO:
        parseSuccess = parseMapSection(map.get(), line);
        break;
      case ParseState::CONTINENTS:
        parseSuccess = parseContinentSection(map.get(), line);
        break;
      case ParseState::TERRITORIES:
        parseSuccess = parseTerritorySection(map.get(), line);
        break;
      default:
        continue;
    }

    if (!parseSuccess && currentState != ParseState::NONE) {
      std::cout << "Warning: Could not parse line: " << line << std::endl;
    }
  }

  // close the file
  file.close();

  if (!hasValidContent || !hasContinents || !hasTerritories) {
    std::cout << "Error: File does not contain valid map format (missing required sections)" << std::endl;
    return nullptr;
  }

  if (map->getNumberOfTerritories() == 0) {
    std::cout << "Error: Map has no territories" << std::endl;
    return nullptr;
  }

  if (map->getNumberOfContinents() == 0) {
    std::cout << "Error: Map has no continents" << std::endl;
    return nullptr;
  }

  // link territory adjacencies
  for (const auto& territory : map->getTerritories()) {
    const auto it = territoryAdjacencies->find(territory->getName());

    if (it != territoryAdjacencies->end()) {
      linkTerritoryAdjacencies(map.get(), territory.get(), it->second);
    }
  }

  return map;
}

bool MapLoader::canReadFile(const std::string& filename) {
  const std::ifstream file(filename);
  return file.is_open();
}

// --- HELPERS ---
std::string MapLoader::trim(const std::string& str) {
  const size_t first = str.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    return "";
  }

  const size_t last = str.find_last_not_of(" \t\r\n");
  return str.substr(first, (last - first + 1));
}

std::vector<std::string> MapLoader::split(const std::string& str, char delimiter) {
  std::vector<std::string> tokens;
  std::istringstream iss(str);
  std::string token;

  while (std::getline(iss, token, delimiter)) {
    std::string trimmed = trim(token);
    if (!trimmed.empty()) {
      tokens.push_back(trimmed);
    }
  }

  return tokens;
}

bool MapLoader::parseMapSection(const Map* map, const std::string& line) {
  const auto parts = split(line, '=');
  if (parts.size() != 2) {
    return false;
  }

  if (parts[0] == "name" || parts[0] == "Name") {
    map->setMapName(parts[1]);
  }
  return true;
}

bool MapLoader::parseContinentSection(Map* map, const std::string& line) {
  const auto parts = split(line, '=');
  if (parts.size() != 2) {
    return false;
  }

  const std::string& continentName = parts[0];
  int controlValue = 0;

  try {
    controlValue = std::stoi(parts[1]);  // Parse the bonus value
  } catch (...) {
    std::cout << "Warning: Invalid control value for continent " << continentName << std::endl;
    return false;
  }

  static int continentId = 1;
  map->addContinent(continentName, continentId++, controlValue);
  return true;
}

bool MapLoader::parseTerritorySection(Map* map, const std::string& line) {
  const auto parts = split(line, ',');
  if (parts.size() < 4) {
    return false; // need at least: name, x, y, continent
  }

  const std::string& territoryName = parts[0];
  if (territoryName.empty()) {
    return false;
  }

  // parts[1] and parts[2] are (x,y) coordinates (not used in this implementation)
  const std::string& continentName = parts[3];
  if (continentName.empty()) {
    std::cout << "Warning: Territory '" << territoryName << "' has no continent assignment" << std::endl;
    return false;
  }

  // get or create territory
  Territory* territory = map->getTerritory(territoryName);
  if (!territory) {
    static int territoryId = 1;
    territory = map->addTerritory(territoryName, territoryId++);
  }

  // assign to continent
  if (Continent* continent = map->getContinent(continentName)) {
    continent->addTerritory(territory);
  } else {
    std::cout << "Warning: Continent '" << continentName
        << "' not found for territory '" << territoryName << "'" << std::endl;
    return false;
  }

  // store adjacent territories for later linking
  std::vector<std::string> adjacentNames;
  for (size_t i = 4; i < parts.size(); i++) {
    if (!parts[i].empty()) {
      adjacentNames.push_back(parts[i]);
    }
  }

  if (!adjacentNames.empty()) {
    (*territoryAdjacencies)[territoryName] = adjacentNames;
  }

  return true;
}

void MapLoader::linkTerritoryAdjacencies(const Map* map, Territory* territory,
                                         const std::vector<std::string>& adjacentNames) {
  for (const std::string& adjacentName : adjacentNames) {
    if (Territory* adj = map->getTerritory(adjacentName)) {
      territory->addAdjacentTerritory(adj);
    } else {
      std::cout << "Warning: Adjacent territory '" << adjacentName
          << "' not found for territory '" << territory->getName() << "'" << std::endl;
    }
  }
}

// --- UTILITY ---
bool MapLoader::operator==(MapLoader* other) const {
  if (this == other) return true;
  if (!other) return false;
  return currentState == other->currentState;
}

// --- STREAM INSERTION ---
std::ostream& operator<<(std::ostream& os, const MapLoader& mapLoader) {
  os << "MapLoader[Current State: ";
  switch (mapLoader.currentState) {
    case MapLoader::ParseState::NONE: os << "NONE";
      break;
    case MapLoader::ParseState::MAP_INFO: os << "MAP_INFO";
      break;
    case MapLoader::ParseState::CONTINENTS: os << "CONTINENTS";
      break;
    case MapLoader::ParseState::TERRITORIES: os << "TERRITORIES";
      break;
  }
  os << "]";
  return os;
}
