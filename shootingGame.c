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
//#define DELAY 20000

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
	int loop; /* used to prevent alien from flashing when bomb is dropped */
	char ch;
};

struct options {
	int overall,alien,shots,bombs,bombchance;
};


struct entry {
   	char name[128];
   	int score;
};

void gameover(int win, int score);
void highscores();
void settings();
void aboutus();
void exitgame();

WINDOW *w, *w2, *w3;

int menu() {
	initscr();
	noecho(); 
	curs_set( 0 );
	
	char list[5][50] = { "PLAY GAME", "SETTINGS", "HIGH SCORES", "ABOUT US", "EXIT"};
	char item[50];
	int ch, i = 0;
	char line[5][100] = {
	"|-------   |-------|   |-------|   |-------   |-------         |-------|   |-------   |-------",
	"|          |       |   |       |   |          |                |       |   |          |       ",
	"|------|   |-------|   |-------|   |          |-------   ---   |-------|   |          |-------",
	"       |   |           |       |   |          |                |       |   |          |       ", 
	"-------|   |           |       |   |-------   |-------         |       |   |-------   |-------"};
	char lineitem[100];

	//WINDOW *w3;
	w3 = newwin(40, 110, 8, (COLS - 110) / 2);
	box(w3, 0, 0);
	wrefresh(w3);
	
	//WINDOW *w2;
	int j = 0;
	w2 = newwin(35, 100, 10, (COLS - 100) / 2);
	//box(w2, 0, 0);
	for(j = 0; j<5; j++) {
		sprintf(lineitem, "%s", line[j]);
	     	mvwprintw( w2, j+1, 2, "%s", lineitem);
	}
	wrefresh( w2);   
		
	//WINDOW *w;
	w = newwin(15, 25, (LINES - 15) / 2, (COLS - 25) / 2 ); 
	box( w, 0, 0 ); 
	for( i=0; i<5; i++) {
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
	while((ch = wgetch(w)) != 'q') { 
            sprintf(item, "%-7s",  list[i]); 
            mvwprintw( w, i+1, 2, "%s", item ); 
            switch( ch ) {
                case KEY_UP:
                	if(i == 0)
              	      	   i = 4;
                    else 
                         i--;
                         
                    break;
                            
                case KEY_DOWN:
                	if(i == 4)
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
	int input, random=0, score=0, win=-1;
	char tellscore[30];
	    	   
	clear();
	noecho();
	curs_set(FALSE);
	cbreak();
	keypad(stdscr, TRUE );
	nodelay(stdscr,1);
	srand(time(NULL));
	
	/* Set default settings */
	//This sets the default setting that is speed of alien and freq. of bombs etc.
	settings.overall = 6500;
	settings.alien = 12;
	settings.shots = 1;
	settings.bombs = 10;
	settings.bombchance = 5;

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
	/* Set shot settings */ 
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
	
	/* Display game title,score header,options */
    move(0,(COLS/2)-9);
    addstr("***--SPACE-ACE--***");
    move (0,1);
    addstr("SCORE: ");
    move(0,COLS-9);
	addstr("q = quit");
	
	while(1) {
      	/* Show score */
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
		
      /* Move aliens */
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
		        if ((settings.bombchance - random*1.5) >= 0 && currentbombs < MAX_BOMBS) {
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
		        
			    /* Set alien's next position */
		        if (aliens[i].direction == 'u')
		        	--aliens[i].r;
		        else if (aliens[i].direction == 'd')
		        	++aliens[i].r;
		           
		        /* Check alien's next positions */
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

/* This function handles displaying the win/lose screen */
void gameover(int win, int score) {
   struct entry list;
   unsigned int input;
   char name[128];	
   FILE *fp;
   nodelay(stdscr, 0);
   curs_set(TRUE);
   
   if (win == 0) {
      clear();
      move((LINES/2)-1,(COLS/2)-5);
      addstr("GAME OVER!");
      move((LINES/2),(COLS/2)-7);
      addstr("PRESS ANY KEY");
      move(0,COLS-1);
      refresh();
      input = getch();
   }
   
   else if (win == 1) {
      clear();
      move((LINES/2)-1,(COLS/2)-5);
      addstr("YOU WIN!");
      move((LINES/2),(COLS/2)-7);
      addstr("PRESS ANY KEY");
      move(0,COLS-1);
      refresh();
      input = getch();
   }
   
   if(input == 10) {
	   clear();
	   echo();
	   nocbreak();
	   move((LINES/2)-2, (COLS/2)-9);
	   addstr("ENTER YOUR NAME: ");
	   refresh();
	   //getch();
	   getstr(list.name);
   }
   //Create a file, write name and scores in it, write a sorting algorithm and then when highscore function is called then display highscores in decreasing order. 
   
   list.score = score;
   fp = fopen("score.txt", "a");
   fprintf(fp, "%s %d", list.name, list.score);
   fprintf(fp,"\n");
   fclose(fp);
}

void highscores () {
	
	//Read data from score.txt
	//Sort according to score with corr. player name
	//Store data in another file ("highscore.txt")
	//print data from "highscore.txt"
/*	
	struct entry b[128], *a;
	int fd, i, line;
	char ch;
	//char *name[128];
	fd = open("score.txt", O_RDONLY, S_IRUSR);
	if(fd == -1) {
		perror("cp: can't open file");
		exit(errno);
	}
	
	//read(fd, &n, sizeof(n));  
	while(read(fd, &ch, sizeof(ch))) {
		if(ch == '\n')
			line++;
	}
	
	a = ((struct entry*)malloc(sizeof(b)));
	while(i != n - 1) { // n is the total number of structures
		read(fd, a, sizeof(struct entry));
		strcpy(b[i].name, a->name);
		b[i].score = a->score;
		i++;
	}	

*/			
}

void settings() {

}

void aboutus() {

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
		settings();
	if(p == 0)
		highscores();
	if(p == 0)
		aboutus();
	if(p == 0)
		exitgame();
}
	
