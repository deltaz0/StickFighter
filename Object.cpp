/// \file object.cpp
/// Code for the game object class CGameObject.
/// Copyright Ian Parberry, 2004.
/// Last updated September 24, 2010.

#include "object.h"

#include "input.h"
#include "defines.h" 
#include "timer.h" 
#include "spriteman.h" 
#include "sound.h"
#include "score.h" //score manager
#include "Objman.h" //added for use of GetXAvg()

#define ARRAY(X, Y) (m_pAnimation[(X) + ((Y) * (m_nAnimationCount))])

extern CObjectManager g_cObjectManager;
extern CInputManager* g_pInputManager;
extern CTimer g_cTimer;
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern CSpriteManager g_cSpriteManager;
extern TiXmlElement* g_xmlSettings;
extern CSoundManager* g_pSoundManager;
extern CScoreManager g_cScoreManager; //score manager



/// Initializes a game object. Gets object-dependent settings from g_xmlSettings
/// from the "object" tag that has the same "name" attribute as parameter name.
/// Assumes that the sprite manager has loaded the sprites already.
/// \param object object type
/// \param name object name in XML settings file object tag
/// \param location initial location of object in 3D space
/// \param xspeed speed along x axis
/// \param yspeed speed along y axis

CGameObject::CGameObject(ObjectType object, int playerNum, char* name, D3DXVECTOR3 location,
                         float xspeed, float yspeed){ //constructor
  
  //defaults
	aTimer = 0;
	bTimer = 0;
	cTimer = 0;
	m_fPushBackSpeed = 0;
	m_fCurrentAttackPostBounceStun = 0;
	m_fPostBounceStun = 0;
	m_nPushBackStartTime = 0;
	m_nPushBackDurationInitial = 0;
	m_nPushBackDurationCurrent = 0;
	m_nReboundCount = 0;
	m_fCurrentAttackBlockPushBackSpeed = 0;
	m_fCurrentAttackHitPushBackSpeed = 0;
	superMax = 100;
	superCurrent = 0;
  m_nCanJump = 1;
  m_nJumpSquat = 0;
	freezeBuffered = -1;
  freeze = 0;
  freezeTimer;
  freezeGap = 0;
  m_nCurrentHitLag = 0;
  m_nCurrentFrame = 0;
	m_nLongestAnimation = 1;
	m_nPlayerNumber = playerNum;
  m_nLastFrameTime = g_cTimer.time();
  m_nFrameInterval = 30; 
  m_nLifeTime = -1; //negative means immortal
  m_bVulnerable = FALSE; 
  m_bIntelligent = FALSE;
  m_nMinXSpeed = -40; m_nMaxXSpeed = 40;
  m_nMinYSpeed = -20; m_nMaxYSpeed = 20;
	m_bGroundBounce = 0;
	m_bCurrentAttackGroundBounce = 0;
	m_bWallBounce = 0;
	m_bCurrentAttackWallBounce = 0;
  m_bCanFly = FALSE;
  m_bCycleSprite = TRUE;
  m_pAnimation = NULL;
  m_pAnimationOffsets = NULL;
  m_nAnimationFrameCount = 0;
  m_nNumSecondaryHitBoxes = 0;
  m_nNumSecondaryHurtBoxes = 0;
  m_nAnimationCount = 1;
  m_nBounceCount = 0;
  m_nStunTimer = 0;
  m_bStatic = 0;
  m_fCurrentAttackHitStun = 0;
	m_fCurrentAttackBlockStun = 0;
  m_fCurrentAttackXKnockBack = 0;
  m_fCurrentAttackYKnockBack = 0;
	m_nCanCancel = 1;
	m_nCurrentAttackHeight = 1;
	m_nCollisionState = 1;
  m_nCurrentAttackDmg = 0;
  m_nMovementCheckFrame = -1;
	m_fFrameMovement = 0;

  airTime = 0;
  airDashesLeft = 1;
  airJumpsLeft = 1;
  airBorne = 0;
  redashLeft = 0;
  redashRight = 0;
  facing = 4;
  hitStunTime = 0;

  rightHeld = 0;
  leftHeld = 0;
  upHeld = 0;
  downHeld = 0;

  //common values
  m_nObjectType = object; //type of object
  m_nLastMoveTime = g_cTimer.time(); //time
  m_structLocation = location; //location
  m_fXspeed = xspeed; m_fYspeed = yspeed; //speed
  m_nFrameCount = g_cSpriteManager.FrameCount(m_nObjectType);
  m_nHeight = g_cSpriteManager.Height(m_nObjectType);
  m_nWidth = g_cSpriteManager.Width(m_nObjectType);
  m_nBirthTime = g_cTimer.time(); //time of creation
  currentAnimationState = 0;
  

  //sound played at creation of object
 /* switch(object){
    case PLAYER1_OBJECT:
      g_pSoundManager->play(WALKING_SOUND, TRUE);
      break;
  }*/
} 

CGameObject::CGameObject(ObjectType object, char* name, D3DXVECTOR3 location,
                         float xspeed, float yspeed){ //constructor
  
  //defaults
  m_nCurrentFrame = 0; 
  m_nLastFrameTime = g_cTimer.time();
  m_nFrameInterval = 30; 
  m_nLifeTime = -1; //negative means immortal
  m_bVulnerable = FALSE; 
  m_bIntelligent = FALSE;
  m_nMinXSpeed = -40; m_nMaxXSpeed = 40;
  m_nMinYSpeed = -20; m_nMaxYSpeed = 20;
  m_bCanFly = FALSE;
  m_bCycleSprite = TRUE;
  m_pAnimation = NULL;
  m_pAnimationOffsets = NULL;
  m_nAnimationFrameCount = 0;
  m_nNumSecondaryHitBoxes = 0;
  m_nNumSecondaryHurtBoxes = 0;
  m_nAnimationCount = 1;
  m_nBounceCount = 0;
  m_nStunTimer = 0;
  m_bStatic = 0;
  m_fCurrentAttackHitStun = 0;
	m_fCurrentAttackBlockStun = 0;
  m_fCurrentAttackXKnockBack = 0;
  m_fCurrentAttackYKnockBack = 0;
	m_nCanCancel = 1;
	m_nCurrentAttackHeight = 1;
    m_nCurrentAttackDmg = 0;

  airTime = 0;
  airDashesLeft = 1;
  airJumpsLeft = 1;
  airBorne = 0;
  redashLeft = 0;
  redashRight = 0;
  facing = 4;
  hitStunTime = 0;

  rightHeld = 0;
  leftHeld = 0;
  upHeld = 0;
  downHeld = 0;

  //common values
  m_nObjectType = object; //type of object
  m_nLastMoveTime = g_cTimer.time(); //time
  m_structLocation = location; //location
  m_fXspeed = xspeed; m_fYspeed = yspeed; //speed
  m_nFrameCount = g_cSpriteManager.FrameCount(m_nObjectType);
  m_nHeight = g_cSpriteManager.Height(m_nObjectType);
  m_nWidth = g_cSpriteManager.Width(m_nObjectType);
  m_nBirthTime = g_cTimer.time(); //time of creation
  currentAnimationState = 0;
  

  //sound played at creation of object
 /* switch(object){
    case PLAYER1_OBJECT:
      g_pSoundManager->play(WALKING_SOUND, TRUE);
      break;
  }*/
} 

CGameObject::~CGameObject(){
  delete []m_pAnimationOffsets;
  delete [] m_pAnimation;
}

/// Draw game object.
/// Draws the current sprite frame at the current position, then
/// computes which frame is to be drawn next time.

//IDEA FOR DRAWING ANIMATIONS:
//make this a virtual function and override it in the character class.  instead of drawing off the one
//base animation, we have each character have a separate array for each animation which we can simply load
//from the xml file (might need to override the load function as well).  Just use a switch statement
//to select the appropriate animation based off of some state flag, and voila.
void CGameObject::draw(){ //draw

  int t = m_nFrameInterval;

  float fOrientation = atan2f(m_fYspeed, 20.0f); //rotation around Z axis

  if(m_pAnimation != NULL){ //if there's an animation sequence
    //draw current frame
    g_cSpriteManager.Draw(m_nObjectType, m_structLocation, fOrientation, 1.0f, m_pAnimation[m_nCurrentFrame], facing);
    //advance to next frame
    if(g_cTimer.elapsed(m_nLastFrameTime, t)) //if enough time passed
      //increment and loop if necessary
      if(++m_nCurrentFrame >= m_nAnimationFrameCount && m_bCycleSprite)   //Alter m_nAnimationFramecount to be
        m_nCurrentFrame = 0;                                  //end offset for idle animation.
  }
  else 
    g_cSpriteManager.Draw(m_nObjectType, m_structLocation, fOrientation, 1.0f, 0);//assume only one frame
}

/// Load settings.
/// Loads settings for object from g_xmlSettings.
/// \param name name of object as found in name tag of XML settings file

void CGameObject::LoadSettings(char* name){

  if(g_xmlSettings){ //got "settings" tag

    //get "objects" tag
    TiXmlElement* objSettings = 
      g_xmlSettings->FirstChildElement("objects"); //objects tag

    if(objSettings){ //got "objects" tag

      //set obj to the first "object" tag with the correct name
      TiXmlElement* obj = objSettings->FirstChildElement("object");
      while(obj && strcmp(name, obj->Attribute("name"))){
        obj = obj->NextSiblingElement("object");
      }

      if(obj){ //got "object" tag with right name
        //get object information from tag
        obj->Attribute("minxspeed",&m_nMinXSpeed);
        obj->Attribute("maxxspeed",&m_nMaxXSpeed);
        obj->Attribute("minyspeed",&m_nMinYSpeed);
        obj->Attribute("maxyspeed",&m_nMaxYSpeed);
        obj->Attribute("frameinterval",&m_nFrameInterval);
        obj->Attribute("vulnerable",&m_bVulnerable);
        obj->Attribute("fly",&m_bCanFly);
        obj->Attribute("cycle",&m_bCycleSprite);
        obj->Attribute("lifetime",&m_nLifeTime);
        obj->Attribute("static",&m_bStatic);   

        //parse animation sequence
        if(obj->Attribute("animation")){ //sequence present

          //get sequence length
          int length=strlen(obj->Attribute("animation")); //length is length of values in "" including commas
          m_nAnimationFrameCount=1; //one more than number of commas
          for(int i=0; i<length; i++) //for each character
          {
            if(obj->Attribute("animation")[i] == ',')
              m_nAnimationFrameCount++; //count commas
          }

          m_pAnimation = new int[m_nAnimationFrameCount]; //memory for animation sequence //aside note why is this getting run with the earlier for loop
          //will need to make a new one of these for each animation.  also probably going to have to redo the while
          //loop below this once for every new animation, will need to rewrite it when we get more than 9 frames per
          //

          int i = 0; //character index
          int count = 0; //number of frame numbers input
          int num; //frame number
          char c = obj->Attribute("animation")[i]; //character in sequence string
          while(i<length){
            //get next frame number
            num = 0;
            while(i<length && c >= '0' && c <= '9'){
              num = num*10 + c - '0';
              c = obj->Attribute("animation")[++i];
            }
            //process frame number
            c = obj->Attribute("animation")[++i]; //skip over comma
            m_pAnimation[count++] = num; //record frame number
          }
        }
      }
    }
  }
}

/// Move object. 
/// The distance that an object moves depends on its speed, 
/// and the amount of time since it last moved.

void CGameObject::move(){ //move object
  
  if(m_bStatic)return; //static objects don't move
  
  const float XSCALE = 32.0f; //to scale back horizontal motion
  const float YSCALE = 32.0f; //to scale back vertical motion
  const float MARGIN = 50.0f; //margin on top of page

  float xdelta = 0.0f, ydelta = 0.0f; //change in position
  int time = g_cTimer.time(); //current time
  int tfactor = time-m_nLastMoveTime; //time since last move

  //compute xdelta and ydelta, horizontal and vertical distance
  xdelta = (m_fXspeed * (float)tfactor)/XSCALE; //x distance moved  

  /////----------------------------------------
  /*CHANGING YDELTA CALCULATIONS FROM ORIGINAL
  if(m_bCanFly)
    ydelta = (m_fYspeed * (float)tfactor)/YSCALE; //y distance moved
  else{
    float t = (float)(time-m_nBirthTime);
    ydelta = 3.0f + m_fYspeed * 2.0f + t*t/10000.0f; //y distance moved
  }
  //---------------------------------------------------
  */

  ydelta = (m_fYspeed * (float)tfactor)/YSCALE; //y distance moved
  




  //delta motion
  if(!(m_structLocation.x + xdelta >= 2*g_nScreenWidth) && !(m_structLocation.x  + xdelta <= 0))//(stage has a range of 0 to 2g_nScreenWidth)
  {
      if(!(abs(g_cObjectManager.GetXAvg() - (m_structLocation.x + xdelta)) >= g_nScreenWidth/2))//(players cannot be farther than g_nScreenWidth/2 apart)
      m_structLocation.x += xdelta; //x motion
  }
  m_structLocation.y -= ydelta; //y motion

  //limit top and bottom
  if(m_structLocation.y > (float)(g_nScreenHeight-MARGIN)){ //limit top
    m_structLocation.y = (float)(g_nScreenHeight-MARGIN);
    m_fYspeed = 0.0f; 
  }
  //collision code
  if(m_nPlayerNumber == 1)
  {
    if(airBorne == 1 || m_fXspeed == 0)
    {
      g_cObjectManager.detectCollisions(0,1,0);
    }
    else if(m_fXspeed > 0)
    {
      g_cObjectManager.detectCollisions(0,1,6);
    }
    else //speed is negative
    {
      g_cObjectManager.detectCollisions(0,1,4);
    }
  }
  else if(m_nPlayerNumber == 2)
  {
    if(airBorne == 1 || m_fXspeed == 0)
    {
      g_cObjectManager.detectCollisions(0,2,0);
    }
    else if(m_fXspeed > 0)
    {
      g_cObjectManager.detectCollisions(0,2,6);
    }
    else //speed is negative
    {
      g_cObjectManager.detectCollisions(0,2,4);
    }
  }
  //end collision code
  //three bounces and you're out
  if(m_nBounceCount>=3)m_nLifeTime = 1; //force cull by making lifetime tiny
 
  //hitting the ground
  if(m_structLocation.y <= 0.1f) //below bottom
    if(m_bCanFly){ //limit flying objects to bottom of screen
      m_structLocation.y = 0.0f; //fix location
      m_fYspeed = 0.0f; //this fixes orientation of object too
    }
    else{ //can't fly, meaning it is falling     
      m_structLocation.y = 1.2f; //put a little above ground
      m_fYspeed = -5.0f / (m_nBounceCount + 1); //fixed rebound speed
      m_fXspeed -= m_nBounceCount; //fake friction
      m_nBirthTime = time; //death time should be from when it hit the ground
      ++m_nBounceCount;
    }

    if(m_structLocation.y >= 1.2 )
    {
      if(airTime > 0)
      {
        if(m_fYspeed != 0)
          m_fYspeed = 0;
        else
          airTime-=((float)tfactor/16);
      }
      else
         m_fYspeed += ((float)tfactor/16);
    }
    else /*if(m_fXspeed != 0) *///if you aren't in the air (so you're grounded)
    {  
      if(m_fXspeed > 0 && rightHeld == 0) //if you aren't holding a direction on landing, stop movement.
      {
        m_fXspeed = 0;
      }
      else if(m_fXspeed < 0 && leftHeld == 0)
      {
        m_fXspeed = 0;
      }
      if(airBorne == 1) //Player is landing from being in the air
      {
        if(rightHeld == 1)
        {
          if(redashRight == 1)
          {
            //m_fXspeed = 40;
            dash(6);
            redashRight = 0;
          }
          else
            walk(6);
            //m_fXspeed = 10;
        }
        else if(leftHeld == 1)
        {
          if(redashLeft == 1)
          {
            dash(4);
            //m_fXspeed = -40;
            redashLeft = 0;
          }
          else
            walk(4);
            //m_fXspeed = -10;
        }
        airDashesLeft = 1;
        airJumpsLeft = 1;
        airBorne = 0;
				m_nCollisionState = 1;
				//setCurrentAnimation(0);	//This should be your standing neutral state.
      }
      if((m_nPlayerNumber == 1 || m_nPlayerNumber == 2) && upHeld/*(tempInt == 8 || tempInt == 7 ||tempInt == 9)*/)
      { 
        jump();
        /*if(facing == 6)
        {
          if(leftHeld)
          {
            walk(4);
            //m_fXspeed = -10;
          }
          else if(rightHeld)
          {
            walk(6);
            //m_fXspeed = 10;
          }*/
        //}
        /*else if(facing == 4)
        {
          if(tempInt == 9)
          {
            m_fXspeed = -10;
          }
          else if(tempInt == 7)
          {
            m_fXspeed = 10;
          }
        }*/
        //m_fYspeed = -20;  //future jump call
      }
    }
    /*else if(m_nObjectType == PLAYER1_OBJECT && (tempInt == 8 || tempInt == 7 ||tempInt == 9))
    {
      m_fYspeed = -20;
    }*/
  //record time of move
  m_nLastMoveTime = time;
}

/// Change speed.
/// Change the object's speed, and check against speed limits to make
/// sure that the object doesn't break them.
/// \param xdelta change in horizontal speed
/// \param ydelta change in vertical speed

void CGameObject::accelerate(int xdelta, int ydelta){ //change speed

  float old_xspeed=m_fXspeed; //old speed

  //horizontal
  m_fXspeed += xdelta;
  //check bounds
  //if(m_fXspeed < m_nMinXSpeed) m_fXspeed = (float)m_nMinXSpeed;
  //if(m_fXspeed > m_nMaxXSpeed) m_fXspeed = (float)m_nMaxXSpeed;

  //vertical
  m_fYspeed+=ydelta;
  //check bounds
  if(m_fYspeed < m_nMinYSpeed) m_fYspeed = (float)m_nMinYSpeed;
  if(m_fYspeed > m_nMaxYSpeed) m_fYspeed = (float)m_nMaxYSpeed;
}

/// Set speed.
/// Sets the objects's speed to a fixed speed, given by two parameters.
/// This function is needed for mouse and joystick control, for which speed
/// may jump by a large amount if the player moves the device jerkily.
/// \param xspeed new speed in the x direction
/// \param yspeed new speed in the y direction

void CGameObject::SetSpeed(int xspeed, int yspeed){ //set speed
  accelerate((int)(xspeed - m_fXspeed), (int)(yspeed - m_fYspeed));
}

void CGameObject::dash(int direction)
{
  
}

void CGameObject::walk(int direction)
{
  m_fYspeed = -60;
}

void CGameObject::crouch()
{

}

void CGameObject::stopCrouch()
{

}

void CGameObject::jump()
{
  m_fXspeed = 100;
}

void CGameObject::stop()
{

}

void CGameObject::expendHitBox()
{

}

void CGameObject::setCurrentAnimation(int newState)
{
  currentAnimationState = newState;
  m_nCurrentFrame = m_pAnimationOffsets[newState].begin;
  for(int a = m_pAnimationOffsets[newState].begin; a < m_pAnimationOffsets[newState].end;a++)
  {
    if(m_pHitBoxes[a].boxPresent == TRUE)
      m_pHitBoxes[a].active = TRUE;
  }
  m_nLastFrameTime = g_cTimer.time();
}

void CGameObject::getHit(float hitStunDuration, int hitLagDuration, float xKnockBack, float yKnockBack, float postBounceStun, BOOL groundBounce, BOOL wallBounce)
{

}

void CGameObject::block(float blockStunDuration, int hitLagDuration)
{

}

void CGameObject::interpretCommand(int directional, int attack)
{

}

int CGameObject::inStun()
{
	return FALSE;
}

int CGameObject::getCurrentAnimationImageNum()
{
	return m_pAnimation[currentAnimationState + (m_nCurrentFrame*m_nAnimationCount)];
}

void CGameObject::hitNotice()
{

}

void CGameObject::attackBlockedNotice()
{

}

void CGameObject::setPushBack(float speed)
{

}