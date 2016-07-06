/// \file defines.h
/// Essential defines.
/// Copyright Ian Parberry, 2004.
/// Last updated September 7, 2010.

#pragma once

#include "d3dx9.h"

/// Billboard vertex structure. 
/// Custom vertex format for representing a vanilla billboard object. Consists 
/// of position, texture coordinates, and flexible vertex format.

struct BILLBOARDVERTEX{ 
  D3DXVECTOR3 p; ///< Position.
  float tu,tv; ///< Texture coordinates.
  static const DWORD FVF; ///< Flexible vertex format.
};

//Animation Offset structure.
//Structure for holding animation offsets.  Holds two integers representing the beginning and ending
//indices for a given animation in a characters animation array.
struct ANIMATIONOFFSET{
  int begin, end;
};

struct DETECTIONBOX{
  int boxPresent, active, secondaryBox;
  float xOffset, yOffset, xSize, ySize;
};

struct BOXSTORAGE{
  float right,left,top,bottom;
};

/// Game object types.
/// Types of game object that can appear in the game. Note: NUM_SPRITES 
/// must be last.

enum ObjectType{
  RUSSIAN_OBJECT, NPC_OBJECT, PLAYER1_OBJECT, BUTTON_OBJECT, BULLET_OBJECT, COLLISION_BOX,
  HIT_BOX, HURT_BOX, READY, FIGHT, NINE, EIGHT, SEVEN, SIX, FIVE, FOUR, THREE,
  TWO, ONE, ZERO, ROUNDONE, ROUNDTWO, ROUNDTHREE, P1WIN, P2WIN, ARROW, ROUNDNOTCHOFF, ROUNDNOTCHON, HITSPARK,
  COMBO, WNINE, WEIGHT, WSEVEN, WSIX, WFIVE, WFOUR, WTHREE, WTWO, WONE, WZERO, NUM_SPRITES
};

enum FileNameType{
  BACKGROUND_IMAGEFILE, GROUND_IMAGEFILE, WALL_IMAGEFILE, BLACK_IMAGEFILE, LOGO_IMAGEFILE, 
  TITLE_IMAGEFILE, CREDITS_IMAGEFILE, MENU_IMAGEFILE, CURSOR_IMAGEFILE, LOADING_IMAGEFILE, 
  DEVICEMENU_IMAGEFILE, HUD_IMAGEFILE, SUCCESS_IMAGEFILE, HEALTH_IMAGEFILE, HEALTH2_IMAGEFILE, MEDIUMHEALTH_IMAGEFILE, 
  MEDIUMHEALTH2_IMAGEFILE, LOWHEALTH_IMAGEFILE, LOWHEALTH2_IMAGEFILE, SUPERBAR_IMAGEFILE, SUPERBAR2_IMAGEFILE
};

/// Game state types.
/// States of the game shell.

enum GameStateType{
  LOGO_GAMESTATE, TITLE_GAMESTATE, CREDITS_GAMESTATE, 
  MENU_GAMESTATE, PLAYING_GAMESTATE, DEVICEMENU_GAMESTATE, SUCCESS_GAMESTATE
};

/// Input device types.
/// Which device the player is currently using to play the game.

enum InputDeviceType{
  KEYBOARD_INPUT, MOUSE_INPUT, JOYSTICK_INPUT, XBOX_INPUT
};

//////-------------------test defines for players
enum PlayerStateType{
  AIRBORNE, STANDING, CROUCHING
};
//------------------------end test defines for players