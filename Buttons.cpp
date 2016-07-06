/// \file buttons.cpp
/// Code for the button manager class CButtonManager.
/// Copyright Ian Parberry, 2004.
/// Last updated September 7, 2010.

#include "buttons.h"
#include "sound.h" //for sound manager
#include "spriteman.h" //sprite manager
#include "defines.h" //essential defines

extern CSoundManager* g_pSoundManager; //sound manager
extern CSpriteManager g_cSpriteManager; //sprite manager
extern int g_nScreenWidth; 
extern int g_nScreenHeight; 

/// Constructor.
/// \param count Number of buttons
/// \param size Size of each button
/// \param point Location of first button on the screen
/// \param ydelta Distance between buttons in the y direction

CButtonManager::CButtonManager(int count, SIZE size, POINT point, int ydelta){

  m_nMaxCount = count; //number of buttons
  m_sizeButton = size; //size of buttons
  m_nButtonDownSound = m_nButtonUpSound = -1; //no sounds yet
  m_bRadio = FALSE; //not radio buttons

  m_rectHotspot = new RECT[m_nMaxCount]; //hotspot space
  m_bDown = new BOOL [m_nMaxCount]; //whether button is down

  for(int i=0; i<m_nMaxCount; i++)
    m_bDown[i] = FALSE; //all buttons up

  //add the buttons
  m_nCount = 0; addbutton(point);
  for(int i=0; i<count-1; i++){
    point.y += ydelta; addbutton(point);
  }
}

CButtonManager::~CButtonManager(){ //destructor
  delete[]m_rectHotspot;
  delete[]m_bDown;
}

/// Add a button in a rectangle.
/// \param rect Rectangle for button hotspot

BOOL CButtonManager::addbutton(RECT rect){ 
  if(m_nCount >= m_nMaxCount)return FALSE; //bail if no space
  m_rectHotspot[m_nCount++] = rect; //insert hotspot
  return TRUE;
}

/// Add a button at a point. 
/// \param point Point for button hotspot's top left corner

BOOL CButtonManager::addbutton(POINT point){
  RECT rect; //bounding rectangle
  rect.left = point.x; rect.right = point.x + m_sizeButton.cx; //horizontal
  rect.top = point.y; rect.bottom = point.y + m_sizeButton.cy; //vertical
  return addbutton(rect); //add hotspot in this rectangle
}

/// Detect whether a point hits a hotspot, and return the index if it does.
/// This function is designed to be called with the mouse position, in order
/// to detect whether the player has clicked inside a button.
/// \param point Point to check for hit
/// \return Index of button that point is in, -1 for no hit

int CButtonManager::hit(POINT point){
  int result=-1; //start by assuming no hit
  int i=0; //index
  while(i<m_nCount && result<0) //for each button
    if(PtInRect(&(m_rectHotspot[i]), point)) //check for hit
      result = i; //record hit
    else ++i; //next button
  return result;
}

/// Detect whether a point hits a hotspot for a button
/// that is currently down, and return the index if it does.
/// This function is designed to be called with the mouse position, in order
/// to detect whether the player has clicked inside a button.
/// \param point Point to check for hit
/// \return Index of button that point is in, -1 for no hit

int CButtonManager::release(POINT point){ 
  int result = -1; //start by assuming no hit
  int i = 0; //index
  while(i<m_nCount && result<0) //for each button
    if(m_bDown[i] && PtInRect(&(m_rectHotspot[i]), point))
      result = i; //a hit
    else ++i; //next button
  return result;
}

/// Check whether a point is within a button that is currently down, 
/// and handle it if it is.
/// \param point Point to check
/// \return TRUE if button was up 

BOOL CButtonManager::buttondown(POINT point){
  return buttondown(hit(point));
}

/// Make a specific button down.
/// Play a sound if it was up.
/// \param index Index of button to put down
/// \param sound TRUE to play the sound of button going down
/// \return TRUE if button was not down 

BOOL CButtonManager::buttondown(int index, BOOL sound){
  if(index>=0 && index<m_nCount && !m_bDown[index]){
    if(sound)g_pSoundManager->play(m_nButtonDownSound);    
    if(m_bRadio && index < m_nCount-1) //if radio button (and not last one)
      for(int i=0; i < m_nCount-1; i++)
        m_bDown[i] = FALSE; //pop up other radio buttons
    m_bDown[index] = TRUE; //button is now down
    return TRUE;
  }
  else return FALSE;
}

/// Check whether a point is within a button that is currently up, 
/// and handle it if it is.
/// \param point Point to check
/// \return TRUE if button was down

BOOL CButtonManager::buttonup(POINT point){
  return buttonup(release(point));
}

/// Make a specific button up.
/// Play a sound if it was down.
/// \param index Index of button to put up
/// \return TRUE if button was not up 

BOOL CButtonManager::buttonup(int index){
   if( (!m_bRadio || index == m_nCount-1) &&
      index >= 0 && index < m_nCount && m_bDown[index]){ //if valid
    g_pSoundManager->play(m_nButtonUpSound);
    m_bDown[index] = FALSE; //button is up
    return TRUE;
  }
  else return FALSE;
}

/// Make all buttons up. 
/// Play a sound if any of them were down.

void CButtonManager::allbuttonsup(){
  if(m_bRadio){ //don't pop up radio buttons
    if(m_bDown[m_nCount-1]){ //pop up quit button if down
      m_bDown[m_nCount-1] = FALSE;
      g_pSoundManager->play(m_nButtonUpSound);
    }
  }
  else{ //not radio buttons, so pop up all of them
    BOOL found=FALSE; //to make sure sound played only once
    for(int i=0; i<m_nCount; i++) //for each button
      if(m_bDown[i]){ //if valid down button
        found = TRUE; //will play sound later
        m_bDown[i] = FALSE; //button is up
      }
    if(found)g_pSoundManager->play(m_nButtonUpSound);
  }
}

/// Draw the buttons.
/// The other button animation functions merely recorded button state,
/// this function draws the buttons according to their current state.

void CButtonManager::DrawButtons(){ //draw buttons 
  D3DXVECTOR3 p; //location of button lower center
  p.z = -300.0f;
  for(int i=0; i<m_nCount; i++){ //for each button
    //find offset p for sprite drawing
    p.x = (float)(-m_rectHotspot[i].left/2 + m_sizeButton.cx/2);
    p.y = (float)(2*g_nScreenHeight - 2 * m_rectHotspot[i].bottom );
    //draw it up or down as the case may be
    //if(m_bDown[m_nCount-i-1])
    if(m_bDown[i])
      g_cSpriteManager.Draw(BUTTON_OBJECT, p, 0.0, 4.0f, 1); //draw down   
    else
      g_cSpriteManager.Draw(BUTTON_OBJECT, p, 0.0, 4.0f, 0);//draw up  
  }
}

/// Record the sounds indices for sounds to be played
/// when buttons go down and up.
/// \param down Index of button down sound
/// \param up Index of button up sound

void CButtonManager::setsounds(int down, int up){ //set sounds
  m_nButtonDownSound = down; m_nButtonUpSound = up;
}

/// Set the radio button property.
/// \param on Value to set the radio button property to: TRUE for on, FALSE for off

void CButtonManager::SetRadio(BOOL on){ //set to radio buttons
  m_bRadio = on;
}


