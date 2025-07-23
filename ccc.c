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
  char * p = strchr(resNames,r);
  if (p) {
    int i = p-resNames;
    return i; 
  }
  printf("WTF is %c?\n", r);
  return 5;
}

int process(int p, int sense, char * l) {
  if (*l >= '0' && *l <= '4') 
    return process(atoi(l), sense, l+1);
  switch (*l) {
    case '\n': return 1;
    case ',':
      for (int a=0;a<players-1;a++) nextPlayer();
      return 0; 
    case '<':
      l++;
      int other = atoi(l);
      l++;
      process(p, 1, l);
      process(other, -1, l);
      return 0;
    case 'R': return process(p, 1, "-wb\n");
    case 'S': return process(p, 1, "-wbsg\n");
    case 'H': return process(p, 1, "-wbsg\n");
    case 'C': return process(p, 1, "-ggooo\n");
    case 'D': return process(p, 1, "-sgo\n");
    default:
      int ret = 1;
      int inc=1;
      if (*l=='-') {inc=-1; l++;}
      for (;*l!='\n';l++) 
        world[p][resource(*l)]+=inc*sense;
      return 0;
  }
}

void printWorld(char * argv[]) {
  printf("\n     \tWOOD\tBRICK\tSHEEP\tGRAIN\tORE  \t?????\n");
  for (int p=0;p<players;p++) {
    printf("%d(%s) ", p, argv[p+1]);
    for (int r=0;r<6;r++) {
      int i = world[p][r];
      if (i) printf("\t %d", i);
      else printf("\t .");
    }
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

