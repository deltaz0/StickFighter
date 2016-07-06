/// \file object.h
/// Interface for the game object class CGameObject.
/// Copyright Ian Parberry, 2004
/// Last updated July 29, 2010

#pragma once

#include "sprite.h"
#include "defines.h"
#include "tinyxml.h"
#include "Input.h"

/// The game object. Game objects are responsible for remembering information about 
/// themselves - including their image, location, and speed - and for moving and
/// drawing themselves.

class CGameObject{ //class for a game object

  friend class CObjectManager;

  protected:
		int freezeBuffered;
		int freezeBufferedAttack;
		int superMax;
		int superCurrent;
    int freeze;
    int freezeTimer;
    int freezeGap;
	float m_fPushBackSpeed;
	float m_fPostBounceStun;
	int m_nReboundCount;
	int m_nPushBackStartTime;
	int m_nPushBackDurationInitial;
	int m_nPushBackDurationCurrent;
	int m_nJumpSquat;
	int m_nCanJump;
  int m_nCurrentHitLag;
	int m_nLongestAnimation;
    D3DXVECTOR3 m_structLocation; ///< Current location.
    float m_fXspeed, m_fYspeed; ///< Current speed.
	float m_fCurrentAttackBlockPushBackSpeed;
	float m_fCurrentAttackHitPushBackSpeed;
	float m_fCurrentAttackHitStun;
	float m_fCurrentAttackPostBounceStun;
	float m_fCurrentAttackBlockStun;
	float m_fCurrentAttackXKnockBack;
	float m_fCurrentAttackYKnockBack;
    int m_nCurrentAttackDmg;
	int m_nStunTimer;
	int m_nCanCancel;
	int m_nCurrentAttackHeight;	//0 is low, 1 is mid, 2 is high.
	int m_nPlayerNumber;
  int m_nMovementCheckFrame;
	float m_fFrameMovement;
    int m_nLastMoveTime; ///< Last time moved.
    int m_nMinXSpeed, m_nMaxXSpeed; ///< Min, max horizontal speeds.
    int m_nMinYSpeed, m_nMaxYSpeed; ///< Min,max vertical speeds.
    int m_nCurrentFrame; ///< Frame to be displayed.
    int m_nFrameCount; ///< Number of frames in animation.
    int m_nLastFrameTime; ///< Last time the frame was changed.
    int m_nFrameInterval; ///< Interval between frames.
    int m_nAnimationCount; //Number of animations
    int* m_pAnimation; ///< Sequence of frame numbers to be repeated.
    ANIMATIONOFFSET* m_pAnimationOffsets; //offsets for various animation states
    int m_nAnimationFrameCount; ///< Number of entries in m_pAnimation.
    int m_nBounceCount; ///< Number of bounces.
    ObjectType m_nObjectType; ///< Object type.
    int m_nWidth, m_nHeight; ///< Width and height of sprite.
    int m_nBirthTime; ///< Time of creation.
    int m_nLifeTime; //< Time that object lives.
    BOOL m_bVulnerable; ///< Vulnerable to bullets.
    BOOL m_bIntelligent; ///< TRUE for an intelligent object.
    BOOL m_bCanFly; ///< TRUE for an object that can fly, all other fall.
    BOOL m_bStatic; ///< TRUE for static objects.
    BOOL m_bCycleSprite; ///< TRUE to cycle sprite frames, otherwise play once.
		BOOL m_bGroundBounce;
		BOOL m_bCurrentAttackGroundBounce;
		BOOL m_bWallBounce;
		BOOL m_bCurrentAttackWallBounce;
    virtual void LoadSettings(char* name); //< Load object-dependent settings from XML element.
    int m_nNumSecondaryHitBoxes;
    int m_nNumSecondaryHurtBoxes;
		int m_nCollisionState;
    DETECTIONBOX collisionBox;
		DETECTIONBOX* m_pCollisionBoxes;
    DETECTIONBOX* m_pHurtBoxes;
    DETECTIONBOX* m_pSecondaryHurtBoxes;
    DETECTIONBOX* m_pHitBoxes;
    DETECTIONBOX* m_pSecondaryHitBoxes;

  public:
    CGameObject(ObjectType object, int playerNum, char* name, D3DXVECTOR3 location,
      float xspeed, float yspeed); ///< Constructor.
		CGameObject(ObjectType object, char* name, D3DXVECTOR3 location,
      float xspeed, float yspeed); ///< Constructor.
    ~CGameObject(); //< Destructor.
    virtual void draw(); ///< Draw at current location.
    virtual void move(); ///< Change location depending on time and speed
    void accelerate(int xdelta,int ydelta=0); //change speed
    void SetSpeed(int xspeed, int yspeed); ///< Set speed.
    virtual void dash(int direction);
    virtual void walk(int direction);
		virtual void crouch();
		virtual void stopCrouch();
    virtual void stop();
    virtual void jump();
    virtual void expendHitBox();
    virtual void setCurrentAnimation(int newState);
		virtual void getHit(float hitStunDuration, int hitLagDuration, float xKnockBack, float yKnockBack, float postBounceStun, BOOL groundBounce, BOOL wallBounce);
		virtual void block(float blockStunDuration, int hitLagDuration);
		virtual void interpretCommand(int directional, int attack);
		virtual int inStun();
		virtual void hitNotice();
		virtual void attackBlockedNotice();
		virtual void setPushBack(float speed);
		int getCurrentAnimationImageNum();	//returns the current animation frame image number (whatever is 
																		//currently being drawn to screen, so if player10 is being
																		//drawn, this will return 10.
    int airTime;
    int airDashesLeft;
    int airJumpsLeft;
    int airBorne;
    int redashLeft;
    int redashRight;
    int facing; //Character facing.  4 is left, 6 is right.
    float fAirDashSpeed;
		float bAirDashSpeed;
		float dashSpeed;
    float walkSpeed;
    float doubleJumpSpeed;
    float jumpSpeed;
	float hitStunTime;

    int leftHeld;
    int rightHeld;
    int upHeld;
    int downHeld;

    int currentAnimationState;
	int aTimer;
	int bTimer;
	int cTimer;
};

