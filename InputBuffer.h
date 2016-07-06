/// \file InputBuffer.h
/// Header file for input buffers!

#pragma once

#include "defines.h" //essential defines

#define INBUF_SIZE 12

class InputBuffer
{
  private:
    int buffer[INBUF_SIZE];
    int tracker;
  public:
    InputBuffer();
    //~InputBuffer();
    int dashTimer6;
    int dashTimer4;
    void incrTracker();
    void write(int input);
    int getCommand();   //Might add a parameter for this later that has the attack button pressed, and maybe
                        //a return type for the attack performed... we'll see
};