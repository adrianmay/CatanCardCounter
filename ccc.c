#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char * resNames="wbsgo?";
char line[1000];
int world[4][6]={0};
int players;
int player=0;

void saveWorld() {
  FILE * fp;
  fp = fopen("data.bin","w");
  fwrite(&player, sizeof(player), 1, fp);
  fwrite(world, sizeof(world), 1, fp);
  fclose(fp);
}

void loadWorld() {
  FILE * fp;
  fp = fopen("data.bin","r");
  if (!fp) return;
  fread(&player, sizeof(player), 1, fp);
  fread(world, sizeof(world), 1, fp);
  fclose(fp);
}

void nextPlayer() { 
  player++; 
  if (player>=players) player=0;
  saveWorld();
}

int resource(char r) {
  char * plr = strchr(resNames,r);
  if (plr) {
    int i = plr-resNames;
    return i; 
  }
  printf("WTF is %c?\n", r);
  return 5;
}

int process(int plr, int sense, char * l) {
  if (*l >= '0' && *l <= '4') // Prefix a player number to apply the change to them
    return process(atoi(l), sense, l+1);
  switch (*l) {
    case '\n': return 1; // Blank entry goes to next player
    case ',': //Back to previous player
      for (int a=0;a<players-1;a++) nextPlayer();
      return 0; 
    case '<': // Trade with other player
      l++;
      int other = atoi(l);
      l++;
      process(plr, 1, l);
      process(other, -1, l);
      return 0;
    case 'R': return process(plr, 1, "-wb\n");    // Road
    case 'S': return process(plr, 1, "-wbsg\n");  // Settlement
    case 'H': return process(plr, 1, "-wbsg\n");  // Ditto
    case 'C': return process(plr, 1, "-ggooo\n"); // City
    case 'D': return process(plr, 1, "-sgo\n");   // Dev card
    default:
      int inc=1;
      if (*l=='-') {inc=-1; l++;} // Prepend minus to lose rather than receive all cards in the list
      for (;*l!='\n';l++) 
        world[plr][resource(*l)]+=inc*sense;
      return 0;
  }
}

void printNum(int i) {
  if (i) printf("\t %d", i);
  else printf("\t .");
}

void printWorld(char * argv[]) {
  printf("\n     \tWOOD\tBRICK\tSHEEP\tGRAIN\tORE  \t?????\tTOTAL\n");
  for (int plr=0;plr<players;plr++) {
    printf("%d(%s) ", plr, argv[plr+1]);
    int tot=0;
    for (int r=0;r<6;r++) {
      int i = world[plr][r];
      tot += i;
      printNum(i);
    }
    printNum(tot);
    printf("\n");
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  if (argc < 2) { printf("Pass some 3-letter player names"); exit(1);}
  players = argc-1;
  loadWorld();
  while (1) {
    printWorld(argv);
    printf("Player: %s: ", argv[1+player]);
    fgets(line, 1000, stdin);
    int nxt = process(player, 1, line);
    saveWorld();
    if (nxt) nextPlayer();
  }
  return 0;
}

