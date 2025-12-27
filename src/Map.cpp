#include "Map.h"
#include "GameEngine.h"
#include "Player.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <ranges>
#include <sstream>
#include <unordered_set>


// ==================== Territory Class Implementation ====================
Territory::Territory() :
  name(new std::string("")),
  id(new int(0)),
  owner(nullptr),
  armies(new int(0)),
  adjTerritories(new std::vector<Territory*>()),
  continent(nullptr) {}

Territory::Territory(const std::string& name, int id) :
  name(new std::string(name)),
  id(new int(id)),
  owner(nullptr),
  armies(new int(0)),
  adjTerritories(new std::vector<Territory*>()),
  continent(nullptr) {}

Territory::Territory(const Territory& other) :
  name(new std::string(*other.name)),
  id(new int(*other.id)),
  owner(nullptr),
  armies(new int(*other.armies)),
  adjTerritories(new std::vector<Territory*>()),
  continent(nullptr) {}

Territory& Territory::operator=(const Territory& other) {
  if (this != &other) {
    delete name;
    delete id;
    delete armies;
    delete adjTerritories;

    // deep copy primitives and container
    name = new std::string(*other.name);
    id = new int(*other.id);
    armies = new int(*other.armies);
    adjTerritories = new std::vector(*other.adjTerritories);

    // shallow copy shared references
    owner = other.owner;
    continent = other.continent;
  }
  return *this;
}

Territory::~Territory() {
  delete name;
  delete id;
  delete armies;
  delete adjTerritories;
}

// --- GETTERS ---
std::string Territory::getName() const {
  return *name;
}

int Territory::getId() const {
  return *id;
}

Player* Territory::getOwner() const {
  return owner;
}

int Territory::getArmies() const {
  return *armies;
}

const std::vector<Territory*>& Territory::getAdjTerritories() const {
  return *adjTerritories;
}

Continent* Territory::getContinent() const {
  return continent;
}

// --- SETTERS ---
void Territory::setName(const std::string& newName) {
  *name = newName;
}

void Territory::setId(int newId) {
  *id = newId;
}

void Territory::setOwner(Player* newOwner) {
  if (!newOwner || owner == newOwner) return;

  Player* oldOwner = owner;
  owner = newOwner;

  // handle territory change
  newOwner->addTerritory(this);
  if (oldOwner) {
    oldOwner->removeTerritory(this);
  }
}

void Territory::setArmies(int newArmies) {
  *armies = newArmies;
}

void Territory::setContinent(Continent* newContinent) {
  continent = newContinent;
}

// --- MANAGEMENT ---
bool Territory::isAdjacentTo(const Territory* terr) const {
  return std::ranges::any_of(*adjTerritories, [terr](const Territory* t) {
    return t == terr;
  });
}

void Territory::addAdjTerritory(Territory* terr) {
  if (!terr || terr == this) return;

  // ensure no duplicates
  if (!isAdjacentTo(terr)) {
    adjTerritories->push_back(terr);
  }
}

void Territory::removeAdjTerritory(Territory* terr) {
  std::erase(*adjTerritories, terr);
}

// --- UTILITY ---
void Territory::displayInfo() const {
  std::cout << "Territory: " << (name ? *name : "<Unnamed>")
            << " (ID: " << (id ? *id : -1) << ")" << std::endl;

  std::cout << "  Owner: " << (owner ? owner->getName() : "None")
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
  return id == other.id && name == other.name;
}

// --- STREAM INSERTION OPERATOR ---
std::ostream& operator<<(std::ostream& os, const Territory& terr) {
  os << "Territory["
     << (terr.name ? *terr.name : "<Unnamed>")
     << " (ID:" << (terr.id ? *terr.id : -1) << "), ";

  os << "Owner:" << (terr.owner ? terr.owner->getName() : "None") << ", ";
  os << "Armies:" << (terr.armies ? *terr.armies : 0) << ", ";

  os << "Continent:" << (terr.continent ? terr.continent->getName() : "None") << ", ";
  os << "Adjacent:" << terr.adjTerritories->size() << "]";
  return os;
}


// ==================== Continent Class Implementation ====================
Continent::Continent() :
  name(new std::string("")),
  id(new int(0)),
  bonus(new int(0)),
  territories(new std::vector<Territory*>()) {}

Continent::Continent(const std::string& name, int id, int bonus) :
  name(new std::string(name)),
  id(new int(id)),
  bonus(new int(bonus)),
  territories(new std::vector<Territory*>()) {}

Continent::Continent(const Continent& other) :
  name(new std::string(*other.name)),
  id(new int(*other.id)),
  bonus(new int(*other.bonus)),
  territories(new std::vector<Territory*>()) {}

Continent& Continent::operator=(const Continent& other) {
  if (this != &other) {
    delete name;
    delete id;
    delete bonus;
    delete territories;

    // deep copy primitives and container
    name = new std::string(*other.name);
    id = new int(*other.id);
    bonus = new int(*other.bonus);
    territories = new std::vector(*other.territories);
  }
  return *this;
}

Continent::~Continent() {
  delete name;
  delete id;
  delete bonus;
  delete territories;
}

// --- GETTERS ---
std::string Continent::getName() const {
  return *name;
}

int Continent::getId() const {
  return *id;
}

int Continent::getBonus() const {
  return *bonus;
}

const std::vector<Territory*>& Continent::getTerritories() const {
  return *territories;
}

// --- SETTERS ---
void Continent::setName(const std::string& newName) {
  *name = newName;
}

void Continent::setId(int newId) {
  *id = newId;
}

void Continent::setBonus(int newBonus) {
  *bonus = newBonus;
}

// --- MANAGEMENT ---
bool Continent::containsTerritory(const Territory* terr) const {
  return std::ranges::any_of(*territories, [terr](const Territory* t) {
    return t == terr;
  });
}

void Continent::addTerritory(Territory* terr) {
  // ensure no duplicates
  if (!terr || containsTerritory(terr)) return;

  territories->push_back(terr);
  terr->setContinent(this); // assign the territory to the current continent
}

void Continent::removeTerritory(Territory* terr) {
  if (terr) terr->setContinent(nullptr);
  std::erase(*territories, terr);
}

// --- VALIDATION ---
bool Continent::isConnected() const {
  if (territories->empty() || territories->size() == 1) return true;

  // use bfs to check if all territories in a continent are connected
  std::unordered_set visited{(*territories)[0]};
  std::queue<Territory*> queue;
  queue.push((*territories)[0]);

  while (!queue.empty()) {
    const auto terr = queue.front();
    queue.pop();

    // only check territories that belong to the current continent and haven't previously traversed
    for (Territory* adj : terr->getAdjTerritories()) {
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
  std::cout << "Continent: " << (name ? *name : "<Unnamed>")
            << " (ID: " << (id ? *id : -1)
            << ", Bonus: " << (bonus ? *bonus : 0) << ")" << std::endl;

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
  return id == other.id && name == other.name;
}

// --- STREAM INSERTION OPERATOR ---
std::ostream& operator<<(std::ostream& os, const Continent& cont) {
  os << "Continent[" << (cont.name ? *cont.name : "<Unnamed>")
     << " (ID:" << (cont.id ? *cont.id : -1) << "),"
     << " Territories:" << cont.territories->size() << "]";
  return os;
}


// ==================== Map Class Implementation ====================
Map::Map() :
  name(new std::string("Warzone")),
  territories(new std::vector<std::unique_ptr<Territory>>()),
  continents(new std::vector<std::unique_ptr<Continent>>()),
  territoryNameMap(new std::unordered_map<std::string, Territory*>()),
  territoryIdMap(new std::unordered_map<int, Territory*>()),
  continentNameMap(new std::unordered_map<std::string, Continent*>()) {}

Map::Map(const std::string& name) :
  name(new std::string(name)),
  territories(new std::vector<std::unique_ptr<Territory>>()),
  continents(new std::vector<std::unique_ptr<Continent>>()),
  territoryNameMap(new std::unordered_map<std::string, Territory*>()),
  territoryIdMap(new std::unordered_map<int, Territory*>()),
  continentNameMap(new std::unordered_map<std::string, Continent*>()) {}

Map::Map(const Map& other) :
  name(new std::string(*other.name)),
  territories(new std::vector<std::unique_ptr<Territory>>()),
  continents(new std::vector<std::unique_ptr<Continent>>()),
  territoryNameMap(new std::unordered_map<std::string, Territory*>()),
  territoryIdMap(new std::unordered_map<int, Territory*>()),
  continentNameMap(new std::unordered_map<std::string, Continent*>()) {

  // deep copy territories
  for (const auto& terr : *other.territories) {
    auto newTerr = std::make_unique<Territory>(*terr);
    territories->push_back(std::move(newTerr));
  }

  // deep copy continents
  for (const auto& cont : *other.continents) {
    auto newCont = std::make_unique<Continent>(*cont);
    continents->push_back(std::move(newCont));
  }

  // rebuild lookup maps
  rebuildMaps();

  // now rebuild all relationships using the new pointers
  for (size_t i = 0; i < territories->size(); i++) {
    Territory* newTerr = (*territories)[i].get();
    const Territory* oldTerr = (*other.territories)[i].get();

    // rebuild continent assignment
    if (oldTerr->getContinent()) {
      Continent* newCont = (*continentNameMap)[oldTerr->getContinent()->getName()];
      newCont->addTerritory(newTerr);
    }

    // rebuild adjacency
    for (const Territory* adj : oldTerr->getAdjTerritories()) {
      Territory* newAdjTerr = (*territoryNameMap)[adj->getName()];

      if (newAdjTerr && !newTerr->isAdjacentTo(newAdjTerr)) {
        newTerr->addAdjTerritory(newAdjTerr);
      }
    }
  }
}

Map& Map::operator=(const Map& other) {
  if (this != &other) {
    delete name;
    delete territories;
    delete continents;
    delete territoryNameMap;
    delete territoryIdMap;
    delete continentNameMap;

    name = new std::string(*other.name);
    territories = new std::vector<std::unique_ptr<Territory>>();
    for (const auto& terr : *other.territories) {
      territories->push_back(std::make_unique<Territory>(*terr));
    }

    continents = new std::vector<std::unique_ptr<Continent>>();
    for (const auto& cont : *other.continents) {
      continents->push_back(std::make_unique<Continent>(*cont));
    }

    territoryNameMap = new std::unordered_map(*other.territoryNameMap);
    territoryIdMap = new std::unordered_map(*other.territoryIdMap);
    continentNameMap = new std::unordered_map(*other.continentNameMap);
  }
  return *this;
}

Map::~Map() {
  delete name;
  delete territories;
  delete continents;
  delete territoryNameMap;
  delete territoryIdMap;
  delete continentNameMap;
}

// --- GETTERS ---
std::string Map::getName() const {
  return *name;
}

const std::vector<std::unique_ptr<Territory>>& Map::getTerritories() const {
  return *territories;
}

const std::vector<std::unique_ptr<Continent>>& Map::getContinents() const {
  return *continents;
}

// --- SETTERS ---
void Map::setName(const std::string& newName) const {
  *name = newName;
}

// --- TERRITORY MANAGEMENT ---
Territory* Map::addTerritory(const std::string& terrName, int id) {
  auto territory = std::make_unique<Territory>(terrName, id);
  territories->push_back(std::move(territory));

  Territory* territoryPtr = territories->back().get();
  (*territoryNameMap)[terrName] = territoryPtr;
  (*territoryIdMap)[id] = territoryPtr;

  return territoryPtr;
}

Territory* Map::getTerritory(const std::string& terrName) const {
  const auto it = territoryNameMap->find(terrName);
  return it != territoryNameMap->end() ? it->second : nullptr;
}

Territory* Map::getTerritory(int id) const {
  const auto it = territoryIdMap->find(id);
  return it != territoryIdMap->end() ? it->second : nullptr;
}

bool Map::removeTerritory(const std::string& terrName) {
  Territory* territory = getTerritory(terrName);
  if (!territory) return false;

  // remove the continent
  if (territory->getContinent()) {
    territory->getContinent()->removeTerritory(territory);
  }

  // disconnect from adjacent territories
  for (Territory* adj : territory->getAdjTerritories()) {
    adj->removeAdjTerritory(territory);
  }

  // remove from maps
  territoryNameMap->erase(terrName);
  territoryIdMap->erase(territory->getId());

  // remove from vector
  std::erase_if(*territories, [territory](const std::unique_ptr<Territory>& t) {
    return t.get() == territory;
  });

  return true;
}

// --- CONTINENT MANAGEMENT ---
Continent* Map::addContinent(const std::string& contName, int id, int bonus) {
  auto continent = std::make_unique<Continent>(contName, id, bonus);
  continents->push_back(std::move(continent));

  Continent* continentPtr = continents->back().get();
  (*continentNameMap)[contName] = continentPtr;

  return continentPtr;
}

Continent* Map::getContinent(const std::string& contName) const {
  const auto it = continentNameMap->find(contName);
  return it != continentNameMap->end() ? it->second : nullptr;
}

bool Map::removeContinent(const std::string& contName) {
  Continent* continent = getContinent(contName);
  if (!continent) return false;

  // remove continent reference from all territories
  for (Territory* territory : continent->getTerritories()) {
    territory->setContinent(nullptr);
  }

  // remove from map
  continentNameMap->erase(contName);

  // remove from vector
  std::erase_if(*continents, [continent](const std::unique_ptr<Continent>& c) {
    return c.get() == continent;
  });

  return true;
}

// --- GRAPH OPERATIONS ---
void Map::addAdjacency(const std::string& terr1, const std::string& terr2) {
  Territory* t1 = getTerritory(terr1);
  Territory* t2 = getTerritory(terr2);

  if (t1 && t2) {
    t1->addAdjTerritory(t2);
    t2->addAdjTerritory(t1);
  }
}

void Map::addAdjacency(Territory* terr1, Territory* terr2) {
  if (terr1 && terr2) {
    terr1->addAdjTerritory(terr2);
    terr2->addAdjTerritory(terr1);
  }
}

// --- VALIDATION ---
bool Map::validate() const {
  return isConnectedGraph() && areContinentsConnected() && eachTerritoryBelongsToOneContinent();
}

bool Map::isConnectedGraph() const {
  if (continents->empty()) return true;

  // for every continent, check if all territories are connected
  for (const auto& cont : *continents) {
    if (!cont->isConnected()) return false;
  }

  // check that continents itself are connected
  return areContinentsConnected();
}

bool Map::areContinentsConnected() const {
  if (continents->empty() || continents->size() == 1) return true;

  // use bfs to check if all continents in the map are connected
  std::unordered_set<Continent*> visitedContinents;
  std::unordered_set<Territory*> visitedTerritories;
  std::queue<Territory*> queue;

  // start from the first territory
  Territory* first = (*territories)[0].get();
  queue.push(first);
  visitedTerritories.insert(first);

  if (first->getContinent()) visitedContinents.insert(first->getContinent());

  while (!queue.empty()) {
    const Territory* terr = queue.front();
    queue.pop();

    // visit all adjacent territories
    for (Territory* adj : terr->getAdjTerritories()) {
      if (!visitedTerritories.contains(adj)) {
        visitedTerritories.insert(adj);
        queue.push(adj);

        // track which continent this territory belongs to
        if (adj->getContinent()) {
          visitedContinents.insert(adj->getContinent());
        }
      }
    }
  }

  // check if we visited all continents
  return visitedContinents.size() == continents->size();
}

bool Map::eachTerritoryBelongsToOneContinent() const {
  std::unordered_map<Territory*, int> territoryCountMap;

  for (const auto& cont : *continents) {
    for (Territory* terr : cont->getTerritories()) {
      territoryCountMap[terr] += 1;
    }
  }

  // 1. all territories must be accounted for (none belong to zero continents)
  if (territoryCountMap.size() != territories->size()) return false;

  // 2. no territory can belong to multiple continents
  for (const auto& cnt : territoryCountMap | std::views::values) {
    if (cnt > 1) return false;
  }

  return true;
}

// --- UTILITY ---
void Map::displayMap() const {
  std::cout << "\n========== Map: " << *name << " ==========" << std::endl;
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
  std::cout << "  Each Territory in One Continent: " << (eachTerritoryBelongsToOneContinent() ? "YES" : "NO")
            << std::endl;
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
  return territories->size();
}

int Map::getNumberOfContinents() const {
  return continents->size();
}

// --- HELPERS ---
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

// --- STREAM INSERTION OPERATOR ---
std::ostream& operator<<(std::ostream& os, const Map& map) {
  os << "Map["
      << (map.name ? *map.name : "<Unnamed>") << ", "
      << "Territories:" << map.territories->size() << ", "
      << "Continents:" << map.continents->size() << ", "
      << "Valid:" << (map.validate() ? "YES" : "NO") << "]";
  return os;
}


// ==================== MapLoader Class Implementation ====================
MapLoader::MapLoader() :
  currentState(ParseState::NONE),
  territoryAdjacency(new std::unordered_map<std::string, std::vector<std::string>>()) {}

MapLoader::MapLoader(const MapLoader& other) :
  currentState(other.currentState),
  territoryAdjacency(nullptr) {}

MapLoader& MapLoader::operator=(const MapLoader& other) {
  if (this != &other) currentState = other.currentState;
  return *this;
}

MapLoader::~MapLoader() {
  delete territoryAdjacency;
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
  territoryAdjacency->clear();

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
      hasContinents = true;
      hasValidContent &= true;
      continue;
    } else if (line == "[Territories]" || line == "[territories]") {
      currentState = ParseState::TERRITORIES;
      hasTerritories = true;
      hasValidContent &= hasContinents;
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

  // link territory adjacency
  for (const auto& territory : map->getTerritories()) {
    const auto it = territoryAdjacency->find(territory->getName());

    if (it != territoryAdjacency->end()) {
      linkTerritoryAdjacency(map.get(), territory.get(), it->second);
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

  if (parts[0] == "image" || parts[0] == "Image") {
    map->setName(parts[1]);
  }
  return true;
}

bool MapLoader::parseContinentSection(Map* map, const std::string& line) {
  const auto parts = split(line, '=');
  if (parts.size() != 2) {
    return false;
  }

  const std::string& continentName = parts[0];
  int bonus = 0;

  try {
    bonus = std::stoi(parts[1]);  // Parse the bonus value
  } catch (...) {
    std::cout << "Warning: Invalid control value for continent " << continentName << std::endl;
    return false;
  }

  static int continentId = 1;
  map->addContinent(continentName, continentId++, bonus);
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

  // parts[1:2] (included) are (x,y) coordinates (not used in this implementation)
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
    std::cout << "Warning: Continent '" << continentName << "' not found for territory '"
              << territoryName << "'" << std::endl;
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
    (*territoryAdjacency)[territoryName] = adjacentNames;
  }

  return true;
}

void MapLoader::linkTerritoryAdjacency(const Map* map, Territory* territory,
                                         const std::vector<std::string>& adjacentNames) {
  for (const std::string& adjacentName : adjacentNames) {
    if (Territory* adj = map->getTerritory(adjacentName)) {
      territory->addAdjTerritory(adj);
    } else {
      std::cout << "Warning: Adjacent territory '" << adjacentName
          << "' not found for territory '" << territory->getName() << "'" << std::endl;
    }
  }
}

// --- UTILITY ---
bool MapLoader::operator==(const MapLoader* other) const {
  if (this == other) return true;
  if (!other) return false;
  return currentState == other->currentState;
}

// --- STREAM INSERTION OPERATOR ---
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
  os << "]" << std::endl;
  return os;
}
