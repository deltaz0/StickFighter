#pragma once

#include "Object.h"
#include "sprite.h"
#include "defines.h"
#include "tinyxml.h"
#include "Input.h"


class CRussian: public CGameObject{
	protected:
		int** m_pAnimationArrays;
  public:
    CRussian(ObjectType object, int playerNum, char* name, D3DXVECTOR3 location,
      float xspeed, float yspeed); ///< Constructor.
    ~CRussian(); //< Destructor.
    void dash(int direction);
    void walk(int direction);
		void crouch();
		void stopCrouch();
    void jump();
    void stop();
    void expendHitBox();
    void setCurrentAnimation(int newState);
    void LoadSettings(char* name);
    void draw();
		void getHit(float hitStunDuration, int hitLagDuration, float xKnockBack, float yKnockBack, float postBounceStun, BOOL groundBounce, BOOL wallBounce);
		void block(float blockStunDuration, int hitLagDuration);
		void move();
		int inStun();
		void interpretCommand(int directional, int attack);
		void hitNotice();
		void attackBlockedNotice();
		void setPushBack(float speed);
};