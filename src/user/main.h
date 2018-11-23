#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f10x.h"

int main(void);

/*release the grab*/
void grabRelease(void);

/*grab things in front of it*/
void grabStart(void);

/*grab will up*/
void grabUp(void);

/*grab will down*/
void grabDown(void);

/*start to throw the ball*/
void throwSet(void);

/*reset the throw part*/
void throwReet(void);



#endif	/* __MAIN_H */



