/// \file sndlist.h
/// Enumerated types for sounds.
/// Copyright Ian Parberry, 2004
/// Last updated October 1, 2004

#pragma once

/// Game sound enumerated type. 
/// These are the sounds used in actual gameplay. Sounds must be listed here in
/// the same order that they are in the sound settings XML file.

enum GameSoundType{ //sounds used in game engine
  //mouse clicks
  MENUDROP_SOUND,
  BREAKBEAT_SOUND,
  ELECTRO_SOUND,
  TRIBAL_SOUND,
  VIBE_SOUND,
  LHIT_SOUND,
  HHIT_SOUND,
  BLOCKHIT_SOUND,
  JUMP_SOUND,
  AIRJUMP_SOUND,
  FDASH_SOUND,
  BDASH_SOUND,
  AFDASH_SOUND,
  ABDASH_SOUND,
  LANDING_SOUND,
  STANDINGL_SOUND,
  STANDINGM_SOUND,
  STANDINGH_SOUND,
  CROUCHINGL_SOUND,
  CROUCHINGM_SOUND,
  CROUCHINGH_SOUND,
  JUMPINGL_SOUND,
  JUMPINGM_SOUND,
  HEADBUTTL_SOUND,
  HEADBUTTM_SOUND,
  BLEEP_SOUND,
  SELECT_SOUND,
  NOW_SOUND,
  FIGHT_SOUND,
  LAUGH_SOUND
};

/// Intro sound enumerated type. 
/// These are the sounds used in the intro sequence and menus. Sounds must 
/// be listed here in  the same order that they are in the sound settings XML file.

enum IntroSoundType{ //sounds used during the intro
  TITLE_SOUND, //sound used during title screen
  LOGO_SOUND, //signature chord
  CREDITS_SOUND,
};
