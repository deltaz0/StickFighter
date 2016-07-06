/// \file buttons.h
/// Interface for the 2D button manager class CButtonManager.
/// Copyright Ian Parberry, 2004
/// Last updated September 6, 2010

#pragma once

#include "sprite.h"

/// The button manager class.
/// Manages a collection of buttons on a menu screen, handling
/// button animation and sound, and point-in-button collision detection.

class CButtonManager{ //button manager class
  private:
    RECT *m_rectHotspot; ///< Hotspot locations.
    BOOL *m_bDown; ///< Is button held down?
    int m_nMaxCount; ///< Number of hotspots allowed.
    int m_nCount; ///< Number of current hotspots.
    SIZE m_sizeButton; ///< Size of buttons, if all the same size.
    int m_nButtonDownSound; ///< Sound index for button down.
    int m_nButtonUpSound; ///< Sound index for button up.
    BOOL m_bRadio; ///< TRUE for radio buttons.
    BOOL addbutton(RECT rect); ///< Add button to list.
    BOOL addbutton(POINT point); ///< Add button of default size.
  public:
    CButtonManager(int count, SIZE size, POINT first, int ydelta); ///< Constructor.
    ~CButtonManager(); ///< Destructor.
    int hit(POINT point); ///< Return index of hotspot hit by point, if any.
    int release(POINT point); ///< Return hostspot hit on down button.
    BOOL buttondown(POINT point); ///< Animate button down at this point.
    BOOL buttondown(int index, BOOL sound=TRUE); ///< Animate button with this index down.
    BOOL buttonup(POINT point); ///< Animate button up at this point.
    BOOL buttonup(int index); ///< Animate button with this index up.
    void allbuttonsup(); ///< Animate all buttons up.
    void setsounds(int down, int up); ///< Set sound indices.
    void DrawButtons(); ///< Draw all buttons.
    void SetRadio(BOOL on = TRUE); ///< Set to radio buttons.
};
