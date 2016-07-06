/// \file SpriteMan.h
/// Interface for the sprite manager class CSpriteManager.
/// Copyright Ian Parberry, 2004
/// Last updated September 3, 2010

#pragma once

#include "defines.h"
#include "sprite.h"
#include "tinyxml.h"

/// The sprite manager. The sprite manager is responsible for managing
/// sprites.

class CSpriteManager{

private:
  C3DSprite* m_pSprite[NUM_SPRITES]; ///< Sprite pointers.
  char m_pBuffer[MAX_PATH]; ///< File name buffer.
  BOOL Load(ObjectType object,
    const char* file, const char* ext, int frames); ///< Load sprite.

public:
  CSpriteManager(); ///< Constructor
  ~CSpriteManager(); ///< Destructor
  void Load(ObjectType object, char* name); ///< Load sprite.
  void Draw(ObjectType object, D3DXVECTOR3 p, float angle, float scale, int frame); ///< Draw sprite.
  void Draw(ObjectType object, D3DXVECTOR3 p, float angle, float scale, int frame, int facing); ///< Draw sprite.
  void DrawDetectionBox(ObjectType object, D3DXVECTOR3 p, float scaleX, float scaleY, float xOffset, float yOffset);  //Draw detection box (hit/hurt/collision)
  int FrameCount(ObjectType object); ///< Number of frames.
  int Height(ObjectType object); ///< Frame height.
  int Width(ObjectType object); ///< Frame width.
  void Release();  ///< Release textures.


};
