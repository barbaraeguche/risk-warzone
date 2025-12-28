#include "LoggingObserver.h"
#include <iostream>


// ==================== Observer Class Implementation ====================
Observer::Observer() = default;

Observer::Observer(const Observer& other) {
  (void) other;
}

Observer& Observer::operator=(const Observer& other) {
  if (this != &other) {
    (void) other;
  }
  return *this;
}

Observer::~Observer() = default;


// ==================== Subject Class Implementation ====================
Subject::Subject() : observers(new std::vector<Observer*>()) {}

Subject::Subject(const Subject& other) : observers(new std::vector(*other.observers)) {}

Subject& Subject::operator=(const Subject& other) {
  if (this != &other) {
    delete observers;
    observers = new std::vector(*other.observers);
  }
  return *this;
}

Subject::~Subject() {
  delete observers;
}

// --- MANAGEMENT ---
void Subject::attach(Observer* obs) {
  if (!obs) return;

  for (Observer* oldObs : *observers) {
    if (oldObs == obs) return;
  }
  observers->push_back(obs);
}

void Subject::detach(Observer* obs) {
  if (!obs) return;
  std::erase(*observers, obs);
}

void Subject::notify() {
  for (Observer* obs : *observers) {
    if (obs) { obs->update(this); }
  }
}


// ==================== LogObserver Class Implementation ====================
LogObserver::LogObserver() : logFile(new std::ofstream()) {
  openLog();
}

LogObserver::LogObserver(const LogObserver& other) :
  Observer(other),
  logFile(new std::ofstream()) {
  openLog();
  if (other.logFile && other.logFile->is_open()) {
    (*logFile) << std::flush;
  }
}

LogObserver& LogObserver::operator=(const LogObserver& other) {
  if (this != &other) {
    closeLog();
    if (!logFile) logFile = new std::ofstream();

    openLog();
    if (other.logFile && other.logFile->is_open()) {
      (*logFile) << std::flush;
    }
  }
  return *this;
}

LogObserver::~LogObserver() {
  closeLog();
  delete logFile;
}

// --- MANAGEMENT ---
void LogObserver::update(Subject* sub) {
  if (!sub) return;

  ILoggable* loggable = dynamic_cast<ILoggable*>(sub);
  if (!loggable) return;

  if (!logFile || !logFile->is_open()) {
    openLog();
  }
  if (logFile && logFile->is_open()) {
    (*logFile) << loggable->stringToLog() << std::endl;
    logFile->flush();
  }
}

// --- HELPERS ---
void LogObserver::openLog() {
  if (!logFile || logFile->is_open()) return;

  logFile->open(GAMELOG_FILE, std::ios::out | std::ios::app);
  if (!(*logFile)) {
    std::cerr << "Unable to open gamelog.txt for writing." << std::endl;
  }
}

void LogObserver::closeLog() {
  if (logFile && logFile->is_open()) {
    logFile->close();
  }
}

// --- STREAM INSERTION OPERATOR ---
std::ostream& operator<<(std::ostream& os, const LogObserver& obs) {
  os << "LogObserver[";
  if (obs.logFile && obs.logFile->is_open()) {
    os << "(open) for writing to" << GAMELOG_FILE;
  } else {
    os << "(closed) for no writing to" << GAMELOG_FILE;
  }
  os << "]" << std::endl;
  return os;
}
