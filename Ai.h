/// \file ai.h
/// Interface for the intelligent object class CIntelligentObject.
/// Copyright Ian Parberry, 2004
/// Last updated August 18, 2004

#pragma once

#include "object.h"

/// The intelligent object class.
/// This is the base class for our artificial intelligence. It maintains
/// the functionality that all intelligent objects have in common, that is,
/// knowledge of the player's location.

class CIntelligentObject: public CGameObject{

  protected:
    D3DXVECTOR3 m_structPlayerLocation; ///< Player location.
    float m_fDistance; ///< Euclidean distance to player.
    float m_fVerticalDistance; ///< Vertical distance to player
    float m_fHorizontalDistance; ///< horizontal distance to player
  public:

    CIntelligentObject(ObjectType object, char* name, D3DXVECTOR3 location,
      float xspeed, float yspeed); ///< Constructor.
    void player(D3DXVECTOR3 playerloc,float d); ///< Relationship to player.

};

