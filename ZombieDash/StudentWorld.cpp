#include "StudentWorld.h"
#include "GameConstants.h"

#include <cmath> //added
#include "Actor.h"
#include <vector>

#include "Level.h" // required to use our provided class


#include <string>
#include <iostream>
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw
using namespace std;


GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp
StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath) {}



//TOP RIGHT COERNER: (x+SPRITE_WIDTH−1, y+SPRITE_HEIGHT−1)
void StudentWorld::activateOnAppropriateActors(Actor *a)
{
    //check if center of penelope overlaps with center of actor
    if(aInRangeOfB(a->getX(), penelope->getX(), a->getY(), penelope->getY(), 100))
        a->activateIfAppropriate(penelope);
    
    //DOES Actor *a OVERLAP WITH ANY OTHER ACTORS BESIDES ITSELF?
    for(vector<Actor*> :: iterator iter = actors.begin(); iter != actors.end(); iter++)
    {
        if(*iter != nullptr && (*iter)->isAlive()){
            if(aInRangeOfB(a->getX(), (*iter)->getX(), a->getY(), (*iter)->getY(), 100))
                a->activateIfAppropriate(*iter); //an agent overlapping with activated object overlapped
        }
    }
}

bool StudentWorld::isFlameBlockedAt(double x, double y) const
{
    //does object overlap with penelope
    if(aInRangeOfB(x, penelope->getX(), y, penelope->getY(), 100))
        return true; //penelope
    
    for(vector<Actor*> :: const_iterator iter = actors.begin(); iter != actors.end(); iter++)
    {
        if((*iter)->blocksFlame()) //if this actor, like a wall or exit, blocks flames
            if(  aInRangeOfB(x, (*iter)->getX(), y, (*iter)->getY(), 100))
                return true;
    }
    return false;//flame can exist here
}

bool StudentWorld::isAgentMovementBlockedAt(int x, int y, Actor*a)
{
    if(a!=penelope){
        //penelope
        int penelope_x_left = penelope->getX(), penelope_y_bottom = penelope->getY();
        int penelope_x_right = penelope_x_left + SPRITE_WIDTH-1, penelope_y_top = penelope_y_bottom+SPRITE_HEIGHT-1;
        
        //agent
        int agent_x_right = x+SPRITE_WIDTH-1,  agent_y_top = y+SPRITE_HEIGHT-1;
        
        if ((penelope_x_left <= agent_x_right &&  agent_x_right <= penelope_x_right && //TOPRIGHT OF AGENT
             penelope_y_bottom <= agent_y_top && agent_y_top <= penelope_y_top)
            ||
            (penelope_x_left <= x && x <= penelope_x_right && //BOTTOMLEFT OF AGENT
             penelope_y_bottom <= y && y <= penelope_y_top))
            return true;
        
        //if bottom left to bottom right of an actor overlaps with bottom right of an agent
        // OR
        //if top left to top right of an actor overlaps with TOPLEFT of an agent
        if((penelope_x_left <= agent_x_right && agent_x_right <= penelope_x_right && //BOTTOMRIGHT OF AGENT
            penelope_y_bottom <= y && y <= penelope_y_top)
           ||
           (penelope_x_left <= x && x <= penelope_x_right && //TOPLEFT OF AGENT
            penelope_y_bottom <= agent_y_top && agent_y_top <= penelope_y_top))
            return true;
    }
    
    for(vector<Actor*> :: iterator iter = actors.begin(); iter != actors.end(); iter++)
    {
        //make sure the object should block objects
        if(!(*iter)->isBlockable() || (*iter) == a) //not blockable
            continue;
        //object x and y Coordinates
        
        int actor_x_left = (*iter)->getX(), actor_y_bottom = (*iter)->getY();
        int actor_x_right = actor_x_left + SPRITE_WIDTH-1, actor_y_top = actor_y_bottom+SPRITE_HEIGHT-1;
        
        int agent_x_right = x+SPRITE_WIDTH-1,  agent_y_top = y+SPRITE_HEIGHT-1;
        //if bottom left to bottom right of agent overlaps with bottom left of actor
        // OR
        //if bottom left to bottom right of actor overlaps with bottom left of agent
        
        if ((actor_x_left <= agent_x_right &&  agent_x_right <= actor_x_right && //TOPRIGHT OF AGENT
             actor_y_bottom <= agent_y_top && agent_y_top <= actor_y_top)
              ||
            (actor_x_left <= x && x <= actor_x_right && //BOTTOMLEFT OF AGENT
             actor_y_bottom <= y && y <= actor_y_top))
                return true;
        
        //if bottom left to bottom right of an actor overlaps with bottom right of an agent
        // OR
        //if top left to top right of an actor overlaps with TOPLEFT of an agent
        if((actor_x_left <= agent_x_right && agent_x_right <= actor_x_right && //BOTTOMRIGHT OF AGENT
            actor_y_bottom <= y && y <= actor_y_top)
            ||
           (actor_x_left <= x && x <= actor_x_right && //TOPLEFT OF AGENT
            actor_y_bottom <= agent_y_top && agent_y_top <= actor_y_top))
            return true;
    }
    return false;
}

int StudentWorld::init()
{
    string level = "level01.txt";
    
    int finalLevel = 99;
    if(currentLevel <= finalLevel)
    {
        if(currentLevel < 10)
            level = "level0" + to_string(currentLevel) + ".txt";
        else
            level = "level" + to_string(currentLevel) + ".txt";
        
        if(!loadThisLevel(level)){
            if(!validLevel)
                return GWSTATUS_LEVEL_ERROR;
            else //must not exist
                return GWSTATUS_PLAYER_WON;
        }
        currentLevel++;
        return GWSTATUS_CONTINUE_GAME;
    }
    else
        return GWSTATUS_PLAYER_WON;
}

int StudentWorld::move()
{
    penelope->doSomething();
    for(vector<Actor*> :: iterator iter = actors.begin(); iter != actors.end(); iter++)
        (*iter)->doSomething();
    if(!penelope->isAlive()){
        levelCompleted = false;
        decLives();
        currentLevel--;
        return GWSTATUS_PLAYER_DIED;
    }
    //Remove and delete dead game objects
    for(vector<Actor*> :: iterator iter = actors.begin(); iter != actors.end(); iter++)
        if(!(*iter)->isAlive())
        {
            delete (*iter);
            iter = actors.erase(iter);
            iter--;
        }
    updateGameText();
    if(numCitizens()==0 && levelCompleted){ //if zero citizens left and penelope is on the exit
        playSound(SOUND_LEVEL_FINISHED);
        levelCompleted = false;
        return GWSTATUS_FINISHED_LEVEL;
    }
    return GWSTATUS_CONTINUE_GAME; //level unfinished
}

void StudentWorld::cleanUp()
{
    if(penelope != nullptr) { //delete penelope
        delete penelope;
        penelope = nullptr;
    }
    //delete vector actor pointers
    for(vector<Actor*> :: iterator iter = actors.begin(); iter != actors.end(); iter++)
    {
        delete (*iter);
        iter = actors.erase(iter);
        iter--;
    }
}

StudentWorld::~StudentWorld() {cleanUp();}

void StudentWorld::addActor(Actor* a) {  actors.push_back(a); }

bool StudentWorld::loadThisLevel(string levelFile) //loads level txt, returns true if successful
{
    validLevel = true;
    levelExists = true;
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result == Level::load_fail_file_not_found){
        levelExists = false;
        cerr << "Cannot find " << levelFile << " data file" << endl;
    }else if (result == Level::load_fail_bad_format){
        validLevel = false;
        cerr << "Level " << levelFile;
        cerr << " was improperly formatted" << endl;
    }else if (result == Level::load_success)
    {
        cerr << "Successfully loaded level" << endl;
        Level::MazeEntry ge;
        for(int i = 0; i < 16; i++){
            for(int j = 0; j < 16; j++)
            {
                ge = lev.getContentsOf(i,j);
                
                switch(ge)
                {
                    case(Level::empty):
                        break;
                    case (Level::player):
                        penelope = new Penelope(i*LEVEL_WIDTH,j*LEVEL_HEIGHT,IID_PLAYER, this);
                        break;
                    case (Level::wall):
                        actors.push_back(new Wall(i*LEVEL_WIDTH, j*LEVEL_HEIGHT, IID_WALL, this));
                        break;
                    case (Level::citizen):
                        actors.push_back(new Citizen(i*LEVEL_WIDTH, j*LEVEL_HEIGHT, IID_CITIZEN, this));
                        citizenCount++;
                        break;
                    case (Level::exit):
                        actors.push_back(new Exit(i*LEVEL_WIDTH, j*LEVEL_HEIGHT, IID_EXIT, this));
                        break;
                    case (Level::vaccine_goodie):
                        actors.push_back(new Vaccine(i*LEVEL_WIDTH, j*LEVEL_HEIGHT, IID_VACCINE_GOODIE,this));
                        break;
                    case (Level::landmine_goodie):
                        actors.push_back(new LandmineGoodie(i*LEVEL_WIDTH, j*LEVEL_HEIGHT, IID_LANDMINE_GOODIE, this));
                        break;
                    case (Level::gas_can_goodie):
                        actors.push_back(new GasCan(i*LEVEL_WIDTH, j*LEVEL_HEIGHT, IID_GAS_CAN_GOODIE,this));
                        break;
                    case (Level::pit):
                        actors.push_back(new Pit(i*LEVEL_WIDTH, j*LEVEL_HEIGHT, IID_PIT,this));
                        break;
                    case (Level::dumb_zombie):
                        actors.push_back(new DumbZombie(i*LEVEL_WIDTH, j*LEVEL_HEIGHT, IID_ZOMBIE, this));
                        break;
                    case (Level::smart_zombie):
                        actors.push_back(new SmartZombie(i*LEVEL_WIDTH, j*LEVEL_HEIGHT, IID_ZOMBIE, this));
                        break;
                }
            }
        }
        return true;
    }
    return false;
}

// Return true if there is a living zombie or Penelope, otherwise false.
// If true, otherX, otherY, and distance will be set to the location and
// distance of the one nearest to (x,y), and isThreat will be set to true
// if it's a zombie, false if a Penelope.
bool StudentWorld::locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) const
{
    isThreat = false; //no threat by default.
    
    double dist_p = 9999999; //set penelope distance to an arbitrarily high number
    
    if(aInRangeOfB(x, penelope->getX(), y, penelope->getY(), pow(80,2))) //is penelope within 80 pixels?
        dist_p = getEuclidianDistance(x, penelope->getX(), y, penelope->getY());
    
    locateNearestCitizenThreat(x, y, otherX, otherY, distance);

    if(dist_p <= distance && dist_p <= pow(80,2)){ //if penelope closer than zombie and within 80 pixels
        otherX = penelope->getX();
        otherY = penelope->getY();
        distance = dist_p;
        isThreat = false;
    }else if(distance <= dist_p && distance <= pow(80,2)){  //otherwise, is a zombie closer and in 80 pixels?
        isThreat = true;
    }

    if(dist_p > pow(80,2) && distance > pow(80,2))
        return false; //if both p and z are more than 80 pixels away, no threat

    return true; //there is a threat or penelope nearby
}

// Return true if there is a living zombie, false otherwise.  If true,
// otherX, otherY and distance will be set to the location and distance
// of the one nearest to (x,y).

bool StudentWorld::locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) const
{
    double shortest_dist_z = 9999999; //arbitrarily high number
    for(vector<Actor*> :: const_iterator iter = actors.begin(); iter != actors.end(); iter++)
    {
        if((*iter)->threatensCitizens()) //it's a zombie
        {
            double z_dist = getEuclidianDistance((*iter)->getX(), x, (*iter)->getY(), y); //zombie dist from citizen
            if(z_dist <= pow(80,2))
            {
                if(z_dist < shortest_dist_z){
                    shortest_dist_z = z_dist;
                    otherX = (*iter)->getX();
                    otherY = (*iter)->getY();
                    distance = shortest_dist_z;
                    return true;
                }
            }
        }
    }
    return false;
}

// Is there something at the indicated location that might cause a
// zombie to vomit (i.e., a human)?
bool StudentWorld::isZombieVomitTriggerAt(double x, double y) const
{
    if(aInRangeOfB(x, penelope->getX(), y, penelope->getY(), 100))
        return true;
    for(vector<Actor*> :: const_iterator iter = actors.begin(); iter != actors.end(); iter++)
    {
        if((*iter)->canBeInfected())
            if(pow(x - (*iter)->getX(), 2) + pow(y - (*iter)->getY(),2) <= pow(10,2))
                return true;
    }
    return false;
}

// Return true if there is a living human, otherwise false.  If true,
// otherX, otherY, and distance will be set to the location and distance
// of the human nearest to (x,y).
bool StudentWorld::locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance)
{
    double shortestDist = getEuclidianDistance(x, penelope->getX(), y, penelope->getY());
    otherX = penelope->getX();
    otherY = penelope->getY();
    for(vector<Actor*> :: const_iterator iter = actors.begin(); iter != actors.end(); iter++)
    {
        if((*iter)->canBeInfected()){
            if(getEuclidianDistance(x, (*iter)->getX(), y, (*iter)->getY()) <= shortestDist){
                shortestDist = getEuclidianDistance(x, (*iter)->getX(), y, (*iter)->getY());
                otherX = (*iter)->getX();
                otherY = (*iter)->getY();
            }
        }
    }
    distance = shortestDist;
    if(distance > pow(80,2)){
        return false;
    }
    return true;
}


// Record that one more citizen on the current level is gone (exited,
// died, or turned into a zombie).
void StudentWorld::recordCitizenGone() { citizenCount--; }

int StudentWorld::numCitizens(){return citizenCount;}

// Indicate that the player has finished the level if all citizens
// are gone.
void StudentWorld::finishedLevel() { levelCompleted = true; }


void StudentWorld::updateGameText()
{
    string finalHeaderText = "";
    //score
    ostringstream oss;
    int score = getScore();
    string s;
    if(score<0) {
        int temp = -score; //make positive
        oss.fill('0');
        oss << setw(5) << temp;
        s = oss.str();
    }else
    {
        oss.fill('0');
        oss << setw(6) << score;
        s = oss.str();
    }
    if(score<0)
        s = '-' + s;
    
    finalHeaderText += "Score: ";
    finalHeaderText += s;
    finalHeaderText += " ";
    finalHeaderText += "Level: ";
    
    if(getLevel() < 10){
        finalHeaderText += '0';
        finalHeaderText += to_string(getLevel());
    }else
        finalHeaderText += to_string(getLevel());
    
    finalHeaderText += " ";
    finalHeaderText += "Lives: ";
    finalHeaderText += to_string(getLives());
    finalHeaderText += " ";
    finalHeaderText += "Vaccines: ";
    finalHeaderText += to_string(penelope->getVaccines());
    finalHeaderText += " ";
    finalHeaderText += "Flames: ";
    finalHeaderText += to_string(penelope->getNumFlames());
    finalHeaderText += " ";
    finalHeaderText += "Mines: ";
    finalHeaderText += to_string(penelope->getLandmines());
    finalHeaderText += " ";
    finalHeaderText += "Infected: ";
    finalHeaderText += to_string(penelope->getInfectionDuration());
    
    setGameStatText(finalHeaderText);
}

//calculate euclidian distance between the centers of two objects; return whether they are within double distance
bool StudentWorld::aInRangeOfB(double ax, double bx, double ay, double by, double distance) const
{
    return (pow(getXCenter(ax) - getXCenter(bx), 2) + pow(getYCenter(ay)-getYCenter(by),2) <= distance);
}

double StudentWorld::getEuclidianDistance(double ax, double bx, double ay, double by) const
{
    return pow(getXCenter(ax) - getXCenter(bx), 2) + pow(getYCenter(ay)-getYCenter(by),2);
}

double StudentWorld::getXCenter(double x) const { return (x*2 + SPRITE_WIDTH - 1) / 2;}
double StudentWorld::getYCenter(double y) const {return (y*2 + SPRITE_HEIGHT - 1) / 2;}
