#include "LoggingObserver.h"
#include "CommandProcessing.h"
#include "Orders.h"
#include "GameEngine.h"
#include "Player.h"
#include "Map.h"

#include <fstream>
#include <iostream>

void testLoggingObserver() {
  std::cout << "=== Logging Observer Test ===" << std::endl;

  // Reset log file
  std::ofstream clearFile("gamelog.txt", std::ios::trunc);
  clearFile.close();

  LogObserver logger;

  // ---- Command logging ----
  CommandProcessor processor;
  processor.attach(&logger);

  Command* cmdLoadMap = new Command("loadmap World.map");
  processor.saveCommand(cmdLoadMap);
  cmdLoadMap->saveEffect("Loaded map World.map");

  Command* cmdValidate = new Command("validatemap");
  processor.saveCommand(cmdValidate);
  cmdValidate->saveEffect("Map validated successfully");

  // ---- Order logging ----
  OrdersList ordersList;
  ordersList.attach(&logger);

  Player playerOne("Alice");
  Player playerTwo("Bob");
  Territory territoryAlpha("Alpha", 1);
  Territory territoryBeta("Beta", 2);

  territoryAlpha.addAdjacentTerritory(&territoryBeta);
  territoryBeta.addAdjacentTerritory(&territoryAlpha);

  playerOne.addTerritory(&territoryAlpha);
  territoryAlpha.setOwner(&playerOne);
  playerOne.setReinforcementPool(10);

  playerTwo.addTerritory(&territoryBeta);
  territoryBeta.setOwner(&playerTwo);

  int deployArmies = 5;
  OrderDeploy* deployOrder = new OrderDeploy(&playerOne, &territoryAlpha, &deployArmies);
  ordersList.add(deployOrder);
  deployOrder->execute();

  int advanceArmies = 3;
  OrderAdvance* advanceOrder = new OrderAdvance(&playerOne, &territoryAlpha, &territoryBeta, &advanceArmies);
  ordersList.add(advanceOrder);
  advanceOrder->execute();

  // ---- Game engine logging ----
  GameEngine engine;
  engine.attach(&logger);
  engine.transitionState("start");
  engine.transitionState("map loaded");
  engine.transitionState("map validated");

  std::cout << "Logging complete. Review gamelog.txt for entries." << std::endl;
}

