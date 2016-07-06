/// \file InputBuffer.cpp
///Code for an 8 frame input buffer. WEE!

#include "InputBuffer.h"
#include "Input.h"

int buffer[INBUF_SIZE];
int dashTimer6 = 0;
int dashTimer4 = 0;
int tracker;  //Tracker should always point at slot one frame AFTER current frame, so reading from the input
              //buffer will require checking buffer[tracker-1]

InputBuffer::InputBuffer()
{
  for(int a = 0; a < (int)INBUF_SIZE; a++)
  {
    buffer[a] = 5;
  }
  tracker = 0;
}

void InputBuffer::incrTracker() //Increments the tracker by one.  Not sure this will really be needed, since
{                               //we can just have the write function increment it automatically...
  if(tracker == INBUF_SIZE - 1)
  {
    tracker = 0;
  }
  else
    tracker++;
}

void InputBuffer::write(int input)
{
  buffer[tracker] = input;
  if(tracker == INBUF_SIZE - 1)
  {
    tracker = 0;
  }
  else
    tracker++;
}

//Checks the buffer to see what command the player has attempted.
//returns:  0 for nothing
//          1 for 236
//          2 for 623
//          3 for 41236
//          4 for 214
//          5 for 421
//          6 for 63214
//          7 for 22
//The system places the highest priority on half circle inputs, so 641236 may have both a 236 and 623 input in it,
//but the algorythm will see the 41236 and use that as the input.  Similarly, 623 has higher priority than 236,
//so 6236 gives a 623 command.  The system does check for some errors in input, namely 6323 and 63236 will still
//be read as a 623 input, but anything other than a 3 between the 6 and 2 will invalidate the input.  
//The same is true of inputs in the opposite direction, essentially making the priority for inputs from highest
//to lowest:
// 22, 41236, 623, 236, 63214, 421, 214.
//The 22 input is an exception and is always checked first as it requires the least computation.
int InputBuffer::getCommand()
{
  int tempTracker = tracker;
  int subTracker = tempTracker;
  int subStart = subTracker;
  int trackerStart = tempTracker;

  //[0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10] [11]
  //for test, tracker assumed to be at [0], so [11] is our current frame of input
  //we'll assume that we had a 236 input at the last possible frames, so 
  //[9] =  2
  //[10] = 3
  //[11] = 6
  //tracker will start at [9]
  if(tracker == 0)
  {
    tempTracker = INBUF_SIZE - 2;
  }
  else if(tracker == 1)
  {
    tempTracker = INBUF_SIZE - 1;
  }
  /*else if(tracker == 2)  //Commenting these out after changing buffer to start checking at second to
                            //to last input to account for 623 inputs
  {
    tempTracker = INBUF_SIZE - 1;
  }*/
  /*else if(tracker == 3)
  {
    tempTracker = INBUF_SIZE - 1;
  }*/
  else
  {
    tempTracker = tracker - 2;
  }
  trackerStart = tempTracker;
  for(int a = 0; a < INBUF_SIZE - 1; a++)
  {
    if(buffer[tempTracker] == 2)
    {
      if(tempTracker == INBUF_SIZE - 1)
      {
        subTracker = 0;
      }
      else
      {
        subTracker = tempTracker + 1;
      }
      //This while loops basically cycles through the buffer till one of two conditions are met:
      // 1.  A number that was input after the 2 is found that is NOT a 2
      // 2.  The entire buffer is checked
      while(buffer[subTracker] == 2 && subTracker != tempTracker && subTracker != tracker)
      {
        if(subTracker == INBUF_SIZE - 1)
        {
          subTracker = 0;
        }
        else
        {
          subTracker++;
        }
      }
      //Exited... either we found something new, or we've exhausted the list... find out!
      if(buffer[subTracker] == 5)
      {
        if(tempTracker == INBUF_SIZE - 1)
        {
          subTracker = 0;
        }
        else
        {
          subTracker = tempTracker + 1;
        }
        while(buffer[subTracker] == 5 && subTracker != tempTracker && subTracker != tracker)
        {
          if(subTracker == INBUF_SIZE - 1)
          {
            subTracker = 0;
          }
          else
          {
            subTracker++;
          }
        }
        if(buffer[subTracker] == 2)
        {
          //22 input
          return 7;
        }
      }
      else if(buffer[subTracker] == 3)
      {
        //This only happens in the event a new number was found.  If it searched the entire 
        //buffer, this will never trigger, it just gets skipped.
        //At this point, we know we've found a 2 followed by a 3.
        //Now to repeat the same steps to check for the rest of the input.  At this point however,
        //the input could be either a 236 OR a 623, so we're going to prioritize the 623 input
        //and check before the first two input and backwards for a 6.  We'll also check if there's a 
        //3 input, preceded by a 6, since this is a common misinput when doing 623 motions.

        //Record subTracker to redefault later in the event we don't find a 623 command.
        subStart = subTracker;
        //We're going to set subTracker to the slot before tempTracker, which marks where we found
        //the first 2 input.
        if(tempTracker == 0)  //if temp tracker is at the start of the array...
        {
          subTracker = INBUF_SIZE - 1;  //set subTracker to last slot in the array
        }
        else  //otherwise decrease it by one.
        {
          subTracker = tempTracker - 1;
        }
        //for loop to check for the 6 input...  The subTracker != temptracker clause might cause problems here later, 
        //so if we run into problems with the command interpreter missing DPs, this is probably to blame.  will
        //change if it becomes necessary, but I doubt it'll become an issue.
        while((buffer[subTracker] == 3 || buffer[subTracker] == 6 || buffer[subTracker] == 2 || buffer[subTracker] == 5) && subTracker != tempTracker && subTracker != tracker)
        {
          
          if(buffer[subTracker] == 6)
          {
            // SHORYUKEN!
            return 2;
          }
          if(subTracker == 0)
          {
            subTracker = INBUF_SIZE - 1;
          }
          else
          {
            subTracker--;
          }
        }
        if(buffer[subTracker] == 6)
        {
          //SHORYUKEN
          return 2;
        }
        





        //If it wasn't a dp input, proceed forward to check for 236 by redefaulting subTracker
        subTracker = subStart;

        if(subTracker == INBUF_SIZE - 1) //Increment subTracker.
        {
          subTracker = 0;
        }
        else
        {
          subTracker++;
        } 
        while(buffer[subTracker] == 3 && subTracker != tempTracker && subTracker != tracker)
        {
          if(subTracker == INBUF_SIZE - 1)
          {
            subTracker = 0;
          }
          else
          {
            subTracker++;
          }
        }
        //Now, again, we've found something new or exhausted the buffer.
        if(buffer[subTracker] == 6)
        {
          //Ok, so here we know that we have a 236 input, but what if the player did 41236? Here we'll work
          //backwards from the original 2 where tracker found the first input, and check backwards till we find
          //a non-2 number, check if it's 1, and proceed from there.  if any of the checks for the 1 and 4 fail,
          //it is assumed that the player entered a 236 motion.
          if(tempTracker == 0)
          {
            subTracker = INBUF_SIZE - 1;
          }
          else 
          {
            subTracker = tempTracker - 1;
          }
          while(buffer[subTracker] == 2 && subTracker != tempTracker && subTracker != tracker)
          {
            if(subTracker == 0)
            {
              subTracker = INBUF_SIZE - 1;
            }
            else
            {
              subTracker--;
            }
          }
          //We've now found a different number.  check if it's a 1...
          if(buffer[subTracker] == 1)
          {
            //We've found a 1! check for a 4.
            if(subTracker == 0)
            {
              subTracker = INBUF_SIZE - 1;
            }
            else 
            {
              subTracker--;
            }
            while(buffer[subTracker] == 1 && subTracker != tempTracker && subTracker != tracker)
            {
              if(subTracker == 0)
              {
                subTracker = INBUF_SIZE - 1;
              }
              else
              {
                subTracker--;
              }
            }
            //Found another number.  checking for a 4...
            if(buffer[subTracker] == 4)
            {
              return 3; //41236 input.
            }
          }
          //Either 1 or 4 not found in the right place, HADOUKEN!
          return 1;
        }

      }
   


    //check for 214s

    
      if(tempTracker == INBUF_SIZE - 1)
      {
        subTracker = 0;
      }
      else
      {
        subTracker = tempTracker + 1;
      }
      //This while loops basically cycles through the buffer till one of two conditions are met:
      // 1.  A number that was input after the 2 is found that is NOT a 2
      // 2.  The entire buffer is checked
      while(buffer[subTracker] == 2 && subTracker != tempTracker && subTracker != tracker)
      {
        if(subTracker == INBUF_SIZE - 1)
        {
          subTracker = 0;
        }
        else
        {
          subTracker++;
        }
      }
      //Exited... either we found something new, or we've exhausted the list... find out!
      if(buffer[subTracker] == 1)
      {
        //This only happens in the event a new number was found.  If it searched the entire 
        //buffer, this will never trigger, it just gets skipped.
        //At this point, we know we've found a 2 followed by a 1.
        //Now to repeat the same steps to check for the rest of the input.  At this point however,
        //the input could be either a 214 OR a 421, so we're going to prioritize the 421 input
        //and check before the first two input and backwards for a 4.  We'll also check if there's a 
        //1 input, preceded by a 4, since this is a common misinput when doing 421 motions.

        //Record subTracker to redefault later in the event we don't find a 421 command.
        subStart = subTracker;
        //We're going to set subTracker to the slot before tempTracker, which marks where we found
        //the first 2 input.
        if(tempTracker == 0)  //if temp tracker is at the start of the array...
        {
          subTracker = INBUF_SIZE - 1;  //set subTracker to last slot in the array
        }
        else  //otherwise decrease it by one.
        {
          subTracker = tempTracker - 1;
        }
        //for loop to check for the 4 input...  The subTracker != temptracker clause might cause problems here later, 
        //so if we run into problems with the command interpreter missing DPs, this is probably to blame.  will
        //change if it becomes necessary, but I doubt it'll become an issue.
        while((buffer[subTracker] == 1 || buffer[subTracker] == 4 || buffer[subTracker] == 2) && subTracker != tempTracker && subTracker != tracker)
        {
          
          if(buffer[subTracker] == 4)
          {
            // REVERSE SHORYUKEN!
            return 5;
          }
          if(subTracker == 0)
          {
            subTracker = INBUF_SIZE - 1;
          }
          else
          {
            subTracker--;
          }
        }
        if(buffer[subTracker] == 4)
        {
          //REVERSE SHORYUKEN (this is necessary since the previous check doesn't check the last input stored in the buffer
          return 5;
        }





        //If it wasn't a dp input, proceed forward to check for 214 by redefaulting subTracker
        subTracker = subStart;

        if(subTracker == INBUF_SIZE - 1) //Increment subTracker.
        {
          subTracker = 0;
        }
        else
        {
          subTracker++;
        } 
        while(buffer[subTracker] == 1 && subTracker != tempTracker && subTracker != tracker)
        {
          if(subTracker == INBUF_SIZE - 1)
          {
            subTracker = 0;
          }
          else
          {
            subTracker++;
          }
        }
        //Now, again, we've found something new or exhausted the buffer.
        if(buffer[subTracker] == 4)
        {
          //Ok, so here we know that we have a 214 input, but what if the player did 63214? Here we'll work
          //backwards from the original 2 where tracker found the first input, and check backwards till we find
          //a non-2 number, check if it's 1, and proceed from there.  if any of the checks for the 1 and 4 fail,
          //it is assumed that the player entered a 214 motion.
          if(tempTracker == 0)
          {
            subTracker = INBUF_SIZE - 1;
          }
          else 
          {
            subTracker = tempTracker - 1;
          }
          while(buffer[subTracker] == 2 && subTracker != tempTracker && subTracker != tracker)
          {
            if(subTracker == 0)
            {
              subTracker = INBUF_SIZE - 1;
            }
            else
            {
              subTracker--;
            }
          }
          //We've now found a different number.  check if it's a 3...
          if(buffer[subTracker] == 3)
          {
            //We've found a 3! check for a 6.
            if(subTracker == 0)
            {
              subTracker = INBUF_SIZE - 1;
            }
            else 
            {
              subTracker--;
            }
            while(buffer[subTracker] == 3 && subTracker != tempTracker && subTracker != tracker)
            {
              if(subTracker == 0)
              {
                subTracker = INBUF_SIZE - 1;
              }
              else
              {
                subTracker--;
              }
            }
            //Found another number.  checking for a 6...
            if(buffer[subTracker] == 6)
            {
              return 6; //63214 input.
            }
          }
          //Either 1 or 4 not found in the right place, HADOUKEN!
          return 4;
        }

      }
    }

    //WEE


    if(tempTracker == 0)
    {
      tempTracker = INBUF_SIZE;
    }
    else
      tempTracker--;
  }
  return 0;
}

/*
void interpretMoves(int buff[])
{
	// [0] [1] [2] [3] [4] [5] [6] [7]
	if(buff[6] == 2 && buff[7] == 2)	//when we do this for real, this needs to start at the same slot as
	{			//the other checks, as it has to read 252 to recognize two presses of down.
		cout << "SHANKING TIME" << endl;
	}
	for(int a = 5; a >= 0; a--)
	{
		if(buff[a] == 6)
		{
			if(buff[a+1] == 2)
			{
				if(buff[a+2] == 3)
				{
					cout << "SHORYUKEN!" << endl;
					break;
				}
			}
			else if(a+3 < 8 && buff[a+2] == 2 && buff[a+1] == 3)
			{
				if(buff[a+3] == 3)
				{
					cout << "SHORYUKEN!" << endl;
					break;
				}
			}
		}
		else if(buff[a] == 4)
		{
			if(buff[a+1] == 2)
			{
				if(buff[a+2] == 1)
				{
					cout << "REVERSE SHORYUKEN!" << endl;
					break;
				}
			}
			else if(a+3 < 8 && buff[a+2] == 2 && buff[a+1] == 1)
			{
				if(buff[a+3] == 1)
				{
					cout << "REVERSE SHORYUKEN!" << endl;
					break;
				}
			}
		}
		else if(buff[a] == 2)
		{
			if(buff[a+1] == 3)
			{
				if((a-1 >= 0 && buff[a-1] == 6) || (a-2 >0 && buff[a-2] == 6 && buff[a-1] == 3))
				{
					cout << "SHORYUKEN!" << endl;
					break;
				}
				if(buff[a+2] == 6)
				{
					cout << "HADOUKEN!" << endl;
					break;
				}
				
				
			}
			else if(buff[a+1] == 1)
			{
				if((a-1 >= 0 && buff[a-1] == 4) || (a-2 >0 && buff[a-2] == 4 && buff[a-1] == 1))
				{
					cout << "REVERSE SHORYUKEN!" << endl;
					break;
				}
				if(buff[a+2] == 4)
				{
					cout << "TATSU!" << endl;
					break;
				}
			}
		}
	}
	for(int a = 0; a < 8; a++)
	{
		cmdBuffer.pop();
		cmdBuffer.push(5);
	}
}
*/