/* s_power.c
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */
#include "sim.h"
#include <string.h>

#ifndef NO_FAST_POWERSCAN		/* Amiga port: -DNO_FAST_POWERSCAN builds the original DoPowerScan (to compare) */
#define AMIGA_FAST_POWERSCAN
#endif


/* Power Scan */


int PowerStackNum;
char PowerStackX[PWRSTKSIZE], PowerStackY[PWRSTKSIZE];
QUAD MaxPower, NumPower;


/* comefrom: TestForCond DoPowerScan TryGo */
MoveMapSim (short MDir)
{
  switch (MDir) {
  case 0:
    if (SMapY > 0) {
      SMapY--;
      return (TRUE);
    }
    if (SMapY < 0)
      SMapY = 0;
    return (FALSE);
  case 1:
    if (SMapX < (WORLD_X - 1)) {
      SMapX++;
      return (TRUE);
    }
    if (SMapX > (WORLD_X - 1))
      SMapX = WORLD_X - 1;
    return (FALSE);
  case 2:
    if (SMapY < (WORLD_Y - 1)) {
      SMapY++;
      return (TRUE);
    }
    if (SMapY > (WORLD_Y - 1))
      SMapY = WORLD_Y - 1;
    return (FALSE);
  case 3:
    if (SMapX > 0) {
      SMapX--;
      return (TRUE);
    }
    if (SMapX < 0)
      SMapX = 0;
    return (FALSE);
  case 4:
    return (TRUE);
  }
  return (FALSE);				
}


/* inlined -Don */
#if 0
/* comefrom: DoPowerScan */
SetPowerBit(void) 
{
  register int PowerWrd;

  /* XXX: assumes 120x100 */
  PowerWrd = (SMapX >>4) + (SMapY <<3);
  PowerMap[PowerWrd] |= 1 << (SMapX & 15);
}
#endif


/* inlined */
#if 0
/* comefrom: TestForCond */
int TestPowerBit(void)
{
  register int PowerWrd;

  if ((CChr9 == NUCLEAR) || (CChr9 == POWERPLANT))
    return(1);
  /* XXX: assumes 120x100 */
  PowerWrd = (SMapX >>4) + (SMapY <<3);
  if (PowerWrd >= PWRMAPSIZE)
    return(0);
  return ((PowerMap[PowerWrd] & (1 << (SMapX & 15))) ? 1 : 0);
}
#endif


/* comefrom: DoPowerScan */
short TestForCond(short TFDir)
{ 	
  register int xsave, ysave, PowerWrd;

  xsave = SMapX;
  ysave = SMapY;
  if (MoveMapSim(TFDir)) {
    /* TestPowerBit was taking a lot of time so I swapped the & order -Don */
#if 0
    if ((Map[SMapX][SMapY] & CONDBIT) &&
	(!TestPowerBit()))
#else
    if ((Map[SMapX][SMapY] & CONDBIT) &&
	(CChr9 != NUCLEAR) &&
	(CChr9 != POWERPLANT) &&
	(
#if 0
	 (PowerWrd = (SMapX >>4) + (SMapY <<3)),
#else
	 (PowerWrd = POWERWORD(SMapX, SMapY)),
#endif
	 ((PowerWrd > PWRMAPSIZE) ||
	  ((PowerMap[PowerWrd] & (1 << (SMapX & 15))) == 0))))
#endif
    {
      SMapX = xsave;
      SMapY = ysave;
      return (TRUE);
    }
  }
  SMapX = xsave;
  SMapY = ysave;
  return (FALSE);
}


/* comefrom: Simulate SpecialInit InitSimMemory */
#ifdef AMIGA_FAST_POWERSCAN
/* Amiga port: the same flood fill as the original below, step by step in the same order with the same results, but
 * without the function calls per neighbour (MoveMapSim + TestForCond moved the global SMapX/SMapY back and forth for every
 * test). On a 68030 the original took about 0.6 s for a big city, and the picture stood still meanwhile. */
static unsigned char pw_state[WORLD_X * WORLD_Y];	/* per field: 0 = does not conduct, 1 = conducts, no power yet, 2 = has power */

static void pw_bits(void)			/* PowerMap from pw_state: the same bits SETPOWERBIT set for every field reached */
{
  register int x, y;
  for (x = 0; x < WORLD_X; x++) {
    register unsigned char *c = pw_state + x * WORLD_Y;
    register int bit = 1 << (x & 15);
    for (y = 0; y < WORLD_Y; y++)
      if (c[y] == 2)
	PowerMap[POWERWORD(x, y)] |= bit;
  }
}

DoPowerScan()
{
  register unsigned char *st = pw_state;
  register int x, y, at, i;
  register QUAD num = 0, maxp;
  short ADir;
  int ConNum;
  int cond_ok = (CChr9 != NUCLEAR) && (CChr9 != POWERPLANT);	/* TestForCond reads CChr9, which the scan never changes */

  for (i = 0; i < PWRMAPSIZE; i++)
    PowerMap[i] = 0;	/* ClearPowerMem */
  if (cond_ok) {				/* conducting fields; when CChr9 is a power plant TestForCond is never true */
    register short *map = Map[0];		/* the map is one block, column by column: field (x, y) is map[x * WORLD_Y + y] */
    for (i = 0; i < WORLD_X * WORLD_Y; i++)
      st[i] = (map[i] & CONDBIT) ? 1 : 0;
  } else
    memset(st, 0, sizeof pw_state);

  MaxPower = maxp = (CoalPop * 700L) + (NuclearPop * 2000L); /* post release */
  NumPower = 0;

  x = SMapX; y = SMapY; at = x * WORLD_Y + y;
  while (PowerStackNum)	{
    if (PowerStackNum > 0) {		/* PullPowerStack */
      x = PowerStackX[PowerStackNum];
      y = PowerStackY[PowerStackNum];
      PowerStackNum--;
      at = x * WORLD_Y + y;
    }
    ADir = 4;
    do {
      if (++num > maxp) {
	NumPower = num;
	pw_bits();
	SMapX = x; SMapY = y;
	SendMes(40);
	return;
      }
      switch (ADir) {			/* MoveMapSim(ADir): a direction that TestForCond found open */
      case 0: if (y > 0) { y--; at--; } else if (y < 0) y = 0; break;
      case 1: if (x < WORLD_X - 1) { x++; at += WORLD_Y; } else if (x > WORLD_X - 1) x = WORLD_X - 1; break;
      case 2: if (y < WORLD_Y - 1) { y++; at++; } else if (y > WORLD_Y - 1) y = WORLD_Y - 1; break;
      case 3: if (x > 0) { x--; at -= WORLD_Y; } else if (x < 0) x = 0; break;
      }
      st[at] = 2;			/* SETPOWERBIT */
      ConNum = 0;			/* TestForCond(Dir) for Dir 0..3 until two are open: conducting and no power yet */
      if (y > 0 && st[at - 1] == 1) { ConNum++; ADir = 0; }
      if (x < WORLD_X - 1 && st[at + WORLD_Y] == 1) { ConNum++; ADir = 1; }
      if (ConNum < 2 && y < WORLD_Y - 1 && st[at + 1] == 1) { ConNum++; ADir = 2; }
      if (ConNum < 2 && x > 0 && st[at - WORLD_Y] == 1) { ConNum++; ADir = 3; }
      if (ConNum > 1) {			/* PushPowerStack */
	if (PowerStackNum < (PWRSTKSIZE - 2)) {
	  PowerStackNum++;
	  PowerStackX[PowerStackNum] = x;
	  PowerStackY[PowerStackNum] = y;
	}
      }
    } while (ConNum);
  }
  NumPower = num;
  pw_bits();
  SMapX = x; SMapY = y;
}
#else
DoPowerScan()
{	
  short ADir;
  register int ConNum, Dir, x;

  for (x = 0; x < PWRMAPSIZE; x++)
    PowerMap[x] = 0;	/* ClearPowerMem */

  MaxPower = (CoalPop * 700L) + (NuclearPop * 2000L); /* post release */
  NumPower = 0;

  while (PowerStackNum)	{
    PullPowerStack();
    ADir = 4;
    do {
      if (++NumPower > MaxPower) {
	SendMes(40);
	return;
      }
      MoveMapSim(ADir);
/* inlined -Don */
#if 0
      SetPowerBit();
#else
#if 1
      SETPOWERBIT(SMapX, SMapY);
#else
      PowerMap[(SMapX >>4) + (SMapY <<3)] |= 1 << (SMapX & 15);
#endif
#endif
      ConNum = 0;
      Dir = 0;
      while ((Dir < 4) && (ConNum < 2))	{
	if (TestForCond(Dir)) {
	  ConNum++;
	  ADir = Dir;
	}
	Dir++;
      }
      if (ConNum > 1)
	PushPowerStack();
    } while (ConNum);
  }
}


#endif


/* comefrom: DoPowerScan DoSPZone */
PushPowerStack(void) 
{ 	
  if (PowerStackNum < (PWRSTKSIZE - 2)) {
    PowerStackNum++;
    PowerStackX[PowerStackNum] = SMapX;
    PowerStackY[PowerStackNum] = SMapY;
   }
}


/* comefrom: DoPowerScan */
PullPowerStack(void)
{ 	
  if (PowerStackNum > 0)  {
    SMapX = PowerStackX[PowerStackNum];
    SMapY = PowerStackY[PowerStackNum];
    PowerStackNum--;
  }
}


