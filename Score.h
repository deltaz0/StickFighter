/// \file score.h
/// Interface for the score manager class CScoreManager.
/// Copyright Ian Parberry, 2004
/// Last updated November 2, 2004

#pragma once

#include "object.h" //for object types

const int LEVEL_BONUS_SCORE = 10; //pts for level completion

/// Score manager class.
/// Stores the player's points, health and life, and provides an
/// interface to manage them.

class CScoreManager{

  private:
    int m_nScore; ///< Points scored.
    int m_nHealth; //how many times player has been hit.
    int m_nLives; //how many lives the player has.

  public:
    CScoreManager(); ///< Constructor.
    void reset(); ///< Reset to start conditions.
    int score(); ///< Return current points score.
    int health(); ///< Return player health.
    int lives(); ///< Return number of lives.
    void AddToScore(int delta); ///< Add delta to score.
    void AddToScore(ObjectType object); ///< For killing object.
    void collision(); ///< Collision, bad for health.
    void DeductLife(); ///< Lose a life if health bad.

};

