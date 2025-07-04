#include"main.h"

char *external[200];
int status;
int id;
int flag=0;

int main()
{
	char input_string[100];
	char prompt[100]="minishell$";
	system("clear");
	scan_input(prompt,input_string);

}
int check_command_type(char *command)
{
	char *builtins[28] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};
	
	for(int i=0;i<27;i++)
	{
		if(strcmp(builtins[i],command)==0)
		{
			return BUILTIN;
		}
	}
	for(int i=0;i<153;i++)
	{
		if(strcmp(external[i],command)==0)
		{
			return EXTERNAL;
		}
	}
	return NO_COMMAND;

}

void extract_external_commands(char *external[200])
{
	int fd=open("cmd.txt",O_RDONLY);
	char ch;
	int count=0,i=0,j=0;
	char str[30];
	while(read(fd,&ch,1)!=0)
	{
		if(ch=='\n')
		{
			str[j]='\0';
			external[i]=malloc(count);
		        strcpy(external[i],str);
			i++;
			j=0;
			count=0;
		        continue;	

		}
		str[j++]=ch;
		count++;

	}

}

char *get_command(char *input_string)
{
	int i=0;
	while(input_string[i]!=' '&& input_string[i]!='\0')
	{
		i++;
	}
	char *str=malloc(i+1);
	int j;
	for( j=0;j<i;j++)
	{
		str[j]=input_string[j];
	}
	str[j]='\0';
	return str;
}
void handler(int n)
{
	printf("\nminishell$ ");
	fflush(stdout);
}

void scan_input(char *prompt,char *input_string)
{
	extract_external_commands(external);
	//signal(SIGINT,handler);
	//signal(SIGTSTP,handler);

	while(1)
	{
		 signal(SIGINT,handler);
                 signal(SIGTSTP,handler);
		printf("%s ",prompt);
		__fpurge(stdin);
		int n=scanf("%[^\n]",input_string);
		if(n==0)
		{
			continue;
		}
		//validate i/p is ps1 or not
		if(strstr(input_string,"PS1=")!=NULL)
		{
			if(input_string[4]==' ')
			{
				printf("command ps1 is not found\n");
				continue;
			}
			else
			{
				strcpy(prompt,input_string+4);
				continue;
			}

		}
		else
		{
			char *cmd=get_command(input_string);
			int ret=check_command_type(cmd);

			if(ret==EXTERNAL)
			{
				int status;
				id=fork();//create child process
			          if(id>0)      //parent
				    {
					waitpid(id,&status,WUNTRACED);

				    }
				 else if(id==0)//child
				    {
					    signal(SIGINT,SIG_DFL);
					    signal(SIGTSTP,SIG_DFL);
					execute_external_commands(input_string);
					exit(0);
	         	            }
	         
		        }
			else if(ret==BUILTIN)
			{
			        execute_internal_commands(input_string);
			}
		}


	}
}
void execute_external_commands(char *input_string)
{
	//cheeck for pipes
	//if present--> n pipes
	//else
	int count=0,n=0,k=0,pipe_count=0;
       for(int  i=0;i<strlen(input_string);i++)
        {
                if(input_string[i]==' ')
                {
                        count++;
                }
		if(input_string[i]=='|')
		{
          		pipe_count++;
		}
        }
        char *str[count+1];
        char *token;
        token=strtok(input_string," ");
        while(token!=NULL)
        {
                str[n]=malloc(strlen(token+1));
                strcpy(str[n++],token);
                token=strtok(NULL," ");
        }
        str[count+1]=NULL;
	if(pipe_count==0)
	{
	execvp(str[0],str);
	}
	else
	{
		
		n_pipes(str);
		
	}

}
void n_pipes(char *argv[])
{
	int count =1;
        int arr[5];
        arr[0]=0;
        for(int i=0;argv[i]!=NULL;i++)
        {
                if(strcmp(argv[i],"|")==0)
                {
                        argv[i]=NULL;
                        arr[count++]=i+1;

                }
        }

        int fd[2];
        for(int i=0;i<count;i++)
        {
                if(i<count-1){
                        pipe(fd);
                }
                        int ret=fork();
                        if(ret==0)
                        {
                            if(i<count-1){
                                close(fd[0]);
                                dup2(fd[1],1);
                            }
                                execvp(argv[arr[i]],argv+arr[i]);
                                exit(0);
                        }
                        else if(ret>0 && i<count-1){
                                close(fd[1]);
                                dup2(fd[0],0);
                                close(fd[0]);
			}
			waitpid(ret,&status,WUNTRACED);
	}
}
void execute_internal_commands(char *input_string)
{
	char buf[100];
	if(strcmp("exit",input_string)==0)
	{
		exit(0);
	}
	else if(strcmp("pwd",input_string)==0)
	{
	 	getcwd(buf,100);
		printf("%s\n",buf);
	}
	else if(strstr("cd",input_string)!=NULL)
	{
		int ret=chdir(input_string+3);
		if(ret==-1)
		{
			printf("Directory change failed\n");
		}
	}
       else if(strcmp("echo $?",input_string)==0)
	{
		
		printf("exit status: %d\n",status);
		

	}
       else if(strcmp("echo $$",input_string)==0)
	{
		printf("msh PID : %d\n",getpid());

	}
	else if(strcmp("echo $SHELL",input_string)==0)
	{
		
		char *env=getenv("SHELL");
		printf("%s\n",env);

	}


	
}

