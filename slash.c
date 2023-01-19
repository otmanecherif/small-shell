#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <libgen.h>
#include <readline/readline.h>
#include <readline/history.h>
#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096
#define MAXCOM 1000 
#define MAXLIST 100
#define green "\033[32m"
#define red "\033[91m"
#define cyan "\033[36m"
#define white "\033[00m"




int val = 0;
char currentPath[1024]={"\0"},prevPath[1024]={"\0"};


/*void printDir()
{
	char cwd[1024]={'\0'};
    char path[1024]={'\0'};
	getcwd(cwd, sizeof(cwd));
    write(STDERR_FILENO,blue,sizeof(blue));

    if (strlen(cwd)>25){
        strcpy(path, "...");
        strcat(path, &cwd[strlen(cwd) - 22]);
        write(STDERR_FILENO,path,sizeof(path));
        write(STDERR_FILENO,white,sizeof(white));
    }
    else{
        write(STDERR_FILENO,cwd,sizeof(cwd));
        write(STDERR_FILENO,white,sizeof(white));
    }
	//printf("%s", cwd);
}*/

char* printDir(){
    char path[1024]={'\0'};
    char* res = malloc(sizeof(char)*1024);
    char* colorVal;
    char v[3];



    if(val==0){
        colorVal=green;
    }else{
        colorVal=red;
    }

    for (int i = 0; i < sizeof(char)*1024; i++) {
        res[i] = '\0';
    }
	//getcwd(cwd, sizeof(cwd));
    sprintf(v,"%d",val);
    if (strlen(currentPath)>25){

        
        strcpy(path,colorVal);
        strcat(path,"[");
        strcat(path,v);
        strcat(path,"]");
        strcat(path,cyan);
        strcat(path, "...");
        strcat(path, &currentPath[strlen(currentPath) - 22]);
        strcat(res,path);
        strcat(res,"$ ");
        strcat(res,white);
        
    }
    else{
        strcpy(res,colorVal);
        strcat(res,"[");
        strcat(res,v);
        strcat(res,"]");
        strcat(res,cyan);
        strcat(res,currentPath);
        strcat(res,"$ ");
        strcat(res,white);
    }
    return res;
}

/*int takeInput(char* str)
{
	char* buf;
    //char* prompt ="$ ";

	buf = readline("$ ");

	if (strlen(buf) != 0) {
		add_history(buf);
		strcpy(str, buf);
        free(buf);
		return 0;
	} else {
        free(buf);
		return 1;
	}
}*/


void parseSlash(char* str, char** parsed)
{
	int i;

	for (i = 0; i < MAXLIST; i++) {
		parsed[i] = strsep(&str, "/");

		if (parsed[i] == NULL)
			break;
		if (strlen(parsed[i]) == 0)
			i--;
	}

    for(i;i< MAXLIST; i++){
        parsed[i]=NULL;
    }
}


void parseSpace(char* str, char** parsed)
{
	int i;

	for (i = 0; i < MAXLIST; i++) {
		parsed[i] = strsep(&str, " ");

		if (parsed[i] == NULL)
			break;
		if (strlen(parsed[i]) == 0)
			i--;
	}

    for(i;i< MAXLIST; i++){
        parsed[i]=NULL;
    }
}

int CommandRunner(char** parsedArgs){
    int NoOfOwnCmds = 3, i, switchOwnArg = 0;
    char* ListOfOwnCmds[NoOfOwnCmds],*paths[MAXLIST];
    char tab[256];
    struct stat sb;
    

    ListOfOwnCmds[0] = "exit";
	ListOfOwnCmds[1] = "cd";
	ListOfOwnCmds[2] = "pwd";


    if(parsedArgs[0]== NULL ){
        write(STDOUT_FILENO,"SLASH : Unknown command\n",24);
            return 1;
    }

    for (i = 0; i < NoOfOwnCmds; i++) {
	    if (strcmp(parsedArgs[0], ListOfOwnCmds[i]) == 0) {
	    	switchOwnArg = i + 1;
	    	break;
	    }
	}
    
    switch (switchOwnArg)
    {
        case 1:
            if(parsedArgs[1]==NULL)
                exit(val);
            else{
                val = atoi(parsedArgs[1]);
                exit(val);
            }
                

        case 2:
            if(parsedArgs[1]== NULL){
                chdir(getenv("HOME"));
                strcpy(prevPath,currentPath);
                strcpy(currentPath,getenv("HOME"));
                return 0;
            }

            if(strcmp("-",parsedArgs[1])==0){
                if(strcmp(prevPath,"") != 0){
                    chdir(prevPath);
                    char* tmp = malloc(sizeof(prevPath));
                    strcpy(tmp,currentPath);
                    strcpy(currentPath,prevPath);
                    strcpy(prevPath,tmp);
                    free(tmp);
                    return 0;
                }else{
                    write(STDOUT_FILENO,"cd - : previeus path undefined \n",33);
                    return 1;
                }
            }

            if(strcmp("-P",parsedArgs[1])==0){
                chdir(currentPath);
                if(chdir(parsedArgs[2])==-1){
                    write(STDOUT_FILENO,"cd : Invalide path\n",20);
                    return 1;
                }
                else{
                    strcpy(prevPath,currentPath);
                    getcwd(currentPath,sizeof(currentPath));
                    return 0;
                }
            }

	        if(strcmp("-L",parsedArgs[1])==0 || parsedArgs[2] == NULL){
                const char s[2] = "/";
                char tmp[1024];
                char cd_arg[1024];
                char tmp_current_path[1024];
                    
                char *token;
                char test[128];

				if(strcmp("-L",parsedArgs[1])==0){
                    strcpy(parsedArgs[1],parsedArgs[2]);
                }

                strcpy(tmp,currentPath);
                chdir(currentPath);
                if(chdir(parsedArgs[1])==-1){
                    write(STDOUT_FILENO,"cd : Invalide path\n",20);
                    return 1;
                }

                strcpy(prevPath,currentPath);
                if(strncmp(parsedArgs[1],"/",1) == 0 ){
                    strcpy(prevPath,currentPath);
                    strcpy(currentPath,parsedArgs[1]);
                    return 0 ;
                }
                

                strcpy(tmp_current_path,currentPath);
                strcpy(cd_arg,parsedArgs[1]);
                strcpy(tmp,currentPath);
                strcat(tmp,"/");
                token = strtok(parsedArgs[1], s);
                 
                while( token != NULL ) {
                    
                    if(strcmp(token,".")==0)
                        continue;
                    if(strcmp(token,"..")==0){
                        strcpy(currentPath,dirname(currentPath));
                    }else{
                        strcat(test,"/");
                        strcat(test,token);
                        strcat(test,"/");
                        if(strstr(tmp,test)!=NULL){
                            chdir(tmp_current_path);
                            chdir(cd_arg);
                            getcwd(currentPath,sizeof(currentPath));
                            break;
                        }
                        else{
                            strcat(currentPath,"/");
                            strcat(currentPath,token);
                        }     
                    }
                    test[0]='\0';
                    
                    token = strtok(NULL, s);
                }
                return 0;
                /*} else {
                    if(chdir(parsedArgs[1])==-1){
                        write(STDOUT_FILENO,"cd : Invalide path\n",20);
                        return 1;
                    }
                    else{
                        strcpy(prevPath,currentPath);
                        getcwd(currentPath,sizeof(currentPath));
                        return 0;
                    }
                }*/


			}
            

            return 0;
        
        case 3:
            if(parsedArgs[1]== NULL){
                printf("%s\n",currentPath);
                return 0;
            }
            if (strcmp(parsedArgs[1], "-P") == 0){
                chdir(currentPath);
                getcwd(tab, sizeof(tab));
                printf("%s\n",tab);
                return 0;
            }
            else if (strcmp(parsedArgs[1], "-L")== 0){
                printf("%s\n",currentPath);
                return 0;
            }
            return 1;

        default:
            write(STDOUT_FILENO,"SLASH : Unknown command\n",24);
            return 1;
    }
}

int main(int argc, char * argv[]){
    char *inputString, *parsedArgs[MAXLIST];
	char* parsedArgsPiped[MAXLIST];
    char *colorPrompt = cyan;
	char *colorStartPrompt = green;
	char *colorResult = white;
	int res = 0;

    rl_outstream =  stderr;
    getcwd(currentPath,sizeof(currentPath));
   
    while(1){
        
        char* prompt=printDir();
        inputString = readline(prompt);

	    if (inputString != NULL) {
		    add_history(inputString);

            parseSpace(inputString,parsedArgs);

            val = CommandRunner(parsedArgs);

            free(inputString);
            free (prompt);
        }
        else{
            return val;
        }
    }

}


