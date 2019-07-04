#include "Actor.h"
#include "StudentWorld.h"

#include <iostream>
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
class GraphObject;

//////////////////////////////////////////
//ACTOR
//////////////////////////////////////////

Actor::Actor(double x, double y, int imageID, StudentWorld* studentWorld) :
GraphObject(imageID, x, y), myWorld(studentWorld) //imageID startX startY Direction 
, isActorAlive(true){}

StudentWorld* Actor::getWorld() { return myWorld; }

bool Actor::isBlockable() const { return false; }

bool Actor::blocksFlame() const { return false; } // Does this object block flames? No by default

void Actor::pickUpGoodieIfAppropriate(Goodie* g){}
void Actor::useExitIfAppropriate(){}
bool Actor::isAlive() { return isActorAlive; }
void Actor::killActor(){isActorAlive=false;}
void Actor::dieByFallOrBurnIfAppropriate(){}

void Actor::activateIfAppropriate(Actor *a) {}

bool Actor::threatensCitizens() const{return false;}

bool Actor::triggersOnlyActiveLandmines() const {return false;}

void Actor::beVomitedOnIfAppropriate(){return;};

bool Actor::canBeInfected(){return false;};

//////////////////////////////////////////
//WALL
//////////////////////////////////////////

Wall::Wall(double x, double y, int imageID, StudentWorld* myWorld)
: Actor(x,y,imageID , myWorld) {}

void Wall::doSomething() { /*Do Nothing */ }
bool Wall::isBlockable() const { return true; }
bool Wall::blocksFlame() const {return true;}

//////////////////////////////////////////
//AGENT
//////////////////////////////////////////

Agent::Agent(double x, double y, int imageID ,  StudentWorld* myWorld)
: Actor(x,y,imageID , myWorld) {}

bool Agent::isBlockable() const { return true; }

void Agent::move(double x, double y){
    if(!getWorld()->isAgentMovementBlockedAt(x,y,this))
        moveTo(x, y);
}

bool Agent::triggersOnlyActiveLandmines() const {return triggersLandmines;}

//////////////////////////////////////////
//HUMAN
//////////////////////////////////////////
Human::Human(double x, double y, int imageID,StudentWorld* myWorld) : Agent(x,y,imageID, myWorld),
infectionCount(0), infection(false) {}

bool Human::canBeInfected(){return true;}
void Human::clearInfection(){ infection = false; infectionCount = 0;}
int Human::getInfectionDuration() const{return infectionCount;}
void Human::incrementInfection() {infectionCount++;}
bool Human::isInfected(){return infection;}
void Human::beVomitedOnIfAppropriate(){
    getWorld()->playSound(SOUND_CITIZEN_INFECTED); infection = true;}

//////////////////////////////////////////
//PENELOPE
//////////////////////////////////////////

Penelope::Penelope(double x, double y, int imageID, StudentWorld* myWorld)
: Human(x,y,imageID , myWorld), numFlames(0), numLandmines(0), numVacc(0)
{}

void Penelope::dieByFallOrBurnIfAppropriate()
{
    getWorld()->playSound(SOUND_PLAYER_DIE);
    killActor();
}

void Penelope::useExitIfAppropriate(){
    if(getWorld()->numCitizens()==0){
        getWorld()->finishedLevel();
    }
}

void Penelope::pickUpGoodieIfAppropriate(Goodie* g)
{
    g->grantSpecificGoodieReward(this);
    g->obtainedGoodie();
}


void Penelope::doSomething() {
    
    if(isInfected() && getInfectionDuration() < 500)
        incrementInfection();
    if(getInfectionDuration()==500)
    {
        this->killActor();
        getWorld()->playSound(SOUND_PLAYER_DIE);
        return;
    }
        
    int ch;
    if(getWorld()->getKey(ch)) { //    Try to get user input (if any is available)
        switch(ch)        // user hit a key during this tick! switch (ch)
        {
            case KEY_PRESS_SPACE:
            {
                if(numFlames > 0){
                    useFlamethrower(getDirection());
                }
                break;
            }
            case KEY_PRESS_TAB:
            {
                if(numLandmines > 0){
                    getWorld()->addActor(new Landmine(getX(),getY(),IID_LANDMINE, getWorld()));
                    numLandmines--;
                }
                break;
            }
            case KEY_PRESS_ENTER:
            {
                if(numVacc > 0){
                    numVacc--;
                    clearInfection();
                }
                break;
            }
            case KEY_PRESS_UP:
            {
                this->setDirection(up);
                this->move(getX(), getY()+4); break;
            }
            case KEY_PRESS_DOWN:
            {
                this->setDirection(down);
                this->move(getX(), getY()-4);
                break;
            }
            case KEY_PRESS_LEFT:
                this->setDirection(left);
                this->move(getX()-4, getY()); break;
            case KEY_PRESS_RIGHT:
                this->setDirection(right);
                this->move(getX()+4, getY()); break;
        }
    }
    getWorld()->activateOnAppropriateActors(this);
}
void Penelope::useFlamethrower(int dir)
{
    numFlames--;
    getWorld()->playSound(SOUND_PLAYER_FIRE);
    int px = getX();
    int py = getY();
    for(int i = 1; i <= 3; i++)
    {
        if(dir==up){
            if(!getWorld()->isFlameBlockedAt(px, py + i * SPRITE_HEIGHT))
                getWorld()->addActor(new Flame(px, py + i * SPRITE_HEIGHT, IID_FLAME,getWorld()));
            else
                break;
        }else if(dir==down){
            if(!getWorld()->isFlameBlockedAt(px, py - i * SPRITE_HEIGHT))
                getWorld()->addActor(new Flame(px, py - i * SPRITE_HEIGHT, IID_FLAME,getWorld()));
            else
                break;
        }else if(dir==left){
            if(!getWorld()->isFlameBlockedAt(px - i *SPRITE_WIDTH, py))
                getWorld()->addActor(new Flame(px - i *SPRITE_WIDTH, py, IID_FLAME,getWorld()));
            else
                break;
        }else if(dir==right){
            if(!getWorld()->isFlameBlockedAt(px + i *SPRITE_WIDTH, py))
                getWorld()->addActor(new Flame(px + i *SPRITE_WIDTH, py, IID_FLAME,getWorld()));
            else
                break;
        }
    }
}

void Penelope::increaseVaccines(){numVacc++;}
void Penelope::increaseLandmines(){numLandmines+=2;}
void Penelope::increaseFlameCharges(){numFlames+=5;}
int Penelope::getLandmines(){return numLandmines;}
int Penelope::getVaccines(){return numVacc;}
int Penelope::getNumFlames(){return numFlames;}

//////////////////////////////////////////
//CITIZEN
//////////////////////////////////////////
Citizen::Citizen(double x, double y, int imageID ,  StudentWorld* myWorld)
: Human(x,y,imageID , myWorld), isParalyzed(false) {}

void Citizen::doSomething()
{
    if(!isAlive())
    {
        return;
    }
    else if(isInfected() && getInfectionDuration() == 500)
    {
        this->killActor();
        getWorld()->playSound(SOUND_ZOMBIE_BORN);
        getWorld()->increaseScore(-1000);
        getWorld()->recordCitizenGone();
        
        int x = randInt(1,10);
        if(x <= 3){ //smart
            getWorld()->addActor(new SmartZombie(getX(), getY(), IID_ZOMBIE, getWorld()));
        }else
            getWorld()->addActor(new DumbZombie(getX(), getY(), IID_ZOMBIE, getWorld()));
        return;
    }
    else //citizen alive and healthy
    {
        if(isInfected() && getInfectionDuration() < 500)
            incrementInfection();
        
        if(isParalyzed){ //if paralyzed, do nothing
            isParalyzed = !isParalyzed;
            return;
        } else {
            isParalyzed = !isParalyzed;
            double x_trigger, y_trigger;
            double distance = 999999;
            bool isThreat;
            getWorld()->locateNearestCitizenTrigger(getX(), getY(), x_trigger, y_trigger, distance, isThreat);
            if(!isThreat && distance < pow(80,2)) //so it's penelope
            {
                if(this->getX() == x_trigger) //if citizen on same column as p
                {
                    if(y_trigger > getY()){ // if penelope is above the citizen
                        setDirection(up); //face toward penelope
                        //make sure nothing blocks citizen and penelope
                        if(!getWorld()->isAgentMovementBlockedAt(getX(), getY()+1, this) &&
                            !getWorld()->isAgentMovementBlockedAt(getX(), getY()+2, this))
                        {
                            move(getX(),getY()+2);
                        }
                    }
                    else{                    //penelope below citizen
                        setDirection(down);
                        if(!getWorld()->isAgentMovementBlockedAt(getX(), getY()-1, this) &&
                           !getWorld()->isAgentMovementBlockedAt(getX(), getY()-2, this))
                            move(getX(),getY() - 2);
                    }
                }
                else if(this->getY() == y_trigger){ //if on same row
                    if(x_trigger > getX()) //penelope to the right of citizen
                    {
                        setDirection(right);
                        if(!getWorld()->isAgentMovementBlockedAt(getX()+1, getY(), this) &&
                           !getWorld()->isAgentMovementBlockedAt(getX()+2, getY(), this))
                            move(getX()+2,getY());
                    }else{
                        setDirection(left);
                        if(!getWorld()->isAgentMovementBlockedAt(getX()-1, getY(), this) &&
                           !getWorld()->isAgentMovementBlockedAt(getX()-2, getY(), this))
                            move(getX()-2,getY());
                    }
                }
                if(getX() != x_trigger && getY() != y_trigger)
                {
                    int dir = randInt(0,1);
                    if(dir == 0) //0 == vertical movement
                    {
                        if(x_trigger > getX()) //penelope to the right of citizen
                        {
                            setDirection(right);
                            if(!getWorld()->isAgentMovementBlockedAt(getX()+1, getY(), this) &&
                               !getWorld()->isAgentMovementBlockedAt(getX()+2, getY(), this))
                                move(getX()+2,getY());
                        }else{
                            setDirection(left);
                            if(!getWorld()->isAgentMovementBlockedAt(getX()-1, getY(), this) &&
                               !getWorld()->isAgentMovementBlockedAt(getX()-2, getY(), this))
                                move(getX()-2,getY());
                        }
                    }
                    else //1 = horitzontal
                    {
                        if(y_trigger > getY()){ // if penelope is above the citizen
                            setDirection(up); //face toward penelope
                            //make sure nothing blocks citizen and penelope
                            if(!getWorld()->isAgentMovementBlockedAt(getX(), getY()+1, this) &&
                               !getWorld()->isAgentMovementBlockedAt(getX(), getY()+2, this))
                                move(getX(),getY()+2);
                        }
                        else{                    //penelope below citizen
                            setDirection(down);
                            if(!getWorld()->isAgentMovementBlockedAt(getX(), getY()-1, this) &&
                               !getWorld()->isAgentMovementBlockedAt(getX(), getY()-2, this))
                                move(getX(),getY() - 2);
                        }
                    }
                }
            }
            else if(isThreat && distance < pow(80,2)) //aka zombie
            {
                int dir = -1;
                double longestZDistance = -999;

                if(!getWorld()->isAgentMovementBlockedAt(getX(), getY()+1, this) &&
                   !getWorld()->isAgentMovementBlockedAt(getX(), getY()+2, this))
                {
                    double x_zombie, y_zombie, distance;
                    getWorld()->locateNearestCitizenThreat(getX(), getY()+2, x_zombie, y_zombie, distance);
                    
                    if(distance > longestZDistance){
                        longestZDistance = distance;
                        dir = up;
                    }
                }
                if(!getWorld()->isAgentMovementBlockedAt(getX(), getY()-1, this) &&
                   !getWorld()->isAgentMovementBlockedAt(getX(), getY()-2, this))
                {
                    double x_zombie, y_zombie;
                    getWorld()->locateNearestCitizenThreat(getX(), getY()-2, x_zombie, y_zombie, distance);
                    if(distance > longestZDistance){
                        longestZDistance = distance;
                        dir = down;
                    }
                }

                if(!getWorld()->isAgentMovementBlockedAt(getX()-1, getY(), this) &&
                   !getWorld()->isAgentMovementBlockedAt(getX()-2, getY(), this))
                {
                    double x_zombie, y_zombie;
                    getWorld()->locateNearestCitizenThreat(getX()-2, getY(), x_zombie, y_zombie, distance);
                    if(distance > longestZDistance){
                        longestZDistance = distance;
                        dir = left;
                    }
                }

                if(!getWorld()->isAgentMovementBlockedAt(getX()+1, getY(), this) &&
                   !getWorld()->isAgentMovementBlockedAt(getX()+2, getY(), this))
                {
                    dir = right;
                    double x_zombie, y_zombie;
                    getWorld()->locateNearestCitizenThreat(getX()+2, getY(), x_zombie, y_zombie, distance);
                    if(distance > longestZDistance){
                        longestZDistance = distance;
                        dir = right;
                    }
                }

                setDirection(dir);
                switch(dir)
                {
                    case(up): move(getX(), getY()+2); break;
                    case down: move(getX(), getY()-2); break;
                    case left: move(getX()-2, getY()); break;
                    case right: move(getX()+2, getY()); break;
                }
            }
            else
                return;
        }
    }
}

void Citizen::useExitIfAppropriate()
{
    this->killActor();
    getWorld()->increaseScore(500);
    getWorld()->playSound(SOUND_CITIZEN_SAVED);
    getWorld()->recordCitizenGone();
    return;
}
void Citizen::dieByFallOrBurnIfAppropriate()
{
    this->killActor();
    getWorld()->playSound(SOUND_CITIZEN_DIE);
    getWorld()->increaseScore(-1000);
    getWorld()->recordCitizenGone();
}

/////////////////////////////////////////
//Zombie
/////////////////////////////////////////
Zombie::Zombie(double x, double y, int imageID ,  StudentWorld* myWorld)
: Agent(x,y,imageID , myWorld) , mvtPlan(0){}

bool Zombie::threatensCitizens() const{return true;}

void Zombie::dieByFallOrBurnIfAppropriate()
{
    killActor();
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
    getWorld()->increaseScore(1000);
    int chance = randInt(1,10);
    int randDir = randInt(0,3) * 90;
    if(chance == 2){
        setDirection(randDir);
        switch(randDir)
        {
            case up:
                getWorld()->addActor(new Vaccine(getX(), getY()+SPRITE_HEIGHT, IID_VACCINE_GOODIE, getWorld()));
                break;
            case down:
                getWorld()->addActor(new Vaccine(getX(), getY()-SPRITE_HEIGHT, IID_VACCINE_GOODIE, getWorld()));
                break;
            case right:
                getWorld()->addActor(new Vaccine(getX()+SPRITE_WIDTH, getY(), IID_VACCINE_GOODIE, getWorld()));
                break;
            case left:
                getWorld()->addActor(new Vaccine(getX()-SPRITE_WIDTH, getY(), IID_VACCINE_GOODIE, getWorld()));
                break;
        }
    }
}

void Zombie::vomitIfAppropriate() {
    int chance = randInt(1,3);
    switch(getDirection())
    {
        case(up):
            if(getWorld()->isZombieVomitTriggerAt(getX(), getY()+SPRITE_HEIGHT) && chance == 3)
            {
                getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
                getWorld()->addActor(new Vomit(getX(),getY()+SPRITE_HEIGHT,IID_VOMIT,getWorld()));
                getWorld()->activateOnAppropriateActors(this);
                return;
            }
            break;
        case(down):
            if(getWorld()->isZombieVomitTriggerAt(getX(), getY()-SPRITE_HEIGHT) && chance == 3){
                getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
                getWorld()->addActor(new Vomit(getX(),getY()-SPRITE_HEIGHT,IID_VOMIT, getWorld()));
                getWorld()->activateOnAppropriateActors(this);
                return;
            }
            break;
        case(left):
            if(getWorld()->isZombieVomitTriggerAt(getX()-SPRITE_WIDTH, getY()) && chance == 3){
                getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
                getWorld()->addActor(new Vomit(getX()-SPRITE_WIDTH,getY(),IID_VOMIT,getWorld()));
                getWorld()->activateOnAppropriateActors(this);
                return;
            }
            break;
        case(right):
            if(getWorld()->isZombieVomitTriggerAt(getX(), getY()+SPRITE_WIDTH) && chance == 3){
                getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
                getWorld()->addActor(new Vomit(getX()+SPRITE_WIDTH,getY(),IID_VOMIT,getWorld()));
                getWorld()->activateOnAppropriateActors(this);
                return;
            }
            break;
    }
}
void Zombie::generalRandomMovement()
{
    //movement
    if(mvtPlan == 0)
    {
        int dir = randInt(0,3); //move in random direction
        this->setDirection(dir*90);
        mvtPlan = randInt(3,10);
    }
    switch(getDirection())
    {
        case(up):
            move(getX(), getY() + 1);
            break;
        case(down):
            move(getX(), getY() - 1);
            break;
        case(left):
            move(getX()-1, getY());
            break;
        case(right):
            move(getX()+1, getY());
            break;
    }
    mvtPlan--;
}

/////////////////////////////////////////
//SMARTZOMBIE
/////////////////////////////////////////
SmartZombie::SmartZombie(double x, double y, int imageID ,  StudentWorld* myWorld)
: Zombie(x,y,imageID , myWorld), isParalyzed(false){}

void SmartZombie::doSomething(){
    if(!this->isAlive())
        return;
    
    if(isParalyzed){ //if paralyzed, do nothing
        isParalyzed = !isParalyzed;
        return;
    }
    else //if not paralyzed
    {
        isParalyzed = !isParalyzed;
        //vomit
        vomitIfAppropriate();
        
        double human_x, human_y;
        double shortestDist = 999999; //arbitrarily high number
        
        //no human within 80 pixels
        if(!getWorld()->locateNearestVomitTrigger(getX(), getY(), human_x, human_y, shortestDist)){
            this->setDirection(randInt(0,3)*90); //randomDirection
            generalRandomMovement();
        }else{
            if(getX() == human_x){
                if(human_y > getY()){
                    setDirection(up);
                    move(getX(),getY()+1);
                }else{
                    setDirection(down);
                    move(getX(),getY()-1);
                }
            }
            else if(getY() == human_y)
            {
                if(human_x > getX()){
                    setDirection(right);
                    move(getX()+1,getY());
                }
                else{
                    setDirection(left);
                    move(getX()-1,getY());
                }
            }
            else{
                int x = randInt(1,2);
                if(x == 1){
                    if(human_y > getY()){
                        setDirection(up);
                        move(getX(),getY()+1);
                    }else{
                        setDirection(down);
                        move(getX(),getY()-1);
                    }
                }else{
                    if (human_x > getX()){
                        setDirection(right);
                        move(getX()+1,getY());
                    }
                    else{
                        setDirection(left);
                        move(getX()-1,getY());
                    }
                }
            }
        }
        return;
    }
}


/////////////////////////////////////////
//DUMBZOMBIE
/////////////////////////////////////////
DumbZombie::DumbZombie(double x, double y, int imageID ,  StudentWorld* myWorld)
: Zombie(x,y,imageID , myWorld), isParalyzed(false){}

void DumbZombie::doSomething() // x is cols y is rows
{
    if(!this->isAlive())
        return;
    
    if(isParalyzed){ //if paralyzed, do nothing
        isParalyzed = !isParalyzed;
        return;
    }
    else //if not paralyzed
    {
        isParalyzed = !isParalyzed;
        
        //vomit
        vomitIfAppropriate();
        //move randomly
        generalRandomMovement();
        return;
    }
}

///////////////////////////////////////////////////////

//////////////////////////////////////////
//ENVIRONMENT
//////////////////////////////////////////
Environment::Environment(double x, double y, int imageID, StudentWorld* myWorld)
: Actor(x,y,imageID , myWorld){}

void Environment::doSomething() {    return; }


//////////////////////////////////////////
//EXIT
//////////////////////////////////////////
Exit::Exit(double x, double y, int imageID, StudentWorld* myWorld)
: Environment(x, y, imageID , myWorld) {}

bool Exit::blocksFlame() const {return true;}

void Exit::activateIfAppropriate(Actor *a)
{
    a->useExitIfAppropriate();
}

void Exit::doSomething()
{
    getWorld()->activateOnAppropriateActors(this);
}

//////////////////////////////////////////
//LANDMINE
//////////////////////////////////////////
Landmine::Landmine(double x, double y, int imageID, StudentWorld* myWorld)
: Environment(x, y, imageID , myWorld), safetyTicks(30)
{}

void Landmine::activateIfAppropriate(Actor* overlappingActor){
    //if landmine active and gets triggered, sned out flames and kill this actor
    if(safetyTicks == 0 && overlappingActor->triggersOnlyActiveLandmines())
    {
        for(int i = -1; i <= 1; i++)
            for(int j = -1; j <= 1; j++)
                getWorld()->addActor(new Flame(getX() + i*SPRITE_WIDTH, getY() + j*SPRITE_HEIGHT,IID_FLAME, getWorld()));
        killActor();
    }
}

void Landmine::doSomething() {
    if(!isAlive())
        return;
    else{
        if(safetyTicks > 0){
            safetyTicks--;
            return;
        }
        getWorld()->activateOnAppropriateActors(this);
    }
}


void Landmine::dieByFallOrBurnIfAppropriate()
{
    if(isAlive()){
        for(int i = -1; i <= 1; i++)
            for(int j = -1; j <= 1; j++)
                getWorld()->addActor(new Flame(getX() + i*SPRITE_WIDTH, getY() + j*SPRITE_HEIGHT,IID_FLAME, getWorld()));
    }
    getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
    getWorld()->addActor(new Pit(getX(), getY(), IID_PIT, getWorld()));
    this->killActor();

}

//////////////////////////////////////////
//PIT
//////////////////////////////////////////
Pit::Pit(double x, double y, int imageID, StudentWorld* myWorld)
: Environment(x, y, imageID , myWorld)
{}

void Pit::doSomething(){
    getWorld()->activateOnAppropriateActors(this);
    
};

void Pit::activateIfAppropriate(Actor *a)
{
    if(a->isBlockable())
        a->dieByFallOrBurnIfAppropriate();
}


//////////////////////////////////////////
//GOODIE
//////////////////////////////////////////

Goodie::Goodie(double x, double y, int imageID, StudentWorld* myWorld)
: Environment(x, y, imageID , myWorld)
{ }

void Goodie::dieByFallOrBurnIfAppropriate()
{
    this->killActor();
}

void Goodie::obtainedGoodie()
{
    getWorld()->increaseScore(50);    //increase score
    killActor(); //set state to dead
    getWorld()->playSound(SOUND_GOT_GOODIE); //play sound
}

void Goodie::activateIfAppropriate(Actor *a)
{
    a->pickUpGoodieIfAppropriate(this);
}


LandmineGoodie::LandmineGoodie(double x, double y, int imageID, StudentWorld* myWorld)
: Goodie(x, y, imageID , myWorld)
{ }

void LandmineGoodie::grantSpecificGoodieReward(Penelope *p) {
    p->increaseLandmines();
}

void LandmineGoodie::doSomething() {
    if(!isAlive())
        return;
    else
        getWorld()->activateOnAppropriateActors(this);
}

Vaccine::Vaccine(double x, double y, int imageID, StudentWorld* myWorld)
: Goodie(x, y, imageID , myWorld) {}

void Vaccine::grantSpecificGoodieReward(Penelope *p) { p->increaseVaccines(); }

void Vaccine::doSomething() {
    if(!isAlive())
        return;
    else
        getWorld()->activateOnAppropriateActors(this);
}

GasCan::GasCan(double x, double y, int imageID, StudentWorld* myWorld)
: Goodie(x, y, imageID , myWorld) {}

void GasCan::grantSpecificGoodieReward(Penelope *p) {  p->increaseFlameCharges(); }

void GasCan::doSomething()
{
    if(!isAlive())
        return;
    else
    {         //The gas can goodie must determine if it overlaps with Penelope.
        getWorld()->activateOnAppropriateActors(this);
    }
}


//////////////////////////////////////////
//VOMIT AND FLAME
//////////////////////////////////////////


Vomit::Vomit(double x, double y, int imageID, StudentWorld* myWorld)
: Environment(x,y,imageID,myWorld), existedPrevTick(false){}

void Vomit::activateIfAppropriate(Actor *a)
{
    a->beVomitedOnIfAppropriate();
}

void Vomit::doSomething()
{
    if(!isAlive())
        return;
    if(!existedPrevTick)
    {
        getWorld()->activateOnAppropriateActors(this);
        existedPrevTick = true;
    }
    else
    {
        this->killActor();
    }
}

Flame::Flame(double x, double y, int imageID, StudentWorld* myWorld)
: Environment(x,y,imageID  ,myWorld), existedPrevTick(false){}

void Flame::activateIfAppropriate(Actor *a)
{
    a->dieByFallOrBurnIfAppropriate();
}

void Flame::doSomething()
{
    if(!isAlive())
        return;
    //check if this instance is its first tick
    if(!existedPrevTick){
        getWorld()->activateOnAppropriateActors(this);
        existedPrevTick = true;
    }
    else
    {
        this->killActor();
    }
}
