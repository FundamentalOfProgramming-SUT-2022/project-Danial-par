#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <dirent.h> 
#include <ncurses.h>
struct screen{
    int line;
    char filename[100];
    int IsSaved;
    char mode[7];
};

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

int main(){
    use_proj("createfile --file /root/.output_on_screen.txt");
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_CYAN);
    init_pair(3, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(4, COLOR_BLACK, COLOR_BLACK);
    init_screen("NORMAL", "tmp.txt", 0);
    attron(COLOR_PAIR(1));
    struct screen screen;
    screen.line = 1;
    strcpy(screen.filename, "/root/.newfile.txt");
    screen.IsSaved = 0;
    strcpy(screen.mode, "NORMAL");
    attroff(stdscr);
    int cx=6, cy=0;
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
            if(ch==':' || ch=='/'){
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
                    if(strcmp(screen.filename, "/root/.newfile.txt")){
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
                    strcpy(command, "createfile --file ");
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
                        if(strcmp(screen.filename, "/root/.newfile.txt"))
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
                            update_output(filename);
                            printf("%s", filename);
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
                noecho();
                nodelay(stdscr, 1);
                attroff(stdscr);
            }
            else if(ch=='u'){
                use_proj("undo --file /root/.output_on_screen.txt");
            }
            else if(ch=='i'){
                strcpy(screen.mode, "INSERT");
            }
            else if(ch==KEY_F(1)){
                strcpy(screen.mode, "VISUAL");
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
                    refresh();
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

        }
    }
    endwin();
}