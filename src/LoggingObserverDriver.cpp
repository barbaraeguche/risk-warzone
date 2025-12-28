#include "Cards.h"
#include "CommandProcessing.h"
#include "GameEngine.h"
#include "LoggingObserver.h"
#include "Map.h"
#include "Orders.h"
#include "Player.h"

#include <fstream>
#include <iostream>

void testLoggingObserver() {
  std::cout << "=== Logging Observer Test ===" << std::endl;

  // reset log file
  std::ofstream clearFile(GAMELOG_FILE, std::ios::trunc);
  clearFile.close();

  LogObserver logger;

  // ---- command logging ----
  CommandProcessor processor;
  processor.attach(&logger);

  Command* cmdLoadMap = new Command("loadmap World.map");
  processor.saveCommand(cmdLoadMap);
  cmdLoadMap->saveEffect("Loaded map World.map");

  Command* cmdValidate = new Command("validatemap");
  processor.saveCommand(cmdValidate);
  cmdValidate->saveEffect("Map validated successfully");

  // ---- order logging ----
  OrdersList ordersList;
  ordersList.attach(&logger);

  Deck* deck = new Deck();
  Player playerOne("Alice", deck);
  Player playerTwo("Bob", deck);
  Territory territoryAlpha("Alpha", 1);
  Territory territoryBeta("Beta", 2);

  territoryAlpha.addAdjTerritory(&territoryBeta);
  territoryBeta.addAdjTerritory(&territoryAlpha);

  playerOne.addTerritory(&territoryAlpha);
  territoryAlpha.setOwner(&playerOne);
  playerOne.setReinforcementPool(10);

  playerTwo.addTerritory(&territoryBeta);
  territoryBeta.setOwner(&playerTwo);

  int deployArmies = 5;
  OrderDeploy* deployOrder = new OrderDeploy(&playerOne, &territoryAlpha, &deployArmies);
  ordersList.addOrder(deployOrder);
  deployOrder->execute();

  int advanceArmies = 3;
  OrderAdvance* advanceOrder = new OrderAdvance(&playerOne, &territoryAlpha, &territoryBeta, &advanceArmies);
  ordersList.addOrder(advanceOrder);
  advanceOrder->execute();

  // ---- game engine logging ----
  GameEngine engine;
  engine.attach(&logger);
  engine.transitionState("start");
  engine.transitionState("map loaded");
  engine.transitionState("map validated");

  std::cout << "Logging complete. Review gamelog.txt for entries." << std::endl;
}
