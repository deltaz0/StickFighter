#pragma once

#include "Object.h"
#include "sprite.h"
#include "defines.h"
#include "tinyxml.h"
#include "Input.h"


class CTemplate: public CGameObject{
	protected:
		int** m_pAnimationArrays;
  public:
    CTemplate(ObjectType object, int playerNum, char* name, D3DXVECTOR3 location,
      float xspeed, float yspeed); ///< Constructor.
    ~CTemplate(); //< Destructor.
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
		void getHit(float hitStunDuration, float xKnockBack, float yKnockBack);
		void block(float blockStunDuration);
		void move();
		int inStun();
		void interpretCommand(int directional, int attack);
};