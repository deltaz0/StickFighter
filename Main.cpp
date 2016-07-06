/// \file main.cpp 
/// Main file for for Demo 10.
/// Copyright Ian Parberry, 2004.
/// Last updated September 27, 2010.

/// \mainpage Blank Game
/// Use this blank to start your game.

//system includes
#include <windows.h>
#include <windowsx.h>
#include <process.h>
#include <dos.h>
#include <stdio.h>

#include <iostream>

#include "defines.h"
#include "abort.h"
#include "gamerenderer.h"
#include "tinyxml.h"
#include "imagefilenamelist.h"
#include "debug.h"
#include "timer.h"
#include "sprite.h"
#include "object.h"
#include "spriteman.h"
#include "objman.h"
#include "random.h"
#include "sound.h"
#include "input.h"
#include "HeaderManager.h"
#include "Score.h"
#include "debug.h"
#include "InputBuffer.h"
#include "testChar.h"

//Hitbox test
DETECTIONBOX testBox;
int curFrameNum = 0;
//globals

HANDLE g_hThread;

int p1SuperCurrent = 0;
int p1SuperMax = 100;
int p2SuperCurrent = 0;
int p2SuperMax = 100;
int PauseGame = 0;
int p1ComboCounter = 0;
int p2ComboCounter = 0;
int p1Health = 100;
int p2Health = 100;
int p1RoundsWon = 0;
int p2RoundsWon = 0;
int tempRand = 0;
BOOL g_bActiveApp;  ///< TRUE if this is the active application
HWND g_HwndApp; ///< Application window handle.
HINSTANCE g_hInstance; ///< Application instance handle.
char g_szGameName[256]; ///< Name of this game
CImageFileNameList g_cImageFileName; ///< List of image file names.
BOOL g_bImagesLoaded = FALSE; ///< TRUE if image loading complete.

//window data for use in windowed mode
POINT g_pointWindowPosition; ///< Position of upper left corner of client area in windowed mode.
int g_nWindowWidth; ///< Actual window width in windowed mode.
int g_nWindowHeight; ///< Actual window height in windowed mode.

//graphics settings
int g_nScreenWidth; ///< Screen width.
int g_nScreenHeight; ///< Screen height.
BOOL g_bWindowed=FALSE; ///< TRUE to run in a window, FALSE for fullscreen.
BOOL g_bWireFrame = FALSE; ///< TRUE for wireframe rendering.

//abort variables
extern char g_errMsg[]; ///< Error message if we had to abort.
extern BOOL g_errorExit; ///< True if we had to abort.

//XML settings
const char* g_xmlFileName="gamesettings.xml"; ///< Settings file name.
TiXmlDocument g_xmlDocument(g_xmlFileName); ///< TinyXML document for settings.
TiXmlElement* g_xmlSettings = NULL; ///< TinyXML element for settings tag.

int hitBoxDisplay = 0;

//debug variables
#ifdef DEBUG_ON
  CDebugManager g_cDebugManager; //< The debug manager.
#endif //DEBUG_ON

//other game variables
CGameRenderer GameRenderer; ///< The game renderer.
CTimer g_cTimer; ///< The game timer.
CSpriteManager g_cSpriteManager; ///< The sprite manager.
CObjectManager g_cObjectManager; ///< The object manager.
CRandom g_cRandom; ///< The random number generator.
CSoundManager* g_pSoundManager; //< The sound manager.
CInputManager* g_pInputManager; ///< The input manager.
CHeaderManager HeaderManager; ///< The header manager for the HUD.
CScoreManager g_cScoreManager; ///< Score manager.


InputBuffer p1InputBuffer;
InputBuffer p2InputBuffer;












//game state variables
GameStateType g_nCurrentGameState; ///< Current game state.
GameStateType g_nNextGameState; ///< Next game state.
BOOL g_bEndGameState=FALSE; ///< Should we abort current state?
int g_nGameStateTime=0; ///< Time in state.
int g_nLogoDisplayTime = 4; //< Number of seconds to display logo screen.
int g_nTitleDisplayTime = 4; //< Number of seconds to display title screen.
int g_nCreditsDisplayTime = 4; //< Number of seconds to display credits screen.
int g_nCurrentLevel = 1; //current level

/// Initialize XML settings.
/// Opens an XML file and prepares to read settings from it. Settings
/// tag is loaded to XML element g_xmlSettings for later processing. Aborts if it
/// cannot load the file or cannot find settings tag in loaded file.

void InitXMLSettings(){
  
  //open and load XML file
  if(!g_xmlDocument.LoadFile())
    ABORT("Cannot load settings file %s.", g_xmlFileName);

  //get settings tag
  g_xmlSettings = g_xmlDocument.FirstChildElement("settings"); //settings tag
  if(!g_xmlSettings) //abort if tag not found
    ABORT("Cannot find <settings> tag in %s.", g_xmlFileName);
} //InitXMLSettings

void LoadGameSettings(){

  if(!g_xmlSettings)return; //bail and fail

  //get game name
  TiXmlElement* ist = g_xmlSettings->FirstChildElement("game"); 
  if(ist){
    int len = strlen(ist->Attribute("name")); //length of name string
    strncpy_s(g_szGameName, len+1, ist->Attribute("name"), len); 
  }

  //get renderer settings
  TiXmlElement* renderSettings = 
    g_xmlSettings->FirstChildElement("renderer"); //renderer tag
  if(renderSettings){ //read renderer tag attributes
    renderSettings->Attribute("width", &g_nScreenWidth);
    renderSettings->Attribute("height", &g_nScreenHeight);
    renderSettings->Attribute("windowed", &g_bWindowed);
  }

  //get intro settings
  TiXmlElement* s = 
    g_xmlSettings->FirstChildElement("intro"); //renderer tag
  if(s){ //read renderer tag attributes
    s->Attribute("logo",&g_nLogoDisplayTime);
    s->Attribute("title",&g_nTitleDisplayTime);
    s->Attribute("credits",&g_nCreditsDisplayTime);
  }

  //get image file names
  g_cImageFileName.GetImageFileNames(g_xmlSettings);

  //get debug settings
  #ifdef DEBUG_ON
    g_cDebugManager.GetDebugSettings(g_xmlSettings);
  #endif //DEBUG_ON

} //LoadGameSettings

/// Create game objects. 

void CreateObjects(){

  D3DXVECTOR3 v; //location vector
 
  v.x = (float)g_nScreenWidth/2;
  v.y = 0.0f;
  v.z = 500.0f;
  g_cObjectManager.create(NPC_OBJECT, "npc", v, -4, 0);

  //the player - save index - first paramater 0 to indicate testChar file
  //g_cObjectManager.create(0, 1, PLAYER1_OBJECT, "player",
  //  D3DXVECTOR3(.5*g_nScreenWidth, 0, 800), 0, 0);

  g_cObjectManager.create(2, 1, RUSSIAN_OBJECT, "russian",
    D3DXVECTOR3(.5*g_nScreenWidth, 0, 800), 0, 0);

  ////player two!
 
  /*g_cObjectManager.create(0, 2, PLAYER1_OBJECT, "player",
    D3DXVECTOR3 (1.5*g_nScreenWidth, 0, 800), 0, 0);*/

  g_cObjectManager.create(2, 2, RUSSIAN_OBJECT, "russian",
    D3DXVECTOR3 (1.5*g_nScreenWidth, 0, 800), 0, 0);

 
} //CreateObjects

/// Change game state.
/// Changes to a new game state and performs all the necessary
/// processing for entering that state.
/// \param newstate new state to enter

void ChangeGameState(GameStateType newstate){ //enter new state
  //change to new state
  GameStateType oldgamestate = g_nCurrentGameState; //save old game state
  g_nCurrentGameState = newstate; 
  g_nGameStateTime = g_cTimer.time();
  g_bEndGameState = FALSE; //we hopefully came here because this was TRUE
  //g_pSoundManager->stop();
  g_pInputManager->ChangeState(newstate); //input changes state too

  //start-of-state housekeeping
  switch(g_nCurrentGameState){
    case LOGO_GAMESTATE:
      g_pSoundManager->stop();
      g_pSoundManager->LoadSounds(0); //load sounds for intro sequence
      g_pSoundManager->play(LOGO_SOUND, TRUE); //signature chord
      GameRenderer.DisplayIntroScreen(LOGO_GAMESTATE);
      break;
    case TITLE_GAMESTATE:
      g_pSoundManager->stop();
      g_pSoundManager->play(TITLE_SOUND, TRUE); //signature chord
      GameRenderer.DisplayIntroScreen(TITLE_GAMESTATE);
      break;
    case CREDITS_GAMESTATE:
      g_pSoundManager->stop();
      g_pSoundManager->play(CREDITS_SOUND, TRUE); //credits sound
      GameRenderer.DisplayIntroScreen(CREDITS_GAMESTATE);
      break;
    case MENU_GAMESTATE:
      GameRenderer.PerspectiveRendering(FALSE); //orthogonal projection
      if(oldgamestate == PLAYING_GAMESTATE){
        g_pSoundManager->stop(); //silence playing phase
      }
      else if(oldgamestate == CREDITS_GAMESTATE){ //if coming in from intro
        g_pSoundManager->clear(); //clear out old sounds
        g_pSoundManager->LoadSounds(1); //load game sounds
      }          
      g_pSoundManager->play(MENUDROP_SOUND, TRUE);
      g_bWireFrame = FALSE; //just in case
      GameRenderer.m_d3ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);  
      break;     
    case PLAYING_GAMESTATE: //prepare the game engine
        p1Health = 100;
        p2Health = 100;
        p1RoundsWon = 0;
        p2RoundsWon = 0;
        GameRenderer.second = 102;
      switch(oldgamestate){//depending on previous game state

        case MENU_GAMESTATE:
          g_nCurrentLevel = 1; //start at level 1
          g_cScoreManager.reset(); //reset score
          break;

        case SUCCESS_GAMESTATE:
          //g_pSoundManager->stop(); //silence success phase
          g_nCurrentLevel++; //go to next level
          g_cScoreManager.AddToScore(LEVEL_BONUS_SCORE); //add score for level completion
          break;
      }
      tempRand = rand() %4;
      if(tempRand==0)
          g_pSoundManager->play(BREAKBEAT_SOUND, TRUE);
      else if(tempRand==1)
          g_pSoundManager->play(ELECTRO_SOUND, TRUE);
      else if(tempRand==2)
          g_pSoundManager->play(TRIBAL_SOUND, TRUE);
      else if(tempRand==3)
          g_pSoundManager->play(VIBE_SOUND, TRUE);
      GameRenderer.PerspectiveRendering(TRUE); //perspective projection
      g_cObjectManager.Reset(); //clear object manager
      CreateObjects(); //create new objects
      break;
      
    case SUCCESS_GAMESTATE: //successful completion of level
      g_pSoundManager->stop(); //stop sounds from previous phase
      GameRenderer.DisplaySuccessScreen(); //display success screen
      break;
  }
} //ChangeGameState

//windows functions: window procedure and winmain

/// Window procedure.
/// Handler for messages from the Windows API. 
///  \param hwnd window handle
///  \param message message code
///  \param wParam parameter for message 
///  \param lParam second parameter for message
///  \return 0 if message is handled

LRESULT CALLBACK WindowProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam){
  
  switch(message){ //handle message

    case WM_ACTIVATEAPP: g_bActiveApp=(BOOL)wParam; break; //iconize
      
    case WM_MOVE: //window has been moved, used for mouse in windowed mode
      g_pointWindowPosition.x = (int)LOWORD(lParam);
      g_pointWindowPosition.y = (int)HIWORD(lParam);
      break;

    case WM_SIZE: //window has been resized, used for mouse in windowed mode
      g_nWindowWidth = (int)LOWORD(lParam);
      g_nWindowHeight = (int)HIWORD(lParam);
      break;

    case WM_DESTROY: //on exit
		WaitForSingleObject(g_hThread, INFINITE);
      delete g_pSoundManager; g_pSoundManager = NULL; //kill sound manager
      delete g_pInputManager; g_pInputManager = NULL; //ditto for input manager
      GameRenderer.Release(); //release textures
      //display error message if we aborted
      if(g_errorExit)
        MessageBox(NULL,g_errMsg,"Error",MB_OK|MB_ICONSTOP);
      //finally, quit
      PostQuitMessage(0); //this is the last thing to do on exit
      break;

    default: //default window procedure
      return DefWindowProc(hwnd,message,wParam,lParam);

  } //switch(message)

  return 0L;
} //WindowProc

/// Create a default window.
/// Registers and creates a full screen window.
///  \param name the name of this application
///  \param hInstance handle to the current instance of this application
///  \param nCmdShow specifies how the window is to be shown
///  \return handle to the application window

HWND CreateDefaultWindow(char* name, HINSTANCE hInstance, int nCmdShow){

  WNDCLASS wc; //window registration info
  //fill in registration information wc
  wc.style = CS_HREDRAW|CS_VREDRAW; //style
  wc.lpfnWndProc = WindowProc; //window message handler
  wc.cbClsExtra = wc.cbWndExtra=0;
  wc.hInstance = hInstance; //instance handle
  wc.hIcon = LoadIcon(hInstance,IDI_APPLICATION); //default icon
  wc.hCursor = NULL; //no cursor
  wc.hbrBackground = NULL; //we will draw background
  wc.lpszMenuName = NULL; //no menu
  wc.lpszClassName = name; //app name provided as parameter

  //register window
  RegisterClass(&wc);

  //create and set up window 
  HWND hwnd;
  if(g_bWindowed){   
    hwnd = CreateWindowEx(WS_EX_APPWINDOW|WS_EX_DLGMODALFRAME, name, name,
      WS_POPUP|WS_CLIPCHILDREN|WS_OVERLAPPEDWINDOW, 0, 0,
      g_nScreenWidth, g_nScreenHeight,
      NULL, NULL, hInstance, NULL);
  
	  //center window on screen
	  int x = (GetSystemMetrics(SM_CXSCREEN)-g_nScreenWidth)/2;
	  int y = (GetSystemMetrics(SM_CYSCREEN)-g_nScreenHeight)/2;
	  ::SetWindowPos(hwnd, NULL, x, y ,g_nScreenWidth, g_nScreenHeight,
	    SWP_NOZORDER|SWP_SHOWWINDOW);
  }
  else 
    hwnd = CreateWindowEx(WS_EX_TOPMOST, name, name,
      WS_POPUP, 0, 0,
      GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
      NULL, NULL, hInstance, NULL);  

  if(hwnd){ //if successfully created window
    ShowWindow(hwnd, nCmdShow); UpdateWindow(hwnd); //show and update
    SetFocus(hwnd); //get input from keyboard
  }

  return hwnd; //return window handle
} //CreateDefaultWindow

/// Initialize the graphics using DirectX.
/// Draws a floor and a backdrop.

void InitGraphics(){ //initialize graphics
  if(!GameRenderer.InitD3D(g_hInstance, g_HwndApp))
      ABORT("Unable to select graphics display mode.");

  GameRenderer.InitBackgroundVertexBuffer();
  //GameRenderer.HealthVertexBuffer(480, 930);
  //GameRenderer.HealthVertexBuffer2(1120, 1570);
  
 
  
} //InitGraphics

/// LoadImages.
/// Multithread the loading process so that things don't freeze
/// up when we're loading. This isn't important now, but it will
/// be later if you have lots of images in your game.

void __cdecl LoadImages(void *pParam){
  GameRenderer.LoadTextures(); //load images
  g_bImagesLoaded=TRUE; //signal that loading is done
} //LoadImages

/// IntroChangeState.
/// Check whether we need to change state in the intro.
///  \param time Amount of time to spend in current intro state
///  \return TRUE if it is time to change state, FALSE otherwise

BOOL IntroChangeState(int time){
  return g_bEndGameState || g_cTimer.elapsed(g_nGameStateTime, time * 1500);
} //IntroChangeState

/// ProcessIntroFrame.
/// Process a frame of the intro. Check whether we should change state,
/// then do it if necessary.

void ProcessIntroFrame(){

  switch(g_nCurrentGameState){ //what state are we in?

    case LOGO_GAMESTATE: //displaying logo screen
      if(IntroChangeState(g_nLogoDisplayTime))
        ChangeGameState(TITLE_GAMESTATE); //go to title screen
      break;

    case TITLE_GAMESTATE: //displaying title screen
      if(IntroChangeState(g_nTitleDisplayTime))
        ChangeGameState(CREDITS_GAMESTATE); //go to credits
      break;

    case CREDITS_GAMESTATE: //displaying credits screen
      if(IntroChangeState(g_nCreditsDisplayTime))
        ChangeGameState(MENU_GAMESTATE); //go to menu
      break;
  }
} //ProcessIntroFrame

/// ProcessFrame.
/// Process a frame of animation.

void ProcessFrame()
{
  //reset sound manager to avoid duplicate sounds starting simultaneously (loud)
  g_pSoundManager->beginframe();

  //input handlers 
  if(g_pInputManager->ProcessKeyboardInput() || g_pInputManager->ProcessMouseInput()){ //process  input
    DestroyWindow(g_HwndApp); 
    return; //bail from ProcessFrame
  }
  g_pInputManager->ProcessJoystickInput(); //process joystick input
	if(g_pInputManager->totalcontrollers >= 1){
		g_pInputManager->ProcessXboxController(0); //process stick input
	}
	if(g_pInputManager->totalcontrollers == 2){
		g_pInputManager->ProcessXboxController(1);
	}
  //HeaderManager.DrawTextHeader();
  //playing game
  switch(g_nCurrentGameState)
	{
    case PLAYING_GAMESTATE:
      if(g_cObjectManager.PlayerWon())//completed level
        ChangeGameState(SUCCESS_GAMESTATE); 
      else
        if(g_bEndGameState)ChangeGameState(MENU_GAMESTATE); //quit, go to menu
      //if(g_bEndGameState || g_cObjectManager.PlayerWon())
        //ChangeGameState(MENU_GAMESTATE);
      else GameRenderer.ProcessFrame();
      break;
    case MENU_GAMESTATE:
      GameRenderer.ProcessMenuFrame();      
      if(GameRenderer.menuOptionSelected!=-1)
          {
            if(GameRenderer.menuOptionSelected==0)
            {
                GameRenderer.menuOptionSelected = -1;
                ChangeGameState(PLAYING_GAMESTATE);
            }
            else if(GameRenderer.menuOptionSelected==1)
            {
                GameRenderer.menuOptionSelected = -1;
                ChangeGameState(DEVICEMENU_GAMESTATE);
            }
            //case 2: (the keyboard handler will process this as if it were an ESC input)
          }
      break;
    case DEVICEMENU_GAMESTATE:
      GameRenderer.ProcessDeviceMenuFrame(); //device menu screen animation
      if(g_bEndGameState)
        ChangeGameState(MENU_GAMESTATE); //main menu
      break;

      case SUCCESS_GAMESTATE: //displaying success screen
          GameRenderer.ProcessSuccessFrame();
          if(GameRenderer.successOptionSelected!=-1)
          {
            if(GameRenderer.successOptionSelected==0)
            {
                GameRenderer.successOptionSelected = -1;
                ChangeGameState(PLAYING_GAMESTATE);
            }
            else if(GameRenderer.successOptionSelected==1)
            {
                GameRenderer.successOptionSelected = -1;
                ChangeGameState(MENU_GAMESTATE);
            }
            //case 2: (the keyboard handler will process this as if it were an ESC input)
          }
          break;

      default: ProcessIntroFrame();
  }
} //ProcessFrame
 
/// Winmain. 
/// Main entry point for this application. 
///  \param hInstance handle to the current instance of this application
///  \param hPrevInstance unused
///  \param lpCmdLine unused 
///  \param nCmdShow specifies how the window is to be shown
///  \return TRUE if application terminates correctly

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,int nCmdShow){
  MSG msg; //current message
  HWND hwnd; //handle to fullscreen window
 
  testBox.active = 0;
  testBox.boxPresent = 1;
  testBox.secondaryBox = -1;
  testBox.xOffset = 0;
  testBox.yOffset = 0;
  testBox.xSize = 1.0f;
  testBox.ySize = 1.0f;


  #ifdef DEBUG_ON
    g_cDebugManager.open(); //open debug streams, settings came from XML file
  #endif //DEBUG_ON

  g_hInstance=hInstance; //save instance handle

  g_cTimer.start(); //start game timer

  InitXMLSettings(); //initialize XML settings reader
  LoadGameSettings();

  //create fullscreen window
  hwnd = CreateDefaultWindow(g_szGameName, hInstance, nCmdShow);
  if(!hwnd)return FALSE; //bail if problem creating window
  g_HwndApp = hwnd; //save window handle

  g_pSoundManager = new CSoundManager(g_HwndApp);
  g_pSoundManager->LoadSettings(); //load general settings
  
  InitGraphics(); //initialize graphics
  g_pInputManager = new CInputManager(g_hInstance, g_HwndApp); //create input manager (after graphics)

  //create sound manager
  //_beginthread(LoadImages, 0, NULL); 

  g_hThread = (HANDLE)_beginthreadex(NULL, 0, (unsigned int (_stdcall*)(void*)) &LoadImages, NULL, 0, NULL);

  //while(!g_bImagesLoaded)
  //{
  //  //this loop causes problems with the release version
  //}

	CreateObjects(); //create game objects
  ChangeGameState(LOGO_GAMESTATE); //start intro screens

  //message loop
  while(TRUE)
    if(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)){ //if message waiting
      if(!GetMessage(&msg,NULL,0,0))return (int)msg.wParam; //get it
      TranslateMessage(&msg); DispatchMessage(&msg); //translate it
    }
    else     
      if(g_bActiveApp)ProcessFrame(); 
      else WaitMessage(); //process frame
} //WinMain
