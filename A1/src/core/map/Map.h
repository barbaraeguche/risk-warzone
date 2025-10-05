#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>


class Territory;
class Continent;
class Map;
class MapLoader;

/**
 * Territory class representing a node in the map graph
 */
class Territory {
private:
	std::string* territoryName;
	int* territoryId;
	std::string* ownerPlayer;
	int* armies;
	std::vector<Territory*> adjTerritories;
	Continent* continent;

public:
	// constructors
	Territory();
	Territory(const std::string& name, int id);
	Territory(const Territory& other); // copy constructor
	Territory& operator=(const Territory& other); // copy assignment
	~Territory(); // destructor

	// getters
	std::string getName() const { return *territoryName; }
	int getId() const { return *territoryId; }
	std::string getOwner() const { return *ownerPlayer; }
	int getArmies() const { return *armies; }
	const std::vector<Territory*>& getAdjTerritories() const { return adjTerritories; }
	Continent* getContinent() const { return continent; }

	// setters
	void setName(const std::string& name) const { *territoryName = name; }
	void setId(int id) const { *territoryId = id; }
	void setOwner(const std::string& owner) const { *ownerPlayer = owner; }
	void setArmies(int armyCount) const { *armies = armyCount; }
	void setContinent(Continent* mainContinent) { continent = mainContinent; }

	// management
	bool isAdjacentTo(Territory* territory) const;
	void addAdjacentTerritory(Territory* territory);
	void removeAdjacentTerritory(Territory* territory);

	// utility
	void displayInfo() const;
	bool operator==(const Territory& other) const; // equality operator
};


/**
 * Continent class representing a connected subgraph of territories
 */
class Continent {
private:
	std::string* continentName;
	int* continentId;
	std::vector<Territory*> territories;

public:
	// constructors
	Continent();
	Continent(const std::string& name, int id);
	Continent(const Continent& other); // copy constructor
	Continent& operator=(const Continent& other); // copy assignment
	~Continent(); // destructor

	// getters
	std::string getName() const { return *continentName; }
	int getId() const { return *continentId; }
	const std::vector<Territory*>& getTerritories() const { return territories; }

	// setters
	void setName(const std::string& name) const { *continentName = name; }
	void setId(int id) const { *continentId = id; }

	// territory management
	bool containsTerritory(Territory* territory) const;
	void addTerritory(Territory* territory);
	void removeTerritory(Territory* territory);

	// validation
	bool isConnected() const;

	// utility
	void displayInfo() const;
	bool operator==(const Continent& other) const;
};


/**
 * Map class representing the entire game map as a connected graph
 */
class Map {
private:
	std::string* mapName;
	std::vector<std::unique_ptr<Territory>> territories;
	std::vector<std::unique_ptr<Continent>> continents;
	std::unordered_map<std::string, Territory*> territoryNameMap;
	std::unordered_map<int, Territory*> territoryIdMap;
	std::unordered_map<std::string, Continent*> continentNameMap;

public:
	// constructors
	Map();
	Map(const std::string& name);
	Map(const Map& other); // copy constructor
	Map& operator=(const Map& other); // copy assignment
	~Map(); // destructor

	// getters
	std::string getMapName() const { return *mapName; }
	const std::vector<std::unique_ptr<Territory>>& getTerritories() const { return territories; }
	const std::vector<std::unique_ptr<Continent>>& getContinents() const { return continents; }

	// setters
	void setMapName(const std::string& name) const { *mapName = name; }

	// territory management
	Territory* addTerritory(const std::string& name, int id);
	Territory* getTerritory(const std::string& name) const;
	Territory* getTerritory(int id) const;
	bool removeTerritory(const std::string& name);

	// continent management
	Continent* addContinent(const std::string& name, int id);
	Continent* getContinent(const std::string& name) const;
	bool removeContinent(const std::string& name);

	// graph operations
	void addAdjacency(const std::string& territory1, const std::string& territory2) const;
	static void addAdjacency(Territory* territory1, Territory* territory2);

	// validation
	bool validate() const;
	bool isConnectedGraph() const;
	bool areContinentsConnected() const;
	bool eachTerritoryBelongsToOneContinent() const;

	// utility
	void displayMap() const;
	void clear();
	int getNumberOfTerritories() const { return territories.size(); }
	int getNumberOfContinents() const { return continents.size(); }

private:
	// helpers
	static void dfsVisit(Territory* territory, std::unordered_set<Territory*>& visited);
	void rebuildMaps();
};


/**
 * MapLoader class for loading maps from .map files
 */
class MapLoader {
private:
	enum class ParseState {
		NONE,
		TERRITORIES,
		CONTINENTS,
		MAP_INFO
	};

public:
	// constructors
	MapLoader();
	MapLoader(const MapLoader& other); // copy constructor
	bool operator==(MapLoader* mapLoader) const;

	MapLoader& operator=(const MapLoader& other); // copy assignment
	~MapLoader(); // destructor

	// map loading
	std::unique_ptr<Map> loadMap(const std::string& filename);
	static bool canReadFile(const std::string& filename);

private:
	// helpers
	static std::string trim(const std::string& str);
	static std::vector<std::string> split(const std::string& str, char delimiter);
	static bool parseMapSection(const Map* map, const std::string& line);
	static bool parseContinentSection(Map* map, const std::string& line);
	bool parseTerritorySection(Map* map, const std::string& line);
	static void linkTerritoryAdjacencies(const Map* map, Territory* territory, const std::vector<std::string>& adjacentNames);

	// state variables
	ParseState currentState;
	std::unordered_map<std::string, std::vector<std::string>> territoryAdjacencies;
};
