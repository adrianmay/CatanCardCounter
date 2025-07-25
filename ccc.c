#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

const char * resNames="lbsgo?";
char line[1000];
int world[4][6]={0};
int players;

void printDateTime() {
  time_t t;   // not a primitive datatype
  time(&t);
  printf("%s\n", ctime(&t));
}

void saveWorld() {
  int junk = 0; // Cos of old file format
  FILE * fp;
  fp = fopen("data.bin","w");
  fwrite(&junk, sizeof(junk), 1, fp);
  fwrite(world, sizeof(world), 1, fp);
  fclose(fp);
}

void loadWorld() {
  int junk = 0; // Cos of old file format
  FILE * fp;
  fp = fopen("data.bin","r");
  if (!fp) return;
  fread(&junk, sizeof(junk), 1, fp);
  fread(world, sizeof(world), 1, fp);
  fclose(fp);
}

int resource(char r) {
  char * plr = strchr(resNames,r);
  if (plr) {
    int i = plr-resNames;
    return i; 
  }
  printf("WTF is %c?\n", r);
  return -1;
}

int processN(int plr, int sense, char * l) {
  switch (*l) {
    case '\n': return 1; // Blank entry goes to next player
    case '<': // Trade with other player
      l++;
      int other = atoi(l);
      l++;
      processN(plr, 1, l);
      processN(other, -1, l);
      return 0;
    case 'R': return processN(plr, 1, "-lb\n");    // Road
    case 'S': return processN(plr, 1, "-lbsg\n");  // Settlement
    case 'H': return processN(plr, 1, "-lbsg\n");  // Ditto
    case 'C': return processN(plr, 1, "-ggooo\n"); // City
    case 'D': return processN(plr, 1, "-sgo\n");   // Dev card
    default:
      for (;*l!='\n';l++) {
        if (*l=='-') sense = (-1)*sense;  // Prepend minus to lose rather than receive all cards in the list
        else {
          int r = resource(*l);
          if (r>=0) world[plr][r] += sense;
        }
      }
      return 0;
  }
}

int processP(int sense, char * l) {
  int plr;
  if (*l >= '0' && *l <= '4') // Prefix a player number to apply the change to them
    processN(atoi(l), sense, l+1);
  else {
    printf("Start with a user number");
    return 0;  
  }
}
  
void printNum(int i) {
  if (i) printf("\t %d", i);
  else printf("\t .");
}

void printWorld(char * argv[]) {
  printf("\n     \tLOGS\tBRICK\tSHEEP\tGRAIN\tORE  \t?????\tTOTAL\n");
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
  printDateTime();
}

int main(int argc, char *argv[]) {
  if (argc < 2) { printf("Pass some 3-letter player names"); exit(1);}
  players = argc-1;
  loadWorld();
  while (1) {
    printWorld(argv);
    printf("News? ");
    fgets(line, 1000, stdin);
    int nxt = processP(1, line);
    saveWorld();
  }
  return 0;
}

