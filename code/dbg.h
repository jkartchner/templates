#ifndef __dbg_h__
#define __dbg_h__

#include "txtlib.h"

#define Assert(Expression) if(!(Expression)) { *(int *)0 = 0; }

#endif
