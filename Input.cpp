/// \file input.cpp
/// Code for the input manager class CInputManager.
/// Copyright Ian Parberry, 2004.
/// Last updated September 7, 2010.

#include <objbase.h>
#include <stdio.h>

#include <iostream>


#include "input.h"
#include "InputBuffer.h"
#include "objman.h" //for object manager
#include "sndlist.h" //for list of sounds
#include "gamerenderer.h"
#include "Sound.h" //to play sounds
//testhitboxes
extern DETECTIONBOX testBox;
//endtest


extern CSoundManager* g_pSoundManager; //< The sound manager.
extern int hitBoxDisplay;
extern GameStateType g_nCurrentGameState; //current game state
extern GameStateType g_nNextGameState; //next game state
extern BOOL g_bEndGameState; //should we abort current game state?
extern CObjectManager g_cObjectManager; //object manager
extern int g_nScreenWidth; //screen width
extern int g_nScreenHeight; //screen height
extern BOOL g_bWindowed; //windowed or fullscreen
extern CGameRenderer GameRenderer; ///< The game renderer.
extern POINT g_pointWindowPosition; //position of upper left corner of client area in windowed mode
extern int g_nWindowWidth; //actual window width in windowed mode
extern int g_nWindowHeight; //actual window height in windowed mode
extern BOOL g_bWireFrame; //draw in wireframe if TRUE
extern InputBuffer p1InputBuffer;
extern InputBuffer p2InputBuffer;
BOOL g_bShowFrameRate = FALSE; //show frame rate on HUD if TRUE

int p1DownHeld = 0;
int p1UpHeld = 0;
int p1RightHeld = 0;
int p1LeftHeld = 0;
int p2DownHeld = 0;
int p2UpHeld = 0;
int p2RightHeld = 0;
int p2LeftHeld = 0;

int upRightHeld = 0;
int upLeftHeld = 0;
int downRightHeld = 0;
int downLeftHeld = 0;
int testing = 0;


unsigned char keystate[256];

int toggletest = 0;

bool p1AtkOK = TRUE;
bool p2AtkOK = TRUE;

bool p1MoveOK = TRUE;
bool p2MoveOK = TRUE;

DWORD p1D;
DWORD p1L;
DWORD p1R;
DWORD p1U;
DWORD p1A;
DWORD p1B;
DWORD p1C;

DWORD p2D;
DWORD p2L;
DWORD p2R;
DWORD p2U;
DWORD p2A;
DWORD p2B;
DWORD p2C;

DWORD p0Start;

int getKeyFlag;


LPDIRECTINPUTDEVICE8 dinkeyboard;    // the pointer to the keyboard device



//init static member variables for use in static callback functions for joystick
LPDIRECTINPUT8 CInputManager::m_lpDirectInput = NULL; 
LPDIRECTINPUTDEVICE8 CInputManager::m_pJoystick = NULL;
BOOL CInputManager::m_bJoystickHasSlider = FALSE;

/// Constructor.
/// \param hInstance Instance handle
/// \param hwnd Window handle

CInputManager::CInputManager(HINSTANCE hInstance, HWND hwnd){ //constructor

  //button manager doesn't exist yet
  m_pButtonManager = NULL; //for managing menu buttons

  //create DirectInput object
  HRESULT hr=DirectInput8Create(hInstance, DIRECTINPUT_VERSION, 
    IID_IDirectInput8A, (void**)&m_lpDirectInput, NULL); 
  if(FAILED(hr)){
    m_lpDirectInput = NULL; //in case it fails
    return; //bail out
  }

  p1DownHeld = 0;
  p1UpHeld = 0;
  p1RightHeld = 0;
  p1LeftHeld = 0;
  p2DownHeld = 0;
  p2UpHeld = 0;
  p2RightHeld = 0;
  p2LeftHeld = 0;

  X1UP = 0;
  X1DOWN = 0;
  X1LEFT = 0;
  X1RIGHT = 0;

  X2UP = 0;
  X2DOWN = 0;
  X2LEFT = 0;
  X2RIGHT = 0;

  X1LSUP = 0;
  X1LSDOWN = 0;
  X1LSLEFT = 0;
  X1LSRIGHT = 0;
	
  X2LSUP = 0;
  X2LSDOWN = 0;
  X2LSLEFT = 0;
  X2LSRIGHT = 0;

  totalcontrollers = 0;

  X1A = 0;
  X1B = 0;
  X1C = 0;

  X2A = 0;
  X2B = 0;
  X2C = 0;

  p1D = DIK_S;
  p1L = DIK_A;
  p1R = DIK_D;
  p1U = DIK_W;
  p1A = DIK_B;
  p1B = DIK_N;
  p1C = DIK_M;

  p2D = DIK_DOWN;
  p2L = DIK_LEFT;
  p2R = DIK_RIGHT;
  p2U = DIK_UP;
  p2A = DIK_NUMPAD1;
  p2B = DIK_NUMPAD2;
  p2C = DIK_NUMPAD3;

  p0Start = DIK_0;

  int getKeyFlag = 0;


  //device initializations
  InitKeyboardInput(hwnd); //set up keyboard
  InitMouseInput(hwnd); //set up mouse
  InitJoystickInput(hwnd); //set up joystick
  InitXboxController();

  //etc.
  m_bCursorOutsideWindow = FALSE;
  m_bCursorShowing = FALSE; //no mouse cursor yet
  m_nInputDevice = KEYBOARD_INPUT; //default to keyboard input
}

CInputManager::~CInputManager(){ //destructor

  if(m_pKeyboard){ //release keyboard
    m_pKeyboard->Unacquire(); //unacquire device
    m_pKeyboard->Release(); //release it
  }

  if(m_pMouse){ //release mouse
    m_pMouse->Unacquire(); //unacquire device
    m_pMouse->Release(); //release it
  }
  
  if(m_pJoystick){ //release joystick
    m_pJoystick->Unacquire(); //unacquire device
    m_pJoystick->Release(); //release it
  }

  //now release DirectInput
  if(m_lpDirectInput)m_lpDirectInput->Release();

  //get rid of the button manager
  delete m_pButtonManager;
}

//DirectInput setup routines

/// Initialize keyboard input.
/// Create the DirectInput keyboard device, set the cooperative level,
///  set for buffered input, and acquire the keyboard.
/// \param hwnd Window handle

BOOL CInputManager::InitKeyboardInput(HWND hwnd){ //set up keyboard input

  //create keyboard device
  HRESULT hr=m_lpDirectInput->CreateDevice(GUID_SysKeyboard,&m_pKeyboard,NULL); 
  if(FAILED(hr)){
    m_pKeyboard=NULL; //in case it fails
    return FALSE; //bail out
  }

  //set keyboard data format, c_dfDIKeyboard is a "predefined global"
  //yucko ugly part of DirectInput, thanks a lot Microsoft
  hr=m_pKeyboard->SetDataFormat(&c_dfDIKeyboard); 
  if(FAILED(hr))return FALSE; //fail and bail

  //set the cooperative level 
  hr=m_pKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);  
  if(FAILED(hr))return FALSE; //fail and bail

  //set buffer size for buffered input
  DIPROPDWORD dipdw;
  dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  dipdw.diph.dwObj = 0;
  dipdw.diph.dwHow = DIPH_DEVICE;
  dipdw.dwData = DI_BUFSIZE; //buffer size
  hr = m_pKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);  
  if(FAILED(hr))return FALSE; //fail and bail

  //acquire the keyboard
  hr=m_pKeyboard->Acquire();  
  if(FAILED(hr))return FALSE; //fail and bail

  return TRUE;
}

/// Initialize mouse input.
/// Create the DirectInput mouse device, set the cooperative level,
/// set for buffered input, and acquire the mouse. Also creates the
/// mouse event to handle mouse interrupts, and sets the mouse cursor.
/// \param hwnd Window handle

BOOL CInputManager::InitMouseInput(HWND hwnd){ //set up mouse input

  //create mouse device 
  HRESULT hr=m_lpDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse,NULL);
    if(FAILED(hr)){
    m_pMouse = NULL; //in case it fails
    return FALSE; //bail out
  }

  //set mouse data format
  hr=m_pMouse->SetDataFormat(&c_dfDIMouse);
  if(FAILED(hr))return FALSE;  //fail and bail

  //set the cooperative level
  if(g_bWindowed)hr = m_pMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
  else hr = m_pMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
  if(FAILED(hr))return FALSE; //fail and bail

  //set up mouse event
  m_hMouseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  if(!m_hMouseEvent)return FALSE; //fail and bail
  hr = m_pMouse->SetEventNotification(m_hMouseEvent);
  if(FAILED(hr))return FALSE; //fail and bail

   //set buffer size
  DIPROPDWORD dipdw;
  dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
  dipdw.diph.dwObj = 0;
  dipdw.diph.dwHow = DIPH_DEVICE;
  dipdw.dwData = DI_BUFSIZE; //buffer size
  hr = m_pMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);  
  if(FAILED(hr))return FALSE; //fail and bail

  //acquire the mouse
  hr=m_pMouse->Acquire();  
  if(FAILED(hr))return FALSE; //fail and bail

  //mouse cursor
  GameRenderer.LoadMouseCursor();
  GameRenderer.SetMouseCursor();

  return TRUE;
}

///< Callback function for joystick enumeration.
///< \param pdidInstance Pointer to a DirectInput device instance.
///< \param pContext Context pointer, unused in this function but DirectInput requires it
///< \return DIENUM_STOP if we find a joystick, DIENUM_CONTINUE otherwise

BOOL CALLBACK CInputManager::EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext){

  //attempt to create joystick device.
  HRESULT hr = m_lpDirectInput->CreateDevice(pdidInstance->guidInstance, &m_pJoystick, NULL);
  
  //if it failed, then we can't use this joystick. Maybe the user unplugged it.
  if(FAILED(hr))return DIENUM_CONTINUE;

  //otherwise take the first joystick we get
  return DIENUM_STOP;
}


/// Callback function to enumerate joystick objects (eg axes, slider).
/// \param pdidoi Pointer to a DirectInput device object
/// \param pContext Context pointer, unused in this function but DirectInput requires it
///< \return DIENUM_STOP if we find a slider, DIENUM_CONTINUE otherwise

BOOL CALLBACK CInputManager::EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi,VOID* pContext){
  if(pdidoi->dwType&DIDFT_AXIS){ //if object is an axis

    //specify range
    DIPROPRANGE diprg; //range property structure
    diprg.diph.dwSize = sizeof(DIPROPRANGE); 
    diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
    diprg.diph.dwHow = DIPH_BYID; //by identifier
    diprg.diph.dwObj = pdidoi->dwType; //specify the enumerated axis
    diprg.lMin = 0; //minimum
    diprg.lMax = 100; //maximum

    //now set range
    if(FAILED(m_pJoystick->SetProperty(DIPROP_RANGE, &diprg.diph))) 
      return DIENUM_STOP; //player may have unplugged joystick
  }

  if(pdidoi->guidType == GUID_Slider)m_bJoystickHasSlider = TRUE;
  return DIENUM_CONTINUE;
}  
                                   
/// Initialize joystick input.
/// Create the DirectInput joystick device, set the cooperative level,
/// set the data format, get the slider, and acquire the joystick. 
/// \param hwnd Window handle
/// \return TRUE if it succeeds

BOOL CInputManager::InitJoystickInput(HWND hwnd){ //set up joystick input

  HRESULT hr; //return result from DirectInput calls

  //enumerate joysticks using EnumJoysticksCallback function
  hr = m_lpDirectInput->EnumDevices(DI8DEVCLASS_GAMECTRL, 
    EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);
  if(FAILED(hr))return FALSE; //fail and bail
  if(m_pJoystick==NULL)return FALSE; //make sure we got a joystick

  //set the data format to "simple joystick" - a predefined data format 
  hr = m_pJoystick->SetDataFormat(&c_dfDIJoystick2);
  if(FAILED(hr))return FALSE; //fail and bail   

  //set the cooperative level
  hr = m_pJoystick->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
  if(FAILED(hr))return FALSE; //fail and bail 

  //enumerate joystick components (eg axes, slider)
  hr = m_pJoystick->EnumObjects(EnumObjectsCallback, (VOID*)hwnd, DIDFT_ALL);
  if(FAILED(hr))return FALSE; //fail and bail 

  //acquire the joystick
  hr = m_pJoystick->Acquire();  
  if(FAILED(hr))return FALSE; //fail and bail

  return TRUE;
}

//keyboard processing

/// Process keyboard input.
/// Processes keyboard events that have been buffered since the last
/// time this function was called.
/// \return TRUE if user has hit the ESC key

//SELF NOTE:  this appears to be running every frame... IDEA!!
BOOL CInputManager::ProcessKeyboardInput(){ //process buffered keyboard events

  /*if(g_nCurrentGameState == PLAYING_GAMESTATE && leftHeld == 1)
  {
    if(m_DIod[0].dwOfs != 203)
    {
      leftHeld = 0;
       g_cObjectManager.Set//Speed(0,0);
    }
    std::cout << "TEStdsagdsa" << std::endl;
  }*/
  




    m_pKeyboard->GetDeviceState(sizeof(unsigned char[256]), (LPVOID)keystate);

    if(g_nCurrentGameState == PLAYING_GAMESTATE)
    {
      //if player 1's left flag is toggled on, but the key is no longer being held...
      if(g_cObjectManager.getDirHeld(1,4) == 1 && !(keystate[p1L] & 0x80))
      {
        g_cObjectManager.setDirHeld(1,4,0); //set left flag for p1 to 0.
        if(g_cObjectManager.getDirHeld(1,6) == 1) //if p1 right flag is 1
          g_cObjectManager.walk(1, 6); //then walk right.
        else  //not holding right, so stop movement.
          g_cObjectManager.stop(1);
      }
      //if player 1's right flag is toggled on, but the key is no longer being held...
      if(g_cObjectManager.getDirHeld(1,6) == 1 && !(keystate[p1R] & 0x80))
      {
        g_cObjectManager.setDirHeld(1,6,0);  //set right flag for p1 to 0.
        if(g_cObjectManager.getDirHeld(1,4) == 1) //if p1 left flag is 1
          g_cObjectManager.walk(1,4);  //then walk left.
        else  //not holding left, stop movement.
          g_cObjectManager.stop(1);
      }
      //if player 1's up flag is toggled on, but the key is no longer being held...
      if(g_cObjectManager.getDirHeld(1,8) == 1 && !(keystate[p1U] & 0x80))
      {
        g_cObjectManager.setDirHeld(1,8,0); //set p1 up flag to 0
        //Jump speed is handled in Object.cpp in the move function so that holding up to jump works more easily
        //g_cObjectManager.Set//SpeedY(0);
      }
      //if player 1's down flag is toggled on, but the key is no longer being held...
      if(g_cObjectManager.getDirHeld(1,2) == 1 && !(keystate[p1D] & 0x80))
      {
        g_cObjectManager.setDirHeld(1,2,0); //set p1 down flag to 0;
				g_cObjectManager.stopCrouching(1);
        //g_cObjectManager.Set//SpeedY(0);
      }

      //p2 controls
      /*
      //if player 1's left flag is toggled on, but the key is no longer being held...
      if(g_cObjectManager.getDirHeld(1,4) == 1 && !(keystate[DIK_LEFT] & 0x80))
      {
        g_cObjectManager.setDirHeld(1,4,0); //set left flag for p1 to 0.
        if(g_cObjectManager.getDirHeld(1,6) == 1) //if p1 right flag is 1
          g_cObjectManager.walk(1, 6); //then walk right.
        else  //not holding right, so stop movement.
          g_cObjectManager.stop(1);
      }
      */
      //if player 2's left flag is toggled on, but the key is no longer being held...
      if(g_cObjectManager.getDirHeld(2,4) == 1 && !(keystate[p2L] & 0x80))
      {
        g_cObjectManager.setDirHeld(2,4,0); //set left flag for p2 to 0
        if(g_cObjectManager.getDirHeld(2,6) == 1) //if p2 right flag is 1
          g_cObjectManager.walk(2,6); //then walk right.
        else  //not holding right, stop.
          g_cObjectManager.stop(2);
      }
      //if player 2's right flag is toggled on, but the key is no longer being held...
      if(g_cObjectManager.getDirHeld(2,6) == 1 && !(keystate[p2R] & 0x80))
      {
        g_cObjectManager.setDirHeld(2,6,0); //then set right flag for p2 to 0
        if(g_cObjectManager.getDirHeld(2,4) == 1) //if p2 left flag is 1
          g_cObjectManager.walk(2,4); //then walk left
        else  //not holding left, stop.
          g_cObjectManager.stop(2);
      }
      //if player 2's up flag is on, but the key is not held...
      if(g_cObjectManager.getDirHeld(2,8) == 1 && !(keystate[p2U] & 0x80))
      {
        g_cObjectManager.setDirHeld(2,8,0); //then set p2 up flag to 0
      }
      //if player 2's down flag is on, but the key is not held...
      if(g_cObjectManager.getDirHeld(2,2) == 1 && !(keystate[p2D] & 0x80))
      {
        g_cObjectManager.setDirHeld(2,2,0); //then set p2 down flag to 0
				g_cObjectManager.stopCrouching(2);
      }
      //end p2 controls

    }
    










  BOOL result=FALSE; //return result
  if(!m_pKeyboard)return FALSE; //no device so fail and bail
  DWORD dwElements=DI_BUFSIZE; //number of events to process

  //get number of events to process into member buffer m_DIod
  HRESULT hr=m_pKeyboard->GetDeviceData(
    sizeof(DIDEVICEOBJECTDATA) ,m_DIod,&dwElements, 0);

  //if it failed, we may be able to acquire the device
  if(FAILED(hr)){ //lost the device somehow
    hr = m_pKeyboard->Acquire(); //try to reacquire
    while(hr == DIERR_INPUTLOST) hr = m_pKeyboard->Acquire(); //try real hard
    return FALSE; //live to fight another day
  }
  if(FAILED(hr))return FALSE; //fail and bail paranoia
  //process inputs from m_DIod now
  for(int i=0; (i<(int)dwElements) && !result; i++){ //for each input
    if(m_DIod[i].dwData & 0x80) //if key went down ///////----------------------------key pressed down!
    {
      result = result || KeyboardHandler(m_DIod[i].dwOfs); //handle it
    }
  }

  return result;
}

//keyboard handlers for different game states

/// Main keyboard handler.
/// This function checks the current game state, then calls the
/// keyboard handler for the appropriate state.
/// \param keystroke Keyboard event to process
/// \return TRUE if the player hit ESC

BOOL CInputManager::KeyboardHandler(DWORD keystroke){ //main keyboard handler

  BOOL result = FALSE; //TRUE if we are to exit game

  switch(g_nCurrentGameState){ //select handler for game state
    case LOGO_GAMESTATE:
    case TITLE_GAMESTATE: 
    case CREDITS_GAMESTATE: IntroKeyboardHandler(keystroke); break;
    case MENU_GAMESTATE: result=MenuKeyboardHandler(keystroke); break;
    case PLAYING_GAMESTATE: GameKeyboardHandler(keystroke); break;
    case DEVICEMENU_GAMESTATE: DeviceMenuKeyboardHandler(keystroke); break;
    case SUCCESS_GAMESTATE: result=SuccessKeyboardHandler(keystroke); break;
  }

  return result;
} //KeyboardHandler

//Success keyboard handler.

BOOL CInputManager::SuccessKeyboardHandler(DWORD keystroke){
  BOOL result=FALSE;
  switch(keystroke){
    case DIK_ESCAPE:
        result = TRUE; break; //exit game
    case DIK_DOWN:
        playSound(BLEEP_SOUND);
        if(GameRenderer.successArrowPos<2)
            GameRenderer.successArrowPos++;
        else
            GameRenderer.successArrowPos = 0;
        break;
    case DIK_UP:
        playSound(BLEEP_SOUND);
        if(GameRenderer.successArrowPos>0)
            GameRenderer.successArrowPos--;
        else
            GameRenderer.successArrowPos = 2;
        break;
    case DIK_RETURN:
        if(GameRenderer.successArrowPos==2)
            result = TRUE; //exit game
        else
        {
            GameRenderer.successOptionSelected = GameRenderer.successArrowPos;
            playSound(SELECT_SOUND);
        }
        break;
    default: break; //do nothing
  }
  return result;
}


/// Intro keyboard handler.
/// Sets g_bEndGameState in response to any keystroke.

void CInputManager::IntroKeyboardHandler(DWORD keystroke){ //intro
   //any key ends the state except for an F-key
  g_bEndGameState = keystroke < DIK_F1 || keystroke > DIK_F12;
} //IntroKeyboardHandler

/// Main menu keyboard handler.
/// Selects menu items in response to the appropriate keystroke.
/// \param keystroke Keyboard event to process
/// \return TRUE if the player hit ESC

BOOL CInputManager::MenuKeyboardHandler(DWORD keystroke){ //menu
  BOOL result=FALSE;
  switch(keystroke){
    case DIK_ESCAPE:
        result = TRUE; break; //exit game
    case DIK_DOWN:
        playSound(BLEEP_SOUND);
        if(GameRenderer.menuArrowPos<2)
            GameRenderer.menuArrowPos++;
        else
            GameRenderer.menuArrowPos = 0;
        break;
    case DIK_UP:
        playSound(BLEEP_SOUND);
        if(GameRenderer.menuArrowPos>0)
            GameRenderer.menuArrowPos--;
        else
            GameRenderer.menuArrowPos = 2;
        break;
    case DIK_RETURN:
         if(GameRenderer.menuArrowPos==2)
            result = TRUE; //exit game
        else
        {
            GameRenderer.menuOptionSelected = GameRenderer.menuArrowPos;
            if(GameRenderer.menuOptionSelected==0)
                g_pSoundManager->stop();
            playSound(SELECT_SOUND);
         }
        break;
    default: break; //do nothing
  }
  return result;
} //MenuKeyboardHandler

/// Keyboard handler for device menu.
/// Sets g_bEndGameState to TRUE if we are to exit the device menu.
/// \param keystroke Keyboard event to process

void CInputManager::DeviceMenuKeyboardHandler(DWORD keystroke){
  if(getKeyFlag==1)
  {
      if(keystroke==DIK_ESCAPE)
      {
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
      }
      switch(GameRenderer.dMenuOptionSelected)
      {
      case 0:
          p1D = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;
      case 1:
          p1L = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;
      case 2:
          p1R = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;
      case 3:
          p1U = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;  
      case 4:
          p1A = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;
      case 5:
          p1B = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;
      case 6:
          p1C = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;
      case 7:
          p2D = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;
      case 8:
          p2L = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;
      case 9:
          p2R = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;
      case 10:
          p2U = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;
      case 11:
          p2A = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;
      case 12:
          p2B = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;
      case 13:
          p2C = keystroke;
          GameRenderer.dMenuOptionSelected = -1;
          getKeyFlag = 0;
          playSound(SELECT_SOUND);
          break;
      default: break;
      }
  }


  else if(getKeyFlag!=1)
  {
    switch(keystroke){
    case DIK_ESCAPE:
        g_bEndGameState = TRUE; break; //exit game
    case DIK_DOWN:
        playSound(BLEEP_SOUND);
        if(GameRenderer.dMenuArrowPos<14)
            GameRenderer.dMenuArrowPos++;
        else
            GameRenderer.dMenuArrowPos = 0;
        break;
    case DIK_UP:
        playSound(BLEEP_SOUND);
        if(GameRenderer.dMenuArrowPos>0)
            GameRenderer.dMenuArrowPos--;
        else
            GameRenderer.dMenuArrowPos = 14;
        break;
    case DIK_RETURN:
        playSound(BLEEP_SOUND);
        if(GameRenderer.dMenuArrowPos==14)
            g_bEndGameState = TRUE; //exit to menu
        else
        {
            GameRenderer.dMenuOptionSelected = GameRenderer.dMenuArrowPos;
            getKeyFlag = 1;
        }
        break;
    default: break; //do nothing
    }
  }
} //DeviceMenuKeyboardHandler




/////////////////////////////////////
//private void intitializeFont()
///{
//  System.Drawing.Font systemfont = new System.Drawing.Font("Arial", 12f, FontStyle.Regular);
//}
////////////////////////////////////



/// Game menu keyboard handler.
/// Handles keystroke during game play.
/// Sets g_bEndGameState if the player hit ESC
/// \param keystroke Keyboard event to process
/// \param override TRUE if we to accept keystroke even when not in keyboard mode

void CInputManager::GameKeyboardHandler(DWORD keystroke, BOOL Override){ //game
  int testCommandInterpreter;
  switch(keystroke){
    case DIK_ESCAPE: g_bEndGameState=TRUE; break;
    case DIK_F1: GameRenderer.FlipCameraMode(); break;        
    case DIK_F2:
      g_bWireFrame = !g_bWireFrame;
      if(g_bWireFrame)
        GameRenderer.m_d3ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
      else
        GameRenderer.m_d3ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
      break;
    case DIK_F3: g_bShowFrameRate = !g_bShowFrameRate; break;
    case DIK_F4: 
      if(hitBoxDisplay == 0)
        hitBoxDisplay = 1;
      else hitBoxDisplay = 0;
  }
  
  //other keys in KEYBOARD_INPUT mode or when overridden
  if(m_nInputDevice == KEYBOARD_INPUT || Override)
    if(toggletest == 0)
    {
      toggletest = 1;
    }
    else if(toggletest == 1)
    {
      std::cout << "TEST" << std::endl;
    }

    int keymapCode = keymap(keystroke);
	switch(keystroke)
	{
	case DIK_P:
		g_cObjectManager.freezeGame();
		break;
	case DIK_U:
		g_cObjectManager.unFreezeGame();
		break;
	case DIK_I:
		g_cObjectManager.getCurrentAnimationFrame(1,0,1,0,0);
		break;
	case DIK_K:
		g_cObjectManager.getCurrentAnimationFrame(1,0,-1,0,0);
		break;
	case DIK_L:
		g_cObjectManager.getCurrentAnimationFrame(1,1,0,0,0);
		break;
	case DIK_J:
		g_cObjectManager.getCurrentAnimationFrame(1,-1,0,0,0);
		break;
	case DIK_T:
		g_cObjectManager.getCurrentAnimationFrame(1,0,0,0,.01);
		break;
	case DIK_G:
		g_cObjectManager.getCurrentAnimationFrame(1,0,0,0,-.01);
		break;
	case DIK_F:
		g_cObjectManager.getCurrentAnimationFrame(1,0,0,-.01,0);
		break;
	case DIK_H:
		g_cObjectManager.getCurrentAnimationFrame(1,0,0,.01,0);
		break;
	}
    switch(keymapCode){
      case 18:
          if(p1MoveOK==FALSE)
              break;
        g_cObjectManager.setDirHeld(1,8,1); //Set p1 up flag to 1.
        g_cObjectManager.jump(1);
        break;
      case 12:
          if(p1MoveOK==FALSE)
              break;
        g_cObjectManager.setDirHeld(1,2,1); //set p1 down flag to 1.
				g_cObjectManager.crouch(1);

        break;
      case 14: 
          if(p1MoveOK==FALSE)
              break;
        if(p1InputBuffer.dashTimer4 > 0)
        {
            g_cObjectManager.dash(1, 4, -40);
        }
        else
        {
          p1InputBuffer.dashTimer4 = INBUF_SIZE;
          g_cObjectManager.walk(1, 4);
        }
        g_cObjectManager.setDirHeld(1,4,1); //set p1 left flag to 1.
        
        break;
      case 16: 
          if(p1MoveOK==FALSE)
              break;
        if(p1InputBuffer.dashTimer6 > 0)
        {
            g_cObjectManager.dash(1, 6, 40);
        }
        else
        {
          p1InputBuffer.dashTimer6 = INBUF_SIZE;
          g_cObjectManager.walk(1,6);
        }
        g_cObjectManager.setDirHeld(1,6,1);  //set p1 right flag to 1.
        
        break;

        //p2 controls
      case 28: 
          if(p2MoveOK==FALSE)
              break;
        g_cObjectManager.setDirHeld(2,8,1); //Set p1 up flag to 1.
        g_cObjectManager.jump(2);
        break;
      case 22: 
          if(p2MoveOK==FALSE)
              break;
        g_cObjectManager.setDirHeld(2,2,1);
				g_cObjectManager.crouch(2);
        break;
	  case 23:
		  if(p2AtkOK==FALSE)
              break;
        testCommandInterpreter = p2InputBuffer.getCommand();
				g_cObjectManager.passInputs(2,testCommandInterpreter,1);
				break;
		case 25:
		  if(p2AtkOK==FALSE)
              break;
        testCommandInterpreter = p2InputBuffer.getCommand();
				g_cObjectManager.passInputs(2,testCommandInterpreter,2);
      case 24:
          if(p2MoveOK==FALSE)
              break;
        if(p2InputBuffer.dashTimer4 > 0)
        {
            g_cObjectManager.dash(2, 4, -40);
            //g_cObjectManager.SetSpeedX(-40);
        }
        else
        {
          p2InputBuffer.dashTimer4 = INBUF_SIZE;
          g_cObjectManager.walk(2, 4);
        }
        g_cObjectManager.setDirHeld(2,4,1);
        break;

      case 26: 
        if(p2MoveOK==FALSE)
            break;
        if(p2InputBuffer.dashTimer6 > 0)
        {
            g_cObjectManager.dash(2, 6, 40);
        }
        else
        {
          p2InputBuffer.dashTimer6 = INBUF_SIZE;
          g_cObjectManager.walk(2,6);
        }
        g_cObjectManager.setDirHeld(2,6,1);  
        break;
        //end p2 controls
      case 21:
          if(p2AtkOK==FALSE)
              break;
        testCommandInterpreter = p2InputBuffer.getCommand();
				g_cObjectManager.passInputs(2,testCommandInterpreter,0);

        break;
      case 11:  //p1 A button	//SET THIS UP TO CALL INTERPRETCOMMAND
          if(p1AtkOK==FALSE)
              break;
        std::cout << "stopping for check" << std::endl;
        testCommandInterpreter = p1InputBuffer.getCommand();
		g_cObjectManager.passInputs(1,testCommandInterpreter,0);
		
        if(testCommandInterpreter == 1)
          std::cout << "236" << std::endl;
        else if(testCommandInterpreter == 2)
          std::cout << "shoryuken!" << std::endl;
        else if(testCommandInterpreter == 3)
          std::cout << "41236" << std::endl;
        else if(testCommandInterpreter == 4)
          std::cout << "214" << std::endl;
        else if(testCommandInterpreter == 5)
          std::cout << "421" << std::endl;
        else if(testCommandInterpreter == 6)
          std::cout << "63214" << std::endl;
        else if(testCommandInterpreter == 7)
          std::cout << "22" << std::endl;
        else
        {
			float a, b, c, d;
          g_cObjectManager.GetPlayerLocations(a,b,c,d);
          std::cout << "no input" << std::endl;
        }
        break;
      case 13:  //p1 B button
          if(p1AtkOK==FALSE)
              break;
		//g_cObjectManager.setAnimationState(1,3);   // animation for light kick
				testCommandInterpreter = p1InputBuffer.getCommand();
				g_cObjectManager.passInputs(1,testCommandInterpreter,1);
        std::cout << "stopping for check" << std::endl;
        break;
      case 15: //p1 C button
          if(p1AtkOK==FALSE)
              break;
				testCommandInterpreter = p1InputBuffer.getCommand();
				g_cObjectManager.passInputs(1,testCommandInterpreter,2);
        std::cout << "stopping for check" << std::endl;
        break;
      default: break;

    }

} //GameKeyboardHandler

//mouse processing

/// Process mouse input.
/// Processes mouse events that have been buffered since the last
/// time this function was called.
/// \return TRUE if we still have control of the mouse

BOOL CInputManager::ProcessMouseInput(){ //process buffered mouse events

  BOOL result=FALSE; //TRUE if game is over
  if(!m_pMouse)return FALSE; //no device so fail and bail
  DWORD dwElements=DI_BUFSIZE; //number of events to process

  //get number of events to process into member buffer m_DIod
  HRESULT hr=m_pMouse->GetDeviceData(
    sizeof(DIDEVICEOBJECTDATA), m_DIod,&dwElements, 0);

  //if it failed, we may be able to acquire the device
  if(FAILED(hr)){ //lost the device somehow
    hr=m_pMouse->Acquire(); //try to reacquire
    while(hr == DIERR_INPUTLOST) hr = m_pMouse->Acquire(); //try real hard
    return FALSE; //live to fight another day
  }
  if(FAILED(hr))return FALSE; //fail and bail (paranoia)

  //process inputs from m_DIod now
  for(int i=0; i<(int)dwElements; i++){ //for each input
    switch(m_DIod[i].dwOfs){
      case DIMOFS_BUTTON0: //left mouse button
        if(m_DIod[i].dwData & 0x80)LMouseDown(); //down
        else result = LMouseUp(); //up
        break;
      case DIMOFS_X: MouseMove(m_DIod[i].dwData, 0); break;
      case DIMOFS_Y: MouseMove(0, m_DIod[i].dwData); break;
    }
  }
  return result;
}

/// Compute relative mouse position.
/// In windowed mode the mouse cursor position is reported relative to the
/// destop, not relative to the window. This function adjusts for window position
/// and size, returning a mouse position relative to the game default resolution.

void CInputManager::MouseAbsoluteToRelative(POINT abs, POINT& rel){  //adjust mouse coordinates relative to top left corner of window
  rel = abs;
  if(g_bWindowed){ //don't do this in fullscreen mode
    //adjust for window position
    rel.x -= g_pointWindowPosition.x;
    rel.y -= g_pointWindowPosition.y;
    //adjust for  window height and width
    rel.y = g_nScreenHeight * rel.y / g_nWindowHeight;
    rel.x = g_nScreenWidth * rel.x / g_nWindowWidth;
  }
}

//mouse handlers

/// Set the player speed from the mouse position.
/// The mouse acts like a joystick, but upside down.
/// \param point Mouse position
/// \param extent Extent of mouse position, meaning limits on how far the mouse can move

void CInputManager::SetPlayerSpeed(POINT point, SIZE extent){

  int xmin, xmax, ymin, ymax; //player speed limits
  g_cObjectManager.GetSpeedLimits(xmin, xmax, ymin, ymax); //get them 

  //bands for speed assignment
  const int XBANDWIDTH = extent.cx/(xmax - xmin + 1);
  const int YBANDWIDTH = extent.cy/(ymax - ymin + 1);


  int xspeed = point.x/XBANDWIDTH + xmin; //horizontal speed
  int yspeed = point.y/YBANDWIDTH + ymin; //vertical speed

  g_cObjectManager.SetSpeed(1,xspeed, yspeed); //pass to player
}

/// Handle mouse motion.
/// \param x Mouse x position
/// \param y Mouse y position

void CInputManager::MouseMove(int x, int y){

  //process mouse move
  m_pointMouse.x += x; m_pointMouse.y += y;
  GameRenderer.m_d3ddevice->SetCursorPosition(m_pointMouse.x, m_pointMouse.y, 0); 
  MouseAbsoluteToRelative(m_pointMouse, m_ptMouseRelative);

  //did it come in from outside? If so, set the image back
  BOOL bWasOutsideWindow = m_bCursorOutsideWindow;
  m_bCursorOutsideWindow = m_ptMouseRelative.x < 0 || m_ptMouseRelative.x > g_nScreenWidth ||
    m_ptMouseRelative.y < 0 || m_ptMouseRelative.y > g_nScreenHeight;
  if(g_bWindowed && bWasOutsideWindow && !m_bCursorOutsideWindow) //it was out, and it came in
    GameRenderer.SetMouseCursor();

  //mouse as joystick
  if(g_nCurrentGameState != PLAYING_GAMESTATE)return; //bail if not playing
  if(m_nInputDevice!=MOUSE_INPUT)return; //bail if mouse not selected
  
  //set extent
  SIZE extent; //extent that mouse moves in
  extent.cx = g_nScreenWidth;
  extent.cy = g_nScreenHeight; 

  //set player speed based on point and extent
  SetPlayerSpeed(m_ptMouseRelative, extent); 
}

/// Main menu left mouse button down handler.

void CInputManager::MainMenuLMouseDown(){
  if(m_pButtonManager->hit(m_ptMouseRelative) >= 0) //if a valid hit
    m_pButtonManager->buttondown(m_ptMouseRelative); //animate a button down
}


/// Mouse left button down handler for device menu.

void CInputManager::DeviceMenuLMouseDown(){
  int hit = m_pButtonManager->hit(m_ptMouseRelative); //which button is hit by mouse

  //fake mouse action by making keyboard handler so all the work
  switch(hit){
    case 0: DeviceMenuKeyboardHandler(DIK_K); break; //keyboard
    case 1: DeviceMenuKeyboardHandler(DIK_M); break; //mouse
    case 2: DeviceMenuKeyboardHandler(DIK_J); break; //mouse
    case 3:
      m_pButtonManager->buttondown(m_ptMouseRelative); //quit button down
      break;
    default: break;
  }
}

/// Game left mouse button down handler.

void CInputManager::GameLMouseDown(){
  if(m_nInputDevice != MOUSE_INPUT)return; //bail if mouse not selected
  GameKeyboardHandler(DIK_SPACE, TRUE); //let keyboard manager handle it
}

/// Main mouse left button down handler.

void CInputManager::LMouseDown(){
  switch(g_nCurrentGameState){
    case MENU_GAMESTATE: MainMenuLMouseDown(); break;
    case PLAYING_GAMESTATE: GameLMouseDown(); break;
    case DEVICEMENU_GAMESTATE: DeviceMenuLMouseDown(); break;
  }
}

//mouse left button up handlers

/// Intro left mouse button up handler.

void CInputManager::IntroLMouseUp(){
  g_bEndGameState = TRUE;
}

/// Main menu left mouse button up handler.
/// \return TRUE if player clicked on the Quit button

BOOL CInputManager::MainMenuLMouseUp(){ 
  int hit = m_pButtonManager->release(m_ptMouseRelative); //get button hit
  BOOL result=FALSE; //return result

  //take action depending on which button was hit
  switch(hit){ //depending on which button was hit
    case 0: MenuKeyboardHandler(DIK_N); break; //new game
    case 1: result = MenuKeyboardHandler(DIK_Q); break; //quit
    default: break;
  }

  //animate button images
  if(hit>=0)m_pButtonManager->buttonup(hit); //hit
  else m_pButtonManager->allbuttonsup(); //nonhit

  return result;
}

/// Mouse left button up handler for device menu.

void CInputManager::DeviceMenuLMouseUp(){
  int hit = m_pButtonManager->release(m_ptMouseRelative);
  switch(hit){
    case 3: //quit button up
      m_pButtonManager->buttonup(hit); //show quit button up
      DeviceMenuKeyboardHandler(DIK_ESCAPE);
      break;
  }
  if(hit<0)m_pButtonManager->allbuttonsup(); //no hit
}

/// Main mouse left button up handler
/// return TRUE if player wants to exit the main menu

BOOL CInputManager::LMouseUp(){ 

  BOOL result = FALSE; //return result

  switch(g_nCurrentGameState){
    case LOGO_GAMESTATE:
    case TITLE_GAMESTATE: 
    case CREDITS_GAMESTATE: IntroLMouseUp(); break;
    case MENU_GAMESTATE: result = MainMenuLMouseUp(); break;
    case DEVICEMENU_GAMESTATE: DeviceMenuLMouseUp(); break;
  }

  return result;
}

/// Set up button manager.
/// \param count Number of buttons.
/// \param x Horizontal coordinate of first button
/// \param y Vertical coordinate of first button
/// \param ydelta Vertical spacing between buttons

void CInputManager::SetupMenuButtons(int count, int x, int y, int ydelta){ 

  SIZE size; //size of buttons
  size.cx = 40; size.cy = 40; //size of buttons

  POINT point; //top left corner of first button
  point.x = x; point.y = y; //first button location

  delete m_pButtonManager; //delete any old one
  m_pButtonManager = new CButtonManager(count, size, point, ydelta); //create new button manager

  //set button sounds
  //m_pButtonManager->setsounds(BIGCLICK_SOUND,SMALLCLICK_SOUND);
}

/// Set up main menu buttons.
/// Button positions were hard-coded, changed to func of screen res.

void CInputManager::SetupMainMenuButtons(){ 
//set up main menu button manager
//  SetupMenuButtons(2, 188, 89, 110); //old
    SetupMenuButtons(2, (int)(g_nScreenWidth*(188.0f/1024.0f)), (int)(g_nScreenWidth*(89.0f/1024.0f)), (int)(g_nScreenHeight*(110.0f/768.0f))); //new
}


/// Set up device menu buttons.
/// Button positions were hard-coded, changed to func of screen res.

void CInputManager::SetupDeviceMenuButtons(){ 
//  SetupMenuButtons(4, 188, 220, 130); //old
    SetupMenuButtons(4, (int)(g_nScreenWidth*(188.0f/1024.0f)), (int)(g_nScreenWidth*(220.0f/1024.0f)), (int)(g_nScreenHeight*(130.0f/768.0f))); //new
  //initialize image of radio button
  m_pButtonManager->SetRadio(); //radio buttons
  m_pButtonManager->buttondown(m_nInputDevice, FALSE); //no sound
}


/// Set up menu buttons for the appropriate game state.
/// \param state Current game state.

void CInputManager::SetupButtons(GameStateType state){
//set up menu buttons
  switch(state){
    case MENU_GAMESTATE: SetupMainMenuButtons(); break;
    case DEVICEMENU_GAMESTATE: SetupDeviceMenuButtons(); break;
  }
}

/// Draw mouse buttons.

void CInputManager::DrawButtons(){ //draw buttons
  if(m_pButtonManager)m_pButtonManager->DrawButtons();
}

/// Change game state.
/// The mouse behaves differently in different game states.
/// \param state Current game state.

void CInputManager::ChangeState(GameStateType state){ //change game state

  switch(g_nCurrentGameState){

    case MENU_GAMESTATE:
      //m_pointMouse.x = g_nScreenWidth/2; 
      //m_pointMouse.y = g_nScreenHeight/2;
      //GameRenderer.m_d3ddevice->SetCursorPosition(
      //  g_pointWindowPosition.x + g_nWindowWidth/2, g_pointWindowPosition.y + g_nWindowHeight/2, 0);     //show mouse cursor if not already showing
      //if(!m_bCursorShowing){ //show mouse cursor if not already showing
      //  m_bCursorShowing = TRUE;
      //  GameRenderer.m_d3ddevice->ShowCursor(TRUE); //show d3d cursor
      //}
      SetupButtons(MENU_GAMESTATE);
      break;
       
    case DEVICEMENU_GAMESTATE:
      SetupButtons(DEVICEMENU_GAMESTATE);
      break;

    case PLAYING_GAMESTATE:
      //m_pointMouse.x = g_nScreenWidth; 
      //m_pointMouse.y = g_nScreenHeight/2; //for initial player speed
      //GameRenderer.m_d3ddevice->SetCursorPosition(m_pointMouse.x, m_pointMouse.y, 0);
      if(m_bCursorShowing){ //hide mouse cursor if not already hidden
        m_bCursorShowing = FALSE;
        GameRenderer.m_d3ddevice->ShowCursor(FALSE); //hide d3d cursor
      }
      break;
  }
}

/*

BOOL CInputManager::XboxMenuHandler(){ //menu
  BOOL result=FALSE;
  switch(keystroke){
    case DIK_DOWN:
        playSound(BLEEP_SOUND);
        if(GameRenderer.menuArrowPos<2)
            GameRenderer.menuArrowPos++;
        else
            GameRenderer.menuArrowPos = 0;
        break;
    case DIK_UP:
        playSound(BLEEP_SOUND);
        if(GameRenderer.menuArrowPos>0)
            GameRenderer.menuArrowPos--;
        else
            GameRenderer.menuArrowPos = 2;
        break;
    case DIK_RETURN:
         if(GameRenderer.menuArrowPos==2)
            result = TRUE; //exit game
        else
        {
            GameRenderer.menuOptionSelected = GameRenderer.menuArrowPos;
            playSound(SELECT_SOUND);
         }
        break;
    default: break; //do nothing
  }
  return result;
} //MenuKeyboardHandler

*/



//Processes XBOX Controllers for Player 1 and Player 2

BOOL CInputManager::ProcessXboxController(int player)
{
	if(g_nCurrentGameState != PLAYING_GAMESTATE)return FALSE; //bail if not in game play
	//if(m_nInputDevice != JOYSTICK_INPUT)return FALSE; //bail if joystick not selected
	DWORD dwResult;
	dwResult = XInputGetState(player, &state);
	if(player == 0){
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER){
			if(X1C == 0){
			GameKeyboardHandler(DIK_M, TRUE);
			X1C = 1;
			}
		}
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_X){
			if(X1A == 0){
			GameKeyboardHandler(DIK_B, TRUE);
			X1A = 1;
			}
		}
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_Y){
			if(X1B == 0){
			GameKeyboardHandler(DIK_N, TRUE);
			X1B = 1;
			}
		}
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_A){

		}
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP){
			if(X1UP == 0){
			GameKeyboardHandler(DIK_W, TRUE);
			g_cObjectManager.setDirHeld(1,8,2);
			X1UP = 1;
			}
		}
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN){
			if(X1DOWN == 0){
			GameKeyboardHandler(DIK_S, TRUE);
			g_cObjectManager.setDirHeld(1,2,2);
			X1DOWN = 1;
			}
		}
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT){
			if(X1RIGHT == 0){
			GameKeyboardHandler(DIK_D, TRUE);
			g_cObjectManager.setDirHeld(1,6,2);
			X1RIGHT = 1;
			}
		}
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT){
			if(X1LEFT == 0){
			GameKeyboardHandler(DIK_A, TRUE);
			g_cObjectManager.setDirHeld(1,4,2);
			X1LEFT = 1;
			}
		}
		if(state.Gamepad.sThumbLY >= 7849){
			if(X1LSUP == 0){
			GameKeyboardHandler(DIK_W, TRUE);
			g_cObjectManager.setDirHeld(1,8,2);
			X1LSUP = 1;
			}
		}
		if(state.Gamepad.sThumbLX >= 7849){
			if(X1LSRIGHT == 0){
			GameKeyboardHandler(DIK_D, TRUE);
			g_cObjectManager.setDirHeld(1,6,2);
			X1LSRIGHT = 1;
			}
		}
		if(state.Gamepad.sThumbLY <= -7849){
			if(X1LSDOWN == 0){
			GameKeyboardHandler(DIK_S, TRUE);
			g_cObjectManager.setDirHeld(1,2,2);
			X1LSDOWN = 1;
			}
		}
		if(state.Gamepad.sThumbLX <= -7849){
			if(X1LSLEFT == 0){
			GameKeyboardHandler(DIK_A, TRUE);
			g_cObjectManager.setDirHeld(1,4,2);
			X1LSLEFT = 1;
			}
		}

		if(X1LSUP== 1 && !(state.Gamepad.sThumbLY >= 7849)){
			g_cObjectManager.setDirHeld(1,8,0);
			X1LSUP = 0;
			g_cObjectManager.stop(1);
		}

		if(X1LSDOWN== 1 && !(state.Gamepad.sThumbLY <= -7849)){
			g_cObjectManager.setDirHeld(1,2,0);
			g_cObjectManager.stopCrouching(1);
			X1LSDOWN = 0;
		}

		if(X1LSRIGHT == 1 && !(state.Gamepad.sThumbLX >= 7849)){
			g_cObjectManager.setDirHeld(1,6,0);
			X1LSRIGHT = 0;
			g_cObjectManager.stop(1);
		}

		if(X1LSLEFT == 1 && !(state.Gamepad.sThumbLX <= -7849)){
			g_cObjectManager.setDirHeld(1,4,0);
			X1LSLEFT = 0;
			g_cObjectManager.stop(1);
		}

		if(X1A == 1 && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_X)){
			X1A = 0;
		}
		if(X1B == 1 && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)){
			X1B = 0;
		}
		if(X1C == 1 && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)){
			X1C = 0;
		}
		if(X1RIGHT == 1 && ! (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)){
			g_cObjectManager.setDirHeld(1,6,0);
			X1RIGHT = 0;
			g_cObjectManager.stop(1);
		}
		if(X1DOWN == 1 && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)){
			g_cObjectManager.setDirHeld(1,2,0);
			g_cObjectManager.stopCrouching(1);
			X1DOWN = 0;
		}
		if(X1LEFT == 1 && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)){
			g_cObjectManager.setDirHeld(1,4,0);
			X1LEFT = 0;
			g_cObjectManager.stop(1);
		}
		if(X1UP == 1 && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)){
			g_cObjectManager.setDirHeld(1,8,0);
			X1UP = 0;
		}
	}

else if(player == 1){
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER){
			if(X2C == 0){
			GameKeyboardHandler(DIK_NUMPAD3, TRUE);
			X2C = 1;
			}
		}
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_X){
			if(X2A == 0){
			GameKeyboardHandler(DIK_NUMPAD1, TRUE);
			X2A = 1;
			}
		}
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_Y){
			if(X2B == 0){
			GameKeyboardHandler(DIK_NUMPAD2, TRUE);
			X2B = 1;
			}
		}
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP){
			if(X2UP == 0){
			GameKeyboardHandler(DIK_UP, TRUE);
			g_cObjectManager.setDirHeld(2,8,2);
			X2UP = 1;
			}
		}
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN){
			if(X2DOWN == 0){
			GameKeyboardHandler(DIK_DOWN, TRUE);
			g_cObjectManager.setDirHeld(2,2,2);
			X2DOWN = 1;
			}
		}
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT){
			if(X2RIGHT == 0){
			GameKeyboardHandler(DIK_RIGHT, TRUE);
			g_cObjectManager.setDirHeld(2,6,2);
			X2RIGHT = 1;
			}
		}
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT){
			if(X2LEFT == 0){
			GameKeyboardHandler(DIK_LEFT, TRUE);
			g_cObjectManager.setDirHeld(2,4,2);
			X2LEFT = 1;
			}
		}

		if(X2RIGHT == 1 && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)){
			g_cObjectManager.setDirHeld(2,6,0);
			X2RIGHT = 0;
			g_cObjectManager.stop(2);
		}
		if(X2DOWN == 1 && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)){
			g_cObjectManager.setDirHeld(2,2,0);
			g_cObjectManager.stopCrouching(2);
			X2DOWN = 0;
		}
		if(X2LEFT == 1 && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)){
			g_cObjectManager.setDirHeld(2,4,0);
			X2LEFT = 0;
			g_cObjectManager.stop(2);
		}
		if(X2UP == 1 && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)){
			g_cObjectManager.setDirHeld(2,8,0);
			X2UP = 0;
		}
		if(X2A == 1 && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_X)){
			X2A = 0;
		}
		if(X2B == 1 && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)){
			X2B = 0;
		}
		if(X2C == 1 && !(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)){
			X2C = 0;
		}

		if(state.Gamepad.sThumbLY >= 7849){
			if(X2LSUP == 0){
			GameKeyboardHandler(DIK_UP, TRUE);
			g_cObjectManager.setDirHeld(2,8,2);
			X2LSUP = 1;
			}
		}
		if(state.Gamepad.sThumbLX >= 7849){
			if(X2LSRIGHT == 0){
			GameKeyboardHandler(DIK_RIGHT, TRUE);
			g_cObjectManager.setDirHeld(2,6,2);
			X2LSRIGHT = 1;
			}
		}
		if(state.Gamepad.sThumbLY <= -7849){
			if(X2LSDOWN == 0){
			GameKeyboardHandler(DIK_DOWN, TRUE);
			g_cObjectManager.setDirHeld(2,2,2);
			X2LSDOWN = 1;
			}
		}
		if(state.Gamepad.sThumbLX <= -7849){
			if(X2LSLEFT == 0){
			GameKeyboardHandler(DIK_LEFT, TRUE);
			g_cObjectManager.setDirHeld(2,4,2);
			X2LSLEFT = 1;
			}
		}

		if(X2LSUP== 1 && !(state.Gamepad.sThumbLY >= 7849)){
			g_cObjectManager.setDirHeld(2,8,0);
			X2LSUP = 0;
			g_cObjectManager.stop(2);
		}

		if(X2LSDOWN== 1 && !(state.Gamepad.sThumbLY <= -7849)){
			g_cObjectManager.setDirHeld(2,2,0);
			g_cObjectManager.stopCrouching(2);
			X2LSDOWN = 0;	
		}

		if(X2LSRIGHT == 1 && !(state.Gamepad.sThumbLX >= 7849)){
			g_cObjectManager.setDirHeld(2,6,0);
			X2LSRIGHT = 0;
			g_cObjectManager.stop(2);
		}

		if(X2LSLEFT == 1 && !(state.Gamepad.sThumbLX <= -7849)){
			g_cObjectManager.setDirHeld(2,4,0);
			X2LSLEFT = 0;
			g_cObjectManager.stop(2);
		}

	}
}

/// Process joystick input.
/// Poll the joystick. Try to reacquire it if its been lost.
/// Process button 0 as if it were the space bar. Process
/// player speed from the slider (horizontal) and the y axis
/// (vertical). If there's no slider, use the x axis, which
/// works best for the gamepad and is weird to play but works for
/// a regular joystick.

BOOL CInputManager::ProcessJoystickInput(){ //process polled joystick events

  HRESULT hr; //result
  DIJOYSTATE2 js; //joystick state 

  if(g_nCurrentGameState != PLAYING_GAMESTATE)return FALSE; //bail if not in game play
  if(m_nInputDevice != JOYSTICK_INPUT)return FALSE; //bail if joystick not selected
  if(m_pJoystick == NULL)return FALSE; //bail and fail

  //poll the device to read the current state
  hr = m_pJoystick->Poll(); 
  if(FAILED(hr)){ //something bad has happened
    hr = m_pJoystick->Acquire(); //try to reacquire
    while(hr == DIERR_INPUTLOST)
      hr = m_pJoystick->Acquire(); //try real hard
    //if something *very* bad has happened, pretend it hasn't 
    return FALSE; 
  }

  //get the input's device state
  hr = m_pJoystick->GetDeviceState(sizeof(DIJOYSTATE2), &js);
  if(FAILED(hr))return FALSE; //should have been reacquired during the Poll()

  //fake space bar if button zero pressed
  if(js.rgbButtons[0] & 0x80)
    GameKeyboardHandler(DIK_SPACE, TRUE);

  //player speed from slider 0 and y axis
  if(g_nCurrentGameState != PLAYING_GAMESTATE)return TRUE; //bail if not playing
  POINT p; //point for joystick position
  if(m_bJoystickHasSlider) p.x = js.rglSlider[0]; //if we have slider, use for speed 
  else p.x = js.lX; //else use x axis (best for gamepad)
  p.y = 100 - js.lY;
  SIZE extent; //extent that joystick moves in
  extent.cx = extent.cy = 100; 

  //set player speed based on point and extent
  SetPlayerSpeed(p, extent);
  return TRUE;
}

//Notes for later:  using the new getDirHeld function, can probably make this
//so that it doesnt need to check what player is, it just hands it to the
//getDirHeld function.
int CInputManager::checkDirection(int player)
{
  int tempFacing = g_cObjectManager.getFacing(player);
  if(player == 1)
  {
    if(tempFacing == 6)
    {
      if(g_cObjectManager.getDirHeld(1,8) == 1 || g_cObjectManager.getDirHeld(1,8) == 2) //if p1upheld == 1
      {
        if(g_cObjectManager.getDirHeld(1,6) == 1 || g_cObjectManager.getDirHeld(1,6) == 2)  //if p1RightHeld == 1
          return 9;
        else if(g_cObjectManager.getDirHeld(1,4) == 1 || g_cObjectManager.getDirHeld(1,4) == 2)  //if p1LeftHeld == 1
          return 7;
        else return 8;
      }
      else if(g_cObjectManager.getDirHeld(1,2) == 1 || g_cObjectManager.getDirHeld(1,2) == 2)  //if p1DownHeld == 1
      {
        if(g_cObjectManager.getDirHeld(1,6) == 1 || g_cObjectManager.getDirHeld(1,6) == 2)  //if p1RightHeld == 1
          return 3;
        else if(g_cObjectManager.getDirHeld(1,4) == 1 || g_cObjectManager.getDirHeld(1,4) == 2)  //if p1LeftHeld == 1
          return 1;
        else return 2;
      }
      else if(g_cObjectManager.getDirHeld(1,6) == 1 || g_cObjectManager.getDirHeld(1,6) == 2) //if p1RightHeld == 1
        return 6;
      else if(g_cObjectManager.getDirHeld(1,4) == 1 || g_cObjectManager.getDirHeld(1,4) == 2)  //if p1LeftHeld == 1
        return 4;
      else 
        return 5;
    }
    else if(tempFacing == 4)
    {
      if(g_cObjectManager.getDirHeld(1,8) == 1 || g_cObjectManager.getDirHeld(1,8) == 2)   //if p1UpHeld == 1
      {
        if(g_cObjectManager.getDirHeld(1,6) == 1 || g_cObjectManager.getDirHeld(1,6) == 2)  //if p1RightHeld == 1
          return 7;
        else if(g_cObjectManager.getDirHeld(1,4) == 1 || g_cObjectManager.getDirHeld(1,4) == 2)  //if p1LeftHeld == 1
          return 9;
        else return 8;
      }
      else if(g_cObjectManager.getDirHeld(1,2) == 1 || g_cObjectManager.getDirHeld(1,2) == 2)  //if p1DownHeld == 1
      {
        if(g_cObjectManager.getDirHeld(1,6) == 1 || g_cObjectManager.getDirHeld(1,6) == 2)  //if p1RightHeld == 1
          return 1;
        else if(g_cObjectManager.getDirHeld(1,4) == 1 || g_cObjectManager.getDirHeld(1,4) == 2)  //if p1LeftHeld == 1
          return 3;
        else return 2;
      }
      else if(g_cObjectManager.getDirHeld(1,6) == 1 || g_cObjectManager.getDirHeld(1,6) == 2)  //if p1RightHeld == 1
        return 4;
        else if(g_cObjectManager.getDirHeld(1,4) == 1 || g_cObjectManager.getDirHeld(1,4) == 2)  //if p1LeftHeld == 1
        return 6;
      else 
        return 5;
    }
  }
  else if(player == 2)
  {
    if(tempFacing == 6)
    {
      if(g_cObjectManager.getDirHeld(2,8) == 1 || g_cObjectManager.getDirHeld(2,8) == 2) //if p1upheld == 1
      {
        if(g_cObjectManager.getDirHeld(2,6) == 1 || g_cObjectManager.getDirHeld(2,6) == 2)  //if p1RightHeld == 1
          return 9;
        else if(g_cObjectManager.getDirHeld(2,4) == 1 || g_cObjectManager.getDirHeld(2,4) == 2)  //if p1LeftHeld == 1
          return 7;
        else return 8;
      }
      else if(g_cObjectManager.getDirHeld(2,2) == 1 || g_cObjectManager.getDirHeld(2,2) == 2)  //if p1DownHeld == 1
      {
        if(g_cObjectManager.getDirHeld(2,6) == 1 || g_cObjectManager.getDirHeld(2,6) == 2)  //if p1RightHeld == 1
          return 3;
        else if(g_cObjectManager.getDirHeld(2,4) == 1 || g_cObjectManager.getDirHeld(2,4) == 2)  //if p1LeftHeld == 1
          return 1;
        else return 2;
      }
      else if(g_cObjectManager.getDirHeld(2,6) == 1 || g_cObjectManager.getDirHeld(2,6) == 2) //if p1RightHeld == 1
        return 6;
      else if(g_cObjectManager.getDirHeld(2,4) == 1 || g_cObjectManager.getDirHeld(2,4) == 2)  //if p1LeftHeld == 1
        return 4;
      else 
        return 5;
    }
    else if(tempFacing == 4)
    {
      if(g_cObjectManager.getDirHeld(2,8) == 1 || g_cObjectManager.getDirHeld(2,8) == 2)   //if p1UpHeld == 1
      {
        if(g_cObjectManager.getDirHeld(2,6) == 1 || g_cObjectManager.getDirHeld(2,6) == 2)  //if p1RightHeld == 1
          return 7;
        else if(g_cObjectManager.getDirHeld(2,4) == 1 || g_cObjectManager.getDirHeld(2,4) == 2)  //if p1LeftHeld == 1
          return 9;
        else return 8;
      }
      else if(g_cObjectManager.getDirHeld(2,2) == 1 || g_cObjectManager.getDirHeld(2,2) == 2)  //if p1DownHeld == 1
      {
        if(g_cObjectManager.getDirHeld(2,6) == 1 || g_cObjectManager.getDirHeld(2,6) == 2)  //if p1RightHeld == 1
          return 1;
        else if(g_cObjectManager.getDirHeld(2,4) == 1 || g_cObjectManager.getDirHeld(2,4) == 2)  //if p1LeftHeld == 1
          return 3;
        else return 2;
      }
      else if(g_cObjectManager.getDirHeld(2,6) == 1 || g_cObjectManager.getDirHeld(2,6) == 2)  //if p1RightHeld == 1
        return 4;
        else if(g_cObjectManager.getDirHeld(2,4) == 1 || g_cObjectManager.getDirHeld(2,4) == 2)  //if p1LeftHeld == 1
        return 6;
      else 
        return 5;
    }
  }
  else
    return 0;
}


//use this to play a sound
// \param index is the index of the sound to be played
// \param looping is TRUE for loop, FALSE otherwise
void CInputManager::playSound(int index, BOOL looping){
    g_pSoundManager->play(index, looping);
}



BOOL CInputManager::InitXboxController()
{
	DWORD dwResult;
	for(DWORD i = 0;i < 2; i++)
	{
		XINPUT_STATE test;
		ZeroMemory(&test, sizeof(XINPUT_STATE));
		dwResult = XInputGetState(i, &test);
		if(dwResult == ERROR_SUCCESS)
			totalcontrollers++;
	}
	return TRUE;
}



int CInputManager::keymap(DWORD keystroke)
{
    if(keystroke==p1D)
        return 12;
    else if(keystroke==p1L)
        return 14;
    else if(keystroke==p1R)
        return 16;
    else if(keystroke==p1U)
        return 18;
    else if(keystroke==p1A)
        return 11;
    else if(keystroke==p1B)
        return 13;
    else if(keystroke==p1C)
        return 15;
    
    else if(keystroke==p2D)
        return 22;
    else if(keystroke==p2L)
        return 24;
    else if(keystroke==p2R)
        return 26;
    else if(keystroke==p2U)
        return 28;
    else if(keystroke==p2A)
        return 21;
    else if(keystroke==p2B)
        return 23;
    else if(keystroke==p2C)
        return 25;

    else if(keystroke==p0Start)
        return 1;

    else
        return 0;
}
