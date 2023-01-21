#include <stdio.h>
#include <stdlib.h>
#include <dir.h>
#include <string.h>

#define LINE_SIZE 1000
#define ORDER_SIZE 50


//           functions          //
void createfile(char[]);
char* findfilename(char*);
char* rmg(char[]);
void insert(char[], char[], long long, long long);

//              //              //
int main(){
    mkdir("root");
    while(1){
        char line[LINE_SIZE];
        gets(line);
        char order[ORDER_SIZE];
        for(int i=0; i<1000 && line[i]!=' '; i++){
            order[i] = line[i];
            order[i+1] = '\0';
        }

        if(!strcmp(order, "exit")){
            break;
        }

        if(!strcmp(order, "createfile")){
            char afterorder[9] = {' '};
            afterorder[9] = '\0';
            for(int i=10; i<=17; i++){
                afterorder[i-10] = line[i];
            }
            if(strcmp(afterorder, " --file ")){
                printf("invalid command\n");
                continue;
            }
            char filename[1000];
            for(int j=18; j<1000 && line[j]!='\0'; j++){
                filename[j-18] = line[j];
                filename[j-17] = '\0';
            }
            createfile(rmg(filename));
            continue;
        }

        else if(!strcmp(order, "insertstr")){
            
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
    char * p = NULL;
    char cpy[strlen(name)];
    strcpy(cpy, name);
    if(1){
        char file[strlen(name)];
        p = strrchr(cpy, '/');
        strcpy(file, p+1);
        char add[strlen(name)];
        *p = '\0';
        p = strtok(cpy, "/");
        while(p!=NULL){
            add[0] = '.'; add[1] = '/'; add[2] = '\0';
            strcat(add, p);
            mkdir(add);
            chdir(add);
            p = strtok(NULL, "/");
        }
        chdir(cpy);
        FILE* f = fopen(file, "r+");
        if(!f){
          FILE* fil = fopen(file, "w");
          fclose(fil);
        }
        else{
          printf("File already exist\n");
          fclose(f);
        }
    }
}

char* findfilename(char *text){
    
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

}