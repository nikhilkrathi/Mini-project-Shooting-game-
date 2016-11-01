/*  Copyright (C) 2016 Nikhil Rathi  
    This file is part of SpaceAce.

    SpaceAce is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    Nikhil Rathi, either version 3 of the License, or
    (at your option) any later version.

    SpaceAce is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    For a copy of the GNU General Public License ,
    see <http://www.gnu.org/licenses/>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_BOMBS 100000


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

void gameover(int win, int score);
void highscores();
int aboutus();
void exitgame();

WINDOW *w, *w2, *w3;

int menu() {
	initscr();
	noecho(); 
	curs_set( 0 );
	
	//Creating UI window.
	
	char list[4][50] = { "PLAY GAME", "HIGH SCORES", "ABOUT GAME", "EXIT"};
	char item[50];
	int ch, i = 0;
	char line[5][100] = {
	"|-------   |-------|   |-------|   |-------   |-------         |-------|   |-------   |-------",
	"|          |       |   |       |   |          |                |       |   |          |       ",
	"|------|   |-------|   |-------|   |          |-------   ---   |-------|   |          |-------",
	"       |   |           |       |   |          |                |       |   |          |       ", 
	"-------|   |           |       |   |-------   |-------         |       |   |-------   |-------"};
	char lineitem[100];

	//WINDOW 3
	
	w3 = newwin(40, 110, 8, (COLS - 110) / 2);
	box(w3, 0, 0);
	wrefresh(w3);
	
	//WINDOW 2
	
	int j = 0;
	w2 = newwin(35, 100, 10, (COLS - 100) / 2);
	for(j = 0; j<5; j++) {
		sprintf(lineitem, "%s", line[j]);
	    mvwprintw( w2, j+1, 2, "%s", lineitem);
	}
	wrefresh( w2);   
		
	//WINDOW 1
	
	w = newwin(15, 25, (LINES - 15) / 2, (COLS - 25) / 2 ); 
	box( w, 0, 0 ); 
	for( i=0; i<4; i++) {
		if( i == 0 ) 
			wattron( w, A_STANDOUT ); 
		else
			wattroff( w, A_STANDOUT );
			
		sprintf(item, "%s",  list[i]);
		mvwprintw( w, i+1, 2, "%s", item );
	}
	wrefresh(w);  
	
	i = 0;
	keypad( w, TRUE );
	while((ch = wgetch(w))) { 
    	sprintf(item, "%-7s",  list[i]); 
   		mvwprintw( w, i+1, 2, "%s", item ); 
        switch( ch ) {
            case KEY_UP:
               	if(i == 0)
              	    i = 3;
                else 
                    i--;
                         
                break;
                            
            case KEY_DOWN:
                if(i == 3)
                    i = 0;
                           
                else
                    i++;
                        
                break; 
                	
            case 10:
	       		return i;
       			break;
        }
            
        wattron( w, A_STANDOUT );
        sprintf(item, "%-7s",  list[i]);
        mvwprintw( w, i+1, 2, "%s", item);
        wattroff( w, A_STANDOUT ); 
	}
}

void playgame() {
	struct player tank[15];
	struct alien aliens[30];
	struct shoot shot[10];
	struct bomb bomb[MAX_BOMBS];
	struct options settings;
	unsigned int loops=0, i=0, k=0, j=0, currentshots=0, currentbombs=0, currentaliens=30;
	int input, random=0, score=0, win=-1, ch;
	char tellscore[30];
	WINDOW *w;
	    	   
	clear();
	noecho();
	curs_set(FALSE);
	cbreak();
	keypad(stdscr, TRUE );
	nodelay(stdscr,1);
	srand(time(NULL));
	
	/* Set default settings */
	//This sets the default setting that is speed of alien and freq. of bombs etc.
	
	settings.overall = 3000;
	settings.alien = 12;
	settings.shots = 1;
	settings.bombs = 10;
	settings.bombchance = 5;
	settings.speed = 1.5;
	
	char list[4][50] = {"EASY", "MEDIUM", "HARD", "IMPOSSIBLE"};
	char item[50];
	
    //levels Window
    
	w = newwin(15, 25, (LINES - 15) / 2, (COLS - 25) / 2 ); 
	box( w, 0, 0 ); 
	for( i=0; i<4; i++) {
		if( i == 0 ) 
			wattron( w, A_STANDOUT ); 
		else
			wattroff( w, A_STANDOUT );
			
		sprintf(item, "%s",  list[i]);
		mvwprintw( w, i+1, 2, "%s", item );
	}
	wrefresh(w);
	
	i = 0;
	keypad( w, TRUE );
	while((ch = wgetch(w)) != 10) { 
    	sprintf(item, "%-7s",  list[i]); 
    	mvwprintw( w, i+1, 2, "%s", item ); 
        	switch( ch ) {
                case KEY_UP:
                    if(i == 0)
              	    	i = 3;
                    else 
                        i--;
                         
                    break;
                            
                case KEY_DOWN:
                	if(i == 3)
                    	i = 0;
                           
                    else
                        i++;
                        
                	break; 
     
            }
          
            wattron( w, A_STANDOUT );
            sprintf(item, "%-7s",  list[i]);
            mvwprintw( w, i+1, 2, "%s", item);
            wattroff( w, A_STANDOUT );
             
      }
      
	if(ch == 10) {
    	if(i == 0) {
			settings.speed = 2.5;
			settings.overall = 6500;
		}
		
		else if(i == 1) {
			settings.speed = 2;
			settings.overall = 5500;
		}
		
		else if(i == 2) {
			settings.speed = 1.5;
			settings.overall = 4500;
		}
		
		else if(i == 3) {
			settings.speed = 0.5;
			settings.overall = 3500;
		}
		
		delwin(w);
		clear();
	}
      
	//Creating tank

	for(i=0; i<3; i++) {
   		tank[i].r = LINES/2 - 2;
   		tank[i].c = (COLS - i*3) - 1;
   		if(i == 2)
   			tank[i].ch = ' ';
   		else
   			tank[i].ch = '+';
   	} 
   			
   	for(i=3; i<6; i++) {
   		tank[i].r = LINES/2 - 1;
   		tank[i].c = (COLS - (i-3)*3) - 1;
   		if(i == 5)
   			tank[i].ch = ' ';
   		else
   			tank[i].ch = '|';
   	} 
   			
   	for(i=6; i<9; i++) {
   		tank[i].r = LINES/2;
   		tank[i].c = (COLS - (i-6)*3) - 1;
   		if(i == 8)
   			tank[i].ch = '<';
   		else if(i == 7)
   			tank[i].ch = ' ';
   		else
   			tank[i].ch = '|';
   	}
   		
   	for(i=9; i<12; i++) {
   		tank[i].r = LINES/2 + 1;
   		tank[i].c = (COLS - (i-9)*3) - 1;
   		if(i == 11)
   			tank[i].ch = ' ';
   		else
   			tank[i].ch = '|';
   	} 
   		
   	for(i=12; i<15; i++) {
   		tank[i].r = LINES/2 + 2;
   		tank[i].c = (COLS - (i-12)*3) - 1;
   		if(i == 14)
   			tank[i].ch = ' ';
   		else
   			tank[i].ch = '+';
   	} 
   
   	//Creating aliens
	
	for (i=0; i<3; ++i) {
    	aliens[i].r = 5;
    	aliens[i].c = i*3;
    	aliens[i].pr = 0;
    	aliens[i].pc = 0;
     	aliens[i].ch = '#';
      	aliens[i].alive = 1;
      	aliens[i].direction = 'd';
   	}
   	for (i=3; i<6; ++i) {
      	aliens[i].r = 7;
      	aliens[i].c = (i-3)*3;
      	aliens[i].pr = 0;
      	aliens[i].pc = 0;
      	aliens[i].ch = '#';
      	aliens[i].alive = 1;
      	aliens[i].direction = 'd';
   	}
   	for (i=6; i<9; ++i) {
      	aliens[i].r = 9;
      	aliens[i].c = (i-6)*3;
      	aliens[i].pr = 0;
      	aliens[i].pc = 0;
      	aliens[i].ch = '#';
      	aliens[i].alive = 1;
      	aliens[i].direction = 'd';
   	}
	for (i=9; i<12; ++i) {
    	aliens[i].r = 11;
    	aliens[i].c = (i-9)*3;
    	aliens[i].pr = 0;
    	aliens[i].pc = 0;
     	aliens[i].ch = '#';
      	aliens[i].alive = 1;
      	aliens[i].direction = 'd';
   	}
   	for (i=12; i<15; ++i) {
      	aliens[i].r = 13;
      	aliens[i].c = (i-12)*3;
      	aliens[i].pr = 0;
      	aliens[i].pc = 0;
      	aliens[i].ch = '#';
      	aliens[i].alive = 1;
      	aliens[i].direction = 'd';
   	}
   	for (i=15; i<18; ++i) {
      	aliens[i].r = 15;
      	aliens[i].c = (i-15)*3;
      	aliens[i].pr = 0;
      	aliens[i].pc = 0;
      	aliens[i].ch = '#';
      	aliens[i].alive = 1;
      	aliens[i].direction = 'd';
   	}
	for (i=18; i<21; ++i) {
    	aliens[i].r = 17;
    	aliens[i].c = (i-18)*3;
    	aliens[i].pr = 0;
    	aliens[i].pc = 0;
     	aliens[i].ch = '#';
      	aliens[i].alive = 1;
      	aliens[i].direction = 'd';
   	}
   	for (i=21; i<24; ++i) {
      	aliens[i].r = 19;
      	aliens[i].c = (i-21)*3;
      	aliens[i].pr = 0;
      	aliens[i].pc = 0;
      	aliens[i].ch = '#';
      	aliens[i].alive = 1;
      	aliens[i].direction = 'd';
   	}
   	for (i=24; i<27; ++i) {
      	aliens[i].r = 21;
      	aliens[i].c = (i-24)*3;
      	aliens[i].pr = 0;
      	aliens[i].pc = 0;
      	aliens[i].ch = '#';
      	aliens[i].alive = 1;
      	aliens[i].direction = 'd';
   	}
   	for (i=27; i<30; ++i) {
      	aliens[i].r = 23;
      	aliens[i].c = (i-27)*3;
      	aliens[i].pr = 0;
      	aliens[i].pc = 0;
      	aliens[i].ch = '#';
      	aliens[i].alive = 1;
      	aliens[i].direction = 'd';
   	}
   	
	// Set shot settings  
	
	for (k=0; k<10; k++) {
	    shot[k].active = 0;
	    shot[k].ch = '*';
	}
	
    // Set bomb settings 
    
    for (i=0; i<MAX_BOMBS; ++i) {
      	bomb[i].active = 0;
      	bomb[i].ch = 'O';
      	bomb[i].loop = 0;
    }
	
	// Display game title,score header,options 
	
    move(0,(COLS/2)-9);
    addstr("***--SPACE-ACE--***");
    move (0,1);
    addstr("SCORE: ");
    move(0,COLS-9);
	addstr("q = quit");
	
	while(1) {
	
      	// Show score 
      	
      	sprintf(tellscore, "%d", score);
      	move(0,8);
      	addstr(tellscore);

		//Move tank
	
		for(i=0; i<15; i++) {
			move(tank[i].r, tank[i].c);
			addch(tank[i].ch);
		
		}
		
		// Move bombs
		                                 
		if (loops % settings.bombs == 0)                  
			for (i=0; i<MAX_BOMBS; i++) {
		  		if (bomb[i].active == 1) {
		        	if (bomb[i].c < COLS) {
		        		if (bomb[i].loop != 0) {
							move(bomb[i].r,bomb[i].c-1);
		              		addch(' ');
		           		}
		           		else
		              		++bomb[i].loop;
		            
		           	move(bomb[i].r,bomb[i].c);
		           	addch(bomb[i].ch);
		           
		           	++bomb[i].c;
		        	}
		        	else {
		           		bomb[i].active = 0;
		           		bomb[i].loop = 0;
		           		--currentbombs;
		           		move(bomb[i].r,bomb[i].c - 1);
		           		addch(' ');
		        	}
		     	}
		  	}
		
		// Move shots
		
		if (loops % settings.shots == 0)
			for (k=0; k<10; k++) {
				if (shot[k].active == 1) {
					if (shot[k].c > 0) {
						if (shot[k].c < COLS - 2) {
							move(shot[k].r ,shot[k].c + 1);
							addch(' ');
						}
					
						for (j=0; j<30; ++j) {
                  			if (aliens[j].alive == 1 && aliens[j].pr == shot[k].r && aliens[j].c == shot[k].c) {
					     		score += 20;
								aliens[j].alive = 0;
								shot[k].active = 0;
								--currentshots;
								--currentaliens;
								move(aliens[j].pr,aliens[j].pc);
								addch(' ');
					     		break;
					 		}
				        }
				       
				       	for(j=0; j<MAX_BOMBS; j++) {
				       		if(bomb[j].active == 1 && bomb[j].r == shot[k].r && bomb[j].c == shot[k].c) {
				       	 		score += 5;
				       	 		bomb[j].active = 0;
				       	 		shot[k].active = 0;
				       	 		--currentshots;
				       	 		move(bomb[j].r, bomb[j].c - 1);
				       	 		addch(' ');
				       	 		break;
				       	 	}
				       	}
				       
						if (shot[k].active == 1) {
							move(shot[k].r,shot[k].c);
							addch(shot[k].ch);
							--shot[k].c;
						}
					}
				
					else {
						shot[k].active = 0;
						--currentshots;
						move(shot[k].r ,shot[k].c + 1);
						addch(' ');
					}
				}
			}
		
        // Move aliens 
      
		if (loops % settings.alien == 0)
			for (i=0; i<30; i++) {
				if (aliens[i].alive == 1) {
					move(aliens[i].pr,aliens[i].pc);
				    addch(' ');
				    
				    move(aliens[i].r,aliens[i].c);
				    addch(aliens[i].ch);
				    
				    aliens[i].pr = aliens[i].r;
				    aliens[i].pc = aliens[i].c;
				    
					// Check if alien should drop bomb
					 
				    random = 1+(rand()%100);
				    if ((settings.bombchance - random * settings.speed) >= 0 && currentbombs < MAX_BOMBS) {
				    	for (j=0; j<MAX_BOMBS; j++) {
				        	if (bomb[j].active == 0) {
				            	bomb[j].active = 1;
				            	++currentbombs;
				            	bomb[j].r = aliens[i].r;
				            	bomb[j].c = aliens[i].c + 1;
				            	break;
				          	}	
				       	}
				    }
				    
					// Set alien's next position 
					
				    if (aliens[i].direction == 'u')
				    	--aliens[i].r;
				    else if (aliens[i].direction == 'd')
				    	++aliens[i].r;
				       
				    // Check alien's next positions 
				    
				    if (aliens[i].r == LINES - 3) {
				    	aliens[i].c = aliens[i]. c + 6;
				    	aliens[i].direction = 'u';
				    }
				    else if (aliens[i].r == 4) {
				    	aliens[i].c = aliens[i]. c + 6;
				    	aliens[i].direction = 'd';
				    }
		     	}
		  	}
		  	
		//Game won or lost
		  	
		if (currentaliens == 0) {
			win = 1;
		    break;
		}
		
		for (i=0; i<30; ++i) {
		    if (aliens[i].r == LINES-1) {
		    	win = 0;
		        break;
		    }
		}
		
		for (i=0; i<MAX_BOMBS; ++i) {
			for(j=0; j<15; j++) {	
				if (bomb[i].r == tank[j].r && bomb[i].c == tank[j].c) {
		    		win = 0;
		        	break;
		        }
		   	}
		} 	
						
		refresh();
		usleep(settings.overall);
		loops++;
	   	
		input = getch();	
		
		for(i=0; i<15; i++) {
			move(tank[i].r,tank[i].c);
		  	addch(' ');
		} 
						
		//Check input
	
		if(input == 'q')
			win = 2;
			 
		else if (input == KEY_UP)
			for(i=0; i<15; i++) 
			tank[i].r--;			
		
	   	else if (input == KEY_DOWN)
	   		for(i=0; i<15; i++)
	   			tank[i].r++;
	   		
		else if (input == ' ' && currentshots < 10) {
			for (k=0; k<10; k++) {
				if (shot[k].active == 0) {
					shot[k].active = 1;
					++currentshots;
					--score;
					shot[k].r = tank[8].r;
					shot[k].c = COLS - 8;
					break;
				}
			}
		}
	
		if (win != -1)
		    break;			   

		// Check for valid tank position 
	   	
	   	if (tank[13].r > LINES-1) {
	   		for(i=12; i<15; i++)
		    	tank[i].r = LINES - 1;
			for(i=9; i<12; i++)
				tank[i].r = LINES - 2;
			for(i=6; i<9; i++) 
				tank[i].r = LINES - 3;
			for(i=3; i<6; i++) 
				tank[i].r = LINES - 4;
			for(i=0; i<3; i++) 
				tank[i].r = LINES - 5; 
		}


	   	if (tank[0].r < 2) {
	   		for(i=0; i<3; i++)
		    	tank[i].r = 2;
			for(i=3; i<6; i++)
				tank[i].r = 3;
			for(i=6; i<9; i++) 
				tank[i].r = 4;
			for(i=9; i<12; i++) 
				tank[i].r = 5;
			for(i=12; i<15; i++) 
				tank[i].r = 6; 
		} 
        
	}
	
	gameover(win, score);
    endwin();
				   
}

// This function handles displaying the win/lose screen 

void gameover(int win, int score) {
	struct entry list;
    unsigned int input;
    int x;
    char a[10];
    char name[128];	
    FILE *fp;
    nodelay(stdscr, 0);
    curs_set(TRUE);
    sprintf(a, "%d", score);
   
    x = strlen(a);
   
    if (win == 0) {
    	clear();
    	move((LINES/2)-4, (COLS/2)-4);
        addstr("SCORE: ");
        move((LINES/2)-4, (COLS/2)+6-x);
        addstr(a);
        move((LINES/2)-1, (COLS/2)-5);
        addstr("GAME OVER!");
        move((LINES/2),(COLS/2)-7);
        addstr("PRESS ANY KEY");
        move(0,COLS-1);
        refresh();
        input = getch();
    }
   
    else if (win == 1) {
        clear();
        move((LINES/2)-4, (COLS/2)-x);
        addstr(a);
        move((LINES/2)-1,(COLS/2)-5);
        addstr("YOU WIN!");
        move((LINES/2),(COLS/2)-7);
        addstr("PRESS ANY KEY");
        move(0,COLS-1);
        refresh();
        input = getch();
    }
   
    if(input != 'q') {
	    clear();
	    echo();
	    nocbreak();
	    move((LINES/2)-2, (COLS/2)-9);
	    addstr("ENTER YOUR NAME: ");
	    refresh();
	    getstr(list.name);
    }
   
    //Creating a file and writing scores in it
   
    list.score = score;
    fp = fopen("score.txt", "a");
    fprintf(fp, "%s %d", list.name, list.score);
    fprintf(fp,"\n");
    fclose(fp);
}

void highscores () {
	struct entry b[128];
	
	int fd, i = 0, j, line = 0, c, d, swap, p, x;
	char ch, a[10], m[10], n[10];
	FILE *fw, *fp;
	char name[128];
	
	fw = fopen("score.txt", "a");
	for(j=0; j<5; j++) 
		fprintf(fw, "%s %d\n", "---", 0);
		
	fclose(fw);
		
	fd = open("score.txt", O_RDONLY, S_IRUSR);
	if(fd == -1) {
		perror("cp: can't open file");
		exit(errno);
	}
	
	//Read data from score.txt and get total no. of lines i.e total no. of structures
	  
	while(read(fd, &ch, sizeof(ch))) {
		if(ch == '\n')
			line++;
	}
	
	close(fd);	

	fp = fopen("score.txt", "r+");
	while(i != line) {
		fscanf(fp, "%s %d", b[i].name, &b[i].score);
		i++;
	}  
	
	fclose(fp);
	
	//Sort according to score with corr. player name
	
	for(c = 0; c < (line - 1); c++) {
		for(d = 0; d < (line - c - 1); d++) {
			if(b[d].score > b[d+1].score) {
				swap = b[d].score;
				b[d].score = b[d+1].score;
				b[d+1].score = swap;
				
				strcpy(name, b[d].name);
				strcpy(b[d].name, b[d+1].name);
				strcpy(b[d+1].name, name);
			}
		}
	}
				  	
   	clear();
   	sprintf(a, "%d", b[i - 1].score);
   	sprintf(m, "%d", b[i - 2].score);
   	sprintf(n, "%d", b[i - 3].score);
   	move((LINES/2 - 3), (COLS/2) - 11);
   	addstr("------TOP SCORERS------");
   	
   	move((LINES/2), (COLS/2 - 10));
   	addstr("1) NAME : ");
    	move((LINES/2), (COLS/2));
   	addstr(b[i - 1].name);
   	move((LINES/2 + 1), (COLS/2 - 7));
   	addstr("SCORE : ");
   	move((LINES/2 + 1), (COLS/2));
   	addstr(a);
   	
   	move((LINES/2 + 2), (COLS/2 - 10));
   	addstr("2) NAME : ");
    	move((LINES/2 + 2), (COLS/2));
   	addstr(b[i - 2].name);
   	move((LINES/2 + 3), (COLS/2 - 7));
   	addstr("SCORE : ");
   	move((LINES/2 + 3), (COLS/2));
   	addstr(m);
   	
   	move((LINES/2 + 4), (COLS/2 - 10));
   	addstr("3) NAME : ");
    	move((LINES/2 + 4), (COLS/2));
   	addstr(b[i - 3].name);
   	move((LINES/2 + 5), (COLS/2 - 7));
   	addstr("SCORE : ");
   	move((LINES/2 + 5), (COLS/2));
   	addstr(n);
   	
   	int input = getch();
	if(input == 'q')
		exitgame();
	else {
		p = menu();
		if(p == 0)
			playgame();
		if(p == 1)
			highscores();
		if(p == 2)
			aboutus();
		if(p == 3)
			exitgame();
	}		
			
}

int aboutus() {

	int p;
	clear();
	move(8, (COLS - 99)/2);
	addstr("TITLE : SPACE-ACE");
	move(10, (COLS - 99)/2);
	addstr("PLAYING INSTRUCTION:");
	move(11, (COLS - 99)/2);
	addstr("KEY_UP = To move the tank up");
	move(12, (COLS - 99)/2);
	addstr("KEY_DOWN = To move the tank down");
	move(13, (COLS - 99)/2);
	addstr("SPACE BAR = To shoot bullet");
	move(15, (COLS - 99)/2);
	addstr("LEVELS:");
	move(16, (COLS - 99)/2);
	addstr("There are 4 levels:");
	move(17, (COLS - 99)/2);
	addstr("1)Easy");
	move(18, (COLS - 99)/2);
	addstr("2)Medium");
	move(19, (COLS - 99)/2);
	addstr("3)Hard");
	move(20, (COLS - 99)/2);
	addstr("4)Impossible");
	move(22, (COLS - 99)/2);
	addstr("AIM AND SCORING:");
	move(23, (COLS - 99)/2);
	addstr("The alien ship moves up and down and moves towards the tank after each lap.");
	move(24, (COLS - 99)/2);
	addstr("Shoot the alien and earn 20 points for each hit to the mini ship.");
	move(25, (COLS - 99)/2);
	addstr("Every shot which does not hit the mini ship results in deduction of a point.");
	move(26, (COLS - 99) /2);
	addstr("Stay away from the bombs fired by the alien ship and shoot them at the same time to earn +5 points");
	move((LINES + 16)/2 , (COLS - 13)/2);
	addstr("Version: v1.0");
	move((LINES + 17)/2 , (COLS - 40)/2);
	addstr("Developed and Designed by : Nikhil Rathi");

	
	refresh();
	int input = getch();
	if(input == 'q')
		exitgame();
	else {
		p = menu();
		if(p == 0)
			playgame();
		if(p == 1)
			highscores();
		if(p == 2)
			aboutus();
		if(p == 3)
			exitgame();
	}
}

void exitgame() {
	endwin();
	exit(0);
}
 
int main() {
	int p;
	p = menu();
	if(p == 0)
		playgame();
	if(p == 1)
		highscores();
	if(p == 2)
		aboutus();
	if(p == 3)
		exitgame();
}
	
