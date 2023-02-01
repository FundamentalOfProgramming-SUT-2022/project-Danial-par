#include <stdio.h>
#include <stdlib.h>
#include <dir.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define LINE_SIZE 1000
#define ORDER_SIZE 50
#define MAX 100

struct finder{
    long long pl;
    long long epl;
    long long outpl;
    long long bw;
    long long byline;
    long long linepos;
};

//           functions          //
void createfile(char[]);
long long strtolli(char*);
long long tavan(long long, int);
char* findfilename(char*, char*);
char* findtext(char*, char*);
char* findpos(char*, long long*, long long*);
char* findsize(char*, long long*);
char* rmg(char[]);
void insertn(FILE*, long long, char);
void insert(char[], char[], long long, long long);
int addcheck(char[]);
int DirCheck(const char *path);
void shiftfile(FILE*, int);
void cat(char[]);
int removestr(char[], long long, long long, long long, char, int);
void copystr(char[], long long, long long, long long, char, int);
int checkpos(char[], long long, long long, long long*);
void pastestr(char[], long long, long long);
struct finder* find(char[], char[], int *);
void replace(char[], char[], char[], int, long long);

//              //              //
int main(){
    mkdir("root");
    mkdir("backups");
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
                free(line); free(col);
                continue;
            }
            insert(filename, text, *line, *col);
            free(line); free(col);
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

        else if(!strcmp(order, "removestr") || !strcmp(order, "copystr") || !strcmp(order, "cutstr")){
            char filename[MAX];
            int w;
            if(!strcmp(order, "removestr")){
                w=0;
            }
            else if(!strcmp(order, "copystr")){
                w=1;
            }
            else if(!strcmp(order, "cutstr")){
                w=2;
            }
            char* ptr;
            switch(w){
                case 0:
                    ptr = findfilename(line+10, filename);
                    break;
                case 1:
                    ptr = findfilename(line+8, filename);
                    break;
                case 2:
                    ptr = findfilename(line+7, filename);
                    break;
            }
            if(ptr==NULL){
                continue;
            }
            long long *line = (long long *)malloc(sizeof(long long));
            long long *col = (long long *)malloc(sizeof(long long));
            long long *size = (long long *)malloc(sizeof(long long));
            ptr = findpos(ptr, line, col);
            if(ptr==NULL){
                free(line); free(col); free(size);
                continue;
            }
            ptr = findsize(ptr, size);
            if(ptr==NULL){
                free(line); free(col); free(size);
                continue;
            }
            char flag[3];
            flag[0]=ptr[0];
            flag[1]=ptr[1];
            flag[2]='\0';
            if((flag[1]!='f' && flag[1]!='b') || flag[0]!='-'){
                free(line); free(col); free(size);
                printf("invalid command! use -f or -b at the end.\n");
                continue;
            }
            switch(w){
                case 0:
                    removestr(filename, *line, *col, *size, flag[1], 0);
                    break;
                case 1:
                    copystr(filename, *line, *col, *size, flag[1], 0);
                    break;
                case 2:
                    copystr(filename, *line, *col, *size, flag[1], 1);
                    break;
            }
            free(line); free(col); free(size);
            continue;
        }

        else if(!strcmp(order, "pastestr")){
            char* ptr = line+9;
            char filename[MAX];
            ptr = findfilename(ptr, filename);
            if(ptr==NULL){
                continue;
            }
            long long *line = (long long*)malloc(sizeof(long long));
            long long *col = (long long*)malloc(sizeof(long long));
            ptr = findpos(ptr, line, col);
            if(ptr==NULL){
                free(line); free(col);
                continue;
            }
            pastestr(filename, *line, *col);
            free(line); free(col);
            continue;
        }

        else if(!strcmp(order, "find")){
            char* ptr = line+5;
            char text[LINE_SIZE];
            ptr = findtext(ptr, text);
            if(ptr==NULL){
                continue;
            }
            char filename[MAX];
            ptr = findfilename(ptr, filename);
            if(ptr==NULL){
                continue;
            }
            ptr--;
            int q=0;
            int flags=0;
            long long at=0;
            for(int i=0; i<4; i++){
                if(ptr[0]=='\0'){
                    break;
                }
                else if(ptr[0]!=' '){
                    printf("invalid command.\n");
                    q=1;
                    break;
                }
                ptr++;
                if(!strncmp(ptr, "-count", 6)){
                    flags+=8;
                    ptr+=6;
                    continue;
                }
                else if(!strncmp(ptr, "-at", 3)){
                    flags+=4;
                    ptr+=4;
                    char atval[LINE_SIZE];
                    atval[0]='\0';
                    while(ptr[0]!=' ' && ptr[0]!='\0'){
                        char tmp[2];
                        tmp[0]=ptr[0];
                        tmp[1]='\0';
                        strcat(atval, tmp);
                        ptr++;
                    }
                    at = strtolli(atval);
                    continue;
                }
                else if(!strncmp(ptr, "-byword", 7)){
                    flags+=2;
                    ptr+=7;
                    continue;
                }
                else if(!strncmp(ptr, "-all", 4)){
                    flags+=1;
                    ptr+=4;
                    continue;
                }
                else{
                    printf("invalid command.\n");
                    q=1;
                    break;
                }
            }
            if(q){
                continue;
            }
            int *counter = (int*)(malloc(sizeof(int)));
            struct finder *matches = (struct finder*)malloc(sizeof(struct finder)*LINE_SIZE);
            matches = find(filename, text, counter);
            int count = *counter;
            free(counter);
            // outputs:
            if(matches==NULL){
                continue;
            }
            if(count==0){
                printf("no match was found.\n");
            }
            else if(flags==0){
                printf("%lli\n", matches[0].outpl);
            }
            else if(flags==1){
                for(int i=0; i<count-1; i++){
                    printf("%lli, ", matches[i].outpl);
                }
                printf("%lli\n", matches[count-1].outpl);
            }
            else if(flags==2){
                printf("%lli\n", matches[0].bw);
            }
            else if(flags==3){
                for(int i=0; i<count-1; i++){
                    printf("%lli, ", matches[i].bw);
                }
                printf("%lli\n", matches[count-1].bw);
            }
            else if(flags==4){
                if(at>count){
                    printf("there are less than %d matches.\n", at);
                    continue;
                }
                printf("%lli\n", matches[at-1].outpl);
            }
            else if(flags==6){
                if(at>count){
                    printf("there are less than %d matches.\n", at);
                    continue;
                }
                printf("%lli\n", matches[at-1].bw);
            }
            else if(flags==8){
                printf("%d\n", count);
            }
            else{
                printf("wrong order. use another combination of attributes.\n");
            }
            free(matches);
        }

        else if(!strcmp(order, "replace")){
            char* ptr = line+8;
            char stext[LINE_SIZE];
            char rtext[LINE_SIZE];
            long long at=0;
            int flag=0;
            ptr = findtext(ptr, stext);
            if(ptr==NULL){
                continue;
            }
            ptr = findtext(ptr, rtext);
            if(ptr==NULL){
                continue;
            }
            char filename[MAX];
            ptr = findfilename(ptr, filename);
            if(ptr==NULL){
                continue;
            }
            if(ptr[0]=='\0'){
            }
            else if(!strncmp(ptr, "-at", 3)){
                ptr+=4;
                char atval[LINE_SIZE];
                atval[0]='\0';
                while(ptr[0]!='\0'){
                    if((int)(ptr[0]-'0') > 9 && (int)(ptr[0]-'0') < 0){
                        printf("invalid command. attribute can be -at or -all.\n");
                        continue;
                    }
                    char tmp[2];
                    tmp[0]=ptr[0];
                    tmp[1]='\0';
                    strcat(atval, tmp);
                    ptr++;
                }
                at = strtolli(atval);
                flag =1;
            }
            else if(!strncmp(ptr, "-all", 4)){
                if(ptr[4]!='\0'){
                    printf("invalid command. attribute can be -at or -all.\n");
                    continue;
                }
                flag = 2;
            }
            else{
                printf("invalid command. attribute can be -at or -all.\n");
                continue;
            }
            replace(filename, stext, rtext, flag, at);
        }

        else{
                printf("invalid command.\n");
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
            printf("invalid address.\n");
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
          printf("operation done successfully.\n");
        }
        else{
          printf("file already exist\n");
          fclose(f);
        }
    }
    chdir(my_add);
}

char* findfilename(char *text, char *hold){
    if(strncmp(text, "--file ", 7)){
        printf("invalid command. use --file.\n");
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
        printf("invalid file address. use \\ at the begining of address.\n");
        return NULL;
    }
    return s+1;
}

char* findtext(char *text, char *hold){
    if(strncmp(text, "--str ", 6)){
        printf("invalid command. use --str.\n");
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
        long long i = s-text;
        *(hold+i) = '\0';
        for(long long i=0; hold[i+1]!='\0'; i++){
            hold[i]=hold[i+1];
            hold[i+1]='\0';
        }
        return s+2;
    }
    while(*s!=' ' && *s!='\0'){
        s++;
    }
    long long i = s-text;
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
        printf("invalid command. use --pos.\n");
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
        printf("invalid line or column number.\n");
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
    long long count=1;
    char tmp;
    while((tmp=getc(f))!=EOF){
        if(tmp=='\n'){
            count++;
        }
    }
    fseek(f, 0, SEEK_SET);
    if(line>count){
        printf("such position doesn't exist in file!\n");
        return;
    }
    for(long long i=0; i<line-1; i++){
        while(getc(f)!='\n'){
        }
    }
    for(long long i=0; i<col; i++){
        tmp = getc(f);
        if(tmp=='\n'){
            printf("such position doesn't exist in file!\n");
            return;
        }
    }
    for(long long i=0; text[i]!='\0'; i++){
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
    printf("operation done successfully.\n");
}

void insertn(FILE* f, long long p, char c){
    fseek(f, p, SEEK_SET);
    shiftfile(f, 1);
    if(c=='\n'){
        shiftfile(f, 1);
    }
    putc(c, f);
}

void shiftfile(FILE* f, int n){
    FILE *g = fopen("backups\\$func_shift.txt", "w");
    long long w = ftell(f);
    char a;
    while((a=getc(f))!=EOF){
        putc(a, g);
    }
    fclose(g);
    FILE *h = fopen("backups\\$func_shift.txt", "r");
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
    printf("here is the file content:\n");
    printf("----------------------------\n");
    while((a=getc(f))!=EOF){
        printf("%c", a);
    }
    printf("\n----------------------------\n");
}

char* findsize(char *ptr, long long *size){
    if(strncmp(ptr, "--size ", 7)){
        printf("invalid command. use --size.\n");
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

int checkpos(char name[], long long line, long long col, long long *ppos){
    FILE *f = fopen(name, "r+");
    long long count=1;
    char tmp;
    while((tmp=getc(f))!=EOF){
        if(tmp=='\n'){
            count++;
        }
    }
    fseek(f, 0, SEEK_SET);
    if(line>count){
        return 1;
    }
    for(long long i=0; i<line-1; i++){
        while(getc(f)!='\n'){
        }
    }
    for(long long i=0; i<col; i++){
        tmp = getc(f);
        if(tmp=='\n'){
            return 1;
        }
    }
    *ppos = ftell(f);
    fclose(f);
    return 0;
}

int removestr(char name[], long long line, long long col, long long size, char flag, int print){
    if(!addcheck(name)){
        printf("such file doesn't exist!\n");
        return -1;
    }
    name++;
    long long *ppos = (long long*)malloc(sizeof(long long));
    if(checkpos(name, line, col, ppos)){
        printf("such position doesn't exist in file!\n");
        return -1;
    }
    FILE *f = fopen(name, "r+");
    long long pos = *ppos;
    free(ppos);
    fseek(f, 0, SEEK_END);
    long long e = ftell(f);
    fseek(f, pos, SEEK_SET);
    FILE *g = fopen("backups\\$func_remove.txt", "w");
    if(flag=='b'){
        long long beg;
        char tmp;
        for(long long i=0; i<size; i++){
            if(ftell(f)==0){
                printf("there aren't enough charachters! decrease size or change position.\n");
                return -1;
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
        FILE *g2 = fopen("backups\\$func_remove.txt", "r");
        while((tmp=getc(g2))!=EOF){
            putc(tmp, f);
        }
        fclose(g2);
        fclose(f2);
    }
    else{
        long long beg = pos;
        char tmp;
        for(long long i=0; i<size; i++){
            if(ftell(f)==e){
                printf("there aren't enough charachters! decrease size or change position.\n");
                return -1;
            }
            tmp = getc(f);
            // if(tmp=='\n'){
                // fseek(f, 1, SEEK_CUR);
            // }
        }
        long long pos = ftell(f);
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
        FILE *g2 = fopen("backups\\$func_remove.txt", "r");
        while((tmp=getc(g2))!=EOF){
            putc(tmp, f);
        }
        fclose(g2);
        fclose(f2);
    }
    if(print){
        printf("operation done successfully.\n");
    }
    return 0;
}

void copystr(char name[], long long line, long long col, long long size, char flag, int cut){
    if(!addcheck(name)){
        printf("such file doesn't exist!\n");
        return;
    }
    name++;
    long long *ppos = (long long*)malloc(sizeof(long long));
    if(checkpos(name, line, col, ppos)){
        printf("such position doesn't exist in file!\n");
        return;
    }
    FILE *f = fopen(name, "r+");
    long long pos = *ppos;
    free(ppos);
    fseek(f, 0, SEEK_END);
    long long e = ftell(f);
    fseek(f, pos, SEEK_SET);
    FILE *g = fopen("backups\\$func_copy2.txt", "w");
    if(flag=='b'){
        long long beg;
        char tmp;
        for(long long i=0; i<size; i++){
            if(ftell(f)==0){
                printf("there aren't enough charachters! decrease size or change position.\n");
                return;
            }
            fseek(f, -1, SEEK_CUR);
            tmp=getc(f);
            fseek(f, -1, SEEK_CUR);
            if(tmp=='\n'){
                fseek(f, -1, SEEK_CUR);
            }
        }
        while(ftell(f)!=pos){
            putc(getc(f), g);
        }
        fclose(g);
    }
    else{
        for(long long i=0; i<size; i++){
            if(ftell(f)==e){
                printf("there aren't enough charachters! decrease size or change position.\n");
                return;
            }
            putc(getc(f), g);
        }
        fclose(g);
    }
    if(cut){
        name--;
        removestr(name, line, col, size, flag, 1);
    }
    fclose(f);
    FILE *h = fopen("backups\\$func_copy2.txt", "r");
    FILE *j = fopen("backups\\$func_copy.txt", "w");
    char tmp;
    while((tmp=getc(h))!=EOF){
        putc(tmp, j);
    }
    fclose(h); fclose(j);
    if(!cut){
        printf("operation done successfully.\n");
    }
}

void pastestr(char name[], long long line, long long col){
    if(!addcheck(name)){
        printf("such file doesn't exist!\n");
        return;
    }
    name++;
    long long *ppos = (long long*)malloc(sizeof(long long));
    if(checkpos(name, line, col, ppos)){
        printf("such position doesn't exist in file!\n");
        return;
    }
    FILE *f = fopen(name, "r+");
    long long pos = *ppos;
    free(ppos);
    fseek(f, pos, SEEK_SET);
    FILE *g = fopen("backups\\$func_copy.txt", "r");
    char tmp;
    while((tmp=getc(g))!=EOF){
        shiftfile(f, 1);
        putc(tmp, f);
    }
    fclose(g);
    fclose(f);
    printf("operation done successfully!\n");
}

struct finder* find(char name[], char text[], int *counter){
    int count=0;
    struct finder *matches = (struct finder*)malloc(sizeof(struct finder)*LINE_SIZE);
    if(!addcheck(name)){
        printf("such file doesn't exist!\n");
        return NULL;
    }
    name++;
    FILE *f = fopen(name, "r");
    char line[LINE_SIZE];
    int star=strlen(text);
    char fp[LINE_SIZE];
    char sp[LINE_SIZE];
    sp[0]='\0';
    fp[0]='\0';
    int hasstar=0;
    for(long long i=0; text[i]!='\0'; i++){
        if(text[i]=='*' && text[i-1]!='\\'){
            star = i;
            hasstar=1;
            break;
        }
    }
    for(long long i=0; text[i]!='\0'; i++){
        if(text[i]=='\\' && text[i+1]=='*'){
            if(i<star){
                star--;
            }
            for(int j=0; text[j+1]!='\0'; j++){
                text[j]=text[j+1];
            }
        }
    }
    for(long long i=0; i<star; i++){
        fp[i]=text[i];
    }
    if(star!=strlen(text)){
        for(long long i=star+1; text[i]!='\0'; i++){
        sp[i-star-1]=text[i];
        sp[i-star]='\0';
    }
    }
    fp[star]='\0';
    char *p = line;
    while(1){
        if(getc(f)==EOF){
            break;
        }
        fseek(f, -1, SEEK_CUR);
        long long bl = ftell(f);
        fgets(line, LINE_SIZE, f);
        p=line;
        char *fpm[LINE_SIZE];
        int fpmc=0;
        char *spm[LINE_SIZE];
        while((p=strstr(p, fp))!=NULL){
            fpm[fpmc] = p;
            fpmc++;
            p++;
        }
        if(fpmc==0){
            continue;
        }
        for(int i=0; i<fpmc; i++){
            char *check = fpm[i]+strlen(fp);
            char *r=NULL;
            if(!strncmp(check, sp, strlen(sp))){
                r=check;
            }
            while(check[0] != ' ' && check[0] != '\0'){
                if(!strncmp(check, sp, strlen(sp))){
                    r=check;
                }
                check++;
            }
            if(r==NULL){
                spm[i]=NULL;
                continue;
            }
            spm[i]=r;
        }
        long long fpmp[fpmc];
        long long spmp[fpmc];
        for(int i=0; i<fpmc; i++){
            fpmp[i] = bl + (long long)(fpm[i]-line);
            if(spm[i]==NULL){
                spmp[i]=-1;
            }
            else{
                spmp[i] = bl + (long long)(spm[i]-line);
            }
        }
        for(int i=0; i<fpmc; i++){
            for(int j=i+1; j<fpmc; j++){
                if(spmp[j]==spmp[i]){
                    spmp[j]=-1;
                }
            }
        }
        for(int i=0; i<fpmc; i++){
            if(spmp[i]!=-1){
                matches[count].pl=fpmp[i];
                matches[count].epl=spmp[i]+strlen(sp)-1;
                matches[count].linepos=fpm[i]-line;
                count++;
            }
        }
    }
    fseek(f, 0, SEEK_SET);
    long long lnum=0;
    long long wnum=1;
    char tmp='\0';
    for(long long i=0; i<count; i++){
        long long where=matches[i].pl;
        while(ftell(f)!=where){
            tmp=getc(f);
            if(tmp==' '){
                wnum++;
            }
            else if(tmp=='\n'){
                lnum++;
                wnum++;
            }
        }
        matches[i].outpl = matches[i].pl - lnum;
        matches[i].byline=lnum+1;
        if(getc(f)!=' '){
            matches[i].bw = wnum;
        }
        else{
            matches[i].bw = wnum+1;
        }
        fseek(f, -1, SEEK_CUR);
    }
    fclose(f);
    *counter=count;    
    return matches;
}

void replace(char name[], char stext[], char rtext[], int flag, long long at){
    int *counter = (int*)(malloc(sizeof(int)));
    struct finder *matches = (struct finder*)malloc(sizeof(struct finder)*LINE_SIZE);
    matches = find(name, stext, counter);
    if(*counter==0){
        printf("no match was found.\n");
        return;
    }
    int count = *counter;
    free(counter);
    if(flag==0){
        int tmp = removestr(name, matches[0].byline, matches[0].linepos, matches[0].epl-matches[0].pl+1, 'f', 0);
        if(tmp!=-1){
            insert(name, rtext, matches[0].byline, matches[0].linepos);
        }
    }
    else if(flag==1){
        if(at>count){
            printf("there are less than %d matches.\n", at);
            return;
        }
        int tmp = removestr(name, matches[at].byline, matches[at].linepos, matches[at].epl-matches[at].pl+1, 'f', 0);
        if(tmp!=-1){
            insert(name, rtext, matches[at].byline, matches[at].linepos);
        }
    }
    else if(flag==2){
        FILE *g = fopen("backups\\$func_replace.txt", "w");
        name++; 
        FILE *f = fopen(name, "r");
        for(int i=0; i<count; i++){
            while(ftell(f)!=matches[i].pl){
                putc(getc(f), g);
            }
            fseek(f, matches[i].epl-matches[i].pl+1, SEEK_CUR);
            fputs(rtext, g);
        }
        char tmp;
        while((tmp=getc(f))!=EOF){
           putc(tmp, g);
        }
        fclose(g); fclose(f);
        FILE *h = fopen("backups\\$func_replace.txt", "r");
        FILE *j = fopen(name, "w");
        while((tmp=getc(h))!=EOF){
            putc(tmp, j);
        }
        fclose(h); fclose(j);
        printf("operation done successfully.\n");
    }
    free(matches);
}


