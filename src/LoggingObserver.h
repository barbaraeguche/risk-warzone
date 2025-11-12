#pragma once

#include <fstream>
#include <ostream>
#include <string>
#include <vector>

class Subject;

class ILoggable {
public:
    virtual std::string stringToLog() const = 0;
    virtual ~ILoggable() = default;
};

class Observer {
public:
    Observer();
    Observer(const Observer& other);
    virtual ~Observer();
    Observer& operator=(const Observer& other);

    virtual void update(Subject* subject) = 0;
};

class Subject {
public:
    Subject();
    Subject(const Subject& other);
    virtual ~Subject();
    Subject& operator=(const Subject& other);

    void attach(Observer* observer);
    void detach(Observer* observer);
    void notify();

protected:
    std::vector<Observer*>* observers;
};

class LogObserver : public Observer {
public:
    LogObserver();
    LogObserver(const LogObserver& other);
    ~LogObserver() override;

    LogObserver& operator=(const LogObserver& other);
    friend std::ostream& operator<<(std::ostream& os, const LogObserver& observer);

    void update(Subject* subject) override;

private:
    std::ofstream* logfile;

    void openLog();
    void closeLog();
};

