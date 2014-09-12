#include "myshell.h" 

int main (int argc, char *argv[])
{
	char buf [MAX_BUFFER], pwd [MAX_ARGS];
	char shell_path[MAX_ARGS]="shell=";
	char readme_path[MAX_ARGS]="readme_path=";
	char newpath[MAX_ARGS*1000];
	int len;  
   	strcpy(newpath,getenv("PATH"));
   	strcat(newpath,":");
	if(strcmp(argv[0],"./myshell")&&strcmp(argv[0],"myshell")) 
	{                 
		len=strlen(argv[0]);  
		while(len&&argv[0][len]!='/')   len-- ; 
		argv[0][len]='\0';           
		strcpy(pwd,argv[0]);    
		get_fullpath(pwd, argv[0]);  
		printf("%s\n",pwd);  
	}
	else  strcpy(pwd  ,getenv("PWD"));    
	strcat(newpath,pwd);   // strcat(newpath,getenv("PWD"));     
	setenv("PATH",newpath,1);// 将当前工作目录添加到PATH环境变量中
	strcat(shell_path,pwd);   //  strcat(shell_path, getenv("PWD"));
	strcat(shell_path,"/myshell");
	putenv( shell_path);         
	strcat(readme_path, pwd);  //   strcat(readme_path, getenv("PWD")); 
	strcat(readme_path,"/readme");         
	putenv(readme_path); // 将readme的文件路径添加到环境变量中，详见函数function  my_help   
	if(argc>1)  // 用户在终端输入例如 ./myshell a.bat  >c.txt 
	{
		strcpy(buf,"myshell ");
		int i;
		for(i=1;i<argc;i++)
		{
			strcat(buf,argv[i]);
			strcat(buf,"  ");
		}
		Execute(buf);// 执行批处理命令
	}
	else  // 若用户在终端输入./myshell
	{
		my_clear();
		fprintf(stderr, "Welcome Enter myshell!\nType \"help\" to see the help information.\n");          
		my_shell(stdin,NULL,NULL);
	}
	return 0 ; 
}


