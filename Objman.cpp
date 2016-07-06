/// \file objman.cpp
/// Code for the object manager class CObjectManager.
/// Copyright Ian Parberry, 2004.
/// Last updated September 23, 2010.

#include "objman.h"
#include "defines.h"
#include "timer.h"
#include "object.h"
#include "score.h"
#include "Ai.h"
#include "Input.h"
//character headers
#include "testChar.h"
//end character headers
#include "sndlist.h" //list of sounds
#include "gamerenderer.h"
#include "russian.h"

const int INVULNERABLE_TIME = 3000; ///< Time that player is invulnerable after death.

//Test hitboxes
extern DETECTIONBOX testBox;
extern int curFrameNum;
//done

extern int PauseGame;
extern int p1Health;
extern int p2Health;
extern int p1ComboCounter;
extern int p2ComboCounter;
extern int p1SuperCurrent;
extern int p2SuperCurrent;
extern CTimer g_cTimer; //game timer
extern CScoreManager g_cScoreManager; //score manager
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern CInputManager* g_pInputManager; ///< The input manager.
extern int p1RoundsWon;
extern int p2RoundsWon;

extern CGameRenderer GameRenderer;


CObjectManager::CObjectManager(){ //constructor 
  m_nMaxCount = MAX_OBJECTS; m_nCount = 0;
  m_nLastGunFireTime = 0; m_pPlayerObject = NULL;
  m_pObjectList = new CGameObject*[m_nMaxCount]; //object list
  m_nNumBoxes = 0;
  m_pCollisionHurtBoxStorage = new BOXSTORAGE[5];
  /*BOXSTORAGE tempBox;
  tempBox.right = 1.0f;
  tempBox.left = -1.0f;
  tempBox.top = 1.0f;
  tempBox.bottom = -1.0f;
  for(int a = 0; a < 5; a++)
  {
    m_pCollisionHurtBoxStorage[a] = tempBox;
  }*/
  for(int i = 0; i < m_nMaxCount; i++) 
    m_pObjectList[i] = NULL;
  m_nStartInvulnerableTime = g_cTimer.time(); //start invulnerable
}

CObjectManager::~CObjectManager(){ //destructor
  for(int i=0; i<m_nMaxCount; i++) //for each object
    delete m_pObjectList[i]; //delete it
  delete [] m_pCollisionHurtBoxStorage; //delete box storage
  delete[]m_pObjectList; //delete object list
  delete m_pPlayerObject; //delete player object
  delete m_pPlayer2Object; //delete player 2 object
}

/// Create a new instance of a game object.
/// \param object the type of the new object
/// \param name name of object as found in name tag of XML settings file
/// \param location initial location
/// \param xspeed horizontal speed
/// \param yspeed vertical speed

void CObjectManager::create(int charID, int playerNum, ObjectType object, char* name, D3DXVECTOR3 location,
                           float xspeed, float yspeed){

  if(charID == 0) //load testchar
  {
    if(playerNum == 1)
      m_pPlayerObject = new CCharObject(object, playerNum, name, location, xspeed, yspeed);
    else if(playerNum == 2)
      m_pPlayer2Object = new CCharObject(object, playerNum, name, location, xspeed, yspeed);
    else //not the player object
      if(m_nCount < m_nMaxCount){ //if room, create object
        int i=0;

        while(m_pObjectList[i]) ++i; //find first free slot

          m_pObjectList[i] = new CGameObject(object, name, location, xspeed, yspeed);

        m_nCount++; //one more object
      }
  }
  else if(charID == 1)  //load as object class
  {
    if(playerNum == 1)
      m_pPlayerObject = new CGameObject(object, name, location, xspeed, yspeed);
    else if(playerNum == 2)
      m_pPlayer2Object = new CGameObject(object, name, location, xspeed, yspeed);
    else //not the player object
      if(m_nCount < m_nMaxCount){ //if room, create object
        int i=0;

        while(m_pObjectList[i]) ++i; //find first free slot

          m_pObjectList[i] = new CGameObject(object, name, location, xspeed, yspeed);

        m_nCount++; //one more object
      }
  }
  else if(charID == 2) //load testchar for russian
  {
    if(playerNum == 1)
      m_pPlayerObject = new CRussian(object, playerNum, name, location, xspeed, yspeed);
    else if(playerNum == 2)
      m_pPlayer2Object = new CRussian(object, playerNum, name, location, xspeed, yspeed);
    else //not the player object
      if(m_nCount < m_nMaxCount){ //if room, create object
        int i=0;

        while(m_pObjectList[i]) ++i; //find first free slot

          m_pObjectList[i] = new CGameObject(object, name, location, xspeed, yspeed);

        m_nCount++; //one more object
      }
  }
}


void CObjectManager::create(ObjectType object, char* name, D3DXVECTOR3 location,
                           float xspeed, float yspeed){

  

    if(object == PLAYER1_OBJECT)
      m_pPlayerObject = new CGameObject(PLAYER1_OBJECT, name, location, xspeed, yspeed);
    else //not the player object
      if(m_nCount < m_nMaxCount){ //if room, create object
        int i=0;

        while(m_pObjectList[i]) ++i; //find first free slot

          m_pObjectList[i] = new CGameObject(object, name, location, xspeed, yspeed);

        m_nCount++; //one more object
      }
}
/// Move all game objects.

void CObjectManager::move(){ 
  m_pPlayer2Object->move();
  m_pPlayerObject->move();
  detectCollisions(1,0,0);  //collision code for hit detection.  Later this will be a loop that calls
                            //this once for each hurtbox in the game.
  for(int i=0; i<m_nMaxCount; i++){ //for each slot

    if(m_pObjectList[i]){ //if there's an object there

      if(m_pObjectList[i]->m_bIntelligent) //if intelligent, tell it about player
        ((CIntelligentObject*)m_pObjectList[i])->player(
          m_pPlayerObject->m_structLocation, 
          distance(m_pObjectList[i], m_pPlayerObject));

      m_pObjectList[i]->move(); //move it

      //wrap objects a fixed distance from player
      const float dX = (float)g_nScreenWidth; // Wrap distance from player.
      const float myX = // My x coordinate.
        m_pObjectList[i]->m_structLocation.x; 
      const float playerX = // Player x coordinate.
        m_pPlayerObject->m_structLocation.x; 

      if(myX > playerX+dX) //too far behind
        m_pObjectList[i]->m_structLocation.x -= 2.0f*dX;

      else if(myX < playerX-dX) //too far ahead
        m_pObjectList[i]->m_structLocation.x += 2.0f*dX;
    }
  }

  //collision detection
  CollisionDetection();

  //cull old objects
  cull();
}

/// Draw all game objects

void CObjectManager::draw(){ 
  for(int i=0; i<m_nMaxCount; i++) //for each object slot
    if(m_pObjectList[i]) //if there's an object there
      m_pObjectList[i]->draw(); //draw it
  if(m_pPlayerObject->m_structLocation.x > m_pPlayer2Object->m_structLocation.x)
  {
		if(m_pPlayerObject->facing == 6 && m_pPlayerObject->airBorne == 0)
      m_pPlayerObject->facing = 4;
    if(m_pPlayer2Object->facing == 4 && m_pPlayer2Object->airBorne == 0)
      m_pPlayer2Object->facing = 6;
  }
  else
  {
    if(m_pPlayerObject->facing == 4 && m_pPlayerObject->airBorne == 0)
      m_pPlayerObject->facing = 6;
    if(m_pPlayer2Object->facing == 6 && m_pPlayer2Object->airBorne == 0)
      m_pPlayer2Object->facing = 4;
  }
  m_pPlayerObject->draw();
  m_pPlayer2Object->draw();
}


void CObjectManager::checkFacing(int player)
{
	if(player == 1)
	{
		if(m_pPlayerObject->facing == 6 && m_pPlayerObject->m_structLocation.x > m_pPlayer2Object->m_structLocation.x)
		{
			m_pPlayerObject->facing = 4;
		}
		else if(m_pPlayerObject->facing == 4 && m_pPlayerObject->m_structLocation.x < m_pPlayer2Object->m_structLocation.x)
		{
			m_pPlayerObject->facing = 6;
		}
	}
	else if(player == 2)
	{
		if(m_pPlayer2Object->facing == 6 && m_pPlayer2Object->m_structLocation.x > m_pPlayerObject->m_structLocation.x)
		{
			m_pPlayerObject->facing = 4;
		}
		else if(m_pPlayer2Object->facing == 4 && m_pPlayer2Object->m_structLocation.x < m_pPlayerObject->m_structLocation.x)
		{
			m_pPlayerObject->facing = 6;
		}
	}
}


/// Get the player's location.
/// \param x returns the player's x coordinate
/// \param y returns the player's y coordinate

void CObjectManager::GetPlayerLocation(float &x, float &y){ //get location
  x = m_pPlayerObject->m_structLocation.x;
  y = m_pPlayerObject->m_structLocation.y;
}

void CObjectManager::GetPlayerLocations(float &x1, float &y1, float &x2, float &y2)
{
	x1 = m_pPlayerObject->m_structLocation.x;
	y1 = m_pPlayerObject->m_structLocation.y;
	x2 = m_pPlayer2Object->m_structLocation.x;
	y2 = m_pPlayer2Object->m_structLocation.y;
}

//returns a float as the midpoint between the players
float CObjectManager::GetXAvg()
{
    return (m_pPlayer2Object->m_structLocation.x + m_pPlayerObject->m_structLocation.x)/2.0f;
}

/// Get player's Y location.
float CObjectManager::GetPlayerLocationY(int player)
{
  if(player == 1)
    return m_pPlayerObject->m_structLocation.y;
  else if(player == 2)
    return m_pPlayer2Object->m_structLocation.y;
  else return 0;  //player set to invalid integer
}


/// Change the speed of the player object up or down.
/// \param xdelta change in players's horizontal speed
/// \param ydelta change in players's vertical speed

void CObjectManager::Accelerate(int xdelta, int ydelta){ 
  m_pPlayerObject->accelerate(xdelta, ydelta);
}

/// Distance between objects.
/// \param pointer to first object 
/// \param pointer to second object
/// \return distance between the two objects

float CObjectManager::distance(CGameObject *first, CGameObject *second){ //return distance between objects
  const float fWorldWidth = 2.0f * (float)g_nScreenWidth; //world width

  if(first == NULL || second == NULL)return -1; //bail if bad pointer

  float x = (float)fabs(first->m_structLocation.x - second->m_structLocation.x); //x distance
  float y = (float)fabs(first->m_structLocation.y - second->m_structLocation.y); //y distance

  //compensate for wrap-around world
  if(x > fWorldWidth) x -= (float)fWorldWidth;

  //return result
  return (float)sqrt(x*x + y*y);
}

/// Remove an object from the object list.
/// Assumes that there is an object there to be deleted.
/// \param index index of the object to be deleted.

void CObjectManager::kill(int index){ //remove object

  if(m_pObjectList[index] == NULL)return; //fail and bail

  //add to score for killing it
   g_cScoreManager.AddToScore(m_pObjectList[index]->m_nObjectType);

  //housekeeping
  m_nCount--;
  delete m_pObjectList[index];
  m_pObjectList[index] = NULL;
}

/// Fire a bullet from the player's gun.

void CObjectManager::FireGun(){ //fire player object's gun
  if(g_cTimer.elapsed(m_nLastGunFireTime, 200)){ //slow down firing rate
    D3DXVECTOR3 v; //location of bullet
    v.x = m_pPlayerObject->m_structLocation.x - 60;
    v.y = m_pPlayerObject->m_structLocation.y + 20;
    v.z = 500.0f;
    //create bullet, note bullet speed is totally faked
    //create(BULLET_OBJECT, "bullet", v, -40.0f, m_pPlayerObject->m_fYspeed*2.0f);
  }
}

/// Cull old objects.
/// Run through the objects in the object list and compare their age to
/// their life span. Kill any that are too old. Immortal objects are
/// flagged with a negative life span, so ignore those.

void CObjectManager::cull(){ //cull old objects

  CGameObject *object;

  for(int i=0; i<m_nMaxCount; i++){ //for each object

    object = m_pObjectList[i]; //current object

    if(object){ //if there's really an object there
      //died of old age
      if(object->m_nLifeTime > 0 && //if mortal and ...
      (g_cTimer.time() - object->m_nBirthTime > object->m_nLifeTime)) //...old...
        kill(i); //...then kill it 
      //one shot animation 
      if(object->m_nFrameCount > 1 && !object->m_bCycleSprite && //if plays one time...
        object->m_nCurrentFrame >= object->m_nAnimationFrameCount) //and played once already...
          replace(i); //...then replace the object
    }
  }

  //player object is special     
  if(m_pPlayerObject->m_nLifeTime > 0 && //if mortal and ...
      (g_cTimer.time() - m_pPlayerObject->m_nBirthTime > m_pPlayerObject->m_nLifeTime)) //...old...
        replaceplayer(); //...then replace it 
  else if(m_pPlayerObject->m_nFrameCount > 1 && !m_pPlayerObject->m_bCycleSprite && //if plays one time...
    m_pPlayerObject->m_nCurrentFrame >= m_pPlayerObject->m_nAnimationFrameCount) //and played once already...
      replaceplayer(); //...then replace it
}

/// Replace an object by the next in the appropriate series (object, exploding
/// object, dead object). If there's no "next" object, just kill it.
/// \param index index of the object to be replaced

void CObjectManager::replace(int index){ //replace object by next in series

  CGameObject *object = m_pObjectList[index]; //current object
  ObjectType newtype;
  BOOL successor=TRUE; //assume it has a successor
  const int namelength = 64;
  char name[namelength]; //name for new object in XML tag

  //decide on new object type
  switch(object->m_nObjectType){
     //your code goes here, newtype gets some sensible value
    default: successor = FALSE; newtype = object->m_nObjectType; break; //has no successor
  }

  //replace old object with new one
  D3DXVECTOR3 location=object->m_structLocation; //location
  float xspeed = object->m_fXspeed;
  float yspeed = object->m_fYspeed;

  kill(index); //kill old object
  if(successor) //if it has a successor
    create(newtype, name, location, xspeed, yspeed); //create new one
}

/// Replace player object by the next in the appropriate series (object, exploding
/// object, dead object). 
/// \param index index of the object to be replaced

void CObjectManager::replaceplayer(){ //replace player object by next in series

  ObjectType newtype;
  BOOL successor=TRUE; //assume it has a successor
  const int namelength = 64;
  char name[namelength]; //name for new object in XML tag

  //decide on new object type
  switch(m_pPlayerObject->m_nObjectType){
    //YOUR CODE GOES HERE
    default: successor = FALSE; break; //has no successor
  }

  //replace old object with new one
  D3DXVECTOR3 location = m_pPlayerObject->m_structLocation; //save location
  float xspeed = m_pPlayerObject->m_fXspeed; //save speed
  float yspeed = m_pPlayerObject->m_fYspeed; //save speed

  delete m_pPlayerObject; //it's gone, that's why we saved location and speed
  m_pPlayerObject = 
    new CGameObject(newtype, name, location, xspeed, yspeed);
}

/// Master collision detection function.
/// Compare every object against every other object for collision. Only
/// bullets can collide right now.

void CObjectManager::CollisionDetection(){
 //YOUR CODE GOES HERE
}

/// Collision detection helper function.
/// Given an object index, compare that object against every other 
/// object for collision. If a collision is detected, replace the object hit
/// with the next in series (if one exists), and kill the object doing the
/// hitting (bullets don't go through objects in this game).
/// \param index index of the object to compare against
/// \return TRUE if something collided with the object whose index is given

BOOL CObjectManager::CollisionDetection(CGameObject* object){ 
  BOOL finished = FALSE; //finished when collision detected
  for(int i=0; i < m_nMaxCount && !finished; i++){
    if(m_pObjectList[i]) //if i is a valid object index
      if(m_pObjectList[i]->m_bVulnerable && 
          distance(object, m_pObjectList[i]) < 15.0f){
        finished = TRUE; //hit found, so exit loop
        m_pObjectList[i]->m_fXspeed -= 10.0f; //add fake impulse to object hit
        replace(i); //replace object that is hit
      }
  }
  return finished;
}

/// Reset.
/// Resets the object manager to initial conditions. This code is needed to make the code
/// re-entrant so that we can re-enter the game engine from the menu screen.

void CObjectManager::Reset(){

  //member variables
  m_nCount = 0; m_nLastGunFireTime = 0;
  delete m_pPlayerObject; m_pPlayerObject = NULL;
  m_nStartInvulnerableTime = g_cTimer.time(); //start invulnerable

  //object list
  for(int i = 0; i < m_nMaxCount; i++){
    delete m_pObjectList[i];
    m_pObjectList[i] = NULL;
  }
}


/// Determine whether the player has won yet.
/// Counts the number of enemies remaining.
/// \return TRUE if there are no enemies left

BOOL CObjectManager::PlayerWon(){
    if((p1RoundsWon==2)&&(GameRenderer.second==102))
    {
        GameRenderer.winner = 1;
        return TRUE;
    }
    if((p2RoundsWon==2)&&(GameRenderer.second==102))
    {
        GameRenderer.winner = 2;
        return TRUE;
    }
    else
        return FALSE;
        //return TRUE; //switch with above to skip to success screen for testing
}


/// Set the player's speed.
/// \param xdelta player's new speed along the x axis
/// \param ydelta player's new speed along the y axis

void CObjectManager::SetSpeed(int player, int xdelta,int ydelta){
  if(player == 1)
    m_pPlayerObject->SetSpeed(xdelta, ydelta);
  else if(player == 2)
    m_pPlayer2Object->SetSpeed(xdelta, ydelta);
}

void CObjectManager::SetSpeedX(int player, int xdelta)
{
  if(player == 1)
    m_pPlayerObject->SetSpeed(xdelta, (int)m_pPlayerObject->m_fYspeed);
  else if(player == 2)
    m_pPlayer2Object->SetSpeed(xdelta, (int)m_pPlayer2Object->m_fYspeed);
}

void CObjectManager::SetSpeedY(int player, int ydelta)
{
  if(player == 1)
    m_pPlayerObject->SetSpeed((int)m_pPlayerObject->m_fXspeed, ydelta);
  else if(player == 2)
    m_pPlayer2Object->SetSpeed((int)m_pPlayer2Object->m_fXspeed, ydelta);
}

/// Get the player's speed limits.
/// Returns the minimum and maximum speed limits for the player along the x and y
/// axes using call-by-reference parameters for the return values.
/// \param xmin minimum speed along x axis
/// \param xmax maximum speed along x axis
/// \param ymin minimum speed along y axis
/// \param ymax maximum speed along y axis

void CObjectManager::GetSpeedLimits(int &xmin, int &xmax, int &ymin, int &ymax){
//return speed limits on current object
  xmin = m_pPlayerObject->m_nMinXSpeed; 
  xmax = m_pPlayerObject->m_nMaxXSpeed;
  ymin = m_pPlayerObject->m_nMinYSpeed; 
  ymax = m_pPlayerObject->m_nMaxYSpeed;
}

/// Count the number of enemies remaining.
/// \return number of enemies left

int CObjectManager::enemies(){ //return number of enemies
  int count = 0; //how many enemies left
  for(int i=0; i<m_nMaxCount; i++)
    if(m_pObjectList[i] != NULL)
     switch(m_pObjectList[i]->m_nObjectType){
       case NPC_OBJECT:
         count++;
         break;
     }

  return count;
}

//void CObjectManager::airJump(int player, int speedY)
//{
//  if(player == 1 && m_pPlayerObject->airJumpsLeft > 0)
//  {
//    m_pPlayerObject->airJumpsLeft--;
//    if(g_pInputManager->p1LeftHeld == 1)
//    {
//      if(m_pPlayerObject->m_fXspeed < 0)  //if you're already moving left...
//        m_pPlayerObject->SetSpeed((int)m_pPlayerObject->m_fXspeed, speedY); //maintain that speed
//      else  //otherwise...
//        m_pPlayerObject->SetSpeed(-10, speedY); //Jump in that direction.
//    }
//    else if(g_pInputManager->p1RightHeld == 1)
//    {
//      if(m_pPlayerObject->m_fXspeed > 0)  //if you're already moving right...
//        m_pPlayerObject->SetSpeed((int)m_pPlayerObject->m_fXspeed, speedY); //maintain that speed
//      else  //otherwise...
//        m_pPlayerObject->SetSpeed(10, speedY); //Jump in that direction.
//    }
//    else
//      m_pPlayerObject->SetSpeed(0, speedY);
//  }
//  else if(player == 2 && m_pPlayer2Object->airJumpsLeft > 0)
//  {
//    m_pPlayer2Object->airJumpsLeft--;
//    if(g_pInputManager->p2LeftHeld == 1)
//    {
//      if(m_pPlayer2Object->m_fXspeed < 0)
//        m_pPlayer2Object->SetSpeed((int)m_pPlayer2Object->m_fXspeed, speedY);
//      else //otherwise...
//        m_pPlayer2Object->SetSpeed(-10, speedY);
//    }
//    else if(g_pInputManager->p2RightHeld == 1)
//    {
//      if(m_pPlayer2Object->m_fXspeed > 0)
//        m_pPlayer2Object->SetSpeed((int)m_pPlayer2Object->m_fXspeed, speedY);
//      else  //otherwise...
//        m_pPlayer2Object->SetSpeed(10, speedY);
//    }
//    else
//      m_pPlayer2Object->SetSpeed(0, speedY);
//  }
//}

//Direction here is not relative: it is either left or right.
void CObjectManager::dash(int player, int direction, int speedX)
{
  if(player == 1)
  {
    m_pPlayerObject->dash(direction);
    
  }
  else if(player == 2)
  {
    m_pPlayer2Object->dash(direction);
  }
}

//Again, direction here is not relative: 4 is left, 6 is right.
void CObjectManager::walk(int player, int direction)
{
  if(player == 1)
  {
    m_pPlayerObject->walk(direction);
  }
  else if(player == 2)
  {
    m_pPlayer2Object->walk(direction);
  }
}

int CObjectManager::getRemainingJumps(int player)
{
  if(player == 1)
    return m_pPlayerObject->airJumpsLeft;
  else if(player == 2)
    return m_pPlayer2Object->airJumpsLeft;
  else 
    return 0;
}

int CObjectManager::getRemainingAirDashes(int player)
{
  if(player == 1)
    return m_pPlayerObject->airDashesLeft;
  else if(player == 2)
    return m_pPlayer2Object->airDashesLeft;
  else
    return 0;
}

void CObjectManager::setAirBorne(int player)
{
  if(player == 1)
    m_pPlayerObject->airBorne = 1;
  else if(player == 2)
    m_pPlayer2Object->airBorne = 1;
}

int CObjectManager::getFacing(int player)
{
  if(player == 1)
    return m_pPlayerObject->facing;
  else if(player == 2)
    return m_pPlayer2Object->facing;
  else 
    return 6;
}

void CObjectManager::changeFacing(int player)
{
  if(player == 1)
  {
    if(m_pPlayerObject->facing == 6)
      m_pPlayerObject->facing = 4;
    else if(m_pPlayerObject->facing == 4)
      m_pPlayerObject->facing = 6;
  }

}

//directions here are not relative.
int CObjectManager::getDirHeld(int player, int direction)
{
  if(player == 1)
  {
    if(direction == 4)
    {
      return m_pPlayerObject->leftHeld;
    }
    else if(direction == 6)
    {
      return m_pPlayerObject->rightHeld;
    }
    else if(direction == 8)
    {
      return m_pPlayerObject->upHeld;
    }
    else if(direction == 2)
    {
      return m_pPlayerObject->downHeld;
    }
  }
  else if(player == 2)
  {
    if(direction == 4)
    {
      return m_pPlayer2Object->leftHeld;
    }
    else if(direction == 6)
    {
      return m_pPlayer2Object->rightHeld;
    }
    else if(direction == 8)
    {
      return m_pPlayer2Object->upHeld;
    }
    else if(direction == 2)
    {
      return m_pPlayer2Object->downHeld;
    }
  }
  else
    return -1;
}

void CObjectManager::setDirHeld(int player, int direction, int value)
{
  if(player == 1)
  {
    if(direction == 4)
    {
      m_pPlayerObject->leftHeld = value;
    }
    else if(direction == 6)
    {
      m_pPlayerObject->rightHeld = value;
    }
    else if(direction == 8)
    {
      m_pPlayerObject->upHeld = value;
    }
    else if(direction == 2)
    {
      m_pPlayerObject->downHeld = value;
    }
  }
  else if(player == 2)
  {
    if(direction == 4)
    {
      m_pPlayer2Object->leftHeld = value;
    }
    else if(direction == 6)
    {
      m_pPlayer2Object->rightHeld = value;
    }
    else if(direction == 8)
    {
      m_pPlayer2Object->upHeld = value;
    }
    else if(direction == 2)
    {
      m_pPlayer2Object->downHeld = value;
    }
  }
}

void CObjectManager::stop(int player)
{
  if(player == 1)
    m_pPlayerObject->stop();
  else if(player == 2)
    m_pPlayer2Object->stop();
}

void CObjectManager::jump(int player)
{
  if(player == 1)
    m_pPlayerObject->jump();
  else if(player == 2)
    m_pPlayer2Object->jump();
}
//returns current speed in specific axis.
/// \param axis 0 for x axis, 1 for y axis
//float getCurrentSpeed(int axis)
//{
// 
//  if(axis == 0)
//  {
//    return *m_pPlayerObject->m_fXspeed;
//  }
//}


//the getDI() function will return an integer value representing the directional input of the calling player object, restricting all possible button combinations into the 9 values of an 8-way DI pad
//a useful, real-time function to get player input, it will allow us to avoid complicated if-statement chains and solves the problem of inputs that don't make sense (such as up+down simultaneously)
//(we can replicate this function later the manager the button inputs, or consolidate both integer representations to getDI()*10+getBI, giving a 1 through 99 representation of input state)
//UPDATE -- I just realized patrick wrote a similar function for this in the Input.cpp file...I'm not sure which one will be more useful/efficient, so we can keep both for now.
int CObjectManager::getDI(int player){

    int val = 0;

    /*
    increase the size of the following arrays if more buttons need to be added...the easiest way to do this was just to sort of 'code' the numbers in binary and then use that
    representation to translate the current state into an integer than could be then used in a switch statement. we may want to use this kind of representation for the overall input held states,
    cause its clever and easy :)
    the only downside is that if more than two buttons are held the switch statement doesn't respond correctly
    */

    if(player==1){
        int dirAr[4] = {m_pPlayerObject->downHeld, m_pPlayerObject->leftHeld, m_pPlayerObject->rightHeld, m_pPlayerObject->upHeld};
        val = dirAr[0] + (2*dirAr[1]) + (4*dirAr[2]) + (8*dirAr[3]);
    }

    if(player==2){
        int dirAr[4] = {m_pPlayer2Object->downHeld, m_pPlayer2Object->leftHeld, m_pPlayer2Object->rightHeld, m_pPlayer2Object->upHeld};
        val = dirAr[0] + (2*dirAr[1]) + (4*dirAr[2]) + (8*dirAr[3]);
    }

    //val is the binary to decimal representation of the button hold values
        switch(val){
        case 0:
            tempDI = 5;
            break;
        case 3:
            tempDI = 1;
            break;
        case 1:
            tempDI = 2;
            break;
        case 5:
            tempDI = 3;
            break;
        case 2:
            tempDI = 4;
            break;
        case 4:
            tempDI = 6;
            break;
        case 10:
            tempDI = 7;
            break;
        case 8:
            tempDI = 8;
            break;
        case 12:
            tempDI = 9;
            break;
        default:
            break;
        }

    return tempDI;

}

void CObjectManager::setAnimationState(int player, int state)
{
  if(player == 1)
  {
    m_pPlayerObject->setCurrentAnimation(state);
  }
  else if(player == 2)
  {
    m_pPlayer2Object->setCurrentAnimation(state);
  }
}

//type 0 = Check collision boxes
//type 1 = check hitboxes
//The reason the names for the last two arguements are so odd is because they are serving a double purpose. 
//When the function is called with collision type 0, they act as variables for passing the player that is 
//moving and the direction of that movement to the function.  On the other hand, if type is 1, they are used
//for a totally different purpose, since neither of the two previous things are necessary.  As such, 
//playerOrHurtBoxSwitch will be 0 or 1, 0 indicating to check the primary hurtbox array, and 1 indicating
//to check the secondary hurtbox array.  If it is 1, directionOrHurtBoxIndex gives the index in that array
//to check.
void CObjectManager::detectCollisions(int type, int playerOrHurtBoxSwitch, int directionOrHurtBoxIndex)
{
  float p1X = m_pPlayerObject->m_structLocation.x;
  float p1Y = m_pPlayerObject->m_structLocation.y;
  float p2X = m_pPlayer2Object->m_structLocation.x;
  float p2Y = m_pPlayer2Object->m_structLocation.y;
  if(type == 0)
  {
    int cs1 = m_pPlayerObject->m_nCollisionState;
		int cs2 = m_pPlayer2Object->m_nCollisionState;	//Tells us which collision box to check for which character.
    float p1LeftBorder = p1X + ((m_pPlayerObject->facing-5)*(m_pPlayerObject->m_pCollisionBoxes[cs1].xOffset))-(.5*(m_pPlayerObject->m_pCollisionBoxes[cs1].xSize*128));
    float p1RightBorder = p1X + ((m_pPlayerObject->facing-5)*(m_pPlayerObject->m_pCollisionBoxes[cs1].xOffset))+(.5*(m_pPlayerObject->m_pCollisionBoxes[cs1].xSize*128));
    float p1TopBorder = p1Y + m_pPlayerObject->m_pCollisionBoxes[cs1].yOffset+(.5*(m_pPlayerObject->m_pCollisionBoxes[cs1].ySize*128));
    float p1BottomBorder = p1Y + m_pPlayerObject->m_pCollisionBoxes[cs1].yOffset-(.5*(m_pPlayerObject->m_pCollisionBoxes[cs1].ySize*128));

    float p2LeftBorder = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->m_pCollisionBoxes[cs2].xOffset))-(.5*(m_pPlayer2Object->m_pCollisionBoxes[cs2].xSize*128));
    float p2RightBorder = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->m_pCollisionBoxes[cs2].xOffset))+(.5*(m_pPlayer2Object->m_pCollisionBoxes[cs2].xSize*128));
    float p2TopBorder = p2Y + m_pPlayer2Object->m_pCollisionBoxes[cs2].yOffset+(.5*(m_pPlayer2Object->m_pCollisionBoxes[cs2].ySize*128));
    float p2BottomBorder = p2Y + m_pPlayer2Object->m_pCollisionBoxes[cs2].yOffset-(.5*(m_pPlayer2Object->m_pCollisionBoxes[cs2].ySize*128));

    //if p1LeftBoundary is left of p2RightBoundary
    if(p1LeftBorder < p2RightBorder && p1RightBorder > p2LeftBorder && p1TopBorder > p2BottomBorder &&
        p1BottomBorder < p2TopBorder)
    {
      handleCollision(type, playerOrHurtBoxSwitch, directionOrHurtBoxIndex, p1LeftBorder, p1RightBorder, p2LeftBorder, p2RightBorder);
    }
  }
  else if(type == 1)  //For hit/hurtbox detection.
  {
    int p1Frame = m_pPlayerObject->getCurrentAnimationImageNum();
		int p2Frame = m_pPlayer2Object->getCurrentAnimationImageNum();
    m_nNumBoxes = 0;
    if((m_pPlayerObject->m_pHitBoxes[p1Frame].boxPresent == TRUE && m_pPlayerObject->m_pHitBoxes[p1Frame].active == TRUE && m_pPlayer2Object->m_pHurtBoxes[p2Frame].boxPresent == TRUE) || (m_pPlayer2Object->m_pHitBoxes[p2Frame].boxPresent == TRUE && m_pPlayer2Object->m_pHitBoxes[p2Frame].active == TRUE && m_pPlayerObject->m_pHurtBoxes[p1Frame].boxPresent == TRUE))
    {
      /*float p1HurtLeftBorder = p1X + ((m_pPlayerObject->facing-5)*(m_pPlayerObject->hurtBox.xOffset))-(.5*(m_pPlayerObject->hurtBox.xSize*128));
      float p1HurtRightBorder = p1X + ((m_pPlayerObject->facing-5)*(m_pPlayerObject->hurtBox.xOffset))+(.5*(m_pPlayerObject->hurtBox.xSize*128));
      float p1HurtTopBorder = p1Y + m_pPlayerObject->hurtBox.yOffset+(.5*(m_pPlayerObject->hurtBox.ySize*128));
      float p1HurtBottomBorder = p1Y + m_pPlayerObject->hurtBox.yOffset-(.5*(m_pPlayerObject->hurtBox.ySize*128));*/

      /*float p2HurtLeftBorder = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->hurtBox.xOffset))-(.5*(m_pPlayer2Object->hurtBox.xSize*128));
      float p2HurtRightBorder = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->hurtBox.xOffset))+(.5*(m_pPlayer2Object->hurtBox.xSize*128));
      float p2HurtTopBorder = p2Y + m_pPlayer2Object->hurtBox.yOffset+(.5*(m_pPlayer2Object->hurtBox.ySize*128));
      float p2HurtBottomBorder = p2Y + m_pPlayer2Object->hurtBox.yOffset-(.5*(m_pPlayer2Object->hurtBox.ySize*128));*/

      if(m_pPlayerObject->m_pHitBoxes[p1Frame].boxPresent == TRUE && m_pPlayerObject->m_pHitBoxes[p1Frame].active == TRUE)
      {
        //float p2HurtLeftBorder 
        m_pCollisionHurtBoxStorage[m_nNumBoxes].left = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->m_pHurtBoxes[p2Frame].xOffset))-(.5*(m_pPlayer2Object->m_pHurtBoxes[p2Frame].xSize*128));
        //float p2HurtRightBorder = 
        m_pCollisionHurtBoxStorage[m_nNumBoxes].right = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->m_pHurtBoxes[p2Frame].xOffset))+(.5*(m_pPlayer2Object->m_pHurtBoxes[p2Frame].xSize*128));
        //float p2HurtTopBorder 
        m_pCollisionHurtBoxStorage[m_nNumBoxes].top = p2Y + m_pPlayer2Object->m_pHurtBoxes[p2Frame].yOffset+(.5*(m_pPlayer2Object->m_pHurtBoxes[p2Frame].ySize*128));
        //float p2HurtBottomBorder
        m_pCollisionHurtBoxStorage[m_nNumBoxes].bottom = p2Y + m_pPlayer2Object->m_pHurtBoxes[p2Frame].yOffset-(.5*(m_pPlayer2Object->m_pHurtBoxes[p2Frame].ySize*128));
        m_nNumBoxes++;
        int moreHurtBoxes = m_pPlayer2Object->m_pHurtBoxes[p2Frame].secondaryBox;
        while(moreHurtBoxes >= 0 && m_nNumBoxes < 5)
        {
          m_pCollisionHurtBoxStorage[m_nNumBoxes].left = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->m_pSecondaryHurtBoxes[moreHurtBoxes].xOffset))-(.5*(m_pPlayer2Object->m_pSecondaryHurtBoxes[moreHurtBoxes].xSize*128));
          m_pCollisionHurtBoxStorage[m_nNumBoxes].right = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->m_pSecondaryHurtBoxes[moreHurtBoxes].xOffset))+(.5*(m_pPlayer2Object->m_pSecondaryHurtBoxes[moreHurtBoxes].xSize*128));
          m_pCollisionHurtBoxStorage[m_nNumBoxes].top = p2Y + m_pPlayer2Object->m_pSecondaryHurtBoxes[moreHurtBoxes].yOffset+(.5*(m_pPlayer2Object->m_pSecondaryHurtBoxes[moreHurtBoxes].ySize*128));
          m_pCollisionHurtBoxStorage[m_nNumBoxes].bottom = p2Y + m_pPlayer2Object->m_pSecondaryHurtBoxes[moreHurtBoxes].yOffset-(.5*(m_pPlayer2Object->m_pSecondaryHurtBoxes[moreHurtBoxes].ySize*128));
          m_nNumBoxes++;
          moreHurtBoxes = m_pPlayer2Object->m_pSecondaryHurtBoxes[moreHurtBoxes].secondaryBox;
        }
        float p1HitLeftBorder = p1X + ((m_pPlayerObject->facing-5)*(m_pPlayerObject->m_pHitBoxes[p1Frame].xOffset))-(.5*(m_pPlayerObject->m_pHitBoxes[p1Frame].xSize*128));
        float p1HitRightBorder = p1X + ((m_pPlayerObject->facing-5)*(m_pPlayerObject->m_pHitBoxes[p1Frame].xOffset))+(.5*(m_pPlayerObject->m_pHitBoxes[p1Frame].xSize*128));
        float p1HitTopBorder = p1Y + m_pPlayerObject->m_pHitBoxes[p1Frame].yOffset+(.5*(m_pPlayerObject->m_pHitBoxes[p1Frame].ySize*128));
        float p1HitBottomBorder = p1Y + m_pPlayerObject->m_pHitBoxes[p1Frame].yOffset-(.5*(m_pPlayerObject->m_pHitBoxes[p1Frame].ySize*128));
        int moreHitBoxes = m_pPlayerObject->m_pHitBoxes[p1Frame].secondaryBox;
        //Now, check the hitbox we just found against each box in collision storage.
        for(int a = 0; a < m_nNumBoxes; a++)
        {
          if(p1HitLeftBorder < m_pCollisionHurtBoxStorage[a].right && p1HitRightBorder > m_pCollisionHurtBoxStorage[a].left && p1HitTopBorder > m_pCollisionHurtBoxStorage[a].bottom &&
          p1HitBottomBorder < m_pCollisionHurtBoxStorage[a].top)
          {
            //handleCollision(type, player, direction, p1LeftBorder, p1RightBorder, p2LeftBorder, p2RightBorder);
            if(m_pPlayerObject->facing==4)
                GameRenderer.DrawHitSpark(p1HitLeftBorder, m_pCollisionHurtBoxStorage[a].right, p1HitBottomBorder, p1HitTopBorder); //zhit
            else
                GameRenderer.DrawHitSpark(m_pCollisionHurtBoxStorage[a].left, p1HitRightBorder, p1HitBottomBorder, p1HitTopBorder);
            m_pPlayerObject->expendHitBox();
           // m_pPlayer2Object->m_fYspeed = -5;
            moreHitBoxes = -1;  //Don't check more hitboxes
            a = m_nNumBoxes;  //Don't check against more hurtboxes.  
          }
        }
        if(moreHitBoxes >= 0) //This only runs if the initial box didnt connect.  only one box needs to
        { //connect for a hit to be confirmed.
          while(moreHitBoxes >= 0)
          {
            p1HitLeftBorder = p1X + ((m_pPlayerObject->facing-5)*(m_pPlayerObject->m_pSecondaryHitBoxes[moreHitBoxes].xOffset))-(.5*(m_pPlayerObject->m_pSecondaryHitBoxes[moreHitBoxes].xSize*128));
            p1HitRightBorder = p1X + ((m_pPlayerObject->facing-5)*(m_pPlayerObject->m_pSecondaryHitBoxes[moreHitBoxes].xOffset))+(.5*(m_pPlayerObject->m_pSecondaryHitBoxes[moreHitBoxes].xSize*128));
            p1HitTopBorder = p1Y + m_pPlayerObject->m_pSecondaryHitBoxes[moreHitBoxes].yOffset+(.5*(m_pPlayerObject->m_pSecondaryHitBoxes[moreHitBoxes].ySize*128));
            p1HitBottomBorder = p1Y + m_pPlayerObject->m_pSecondaryHitBoxes[moreHitBoxes].yOffset-(.5*(m_pPlayerObject->m_pSecondaryHitBoxes[moreHitBoxes].ySize*128));
            moreHitBoxes = m_pPlayerObject->m_pSecondaryHitBoxes[moreHitBoxes].secondaryBox;
            for(int a = 0; a < m_nNumBoxes; a++)
            {
              if(p1HitLeftBorder < m_pCollisionHurtBoxStorage[a].right && p1HitRightBorder > m_pCollisionHurtBoxStorage[a].left && p1HitTopBorder > m_pCollisionHurtBoxStorage[a].bottom &&
              p1HitBottomBorder < m_pCollisionHurtBoxStorage[a].top)
              {
                //handleCollision(type, player, direction, p1LeftBorder, p1RightBorder, p2LeftBorder, p2RightBorder);
                if(m_pPlayerObject->facing==4)
                    GameRenderer.DrawHitSpark(p1HitLeftBorder, m_pCollisionHurtBoxStorage[a].right, p1HitBottomBorder, p1HitTopBorder); //zhit
                else
                    GameRenderer.DrawHitSpark(m_pCollisionHurtBoxStorage[a].left, p1HitRightBorder, p1HitBottomBorder, p1HitTopBorder);
                m_pPlayerObject->expendHitBox();
               // m_pPlayer2Object->m_fYspeed = -5;
                moreHitBoxes = -1;  //Don't check more hitboxes
                a = m_nNumBoxes;  //Don't check against more hurtboxes.  
              }
            }
            
          }
        }
      }
      m_nNumBoxes = 0;
      if(m_pPlayer2Object->m_pHitBoxes[p2Frame].boxPresent == TRUE && m_pPlayer2Object->m_pHitBoxes[p2Frame].active == TRUE)
      {
        //============
        //new code====
        //============
        //float p2HurtLeftBorder 
        m_pCollisionHurtBoxStorage[m_nNumBoxes].left = p1X + ((m_pPlayerObject->facing-5)*(m_pPlayerObject->m_pHurtBoxes[p1Frame].xOffset))-(.5*(m_pPlayerObject->m_pHurtBoxes[p1Frame].xSize*128));
        //float p2HurtRightBorder = 
        m_pCollisionHurtBoxStorage[m_nNumBoxes].right = p1X + ((m_pPlayerObject->facing-5)*(m_pPlayerObject->m_pHurtBoxes[p1Frame].xOffset))+(.5*(m_pPlayerObject->m_pHurtBoxes[p1Frame].xSize*128));
        //float p2HurtTopBorder 
        m_pCollisionHurtBoxStorage[m_nNumBoxes].top = p1Y + m_pPlayerObject->m_pHurtBoxes[p1Frame].yOffset+(.5*(m_pPlayerObject->m_pHurtBoxes[p1Frame].ySize*128));
        //float p2HurtBottomBorder
        m_pCollisionHurtBoxStorage[m_nNumBoxes].bottom = p1Y + m_pPlayerObject->m_pHurtBoxes[p1Frame].yOffset-(.5*(m_pPlayerObject->m_pHurtBoxes[p1Frame].ySize*128));
        m_nNumBoxes++;
        int moreHurtBoxes = m_pPlayerObject->m_pHurtBoxes[p1Frame].secondaryBox;
        while(moreHurtBoxes >= 0 && m_nNumBoxes < 5)
        {
          m_pCollisionHurtBoxStorage[m_nNumBoxes].left = p1X + ((m_pPlayerObject->facing-5)*(m_pPlayerObject->m_pSecondaryHurtBoxes[moreHurtBoxes].xOffset))-(.5*(m_pPlayerObject->m_pSecondaryHurtBoxes[moreHurtBoxes].xSize*128));
          m_pCollisionHurtBoxStorage[m_nNumBoxes].right = p1X + ((m_pPlayerObject->facing-5)*(m_pPlayerObject->m_pSecondaryHurtBoxes[moreHurtBoxes].xOffset))+(.5*(m_pPlayerObject->m_pSecondaryHurtBoxes[moreHurtBoxes].xSize*128));
          m_pCollisionHurtBoxStorage[m_nNumBoxes].top = p1Y + m_pPlayerObject->m_pSecondaryHurtBoxes[moreHurtBoxes].yOffset+(.5*(m_pPlayerObject->m_pSecondaryHurtBoxes[moreHurtBoxes].ySize*128));
          m_pCollisionHurtBoxStorage[m_nNumBoxes].bottom = p1Y + m_pPlayerObject->m_pSecondaryHurtBoxes[moreHurtBoxes].yOffset-(.5*(m_pPlayerObject->m_pSecondaryHurtBoxes[moreHurtBoxes].ySize*128));
          m_nNumBoxes++;
          moreHurtBoxes = m_pPlayerObject->m_pSecondaryHurtBoxes[moreHurtBoxes].secondaryBox;
        }
        float p2HitLeftBorder = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->m_pHitBoxes[p2Frame].xOffset))-(.5*(m_pPlayer2Object->m_pHitBoxes[p2Frame].xSize*128));
        float p2HitRightBorder = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->m_pHitBoxes[p2Frame].xOffset))+(.5*(m_pPlayer2Object->m_pHitBoxes[p2Frame].xSize*128));
        float p2HitTopBorder = p2Y + m_pPlayer2Object->m_pHitBoxes[p2Frame].yOffset+(.5*(m_pPlayer2Object->m_pHitBoxes[p2Frame].ySize*128));
        float p2HitBottomBorder = p2Y + m_pPlayer2Object->m_pHitBoxes[p2Frame].yOffset-(.5*(m_pPlayer2Object->m_pHitBoxes[p2Frame].ySize*128));
        int moreHitBoxes = m_pPlayer2Object->m_pHitBoxes[p2Frame].secondaryBox;
        //Now, check the hitbox we just found against each box in collision storage.
        for(int a = 0; a < m_nNumBoxes; a++)
        {
          if(p2HitLeftBorder < m_pCollisionHurtBoxStorage[a].right && p2HitRightBorder > m_pCollisionHurtBoxStorage[a].left &&
            p2HitTopBorder > m_pCollisionHurtBoxStorage[a].bottom && p2HitBottomBorder < m_pCollisionHurtBoxStorage[a].top)
          {
            //handleCollision(type, player, direction, p1LeftBorder, p1RightBorder, p2LeftBorder, p2RightBorder);
                if(m_pPlayer2Object->facing==4)
                    GameRenderer.DrawHitSpark(p2HitLeftBorder, m_pCollisionHurtBoxStorage[a].right, p2HitBottomBorder, p2HitTopBorder); //zhit
                else
                    GameRenderer.DrawHitSpark(m_pCollisionHurtBoxStorage[a].left, p2HitRightBorder, p2HitBottomBorder, p2HitTopBorder);
                m_pPlayer2Object->expendHitBox();
            //m_pPlayerObject->m_fYspeed = -5;
            moreHitBoxes = -1;  //Don't check more hitboxes
            a = m_nNumBoxes;  //Don't check against more hurtboxes.  
          }
        }
        if(moreHitBoxes >= 0) //This only runs if the initial box didnt connect.  only one box needs to
        { //connect for a hit to be confirmed.
          while(moreHitBoxes >= 0)
          {
            p2HitLeftBorder = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->m_pSecondaryHitBoxes[moreHitBoxes].xOffset))-(.5*(m_pPlayer2Object->m_pSecondaryHitBoxes[moreHitBoxes].xSize*128));
            p2HitRightBorder = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->m_pSecondaryHitBoxes[moreHitBoxes].xOffset))+(.5*(m_pPlayer2Object->m_pSecondaryHitBoxes[moreHitBoxes].xSize*128));
            p2HitTopBorder = p2Y + m_pPlayer2Object->m_pSecondaryHitBoxes[moreHitBoxes].yOffset+(.5*(m_pPlayer2Object->m_pSecondaryHitBoxes[moreHitBoxes].ySize*128));
            p2HitBottomBorder = p2Y + m_pPlayer2Object->m_pSecondaryHitBoxes[moreHitBoxes].yOffset-(.5*(m_pPlayer2Object->m_pSecondaryHitBoxes[moreHitBoxes].ySize*128));
            moreHitBoxes = m_pPlayer2Object->m_pSecondaryHitBoxes[moreHitBoxes].secondaryBox;
            for(int a = 0; a < m_nNumBoxes; a++)
            {
              if(p2HitLeftBorder < m_pCollisionHurtBoxStorage[a].right && p2HitRightBorder > m_pCollisionHurtBoxStorage[a].left && 
                p2HitTopBorder > m_pCollisionHurtBoxStorage[a].bottom && p2HitBottomBorder < m_pCollisionHurtBoxStorage[a].top)
              {
                //handleCollision(type, player, direction, p1LeftBorder, p1RightBorder, p2LeftBorder, p2RightBorder);
                if(m_pPlayer2Object->facing==4)
                    GameRenderer.DrawHitSpark(p2HitLeftBorder, m_pCollisionHurtBoxStorage[a].right, p2HitBottomBorder, p2HitTopBorder); //zhit
                else
                    GameRenderer.DrawHitSpark(m_pCollisionHurtBoxStorage[a].left, p2HitRightBorder, p2HitBottomBorder, p2HitTopBorder);
                m_pPlayer2Object->expendHitBox();
                //m_pPlayerObject->m_fYspeed = -5;
                moreHitBoxes = -1;  //Don't check more hitboxes
                a = m_nNumBoxes;  //Don't check against more hurtboxes.  
              }
            }
            
          }
        }
        //===============
        //end new code===
        //===============
        //float p2HitLeftBorder = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->m_pHitBoxes[p2Frame].xOffset))-(.5*(m_pPlayer2Object->m_pHitBoxes[p2Frame].xSize*128));
        //float p2HitRightBorder = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->m_pHitBoxes[p2Frame].xOffset))+(.5*(m_pPlayer2Object->m_pHitBoxes[p2Frame].xSize*128));
        //float p2HitTopBorder = p2Y + m_pPlayer2Object->m_pHitBoxes[p2Frame].yOffset+(.5*(m_pPlayer2Object->m_pHitBoxes[p2Frame].ySize*128));
        //float p2HitBottomBorder = p2Y + m_pPlayer2Object->m_pHitBoxes[p2Frame].yOffset-(.5*(m_pPlayer2Object->m_pHitBoxes[p2Frame].ySize*128));
        //int tempInt = m_pPlayer2Object->m_pHitBoxes[p2Frame].secondaryBox;

        //if(p1HurtLeftBorder < p2HitRightBorder && p1HurtRightBorder > p2HitLeftBorder && p1HurtTopBorder > p2HitBottomBorder &&
        //p1HurtBottomBorder < p2HitTopBorder)
        //{
        //  //handleCollision(type, player, direction, p1LeftBorder, p1RightBorder, p2LeftBorder, p2RightBorder);
        //  m_pPlayer2Object->expendHitBox();
        //  m_pPlayerObject->m_fYspeed = -5;
        //  tempInt = -1
        //  
        //}
        //else if(tempInt >= 0) //This only runs if the initial box didnt connect.  only one box needs to
        //{ //connect for a hit to be confirmed.
        //  while(tempInt >= 0)
        //  {
        //    p2HitLeftBorder = p2X + ((m_pPlayer2Object->facing-5)*(m_pPlayer2Object->m_pSecondaryHitBoxes[tempInt].xOffset))-(.5*(m_pPlayer2Object->m_pSecondaryHitBoxes[tempInt].xSize*128));
        //    p2HitRightBorder = p2X + ((m_pPlayerObject->facing-5)*(m_pPlayer2Object->m_pSecondaryHitBoxes[tempInt].xOffset))+(.5*(m_pPlayer2Object->m_pSecondaryHitBoxes[tempInt].xSize*128));
        //    p2HitTopBorder = p2Y + m_pPlayer2Object->m_pSecondaryHitBoxes[tempInt].yOffset+(.5*(m_pPlayer2Object->m_pSecondaryHitBoxes[tempInt].ySize*128));
        //    p2HitBottomBorder = p2Y + m_pPlayer2Object->m_pSecondaryHitBoxes[tempInt].yOffset-(.5*(m_pPlayer2Object->m_pSecondaryHitBoxes[tempInt].ySize*128));
        //    tempInt = m_pPlayer2Object->m_pSecondaryHitBoxes[tempInt].secondaryBox;
        //    if(p1HurtLeftBorder < p2HitRightBorder && p1HurtRightBorder > p2HitLeftBorder && p1HurtTopBorder > p2HitBottomBorder &&
        //    p1HurtBottomBorder < p2HitTopBorder)
        //    {
        //      //handleCollision(type, player, direction, p1LeftBorder, p1RightBorder, p2LeftBorder, p2RightBorder);
        //      m_pPlayer2Object->expendHitBox();
        //      m_pPlayerObject->m_fYspeed = -5;
        //      tempInt = -1;
        //    }
        //  }
        //}
      }
    }
  }
}

void CObjectManager::handleCollision(int type, int player, int direction, float p1LB, float p1RB, float p2LB, float p2RB)
{
  //Bit of a foreword here:  Yes, I realize that we could essentially use the case for direction == 0 for all
  //of these, but the problem there is if you have a lag spike or bad enough frame rate with a fast enough
  //character, he can potentially move far enough into the other character in between a frame that the
  //algorithm assumes him to be on the other side, effectively making him run straight through the other
  //player.  To avoid this, I made the other two cases that rely on the direction of movement instead of
  //simply the distance between the boxes to determine which player is on which side.
  if(type == 0)
  {
    if(player == 1)
    {
      if(direction == 4)
      {
        //find distance between p1 left and p2 right...
        float distance = abs(p1LB-p2RB)/2;
        //and move them away from eachother by that distance
        //if(!(m_structLocation.x + xdelta >= 2*g_nScreenWidth) && !(m_structLocation.x  + xdelta <= 0))//(stage has a range of 0 to 2g_nScreenWidth)
        if(m_pPlayerObject->m_structLocation.x+distance >= 2*g_nScreenWidth)  //if player would go offscreen...
        {
          m_pPlayerObject->m_structLocation.x=2*g_nScreenWidth; //put him at end of screen instead.
          distance*=2;
        }
        else
          m_pPlayerObject->m_structLocation.x+=distance;
        if(m_pPlayer2Object->m_structLocation.x-distance <= 0)
        {
          m_pPlayer2Object->m_structLocation.x = 0;
          m_pPlayerObject->m_structLocation.x+=distance;
        }
        else
          m_pPlayer2Object->m_structLocation.x-=distance;
      }
      else if(direction == 6)
      {
        //find distance between p1 right and p2 left...
        float distance = abs(p1RB-p2LB)/2;
        //more of the same.
        if(m_pPlayerObject->m_structLocation.x-distance <= 0)
        {
          m_pPlayerObject->m_structLocation.x = 0;
          distance*=2;
        }
        else
          m_pPlayerObject->m_structLocation.x-=distance;
        if(m_pPlayer2Object->m_structLocation.x+distance >= 2*g_nScreenWidth)
        {
          m_pPlayer2Object->m_structLocation.x = 2*g_nScreenWidth;
          m_pPlayerObject->m_structLocation.x-=distance;
        }
        else
          m_pPlayer2Object->m_structLocation.x+=distance;
      }
      else if(direction == 0)
      {
        float distance1 = abs(p1RB-p2LB)/2;
        float distance2 = abs(p1LB-p2RB)/2;
        if(distance1 < distance2)
        {
          if(m_pPlayerObject->m_structLocation.x-distance1 <= 0)
          {
            m_pPlayerObject->m_structLocation.x = 0;
            distance1*=2;
          }
          else
            m_pPlayerObject->m_structLocation.x-=distance1;
          if(m_pPlayer2Object->m_structLocation.x+distance1 >= 2*g_nScreenWidth)
          {
            m_pPlayer2Object->m_structLocation.x = 2*g_nScreenWidth;
            m_pPlayerObject->m_structLocation.x-=distance1;
          }
          else
            m_pPlayer2Object->m_structLocation.x+=distance1;
         }
        else
        {
          if(m_pPlayerObject->m_structLocation.x+distance2 >= 2*g_nScreenWidth)  //if player would go offscreen...
          {
            m_pPlayerObject->m_structLocation.x=2*g_nScreenWidth; //put him at end of screen instead.
            distance2*=2;
          }
          else
            m_pPlayerObject->m_structLocation.x+=distance2;
          if(m_pPlayer2Object->m_structLocation.x-distance2 <= 0)
          {
            m_pPlayer2Object->m_structLocation.x = 0;
            m_pPlayerObject->m_structLocation.x+=distance2;
          }
          else
            m_pPlayer2Object->m_structLocation.x-=distance2;
        }
      }
    }
    else if(player == 2)
    {
      if(direction == 4)
      {
        //find distance between p1 left and p2 right...
        float distance = abs(p2LB-p1RB)/2;
        //and move them away from eachother by that distance
        //if(!(m_structLocation.x + xdelta >= 2*g_nScreenWidth) && !(m_structLocation.x  + xdelta <= 0))//(stage has a range of 0 to 2g_nScreenWidth)
        if(m_pPlayer2Object->m_structLocation.x+distance >= 2*g_nScreenWidth)  //if player would go offscreen...
        {
          m_pPlayer2Object->m_structLocation.x=2*g_nScreenWidth; //put him at end of screen instead.
          distance*=2;
        }
        else
          m_pPlayer2Object->m_structLocation.x+=distance;
        if(m_pPlayerObject->m_structLocation.x-distance <= 0)
        {
          m_pPlayerObject->m_structLocation.x = 0;
          m_pPlayer2Object->m_structLocation.x+=distance;
        }
        else
          m_pPlayerObject->m_structLocation.x-=distance;
      }
      else if(direction == 6)
      {
        //find distance between p1 right and p2 left...
        float distance = abs(p2RB-p1LB)/2;
        //more of the same.
        if(m_pPlayer2Object->m_structLocation.x-distance <= 0)
        {
          m_pPlayer2Object->m_structLocation.x = 0;
          distance*=2;
        }
        else
          m_pPlayer2Object->m_structLocation.x-=distance;
        if(m_pPlayerObject->m_structLocation.x+distance >= 2*g_nScreenWidth)
        {
          m_pPlayerObject->m_structLocation.x = 2*g_nScreenWidth;
          m_pPlayer2Object->m_structLocation.x-=distance;
        }
        else
          m_pPlayerObject->m_structLocation.x+=distance;
      }
      else if(direction == 0)
      {
        float distance1 = abs(p2RB-p1LB)/2;
        float distance2 = abs(p2LB-p1RB)/2;
        if(distance1 < distance2)
        {
          if(m_pPlayer2Object->m_structLocation.x-distance1 <= 0)
          {
            m_pPlayer2Object->m_structLocation.x = 0;
            distance1*=2;
          }
          else
            m_pPlayer2Object->m_structLocation.x-=distance1;
          if(m_pPlayerObject->m_structLocation.x+distance1 >= 2*g_nScreenWidth)
          {
            m_pPlayerObject->m_structLocation.x = 2*g_nScreenWidth;
            m_pPlayer2Object->m_structLocation.x-=distance1;
          }
          else
            m_pPlayerObject->m_structLocation.x+=distance1;
         }
        else
        {
          if(m_pPlayer2Object->m_structLocation.x+distance2 >= 2*g_nScreenWidth)  //if player would go offscreen...
          {
            m_pPlayer2Object->m_structLocation.x=2*g_nScreenWidth; //put him at end of screen instead.
            distance2*=2;
          }
          else
            m_pPlayer2Object->m_structLocation.x+=distance2;
          if(m_pPlayerObject->m_structLocation.x-distance2 <= 0)
          {
            m_pPlayerObject->m_structLocation.x = 0;
            m_pPlayer2Object->m_structLocation.x+=distance2;
          }
          else
            m_pPlayerObject->m_structLocation.x-=distance2;
        }
      }
    }
  }
}

void CObjectManager::hitChar(int hittingPlayer, int attackHeight, float hitStunDuration, float blockStunDuration, float xKnockBack, float yKnockBack, int dmg)
{
	if(hittingPlayer == 1)
	{
		BOOL gBounce = m_pPlayerObject->m_bCurrentAttackGroundBounce;
		BOOL wBounce = m_pPlayerObject->m_bCurrentAttackWallBounce;
		float bounceStun = m_pPlayerObject->m_fCurrentAttackPostBounceStun;
		if(m_pPlayer2Object->inStun() == TRUE)	//Can't block in hitstun.
		{
            m_pPlayer2Object->getHit(hitStunDuration, m_pPlayerObject->m_nCurrentHitLag, xKnockBack, yKnockBack, bounceStun, gBounce, wBounce);
            p1ComboCounter++;
            if(p1ComboCounter==1||p1ComboCounter==0)
                p2Health = p2Health - dmg;
            else
                p2Health = p2Health - ((dmg+(dmg/p1ComboCounter))/p1ComboCounter);
			m_pPlayerObject->hitNotice();
			m_pPlayer2Object->setPushBack(m_pPlayerObject->m_fCurrentAttackHitPushBackSpeed/2);
		}
		else
		{
			int tempDirHeld = g_pInputManager->checkDirection(2);
			if(attackHeight == 0)	//if its a low, and other player isn't blocking low.
			{
				if(tempDirHeld != 1) //and other player isn't blocking low.
				{
					m_pPlayer2Object->getHit(hitStunDuration, m_pPlayerObject->m_nCurrentHitLag, xKnockBack, yKnockBack, bounceStun, gBounce, wBounce);		//hit him
                    p1ComboCounter++;
            if(p1ComboCounter==1||p1ComboCounter==0)
                p2Health = p2Health - dmg;
            else
                p2Health = p2Health - ((dmg+(dmg/p1ComboCounter))/p1ComboCounter);
					m_pPlayerObject->hitNotice();
					m_pPlayer2Object->setPushBack(m_pPlayerObject->m_fCurrentAttackHitPushBackSpeed/2);
				}
				else	//else he's blocking
				{
					m_pPlayer2Object->block(blockStunDuration, m_pPlayerObject->m_nCurrentHitLag);
					m_pPlayerObject->attackBlockedNotice();
					m_pPlayer2Object->setPushBack(m_pPlayerObject->m_fCurrentAttackBlockPushBackSpeed/2);
				}
			}
			else if(attackHeight == 1)	//if its mid
			{
				if(tempDirHeld == 1 || tempDirHeld == 4 || tempDirHeld == 7)
				{
					m_pPlayer2Object->block(blockStunDuration, m_pPlayerObject->m_nCurrentHitLag);	//block!
					m_pPlayerObject->attackBlockedNotice();
					m_pPlayer2Object->setPushBack(m_pPlayerObject->m_fCurrentAttackBlockPushBackSpeed/2);
				}
				else //hit
				{
					m_pPlayer2Object->getHit(hitStunDuration, m_pPlayerObject->m_nCurrentHitLag, xKnockBack, yKnockBack, bounceStun, gBounce, wBounce);
                    p1ComboCounter++;
            if(p1ComboCounter==1||p1ComboCounter==0)
                p2Health = p2Health - dmg;
            else
                p2Health = p2Health - ((dmg+(dmg/p1ComboCounter))/p1ComboCounter);
					m_pPlayerObject->hitNotice();
					m_pPlayer2Object->setPushBack(m_pPlayerObject->m_fCurrentAttackHitPushBackSpeed/2);
				}
			}
			else if(attackHeight == 2)	//if its high
			{
				if(tempDirHeld != 4)
				{
					m_pPlayer2Object->getHit(hitStunDuration, m_pPlayerObject->m_nCurrentHitLag, xKnockBack, yKnockBack, bounceStun, gBounce, wBounce);		//hit him
                    p1ComboCounter++;
            if(p1ComboCounter==1||p1ComboCounter==0)
                p2Health = p2Health - dmg;
            else
                p2Health = p2Health - ((dmg+(dmg/p1ComboCounter))/p1ComboCounter);
					m_pPlayerObject->hitNotice();
					m_pPlayer2Object->setPushBack(m_pPlayerObject->m_fCurrentAttackHitPushBackSpeed/2);
				}
				else
				{
					m_pPlayer2Object->block(blockStunDuration, m_pPlayerObject->m_nCurrentHitLag);	//block
					m_pPlayerObject->attackBlockedNotice();
					m_pPlayer2Object->setPushBack(m_pPlayerObject->m_fCurrentAttackBlockPushBackSpeed/2);
				}
			}
		}
		
		
	}
	else if(hittingPlayer == 2)
	{
		BOOL gBounce = m_pPlayer2Object->m_bCurrentAttackGroundBounce;
		BOOL wBounce = m_pPlayer2Object->m_bCurrentAttackWallBounce;
		float bounceStun = m_pPlayer2Object->m_fCurrentAttackPostBounceStun;
		if(m_pPlayerObject->inStun() == TRUE)	//Can't block in hitstun.
		{
			m_pPlayerObject->getHit(hitStunDuration, m_pPlayer2Object->m_nCurrentHitLag, xKnockBack, yKnockBack, bounceStun, gBounce, wBounce);	
            p2ComboCounter++;
            if(p2ComboCounter==1||p2ComboCounter==0)
                p1Health = p1Health - dmg;
            else
                p1Health = p1Health - ((dmg+(dmg/p2ComboCounter))/p2ComboCounter);
			m_pPlayer2Object->hitNotice();
			m_pPlayerObject->setPushBack(m_pPlayer2Object->m_fCurrentAttackHitPushBackSpeed/2);
		}
		else
		{
			int tempDirHeld = g_pInputManager->checkDirection(1);
			if(attackHeight == 0)	//if its a low, and other player isn't blocking low.
			{
				if(tempDirHeld != 1) //and other player isn't blocking low.
				{
					m_pPlayerObject->getHit(hitStunDuration, m_pPlayer2Object->m_nCurrentHitLag, xKnockBack, yKnockBack, bounceStun, gBounce, wBounce);		//hit him
                    p2ComboCounter++;
            if(p2ComboCounter==1||p2ComboCounter==0)
                p1Health = p1Health - dmg;
            else
                p1Health = p1Health - ((dmg+(dmg/p2ComboCounter))/p2ComboCounter);
					m_pPlayer2Object->hitNotice();
					m_pPlayerObject->setPushBack(m_pPlayer2Object->m_fCurrentAttackHitPushBackSpeed/2);
				}
				else	//else he's blocking
				{
					m_pPlayerObject->block(blockStunDuration, m_pPlayer2Object->m_nCurrentHitLag);
					m_pPlayer2Object->attackBlockedNotice();
					m_pPlayerObject->setPushBack(m_pPlayer2Object->m_fCurrentAttackBlockPushBackSpeed/2);
				}
			}
			else if(attackHeight == 1)	//if its mid
			{
				if(tempDirHeld == 1 || tempDirHeld == 4 || tempDirHeld == 7)
				{
					m_pPlayerObject->block(blockStunDuration, m_pPlayer2Object->m_nCurrentHitLag);	//block!
					m_pPlayer2Object->attackBlockedNotice();
					m_pPlayerObject->setPushBack(m_pPlayer2Object->m_fCurrentAttackBlockPushBackSpeed/2);
				}
				else //hit
				{
					m_pPlayerObject->getHit(hitStunDuration, m_pPlayer2Object->m_nCurrentHitLag, xKnockBack, yKnockBack, bounceStun, gBounce, wBounce);
                    p2ComboCounter++;
            if(p2ComboCounter==1||p2ComboCounter==0)
                p1Health = p1Health - dmg;
            else
                p1Health = p1Health - ((dmg+(dmg/p2ComboCounter))/p2ComboCounter);
					m_pPlayer2Object->hitNotice();
					m_pPlayerObject->setPushBack(m_pPlayer2Object->m_fCurrentAttackHitPushBackSpeed/2);
				}
			}
			else if(attackHeight == 2)	//if its high
			{
				if(tempDirHeld != 4)
				{
					m_pPlayerObject->getHit(hitStunDuration, m_pPlayer2Object->m_nCurrentHitLag, xKnockBack, yKnockBack, bounceStun, gBounce, wBounce);		//hit him
                    p2ComboCounter++;
            if(p2ComboCounter==1||p2ComboCounter==0)
                p1Health = p1Health - dmg;
            else
                p1Health = p1Health - ((dmg+(dmg/p2ComboCounter))/p2ComboCounter);
					m_pPlayer2Object->hitNotice();
					m_pPlayerObject->setPushBack(m_pPlayer2Object->m_fCurrentAttackHitPushBackSpeed/2);
				}
				else
				{
					m_pPlayerObject->block(blockStunDuration, m_pPlayer2Object->m_nCurrentHitLag);	//block
					m_pPlayer2Object->attackBlockedNotice();
					m_pPlayerObject->setPushBack(m_pPlayer2Object->m_fCurrentAttackBlockPushBackSpeed/2);
				}
			}
		}
	}
}

void CObjectManager::passInputs(int player, int directional, int command)
{
	if(player == 1)
	{
		switch(command)
		{
			case 0:
				m_pPlayerObject->aTimer = g_cTimer.time();
				break;
			case 1:
				m_pPlayerObject->bTimer = g_cTimer.time();
				break;
			case 2:
				m_pPlayerObject->cTimer = g_cTimer.time();
				break;
		}
		m_pPlayerObject->interpretCommand(directional, command);
	}
	else if(player == 2)
	{
		switch(command)
		{
			case 0:
				m_pPlayer2Object->aTimer = g_cTimer.time();
				break;
			case 1:
				m_pPlayer2Object->bTimer = g_cTimer.time();
				break;
			case 2:
				m_pPlayer2Object->cTimer = g_cTimer.time();
				break;
		}
		m_pPlayer2Object->interpretCommand(directional, command);
	}
}

void CObjectManager::resetCounter(int player)
{
	if(player == 1)
    {
        if(p1ComboCounter>0)
            GameRenderer.p1ComboSlide[0] = p1ComboCounter;
		p1ComboCounter = 0;
    }
	else if(player == 2)
    {
        if(p2ComboCounter>0)
            GameRenderer.p2ComboSlide[0] = p2ComboCounter;
		p2ComboCounter = 0;
    }
}

void CObjectManager::crouch(int player)
{
	if(player == 1)
	{
		m_pPlayerObject->crouch();
	}
	else if(player == 2)
	{
		m_pPlayer2Object->crouch();
	}
}

void CObjectManager::stopCrouching(int player)
{
	if(player == 1)
	{
		m_pPlayerObject->stopCrouch();
	}
	else if(player == 2)
	{
		m_pPlayer2Object->stopCrouch();
	}
}

void CObjectManager::freezeGame()
{
	PauseGame = 1;
	m_pPlayerObject->freeze = 1;
	m_pPlayerObject->freezeTimer = 1000000000;
	m_pPlayerObject->freezeGap = g_cTimer.time()/* - m_pPlayerObject->m_nLastFrameTime*/;
	m_pPlayer2Object->freeze = 1;
	m_pPlayer2Object->freezeTimer = 1000000000;
	m_pPlayer2Object->freezeGap = g_cTimer.time()/* - m_pPlayerObject->m_nLastFrameTime*/;
}

void CObjectManager::unFreezeGame()
{
	PauseGame = 0;
	m_pPlayerObject->freeze = 0;
	m_pPlayer2Object->freeze = 0;
}

void CObjectManager::frameAdvance(int player)
{
	if(player == 1)
	{
		m_pPlayerObject->m_nCurrentFrame++;
		if(m_pPlayerObject->getCurrentAnimationImageNum() == -1)
		{
			m_pPlayerObject->m_nCurrentFrame--;
		}
		curFrameNum = m_pPlayerObject->getCurrentAnimationImageNum();
	}
}

void CObjectManager::frameReduce(int player)
{
	if(player == 1)
	{
		if(m_pPlayerObject->m_nCurrentFrame != 0)
		{
			m_pPlayerObject->m_nCurrentFrame--;
			curFrameNum = m_pPlayerObject->getCurrentAnimationImageNum();
		}
	}
}

int CObjectManager::getCurrentAnimationFrame(int player, float xOffsetChange, float yOffsetChange, float xSizeChange, float ySizeChange)
{
	if(player == 1)
	{
		int tempIndex = m_pPlayerObject->getCurrentAnimationImageNum();
		m_pPlayerObject->m_pHurtBoxes[tempIndex].xOffset+=xOffsetChange;
		m_pPlayerObject->m_pHurtBoxes[tempIndex].yOffset+=yOffsetChange;
		m_pPlayerObject->m_pHurtBoxes[tempIndex].xSize+=xSizeChange;
		m_pPlayerObject->m_pHurtBoxes[tempIndex].ySize+=ySizeChange;
		testBox = m_pPlayerObject->m_pHurtBoxes[tempIndex];
	}
	else
		return 0;
}