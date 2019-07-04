#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
//#include "StudentWorld.h" 

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Goodie;
class StudentWorld;

///////////////////////////////////////
// ACTOR - GraphObject
///////////////////////////////////////
class Actor : public GraphObject
{
public:
    Actor(double x,double y, int imageID, StudentWorld* myWorld);
    virtual void doSomething() = 0;
    virtual bool isBlockable() const;
    virtual void pickUpGoodieIfAppropriate(Goodie* g);
    virtual void activateIfAppropriate(Actor *a);
    virtual bool isAlive();
    virtual void dieByFallOrBurnIfAppropriate();
    void killActor(); //goodbye
    virtual bool blocksFlame() const;     // Does this object block flames?
    virtual bool threatensCitizens() const;
    StudentWorld* getWorld();
    virtual bool triggersOnlyActiveLandmines() const;
    virtual void beVomitedOnIfAppropriate();
    // If this object uses exits, use the exit.
    virtual void useExitIfAppropriate();
    virtual bool canBeInfected();

private:
    bool isActorAlive;
    StudentWorld* myWorld;
};

///////////////////////////////////////
// WALL -- Actor
///////////////////////////////////////

class Wall : public Actor
{
public:
    Wall(double x, double y, int imageID, StudentWorld* myWorld);
    bool isBlockable() const;
    void doSomething();
    bool blocksFlame() const;
};

///////////////////////////////////////
// AGENT -- Actor
///////////////////////////////////////

class Agent: public Actor
{
public:
    Agent(double x, double y, int imageID,  StudentWorld* myWorld);
    bool isBlockable() const;
    void move(double x, double y);
    bool triggersOnlyActiveLandmines() const;
private:
    bool triggersLandmines = true;
};

//////////////////////////////////////////////////
// HUMAN -- Blocking -- Actor
//////////////////////////////////////////////////

class Human: public Agent
{
public:
    virtual bool canBeInfected();
    Human(double x, double y, int imageID,StudentWorld* myWorld);
    void clearInfection();    // Make this human uninfected by vomit.
    int getInfectionDuration() const; // How many ticks since this human was infected by vomit?
    void incrementInfection();
    bool isInfected();
    void beVomitedOnIfAppropriate();
private:
    bool infection = false;
    int infectionCount;
};

///////////////////////////////////////
// PENELOPE -- Human -- Blocking -- Actor
///////////////////////////////////////

class Penelope : public Human
{
public:
    Penelope(double x, double y, int imageID ,  StudentWorld* myWorld);
    void doSomething();
    void useFlamethrower(int dir);
    void increaseVaccines();
    void increaseLandmines();
    void increaseFlameCharges();
    virtual void pickUpGoodieIfAppropriate(Goodie* g);
    int getLandmines();
    int getVaccines();
    int getNumFlames();
    virtual void useExitIfAppropriate();
    virtual void dieByFallOrBurnIfAppropriate();
private:
    int numLandmines, numVacc, numFlames;
};

///////////////////////////////////////
// CITIZEN -- Human -- Blocking -- Actor
///////////////////////////////////////
class Citizen : public Human
{
public:
    Citizen(double x, double y, int imageID ,  StudentWorld* myWorld);
    void doSomething();
    virtual void useExitIfAppropriate();
    virtual void dieByFallOrBurnIfAppropriate();
private:
    bool isParalyzed;
};

///////////////////////////////////////
// ZOMBIE -- Blocking -- Actor
///////////////////////////////////////
class Zombie : public Agent
{
public:
    Zombie(double x, double y, int imageID ,  StudentWorld* myWorld);
    bool threatensCitizens() const;
    void dieByFallOrBurnIfAppropriate();
    void vomitIfAppropriate();
    void generalRandomMovement();
private:
    int mvtPlan;
};

///////////////////////////////////////
// SMARTZOMBIE -- Zombie -- Blocking -- Actor
///////////////////////////////////////
class SmartZombie : public Zombie
{
public:
    SmartZombie(double x, double y, int imageID ,  StudentWorld* myWorld);
    void doSomething();
private:
    bool isParalyzed = false;
};
///////////////////////////////////////
// DUMBZOMBIE -- Zombie -- Blocking -- Actor
///////////////////////////////////////
class DumbZombie : public Zombie
{
public:
    DumbZombie(double x, double y, int imageID ,  StudentWorld* myWorld);
    void doSomething(); // x is cols y is rows
private:
    bool isParalyzed = false;
};

///////////////////////////////////////
// ENVIRONMENT -- Actor
///////////////////////////////////////
class Environment : public Actor
{
public:
    Environment(double x, double y, int imageID, StudentWorld* myWorld);
    void doSomething();
};

///////////////////////////////////////
// EXIT -- Environment -- Actor
///////////////////////////////////////

class Exit : public Environment
{
public:
    Exit(double x, double y, int imageID, StudentWorld* myWorld);
    void doSomething();
    bool blocksFlame() const;
    void activateIfAppropriate(Actor *a);
};

///////////////////////////////////////
// Landmine -- Environment -- Actor
///////////////////////////////////////

class Landmine : public Environment
{
public:
    Landmine(double x, double y, int imageID, StudentWorld* myWorld);
    void doSomething();
    void activateIfAppropriate(Actor* overlappingActor);
    void dieByFallOrBurnIfAppropriate();
private:
    int safetyTicks;
//    virtual void dieByFallOrBurnIfAppropriate();
};
///////////////////////////////////////
// Pit -- Environment -- Actor
///////////////////////////////////////
class Pit : public Environment
{
public:
    Pit(double x, double y, int imageID, StudentWorld* myWorld);
    void doSomething();
    void activateIfAppropriate(Actor *a);
};

///////////////////////////////////////
// GOODIE -- Environment -- Actor
///////////////////////////////////////
class Goodie : public Environment
{
public:
    Goodie(double x, double y, int imageID , StudentWorld* myWorld);
    void obtainedGoodie();
    void activateIfAppropriate(Actor *a);
    virtual void grantSpecificGoodieReward(Penelope *p) = 0;
    virtual void dieByFallOrBurnIfAppropriate();
private:
};

class LandmineGoodie : public Goodie
{
public:
    LandmineGoodie(double x, double y, int imageID , StudentWorld* myWorld);
    void grantSpecificGoodieReward(Penelope *p);
    void doSomething();
};

class Vaccine : public Goodie
{
public:
    Vaccine(double x, double y, int imageID  , StudentWorld* myWorld);
    void doSomething();
    void grantSpecificGoodieReward(Penelope *p);
};

class GasCan : public Goodie
{
public:
    GasCan(double x, double y, int imageID  , StudentWorld* myWorld);
    void doSomething();
    void grantSpecificGoodieReward(Penelope *p);
};


///////////////////////////////////////
// Vomit -- Environment -- Actor
///////////////////////////////////////
class Vomit : public Environment
{
public:
    Vomit(double x, double y, int imageID  , StudentWorld* myWorld);
    void doSomething();
    void activateIfAppropriate(Actor* a);
private:
    bool existedPrevTick;
};

///////////////////////////////////////
// FLAME -- Environment -- Actor
///////////////////////////////////////
class Flame : public Environment
{
public:
    Flame(double x, double y, int imageID, StudentWorld* myWorld);
    void activateIfAppropriate(Actor *a);
    void doSomething();
private:
    bool existedPrevTick;
};

#endif // ACTOR_H_
