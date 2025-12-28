#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Player;
class Continent;

/**
 * The Territory class represents a node in the map graph.
 */
class Territory {
private:
  std::string* name;
  int* id;
  Player* owner;                           // player who owns this territory
  int* armies;                             // armies owned by the player
  std::vector<Territory*>* adjTerritories;
  Continent* continent;                    // the continent which the territory belongs to

public:
  Territory();
  Territory(const std::string& name, int id);
  Territory(const Territory& other); // copy constructor
  Territory& operator=(const Territory& other); // assignment operator
  ~Territory(); // destructor

  // getters
  std::string getName() const;
  int getId() const;
  Player* getOwner() const;
  int getArmies() const;
  const std::vector<Territory*>& getAdjTerritories() const;
  Continent* getContinent() const;

  // setters
  void setName(const std::string& newName);
  void setId(int newId);
  void setOwner(Player* newOwner);
  void setArmies(int newArmies);
  void setContinent(Continent* newContinent);

  // management
  bool isAdjacentTo(const Territory* terr) const;
  void addAdjTerritory(Territory* terr);
  void removeAdjTerritory(Territory* terr);

  // utility
  void displayInfo() const;
  bool operator==(const Territory& other) const; // equality operator

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const Territory& terr);
};


/**
 * The Continent class represents a connected subgraph of territories
 */
class Continent {
private:
  std::string* name;
  int* id;
  int* bonus;
  std::vector<Territory*>* territories; // territories within this continent

public:
  Continent();
  Continent(const std::string& name, int id, int bonus);
  Continent(const Continent& other); // copy constructor
  Continent& operator=(const Continent& other); // assignment operator
  ~Continent(); // destructor

  // getters
  std::string getName() const;
  int getId() const;
  int getBonus() const;
  const std::vector<Territory*>& getTerritories() const;

  // setters
  void setName(const std::string& newName);
  void setId(int newId);
  void setBonus(int newBonus);

  // territory management
  bool containsTerritory(const Territory* terr) const;
  void addTerritory(Territory* terr);
  void removeTerritory(Territory* terr);

  // validation
  bool isConnected() const;

  // utility
  void displayInfo() const;
  bool operator==(const Continent& other) const;

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const Continent& cont);
};


/**
 * Map class representing the entire game map as a connected graph
 */
class Map {
private:
  std::string* name;
  std::vector<std::unique_ptr<Territory>>* territories;
  std::vector<std::unique_ptr<Continent>>* continents;
  std::unordered_map<std::string, Territory*>* territoryNameMap;
  std::unordered_map<int, Territory*>* territoryIdMap;
  std::unordered_map<std::string, Continent*>* continentNameMap;

public:
  Map();
  Map(const std::string& name);
  Map(const Map& other); // copy constructor
  Map& operator=(const Map& other); // assignment operator
  ~Map(); // destructor

  // getters
  std::string getName() const;
  const std::vector<std::unique_ptr<Territory>>& getTerritories() const;
  const std::vector<std::unique_ptr<Continent>>& getContinents() const;

  // setters
  void setName(const std::string& newName) const;

  // territory management
  Territory* addTerritory(const std::string& terrName, int id);
  Territory* getTerritory(const std::string& terrName) const;
  Territory* getTerritory(int id) const;
  bool removeTerritory(const std::string& terrName);

  // continent management
  Continent* addContinent(const std::string& contName, int id, int bonus);
  Continent* getContinent(const std::string& contName) const;
  bool removeContinent(const std::string& contName);

  // graph operations
  void addAdjacency(const std::string& terr1, const std::string& terr2);
  static void addAdjacency(Territory* terr1, Territory* terr2);

  // validation
  bool validate() const;
  bool isConnectedGraph() const;
  bool areContinentsConnected() const;
  bool eachTerritoryBelongsToOneContinent() const;

  // utility
  void displayMap() const;
  void clear();
  int getNumberOfTerritories() const;
  int getNumberOfContinents() const;

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const Map& map);

private:
  // helpers
  void rebuildMaps();
};


/**
 * MapLoader class for loading maps from .map files
 */
class MapLoader {
private:
  enum class ParseState { NONE, TERRITORIES, CONTINENTS, MAP_INFO };

  ParseState currentState;
  std::unordered_map<std::string, std::vector<std::string>>* territoryAdjacency;

public:
  MapLoader();
  MapLoader(const MapLoader& other); // copy constructor
  MapLoader& operator=(const MapLoader& other); // assignment operator
  ~MapLoader(); // destructor

  // map loading
  std::unique_ptr<Map> loadMap(const std::string& filename);

  // validation
  static bool canReadFile(const std::string& filename);

  // utility
  bool operator==(const MapLoader* other) const;

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const MapLoader& mapLoader);

private:
  // helpers
  static std::string trim(const std::string& str);
  static std::vector<std::string> split(const std::string& str, char delimiter);
  static bool parseMapSection(const Map* map, const std::string& line);
  static bool parseContinentSection(Map* map, const std::string& line);
  bool parseTerritorySection(Map* map, const std::string& line);
  static void linkTerritoryAdjacency(
    const Map* map, Territory* territory, const std::vector<std::string>& adjacentNames
  );
};

// free function
void testLoadMaps();
