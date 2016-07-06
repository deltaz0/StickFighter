#include "testChar.h"

#include "input.h"
#include "defines.h" 
#include "timer.h" 
#include "spriteman.h" 
#include "sound.h"
#include "score.h" //score manager
#include <math.h>
#include "Objman.h"

#define ARRAY(X, Y) (m_pAnimation[(X) + ((Y) * (m_nAnimationCount))])

extern CObjectManager g_cObjectManager;
extern int hitBoxDisplay;
extern CInputManager* g_pInputManager;
extern CTimer g_cTimer;
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern CSpriteManager g_cSpriteManager;
extern TiXmlElement* g_xmlSettings;
extern CSoundManager* g_pSoundManager;
extern CScoreManager g_cScoreManager; //score manager

CCharObject::CCharObject(ObjectType object, int playerNum, char* name, D3DXVECTOR3 location,
      float xspeed, float yspeed): CGameObject(object, playerNum, name, location, xspeed, yspeed){ ///< Constructor.
  //object-dependent settings loaded from XML
	m_nLongestAnimation = 1;
  LoadSettings(name);
	dashSpeed = 40;
  walkSpeed = 10;
  jumpSpeed = -26;
  doubleJumpSpeed = -26;
  //collisionBox.boxPresent = TRUE;
  //collisionBox.xOffset = 10.0f;
  //collisionBox.yOffset = 30.0f;
  //collisionBox.xSize = .45f;
  //collisionBox.ySize = 1.2f;

  /*hurtBox.boxPresent = TRUE;
  hurtBox.xOffset = 10.0f;
  hurtBox.yOffset = 40.0f;
  hurtBox.xSize = .7f;
  hurtBox.ySize = 1.3f;*/

  /*hitBox = new DETECTIONBOX[22];
  secondaryHitBoxes = new DETECTIONBOX[2];
  for(int a = 0; a < 22; a++)
  {
    hitBox[a].boxPresent = FALSE;
  }
  hitBox[7].boxPresent = TRUE;
  hitBox[7].active = TRUE;
  hitBox[7].xOffset = 50.0f;
  hitBox[7].yOffset = 90.0f;
  hitBox[7].xSize = .7f;
  hitBox[7].ySize = .2f;
  hitBox[7].secondaryBox = 0;
  hitBox[8].boxPresent = TRUE;
  hitBox[8].active = TRUE;
  hitBox[8].xOffset = 50.0f;
  hitBox[8].yOffset = 90.0f;
  hitBox[8].xSize = 0.7f;
  hitBox[8].ySize = .2f;
  hitBox[8].secondaryBox = 0;*/

  /*secondaryHitBoxes[0].boxPresent = TRUE;
  secondaryHitBoxes[0].active = TRUE;
  secondaryHitBoxes[0].xOffset = 100.0f;
  secondaryHitBoxes[0].yOffset = 90.0f;
  secondaryHitBoxes[0].xSize = .3f;
  secondaryHitBoxes[0].ySize = .3f;
  secondaryHitBoxes[0].secondaryBox = 1;

  secondaryHitBoxes[1].boxPresent = TRUE;
  secondaryHitBoxes[1].active = TRUE;
  secondaryHitBoxes[1].xOffset = 150.0f;
  secondaryHitBoxes[1].yOffset = 90.0f;
  secondaryHitBoxes[1].xSize = .5f;
  secondaryHitBoxes[1].ySize = .5f;
  secondaryHitBoxes[1].secondaryBox = -1;*/

  //hitBox[11].boxPresent = TRUE;
  //hitBox[11].active = TRUE;
  //hitBox[11].xOffset = 40.0f;
  //hitBox[11].yOffset = 40.0f;
  //hitBox[11].xSize = 0.5f;
  //hitBox[11].ySize = .2f;
  //hitBox[12].boxPresent = TRUE;
  //hitBox[12].active = TRUE;
  //hitBox[12].xOffset = 40.0f;
  //hitBox[12].yOffset = 40.0f;
  //hitBox[12].xSize = 0.5f;
  //hitBox[12].ySize = .2f;
  
}

CCharObject::~CCharObject(){
	delete [] m_pCollisionBoxes;
  delete [] m_pAnimationOffsets;
  delete [] m_pAnimation;
  delete [] m_pSecondaryHitBoxes;
  delete [] m_pHitBoxes;
  delete [] m_pSecondaryHurtBoxes;
  delete [] m_pHurtBoxes;
}

void CCharObject::expendHitBox()
{
	m_nCanCancel = 1;

	if(m_nPlayerNumber == 1)
		g_cObjectManager.hitChar(1, m_nCurrentAttackHeight, m_fCurrentAttackHitStun, m_fCurrentAttackBlockStun, m_fCurrentAttackXKnockBack, m_fCurrentAttackYKnockBack, m_nCurrentAttackDmg);
	else if(m_nPlayerNumber == 2)
		g_cObjectManager.hitChar(2, m_nCurrentAttackHeight, m_fCurrentAttackHitStun, m_fCurrentAttackBlockStun, m_fCurrentAttackXKnockBack, m_fCurrentAttackYKnockBack, m_nCurrentAttackDmg);
	m_pHitBoxes[ARRAY(currentAnimationState, m_nCurrentFrame)].active = FALSE;
  /*int i = m_nCurrentFrame;
  while(i < m_pAnimationOffsets[currentAnimationState].end && m_pHitBoxes[i].boxPresent == TRUE)
  {
    m_pHitBoxes[i].active = FALSE;
    int tempInt = m_pHitBoxes[i].secondaryBox;
    while(tempInt >= 0)
    {
      m_pSecondaryHitBoxes[tempInt].active = FALSE;
      tempInt = m_pSecondaryHitBoxes[tempInt].secondaryBox;
    }
    i++;
  }*/
}

void CCharObject::setCurrentAnimation(int newState)
{
  currentAnimationState = newState;
  m_nCurrentFrame = 0;//m_pAnimationOffsets[newState].begin;
	if(newState == 0 || newState == 1 || newState == 11 || newState == 14)	//list of all actionable states.  basically, if you want to be able to
	{																	//do an attack in this state, check for it here
		m_nCanCancel = 1;
		if(newState == 0)	//list of all states that you can walk in.  All this does is make you walk if you're
		{									//holding a direction as an animation ends.
			m_nCollisionState = 1;
			if(rightHeld == 1)
			{
				walk(6);
			}
			else if(leftHeld == 1)
			{
				walk(4);
			}
		}
		if(newState == 14)	//if crouching
		{
			m_nCollisionState = 0;	//set crouching collision
		}
	}
	else if(newState == 15)	//Wakeup collision
	{
		m_nCollisionState = 3;
	}
	else
	{
		m_nCanCancel = 0;
	}
	
	
	//int tempImageNum = ARRAY(currentAnimationState, m_nCurrentFrame);
	int temp = 0;
	//int tempImageNum = ARRAY(currentAnimationState, (m_nCurrentFrame+temp));
	int tempAnimNum = currentAnimationState + ((m_nCurrentFrame+temp)*m_nAnimationCount);
	int tempImageNum = m_pAnimation[currentAnimationState + (m_nCurrentFrame*m_nAnimationCount)];

	while(tempImageNum != -1)	//check every frame
	{																									//of the animation for hitboxes.
		m_pHitBoxes[tempImageNum].active = TRUE;
		temp++;
		tempImageNum = m_pAnimation[currentAnimationState+((m_nCurrentFrame+temp)*m_nAnimationCount)];
	}

  //if(newState != 0)	//can remove this if statement (NOT THE CODE IN IT) in template version.
  //{
  //  int i = m_pAnimationOffsets[newState].begin;
  //  while(i < m_pAnimationOffsets[newState].end)
  //  {
  //    if(m_pHitBoxes[i].boxPresent == TRUE)
  //    {
  //      m_pHitBoxes[i].active = TRUE;
  //      int tempInt = m_pHitBoxes[i].secondaryBox;
  //      while(tempInt >= 0)
  //      {
  //        m_pSecondaryHitBoxes[tempInt].active = TRUE;
  //        tempInt = m_pSecondaryHitBoxes[tempInt].secondaryBox;
  //      }
  //    }
  //    i++;
  //  }
  //}
  m_nLastFrameTime = g_cTimer.time();
}

void CCharObject::dash(int direction)
{
	if(currentAnimationState == 0 || currentAnimationState == 1 || currentAnimationState == 11 || currentAnimationState == 14)//set this to all animation states that a character
	{																								//can dash out of.  
		
		if(m_structLocation.y > 1.2)  //if we're airborn
		{
		//if you're very close to the ground, buffer a dash instead
			if(m_structLocation.y < 40)  
			{
				if(direction == 6)
				{
					redashLeft = 0;
					redashRight = 1;
				}
				else if(direction == 4)
				{
					redashRight = 0;
					redashLeft = 1;
				}
			}
			else if(airDashesLeft > 0)
			{
				airTime = 7;
				airDashesLeft--;
				if(direction == 6)//If airdashing right..
				{
					if(facing == 6)//and facing right...
					{
						m_fXspeed = 40;  //airdash forward code
					}
					else if(facing == 4)  //else if facing left...
					{
						m_fXspeed = 40;  // airdash backwards code
					}
				}
				else if(direction == 4)//If airdashing left...
				{
					if(facing == 4)//and facing left
					{
						m_fXspeed = -40;  //airdash forward code
					}
					else if(facing == 6)// else if facing right...
					{
						m_fXspeed = -40;  //airdash backwards code
					}
				}
			}
		}
		else //else we're grounded
		{
			setCurrentAnimation(1);
			if(direction == 6) //if dashing right...
			{
				if(facing == 6) //and facing right...
				{
					m_fXspeed = 40; //dash forward code.
				}
				else if(facing == 4)  //else facing left...
				{
					m_fXspeed = 40;  //dash backwards code.
				}
			}
			else if(direction  == 4)  //if dashing left...
			{
				if(facing == 4) //and facing left...
				{
					m_fXspeed = -40;  //dash forward code.
				}
				else if(facing == 6)  //else facing right...
				{
					m_fXspeed = -40; //dash backwards code.
				}
			}
		}
	}
}

void CCharObject::walk(int direction)
{
	if(currentAnimationState == 0 || currentAnimationState == 1 || (currentAnimationState == 14 && downHeld == 0))//set this to all animation states that a character
	{																								//can walk out of.  The downHeld == 0 is there just so you can
		if(m_structLocation.y > 1.2)  //if airborn		//walk out of crouch without having to transition back to neutral
		{																							//first.  This way, you won't move while crouching if you push
			m_nCollisionState = 2;											//down and sideways.
		}
		else  //if grounded
		{
			setCurrentAnimation(1);
			m_nCollisionState = 1;
			if(direction == 6)  //if moving right...
			{
				if(facing == 6) //and if facing right...
				{
					m_fXspeed = 10; //walk forward code.
				}
				else if(facing == 4)  //else if facing left...
				{
					m_fXspeed = 10;  //walk backward code.
				}
			}
			else if(direction == 4) //if moving left...
			{
				if(facing == 4) //and facing left...
				{
					m_fXspeed = -10;  //walk forward code.
				}
				else if(facing == 6)  //else facing right...
				{
					m_fXspeed = -10; //walk backward code.
				}
			}
		}
	}
}

void CCharObject::crouch()
{
	if(airBorne == 0)
	{
		setCurrentAnimation(14);
		m_fXspeed = 0;
	}
}

void CCharObject::stopCrouch()
{
	if(currentAnimationState == 14)	//Add an or check for every animation state involving crouching
	{
		setCurrentAnimation(0);	//neutral
	}
}

void CCharObject::jump()
{
	if(currentAnimationState == 0 || currentAnimationState == 1 || currentAnimationState == 11 || currentAnimationState == 14)//set this to all animation states that a character
	{																								//can jump out of.
		setCurrentAnimation(11);
		if(m_structLocation.y > 1.2)  //if you're in the air...
		{
			if(airJumpsLeft > 0)
			{
				airJumpsLeft--;
				m_fYspeed = doubleJumpSpeed;
				if(leftHeld == 1)
				{
					if(m_fXspeed >= 0)  //if not moving left...
					{
						m_fXspeed = -walkSpeed; //move left
					}
				}
				else if(rightHeld == 1)
				{
					if(m_fXspeed <= 0)  //if not moving right...
					{
						m_fXspeed = walkSpeed;  //move right
					}
				}
				else  //no direction.
				{
					m_fXspeed = 0;
				}
			}
		}
		else  //if grounded...
		{
			airBorne = 1;
			m_nCollisionState = 2;
			m_fYspeed = jumpSpeed;
			if(rightHeld == 1)
			{
				if(m_fXspeed < walkSpeed) //is moving left or still
				{
					m_fXspeed = walkSpeed;
				}
			}
			else if(leftHeld == 1)
			{
				if(m_fXspeed > -walkSpeed)  //is moving right or still
				{
					m_fXspeed = -walkSpeed;
				}
			}
		}
	}
}

void CCharObject::stop()
{
	//if(currentAnimationState != 5 && currentAnimationState != 6 && currentAnimationState != 7)
	if(currentAnimationState == 1)	//list any animation states that involve dashing (grounded) or walking.
	{
		if(m_structLocation.y <= 1.2) //if grounded
		{
			m_nCollisionState = 1;
			m_fXspeed = 0;  //stop movement.
		}
	  setCurrentAnimation(0);
	}
}

/// Move object. 
/// The distance that an object moves depends on its speed, 
/// and the amount of time since it last moved.

void CCharObject::move(){ //move object
  
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
	{
    m_structLocation.y = 0.0f; //fix location
    m_fYspeed = 0.0f; //this fixes orientation of object too
  }

    if(m_structLocation.y > 0 )
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
            dash(6);
            redashRight = 0;
          }
          else
            walk(6);
        }
        else if(leftHeld == 1)
        {
          if(redashLeft == 1)
          {
            dash(4);
            redashLeft = 0;
          }
          else
            walk(4);
        }
        airDashesLeft = 1;
        airJumpsLeft = 1;
        airBorne = 0;
				m_nCollisionState = 1;
				if(currentAnimationState != 12)	//if you are not in aerial hitstun
				{
					if(downHeld == 1)
						setCurrentAnimation(14);	//If holding down, crouch
					else
						setCurrentAnimation(0);	//This should be your standing neutral state.
				}
				else	//if you land in aerial hitstun
				{
					setCurrentAnimation(15);	//knockdown/wakeup animation
					m_nCollisionState = 3;
				}
      }
      if(upHeld/*(tempInt == 8 || tempInt == 7 ||tempInt == 9)*/)
      { 
        jump();
      }
    }
  //record time of move
  m_nLastMoveTime = time;
}

/// Load settings.
/// Loads settings for object from g_xmlSettings.
/// \param name name of object as found in name tag of XML settings file

void CCharObject::LoadSettings(char* name)
{

	//----------------
			//		m_nFrameCount =25;
	//---------------------
  if(g_xmlSettings)
	{ //got "settings" tag

    //get "objects" tag
    TiXmlElement* objSettings = 
      g_xmlSettings->FirstChildElement("objects"); //objects tag

    if(objSettings)
		{ //got "objects" tag

      //set obj to the first "object" tag with the correct name
      TiXmlElement* obj = objSettings->FirstChildElement("object");
      while(obj && strcmp(name, obj->Attribute("name")))
			{
        obj = obj->NextSiblingElement("object");
      }

      if(obj)
			{ //got "object" tag with right name
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
        obj->Attribute("animationcount",&m_nAnimationCount);
				obj->Attribute("longestanimation",&m_nLongestAnimation);
        obj->Attribute("secondaryhitboxcount", &m_nNumSecondaryHitBoxes);
        obj->Attribute("secondaryhurtboxcount", &m_nNumSecondaryHurtBoxes);
        //parse animation sequence
        if(obj->Attribute("animation0"))
				{ //sequence present

          //get sequence length
          int length=strlen(obj->Attribute("animation")); //length is length of values in "" including commas
          m_nAnimationFrameCount=1; //one more than number of commas
          for(int i=0; i<length; i++) //for each character
          {
            if(obj->Attribute("animation")[i] == ',')
              m_nAnimationFrameCount++; //count commas
          }
          m_pAnimationOffsets = new ANIMATIONOFFSET[m_nAnimationCount];
          m_pAnimation = new int[m_nAnimationCount*(m_nLongestAnimation+1)]; //memory for animation sequence //aside note why is this getting run with the earlier for loop
          m_pHitBoxes = new DETECTIONBOX[m_nFrameCount];
          m_pHurtBoxes = new DETECTIONBOX[m_nFrameCount];
          m_pSecondaryHitBoxes = new DETECTIONBOX[m_nNumSecondaryHitBoxes];
          m_pSecondaryHurtBoxes = new DETECTIONBOX[m_nNumSecondaryHurtBoxes];
					m_pCollisionBoxes = new DETECTIONBOX[4];
					
          //while loop to set all box.present values in hitboxes to false.
          //uncomment these later

          

					//========================
					//Load Animation Indices =
					//========================
          ANIMATIONOFFSET offset;
          int offsetInt = 0;
          for(int a = 0; a < m_nAnimationCount; a++)
          {
						
            int tracker = 0;
            int tempArray[4] = {0,0,0,0};
            int arrayTracker = 1;
            char tempString[21];
            char indexName[37] = "animationindices";
            sprintf(tempString, "%d", a);
            strcat(indexName,tempString);

			      int offsetLength = strlen(obj->Attribute(indexName));
			      int i = 0;
			      int count = 0;
			      int num = 0;
			      char c = obj->Attribute(indexName)[i];
			      while(obj->Attribute(indexName)[i] != ',' && c >= '0' && c <= '9')
			      {
				      num = num*10 + c - '0';
				      c = obj->Attribute(indexName)[++i];
			      }
			      offset.begin = num;
			      c = obj->Attribute(indexName)[++i];
			      num = 0;
			      while(i<offsetLength && c >= '0' && c <= '9')
			      {
				      num = num*10 + c - '0';
				      c = obj->Attribute(indexName)[++i];
			      }
			      offset.end = num;
			      m_pAnimationOffsets[offsetInt++] = offset;
            
          }
					//==========================
					//Finished Loading Indices =
					//==========================

					//=================
					//Load Animations =
					//=================
          int i = 0; //character index
          int count = 0; //number of frame numbers input
          int num; //frame number
					/*
					char tempString[21];
            char indexName[37] = "animationindices";
            sprintf(tempString, "%d", a);
            strcat(indexName,tempString);
					*/
					
					char c = obj->Attribute("animation0")[i]; //character in sequence string
					for(int a = 0; a < m_nAnimationCount; a++)
					{
						count = 0;
						i = 0;
						char tempString[21];
						char indexName[37] = "animation";
						sprintf(tempString, "%d", a);
            strcat(indexName,tempString);
						c = obj->Attribute(indexName)[0];
						while(c!='-')
						{
							//get next frame number
							num = 0;
							while(c >= '0' && c <= '9')
							{
								num = num*10 + c - '0';
								c = obj->Attribute(indexName)[++i];
							}
							//process frame number
							while(obj->Attribute(indexName)[i] == ',')
							{
								i++;
							}
							ARRAY(a,count) = num;
							m_pAnimation[a+(count*m_nAnimationCount)] = num;
							count++;
							c = obj->Attribute(indexName)[i]; //skip over comma

						}
						ARRAY(a,count) = -1;
						m_pAnimation[a+(count*m_nAnimationCount)] = -1;
					}
					//============================
					//New Load Hitboxes Function =
					//============================
					for(int a = 0; a < m_nFrameCount; a++)
					{
						char hitBoxName[37] = "hitbox";
            char tempString[21];
            sprintf(tempString, "%d", a);
            strcat(hitBoxName, tempString);
						if(obj->Attribute(hitBoxName))  //if there is a hitbox for this frame...
            {
              DETECTIONBOX tempBox;
              int j = 0;
              int num2 = 0;
              int modifier = 1;
              char ch = obj->Attribute(hitBoxName)[j];
              if(ch == '-')
              {
                modifier = -1;
                ch = obj->Attribute(hitBoxName)[++j];
              }
              //find xOffset...
              while(ch != ',' && ch >= '0' && ch <= '9')
			        {
				        num2 = num2*10 + ch - '0';
				        ch = obj->Attribute(hitBoxName)[++j];
			        }
              num2*=modifier;
              tempBox.xOffset = (float)num2;
              //find yOffset...
              ch = obj->Attribute(hitBoxName)[++j];
              num2 = 0;
              if(ch == '-')
              {
                modifier = -1;
                ch = obj->Attribute(hitBoxName)[++j];
              }
              else
                modifier = 1;
              while(ch != ',' && ch >= '0' && ch <= '9')
			        {
				        num2 = num2*10 + ch - '0';
				        ch = obj->Attribute(hitBoxName)[++j];
			        }
              num2*=modifier;
              tempBox.yOffset = (float)num2;
              //find xSize...
              ch = obj->Attribute(hitBoxName)[++j];
              num2 = 0;
              int counter = 0;
              float deciNum = -1;
              while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
			        {
                if(ch == '.')
                {
                  deciNum = 0;
                  ch = obj->Attribute(hitBoxName)[++j];
                }
                else if(deciNum < 0)
                {
				          num2 = num2*10 + ch - '0';
				          ch = obj->Attribute(hitBoxName)[++j];
                }
                else
                {
                  counter++;
                  deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
                  ch = obj->Attribute(hitBoxName)[++j];
                }
			        }
              tempBox.xSize = (float)(num2+deciNum);
              //find ySize..
              ch = obj->Attribute(hitBoxName)[++j];
              num2 = 0;
              counter = 0;
              deciNum = -1;
              while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
			        {
                if(ch == '.')
                {
                  deciNum = 0;
                  ch = obj->Attribute(hitBoxName)[++j];
                }
                else if(deciNum < 0)
                {
				          num2 = num2*10 + ch - '0';
				          ch = obj->Attribute(hitBoxName)[++j];
                }
                else
                {
                  counter++;
                  deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
                  ch = obj->Attribute(hitBoxName)[++j];
                }
			        }
              tempBox.ySize = (float)(num2+deciNum);
              //find secondBox index...
              ch = obj->Attribute(hitBoxName)[++j];
              num2 = 0;
              if(ch == '-')
              {
                modifier = -1;
                ch = obj->Attribute(hitBoxName)[++j];
              }
              else
                modifier = 1;
              while(ch != ',' && ch >= '0' && ch <= '9')
			        {
				        num2 = num2*10 + ch - '0';
				        ch = obj->Attribute(hitBoxName)[++j];
			        }
              num2*=modifier;
              tempBox.secondaryBox = num2;
              tempBox.boxPresent = TRUE;
              tempBox.active = TRUE;
              m_pHitBoxes[a] = tempBox;
            }
						else
						{
							DETECTIONBOX tempBox;
							tempBox.boxPresent = 0;
							m_pHitBoxes[a] = tempBox;
						}
					
					//================================
					//End New Load Hitboxes Function =
					//================================
            //============================
            //Add hitbox for this frame. =
            //============================
      //      char hitBoxName[37] = "hitbox";
      //      char tempString[21];
      //      sprintf(tempString, "%d", count);
      //      strcat(hitBoxName, tempString);
      //      if(obj->Attribute(hitBoxName))  //if there is a hitbox for this frame...
      //      {
      //        DETECTIONBOX tempBox;
      //        int hitBoxLength = strlen(obj->Attribute(hitBoxName));
      //        int j = 0;
      //        int num2 = 0;
      //        int modifier = 1;
      //        char ch = obj->Attribute(hitBoxName)[j];
      //        if(ch == '-')
      //        {
      //          modifier = -1;
      //          ch = obj->Attribute(hitBoxName)[++j];
      //        }
      //        //find xOffset...
      //        while(ch != ',' && ch >= '0' && ch <= '9')
			   //     {
				  //      num2 = num2*10 + ch - '0';
				  //      ch = obj->Attribute(hitBoxName)[++j];
			   //     }
      //        num2*=modifier;
      //        tempBox.xOffset = (float)num2;
      //        //find yOffset...
      //        ch = obj->Attribute(hitBoxName)[++j];
      //        num2 = 0;
      //        if(ch == '-')
      //        {
      //          modifier = -1;
      //          ch = obj->Attribute(hitBoxName)[++j];
      //        }
      //        else
      //          modifier = 1;
      //        while(ch != ',' && ch >= '0' && ch <= '9')
			   //     {
				  //      num2 = num2*10 + ch - '0';
				  //      ch = obj->Attribute(hitBoxName)[++j];
			   //     }
      //        num2*=modifier;
      //        tempBox.yOffset = (float)num2;
      //        //find xSize...
      //        ch = obj->Attribute(hitBoxName)[++j];
      //        num2 = 0;
      //        int counter = 0;
      //        float deciNum = -1;
      //        while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
			   //     {
      //          if(ch == '.')
      //          {
      //            deciNum = 0;
      //            ch = obj->Attribute(hitBoxName)[++j];
      //          }
      //          else if(deciNum < 0)
      //          {
				  //        num2 = num2*10 + ch - '0';
				  //        ch = obj->Attribute(hitBoxName)[++j];
      //          }
      //          else
      //          {
      //            counter++;
      //            deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
      //            ch = obj->Attribute(hitBoxName)[++j];
      //          }
			   //     }
      //        tempBox.xSize = (float)(num2+deciNum);
      //        //find ySize..
      //        ch = obj->Attribute(hitBoxName)[++j];
      //        num2 = 0;
      //        counter = 0;
      //        deciNum = -1;
      //        while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
			   //     {
      //          if(ch == '.')
      //          {
      //            deciNum = 0;
      //            ch = obj->Attribute(hitBoxName)[++j];
      //          }
      //          else if(deciNum < 0)
      //          {
				  //        num2 = num2*10 + ch - '0';
				  //        ch = obj->Attribute(hitBoxName)[++j];
      //          }
      //          else
      //          {
      //            counter++;
      //            deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
      //            ch = obj->Attribute(hitBoxName)[++j];
      //          }
			   //     }
      //        tempBox.ySize = (float)(num2+deciNum);
      //        //find secondBox index...
      //        ch = obj->Attribute(hitBoxName)[++j];
      //        num2 = 0;
      //        if(ch == '-')
      //        {
      //          modifier = -1;
      //          ch = obj->Attribute(hitBoxName)[++j];
      //        }
      //        else
      //          modifier = 1;
      //        while(ch != ',' && ch >= '0' && ch <= '9')
			   //     {
				  //      num2 = num2*10 + ch - '0';
				  //      ch = obj->Attribute(hitBoxName)[++j];
			   //     }
      //        num2*=modifier;
      //        tempBox.secondaryBox = num2;
      //        tempBox.boxPresent = TRUE;
      //        tempBox.active = TRUE;
      //        m_pHitBoxes[count] = tempBox;
      //        int dsklfjhds = 0;
      //      }
						//else
						//{
						//	DETECTIONBOX tempBox;
						//	tempBox.boxPresent = 0;
						//	m_pHitBoxes[count] = tempBox;
						//}
            //===============================
            //Finished reading hitbox data. =
            //===============================
						
						//===========================
						//New Load Hurtbox Function =
						//===========================
						char tempString2[21];
						char hurtBoxName[37] = "hurtbox";
						sprintf(tempString2, "%d", a);
            strcat(hurtBoxName, tempString2);	//find here
            if(obj->Attribute(hurtBoxName))  //if there is a hurtbox for this frame...
            {
              DETECTIONBOX tempBox;
              int j = 0;
              int num2 = 0;
              int modifier = 1;
              char ch = obj->Attribute(hurtBoxName)[j];
              if(ch == '-')
              {
                modifier = -1;
                ch = obj->Attribute(hurtBoxName)[++j];
              }
              //find xOffset...
              while(ch != ',' && ch >= '0' && ch <= '9')
			        {
				        num2 = num2*10 + ch - '0';
				        ch = obj->Attribute(hurtBoxName)[++j];
			        }
              num2*=modifier;
              tempBox.xOffset = (float)num2;
              //find yOffset...
              ch = obj->Attribute(hurtBoxName)[++j];
              num2 = 0;
              if(ch == '-')
              {
                modifier = -1;
                ch = obj->Attribute(hurtBoxName)[++j];
              }
              else
                modifier = 1;
              while(ch != ',' && ch >= '0' && ch <= '9')
			        {
				        num2 = num2*10 + ch - '0';
				        ch = obj->Attribute(hurtBoxName)[++j];
			        }
              num2*=modifier;
              tempBox.yOffset = (float)num2;
              //find xSize...
              ch = obj->Attribute(hurtBoxName)[++j];
              num2 = 0;
              int counter = 0;
              float deciNum = -1;
              while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
			        {
                if(ch == '.')
                {
                  deciNum = 0;
                  ch = obj->Attribute(hurtBoxName)[++j];
                }
                else if(deciNum < 0)
                {
				          num2 = num2*10 + ch - '0';
				          ch = obj->Attribute(hurtBoxName)[++j];
                }
                else
                {
                  counter++;
                  deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
                  ch = obj->Attribute(hurtBoxName)[++j];
                }
			        }
              tempBox.xSize = (float)(num2+deciNum);
              //find ySize..
              ch = obj->Attribute(hurtBoxName)[++j];
              num2 = 0;
              counter = 0;
              deciNum = -1;
              while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
			        {
                if(ch == '.')
                {
                  deciNum = 0;
                  ch = obj->Attribute(hurtBoxName)[++j];
                }
                else if(deciNum < 0)
                {
				          num2 = num2*10 + ch - '0';
				          ch = obj->Attribute(hurtBoxName)[++j];
                }
                else
                {
                  counter++;
                  deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
                  ch = obj->Attribute(hurtBoxName)[++j];
                }
			        }
              tempBox.ySize = (float)(num2+deciNum);
              //find secondBox index...
              ch = obj->Attribute(hurtBoxName)[++j];
              num2 = 0;
              if(ch == '-')
              {
                modifier = -1;
                ch = obj->Attribute(hurtBoxName)[++j];
              }
              else
                modifier = 1;
              while(ch != ',' && ch >= '0' && ch <= '9')
			        {
				        num2 = num2*10 + ch - '0';
				        ch = obj->Attribute(hurtBoxName)[++j];
			        }
              num2*=modifier;
              tempBox.secondaryBox = num2;
              tempBox.boxPresent = TRUE;
              tempBox.active = TRUE;
              m_pHurtBoxes[a] = tempBox;
            }
						else
						{
							DETECTIONBOX tempBox;
							tempBox.boxPresent = 0;
							m_pHurtBoxes[a] = tempBox;
						}
					}
						//====================================
						//Finished New Load Hurtbox Function =
						//====================================
						

            //====================
            //Read hurtbox data. =
            //====================
      //      char hurtBoxName[37] = "hurtbox";
      //      strcat(hurtBoxName, tempString);
      //      if(obj->Attribute(hurtBoxName))  //if there is a hurtbox for this frame...
      //      {
      //        DETECTIONBOX tempBox;
      //        int hurtBoxLength = strlen(obj->Attribute(hurtBoxName));
      //        int j = 0;
      //        int num2 = 0;
      //        int modifier = 1;
      //        char ch = obj->Attribute(hurtBoxName)[j];
      //        if(ch == '-')
      //        {
      //          modifier = -1;
      //          ch = obj->Attribute(hurtBoxName)[++j];
      //        }
      //        //find xOffset...
      //        while(ch != ',' && ch >= '0' && ch <= '9')
			   //     {
				  //      num2 = num2*10 + ch - '0';
				  //      ch = obj->Attribute(hurtBoxName)[++j];
			   //     }
      //        num2*=modifier;
      //        tempBox.xOffset = (float)num2;
      //        //find yOffset...
      //        ch = obj->Attribute(hurtBoxName)[++j];
      //        num2 = 0;
      //        if(ch == '-')
      //        {
      //          modifier = -1;
      //          ch = obj->Attribute(hurtBoxName)[++j];
      //        }
      //        else
      //          modifier = 1;
      //        while(ch != ',' && ch >= '0' && ch <= '9')
			   //     {
				  //      num2 = num2*10 + ch - '0';
				  //      ch = obj->Attribute(hurtBoxName)[++j];
			   //     }
      //        num2*=modifier;
      //        tempBox.yOffset = (float)num2;
      //        //find xSize...
      //        ch = obj->Attribute(hurtBoxName)[++j];
      //        num2 = 0;
      //        int counter = 0;
      //        float deciNum = -1;
      //        while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
			   //     {
      //          if(ch == '.')
      //          {
      //            deciNum = 0;
      //            ch = obj->Attribute(hurtBoxName)[++j];
      //          }
      //          else if(deciNum < 0)
      //          {
				  //        num2 = num2*10 + ch - '0';
				  //        ch = obj->Attribute(hurtBoxName)[++j];
      //          }
      //          else
      //          {
      //            counter++;
      //            deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
      //            ch = obj->Attribute(hurtBoxName)[++j];
      //          }
			   //     }
      //        tempBox.xSize = (float)(num2+deciNum);
      //        //find ySize..
      //        ch = obj->Attribute(hurtBoxName)[++j];
      //        num2 = 0;
      //        counter = 0;
      //        deciNum = -1;
      //        while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
			   //     {
      //          if(ch == '.')
      //          {
      //            deciNum = 0;
      //            ch = obj->Attribute(hurtBoxName)[++j];
      //          }
      //          else if(deciNum < 0)
      //          {
				  //        num2 = num2*10 + ch - '0';
				  //        ch = obj->Attribute(hurtBoxName)[++j];
      //          }
      //          else
      //          {
      //            counter++;
      //            deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
      //            ch = obj->Attribute(hurtBoxName)[++j];
      //          }
			   //     }
      //        tempBox.ySize = (float)(num2+deciNum);
      //        //find secondBox index...
      //        ch = obj->Attribute(hurtBoxName)[++j];
      //        num2 = 0;
      //        if(ch == '-')
      //        {
      //          modifier = -1;
      //          ch = obj->Attribute(hurtBoxName)[++j];
      //        }
      //        else
      //          modifier = 1;
      //        while(ch != ',' && ch >= '0' && ch <= '9')
			   //     {
				  //      num2 = num2*10 + ch - '0';
				  //      ch = obj->Attribute(hurtBoxName)[++j];
			   //     }
      //        num2*=modifier;
      //        tempBox.secondaryBox = num2;
      //        tempBox.boxPresent = TRUE;
      //        tempBox.active = TRUE;
      //        m_pHurtBoxes[count] = tempBox;
      //      }
						//else
						//{
						//	DETECTIONBOX tempBox;
						//	tempBox.boxPresent = 0;
						//	m_pHurtBoxes[count] = tempBox;
						//}
            //================================
            //Finished reading hurtbox data. =
            //================================
//            m_pAnimation[count++] = num; //record frame number
        }
        //read secondary hitbox data.
        for(int a = 0; a < m_nNumSecondaryHitBoxes; a++)
        {
          DETECTIONBOX tempBox;
          char hitBoxName[37] = "secondaryhitbox";
          char tempString[21];
          sprintf(tempString, "%d", a);
          strcat(hitBoxName, tempString);
          int j = 0;
          int num2 = 0;
          int modifier = 1;
          char ch = obj->Attribute(hitBoxName)[j];
          if(ch == '-')
          {
              modifier = -1;
              ch = obj->Attribute(hitBoxName)[++j];
          }
          //find xOffset...
          while(ch != ',' && ch >= '0' && ch <= '9')
			    {
				    num2 = num2*10 + ch - '0';
				    ch = obj->Attribute(hitBoxName)[++j];
			    }
          num2*=modifier;
          tempBox.xOffset = (float)num2;
          //find yOffset...
          ch = obj->Attribute(hitBoxName)[++j];
          num2 = 0;
          if(ch == '-')
          {
            modifier = -1;
            ch = obj->Attribute(hitBoxName)[++j];
          }
          else
            modifier = 1;
          while(ch != ',' && ch >= '0' && ch <= '9')
			    {
				    num2 = num2*10 + ch - '0';
				    ch = obj->Attribute(hitBoxName)[++j];
			    }
          num2*=modifier;
          tempBox.yOffset = (float)num2;
          //find xSize...
          ch = obj->Attribute(hitBoxName)[++j];
          num2 = 0;
          int counter = 0;
          float deciNum = -1;
          while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
			    {
            if(ch == '.')
            {
              deciNum = 0;
              ch = obj->Attribute(hitBoxName)[++j];
            }
            else if(deciNum < 0)
            {
				      num2 = num2*10 + ch - '0';
				      ch = obj->Attribute(hitBoxName)[++j];
            }
            else
            {
              counter++;
              deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
              ch = obj->Attribute(hitBoxName)[++j];
            }
			    }
          tempBox.xSize = (float)(num2+deciNum);
          //find ySize..
          ch = obj->Attribute(hitBoxName)[++j];
          num2 = 0;
          counter = 0;
          deciNum = -1;
          while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
			    {
            if(ch == '.')
            {
              deciNum = 0;
              ch = obj->Attribute(hitBoxName)[++j];
            }
            else if(deciNum < 0)
            {
				      num2 = num2*10 + ch - '0';
				      ch = obj->Attribute(hitBoxName)[++j];
            }
            else
            {
              counter++;
              deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
              ch = obj->Attribute(hitBoxName)[++j];
            }
			    }
            tempBox.ySize = (float)(num2+deciNum);
            //find secondBox index...
            ch = obj->Attribute(hitBoxName)[++j];
            num2 = 0;
            if(ch == '-')
            {
                modifier = -1;
                ch = obj->Attribute(hitBoxName)[++j];
            }
              else
                modifier = 1;
            while(ch != ',' && ch >= '0' && ch <= '9')
			      {
				      num2 = num2*10 + ch - '0';
				      ch = obj->Attribute(hitBoxName)[++j];
			      }
            num2*=modifier;
            tempBox.secondaryBox = num2;
            tempBox.boxPresent = TRUE;
            tempBox.active = TRUE;
            m_pSecondaryHitBoxes[a] = tempBox;
            int dsklfjhds = 0;
        }
        //==============================
        //Read secondary hurtbox data. =
        //==============================
        for(int a = 0; a < m_nNumSecondaryHurtBoxes; a++)
        {
          char hurtBoxName[37] = "secondaryhurtbox";
          char tempString[21];
          sprintf(tempString, "%d", a);
          strcat(hurtBoxName, tempString);
          if(obj->Attribute(hurtBoxName))  //if there is a hurtbox for this frame...
          {
            DETECTIONBOX tempBox;
            int hurtBoxLength = strlen(obj->Attribute(hurtBoxName));
            int j = 0;
            int num2 = 0;
            int modifier = 1;
            char ch = obj->Attribute(hurtBoxName)[j];
            if(ch == '-')
            {
              modifier = -1;
              ch = obj->Attribute(hurtBoxName)[++j];
            }
            //find xOffset...
            while(ch != ',' && ch >= '0' && ch <= '9')
			      {
				      num2 = num2*10 + ch - '0';
				      ch = obj->Attribute(hurtBoxName)[++j];
			      }
            num2*=modifier;
            tempBox.xOffset = (float)num2;
            //find yOffset...
            ch = obj->Attribute(hurtBoxName)[++j];
            num2 = 0;
            if(ch == '-')
            {
              modifier = -1;
              ch = obj->Attribute(hurtBoxName)[++j];
            }
            else
              modifier = 1;
            while(ch != ',' && ch >= '0' && ch <= '9')
			      {
				      num2 = num2*10 + ch - '0';
				      ch = obj->Attribute(hurtBoxName)[++j];
			      }
            num2*=modifier;
            tempBox.yOffset = (float)num2;
            //find xSize...
            ch = obj->Attribute(hurtBoxName)[++j];
            num2 = 0;
            int counter = 0;
            float deciNum = -1;
            while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
			      {
              if(ch == '.')
              {
                deciNum = 0;
                ch = obj->Attribute(hurtBoxName)[++j];
              }
              else if(deciNum < 0)
              {
				        num2 = num2*10 + ch - '0';
				        ch = obj->Attribute(hurtBoxName)[++j];
              }
              else
              {
                counter++;
                deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
                ch = obj->Attribute(hurtBoxName)[++j];
              }
			      }
            tempBox.xSize = (float)(num2+deciNum);
            //find ySize..
            ch = obj->Attribute(hurtBoxName)[++j];
            num2 = 0;
            counter = 0;
            deciNum = -1;
            while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
			      {
              if(ch == '.')
              {
                deciNum = 0;
                ch = obj->Attribute(hurtBoxName)[++j];
              }
              else if(deciNum < 0)
              {
				        num2 = num2*10 + ch - '0';
				        ch = obj->Attribute(hurtBoxName)[++j];
              }
              else
              {
                counter++;
                deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
                ch = obj->Attribute(hurtBoxName)[++j];
              }
			      }
            tempBox.ySize = (float)(num2+deciNum);
            //find secondBox index...
            ch = obj->Attribute(hurtBoxName)[++j];
            num2 = 0;
            if(ch == '-')
            {
              modifier = -1;
              ch = obj->Attribute(hurtBoxName)[++j];
            }
            else
              modifier = 1;
            while(ch != ',' && ch >= '0' && ch <= '9')
			      {
				      num2 = num2*10 + ch - '0';
				      ch = obj->Attribute(hurtBoxName)[++j];
			      }
            num2*=modifier;
            tempBox.secondaryBox = num2;
            tempBox.boxPresent = TRUE;
            tempBox.active = TRUE;
            m_pSecondaryHurtBoxes[a] = tempBox;
           }
         }
         //==========================================
         //Finished reading secondary hurtbox data. =
         //==========================================
				 //==========================
				 //Read Collision Box Data. =
				 //==========================
				 for(int a = 0; a < 4; a++)
				 {
					 char collisionBoxName[37] = "collisionbox";
					 char tempString[21];
					 sprintf_s(tempString, "%d", a);
					 strcat_s(collisionBoxName, tempString);
					 DETECTIONBOX tempBox;
					 //int hurtBoxLength = strlen(obj->Attribute(hurtBoxName));
					 int j = 0;
					 int num2 = 0;
					 int modifier = 1;
					 char ch = obj->Attribute(collisionBoxName)[j];
					 if(ch == '-')
					 {
					 	 modifier = -1;
					 	 ch = obj->Attribute(collisionBoxName)[++j];
					 }
					 //find xOffset...
					 while(ch != ',' && ch >= '0' && ch <= '9')
					 {
					 	 num2 = num2*10 + ch - '0';
					 	 ch = obj->Attribute(collisionBoxName)[++j];
					 }
					 num2*=modifier;
					 tempBox.xOffset = (float)num2;
					 //find yOffset...
					 ch = obj->Attribute(collisionBoxName)[++j];
					 num2 = 0;
					 if(ch == '-')
					 {
					   modifier = -1;
						 ch = obj->Attribute(collisionBoxName)[++j];
					 }
					 else
					 	 modifier = 1;
					 while(ch != ',' && ch >= '0' && ch <= '9')
					 {
						 num2 = num2*10 + ch - '0';
						 ch = obj->Attribute(collisionBoxName)[++j];
					 }
					 num2*=modifier;
					 tempBox.yOffset = (float)num2;
					 //find xSize...
					 ch = obj->Attribute(collisionBoxName)[++j];
					 num2 = 0;
					 int counter = 0;
					 float deciNum = -1;
					 while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
					 {
						 if(ch == '.')
						 {
							 deciNum = 0;
							 ch = obj->Attribute(collisionBoxName)[++j];
						 }
						 else if(deciNum < 0)
						 {
							 num2 = num2*10 + ch - '0';
							 ch = obj->Attribute(collisionBoxName)[++j];
						 }
						 else
						 {
							 counter++;
							 deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
							 ch = obj->Attribute(collisionBoxName)[++j];
						 }
					 }
					 tempBox.xSize = (float)(num2+deciNum);
					 //find ySize..
					 ch = obj->Attribute(collisionBoxName)[++j];
					 num2 = 0;
					 counter = 0;
					 deciNum = -1;
					 while((ch != ',' && ch >= '0' && ch <= '9') || ch == '.')
					 {
						 if(ch == '.')
						 {
							 deciNum = 0;
							 ch = obj->Attribute(collisionBoxName)[++j];
						 }
						 else if(deciNum < 0)
						 {
							 num2 = num2*10 + ch - '0';
							 ch = obj->Attribute(collisionBoxName)[++j];
						 }
						 else
						 {
							 counter++;
							 deciNum = deciNum + ((ch - '0')/(pow(10.0f,counter))); //handle decimals...
							 ch = obj->Attribute(collisionBoxName)[++j];
						 }
					 }
					 tempBox.ySize = (float)(num2+deciNum);
					 tempBox.secondaryBox = -1;
					 tempBox.boxPresent = TRUE;
					 tempBox.active = TRUE;
					 m_pCollisionBoxes[a] = tempBox;
				 }
				 //======================================
				 //Finished Reading Collision Box Data. =
				 //======================================
      }
    }
  }
}



/// Draw game object.
/// Draws the current sprite frame at the current position, then
/// computes which frame is to be drawn next time.

//IDEA FOR DRAWING ANIMATIONS:
//make this a virtual function and override it in the character class.  instead of drawing off the one
//base animation, we have each character have a separate array for each animation which we can simply load
//from the xml file (might need to override the load function as well).  Just use a switch statement
//to select the appropriate animation based off of some state flag, and voila.
void CCharObject::draw(){ //draw

  int t = m_nFrameInterval;

	float fOrientation = 0;
  if(m_pAnimation != NULL){ //if there's an animation sequence
    //draw current frame
		int tempFrame = ARRAY(currentAnimationState,m_nCurrentFrame);
    g_cSpriteManager.Draw(m_nObjectType, m_structLocation, fOrientation, 1.0f, tempFrame, facing);
    if(hitBoxDisplay == 1)
    {
      g_cSpriteManager.DrawDetectionBox(COLLISION_BOX, m_structLocation, m_pCollisionBoxes[m_nCollisionState].xSize, m_pCollisionBoxes[m_nCollisionState].ySize, m_pCollisionBoxes[m_nCollisionState].xOffset*(facing-5),m_pCollisionBoxes[m_nCollisionState].yOffset);
      if(m_pHurtBoxes[tempFrame].boxPresent == 1)
      {
        g_cSpriteManager.DrawDetectionBox(HURT_BOX, m_structLocation, m_pHurtBoxes[tempFrame].xSize, m_pHurtBoxes[tempFrame].ySize, m_pHurtBoxes[tempFrame].xOffset*(facing-5), m_pHurtBoxes[tempFrame].yOffset);
        int tempInt = m_pHurtBoxes[tempFrame].secondaryBox;
        while(tempInt >= 0)
        {
          g_cSpriteManager.DrawDetectionBox(HURT_BOX, m_structLocation, m_pSecondaryHurtBoxes[tempInt].xSize, m_pSecondaryHurtBoxes[tempInt].ySize, m_pSecondaryHurtBoxes[tempInt].xOffset*(facing-5), m_pSecondaryHurtBoxes[tempInt].yOffset);
          tempInt = m_pSecondaryHurtBoxes[tempInt].secondaryBox;
        }
      }
      if(m_pHitBoxes[tempFrame].boxPresent == 1)
      {
        g_cSpriteManager.DrawDetectionBox(HIT_BOX, m_structLocation, m_pHitBoxes[tempFrame].xSize, m_pHitBoxes[tempFrame].ySize, m_pHitBoxes[tempFrame].xOffset*(facing-5), m_pHitBoxes[tempFrame].yOffset);
        int tempInt = m_pHitBoxes[tempFrame].secondaryBox;
        while(tempInt >= 0)
        {
          g_cSpriteManager.DrawDetectionBox(HIT_BOX, m_structLocation, m_pSecondaryHitBoxes[tempInt].xSize, m_pSecondaryHitBoxes[tempInt].ySize, m_pSecondaryHitBoxes[tempInt].xOffset*(facing-5), m_pSecondaryHitBoxes[tempInt].yOffset);
          tempInt = m_pSecondaryHitBoxes[tempInt].secondaryBox;
        }
      }
    }
    //advance to next frame
    if(g_cTimer.elapsed(m_nLastFrameTime, t)) //if enough time passed
		{
      //increment and loop if necessary
      if(ARRAY(currentAnimationState,++m_nCurrentFrame) == -1 && m_bCycleSprite)
      {
				if(currentAnimationState == 5 || currentAnimationState == 6)	//if in hitstun
				{
					int tempTimer = g_cTimer.time();
					//the 400 in the next line is for 2 frames, since this characters frame interval is 200.
					//it's set to 2 frames because the animation for transitioning out of hitstun is 2 frames.
					if(currentAnimationState == 6 && hitStunTime-(tempTimer-m_nStunTimer) <=400)	//time to transition
						setCurrentAnimation(7);						//out of hitstun (state 7)
					else
						setCurrentAnimation(6);
				}
				else if(currentAnimationState == 12)	//airborne hitstun
				{
					int tempTimer = g_cTimer.time();
					if(hitStunTime-(tempTimer-m_nStunTimer) <= 0)
					{
						//If we add air teching later, we can put the code here.
						setCurrentAnimation(11);	//Airborne state
					}
					else	//if hitstun isn't up
						setCurrentAnimation(12);	//loop
				}
				else if(currentAnimationState == 8 || currentAnimationState == 9)	//transition into blockstun and blockstun
				{																																//states
					int tempTimer = g_cTimer.time();
					if(currentAnimationState == 9 && hitStunTime-(tempTimer-m_nStunTimer) <= 400)
						setCurrentAnimation(10);
					else
						setCurrentAnimation(9);
				}
				else if(currentAnimationState == 1)	//loop walking
				{
					setCurrentAnimation(1);
				}
				else
				{
					if(currentAnimationState == 7 || currentAnimationState == 15)	//if hitstun has just ended
					{																															//or on wakeup
						if(m_nPlayerNumber == 1)
						{
							g_cObjectManager.resetCounter(2);
						}
						else	//so this is player 2
						{
							g_cObjectManager.resetCounter(1);
						}
					}
					if(airBorne == 1)
						setCurrentAnimation(11);
					else if(currentAnimationState == 14)	//Add any crouching states necessary...
						setCurrentAnimation(14);
					else
						setCurrentAnimation(0);
				}
      }
		}
  }
  else 
    g_cSpriteManager.Draw(m_nObjectType, m_structLocation, fOrientation, 1.0f, 0);//assume only one frame
}

void CCharObject::getHit(float hitStunDuration, float xKnockBack, float yKnockBack)
{
	if(yKnockBack < 0)	//if the attack puts them in an airborne state
	{
		setCurrentAnimation(12);	//set airborne hitstun state
		airBorne = 1;
	}
	else	//doesn't put them in the air
	{
		setCurrentAnimation(5);	//This should be the beginning of the hitstun animation
	}
	hitStunTime = hitStunDuration;	//This should be the duration of the hitstun.
	m_fXspeed = xKnockBack;	//These two are pretty self explanatory.
	m_fYspeed = yKnockBack;
	m_nStunTimer = g_cTimer.time();	//When the hitstun began.
}

void CCharObject::block(float blockStunDuration)
{
	setCurrentAnimation(8);	//This should be the beginning of the blockstun animation
	hitStunTime = blockStunDuration;
	//add pushback code here.
	m_nStunTimer = g_cTimer.time();	//When blockstun began.
}

//Returns whether or not the character is in hitstun.
int CCharObject::inStun()
{
	//list all hitstun animations here.
	if(currentAnimationState == 5 || currentAnimationState == 6 || currentAnimationState == 7
		|| currentAnimationState == 12)
		return TRUE;
	else
		return FALSE;
}

void CCharObject::interpretCommand(int directional, int attack)
{
	/*
	  236 = 1
	  623 = 2
	41236 = 3
	  214 = 4
		421 = 5
	63214 = 6
	   22 = 7
	*/
	if(m_nCanCancel == 1)
	{
		if(directional == 0)	//no command input
		{
			if(attack == 0)	//a attack
			{
				setCurrentAnimation(2);	//Set light jab animation.
				m_fXspeed = 0;
				m_nCurrentAttackHeight = 1;
				m_fCurrentAttackHitStun = 1200;
				m_fCurrentAttackBlockStun = 1000;
				m_fCurrentAttackXKnockBack = 0;
				m_fCurrentAttackYKnockBack = 0;
                m_nCurrentAttackDmg = 4;
			}
			else if(attack == 1)	//b attack
			{
				setCurrentAnimation(3);
				m_fXspeed = 0;
				m_nCurrentAttackHeight = 2;
				m_fCurrentAttackHitStun = 1600;
				m_fCurrentAttackBlockStun = 1400;
				m_fCurrentAttackXKnockBack = 0;
				m_fCurrentAttackYKnockBack = 0;
                m_nCurrentAttackDmg = 8;
			}
			else if(attack == 2)	//c attack
			{
				setCurrentAnimation(4);
				m_fXspeed = 0;
				m_nCurrentAttackHeight = 0;
				m_fCurrentAttackHitStun = 2000;
				m_fCurrentAttackBlockStun = 1800;
				m_fCurrentAttackXKnockBack = 20*(facing-5);
				m_fCurrentAttackYKnockBack = -15;
                m_nCurrentAttackDmg = 12;
			}
		}
	}
}