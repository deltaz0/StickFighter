/// \file HeaderManager.h
/// Interface for the text header class CHeaderManager.
/// Copyright Ian Parberry, 2004
/// Last updated September 23, 2010

#pragma once

/// The header manager class. 
/// The header manager allows you to put the text header
/// at the top of the game screen. It also manages the frame
/// rate display in the lower left-hand corner of the screen.

class CHeaderManager{

  private:
    int m_nLastFrameCountTime; ///< Timer for updating frame rate display.
    int m_nFrameCount; ///< Number of frames in this interval (so far).
    int m_nLastFrameCount; ///< Number of frames in last full interval.

    void DrawValue(char* title, int value, int x, int y); //Write a value with a title. 
    void DrawDMenuChar(char* title, int x, int y); //Write a value with a title. 
	void DrawDMenuDword(DWORD title, int x, int y); //Write a value with a title

  public:
	  CHeaderManager(); ///< Constructor.
    void DrawTextHeader(); ///< Draw header with score, lives, etc.
    void DrawTextDMenu();
	char* ScanToChar(DWORD scanCode);
	char* DKKeyMapper(DWORD key, char* buffer);

};