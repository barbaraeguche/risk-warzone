#include "LoggingObserver.h"

#include <algorithm>
#include <iostream>

// ===== Observer =====
Observer::Observer() = default;

Observer::Observer(const Observer& other) {
    (void)other;
}

Observer::~Observer() = default;

Observer& Observer::operator=(const Observer& other) {
    if (this != &other) {
        (void)other;
    }
    return *this;
}

// ===== Subject =====
Subject::Subject() {
    observers = new std::vector<Observer*>();
}

Subject::Subject(const Subject& other) {
    observers = new std::vector<Observer*>(*other.observers);
}

Subject::~Subject() {
    delete observers;
}

Subject& Subject::operator=(const Subject& other) {
    if (this != &other) {
        delete observers;
        observers = new std::vector<Observer*>(*other.observers);
    }
    return *this;
}

void Subject::attach(Observer* observer) {
    if (!observer) {
        return;
    }
    for (Observer* existing : *observers) {
        if (existing == observer) {
            return;
        }
    }
    observers->push_back(observer);
}

void Subject::detach(Observer* observer) {
    if (!observer) {
        return;
    }
    observers->erase(
        std::remove(observers->begin(), observers->end(), observer),
        observers->end());
}

void Subject::notify() {
    for (Observer* observer : *observers) {
        if (observer) {
            observer->update(this);
        }
    }
}

// ===== LogObserver =====
LogObserver::LogObserver() {
    logfile = new std::ofstream();
    openLog();
}

LogObserver::LogObserver(const LogObserver& other) : Observer(other) {
    logfile = new std::ofstream();
    openLog();
    if (other.logfile && other.logfile->is_open()) {
        (*logfile) << std::flush;
    }
}

LogObserver::~LogObserver() {
    closeLog();
    delete logfile;
}

LogObserver& LogObserver::operator=(const LogObserver& other) {
    if (this != &other) {
        closeLog();
        if (!logfile) {
            logfile = new std::ofstream();
        }
        openLog();
        if (other.logfile && other.logfile->is_open()) {
            (*logfile) << std::flush;
        }
    }
    return *this;
}

std::ostream& operator<<(std::ostream& os, const LogObserver& observer) {
    os << "LogObserver writing to gamelog.txt";
    if (observer.logfile && observer.logfile->is_open()) {
        os << " (open)";
    } else {
        os << " (closed)";
    }
    return os;
}

void LogObserver::update(Subject* subject) {
    if (!subject) {
        return;
    }

    ILoggable* loggable = dynamic_cast<ILoggable*>(subject);
    if (!loggable) {
        return;
    }

    if (!logfile || !logfile->is_open()) {
        openLog();
    }

    if (logfile && logfile->is_open()) {
        (*logfile) << loggable->stringToLog() << std::endl;
        logfile->flush();
    }
}

void LogObserver::openLog() {
    if (!logfile) {
        return;
    }

    if (logfile->is_open()) {
        return;
    }

    logfile->open("gamelog.txt", std::ios::out | std::ios::app);
    if (!(*logfile)) {
        std::cerr << "Unable to open gamelog.txt for writing." << std::endl;
    }
}

void LogObserver::closeLog() {
    if (logfile && logfile->is_open()) {
        logfile->close();
    }
}

