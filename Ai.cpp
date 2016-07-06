/// \file ai.cpp
/// Code for the intelligent object class CIntelligentObject.
/// Copyright Ian Parberry, 2004.
/// Last updated August 20, 2004.

#include "ai.h"

extern int g_nScreenWidth;

CIntelligentObject::CIntelligentObject(ObjectType object, char* name,
  D3DXVECTOR3 location, float xspeed, float yspeed):
CGameObject(object, name, location, xspeed, yspeed){ //constructor
  m_bIntelligent = TRUE;
  m_fDistance = m_fHorizontalDistance = m_fVerticalDistance = 0.0f;
  m_structPlayerLocation.x = m_structPlayerLocation.y = 0.0f;
}

/// Compute relationship with player. Intelligent objects need to know where
/// the player is so that they can intelligently avoid it. This function allows
/// an intelligent object to be notified of the player's whereabouts. It squirrels
/// away information about the player into private member variables.
/// \param playerloc player location
/// \param d distance to player

void CIntelligentObject::player(D3DXVECTOR3 playerloc, float d){ //compute relationship with player

  //remember player location
  m_structPlayerLocation = playerloc;

  //Euclidean and verticaldistances from player
  m_fDistance = d;
  m_fVerticalDistance = (float)fabs(m_structLocation.y-playerloc.y);

  //horizontal distance
  m_fHorizontalDistance = m_structLocation.x-playerloc.x;
  //wrap horizontal distance to half of world width in magnitude
  const int nWorldWidth = 2 * g_nScreenWidth; //should be at least 2* screen width
  if(m_fHorizontalDistance > nWorldWidth/2)
    m_fHorizontalDistance -= nWorldWidth;
  if(m_fHorizontalDistance < -nWorldWidth/2)
    m_fHorizontalDistance += nWorldWidth;

}

