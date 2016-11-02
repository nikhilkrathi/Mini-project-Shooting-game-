/*  Copyright (C) 2016 Nikhil Rathi <rathin15.it@coep.ac.in/>
 
    This file is part of SpaceAce.
    SpaceAce is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    Nikhil Rathi, either version 3 of the License, or
    (at your option) any later version.
    
    SpaceAce is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>. 
   
*/


struct player {
	int r,c;
	char ch;
};

struct alien {
	int r,c;
	int pr,pc;
	int alive; /* 1=alive 0=destroyed */
	char direction; /* 'u'= up 'd'= down */
	char ch;
};

struct shoot {
	int r,c;
	int active; /* 1=active 0=inactive */
	char ch;
};

struct bomb {
	int r,c;
	int active; /* 1=active 0=inactive */
	int loop;
	char ch;
};

struct options {
	int overall,alien,shots,bombs,bombchance;
	double speed;
};


struct entry {
   	char name[128];
   	int score;
};
