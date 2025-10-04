#pragma once

#include <string>
#include <unordered_map>
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
	std::string territoryName;
	int territoryId;
	std::string owner;
	int armies;
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
	std::string getName() const { return territoryName; }
	int getId() const { return territoryId; }
	std::string getOwner() const { return owner; }
	int getArmies() const { return armies; }
	const std::vector<Territory*>& getAdjTerritories() const { return adjTerritories; }
	Continent* getContinent() const { return continent; }

	// setters
	void setName(const std::string& name) { this->territoryName = name; }
	void setId(int id) { this->territoryId = id; }
	void setOwner(const std::string& ownerPlayer) { this->owner = ownerPlayer; }
	void setArmies(int armyCount) { this->armies = armyCount; }
	void setContinent(Continent* mainContinent) { this->continent = mainContinent; }

	// utility
	void displayInfo() const;
	bool operator==(const Territory& other) const; // equality operator
};

/**
 * Continent class representing a connected subgraph of territories
 */
class Continent {
private:
	std::string continentName;
	int continentId;
	std::vector<Territory*> territories;

public:
	// constructors
	Continent();
	Continent(const std::string& name, int id);
	Continent(const Continent& other); // copy constructor
	Continent& operator=(const Continent& other); // copy assignment
	~Continent(); // destructor

	// getters
	std::string getName() const { return continentName; }
	int getId() const { return continentId; }
	const std::vector<Territory*>& getTerritories() const { return territories; }

	// setters
	void setName(const std::string& name) { this->continentName = name; }
	void setId(int id) { this->continentId = id; }

	// utility
	void displayInfo() const;
	bool operator==(const Continent& other) const;
};

/**
 * Map class representing the entire game map as a connected graph
 */
class Map {
private:
	std::string mapName;
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
	std::string getMapName() const { return mapName; }
	const std::vector<std::unique_ptr<Territory>>& getTerritories() const { return territories; }
	const std::vector<std::unique_ptr<Continent>>& getContinents() const { return continents; }

	// setters
	void setMapName(const std::string& name) { this->mapName = name; }

	// validation methods
	bool validate() const;

	// utility
	void displayMap() const;
	int getNumberOfTerritories() const { return territories.size(); }
	int getNumberOfContinents() const { return continents.size(); }
};

/**
 * MapLoader class for loading maps from .map files
 */
class MapLoader {
private:
	enum class ParseState {
		NONE,
		MAP_INFO,
		CONTINENTS,
		TERRITORIES
	};

public:
	// constructors
	MapLoader();
	MapLoader(const MapLoader& other); // copy constructor
	MapLoader& operator=(const MapLoader& other); // copy assignment
	~MapLoader(); // destructor

	// map loading
	std::unique_ptr<Map> loadMap(const std::string& filename);
	bool canReadFile(const std::string& filename);

	
};
