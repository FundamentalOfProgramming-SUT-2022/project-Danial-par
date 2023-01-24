#include <stdio.h>
#include <stdlib.h>
#include <dir.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define LINE_SIZE 1000
#define ORDER_SIZE 50
#define MAX 100


//           functions          //
void createfile(char[]);
long long strtolli(char*);
long long tavan(long long, int);
char* findfilename(char*, char*);
char* findtext(char*, char*);
char* findpos(char*, long long*, long long*);
char* findsize(char*, long long*);
char* rmg(char[]);
void insertn(FILE*, long, char);
void insert(char[], char[], long long, long long);
int addcheck(char[]);
int DirCheck(const char *path);
void shiftfile(FILE*, int);
void cat(char[]);
void removestr(char[], long long, long long, long long, char);

//              //              //
int main(){
    mkdir("root");
    mkdir("root\\.hidden");
    while(1){
        char line[LINE_SIZE];
        gets(line);
        char order[ORDER_SIZE];
        for(int i=0; i<LINE_SIZE && line[i]!=' '; i++){
            order[i] = line[i];
            order[i+1] = '\0';
        }

        if(!strcmp(order, "exit")){
            break;
        }

        else if(!strcmp(order, "createfile")){
            char filename[MAX];
            char *s = findfilename(line+11, filename);
            
            if(s!=NULL){
                createfile(filename);
            }
            continue;
        }

        else if(!strcmp(order, "insertstr")){
            char* ptr = line+10;
            char filename[MAX];
            ptr = findfilename(ptr, filename);
            if(ptr==NULL){
                continue;
            }
            char text[LINE_SIZE];
            ptr = findtext(ptr, text);
            if(ptr==NULL){
                continue;
            }
            long long *line = (long long*)malloc(sizeof(long long));
            long long *col = (long long*)malloc(sizeof(long long));
            ptr = findpos(ptr, line, col);
            if(ptr==NULL){
                continue;
            }
            insert(filename, text, *line, *col);
            continue;
        }

        else if(!strcmp(order, "cat")){
            char filename[MAX];
            char *s = findfilename(line+4, filename);
            if(s==NULL){
                continue;
            }
            cat(filename);
        }

        else if(!strcmp(order, "removestr")){
            char filename[MAX];
            char *ptr = findfilename(line+10, filename);
            if(ptr==NULL){
                continue;
            }
            long long *line = (long long *)malloc(sizeof(long long));
            long long *col = (long long *)malloc(sizeof(long long));
            long long *size = (long long *)malloc(sizeof(long long));
            ptr = findpos(ptr, line, col);
            ptr = findsize(ptr, size);
            char flag[3];
            flag[0]=ptr[0];
            flag[1]=ptr[1];
            flag[2]='\0';
            if((flag[1]!='f' && flag[1]!='b') || flag[0]!='-'){
                printf("Invalid command! use -f or -b at the end.\n");
                continue;
            }
            removestr(filename, *line, *col, *size, flag[1]);
        }

        else{
            printf("Invalid command\n");
            continue;
        }
    }
    return 0;
}


//              //              //
void createfile(char name[]){
    char my_add[10000];
    getcwd(my_add, 10000);
    char * p = NULL;
    char name2[strlen(name)+1];
    strcpy(name2, name);
    char *cpy = name2;
    if(1){
        char file[strlen(name)];
        p = strrchr(cpy, '\\');
        if(p==NULL || p==name){
            printf("Invalid address.\n");
            chdir(my_add);
            return;
        }
        strcpy(file, p+1);
        char add[strlen(name)+3];
        *p = '\0';
        cpy++;
        p = strtok(cpy, "\\");
        while(p!=NULL){
            add[0] = '.'; add[1] = '\\'; add[2] = '\0';
            strcat(add, p);
            mkdir(add);
            chdir(add);
            p = strtok(NULL, "\\");
        }
        chdir(name+1);
        FILE* f = fopen(file, "r+");
        if(!f){
          FILE* fil = fopen(file, "w");
          fclose(fil);
          printf("Operation is done successfully.\n");
        }
        else{
          printf("File already exist\n");
          fclose(f);
        }
    }
    chdir(my_add);
}

char* findfilename(char *text, char *hold){
    if(strncmp(text, "--file ", 7)){
        printf("Invalid command. use --file.\n");
        return NULL;
    }
    text+=7;
    char *s = text;
    strcpy(hold, text);
    if(s[0] == '"'){
        s++;
        s = strchr(s, '"');
        while(*(s-1)=='\\'){
            s++;
            s = strchr(s, '"');
        }
        int i = s-text;
        *(hold+i) = '\0';
        for(int i=0; hold[i+1]!='\0'; i++){
            hold[i]=hold[i+1];
            hold[i+1]='\0';
        }
        return s+2;
    }
    while(*s!=' ' && *s!='\0'){
        s++;
    }
    int i = s-text;
    hold[i] = '\0';
    if(hold[0]!='\\'){
        printf("Invalid file address. use \\ at the begining of address.\n");
        return NULL;
    }
    return s+1;
}

char* findtext(char *text, char *hold){
    if(strncmp(text, "--str ", 6)){
        printf("Invalid command. use --str.\n");
        return NULL;
    }
    text+=6;
    char *s = text;
    strcpy(hold, text);
    if(s[0] == '"'){
        s++;
        s = strchr(s, '"');
        while(*(s-1)=='\\'){
            s++;
            s = strchr(s, '"');
        }
        int i = s-text;
        *(hold+i) = '\0';
        for(int i=0; hold[i+1]!='\0'; i++){
            hold[i]=hold[i+1];
            hold[i+1]='\0';
        }
        return s+2;
    }
    while(*s!=' ' && *s!='\0'){
        s++;
    }
    int i = s-text;
    hold[i] = '\0';
    return s+1;
}

long long tavan(long long a, int b){
    if(b==0){
        return 1;
    }
    long long c = a;
    for(int i=0; i<b-1; i++){
        c*=a;
    }
    return c;
}

long long strtolli(char str[]){
    long long u=0;
    for(int i=0; str[i]!='\0'; i++){
        u += tavan(10, strlen(str)-1-i) * ((long long)(str[i]) - (long long)('0'));
    }
    return u;
}

char* findpos(char *text, long long *line, long long *col){
    char cline[strlen(text)];
    char ccol[strlen(text)];
    if(strncmp(text, "--pos ", 6)){
        printf("Invalid command. use --pos.\n");
        return NULL;
    }
    text+=6;
    for(int i=0; text[i]!=':'; i++){
        cline[i] = text[i];
        cline[i+1] = '\0';
    }
    text+=strlen(cline)+1;
    for(int i=0; text[i]!=' ' && text[i]!='\0'; i++){
        ccol[i] = text[i];
        ccol[i+1] = '\0';
    }
    text+=strlen(ccol)+1;
    (*line) = strtolli(cline);
    (*col) = strtolli(ccol);
    if(*line<1 || *col<0){
        printf("Invalid line or column number.\n");
        return NULL;
    }
    return text;
}

char* rmg(char name[]){
    if(name[0]!='"'){
        return name;
    }
    else if(name[0]='"'){
        char* p = strrchr(name, '"');
        *p = '\0';
        return name+1;
    }
}

void insert(char name[], char text[], long long line, long long col){
    if(!addcheck(name)){
        printf("such file doesn't exist!\n");
        return;
    }
    name++;
    FILE *f = fopen(name, "r+");
    int count=1;
    char tmp;
    while((tmp=getc(f))!=EOF){
        if(tmp=='\n'){
            count++;
        }
    }
    fseek(f, 0, SEEK_SET);
    if(line>count){
        printf("Such position doesn't exist in file!\n");
        return;
    }
    for(int i=0; i<line-1; i++){
        while(getc(f)!='\n'){
        }
    }
    for(int i=0; i<col; i++){
        tmp = getc(f);
        if(tmp=='\n'){
            printf("Such position doesn't exist in file!\n");
            return;
        }
    }
    for(int i=0; text[i]!='\0'; i++){
        if(text[i]=='\\' && text[i+1]!='\0'){
            if(text[i+1]=='"'){
                insertn(f, ftell(f), '"');
                i++;
            }
            else if(text[i+1]=='n'){
                insertn(f, ftell(f), '\n');
                i++;
            }
            else if(text[i+1]=='\\' && text[i+2]=='n'){
                insertn(f, ftell(f), '\\');
                insertn(f, ftell(f), 'n');
                i+=2;
            }
            else{
                insertn(f, ftell(f), '\\');
            }
        }
        else{
            insertn(f, ftell(f), text[i]);
        }
    }
    fclose(f);
    printf("Operation is done successfully.\n");
}

void insertn(FILE* f, long p, char c){
    fseek(f, p, SEEK_SET);
    shiftfile(f, 1);
    if(c=='\n'){
        shiftfile(f, 1);
    }
    putc(c, f);
}

void shiftfile(FILE* f, int n){
    FILE *g = fopen("root\\.hidden\\$func_shift.txt", "w");
    int w = ftell(f);
    char a;
    while((a=getc(f))!=EOF){
        putc(a, g);
    }
    fclose(g);
    FILE *h = fopen("root\\.hidden\\$func_shift.txt", "r");
    fseek(f, w, SEEK_SET);
    for(int i=0; i<n; i++){
        putc(' ', f);
    }
    while((a=getc(h))!=EOF){
        putc(a, f);
    }
    fclose(h);
    fseek(f, w, SEEK_SET);
}

int addcheck(char name[]){
    char my_add[10000];
    getcwd(my_add, 10000);
    char name2[strlen(name)+2];
    char filename[MAX];
    strcpy(name2, name);
    char* p = name2+1;
    p = strrchr(p, '\\');
    if(p==NULL){
        return 0;
    }
    strcpy(filename, p+1);
    *p = '\0';
    p = name2+1;
    if(!DirCheck(p)){
        return 0;
    }
    chdir(p);
    FILE *f = fopen(filename, "r+");
    if(!f){
        chdir(my_add);
        return 0;
    }
    chdir(my_add);
    fclose(f);
    return 1;
}

int DirCheck(const char *path)
{
    struct stat stats;

    stat(path, &stats);

    // Check for file existence
    if (S_ISDIR(stats.st_mode))
        return 1;

    return 0;
}

void cat(char name[]){
    if(!addcheck(name)){
        printf("such file doesn't exist!\n");
        return;
    }
    name++;
    FILE *f = fopen(name, "r+");
    char a;
    printf("Here is the file content:\n");
    printf("----------------------------\n");
    while((a=getc(f))!=EOF){
        printf("%c", a);
    }
    printf("\n----------------------------\n");
}

char* findsize(char *ptr, long long *size){
    if(strncmp(ptr, "--size ", 7)){
        printf("Invalid command. use --size.\n");
        return NULL;
    }
    ptr+=7;
    char csize[strlen(ptr)];
    for(int i=0; ptr[i]!='\0' && ptr[i]!=' '; i++){
        csize[i] = ptr[i];
        csize[i+1] = '\0';
    }
    *size = strtolli(csize);
    ptr += strlen(csize) + 1;
    return ptr;
}

void removestr(char name[], long long line, long long col, long long size, char flag){
    if(!addcheck(name)){
        printf("such file doesn't exist!\n");
        return;
    }
    name++;
    FILE *f = fopen(name, "r+");
    int count=1;
    char tmp;
    while((tmp=getc(f))!=EOF){
        if(tmp=='\n'){
            count++;
        }
    }
    fseek(f, 0, SEEK_SET);
    if(line>count){
        printf("Such position doesn't exist in file!\n");
        return;
    }
    for(int i=0; i<line-1; i++){
        while(getc(f)!='\n'){
        }
    }
    for(int i=0; i<col; i++){
        tmp = getc(f);
        if(tmp=='\n'){
            printf("Such position doesn't exist in file!\n");
            return;
        }
    }
    int pos = ftell(f);
    fseek(f, 0, SEEK_END);
    int e = ftell(f);
    fseek(f, pos, SEEK_SET);
    FILE *g = fopen("root\\.hidden\\$func_remove.txt", "w");
    if(flag=='b'){
        int beg;
        char tmp;
        for(int i=0; i<size; i++){
            if(ftell(f)==0){
                printf("there aren't enough charachters to remove!\n");
                return;
            }
            fseek(f, -1, SEEK_CUR);
            tmp=getc(f);
            fseek(f, -1, SEEK_CUR);
            if(tmp=='\n'){
                fseek(f, -1, SEEK_CUR);
            }
        }
        beg = ftell(f);
        fseek(f, 0, SEEK_SET);
        while(ftell(f)!=beg){
            putc(getc(f), g);
        }
        while(ftell(f)!=pos){
            fseek(f, 1, SEEK_CUR);
        }
        while((tmp=getc(f))!=EOF){
            putc(tmp, g);
        }
        fclose(g);
        fclose(f);
        FILE *f2 = fopen(name, "w");
        FILE *g2 = fopen("root\\.hidden\\$func_remove.txt", "r");
        while((tmp=getc(g2))!=EOF){
            putc(tmp, f);
        }
        fclose(g2);
        fclose(f2);
    }
    else if(flag=='f'){
        int beg = pos;
        char tmp;
        for(int i=0; i<size; i++){
            if(ftell(f)==e){
                printf("there aren't enough charachters to remove!\n");
                return;
            }
            tmp = getc(f);
            if(tmp=='\n'){
                fseek(f, 1, SEEK_CUR);
            }
        }
        int pos = ftell(f);
        fseek(f, 0, SEEK_SET);
        while(ftell(f)!=beg){
            putc(getc(f), g);
        }
        while(ftell(f)!=pos){
            fseek(f, 1, SEEK_CUR);
        }
        while((tmp=getc(f))!=EOF){
            putc(tmp, g);
        }
        fclose(g);
        fclose(f);
        FILE *f2 = fopen(name, "w");
        FILE *g2 = fopen("root\\.hidden\\$func_remove.txt", "r");
        while((tmp=getc(g2))!=EOF){
            putc(tmp, f);
        }
        fclose(g2);
        fclose(f2);
    }
    printf("Operation is done successfully.\n");
}


