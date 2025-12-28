#pragma once
#include <fstream>
#include <string>
#include <vector>

class Subject;

class ILoggable {
public:
  virtual ~ILoggable() = default; // destructor

  // utility
  virtual std::string stringToLog() const = 0;
};


class Observer {
public:
  Observer();
  Observer(const Observer& other); // copy constructor
  Observer& operator=(const Observer& other); // assignment operator
  virtual ~Observer(); // destructor

  // management
  virtual void update(Subject* subject) = 0;
};


class Subject {
protected:
  std::vector<Observer*>* observers;

public:
  Subject();
  Subject(const Subject& other); // copy constructor
  Subject& operator=(const Subject& other); // assignment operator
  virtual ~Subject(); // destructor

  // management
  void attach(Observer* obs);
  void detach(Observer* obs);
  void notify();
};


class LogObserver : public Observer {
private:
  std::ofstream* logFile;

public:
  LogObserver();
  LogObserver(const LogObserver& other); // copy constructor
  LogObserver& operator=(const LogObserver& other); // assignment operator
  ~LogObserver() override; // destructor

  // management
  void update(Subject* sub) override;

  // stream insertion operator
  friend std::ostream& operator<<(std::ostream& os, const LogObserver& obs);

private:
  // helpers
  void openLog();
  void closeLog();
};

constexpr auto GAMELOG_FILE = "gamelog.txt";