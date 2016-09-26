#include <ncurses.h>
#include <stdlib.h>
#define DELAY 1000

struct player {
	int r,c;
	char ch;
};


struct alien {
	int r, c;
	int alive;
	char ch;
	};
	
struct shoot {
	int r,c;
	int active; /* 1=active 0=inactive */
	char ch;
};

int main() {
	   int k, x, y, input, max_x = 0, max_y = 0;
	   unsigned int currentshots = 0;
	   struct alien alien; //[MAX_ALIEN];
	   struct player tank;
	   struct shoot shot[3];

	   tank.r = LINES / 2;
	   tank.c = COLS - 1;
	   tank.ch = '<';
	   
	  /* for(i = 0; i < MAX_ALIEN; i++) {
	  	alien.r = 1;
	  	alien.c = 1;
	  	alien.alive = 1;
	  	alien.ch = '%';
		}
	  */
	  
	   for (k=0; k<3; ++k) {
	      shot[k].active = 0;
	      shot[k].ch = '*';
	   }
     
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
                	if(i == 1) {
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
	
			if(i == 0) {
			
				   clear();
				   noecho();
				   curs_set(FALSE);
				   cbreak();
				   nodelay(stdscr,1);
											          
				   while(1) {
				      /* Move aliens */
				    /*  move(alien.r,alien.c);
				      addch(alien.ch);
					    
				      alien.c++;
				      
				      move(alien.r,alien.c);
				      addch(' ');
				     */ 
				      /* Move tank */
				      move(tank.r,tank.c);
				      addch(tank.ch);

				      input = getch();
				      
				      move(tank.r,tank.c);
				      addch(' ');      
				      
				    /*  for (k=0; k<3; ++k) {
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
				  */
				   
				   for(k = 0; k < 3; k++) {
				   	if(shot[k].active == 1) {
				   		while(shot[k].c != 0) {
				   			move(shot[k].r ,shot[k].c);
				   			addch(shot[k].ch);
				   			move(shot[k].r ,shot[k].c + 1);
				   			addch(' ');
				   			shot[k].c--;
				   		}
				   	}
				   }
				   
				      /* Check input */
				   if (input == KEY_UP)
					tank.r--;
				   else if (input == KEY_DOWN)
					tank.r++;
				   else if (input == ' ' && currentshots < 3) {
					for (i=0; i<3; ++i) {
					    if (shot[k].active == 0) {
					       shot[k].active = 1;
					       ++currentshots;
					       //--score;
					       shot[k].r = tank.r;
					       shot[k].c = COLS - 2;
					       break;
					    }
					 }
				  
				  
					/*Move shots*/
				      //currentshots++;
				/*      x = COLS - 2;
				      y = tank.r;
				     while(x != -10) {
				      getmaxyx(stdscr, max_y, max_x);
				 
				      clear();
				      mvprintw(y, x, "---");
				      refresh();

				      usleep(DELAY);
				      x = x - 1;
				     }
				 */     
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
							 
			if(i == 2) {
			
				//High scores
			
			} 
			 
		   	if(i == 3) {
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
