project: SpaceAce.c
	cc -c SpaceAce.c -lncurses 
	cc SpaceAce.o -o project -lncurses
rm *.o : SpaceAce.o
rm: project
