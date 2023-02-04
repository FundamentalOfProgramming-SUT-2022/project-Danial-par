#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <dirent.h> 
#include <ncurses.h>
#define LINE_SIZE 400
struct screen{
    int line;
    char filename[100];
    int IsSaved;
    char mode[7];
};
struct finder{
    long long pl;
    long long epl;
    long long outpl;
    long long bw;
    long long byline;
    long long linepos;
};

int DirCheck(const char *path)
{
    struct stat stats;

    stat(path, &stats);

    // Check for file existence
    if (S_ISDIR(stats.st_mode))
        return 1;

    return 0;
}

int addcheck(char name[]){
    char my_add[10000];
    getcwd(my_add, 10000);
    char name2[strlen(name)+2];
    char filename[100];
    strcpy(name2, name);
    char* p = name2+1;
    p = strrchr(p, '/');
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

struct finder* find(char name[], char text[], int *counter){
    int count=0;
    struct finder *matches = (struct finder*)malloc(sizeof(struct finder)*LINE_SIZE);
    if(!addcheck(name)){
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
                if(hasstar){
                    matches[count].epl=spmp[i]+strlen(sp)-1;
                }
                else{
                    matches[count].epl=fpmp[i]+strlen(fp)-1;
                }
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
        matches[i].outpl = matches[i].pl;
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

void init_screen(char mode[], char filename[], int IsSaved){
    int r, c; 
    getmaxyx(stdscr, r, c);
    for(int i=0; i<r-1; i++){
        move(i, 0);
        printw("~");
    }
    attron(COLOR_PAIR(2));
    move(r-2, 0);
    printw("  %s", mode);
    int a, b;
    getyx(stdscr, a, b);
    for(int i=b; i<12; i++){
        printw(" ");
    }
    attroff(stdscr);
    move(r-2, 12);
    attron(COLOR_PAIR(3));
    printw("  %s", filename);
    if(!IsSaved){
        printw("  +");
    }
    getyx(stdscr, a, b);
    for(int i=b; i<c; i++){
        printw(" ");
    }
    attroff(stdscr);
    move(0, 0);
}

void use_proj(char input[]){
    FILE *f = fopen(".input.txt", "w");
    fprintf(f, "%s\nexit", input);
    fclose(f);
    system("./proj.out < .input.txt > .result.txt");
}

void print_st(char filename[], int line){
    attron(COLOR_PAIR(1));
    int xmax = getmaxx(stdscr);
    int ymax = getmaxy(stdscr);
    move(0, 0);
    FILE *f = fopen(filename, "r");
    int valid=1;
    for(int i=0; i<line-1; i++){
        char tmp;
        while((tmp=getc(f))!='\n'){
            if(tmp==EOF){
                valid=0;
                break;
            }
        }
    }
    if(!valid){
    }
    else{
        char tmp;
        int x, y;
        int dokhati=0;
        for(int i=0; i<getmaxy(stdscr)-2; i++){
            printw("%.4d  ", line+i-dokhati);
            while((tmp=getc(f))!='\n' && tmp!=EOF){
                getyx(stdscr, y, x);
                if(x==xmax-1){
                    move(y+1, 0);
                    printw(" ");
                    move(y+1, 6);
                    i++;
                    dokhati++;
                }
                printw("%c", tmp);
            }
            printw(" ");
            attron(COLOR_PAIR(4));
            printw("%c", 232);
            attroff(stdscr);
            printw("\n");
            if(tmp==EOF){
                break;
            }
        }
    }
    attroff(stdscr);
    fclose(f);
}

void print_st_highlight(char filename[], struct finder matches[], int n, int count){
    int line=1;
    if(matches[n].byline>3){
        line=matches[n].byline-3;
    }
    int find_num=n;
    attron(COLOR_PAIR(1));
    int xmax = getmaxx(stdscr);
    int ymax = getmaxy(stdscr);
    move(0, 0);
    FILE *f = fopen(filename, "r");
    int valid=1;
    for(int i=0; i<line-1; i++){
        char tmp;
        while((tmp=getc(f))!='\n'){
            if(tmp==EOF){
                valid=0;
                break;
            }
        }
    }
    if(!valid){
    }
    else{
        char tmp;
        int x, y;
        int dokhati=0;
        for(int i=0; i<getmaxy(stdscr)-2; i++){
            printw("%.4d  ", line+i-dokhati);
            while((tmp=getc(f))!='\n' && tmp!=EOF){
                if(ftell(f)==matches[find_num].pl+1){
                    attron(COLOR_PAIR(5));
                }
                getyx(stdscr, y, x);
                if(x==xmax-1){
                    move(y+1, 0);
                    printw(" ");
                    move(y+1, 6);
                    i++;
                    dokhati++;
                }
                printw("%c", tmp);
                if(ftell(f)==matches[find_num].epl+1){
                    attroff(stdscr);
                    find_num++;
                    if(find_num>count-1){
                        find_num=0;
                    }
                }
            }
            printw(" ");
            attron(COLOR_PAIR(4));
            printw("%c", 232);
            attroff(stdscr);
            printw("\n");
            if(tmp==EOF){
                break;
            }
        }
    }
    attroff(stdscr);
    fclose(f);
}

void fix_cursor(int *cx, int *cy){
    int ccx=*cx;
    int ccy=*cy;
    while(1){
        char tmp = inch();
        if(tmp==' ' && ccx!=6){
            ccx--;
            move(ccy, ccx);
        }
        else if(tmp==-24){
            ccx--;
            *cx=ccx; *cy=ccy;
            move(ccy, ccx);
            break;
        }
        else{
            move(*cy, *cx);
            break;
        }
    }
}

void find_pos(char* line, char* col){
    int x, y;
    getyx(stdscr, y, x);
    int cy=y;
    move(y, 0);
    while(inch()==' '){
        cy--;
        move(cy, 0);
    }
    int a=inch(); move(cy, 1);
    int b=inch(); move(cy, 2);
    int c=inch(); move(cy, 3);
    int d=inch(); 
    char tmp[2]; tmp[1]='\0';
    tmp[0] = a;
    strcat(line, tmp);
    tmp[0] = b;
    strcat(line, tmp);
    tmp[0] = c;
    strcat(line, tmp);
    tmp[0] = d;
    strcat(line, tmp);
    int column = (getmaxx(stdscr)-6)*(y-cy)+x-6;
    char ctmp[1000]; ctmp[0]='\0';
    if(column==0){
        tmp[0]=48;
        strcat(ctmp, tmp);
    }
    while(column>0){
        tmp[0]=(char)((column%10)+48);
        strcat(ctmp, tmp);
        column/=10;
    }
    for(int i=strlen(ctmp)-1; i>=0; i--){
        tmp[0]=ctmp[i];
        strcat(col, tmp);
    }
}

void update_output(char filename[]){
    FILE *f = fopen("root/.output_on_screen.txt", "w");
    FILE *g = fopen(filename+1, "r");
    char tmp;
    while((tmp=getc(g))!=EOF){
        putc(tmp, f);
    }
    fclose(f); fclose(g);
}

void update_file(char filename[]){
    FILE *f = fopen("root/.output_on_screen.txt", "r");
    FILE *g = fopen(filename+1, "w");
    char tmp;
    while((tmp=getc(f))!=EOF){
        putc(tmp, g);
    }
    fclose(f); fclose(g);
}

void clear_commandline(){
    int x, y;
    getyx(stdscr, y, x);
    move(getmaxy(stdscr)-1, 0);
    for(int i=0; i<getmaxx(stdscr); i++){
        printw(" ");
    }
    move(y, x);
}

long long find_place(long long line, long long col){
    FILE *f = fopen("root/.output_on_screen.txt", "r");
    for(long long i=0; i<line-1; i++){
        while(getc(f)!='\n'){
        }
    }
    for(long long i=0; i<col; i++){
        getc(f);
    }
    long long result=ftell(f);
    fclose(f);
    return result;
}

int main(){
    initscr();
    mkdir("root", 0777);
    FILE *init = fopen("root/.output_on_screen.txt", "w");
    fclose(init);
    FILE *ini = fopen(".result.txt", "w");
    fclose(ini);
    raw();
    keypad(stdscr, TRUE);
    noecho();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_CYAN);
    init_pair(3, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(4, COLOR_BLACK, COLOR_BLACK);
    init_pair(5, COLOR_WHITE, COLOR_YELLOW);
    init_screen("NORMAL", "tmp.txt", 0);
    attron(COLOR_PAIR(1));
    struct screen screen;
    screen.line = 1;
    strcpy(screen.filename, "/root/newfile.txt");
    screen.IsSaved = 0;
    strcpy(screen.mode, "NORMAL");
    attroff(stdscr);
    int cx=6, cy=0;
    int visual=1, visualx=6, visualy=0, visual_first_line=1;
    char vline[5];char vcol[500]; vline[0]='\0'; vcol[0]='\0';
    nodelay(stdscr, 1);
    while(1){
        clear();
        init_screen(screen.mode, screen.filename+6, screen.IsSaved);
        print_st("root/.output_on_screen.txt", screen.line);
        move(cy, cx);
        fix_cursor(&cx, &cy);
        refresh();
        usleep(60000);
        if(!strcmp(screen.mode, "NORMAL")){
            clear_commandline();
            int ch = getch();
            if(ch==':'){
                nodelay(stdscr, 0);
                move(getmaxy(stdscr)-1, 2);
                attron(COLOR_PAIR(1));
                printw("%c", ch);
                echo();
                char command[1000];
                getstr(command);
                if(!strcmp(command, "q")){
                    break;
                }
                else if(!strcmp(command, "save")){
                    int update=1;
                    if(strcmp(screen.filename, "/root/newfile.txt") && strcmp(screen.filename, "/root/ ")){
                        screen.IsSaved=1;
                    }
                    else{
                        int wrong=0;
                        while(1){
                            int maxy = getmaxy(stdscr);
                            move(maxy-1, 0);
                            if(!wrong){
                                printw("Pick a name for your file: ");
                                for(int i=28; i<getmaxx(stdscr); i++){
                                    printw(" ");
                                }
                                move(maxy-1, 27);
                            }
                            else if(wrong==2){
                                clear_commandline();
                                printw("Invalid name/address. Pick another name: ");
                                for(int i=42; i<getmaxx(stdscr); i++){
                                    printw(" ");
                                }
                                move(maxy-1, 41);
                            }
                            else{
                                printw("that file already exists. Pick another name: ");
                                for(int i=46; i<getmaxx(stdscr); i++){
                                    printw(" ");
                                }
                                move(maxy-1, 45);
                            }
                            char filename[200];
                            strcpy(filename, "/root/");
                            getstr(command);
                            if(command[0]==27){
                                update=0;
                                break;
                            }
                            strcat(filename, command);
                            strcpy(command, "createfile --file ");
                            strcat(command, filename);
                            use_proj(command);
                            FILE *f = fopen(".result.txt", "r");
                            if(getc(f)=='o'){
                                strcpy(screen.filename, filename);
                                screen.IsSaved=1;
                                fclose(f);
                                break;
                            }
                            else if(getc(f)=='i'){
                                wrong=1;
                            }
                            else{
                                wrong=2;
                            }
                            fclose(f);
                        }
                    }
                    if(update){
                        update_file(screen.filename);
                        clear_commandline();
                        move(getmaxy(stdscr)-1, 0);
                        printw("file saved successfully. press any key to continue.");
                        getch();
                    }
                }
                else if(!strncmp(command, "saveas", 6)){
                    char filename[200];
                    strcpy(filename, command+7);
                    strcpy(command, "createfile --file /root/");
                    strcat(command, filename);
                    use_proj(command);
                    FILE *f = fopen(".result.txt", "r");
                    if(getc(f)=='o'){
                        strcpy(screen.filename, filename);
                        update_file(screen.filename);
                        screen.IsSaved=1;
                        fclose(f);
                        clear_commandline();
                        move(getmaxy(stdscr)-1, 0);
                        printw("file saved successfully. press any key to continue.");
                        getch();
                    }
                    else if(getc(f)=='i'){
                        clear_commandline();
                        move(getmaxy(stdscr)-1, 0);
                        printw("a file with thar name already exists. press any key to continue.");
                        getch();
                    }
                    else{
                        clear_commandline();
                        move(getmaxy(stdscr)-1, 0);
                        printw("invalid file name/address. press any key to continue.");
                        getch();
                    }
                }
                else if(!strncmp(command, "open", 4)){
                    int open=1;
                    if(!screen.IsSaved){
                        if(strcmp(screen.filename, "/root/newfile.txt") && strcmp(screen.filename, "/root/ "))
                            update_file(screen.filename);
                        else{
                            clear_commandline();
                            int maxy=getmaxy(stdscr);
                            move(maxy-1, 0);
                            printw("warning: you'll use your changes at this file. are you sure?(press ESC to cancel)");
                            move(maxy-1, 83);
                            int ch = getch();
                            if(ch==27){
                                open=0;
                            }
                        }
                    }
                    if(open){
                        char order[100];
                        strcpy(order, "createfile --file /root/");
                        strcat(order, command+5);
                        use_proj(order);
                        char filename[100]; strcpy(filename, "/root/");
                        strcat(filename, command+5);
                        FILE *f = fopen(".result.txt", "r");
                        if(getc(f)=='o' || getc(f)=='i'){
                            strcpy(screen.filename, filename);
                            screen.IsSaved=1;
                            update_output(filename);
                            printf("%s", filename);
                            cx=6; cy=0;
                        }
                        else{
                            clear_commandline();
                            int maxy=getmaxy(stdscr);
                            move(maxy-1, 0);
                            printw("something went wrong.");
                            int ch=getch();
                        }
                        fclose(f);
                    }
                }
                else if(!strcmp(command, "undo")){
                    use_proj("undo --file /root/.output_on_screen.txt");
                }
                else if(!strcmp(command, "auto-indent")){
                    use_proj("auto-indent --file /root/.output_on_screen.txt");
                    cy=0; cx=6;
                    screen.line=1;
                }
                else if(!strncmp(command, "replace", 7)){
                    if(strstr(command, "--file")==NULL){
                        char batel[10]; char batel2[10]; char batel3[10]; char str1[30]; char str2[30]; 
                        char *ptr = command+15;
                        sscanf(ptr, "%s", str1);
                        ptr+=strlen(str1)+8;
                        sscanf(ptr, "%s", str2);
                        ptr+=strlen(str2)+1;
                        char order[100];
                        strcpy(order, "replace --str1 ");
                        strcat(order, str1);
                        strcat(order, " --str2 ");
                        strcat(order, str2);
                        strcat(order, " --file /root/.output_on_screen.txt ");
                        strcat(order, ptr);
                        use_proj(order);
                        screen.line=0; screen.IsSaved=0;
                    }
                    else{
                        use_proj(command);
                        update_output("/.result.txt");
                        strcpy(screen.filename, "/root/ ");
                        screen.line=1;
                        cx = 6; cy=0;
                        screen.IsSaved=0;
                    }
                }
                else if(!strncmp(command, "createfile", 10) || !strncmp(command, "insertstr", 9) || 
                        !strncmp(command, "cat", 3) || !strncmp(command, "removestr", 9) || !strncmp(command, "copystr", 7) 
                        || !strncmp(command, "cutstr", 6) || !strncmp(command, "pastestr", 8) || !strncmp(command, "find", 4)||
                        !strncmp(command, "grep", 4) || !strncmp(command, "replace", 7) || !strncmp(command, "undo", 4) ||
                        !strncmp(command, "auto-indent", 12) || !strncmp(command, "compare", 7) || !strncmp(command, "tree", 4)){
                    use_proj(command);
                    update_output("/.result.txt");
                    strcpy(screen.filename, "/root/ ");
                    screen.line=1;
                    cx = 6; cy=0;
                    screen.IsSaved=0;
                }
                noecho();
                nodelay(stdscr, 1);
                attroff(stdscr);
                }
            else if(ch=='/'){
                nodelay(stdscr, 0);
                int loop=1;
                move(getmaxy(stdscr)-1, 2);
                attron(COLOR_PAIR(1));
                printw("%c", ch);
                echo();
                char str[1000];
                getstr(str);
                noecho();
                struct finder *matches = (struct finder*)malloc(sizeof(struct finder)*LINE_SIZE);
                int count=0;
                matches = find("/root/.output_on_screen.txt", str, &count);
                if(count==0){
                    clear_commandline();
                    move(getmaxy(stdscr)-1, 0);
                    printw("no match was found.");
                    int ch=getch();
                    loop=0;
                }
                int n=0;
                if(loop){
                    if(matches[0].byline>3){
                        cy=3;
                    }
                    else{
                        cy=matches[0].byline-1;
                    }
                    cx=6+matches[0].linepos;
                }
                while(loop){
                    clear();
                    init_screen("NORMAL", screen.filename+6, screen.IsSaved);
                    print_st_highlight("root/.output_on_screen.txt", matches, n, count);
                    move(cy, cx);
                    refresh();
                    int ch=getch();
                    if(ch!='n'){
                        loop=0;
                        getyx(stdscr, cy, cx);
                        int line=matches[n].byline;
                        if(line>3){
                            screen.line=line-3;
                        }
                        else{
                            screen.line=1;
                        }
                    }
                    else{
                        n++;
                        if(n==count){
                            n=0;
                        }
                        if(matches[n].byline>3){
                            cy=3;
                        }
                        else{
                            cy=matches[n].byline-1;
                        }
                        cx=6+matches[n].linepos;
                    }
                }
                nodelay(stdscr, 1);
            }
            else if(ch=='u'){
                use_proj("undo --file /root/.output_on_screen.txt");
            }
            else if(ch=='i'){
                strcpy(screen.mode, "INSERT");
            }
            else if(ch=='v'){
                strcpy(screen.mode, "VISUAL");
            }
            else if(ch=='='){
                use_proj("auto-indent --file /root/.output_on_screen.txt");
                cy=0; cx=6;
                /* clear();
                init_screen(screen.mode, screen.filename+6, screen.IsSaved);
                print_st("root/.output_on_screen.txt", screen.line);
                move(cy, cx);
                refresh();
                nodelay(stdscr, 0);
                int ch=getch();
                nodelay(stdscr, 1); */
            }
            else if(ch=='p'){
                char line[5]; char col[50];
                line[0]='\0'; col[0]='\0';
                find_pos(line, col);
                char order[100];
                strcpy(order, "pastestr --file /root/.output_on_screen.txt --pos ");
                strcat(order, line);
                char tmp[]=":";
                strcat(order, tmp);
                strcat(order, col);
                use_proj(order);
            }
            else if(ch==KEY_UP){
                if(cy>3){
                    cy--;
                }
                else{
                    if(screen.line!=1)
                        screen.line--;
                    else{
                        if(cy!=0){
                            cy--;
                        }
                    }
                }
            }
            else if(ch==KEY_DOWN){
                if(cy<getmaxy(stdscr)-6){
                    move(cy+1, 0);
                    char tmp;
                    tmp = inch();
                    if(tmp!='~'){
                        cy++;
                    }
                }
                else{
                    char tmp;
                    move(cy+1, 0);
                    tmp = inch();
                    move(cy, cx);
                    if(tmp!='~'){
                        screen.line++;
                    }
                    else{
                    }
                }
            }
            else if(ch==KEY_LEFT){
                if(cx>6){
                    cx--;
                }
                else{
                }
            }
            else if(ch==KEY_RIGHT){
                char tmp;
                move(cy, cx+1);
                tmp = inch();
                move(cy, cx);
                if((int)(tmp)==-24){
                }
                else if(cx==getmaxx(stdscr)-1){
                    cx=6; cy++;
                }
                else{
                    cx++;
                }
            }
        }
    
        else if(!strcmp(screen.mode, "INSERT")){
            int ch = getch();
            if(ch==27){ //ESC
                strcpy(screen.mode, "NORMAL");
            }
            else if(ch==KEY_UP){
                if(cy>3){
                    cy--;
                }
                else{
                    if(screen.line!=1)
                        screen.line--;
                    else{
                        if(cy!=0){
                            cy--;
                        }
                    }
                }
            }
            else if(ch==KEY_DOWN){
                if(cy<getmaxy(stdscr)-6){
                    move(cy+1, 0);
                    char tmp;
                    tmp = inch();
                    if(tmp!='~'){
                        cy++;
                    }
                }
                else{
                    char tmp;
                    move(cy+1, 0);
                    tmp = inch();
                    move(cy, cx);
                    if(tmp!='~'){
                        screen.line++;
                    }
                    else{
                    }
                }
            }
            else if(ch==KEY_LEFT){
                if(cx>6){
                    cx--;
                }
                else{
                }
            }
            else if(ch==KEY_RIGHT){
                char tmp;
                move(cy, cx+1);
                tmp = inch();
                move(cy, cx);
                if((int)(tmp)==-24){
                }
                else if(cx==getmaxx(stdscr)-1){
                    cx=6; cy++;
                }
                else{
                    cx++;
                }
            }
            else if((ch<=128 && ch>0 )|| ch==KEY_BACKSPACE){
                char *line=(char*)malloc(sizeof(char)*5);
                char *col=(char*)malloc(sizeof(char)*1000);
                line[0]='\0'; col[0]='\0';
                find_pos(line, col);
                char order[100];
                if(ch!=KEY_BACKSPACE){
                    order[0]='\0';
                    strcat(order, "insertstr --file ");
                    strcat(order, "/root/.output_on_screen.txt");
                    strcat(order, " --str ");
                    char tmp[3]; tmp[1]='\0'; 
                    tmp[0]='"';
                    strcat(order, tmp);
                    if(ch=='"'){
                        tmp[0]='\\'; tmp[1]='"'; tmp[2]='\0';
                        strcat(order, tmp);
                        tmp[1]='\0';
                    }
                    else{
                        tmp[0]=ch;
                        if(ch=='\n'){
                            strcat(order, "\\n");
                        }
                        else if(ch=='\t'){
                            strcat(order, "    ");
                        }
                        else{
                            strcat(order, tmp);
                        }
                    }
                    tmp[0]='"';
                    strcat(order, tmp);
                    strcat(order, " --pos ");
                    strcat(order, line);
                    tmp[0]=':';
                    strcat(order, tmp);
                    strcat(order, col);
                    use_proj(order);
                    free(line); free(col);
                    if(ch!='\n' && ch!='\t'){
                        if(cx<getmaxx(stdscr)-2){
                            cx++;
                            move(cy, cx);
                            printw(" ");
                            move(cy, cx+1);
                            attron(COLOR_PAIR(4));
                            printw("%c", 232);
                            attroff(stdscr);
                        }
                        else if(cx==getmaxx(stdscr)-2){
                            cx++;
                            move(cy, cx);
                            printw(" ");
                            move(cy+1, 6);
                            attron(COLOR_PAIR(4));
                            printw("%c", 232);
                            attroff(stdscr);
                        }
                        else{
                            cy++; cx=6;
                            move(cy, cx);
                            printw(" ");
                            move(cy, cx+1);
                            attron(COLOR_PAIR(4));
                            printw("%c", 232);
                            attroff(stdscr);
                        }
                    }
                    else if(ch=='\n'){
                        if(cy<getmaxy(stdscr)-6){
                            cy++;
                            cx=6;
                        }
                        else
                            screen.line++; 
                        cx==6;
                    }
                    else{
                        cx+=4;
                    }
                }
                else if(!(cy==0 && cx==6)){
                    strcpy(order, "removestr --file /root/.output_on_screen.txt --pos ");
                    strcat(order, line);
                    char tmp[2]; tmp[1]='\0'; tmp[0]=':';
                    strcat(order, tmp);
                    strcat(order, col);
                    strcat(order, " --size 1 -b");
                    use_proj(order);
                    if(cx!=6)
                        cx--;
                    else{
                        cy--;
                        int maxx=getmaxx(stdscr);
                        cx=6;
                        move(cy, cx);
                        char tmp = inch();
                        while(tmp!=-24 && cx<maxx){
                            cx++;
                            move(cy, cx);
                            tmp=inch();
                        }
                        cx--;
                    }
                }
                screen.IsSaved=0;
            }    
        }
        
        else if(!strcmp(screen.mode, "VISUAL")){
            if(visual){
                visualx=cx, visualy=cy;
                char tmp[1]; tmp[0]='\0';
                strcpy(vline, tmp); strcpy(vcol, tmp);
                find_pos(vline, vcol);
                visual=0;
                visual_first_line=screen.line;
            }
            int ch=getch();
            if(ch==27){ //ESC
                strcpy(screen.mode, "NORMAL");
                visual=1;
            }
            else if(ch==KEY_UP){
                if(cy>3){
                    cy--;
                }
                else{
                    if(screen.line!=1)
                        screen.line--;
                    else{
                        if(cy!=0){
                            cy--;
                        }
                    }
                }
            }
            else if(ch==KEY_DOWN){
                if(cy<getmaxy(stdscr)-6){
                    move(cy+1, 0);
                    char tmp;
                    tmp = inch();
                    if(tmp!='~'){
                        cy++;
                    }
                }
                else{
                    char tmp;
                    move(cy+1, 0);
                    tmp = inch();
                    move(cy, cx);
                    if(tmp!='~'){
                        screen.line++;
                    }
                    else{
                    }
                }
            }
            else if(ch==KEY_LEFT){
                if(cx>6){
                    cx--;
                }
                else{
                }
            }
            else if(ch==KEY_RIGHT){
                char tmp;
                move(cy, cx+1);
                tmp = inch();
                move(cy, cx);
                if((int)(tmp)==-24){
                }
                else if(cx==getmaxx(stdscr)-1){
                    cx=6; cy++;
                }
                else{
                    cx++;
                }
            }
            else if(ch=='y' || ch=='d' || ch==KEY_BACKSPACE){
                char eline[5]; char ecol[500];
                eline[0]='\0'; ecol[0]='\0';
                int size=0;
                find_pos(eline, ecol);
                long long first=find_place(strtolli(vline), strtolli(vcol));
                long long last=find_place(strtolli(eline), strtolli(ecol));
                char flag[2]; flag[1]='\0';
                if(last>first){
                    size=last-first;
                    flag[0]='f';
                }
                else if(first>last){
                    size=first-last;
                    flag[0]='b';
                }
                else{
                    strcpy(screen.mode, "NORMAL");
                    visual=1;
                    continue;
                }
                char tmp[2]; tmp[1]='\0';
                char ssize[10];
                char reverse_ssize[10];
                reverse_ssize[0]='\0'; ssize[0]='\0';
                while(size>0){
                    tmp[0]=(size%10)+48;
                    strcat(reverse_ssize, tmp);
                    size/=10;
                }
                for(int i=strlen(reverse_ssize)-1; i>=0; i--){
                    tmp[0]=reverse_ssize[i];
                    strcat(ssize, tmp);
                }
                char command[200];
                strcpy(command, "--file /root/.output_on_screen.txt --pos ");
                strcat(command, vline);
                tmp[0]=':';
                strcat(command, tmp);
                strcat(command, vcol);
                strcat(command, " --size ");
                strcat(command, ssize);
                strcat(command, " -");
                strcat(command, flag);
                if(ch=='y'){
                    // copystr --file /root/.output_on_screen.txt --pos <vline>:<vcol> --size <string(size)> -<flag>
                    char order[210];
                    strcpy(order, "copystr ");
                    strcat(order, command);\
                    use_proj(order);
                    strcpy(screen.mode, "NORMAL");
                    visual=1;
                }
                else if(ch=='d'){
                    // cutstr --file /root/.output_on_screen.txt --pos <vline>:<vcol> --size <string(size)> -<flag>
                    char order[210];
                    strcpy(order, "cutstr ");
                    strcat(order, command);
                    use_proj(order);
                    strcpy(screen.mode, "NORMAL");
                    visual=1;
                    if(last>first){
                        screen.line=visual_first_line;
                        cx=visualx; cy=visualy;
                    }
                }
                else if(ch==KEY_BACKSPACE){
                    char order[210];
                    strcpy(order, "removestr ");
                    strcat(order, command);
                    use_proj(order);
                    strcpy(screen.mode, "NORMAL");
                    visual=1;
                    if(last>first){
                        screen.line=visual_first_line;
                        cx=visualx; cy=visualy;
                    }
                }
            }
        }
    }
    endwin();
}