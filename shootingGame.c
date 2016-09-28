#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DELAY 20000
#define MAX_ALIENS 3

struct player {
	int r,c;
	char ch;
};
	
struct shoot {
	int r,c;
	int active; /* 1=active 0=inactive */
	char ch;
};

struct alien {
	int r,c;
	int pr,pc;
	int alive; /* 1=alive 0=destroyed */
	char direction; 
	char ch;
};

int main() {
	int k, x, y, input;
   	unsigned int currentshots = 0;
   	struct player tank;
   	struct alien aliens[MAX_ALIENS];
 	struct shoot shot[3];
     
	initscr();
	
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

	WINDOW *w3;
	w3 = newwin(40, 110, 8, (COLS - 110) / 2);
	box(w3, 0, 0);
	wrefresh(w3);
	
	WINDOW *w2;
	int j = 0;
	w2 = newwin(35, 100, 10, (COLS - 100) / 2);
	//box(w2, 0, 0);
	for(j = 0; j<5; j++) {
		sprintf(lineitem, "%s", line[j]);
	     	mvwprintw( w2, j+1, 2, "%s", lineitem);
	}
	wrefresh( w2);   
		
	WINDOW *w;
	w = newwin( 15, 25, (LINES - 15) / 2, (COLS - 25) / 2 ); 
	box( w, 0, 0 ); 
	for( i=0; i<5; i++) {
		if( i == 0 ) 
		    wattron( w, A_STANDOUT ); 
		else
		    wattroff( w, A_STANDOUT );
		sprintf(item, "%s",  list[i]);
		mvwprintw( w, i+1, 2, "%s", item );
	}
	wrefresh( w );    

	i = 0;
	noecho();
	keypad( w, TRUE ); 
	curs_set( 0 );
	
	   tank.r = LINES / 2;
   	   tank.c = COLS - 1;
	   tank.ch = '<';
	   
	   for (k=0; k<MAX_ALIENS; k++) {
	      aliens[k].r = k+1;
	      aliens[k].c = 1;
	      aliens[k].pr = 0;
	      aliens[k].pc = 0;
	      aliens[k].ch = '@';
	      aliens[k].alive = 1;
	      aliens[k].direction = 'r';
	   }
	     
	   for (k=0; k<3; k++) {
	      shot[k].active = 0;
	      shot[k].ch = '-';
	   }
     
    	while(( ch = wgetch(w)) != 'q') { 
            sprintf(item, "%-7s",  list[i]); 
            mvwprintw( w, i+1, 2, "%s", item ); 
            switch( ch ) {
                case KEY_UP:
                	if(i == 0) {
              	      	i = 4;
                        }
                            
                        else
                        i--;
 
                        break;
                            
                case KEY_DOWN:
                	if(i == 4) {
                        i = 0;
                        } 
                            
                        else
                        i++;

                        break;
                            
                case 10:
                
                	if(i == 0) {
                	
                 	   clear();
			   noecho();
			   curs_set(FALSE);
			   cbreak();
			   nodelay(stdscr,1);
			   
			      while(1) {

				      /* Move tank */
				      move(tank.r,tank.c);
				      addch(tank.ch);   
				      
				      
				      /* Move aliens */
				      for (k=0; k<MAX_ALIENS; k++) {
					 if (aliens[k].alive == 1) {
					    move(aliens[k].pr,aliens[k].pc);
					    addch(' ');
					    
					    move(aliens[k].r,aliens[k].c);
					    addch(aliens[k].ch);
					    
					    aliens[k].pr = aliens[k].r;
					    aliens[k].pc = aliens[k].c;
				       
					    /* Set alien's next position */
					    if (aliens[k].direction == 'r')
					       ++aliens[k].c;
					       
					    if(aliens[k].c == COLS - 2)
						aliens[k].ch = ' ';
					 }
				      }
				      
				      for (k=0; k<3; k++) {
					 if (shot[k].active == 1) {
					    if (shot[k].c > 0) {
					       if (shot[k].c < COLS - 2) {
						  move(shot[k].r ,shot[k].c + 1);
						  addch(' ');
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
				    
				     refresh();
				     usleep(DELAY);   
				   
				     input = getch();
				      
				     move(tank.r,tank.c);
				     addch(' ');   
				    
				  /* Check input */
				   if (input == 'w')
					tank.r--;
				   else if (input == 's')
					tank.r++;
				   else if (input == ' ' && currentshots < 3) {
					for (k=0; k<3; k++) {
					    if (shot[k].active == 0) {
					       shot[k].active = 1;
					       ++currentshots;
					       //--score;
					       shot[k].r = tank.r;
					       shot[k].c = COLS - 2;
					       break;
					    }
					 }
				   
				   }
				   
				   else if(input == 'q')
				      	break;

				  /* Check for valid tank position */
				   if (tank.r > LINES - 1)
					tank.r = LINES - 1;
				   else if (tank.r < 0)
					tank.r = 0;    
				   }
				   
		        }
                	
                	if(i == 1) {
                	//Settings
                		clear();
				move((LINES - 5)/2, (COLS - 8)/2);
				addstr("Choice 1");
				move((LINES - 3)/2, (COLS - 8)/2);
				addstr("Choice 2");
				move((LINES - 1)/2, (COLS - 8)/2);
				addstr("Choice 3");
				move((LINES + 1)/2, (COLS - 8)/2);
				addstr("Choice 4");
				move((LINES + 3)/2, (COLS - 8)/2);
				addstr("Choice 5");
				move((LINES + 5)/2, (COLS - 8)/2);
				addstr("Choice 6");
				move((LINES + 7)/2, (COLS - 8)/2);
				addstr("Choice 7");
				move((LINES + 9)/2, (COLS - 8)/2);
				addstr("Choice : ");
				refresh();
				char input = getch();
				switch(input) {
					case 'b':
						break;
							
					default:
						endwin();
						exit(0);

				}
			}
				 
			if(i == 2) {
			
				//High scores
			
			} 
			 
		   	if(i == 3) {
		   	//Developed by
				clear();
		   		//delwin(w);
				//delwin(w2);
				//delwin(w3);
				//wclear(w);
				//wclear(w2);
				//wclear(w3);
		   		move((LINES + 2)/2 , (COLS - 13)/2);
		   		addstr("Version: v1.0");
		   		move((LINES + 3)/2 , (COLS - 40)/2);
		   		addstr("Developed and Designed by : Nikhil Rathi");
		   		refresh();
		   		getch();
		   		
		   		switch(ch) {
					default:
						endwin();
						exit(0);
				}
		   	}
		   	
			if(i == 4) {
			//Exit Game
			 	endwin();
			   	exit(0);
			}
			break;
	    }

            wattron( w, A_STANDOUT );
            sprintf(item, "%-7s",  list[i]);
            mvwprintw( w, i+1, 2, "%s", item);
            wattroff( w, A_STANDOUT );
    }
 
    delwin(w3);
    delwin(w2);
    delwin(w);
    endwin();
}
