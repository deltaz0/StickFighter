/// \file HeaderManager.cpp
/// Code for header manager class CHeaderManager.
/// Copyright Ian Parberry, 2004.
/// Last updated September 23, 2010.

#include <windows.h>
#include <stdio.h>

#include "HeaderManager.h" //header text manager
#include "timer.h" //timer
#include "objman.h" //object manager
#include "text.h" //text manager
#include "score.h" //score manager
#include "gamerenderer.h" //game renderer
//test hitbox
extern DETECTIONBOX testBox;
extern int curFrameNum;

extern int p1Health;
extern int p2Health;
extern int p1ComboCounter;
extern int p2ComboCounter;
extern CTimer g_cTimer; //game timer
extern CObjectManager g_cObjectManager; //object manager
extern CTextManager g_cTextManager; //text manager
extern CScoreManager g_cScoreManager; //score manager
extern IDirect3DSurface9* g_d3dbackbuffer; //back buffer for rendering
extern int g_nCurrentLevel; //current level
extern CGameRenderer GameRenderer;
extern BOOL g_bShowFrameRate;
extern int p1RoundsWon;
extern int p2RoundsWon;
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern CInputManager* g_pInputManager; 

CHeaderManager::CHeaderManager(){
  m_nLastFrameCountTime = 0; //timer for updating frame rate display
  m_nFrameCount = 0; //number of frames in this interval (so far)
  m_nLastFrameCount = 0; //number of frames in last full interval
}

void CHeaderManager::DrawValue(char* title, int value, int x, int y){ 
  const int bufsize=256;
  char buffer[bufsize];
  
  GameRenderer.TextMoveTo(x, y);
  sprintf_s(buffer, bufsize, "%s %d", title, value);
  GameRenderer.TextWrite(buffer);
}

/// Draw the text header with characters, lives etc. to the the back buffer.
/// Positions of the text strings on the screen are hard-coded.

void CHeaderManager::DrawTextHeader(){ //draw header with score, lives, etc.
  
 // GameRenderer.GetTextDC();

 // char getp1Key[6] = {'P', '1', ':', ' ', g_cObjectManager.getDI(1) + '0', NULL}; //get player 1's real-time directional input
 // char getp2Key[6] = {'P', '2', ':', ' ', g_cObjectManager.getDI(2) + '0', NULL}; //get player 2's real-time directional input
	//char temp1[5] = {'h', 'i', 't', 's', NULL};
	//char p1CC[8];
	////char p1CC[8];
	//itoa(p1ComboCounter, p1CC, 10);
	////strcat_s(p1CC, temp1);
	//
 //   DrawValue("p1Health", p1Health, 60, 10);
 //   DrawValue("p2Health", p2Health, 60, 30);
 //   DrawValue("second", GameRenderer.second, 300, 10);
 //   DrawValue("p1Rounds", p1RoundsWon, 500, 10);
 //   DrawValue("p2Rounds", p2RoundsWon, 500, 30);

	//int tempX = testBox.xSize*100;
	//int tempY = testBox.ySize*100;
 // DrawValue("Hits", 0, 10, 10);
	//DrawValue(p1CC, 0, 10, 30);
 // DrawValue("Input 1", 0, 200, 10);
 // DrawValue(getp1Key, 0, 200, 30); //display player 1's  1-9 DI
 // DrawValue("Input 2", 0, 400, 10);
 // DrawValue(getp2Key, 0, 400, 30); //diaplsy player 2's 1-9 DI
	//DrawValue("Frame: ", curFrameNum, 800, 10);
 // DrawValue("xLoc", testBox.xOffset, 600, 10);
 // DrawValue("yLoc", testBox.yOffset, 600, 30);
 // DrawValue("xSize", tempX, 700, 10);
 // DrawValue("ySize", tempY, 700, 30);

 // //count frames
 // m_nFrameCount++; //increment frame count
 // if(g_cTimer.elapsed(m_nLastFrameCountTime, 1000)){
 //   m_nLastFrameCount = m_nFrameCount; 
 //   m_nFrameCount = 0;
 // }

 // /*if(g_bShowFrameRate)*/DrawValue("", m_nLastFrameCount, 1000, 10);

 // GameRenderer.ReleaseTextDC();
} //DrawTextHeader



void CHeaderManager::DrawTextDMenu(){
  
    GameRenderer.GetTextDC();

    int x1 = g_nScreenWidth*0.23f;
    int x2 = g_nScreenWidth*0.63f;

    int x12 = g_nScreenWidth*0.35f;
    int x22 = g_nScreenWidth*0.75f;

    int y1 = 0.14f*g_nScreenHeight;
    float y2 = g_nScreenHeight*0.101f;

    


    DrawDMenuChar("P1 Down", x1, y1);
    DrawDMenuChar("P1 Left", x1, y1+y2);
    DrawDMenuChar("P1 Right", x1, y1+(2*y2));
    DrawDMenuChar("P1 Up", x1, y1+(3*y2));
    DrawDMenuChar("P1 L Atk", x1, y1+(4*y2));
    DrawDMenuChar("P1 M Atk", x1, y1+(5*y2));
    DrawDMenuChar("P1 H Atk", x1, y1+(6*y2));

    DrawDMenuChar(ScanToChar(g_pInputManager->p1D), x12, y1);
    DrawDMenuChar(ScanToChar(g_pInputManager->p1L), x12, y1+y2);
    DrawDMenuChar(ScanToChar(g_pInputManager->p1R), x12, y1+(2*y2));
    DrawDMenuChar(ScanToChar(g_pInputManager->p1U), x12, y1+(3*y2));
    DrawDMenuChar(ScanToChar(g_pInputManager->p1A), x12, y1+(4*y2));
    DrawDMenuChar(ScanToChar(g_pInputManager->p1B), x12, y1+(5*y2));
    DrawDMenuChar(ScanToChar(g_pInputManager->p1C), x12, y1+(6*y2));
	
    DrawDMenuChar("P2 Down", x2, y1);
    DrawDMenuChar("P2 Left", x2, y1+y2);
    DrawDMenuChar("P2 Right", x2, y1+(2*y2));
    DrawDMenuChar("P2 Up", x2, y1+(3*y2));
    DrawDMenuChar("P2 L Atk", x2, y1+(4*y2));
    DrawDMenuChar("P2 M Atk", x2, y1+(5*y2));
    DrawDMenuChar("P2 H Atk", x2, y1+(6*y2));
	
    DrawDMenuChar(ScanToChar(g_pInputManager->p2D), x22, y1);
    DrawDMenuChar(ScanToChar(g_pInputManager->p2L), x22, y1+y2);
    DrawDMenuChar(ScanToChar(g_pInputManager->p2R), x22, y1+(2*y2));
    DrawDMenuChar(ScanToChar(g_pInputManager->p2U), x22, y1+(3*y2));
    DrawDMenuChar(ScanToChar(g_pInputManager->p2A), x22, y1+(4*y2));
    DrawDMenuChar(ScanToChar(g_pInputManager->p2B), x22, y1+(5*y2));
    DrawDMenuChar(ScanToChar(g_pInputManager->p2C), x22, y1+(6*y2));


    DrawValue("Exit to Menu", 0, g_nScreenWidth*0.43f, y1+(7*y2));



    GameRenderer.ReleaseTextDC();

}

void CHeaderManager::DrawDMenuChar(char* title, int x, int y){ 
  const int bufsize=256;
  char buffer[bufsize];
  
  GameRenderer.TextMoveTo(x, y);
  sprintf_s(buffer, bufsize, "%s", title, 0);
  GameRenderer.TextWrite(buffer);
}


void CHeaderManager::DrawDMenuDword(DWORD title, int x, int y){ 
  const int bufsize=256;
  char buffer[bufsize];

  
  GameRenderer.TextMoveTo(x, y);
  sprintf_s(buffer, bufsize, "%s %d", "", title);
  GameRenderer.TextWrite(buffer);
}


char* CHeaderManager::ScanToChar(DWORD scanCode) 
{
	char buffer[32];

	//int keyCode = (int)scanCode;

	return DKKeyMapper(scanCode, buffer);
	
	
}


/*
char CHeaderManager::ScanToChar(DWORD scanCode) 
{

	static HKL layout = GetKeyboardLayout(0);
	static UCHAR keyboardState[256];
	
	UINT vk = MapVirtualKeyEx (scanCode, 1, layout);
	USHORT asciiValue;
	ToAscii(vk, scanCode, keyboardState, &asciiValue, 0);

	return static_cast<char>(asciiValue);
}
*/

char* CHeaderManager::DKKeyMapper(DWORD key, char* buffer){

  switch(key){
    case 0x01: buffer = "ESCAPE"; break;
    case 0x02: buffer = "1"; break;
    case 0x03: buffer = "2"; break;
    case 0x04: buffer = "3"; break;
    case 0x05: buffer = "4"; break;
    case 0x06: buffer = "5"; break;
    case 0x07: buffer = "6"; break;
    case 0x08: buffer = "7"; break;
    case 0x09: buffer = "8"; break;
    case 0x0A: buffer = "9"; break;
    case 0x0B: buffer = "0"; break;
    case 0x0C: buffer = "MINUS"; break;
    case 0x0D: buffer = "EQUALS"; break;
    case 0x0E: buffer = "BACK"; break;
    case 0x0F: buffer = "TAB"; break;
    case 0x10: buffer = "Q"; break;
    case 0x11: buffer = "W"; break;
    case 0x12: buffer = "E"; break;
    case 0x13: buffer = "R"; break;
    case 0x14: buffer = "T"; break;
    case 0x15: buffer = "Y"; break;
    case 0x16: buffer = "U"; break;
    case 0x17: buffer = "I"; break;
    case 0x18: buffer = "O"; break;
    case 0x19: buffer = "P"; break;
    case 0x1A: buffer = "LBRACKET"; break;
    case 0x1B: buffer = "RBRACKET"; break;
    case 0x1C: buffer = "RETURN"; break;
    case 0x1D: buffer = "LCONTROL"; break;
    case 0x1E: buffer = "A"; break;
    case 0x1F: buffer = "S"; break;
    case 0x20: buffer = "D"; break;
    case 0x21: buffer = "F"; break;
    case 0x22: buffer = "G"; break;
    case 0x23: buffer = "H"; break;
    case 0x24: buffer = "J"; break;
    case 0x25: buffer = "K"; break;
    case 0x26: buffer = "L"; break;
    case 0x27: buffer = "SEMICOLON"; break;
    case 0x28: buffer = "APOSTROPHE"; break;
    case 0x29: buffer = "GRAVE"; break;
    case 0x2A: buffer = "LSHIFT"; break;
    case 0x2B: buffer = "BACKSLASH"; break;
    case 0x2C: buffer = "Z"; break;
    case 0x2D: buffer = "X"; break;
    case 0x2E: buffer = "C"; break;
    case 0x2F: buffer = "V"; break;
    case 0x30: buffer = "B"; break;
    case 0x31: buffer = "N"; break;
    case 0x32: buffer = "M"; break;
    case 0x33: buffer = "COMMA"; break;
    case 0x34: buffer = "PERIOD"; break;
    case 0x35: buffer = "SLASH"; break;
    case 0x36: buffer = "RSHIFT"; break;
    case 0x37: buffer = "MULTIPLY"; break;
    case 0x38: buffer = "LMENU"; break;
    case 0x39: buffer = "SPACE"; break;
    case 0x3A: buffer = "CAPITAL"; break;
    case 0x3B: buffer = "F1"; break;
    case 0x3C: buffer = "F2"; break;
    case 0x3D: buffer = "F3"; break;
    case 0x3E: buffer = "F4"; break;
    case 0x3F: buffer = "F5"; break;
    case 0x40: buffer = "F6"; break;
    case 0x41: buffer = "F7"; break;
    case 0x42: buffer = "F8"; break;
    case 0x43: buffer = "F9"; break;
    case 0x44: buffer = "F10"; break;
    case 0x45: buffer = "NUMLOCK"; break;
    case 0x46: buffer = "SCROLL"; break;
    case 0x47: buffer = "NUMPAD7"; break;
    case 0x48: buffer = "NUMPAD8"; break;
    case 0x49: buffer = "NUMPAD9"; break;
    case 0x4A: buffer = "SUBTRACT"; break;
    case 0x4B: buffer = "NUMPAD4"; break;
    case 0x4C: buffer = "NUMPAD5"; break;
    case 0x4D: buffer = "NUMPAD6"; break;
    case 0x4E: buffer = "ADD"; break;
    case 0x4F: buffer = "NUMPAD1"; break;
    case 0x50: buffer = "NUMPAD2"; break;
    case 0x51: buffer = "NUMPAD3"; break;
    case 0x52: buffer = "NUMPAD0"; break;
    case 0x53: buffer = "DECIMAL"; break;
    case 0x56: buffer = "OEM_102"; break;
    case 0x57: buffer = "F11"; break;
    case 0x58: buffer = "F12"; break;
    case 0x64: buffer = "F13"; break;
    case 0x65: buffer = "F14"; break;
    case 0x66: buffer = "F15"; break;
    case 0x70: buffer = "KANA"; break;
    case 0x73: buffer = "ABNT_C1"; break;
    case 0x79: buffer = "CONVERT"; break;
    case 0x7B: buffer = "NOCONVERT"; break;
    case 0x7D: buffer = "YEN"; break;
    case 0x7E: buffer = "ABNT_C2"; break;
    case 0x8D: buffer = "NUMPADEQUALS"; break;
    case 0x90: buffer = "PREVTRACK"; break;
    case 0x91: buffer = "AT"; break;
    case 0x92: buffer = "COLON"; break;
    case 0x93: buffer = "UNDERLINE"; break;
    case 0x94: buffer = "KANJI"; break;
    case 0x95: buffer = "STOP"; break;
    case 0x96: buffer = "AX"; break;
    case 0x97: buffer = "UNLABELED"; break;
    case 0x99: buffer = "NEXTTRACK"; break;
    case 0x9C: buffer = "NUMPADENTER"; break;
    case 0x9D: buffer = "RCONTROL"; break;
    case 0xA0: buffer = "MUTE"; break;
    case 0xA1: buffer = "CALCULATOR"; break;
    case 0xA2: buffer = "PLAYPAUSE"; break;
    case 0xA4: buffer = "MEDIASTOP"; break;
    case 0xAE: buffer = "VOLUMEDOWN"; break;
    case 0xB0: buffer = "VOLUMEUP"; break;
    case 0xB2: buffer = "WEBHOME"; break;
    case 0xB3: buffer = "NUMPADCOMMA"; break;
    case 0xB5: buffer = "DIVIDE"; break;
    case 0xB7: buffer = "SYSRQ"; break;
    case 0xB8: buffer = "RMENU"; break;
    case 0xC5: buffer = "PAUSE"; break;
    case 0xC7: buffer = "HOME"; break;
    case 0xC8: buffer = "UP"; break;
    case 0xC9: buffer = "PRIOR"; break;
    case 0xCB: buffer = "LEFT"; break;
    case 0xCD: buffer = "RIGHT"; break;
    case 0xCF: buffer = "END"; break;
    case 0xD0: buffer = "DOWN"; break;
    case 0xD1: buffer = "NEXT"; break;
    case 0xD2: buffer = "INSERT"; break;
    case 0xD3: buffer = "DELETE"; break;
    case 0xDB: buffer = "LWIN"; break;
    case 0xDC: buffer = "RWIN"; break;
    case 0xDD: buffer = "APPS"; break;
    case 0xDE: buffer = "POWER"; break;
    case 0xDF: buffer = "SLEEP"; break;
    case 0xE3: buffer = "WAKE"; break;
    case 0xE5: buffer = "WEBSEARCH"; break;
    case 0xE6: buffer = "WEBFAVORITES"; break;
    case 0xE7: buffer = "WEBREFRESH"; break;
    case 0xE8: buffer = "WEBSTOP"; break;
    case 0xE9: buffer = "WEBFORWARD"; break;
    case 0xEA: buffer = "WEBBACK"; break;
    case 0xEB: buffer = "MYCOMPUTER"; break;
    case 0xEC: buffer = "MAIL"; break;
    case 0xED: buffer = "MEDIASELECT"; break;
  }
  return buffer;
}
