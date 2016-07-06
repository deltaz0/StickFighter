#include "Russian.h"

#include "input.h"
#include "defines.h" 
#include "timer.h" 
#include "spriteman.h" 
#include "sound.h"
#include "score.h" //score manager
#include <math.h>
#include "Objman.h"

#define ARRAY(X, Y) (m_pAnimation[(X) + ((Y) * (m_nAnimationCount))])

extern int curFrameNum;

extern int p1SuperCurrent;
extern int p2SuperCurrent;
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

CRussian::CRussian(ObjectType object, int playerNum, char* name, D3DXVECTOR3 location,
      float xspeed, float yspeed): CGameObject(object, playerNum, name, location, xspeed, yspeed){ ///< Constructor.
  //object-dependent settings loaded from XML
	m_nLongestAnimation = 1;
  LoadSettings(name);
	dashSpeed = 30;
	fAirDashSpeed = 28;
	bAirDashSpeed = 18;
  walkSpeed = 7;
  jumpSpeed = -26;
  doubleJumpSpeed = -26;
	//Set all of these to whatever speeds you want your character to have.  If you want to specify a different
	//speed for backwards movement, just make another variable and change the directional checks (they should be
	//labeled) in the walk/dash/whatever function.
}

CRussian::~CRussian(){
	delete [] m_pCollisionBoxes;
	delete [] m_pSecondaryHurtBoxes;
	delete [] m_pSecondaryHitBoxes;
	delete [] m_pHurtBoxes;
	delete [] m_pHitBoxes;
  delete [] m_pAnimation;
  
  
  
  
}

void CRussian::expendHitBox()
{
	if(currentAnimationState)	//knee of justice
		m_nCanCancel = 1;
	if(superCurrent+4 >= 100)
	{
		superCurrent = 100;
		if(m_nPlayerNumber == 1)
			p1SuperCurrent == 100;
		else if(m_nPlayerNumber == 2)
			p2SuperCurrent == 100;
	}
	else 
		{
			superCurrent += 4;
	if(m_nPlayerNumber == 1)
		p1SuperCurrent += 4;
	else if(m_nPlayerNumber == 2)
		p2SuperCurrent += 4;
	}
  freezeTimer = g_cTimer.time() + m_nCurrentHitLag;
  freeze = 1;
  freezeGap = g_cTimer.time() - m_nLastFrameTime;
	if(m_nPlayerNumber == 1)
		g_cObjectManager.hitChar(1, m_nCurrentAttackHeight, m_fCurrentAttackHitStun, m_fCurrentAttackBlockStun, m_fCurrentAttackXKnockBack, m_fCurrentAttackYKnockBack, m_nCurrentAttackDmg);
	else if(m_nPlayerNumber == 2)
		g_cObjectManager.hitChar(2, m_nCurrentAttackHeight, m_fCurrentAttackHitStun, m_fCurrentAttackBlockStun, m_fCurrentAttackXKnockBack, m_fCurrentAttackYKnockBack, m_nCurrentAttackDmg);
	DETECTIONBOX testtwo = m_pHitBoxes[ARRAY(currentAnimationState, m_nCurrentFrame)];
	int tempIndex2 = m_pAnimation[(currentAnimationState) + ((m_nCurrentFrame) * (m_nAnimationCount))];
	m_pHitBoxes[ARRAY(currentAnimationState, m_nCurrentFrame)].active = FALSE;
	int tempInt = m_pHitBoxes[ARRAY(currentAnimationState, m_nCurrentFrame)].secondaryBox;
	while(tempInt >= 0)
	{
		m_pSecondaryHitBoxes[tempInt].active = FALSE;
		tempInt = m_pSecondaryHitBoxes[tempInt].secondaryBox;
	}
	int tempTracker = 1;
	while(m_pHitBoxes[ARRAY(currentAnimationState, m_nCurrentFrame)+tempTracker].boxPresent == TRUE && m_pHitBoxes[ARRAY(currentAnimationState, m_nCurrentFrame)+tempTracker].active == TRUE)
	{
		m_pHitBoxes[ARRAY(currentAnimationState, m_nCurrentFrame)+tempTracker].active = FALSE;
		tempInt = m_pHitBoxes[ARRAY(currentAnimationState, m_nCurrentFrame)+tempTracker].secondaryBox;
		while(tempInt >= 0)
		{
			m_pSecondaryHitBoxes[tempInt].active = FALSE;
			tempInt = m_pSecondaryHitBoxes[tempInt].secondaryBox;
		}
		tempTracker++;
	}
	
}

void CRussian::setCurrentAnimation(int newState)
{
	if(freeze == 0)
	{
		currentAnimationState = newState;
		m_nCurrentFrame = 0;
		m_nMovementCheckFrame = -1;
		m_nCanJump = 0;
		//In the below if statement, use or statements to check for any and all states from which you can attack (so
		//crouching, standing, airborne, etc).  Don't include specific attacks in this, just general character states.
		if(newState == 0 || newState == 1 || newState == 2 || newState == 3 || newState == 4 || newState == 5
			 || newState == 6 || newState == 9 || newState == 10 || newState == 11 || newState == 12 || newState == 13
			  || newState == 14 || newState == 15 || newState == 50)
		{																	
			m_nCanCancel = 1;
			if(newState != 50)
				m_nCanJump = 1;
			//just like above, except include only those states here that the character can walk from (so not 
			//crouching, for instance).  All this does is make you walk if you're holding a direction as an animation
			//ends.
			if(newState == 0)	
			{									
				m_nCollisionState = 1;
				if(rightHeld == 1 && redashRight != 2 && redashLeft !=2)
				{
					walk(6);
				}
				else if(leftHeld == 1 && redashRight != 2 && redashLeft !=2)
				{
					walk(4);
				}
				else
				{
					redashRight = 0;
					redashLeft = 0;
				}
			}
			//Set this one to the crouching state
			if(newState == 11 || newState == 12 || newState == 13)	//if crouching
			{
				m_nCollisionState = 0;	//set crouching collision
			}
			else if(newState == 9 || newState == 50)
			{
				m_nCollisionState =  2;	//set air collision
			}
		}
		//Set this one to be your wakeup state
		else if(newState == 15)	//Wakeup collision
		{
			m_nCollisionState = 3;
		}
		else
		{
			m_nCanCancel = 0;
		}
		int temp = 0;
		int tempImageNum = -1;
		tempImageNum = m_pAnimation[currentAnimationState + (m_nCurrentFrame*m_nAnimationCount)];

		while(tempImageNum != -1)	//check every frame
		{																									//of the animation for hitboxes.
			m_pHitBoxes[tempImageNum].active = TRUE;
			int tempInt = m_pHitBoxes[tempImageNum].secondaryBox;
			while(tempInt >= 0)
			{
				m_pSecondaryHitBoxes[tempInt].active = TRUE;
				tempInt = m_pSecondaryHitBoxes[tempInt].secondaryBox;
			}
			temp++;
			tempImageNum = m_pAnimation[currentAnimationState+((m_nCurrentFrame+temp)*m_nAnimationCount)];
		}
		m_nLastFrameTime = g_cTimer.time();
	}
	else if(freeze == 1)
	{
		freezeBuffered = newState;
	}
}

void CRussian::dash(int direction)
{
	if(freeze == 0)
	{
		//set this to check for any and all animation states that a character can dash out of
		if(currentAnimationState == 0 || currentAnimationState == 1 || currentAnimationState == 11 
			|| currentAnimationState == 14 || currentAnimationState == 2 || currentAnimationState == 3
			|| currentAnimationState == 4 || currentAnimationState == 5 || currentAnimationState == 6
			|| currentAnimationState == 9 || currentAnimationState == 10 || currentAnimationState == 11
			|| currentAnimationState == 12 || currentAnimationState == 13 || currentAnimationState == 15)
		{																								
		
			if(m_structLocation.y > .1)  //if character is airborn
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
					airTime = 15;	//Number of frames your character will suspend downards movement after an airdash
												//Lower this and he'll start to fall sooner.
					airDashesLeft--;
					if(direction == 6)//If airdashing right..
					{
						if(facing == 6)//and facing right...
						{
							setCurrentAnimation(14);
													g_pInputManager->playSound(AFDASH_SOUND);
							m_fYspeed = -5;
							m_fXspeed = fAirDashSpeed+(.2*m_fXspeed);  //airdash forward code
						}
						else if(facing == 4)  //else if facing left...
						{
							setCurrentAnimation(15);
													g_pInputManager->playSound(ABDASH_SOUND);
							m_fXspeed = bAirDashSpeed+(.2*m_fXspeed);  // airdash backwards code
						}
					}
					else if(direction == 4)//If airdashing left...
					{
						if(facing == 4)//and facing left
						{
							setCurrentAnimation(14);
													g_pInputManager->playSound(AFDASH_SOUND);
							m_fYspeed = -2;
							m_fXspeed = (-1)*fAirDashSpeed+(.2*m_fXspeed);  //airdash forward code
						}
						else if(facing == 6)// else if facing right...
						{
							setCurrentAnimation(15);
													g_pInputManager->playSound(ABDASH_SOUND);
							m_fXspeed = (-1)*bAirDashSpeed+(.2*m_fXspeed);  //airdash backwards code
						}
					}
				}
			}
			else //else we're grounded
			{
				if(direction == 6) //if dashing right...
				{
					if(facing == 6) //and facing right...
					{
						setCurrentAnimation(5);
											g_pInputManager->playSound(FDASH_SOUND);
						m_fXspeed = dashSpeed; //dash forward code.
					}
					else if(facing == 4)  //else facing left...
					{
						setCurrentAnimation(7);
											g_pInputManager->playSound(BDASH_SOUND);
						m_nMovementCheckFrame = 3;
						m_fXspeed = 5;  //dash backwards code.
						m_fYspeed = -10;
					}
				}
				else if(direction  == 4)  //if dashing left...
				{
					if(facing == 4) //and facing left...
					{
						setCurrentAnimation(5);
											g_pInputManager->playSound(FDASH_SOUND);
						m_fXspeed = (-1)*dashSpeed;  //dash forward code.
					}
					else if(facing == 6)  //else facing right...
					{
						setCurrentAnimation(7);
											g_pInputManager->playSound(BDASH_SOUND);
						m_nMovementCheckFrame = 3;
						m_fXspeed = -5; //dash backwards code.
						m_fYspeed = -10;
					}
				}
			}
		}
	}
}

void CRussian::walk(int direction)
{
	if(freeze == 0)
	{
		if(currentAnimationState == 0 || currentAnimationState == 1 || currentAnimationState == 2 
			|| currentAnimationState == 3 || currentAnimationState == 4 || currentAnimationState == 5
			|| currentAnimationState == 6 || currentAnimationState == 13 || (currentAnimationState == 12 && downHeld == 0))//set this to all animation states that a character
		{																								//can walk out of.  The downHeld == 0 is there just so you can
			if(m_structLocation.y > .1)  //if airborn		//walk out of crouch without having to transition back to neutral
			{																							//first.  This way, you won't move while crouching if you push
				m_nCollisionState = 2;											//down and sideways.
			}
			else  //if grounded
			{
				m_nCollisionState = 1;
				if(direction == 6)  //if moving right...
				{
					if(facing == 6) //and if facing right...
					{
						setCurrentAnimation(1);
						m_fXspeed = walkSpeed; //walk forward code.
					}
					else if(facing == 4)  //else if facing left...
					{
						setCurrentAnimation(3);
						m_fXspeed = walkSpeed;  //walk backward code.
					}
				}
				else if(direction == 4) //if moving left...
				{
					if(facing == 4) //and facing left...
					{
						setCurrentAnimation(1);
						m_fXspeed = (-1)*walkSpeed;  //walk forward code.
					}
					else if(facing == 6)  //else facing right...
					{
						setCurrentAnimation(3);
						m_fXspeed = (-1)*walkSpeed; //walk backward code.
					}
				}
			}
		}
	}
}

void CRussian::crouch()
{
	if(freeze == 0)
	{
		if(airBorne == 0 && (currentAnimationState == 0 || currentAnimationState == 1 || currentAnimationState == 2
			 || currentAnimationState == 3 || currentAnimationState == 4 || currentAnimationState == 5
			  || currentAnimationState == 6 || currentAnimationState == 13))
		{
			setCurrentAnimation(11);	//Set this to your crouching animation state.
            g_cObjectManager.resetCounter(m_nPlayerNumber);
			m_fXspeed = 0;
		}
	}
}

void CRussian::stopCrouch()
{
	if(freeze == 0)
	{
		//Change the 14 to the number for your crouching state
		if((currentAnimationState == 11 || currentAnimationState == 12) && freeze == 0)	//Add an or check for every animation state involving crouching
		{
			setCurrentAnimation(13);
		}
	}
}

void CRussian::jump()
{
	if(freeze == 0)
	{
		//set this to check for any and all animation states that a character can jump out of
		if((m_nCanJump || currentAnimationState == 0 || currentAnimationState == 1 || currentAnimationState == 2 || currentAnimationState == 3 ||
			 currentAnimationState == 4 || currentAnimationState == 5 || currentAnimationState == 6 || currentAnimationState == 8 ||
			 currentAnimationState == 9 || currentAnimationState == 10 || currentAnimationState == 13 || currentAnimationState == 14
			 || currentAnimationState == 15) && (currentAnimationState != 38 && currentAnimationState != 39 && currentAnimationState != 41 && 
			 currentAnimationState != 42 && currentAnimationState != 44 && currentAnimationState != 45))
		{	
			if(currentAnimationState != 8 && airBorne == 0 && m_nJumpSquat == 0)	//Not in jumpsquat and on ground
			{
				setCurrentAnimation(8);
				m_nJumpSquat = 1;
			}
			else if(currentAnimationState != 8)
			{	//We are in jumpsquat...
				//setCurrentAnimation(9);	//Moved lower.
				//Set this to whatever your jump animation will be.  Since we'll eventually
				//want a jumpsquat animation, this should be the jumpsquat animation, and the actual speed should be
				//set when the character transitions into their aerial jump animation.  Later on in this code, in the
				//draw function, you'll see where it loops animations.  Basically, you'd set it up so that whenever an
				//animation ends, it checks if the one that just ended was your jumpsquat, and if it was, set it to your
				//airborne animation and set the speed as needed.  
				if(m_structLocation.y > .1)  //if you're in the air...
				{
					if(airJumpsLeft > 0)
					{
						setCurrentAnimation(9);
											g_pInputManager->playSound(AIRJUMP_SOUND);
						airJumpsLeft--;
						m_fYspeed = doubleJumpSpeed;
						g_cObjectManager.checkFacing(m_nPlayerNumber);
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
				else if(m_nJumpSquat == 1)  //if grounded...
				{
					m_nJumpSquat = 0;
					setCurrentAnimation(9);
									g_pInputManager->playSound(JUMP_SOUND);
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
	}
}

void CRussian::stop()
{
	if(freeze == 0 && (currentAnimationState == 1 || currentAnimationState == 2 || currentAnimationState == 3
		|| currentAnimationState == 4 || currentAnimationState == 5 || currentAnimationState == 6))	//check any animation states that involve dashing (grounded) or walking.
	{																//again, separate them with "or" flags.
		if(m_structLocation.y <= .1) //if grounded
		{
			m_nCollisionState = 1;
			m_fXspeed = 0;  //stop movement.
		}
	  setCurrentAnimation(0);	//Set this to your neutral state
      g_cObjectManager.resetCounter(m_nPlayerNumber);
	}
}

/// Move object. 
/// The distance that an object moves depends on its speed, 
/// and the amount of time since it last moved.

void CRussian::move(){ //move object
	if(m_bStatic)return; //static objects don't move
	if(freeze == 1)
	{
		if(g_cTimer.time() < freezeTimer)
		{
			if(currentAnimationState != 52 )
				m_nLastFrameTime = g_cTimer.time();
			else if(ARRAY(currentAnimationState,(m_nCurrentFrame+1)) == -1)
				m_nLastFrameTime = g_cTimer.time();
			m_nLastMoveTime = g_cTimer.time();
			m_nPushBackStartTime = g_cTimer.time();
			return;
		}
		else
		{
			m_nLastFrameTime = g_cTimer.time() - freezeGap;
			freeze = 0;
			if(freezeBuffered >= 0)
			{
				setCurrentAnimation(freezeBuffered);
				freezeBuffered = -1;
			}
		}
	}
	int groundBounceFlag = 0;
	const float XSCALE = 32.0f; //to scale back horizontal motion
	const float YSCALE = 32.0f; //to scale back vertical motion
	const float MARGIN = 50.0f; //margin on top of page

	float xdelta = 0.0f, ydelta = 0.0f; //change in position
	int time = g_cTimer.time(); //current time
	int tfactor = time-m_nLastMoveTime; //time since last move

	//compute xdelta and ydelta, horizontal and vertical distance
	xdelta = (m_fXspeed * (float)tfactor)/XSCALE; //x distance moved  
	ydelta = (m_fYspeed * (float)tfactor)/YSCALE; //y distance moved
  




	//delta motion
	if(!(m_structLocation.x + xdelta > 2*g_nScreenWidth) && !(m_structLocation.x  + xdelta < 0))//(stage has a range of 0 to 2g_nScreenWidth)
	{
			if(!(abs(g_cObjectManager.GetXAvg() - (m_structLocation.x + xdelta)) >= g_nScreenWidth/2))//(players cannot be farther than g_nScreenWidth/2 apart)
			m_structLocation.x += xdelta; //x motion
	}
	else if(m_structLocation.x + xdelta > 2*g_nScreenWidth)	//if instead moving off right side
	{
		m_structLocation.x = 2*g_nScreenWidth;
		if(m_bWallBounce == TRUE)
		{
			/*if(facing == 6)
				facing = 4;
			else if(facing == 4)
				facing = 6;*/
			m_fYspeed = -20;
			m_fXspeed = 4*(facing-5);
			m_bWallBounce = FALSE;
			hitStunTime = m_fPostBounceStun;
			m_nStunTimer = g_cTimer.time();
		}
	}
	else if(m_structLocation.x + xdelta < 0)	//if moving off left side
	{
		m_structLocation.x = 0;
		if(m_bWallBounce == TRUE)
		{
			m_fYspeed = -20;
			m_fXspeed = 4*(facing-5);
			m_bWallBounce = FALSE;
			hitStunTime = m_fPostBounceStun;
			m_nStunTimer = g_cTimer.time();
		
		}
	}
	//pushback
	if(time <= m_nPushBackStartTime + m_nPushBackDurationInitial)
	{
		int tempTime = time-m_nPushBackStartTime;	//amount of time that has passed since pushback started
		m_nPushBackDurationCurrent = m_nPushBackDurationInitial-tempTime;	//this should leave current duration as a
																																	//value from 0 to the intial duration.
		float tempTFactor = (float)m_nPushBackDurationCurrent/m_nPushBackDurationInitial;
		xdelta = (m_fPushBackSpeed * tempTFactor);
		if(!(m_structLocation.x + xdelta >= 2*g_nScreenWidth) && !(m_structLocation.x  + xdelta <= 0))//(stage has a range of 0 to 2g_nScreenWidth)
		{
				if(!(abs(g_cObjectManager.GetXAvg() - (m_structLocation.x + xdelta)) >= g_nScreenWidth/2))//(players cannot be farther than g_nScreenWidth/2 apart)
				m_structLocation.x += xdelta; //x motion
		}
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
 
	//hitting the ground
	if(m_structLocation.y <= 0.1f) //below bottom
	{
		if(m_bGroundBounce == TRUE)
		{
			if((currentAnimationState == 25 || currentAnimationState == 26) && m_structLocation.y <= -70)
			{
				m_structLocation.y = -70;
				m_fYspeed = -20;	//bounce
				hitStunTime = m_fPostBounceStun;
				m_nStunTimer = g_cTimer.time();
				m_nReboundCount++;
				//groundBounceFlag = 1;
				//setCurrentAnimation(25);
			}
		}
		//if((currentAnimationState == 25 || currentAnimationState == 26)/* && m_bGroundBounce && m_structLocation.y <= -3*/)
		//{
		//	if(m_bGroundBounce && m_structLocation.y <= -3)
		//	{
		//	m_structLocation.y = -3;
		//	m_fYspeed = -m_fYspeed;	//bounce
		//	hitStunTime = m_fPostBounceStun;
		//	m_nStunTimer = g_cTimer.time();
		//	}
		//}
		else
		{
			m_structLocation.y = 0.0f; //fix location
			m_fYspeed = 0.0f; //this fixes orientation of object too
		}
	}
		if(m_structLocation.y > .1f && m_nReboundCount > 0)
		{
			groundBounceFlag = 1;
		}

		if(m_structLocation.y > 0 )
		{
			if(airTime > 0)
			{
				if(currentAnimationState == 14)
					m_fYspeed += ((float)tfactor/100);
				else if(currentAnimationState == 15 && m_fYspeed != 0)
					m_fYspeed = 0;

				airTime-=((float)tfactor/16);
			}
			else
					m_fYspeed += ((float)tfactor/16);
		}
		else /*if(currentAnimationState != 25 && currentAnimationState != 26)*///if you aren't in the air (so you're grounded)
		{																																	//and not in hitstun
			if(m_fXspeed > 0 && rightHeld == 0 && currentAnimationState != 25 && currentAnimationState != 26) //if you aren't holding a direction on landing, stop movement.
			{
				if(currentAnimationState != 7 && m_nMovementCheckFrame == -1)	//might need to change this back to m_fXspeed = 0;
					m_fXspeed = 0;
			}
			else if(m_fXspeed < 0 && leftHeld == 0 && currentAnimationState != 25 && currentAnimationState != 26)
			{
				if(currentAnimationState != 7 && m_nMovementCheckFrame == -1)
					m_fXspeed = 0;	//without this if statement, this interferes with backdashes.
			}
			if(airBorne == 1) //Player is landing from being in the air
			{
				if(m_bGroundBounce == FALSE)
					airBorne = 0;
				if(currentAnimationState != 25 && currentAnimationState != 26)
				{
					m_nCollisionState = 1;
					if(rightHeld == 1)
					{
						if(redashRight == 1)
						{
							g_cObjectManager.checkFacing(m_nPlayerNumber);
							dash(6);
							redashRight = 2;
						}
						else
						{
							g_cObjectManager.checkFacing(m_nPlayerNumber);
							walk(6);
						}
					}
					else if(leftHeld == 1)
					{
						if(redashLeft == 1)
						{
							g_cObjectManager.checkFacing(m_nPlayerNumber);
							dash(4);
							redashLeft = 2;
						}
						else
						{
							g_cObjectManager.checkFacing(m_nPlayerNumber);
							walk(4);
						}
					}
				}
				airDashesLeft = 1;	//This resets your airdashes and air jumps.  If you want your character to have
				airJumpsLeft = 1;		//more than one, you'll have to A) change this and B) set the value in the constructor
				g_pInputManager->playSound(LANDING_SOUND);
				m_nCollisionState = 1;
				//Change this 12 to be your aerial hitstun state
				if(currentAnimationState != 25 && currentAnimationState != 26 &&redashLeft != 2 && redashRight != 2)	//if you are not in aerial hitstun
				{
					if(downHeld == 1 || (m_nPlayerNumber == 1 && g_pInputManager->X1DOWN == 1) || (m_nPlayerNumber == 2 && g_pInputManager->X2DOWN == 1))
                    {
						//Set the 14 to be your crouching state
						setCurrentAnimation(11);	//If holding down, crouch
                        g_cObjectManager.resetCounter(m_nPlayerNumber);
						m_fXspeed = 0;
                    }
					else
                    {
						//Set 0 to your neutral state
						setCurrentAnimation(0);	//This should be your standing neutral state.
                        g_cObjectManager.resetCounter(m_nPlayerNumber);
				
                    }
                }
				else if(currentAnimationState == 25 || currentAnimationState == 26)	//if you land in aerial hitstun
				{
					if(m_bGroundBounce == FALSE)
					{
						//Change the 15 to your knockdown/wakeup state
						setCurrentAnimation(27);	//knockdown/wakeup animation
						m_nCollisionState = 3;
					}
				}
			}
			if(upHeld)
			{ 
				jump();
			}
		}
		if(groundBounceFlag == 1)
		{
			m_bGroundBounce = FALSE;
			m_nReboundCount = 0;
		}
	//record time of move
	m_nLastMoveTime = time;
}

/// Load settings.
/// Loads settings for object from g_xmlSettings.
/// \param name name of object as found in name tag of XML settings file

//You do not need to change ANYTHING in this function.  Yay.
void CRussian::LoadSettings(char* name)
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
          //m_pAnimationOffsets = new ANIMATIONOFFSET[m_nAnimationCount];
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
          /*ANIMATIONOFFSET offset;
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
            
          }*/
					//==========================
					//Finished Loading Indices =
					//==========================

					//=================
					//Load Animations =
					//=================
          int i = 0; //character index
          int count = 0; //number of frame numbers input
          int num; //frame number
					
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
							tempBox.secondaryBox = -1;
							tempBox.active = FALSE;
							m_pHitBoxes[a] = tempBox;
						}
					
						//================================
						//End New Load Hitboxes Function =
						//================================
						
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
							tempBox.secondaryBox = -1;
							tempBox.active = FALSE;
							m_pHurtBoxes[a] = tempBox;
						}
					}
						//====================================
						//Finished New Load Hurtbox Function =
						//====================================
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

void CRussian::draw(){ //draw

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
	if(currentAnimationState == 24)	//in crouching heavy
	{
		if(m_nCurrentFrame == 16)
		{        
				m_nCurrentAttackHeight = 1;
				m_fCurrentAttackHitStun = 1200+m_nCurrentHitLag;
				m_fCurrentAttackBlockStun = 500+m_nCurrentHitLag;
				m_fCurrentAttackXKnockBack = 10*(facing-5);
				m_fCurrentAttackYKnockBack = 20;
				m_nCurrentAttackDmg = 6;
				m_nCanJump = 0;
				m_fCurrentAttackBlockPushBackSpeed = 0;
			  m_fCurrentAttackHitPushBackSpeed = 0;
				m_nCanCancel = 0;
				m_fCurrentAttackPostBounceStun = 1000;
				m_bCurrentAttackGroundBounce = TRUE;
				m_bCurrentAttackWallBounce = FALSE;

		}
	}
		if(m_nMovementCheckFrame >= 0)
		{
			switch(currentAnimationState)
			{
				case 7:
					if(m_nCurrentFrame == 3)
					{
						m_fXspeed = -20*(facing-5);
					}
					else if(m_nCurrentFrame == 17)
					{
						//m_fXspeed = -25*(facing-5);
						m_fXspeed = -8*(facing-5);
					}
					else if(m_nCurrentFrame == 20)
					{
						m_fXspeed = 0;
						m_nMovementCheckFrame = -1;
					}
					break;
				case 36:
					if(m_nCurrentFrame == 21)	//begin movement on 21st frame
					{
						m_fXspeed = 25*(facing-5);
					}
					else if(m_nCurrentFrame == 28)	//stop movement on 28th
					{
						m_fXspeed = 0;
						m_nMovementCheckFrame = -1;
					}
					break;
				case 37:
					if(m_nCurrentFrame == 35)
					{
						m_fXspeed = 27*(facing-5);
					}
					else if(m_nCurrentFrame == 42)
					{
						m_fXspeed = 0;
						m_nMovementCheckFrame = -1;
					}
					break;
				case 20:
					if(m_nCurrentFrame == 0)
					{
						m_fXspeed = 7*(facing-5);
						int dlksjfdlks = 0;
					}
					else if(m_nCurrentFrame == 8)
					{
						m_fXspeed = 5*(facing-5);
					}
					else if(m_nCurrentFrame == 14)
					{
						m_fXspeed = 0;
						m_nMovementCheckFrame = -1;
					}
					break;
				default:
					break;
			}
		}
    //advance to next frame
		/*Alright, the code down here you'll need to change to transition between various states properly.
		Basically, whenever an animation ends (reaches the end of it's offset), this code here runs.  You
		can then check to see what animation just ended, and tell the game what animation to transition into
		afterwards. I've removed most of the miscelaneous transitions between states from the testChar file,
		but I left in those regarding transitions in blockstun/hitstun since they're a bit more complicated.
		Hopefully it'll make interpreting this mess a little easier.
		*/
    if(g_cTimer.elapsed(m_nLastFrameTime, t)) //if enough time passed
      //increment and loop if necessary
			//The animation array uses the -1 indicator as a marker for the end of the animation
      if(ARRAY(currentAnimationState,++m_nCurrentFrame) == -1 && m_bCycleSprite)
      {
				
		  m_nCanJump = 1;
				if(currentAnimationState == 1 || currentAnimationState == 2)	//Transition into, or loop, forward walk
				{
          //if(currentAnimation == 1 || currentAnimationState == 2 && interpretCommand()) //Check to see if you're trying to attack
					setCurrentAnimation(2);
				}
				else if(currentAnimationState == 3 || currentAnimationState == 4)	//Transition into, or loop, back walk
				{
					setCurrentAnimation(4);
				}
				else if(currentAnimationState == 5 || currentAnimationState == 6)	//Forward run checks
				{
					setCurrentAnimation(6);
				}
				else if(currentAnimationState == 8)	//if in jumpsquat
				{
					setCurrentAnimation(9);
					jump();	//Jump!
				}
				else if(currentAnimationState == 11 || currentAnimationState == 12) // if crouching
				{
					setCurrentAnimation(12);
				}
				else if(currentAnimationState == 14 || currentAnimationState == 15)	//if airdashing
				{
					setCurrentAnimation(10);	//Default to air neutral
                    //g_cObjectManager.resetCounter(m_nPlayerNumber);
				}
        else if(currentAnimationState == 16 || currentAnimationState == 17)
        {
          int timer = g_cTimer.time();
          if(timer >= (hitStunTime + m_nStunTimer))
          {
            setCurrentAnimation(18);
						m_bGroundBounce = FALSE;
						m_bWallBounce = FALSE;
          }
          else
          {
            setCurrentAnimation(17);
          }
        }
        else if(currentAnimationState == 18)
        {
          if(downHeld == 1 || (m_nPlayerNumber == 1 && g_pInputManager->X1DOWN == 1) || (m_nPlayerNumber == 2 && g_pInputManager->X2DOWN == 1))
          {
            setCurrentAnimation(11);
            g_cObjectManager.resetCounter(m_nPlayerNumber);
          }
          else if(leftHeld == 1)
          {
            setCurrentAnimation(0);
            g_cObjectManager.resetCounter(m_nPlayerNumber);
            walk(4);
          }
          else if(rightHeld == 1)
          {
            setCurrentAnimation(0);
            g_cObjectManager.resetCounter(m_nPlayerNumber);
            walk(6);
          }
          else// if(downHeld == 0 && rightHeld == 0 & leftHeld == 0 && upHeld == 0)
          {
            setCurrentAnimation(0);
            g_cObjectManager.resetCounter(m_nPlayerNumber);
          }
        }
        else if(currentAnimationState == 30 || currentAnimationState == 31) // crouching hitstun
        {
          int timer = g_cTimer.time();
          if(timer >= (hitStunTime + m_nStunTimer))
          {
            setCurrentAnimation(32);
						m_bGroundBounce = FALSE;
						m_bWallBounce = FALSE;
          }
          else
          {
            setCurrentAnimation(31);
          }
        }
        else if(currentAnimationState == 25 || currentAnimationState == 26)
        {
          int timer = g_cTimer.time();
          if(timer >= (hitStunTime + m_nStunTimer))
          {
            setCurrentAnimation(10);
            //g_cObjectManager.resetCounter(m_nPlayerNumber);
						m_bGroundBounce = FALSE;
						m_bWallBounce = FALSE;
          }
          else
          {
            setCurrentAnimation(26);
          }
        }
				else if(currentAnimationState == 38 || currentAnimationState == 39)	//if in standing blockstun
				{
					int timer = g_cTimer.time();
					if(timer >= (hitStunTime + m_nStunTimer))
					{
						setCurrentAnimation(40);
					}
					else
					{
						setCurrentAnimation(39);
					}
				}
				else if(currentAnimationState == 41 || currentAnimationState == 42)	//if in crouching blockstun
				{
					int timer = g_cTimer.time();
					if(timer >= (hitStunTime + m_nStunTimer))
					{
						setCurrentAnimation(43);
					}
					else
					{
						setCurrentAnimation(42);
					}
				}
				else if(currentAnimationState == 44 || currentAnimationState == 45)	//if in aerial blockstun
				{
					int timer = g_cTimer.time();
					if(timer >= (hitStunTime + m_nStunTimer))
					{
						setCurrentAnimation(46);
					}
					else
					{
						setCurrentAnimation(45);
					}
				}
        else if(currentAnimationState == 27)
        {
					if(m_bGroundBounce == TRUE)
					{
						setCurrentAnimation(25);
					}
					else
						setCurrentAnimation(28);
        }
        else if(currentAnimationState == 28)
        {
          setCurrentAnimation(29);
        }
				else if(currentAnimationState == 51)
				{
					setCurrentAnimation(52);
				}
				else if(currentAnimationState == 52)
				{
					setCurrentAnimation(53);
				}
				else if(currentAnimationState == 53)
				{
					setCurrentAnimation(50);
				}
				//else if(currentAnimationState == 5 || currentAnimationState == 6)	//if in hitstun
				//{
				//	int tempTimer = g_cTimer.time();
				//	//hitStunTime: Total amount of time character should be in hitstun.
				//	//tempTimer: Current time.
				//	//m_nStunTimer: Time when character entered hitstun.
				//	//Thus,hitStunTime-(tempTimer-m_nStunTimer) gives you the remaining amount of time until
				//	//stun wears off.
				//	//the 400 in the next line is for 2 frames, since this characters frame interval is 200.
				//	//it's set to 2 frames because the animation for transitioning out of hitstun is 2 frames.
				//	if(currentAnimationState == 6 && hitStunTime-(tempTimer-m_nStunTimer) <=400)	//time to transition
				//		setCurrentAnimation(7);						//out of hitstun (state 7)
				//	else
				//		setCurrentAnimation(6);
				//}
				//else if(currentAnimationState == 12)	//airborne hitstun
				//{
				//	int tempTimer = g_cTimer.time();
				//	if(hitStunTime-(tempTimer-m_nStunTimer) <= 0)
				//	{
				//		//If we add air teching later, we can put the code here.
				//		//What we'd probably need to do is add a new state that has the same animation as air hitstun
				//		//(or if we wanted we could actually make a new animation for it so players can actually SEE when
				//		//they come out of stun), and then add some code in the interpretCommands function to check if you're
				//		//in that state and tech if you are.
				//		setCurrentAnimation(11);	//Airborne state
				//	}
				//	else	//if hitstun isn't up
				//		setCurrentAnimation(12);	//loop
				//}
				//else if(currentAnimationState == 8 || currentAnimationState == 9)	//transition into blockstun and blockstun
				//{																																//states
				//	int tempTimer = g_cTimer.time();
				//	if(currentAnimationState == 9 && hitStunTime-(tempTimer-m_nStunTimer) <= 400)
				//		setCurrentAnimation(10);
				//	else
				//		setCurrentAnimation(9);
				//}
				else
				{
					//if(currentAnimationState == 7)	//if hitstun has just ended
					//{
					//	if(m_nPlayerNumber == 1)
					//	{
					//		g_cObjectManager.resetCounter(2);	//These reset the combo counter of the opposite character
					//	}
					//	else	//so this is player 2
					//	{
					//		g_cObjectManager.resetCounter(1);
					//	}
					//}
					if(airBorne == 1)
					{
						if(currentAnimationState == 50)
							setCurrentAnimation(50);
						else
                        {
							setCurrentAnimation(10);	//10 is air neutral
                            //g_cObjectManager.resetCounter(m_nPlayerNumber);
                        }
					}
					else if(currentAnimationState == 14)	//Add any crouching states necessary...
						setCurrentAnimation(14);
					else
					{
						if(downHeld == 1 || (m_nPlayerNumber == 1 && g_pInputManager->X1DOWN == 1) || (m_nPlayerNumber == 2 && g_pInputManager->X2DOWN == 1))
						{
							setCurrentAnimation(12);
						}
						else if(leftHeld == 1)
						{
							setCurrentAnimation(0);
                            g_cObjectManager.resetCounter(m_nPlayerNumber);
							walk(4);
						}
						else if(rightHeld == 1)
						{
							setCurrentAnimation(0);
                            g_cObjectManager.resetCounter(m_nPlayerNumber);
							walk(6);
						}
						else// if(downHeld == 0 && rightHeld == 0 & leftHeld == 0 && upHeld == 0)
						{
							setCurrentAnimation(0);
                            g_cObjectManager.resetCounter(m_nPlayerNumber);
						}
					}
				}
      }
			if(m_nPlayerNumber == 1)
				curFrameNum = ARRAY(currentAnimationState,m_nCurrentFrame);
  }
  else 
    g_cSpriteManager.Draw(m_nObjectType, m_structLocation, fOrientation, 1.0f, 0);//assume only one frame
}

void CRussian::getHit(float hitStunDuration, int hitLagDuration, float xKnockBack, float yKnockBack, float postBounceStun, BOOL groundBounce, BOOL wallBounce)
{
	if(freeze == 0)
	{
		float tempKnockBack = yKnockBack;
		m_bGroundBounce = groundBounce;
		m_bWallBounce = wallBounce;
		m_fPostBounceStun = postBounceStun;
		if(yKnockBack < 0)	//if the attack puts them in an airborne state
		{
					g_pInputManager->playSound(HHIT_SOUND);
			setCurrentAnimation(25);	//set airborne hitstun state
			airBorne = 1;
		}
		else	//doesn't put them in the air
		{
			if(airBorne == 1)
			{
					g_pInputManager->playSound(HHIT_SOUND);
				setCurrentAnimation(25);  //airborne hit state, set tempKnockback value to -1 or -2
				m_fYspeed = -14;
				m_fXspeed = -5*(facing-5);
			}
			else if(downHeld == 0)
			{
					g_pInputManager->playSound(LHIT_SOUND);
				setCurrentAnimation(16);	//This should be the beginning of the hitstun animation standing
			}
			else
			{
					g_pInputManager->playSound(LHIT_SOUND);
  			setCurrentAnimation(30);	//This should be the beginning of the hitstun animation crouching
			}
		}
		hitStunTime = hitStunDuration;	//This should be the duration of the hitstun.
		if(xKnockBack != 0)
			m_fXspeed = xKnockBack;	
		if(groundBounce == TRUE)
		{
			if(airBorne == 1)
				m_fYspeed = tempKnockBack;
		}
		else if(tempKnockBack != 0)
			m_fYspeed = tempKnockBack;
		freezeTimer = g_cTimer.time() + hitLagDuration;
		freeze = 1;
		freezeGap = g_cTimer.time() - m_nLastFrameTime;
		m_nStunTimer = g_cTimer.time();	//When the hitstun began.
	}
}

void CRussian::block(float blockStunDuration, int hitLagDuration)
{
	if(freeze == 0)
	{
		//Will probably need to change this later to check if you're in the air or not.  If you are,
		//you can just set the animation state to your air hitstun state and set some generic y and x
		//knockback speed values to make it look a little better.  Not doing it now cause it's bloody
		//late and I'm incredibly tired.
		if(airBorne == 1) //air blocking takes priority over others.
		{
			if(currentAnimationState == 44 || currentAnimationState == 45)
            {
				setCurrentAnimation(45);
                g_cObjectManager.resetCounter(m_nPlayerNumber);
            }
			else
            {
				setCurrentAnimation(44);
                g_cObjectManager.resetCounter(m_nPlayerNumber);
            }
			freezeTimer = g_cTimer.time() + hitLagDuration;
			freeze = 1;
			freezeGap = g_cTimer.time() - m_nLastFrameTime;
			m_nStunTimer = freezeTimer;
			m_fYspeed = -10;
		}
		else if(downHeld == 1 || (m_nPlayerNumber == 1 && g_pInputManager->X1DOWN == 1) || (m_nPlayerNumber == 2 && g_pInputManager->X2DOWN == 1))
		{
			//This should be the beginning of the blockstun animation
				g_pInputManager->playSound(BLOCKHIT_SOUND);
				m_fXspeed = 0;
		if(currentAnimationState == 42 || currentAnimationState == 39)
        {
			setCurrentAnimation(42);
            g_cObjectManager.resetCounter(m_nPlayerNumber);
        }
		else if(currentAnimationState != 41)
        {
			setCurrentAnimation(41);
            g_cObjectManager.resetCounter(m_nPlayerNumber);
        }
			hitStunTime = blockStunDuration;
			freezeTimer = g_cTimer.time() + hitLagDuration;
			freeze = 1;
			freezeGap = g_cTimer.time() - m_nLastFrameTime;
			m_nStunTimer = freezeTimer;	//When blockstun began.
		}
		else
		{
			//This should be the beginning of the blockstun animation
				g_pInputManager->playSound(BLOCKHIT_SOUND);
				m_fXspeed = 0;
		if(currentAnimationState == 39 || currentAnimationState == 42)	//if you're already in full blockstun...
        {
			setCurrentAnimation(39);	//Skip the transition
            g_cObjectManager.resetCounter(m_nPlayerNumber);
        }
		else if(currentAnimationState != 38)
        {
			setCurrentAnimation(38);
            g_cObjectManager.resetCounter(m_nPlayerNumber);
        }
			hitStunTime = blockStunDuration;
			freezeTimer = g_cTimer.time() + hitLagDuration;
			freeze = 1;
			freezeGap = g_cTimer.time() - m_nLastFrameTime;
			//add pushback code here. (will do later.  Again.  Tired.
			m_nStunTimer = freezeTimer;	//When blockstun began.
		}
	}
}

//Returns whether or not the character is in hitstun.
int CRussian::inStun()
{
	//list all hitstun animations here.
	if(currentAnimationState == 16 || currentAnimationState == 17 || currentAnimationState == 18 || currentAnimationState == 25
    || currentAnimationState == 26 || currentAnimationState == 27 || currentAnimationState == 28 || currentAnimationState == 30
    || currentAnimationState == 31 || currentAnimationState == 32)
		return TRUE;
	else
		return FALSE;
}

void CRussian::hitNotice()
{
	m_fPushBackSpeed = m_fCurrentAttackHitPushBackSpeed;
	m_nPushBackStartTime = g_cTimer.time();
	m_nPushBackDurationInitial = 200;
	m_nPushBackDurationCurrent = 200;
	if(currentAnimationState == 49)	//Set this to check for all launcher special states
	{
		setCurrentAnimation(50);	//set to special airborne state
		airBorne = 1;
		m_fYspeed = -35;
		m_fXspeed = 0;
	}
	if(currentAnimationState != 51 && currentAnimationState != 52 && currentAnimationState != 53)
		m_nCanJump = 1;
	if(currentAnimationState == 21 || currentAnimationState == 24 || currentAnimationState == 35)
		m_nCanJump == 0;
	
}

void CRussian::attackBlockedNotice()
{
	m_fPushBackSpeed = m_fCurrentAttackHitPushBackSpeed/2;
	m_nPushBackStartTime = g_cTimer.time();
	m_nPushBackDurationInitial = 200;
	m_nPushBackDurationCurrent = 200;
	if(currentAnimationState == 49)	//Set this to check for all launcher special states
	{
		m_nCanCancel = 0;
	}
}

void CRussian::setPushBack(float speed)
{
	m_fPushBackSpeed = -speed;
	m_nPushBackStartTime = g_cTimer.time();
	m_nPushBackDurationInitial = 200;
	m_nPushBackDurationCurrent = 200;
}

//This function is called by the input manager when you press an attack button.
//directional:	an integer representing which combination of directions your buffer shows you
//							to have input before pressing the attack button.  They are:
/*
	  236 = 1
	  623 = 2
	41236 = 3
	  214 = 4
		421 = 5
	63214 = 6
	   22 = 7
	*/
//attack:	an integer representing the attack button used.  Right now, it's set up to use
//0 as a, 1 as b, 2 as c, and it doesn't actually recognize d right now since I only had 3 attack
//animations, but eventually it should be 4.
void CRussian::interpretCommand(int directional, int attack)
{
	int curTime = g_cTimer.time();
	//Small note: all the hitstun and blockstun times here are complete outrageous because this was being
	//tested with the testChar file and it has a frame interval that is really REALLY slow, so they kinda have
	//to be.  
	//ANYWAY, stun times are in milliseconds (at 60 fps, 1 frame = 16.67 ms)
	if(!g_cTimer.elapsed(aTimer,128) && !g_cTimer.elapsed(bTimer,128) && /*!g_cTimer.elapsed(cTimer,128) && */superCurrent >= 50)
	{
		if(currentAnimationState == 19 || currentAnimationState == 20 || currentAnimationState == 21
			|| currentAnimationState == 22 || currentAnimationState == 23 || currentAnimationState == 24
			|| currentAnimationState == 33 || currentAnimationState == 34 || currentAnimationState == 35
			|| currentAnimationState == 47 || currentAnimationState == 48 || currentAnimationState == 49
			|| currentAnimationState == 51 || currentAnimationState == 52 || currentAnimationState == 53
			|| currentAnimationState == 36 || currentAnimationState == 37)
		{
			if(airBorne == 1)
			{
				setCurrentAnimation(10);
				superCurrent-=50;
				if(m_nPlayerNumber == 1)
					p1SuperCurrent -= 50;
				else if(m_nPlayerNumber == 2)
					p2SuperCurrent -= 50;
			}
			else
			{
				superCurrent-=50;
				setCurrentAnimation(0);
				if(m_nPlayerNumber == 1)
					p1SuperCurrent -= 50;
				else if(m_nPlayerNumber == 2)
					p2SuperCurrent -= 50;
			}
		}
		return;
	}
	if(m_nCanCancel == 1 || currentAnimationState == 50)
	{
		if(airBorne == 1)	//if in the air
		{
			if(currentAnimationState == 50 || currentAnimationState == 49)	//if in special airborne state after launcher
			{
				if(attack == 0)
				{
					setCurrentAnimation(51);	//jumping light followup for launcher
					g_pInputManager->playSound(JUMPINGL_SOUND);
					m_nCurrentAttackHeight = 1;
					m_fCurrentAttackXKnockBack = 45*(facing-5);
					m_fCurrentAttackYKnockBack = -10;
					m_nCurrentAttackDmg = 5;
					m_nCurrentHitLag = 544;
					m_fCurrentAttackHitStun = 1200+m_nCurrentHitLag;
					m_fCurrentAttackBlockStun = 1000+m_nCurrentHitLag;
					m_nCanJump = 0;
					m_fCurrentAttackBlockPushBackSpeed = 0;
					m_fCurrentAttackHitPushBackSpeed = 0;
					m_nCanCancel = 0;
					m_bCurrentAttackGroundBounce = FALSE;
					m_bCurrentAttackWallBounce = TRUE;
					m_fCurrentAttackPostBounceStun = 2000;
				}
				else if(attack == 1)
				{
					setCurrentAnimation(34);	//jumping medium
					g_pInputManager->playSound(JUMPINGM_SOUND);
					m_nCurrentAttackHeight = 1;
					m_fCurrentAttackHitStun = 1200;
					m_fCurrentAttackBlockStun = 10000;
					m_fCurrentAttackXKnockBack = 30*(facing-5);
					m_fCurrentAttackYKnockBack = -2;
					m_nCurrentAttackDmg = 4;
					m_nCurrentHitLag = 200;
					m_nCanJump = 0;
					m_fCurrentAttackBlockPushBackSpeed = -2*(facing-5);
					m_fCurrentAttackHitPushBackSpeed = -2*(facing-5);
					m_nCanCancel = 0;
					m_bCurrentAttackGroundBounce = FALSE;
					m_bCurrentAttackWallBounce = FALSE;
				}
				else if(attack == 2)	//c attack
				{
					setCurrentAnimation(35);	//jumping strong followup
					g_pInputManager->playSound(JUMPINGM_SOUND);
					m_nCurrentAttackHeight = 1;
					m_fCurrentAttackHitStun = 1200;
					m_fCurrentAttackBlockStun = 10000;
					m_fCurrentAttackXKnockBack = 0;
					m_fCurrentAttackYKnockBack = 20;
					m_nCurrentAttackDmg = 4;
					m_nCurrentHitLag = 200;
					m_nCanJump = 0;
					m_fCurrentAttackBlockPushBackSpeed = -2*(facing-5);
					m_fCurrentAttackHitPushBackSpeed = -2*(facing-5);
					m_bCurrentAttackGroundBounce = FALSE;
					m_bCurrentAttackWallBounce = FALSE;
					m_nCanCancel = 0;
				}
			}
			else if(attack == 0)
			{
				setCurrentAnimation(33);	//jumping light
        g_pInputManager->playSound(JUMPINGL_SOUND);
				m_nCurrentAttackHeight = 1;
				m_nCurrentHitLag = 48;
				m_fCurrentAttackHitStun = 240+m_nCurrentHitLag;
				m_fCurrentAttackBlockStun = 240+m_nCurrentHitLag;
				m_fCurrentAttackXKnockBack = 0;
				m_fCurrentAttackYKnockBack = 0;
				m_nCurrentAttackDmg = 4;
				
				m_nCanJump = 0;
				m_fCurrentAttackBlockPushBackSpeed = 0;
				m_fCurrentAttackHitPushBackSpeed = 0;
				m_nCanCancel = 0;
				m_bCurrentAttackGroundBounce = FALSE;
				m_bCurrentAttackWallBounce = FALSE;
			}
			else if(attack == 1)
			{
				setCurrentAnimation(34);	//jumping medium
        g_pInputManager->playSound(JUMPINGM_SOUND);
				m_nCurrentAttackHeight = 1;
				m_nCurrentHitLag = 64;
				m_fCurrentAttackHitStun = 352+m_nCurrentHitLag;
				m_fCurrentAttackBlockStun = 352+m_nCurrentHitLag;
				m_fCurrentAttackXKnockBack = 0;
				m_fCurrentAttackYKnockBack = 0;
				m_nCurrentAttackDmg = 4;
				m_nCanJump = 0;
				m_fCurrentAttackBlockPushBackSpeed = 0;
			    m_fCurrentAttackHitPushBackSpeed = 0;
				m_nCanCancel = 0;
				m_bCurrentAttackGroundBounce = FALSE;
				m_bCurrentAttackWallBounce = FALSE;
			}
			else if(attack == 2)	//c attack
			{
				setCurrentAnimation(35);	//jumping strong
        g_pInputManager->playSound(JUMPINGM_SOUND);
				m_nCurrentAttackHeight = 1;
				m_nCurrentHitLag = 80;
				m_fCurrentAttackHitStun = 1200+m_nCurrentHitLag;
				m_fCurrentAttackBlockStun = 500+m_nCurrentHitLag;
				m_fCurrentAttackXKnockBack = 10*(facing-5);
				m_fCurrentAttackYKnockBack = 20;
				m_nCurrentAttackDmg = 4;
				m_nCanJump = 0;
				m_fCurrentAttackBlockPushBackSpeed = 0;
			  m_fCurrentAttackHitPushBackSpeed = 0;
				m_nCanCancel = 0;
				m_fCurrentAttackPostBounceStun = 1000;
				m_bCurrentAttackGroundBounce = TRUE;
				m_bCurrentAttackWallBounce = FALSE;
			}
		}
		else	//if on the ground
		{
			if((directional == 4 || directional == 6))	//if 214
			{	//also works for 6 since we don't have a 63214 command
				if(attack == 0)	//if a attack
				{
					setCurrentAnimation(36);	//light headbutt
          g_pInputManager->playSound(HEADBUTTL_SOUND);
					m_fXspeed = 0;
					m_nMovementCheckFrame = 21;
					m_fFrameMovement = 130;
					m_nCurrentHitLag = 200;
					m_nCurrentAttackHeight = 2;
					m_fCurrentAttackHitStun = 500+m_nCurrentHitLag;
					m_fCurrentAttackBlockStun = 480+m_nCurrentHitLag;
					m_fCurrentAttackXKnockBack = 6*(facing-5);
					m_fCurrentAttackYKnockBack = -10;
					m_nCurrentAttackDmg = 10;
					m_nCanJump = 0;
					m_fCurrentAttackBlockPushBackSpeed = -2*(facing-5);
					m_fCurrentAttackHitPushBackSpeed = -2*(facing-5);
					m_nCanCancel = 0;
					m_bCurrentAttackGroundBounce = FALSE;
					m_bCurrentAttackWallBounce = FALSE;
				}
				else if(attack == 1)	//if b attack
				{
					setCurrentAnimation(37);	//mid headbutt
					g_pInputManager->playSound(HEADBUTTM_SOUND);
					m_fXspeed = 0;
					m_nMovementCheckFrame = 35;
					m_fFrameMovement = 150;
					m_nCurrentHitLag = 200;
					m_nCurrentAttackHeight = 2;
					m_fCurrentAttackHitStun = 500+m_nCurrentHitLag;
					m_fCurrentAttackBlockStun = 464+m_nCurrentHitLag;
					m_fCurrentAttackXKnockBack = 6*(facing-5);
					m_fCurrentAttackYKnockBack = -10;
					m_nCurrentAttackDmg = 10;
                    m_nCanJump = 0;
					m_fCurrentAttackBlockPushBackSpeed = -2*(facing-5);
					m_fCurrentAttackHitPushBackSpeed = -2*(facing-5);
					m_nCanCancel = 0;
					m_bCurrentAttackGroundBounce = FALSE;
					m_bCurrentAttackWallBounce = FALSE;
				}
			}
			else if(directional == 1 || directional == 3)
			{
					setCurrentAnimation(49);
					g_pInputManager->playSound(CROUCHINGL_SOUND);
					m_fXspeed = 0;
					m_nCurrentAttackHeight = 1;
					m_nCurrentHitLag = 200;
					m_fCurrentAttackHitStun = 1000+m_nCurrentHitLag;
					m_fCurrentAttackBlockStun = 320+m_nCurrentHitLag;
					m_fCurrentAttackXKnockBack = 0;
					m_fCurrentAttackYKnockBack = -35;
					m_nCurrentAttackDmg = 4;
					m_nCanJump = 0;
					m_fCurrentAttackBlockPushBackSpeed = -2*(facing-5);
					m_fCurrentAttackHitPushBackSpeed = 0;
					m_nCanCancel = 0;
					m_bCurrentAttackGroundBounce = FALSE;
					m_bCurrentAttackWallBounce = FALSE;
			}
			else	//no command input
			{
				if(downHeld == 1 || (m_nPlayerNumber == 1 && g_pInputManager->X1DOWN == 1) || (m_nPlayerNumber == 2 && g_pInputManager->X2DOWN == 1))
				{
					if(attack == 0)	//a attack
					{
						if(currentAnimationState == 22)	//if already crouch lighting
						{
							setCurrentAnimation(48);
							g_pInputManager->playSound(CROUCHINGL_SOUND);
							m_fXspeed = 0;
							m_nCurrentAttackHeight = 0;
							m_nCurrentHitLag = 48;
							m_fCurrentAttackHitStun = 320+m_nCurrentHitLag;
							m_fCurrentAttackBlockStun = 112+m_nCurrentHitLag;
							m_fCurrentAttackXKnockBack = 0;
							m_fCurrentAttackYKnockBack = 0;
							m_nCurrentAttackDmg = 4;
							m_nCanJump = 0;
							m_fCurrentAttackBlockPushBackSpeed = -2*(facing-5);
						  m_fCurrentAttackHitPushBackSpeed = -2*(facing-5);
							m_nCanCancel = 0;
							m_bCurrentAttackGroundBounce = FALSE;
							m_bCurrentAttackWallBounce = FALSE;
						}
						else
						{
							setCurrentAnimation(22);	//crouching light
							g_pInputManager->playSound(CROUCHINGL_SOUND);
							m_fXspeed = 0;
							m_nCurrentAttackHeight = 0;
							m_nCurrentHitLag = 48;
							m_fCurrentAttackHitStun = 320+m_nCurrentHitLag;
							m_fCurrentAttackBlockStun = 112+m_nCurrentHitLag;
							m_fCurrentAttackXKnockBack = 0;
							m_fCurrentAttackYKnockBack = 0;
							m_nCurrentAttackDmg = 4;
							m_nCanJump = 0;
							m_fCurrentAttackBlockPushBackSpeed = -2*(facing-5);
						  m_fCurrentAttackHitPushBackSpeed = -2*(facing-5);
							m_nCanCancel = 0;
							m_bCurrentAttackGroundBounce = FALSE;
							m_bCurrentAttackWallBounce = FALSE;
						}
					}
					else if(attack == 1)	//b attack
					{
						setCurrentAnimation(23);	//crouching medium
            g_pInputManager->playSound(CROUCHINGM_SOUND);
						m_fXspeed = 0;
						m_nCurrentAttackHeight = 1;
						m_nCurrentHitLag = 64;
						m_fCurrentAttackHitStun = 500+m_nCurrentHitLag;
						m_fCurrentAttackBlockStun = 320+m_nCurrentHitLag;
						m_fCurrentAttackXKnockBack = 0;
						m_fCurrentAttackYKnockBack = 0;
						m_nCurrentAttackDmg = 8;
			      m_nCanJump = 0;
						m_fCurrentAttackBlockPushBackSpeed = -2*(facing-5);
						m_fCurrentAttackHitPushBackSpeed = -2*(facing-5);
						m_nCanCancel = 0;
						m_bCurrentAttackGroundBounce = FALSE;
						m_bCurrentAttackWallBounce = FALSE;
					}
					else if(attack == 2)	//c attack
					{
						setCurrentAnimation(24);	//crouching heavy
            g_pInputManager->playSound(CROUCHINGH_SOUND);
						m_fXspeed = 0;
						m_nCurrentAttackHeight = 0;
						m_nCurrentHitLag = 64;
						m_fCurrentAttackHitStun = 512+m_nCurrentHitLag;
						m_fCurrentAttackBlockStun = 512+m_nCurrentHitLag;
						m_fCurrentAttackXKnockBack = 0;
						m_fCurrentAttackYKnockBack = -15;
						m_nCurrentAttackDmg = 6;
            
			      m_nCanJump = 0;
						m_fCurrentAttackBlockPushBackSpeed = -2*(facing-5);
						m_fCurrentAttackHitPushBackSpeed = -2*(facing-5);
						m_nCanCancel = 0;
						m_bCurrentAttackGroundBounce = TRUE;
						m_bCurrentAttackWallBounce = FALSE;
					}
				}
				else
				{
					if(attack == 0)	//a attack
					{
						setCurrentAnimation(19);	//Set light jab animation.
            g_pInputManager->playSound(STANDINGL_SOUND);
						m_fXspeed = 0;
						m_nCurrentAttackHeight = 1;
						m_nCurrentHitLag = 48;
						m_fCurrentAttackHitStun = 320+m_nCurrentHitLag;
						m_fCurrentAttackBlockStun = 112+m_nCurrentHitLag;
						m_fCurrentAttackXKnockBack = 0;
						m_fCurrentAttackYKnockBack = 0;
						m_nCurrentAttackDmg = 4;
            
						m_nCanJump = 0;
						m_fCurrentAttackBlockPushBackSpeed = -2*(facing-5);
						m_fCurrentAttackHitPushBackSpeed = -2*(facing-5);
						m_nCanCancel = 0;
						m_bCurrentAttackGroundBounce = FALSE;
						m_bCurrentAttackWallBounce = FALSE;
					}
					else if(attack == 1)	//b attack
					{
						if(currentAnimationState == 20)	//If already in this attack
						{//cancel into followup
							setCurrentAnimation(47);
							g_pInputManager->playSound(STANDINGM_SOUND);
							m_fXspeed = 0;
							m_nCurrentAttackHeight = 2;
							m_nCurrentHitLag = 64;
						m_fCurrentAttackHitStun = 500+m_nCurrentHitLag;
						m_fCurrentAttackBlockStun = 320+m_nCurrentHitLag;
							m_fCurrentAttackXKnockBack = 0;
							m_fCurrentAttackYKnockBack = 15;
							m_nCurrentAttackDmg = 8;
							m_nCanJump = 0;
							m_fCurrentAttackBlockPushBackSpeed = -4*(facing-5);
						  m_fCurrentAttackHitPushBackSpeed = -4*(facing-5);
							m_nCanCancel = 0;
							m_bCurrentAttackGroundBounce = FALSE;
							m_bCurrentAttackWallBounce = FALSE;
						}
						else
						{
							setCurrentAnimation(20);	//standing medium
							g_pInputManager->playSound(STANDINGM_SOUND);
							m_fXspeed = 40;
							m_nMovementCheckFrame = 1;
							m_nCurrentAttackHeight = 1;
							m_nCurrentHitLag = 64;
							m_fCurrentAttackHitStun = 500+m_nCurrentHitLag;
							m_fCurrentAttackBlockStun = 320+m_nCurrentHitLag;
							m_fCurrentAttackXKnockBack = 0;
							m_fCurrentAttackYKnockBack = 0;
							m_nCurrentAttackDmg = 8;
							m_nCanJump = 0;
							m_fCurrentAttackBlockPushBackSpeed = -4*(facing-5);
						  m_fCurrentAttackHitPushBackSpeed = -4*(facing-5);
							m_nCanCancel = 0;
							m_bCurrentAttackGroundBounce = FALSE;
							m_bCurrentAttackWallBounce = FALSE;
						}
					}
					else if(attack == 2)	//c attack
					{
						setCurrentAnimation(21);	//heavy punch
            g_pInputManager->playSound(STANDINGH_SOUND);
						m_fXspeed = 0;
						m_nCurrentAttackHeight = 0;
						m_nCurrentHitLag = 64;
						m_fCurrentAttackHitStun = 512+m_nCurrentHitLag;
						m_fCurrentAttackBlockStun = 512+m_nCurrentHitLag;
						m_fCurrentAttackXKnockBack = 0;
						m_fCurrentAttackYKnockBack = -3;
						m_nCurrentAttackDmg = 12;
            m_nCurrentHitLag = 200;
			      m_nCanJump = 0;
						m_fCurrentAttackBlockPushBackSpeed = -2*(facing-5);
						m_fCurrentAttackHitPushBackSpeed = -2*(facing-5);
						m_nCanCancel = 0;
						m_bCurrentAttackGroundBounce = FALSE;
						m_bCurrentAttackWallBounce = FALSE;
					}
				}
			}
		}
	}
}