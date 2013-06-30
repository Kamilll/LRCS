#ifndef CURSOR_H
#define CURSOR_H

//Abstract Class to define a cursor
#include "../common/Constants.h"

class Cursor{
	public:
		Cursor();

		virtual ~Cursor()=0;	

		virtual	unsigned int getNext()=0;

		virtual void setCurrStartPosition()=0;

		virtual unsigned int getCurrStartPosition()=0;

		virtual unsigned int getCurrPosition()=0;

        virtual void resetCursor()=0;

		
	protected:



};

#endif // CURSOR_H
