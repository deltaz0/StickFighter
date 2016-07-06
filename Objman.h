/// \file objman.h
/// Interface for the object manager class CObjectManager.
/// Copyright Ian Parberry, 2004
/// Last updated September 23, 2010

#pragma once

#define MAX_OBJECTS 128 //max number of objects in game

#include "object.h"

/// The object manager. The object manager is responsible for managing
/// the game objects. The player object is a special object that gets
/// special treatment because it represents the player.

class CObjectManager{

  private:
    CGameObject **m_pObjectList; ///< List of game objects.
    CGameObject *m_pPlayerObject; //< Player object - the player.
    CGameObject *m_pPlayer2Object;
    int m_nCount; ///< How many objects in list.
    int m_nMaxCount; ///< Maximum number of objects.
    int m_nLastGunFireTime; ///< Time gun was last fired.
    int m_nStartInvulnerableTime; ///< Time player started invulnerability.

    //distance functions
    float distance(CGameObject *first, CGameObject *second); ///< Distance between objects.

    //collision detection
    void CollisionDetection(); ///< Process all collisions.
    BOOL CollisionDetection(CGameObject* object); ///< Process collisions with one object.
    BOXSTORAGE* m_pCollisionHurtBoxStorage;
    int m_nNumBoxes;

    //managing dead objects
    void cull(); ///< Cull dead objects
    void kill(int index); ///< Remove dead object from list.
    void replace(int index); ///< Replace object by next in series.
    void replaceplayer(); ///< Replace player object by next in series.

  public:
    CObjectManager(); ///< Constructor
    ~CObjectManager(); ///< Destructor
    void create(int charID, int playerNum, ObjectType object, char* name, D3DXVECTOR3 location,
      float xspeed, float yspeed); ///< Create new object(include character check)
    void create(ObjectType object, char* name, D3DXVECTOR3 location,
      float xspeed, float yspeed); ///< Create new object(don't include character check)
    void move(); ///< Move all objects
    void draw(); ///< Draw all objects
    void GetPlayerLocation(float &x, float &y); ///< Get location of player.
	void GetPlayerLocations(float &x1, float &y1, float &x2, float &y2);//<Returns x and y locations of both players.
    float GetPlayerLocationY(int player); ///< Returns y location of player.
    float GetXAvg();//returns the x midpoint between players
    void Accelerate(int xdelta,int ydelta); ///< Change speed.
    void FireGun(); ///< Fire the player's gun.
    void Reset(); ///< Reset to original conditions.
    BOOL PlayerWon(); ///< TRUE if all enemies dead.
    void SetSpeed(int player, int xdelta, int ydelta); ///< Set speed of player.
    void SetSpeedX(int player, int xdelta); ///< Set X speed of player.
    void SetSpeedY(int player, int ydelta); ///< Set Y speed of player.
    void GetSpeedLimits(int &xmin, int &xmax,
      int &ymin, int &ymax); ///< Get speed limits of player.
    int enemies(); ///< Get number of enemies left.
    void dash(int player, int direction, int speedX); //sets values for airdashing characters.
    void walk(int player, int direction);
		void crouch(int player);
		void stopCrouching(int player);
    void airJump(int player, int speedY);
    void setAirBorne(int player); //Sets the given player's airborne flag to 1
    int getRemainingJumps(int player);  //Get remaining aerial jumps for player.
    int getRemainingAirDashes(int player);  //Get remaining air dashes for player.
	  int getFacing(int player);  //Get facing of player
    void changeFacing(int player);
    // for getDirHeld and dirHeld, the direction is represented as either 4, 6, 8, or 2, corresponding
    //to left, right, up, and down respectively.
    int getDirHeld(int player, int direction); //Returns whether player is holding given direction.
    void setDirHeld(int player, int direction, int value);  //set held direction for player to value (0 or 1).
    void stop(int player);
    void jump(int player);
    void setAnimationState(int player, int state);
		void passInputs(int player, int directional, int command);
    void detectCollisions(int type, int playerOrHurtBoxSwitch, int directionOrHurtBoxIndex);
    void handleCollision(int type, int player, int direction, float p1LB, float p1RB, float p2LB, float p2RB);
	void hitChar(int hittingPlayer, int attackHeight, float hitStunDuration, float blockStunDuration, float xKnockBack, float yKnockBack, int dmg);
	void resetCounter(int player);
    int getDI(int player); //returns current held directional input for player
    int tempDI; //temporary value used by getDI() to track held inputs
    void checkFacing(int player);
	void freezeGame();
	void unFreezeGame();
	void frameAdvance(int player);
	void frameReduce(int player);

	int getCurrentAnimationFrame(int player, float xOffsetChange, float yOffsetChange, float xSizeChange, float ySizeChange);
};
