/// \file score.cpp
/// Code for the score manager class CScoreManager.
/// Copyright Ian Parberry, 2004
/// Last updated November 2, 2004 

#include <windows.h>
#include <windowsx.h>

#include "score.h"
#include "sound.h" //for sound manager

extern CSoundManager* g_pSoundManager; //sound manager
extern BOOL g_bEndGameState; //level ends when last life gone

const int START_LIVES = 3; //initial number of lives
const int MAX_HEALTH = 3; //maximum player health
const int SCORE_PER_LIFE = 100; //pts to earn another life

CScoreManager::CScoreManager(){ //constructor
  reset();
}

/// Reset to initial conditions.
/// Zero points, maximum health, and a few lives to start with.

void CScoreManager::reset(){ //reset to start conditions
  m_nScore = 0; 
  m_nHealth = MAX_HEALTH; 
  m_nLives = START_LIVES;
}

/// Add to the player's score.
/// Adds some points to the player's score and computes whether the
/// player gets any extra lives. If so, it plays a sound and racks up
/// the new lives.
/// \param delta amount to add to the player's score

void CScoreManager::AddToScore(int delta){

  //add delta to score
  int oldscore=m_nScore; //old score
  m_nScore+=delta; //add to score

  //compute new lives, if any
  int newlives=  m_nScore/SCORE_PER_LIFE - oldscore/SCORE_PER_LIFE; //new lives earned
  if(newlives) //play sound if nonzero
  m_nLives += newlives; //add new lives to counter
}

/// Add to the player's score for killing an object.
/// \param object the type of object killed

void CScoreManager::AddToScore(ObjectType object){
  switch(object){ //score depends on object
    //YOUR CODE GOES HERE
    default: break; //default is nothing
  }
}

/// What to do when a player collides with something.
/// Deducts from the player's health.

void CScoreManager::collision(){ //collision, bad for health
  --m_nHealth; //deduct from health
}

/// Deduct a life from the player, if health has dropped low enough.
/// Examines the player's health. If it is too low, they lose a life,
/// anf their health gets restored. If they have no lives remaining, the
/// game is ended by setting g_bEndGameState to TRUE.

void CScoreManager::DeductLife(){ //lose a life
  if(m_nHealth <= 0){ //if health bad
    m_nHealth = MAX_HEALTH; //make healthy
    if(--m_nLives < 0) //deduct a life, if last
      g_bEndGameState = TRUE; //end of game
  }
}

/// Reader function for the number of lives.
/// \return number of lives remaining

int CScoreManager::lives(){ //return number of lives
  return m_nLives;
}

/// Reader function for the score.
/// \return nthe score

int CScoreManager::score(){ //return score
  return m_nScore;
}

/// Reader function for the payer health.
/// \return health value

int CScoreManager::health(){ //return health
  return m_nHealth;
}
