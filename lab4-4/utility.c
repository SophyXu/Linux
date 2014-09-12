// 支持外部命令
#include "myshell.h"

int back_bat=0; // 标志既是后台运行，又是批处理；值为1有效。
int output_num=0; // 批处理的输出重定向个数
char batchfile[MAX_PATH] ; // 当前的批处理文件
int bat_line=0;// 批处理的命令行数
int isbat=0;// 批处理标志
int letter; // 在my_strtok( )   Error( )中使用
char *open; // 在my_strtok( )   Error( )中使用


/*辅助函数*/ 
/*my_strtok函数：（1）将输入命令行（储存在buf[]中）读入args[]*/
/*my_strtok函数：（2）在states[]中记录execute,argc等等*/
/*my_strtok函数：（3）记录重定向输入输出文件*/
int my_strtok(char *buf,char **args,int *states,Redirect *Inputs,Redirect *Outputs) // InPut和OutPuts记录重定向
{      
	int i,j,n,m, flag,argc,errortype; // flag是空白标记；argc是参数个数(不包括重定向和后台运行）
	char c ; 
	states[0]=states[1]=states[2]=states[3]=states[4]=0; // states[0]是后台运行标志；states[1]是输入重定向个数；states[2]是输出重定向个数；
                                                         //states[3]是参数个数； states[4]标记输入重定向在键盘输入之前；
	errortype=letter=argc=0;
	args[0]=NULL;  
	open=NULL;// open是打开方式
	flag=1;
	i=m=n=-1;  
	while(buf[++i]&&buf[i]!='#') //扫描命令行,允许用户把 '#'用于注释
	{             
		c=buf[i]; 
		switch(c) //  字符解析
		{                       
			case '<':  
				letter++;  
				if(flag==0)
				{   
					flag=1;	
					buf[i]='\0';	
				}
				open="<"; // "r" opentype  
				while(buf[++i]==' '|| buf[i]=='\t'); //跳过多个连续的空格  
				if(buf[i]<32|| buf[i]=='#' ) // <输入无效，无输入文件
				{  
					errortype=Error(1,NULL,NULL,NULL,"<");
					break; 
				}
				else if(buf[i]=='&'|| buf[i]=='<'|| buf[i]=='>'|| buf[i]=='|'|| buf[i]==';') // 不能是<& <; << <> <|    
				{
					letter++; 
					errortype=Error(2,NULL,NULL,NULL,buf+i);
					break;
				}				  
				if(argc<2)
					states[4]=1;
				m++;                               
				i--;  						  	  
				break; 
			case '>':    
				letter++; 
				if(flag==0)					  
				{    
					flag=1;					      
					buf[i]='\0'; 
				}	
				n++;				                      
				if(buf[i+1]=='>') 					    
				{    
					buf[++i]='\0';						 
					open=">>"; // "a"的打开类型
				} 
				else 	
				{    						            
					open=">";  // "w"的打开类型
				}
				while(buf[++i]==' '|| buf[i]=='\t'); // 跳过多个连续的空格      
				if(buf[i]<32|| buf[i]=='#' )
				{    						
					errortype=Error(1,NULL,NULL,NULL,NULL);
					break; 
				}
				else if(buf[i]=='&'|| buf[i]=='<'|| buf[i]=='>'|| buf[i]=='|' || buf[i]==';') // 不能是>&  <; >< <|    
				{
					letter++; 						
					errortype=Error(2,NULL,NULL,NULL,buf+i); 
					break; 
				}
				i--; 
				break; 
			case '&':    
				letter++;
				if(flag==0)					  
				{  
					flag=1;    
					buf[i]='\0';
				}
				if(states[0]) 					  
				{  
				  errortype= Error(0,NULL,NULL,NULL,"Format Error: argument '&' occurs more than once!"); 					break; 
				}			           
				states[0]=1; 				           
				break;
	  
			case ' ':
			case '\t': 	 
				if(flag==0) 
				{	
					flag=1; 
					buf[i]='\0'; 		  
				} 
				while(buf[++i]==' '|| buf[i]=='\t');  // 跳过多个连续的空格      
				i--;
				break; 

			case '\n':
			case '\r':	  
				buf[i]='\0';
				i--;
				break; 		
			default: 	 
				letter++; 
				if(flag)					   
				{      
					flag=0;
					if(open&&m<=MAX_OPEN&&n<=MAX_OPEN)
					{
						if(m==MAX_OPEN)// 过多输入重定向
						errortype=Error(5,NULL,NULL,NULL,"input");  
						else  if(n==MAX_OPEN)// 过多输出重定向
							errortype=Error(5,NULL,NULL,NULL,"output");
						else if( !strcmp(open,"<" ) )
							Inputs[m].filename=buf+i; 
						else  if( !strcmp(open,">>" ) )
						{
							strcpy(Outputs[n].opentype,"a"); 
							strcpy(Outputs[n].open,">>"); 
							Outputs[n].filename=buf+i;
						}
						else  if( !strcmp(open,">" ) )
						{
							strcpy(Outputs[n].opentype,"w"); 
							strcpy(Outputs[n].open,">"); 
							Outputs[n].filename=buf+i;
						}
						open=NULL;
					}					      
					else 
						args[argc++]=buf+i;
				}
				if(c=='\\'  &&buf[i+1]==' ')   //  转义字符，例如"a\ b"在文件或目录名称中代表"a b"  
				{     
					buf[i]=' ';
						if( ! isspace(buf[i+2]))
						{     
							j=i+1; 
							while(  buf[++j])  
								buf[j-1]=buf[j]; 
						}                                                                                                                      
				}  	
		} // switch结束
	} // loop循环结束
	args[argc]=NULL; // args以NULL结束 
	states[1]=m+1;//   0,12,...,m
	states[2]=n+1;//  0,1,2,...,n
	states[3]=argc;
	if(errortype||(argc==0&&letter))   
 		Error(0,NULL,NULL,NULL,"Warning: nothing will be executed!");     
	return errortype;					
}



/*函数my_shell：从标准输入或输入文件中读取命令，并调用函数Execute(）执行命令*/ 
int my_shell(FILE *inputfile,const Redirect *Outputs,const int *states)
{	       
	FILE *outputfile;
	char filepath[MAX_PATH];
	char buf[MAX_BUFFER];
	char _HOME[MAX_PATH];
	char _PWD[MAX_PATH];
	char _PATH[MAX_PATH];
	char _HOSTNAME[MAX_PATH];
	int done=0;  //
	if(Outputs)
	{
		get_fullpath(filepath,Outputs->filename);
 		outputfile=freopen(filepath,Outputs->opentype,stdout);
		if(outputfile==NULL)
		{     
			Error(-6,NULL,NULL,NULL,Outputs->filename);		
			return -2;
		}
		fprintf(stderr,"\nThe results will be writen into file \"%s\".\n",Outputs->filename); 
	} 
	bat_line=0;
	do    // /* 保持读状态直到quit命令或eof*/
	  {   //从输入文件中读取命令
	    if (inputfile==stdin&&Outputs==NULL) {

		gethostname(_HOSTNAME, sizeof(_HOSTNAME));
		strncpy(_HOME, getenv("HOME"), MAX_PATH - 1);
		strncpy(_PWD, getenv("PWD"), MAX_PATH - 1);
		int tmp = strlen(_HOME);
		if(!strncmp(_HOME, _PWD, tmp)) {
		    strcpy(_PATH, "~");
		    strncat(_PATH, _PWD + tmp, strlen(_PWD) - tmp);
		    fprintf(stderr,"\n%s@%s:%s$ ",getenv("USER"), _HOSTNAME, _PATH);
		}
		else 
		    fprintf(stderr,"\n%s@%s:%s$ ",getenv("USER"),_HOSTNAME, _PWD);
	    }
	    if (fgets(buf, MAX_BUFFER, inputfile))  //读命令
		{  			 
			bat_line++; 		
			done=Execute(buf); 
			if(done==1) // 执行quit命令 
			{  
				if(Outputs) 
					freopen("/dev/tty", "w", stdout);
				break;//exit(0);  
			}		                      
		}		 
	}  
	while (!feof(inputfile));  
	if(Outputs) 
		freopen("/dev/tty", "w", stdout);
	return 0; 
}


/*函数Execute:分析输入，调用my_spawn执行命令*/ 
int Execute(char *buffer)
{      
	pid_t pid ;
	char *args[MAX_ARGS];  /// 指向arg字符串指针
	int error ;
	int states[5]; // states[0]为back exec; states[1]表示输入文件数量，states[2]表示输出文件数量
	                          // states[3]不是argc的输入文件，states[4]代表argc 
	Redirect  Inputs [MAX_OPEN];  // 输入重定向(最多10个) 
	Redirect  Outputs[MAX_OPEN];  // 输出重定向(最多10)

	error=my_strtok(buffer,args,states,Inputs,Outputs);
	if(error||args[0]==NULL) 
		return -1; // 如果出现输入格式错误
	if ( !strcmp(args[0],"quit") || !strcmp(args[0],"exit"))   // quit命令
	{
		if(args[1])   // quit命令后有参数 
			Error(-2,args+1,NULL,NULL,args[0]); ///
		if(output_num>1)// 例如 e.g.  myshell a.bat >m.txt >n.txt 						 
		{         
			fprintf(stderr,"Exit!\n");
			return 1; 
		}
		if(isbat)
			fprintf(stderr,"Execution of batch file \"%s\" is finished!\n",batchfile);
		else fprintf(stderr,"Exit MyShell, Goodbye!\n\n"); 
			exit (0); 		               
	}
	else if(states[0])// states[0]==1 后台执行
	{                    
		switch (pid=fork( ))
		{
			case -1:   Error(-9,NULL,NULL,states,"fork");
			case  0:  // child   //sleep(1); 
				my_delay(12);                                                
				fprintf(stderr,"\n");                                                                       
				my_spawn(args,Inputs,Outputs,states);       
				exit(1);                   
			default:      
				if(isbat==0)
					fprintf(stderr,"pid=%d\n",pid);
		}	     
	}
	else // states[0]==0 前台执行  
		my_spawn(args,Inputs,Outputs,states);                          
	return 0;     
}


/*函数my_spawn：执行命令 */
int my_spawn(char **args, const Redirect *Inputs,const Redirect *Outputs,int *states)
{        
	char filepath[MAX_PATH] , parent[MAX_ARGS] ;   
	FILE * outputfile=NULL,* inputfile;    
	pid_t newpid;
	int flag ;    
	if (!strcmp(args[0],"myshell")||!strcmp(args[0],"shell"))   // myshell命令 
	{
		flag=0; 
		if(isbat)// 例如 在a.bat里执行myshell b.txt
		{			
			switch(newpid=fork( ))
			{  
				case -1:	
					Error(-9,NULL,NULL,states,"fork");
				case 0:
					if(states[0] &&(args[1]|| states[1]))
					{
						back_bat++;	
						flag=1;
					}                                                                  
					output_num=states[2];
					my_bat(args,Inputs,Outputs,states);    
					if(flag)   
						back_bat--;
					output_num=0;
					exit (0);
				default:
					waitpid(newpid, NULL, WUNTRACED); 
				}
			}
			else 
			{   
				if(states[0]&&(args[1]|| states[1]))
				{            
					back_bat++;
					flag=1;
				}
				output_num=states[2];
				my_bat(args,Inputs,Outputs,states); 
				if(flag)   
					back_bat--;
				output_num=0;
			}
			if(states[0])   
				exit(1);   
			else   
				return 0;  
		}
		if(states[2])// 输出重定向：使用freopen() 
		{
			get_fullpath(filepath, Outputs->filename);
			outputfile=freopen(filepath, Outputs->opentype,stdout);
			if(outputfile==NULL) 
			{       
				Error(-6,NULL,NULL,NULL,Outputs->filename);				 
				if(states[0]) exit(1);   
				else    
					return -4;  
			}		     		   
		}		 				 
       // 检查内部、外部命令 	
		if (!strcmp(args[0],"cd")) // cd命令				   
			my_cd(args,Inputs,states);	
		else if (!strcmp(args[0],"clr")||!strcmp(args[0],"clear")) // clear命令
		{       // system("clear");
			if(output_num==0 ) 
				my_clear( );  // my_clear()里，使用exec调用clear	                		      		
			if(args[1]|| states[1]||states[2])     //clear命令后有参数
				Error(4,NULL,NULL,NULL,args[0]); 	
		}
		else if (!strcmp(args[0],"dir")) // dir命令 
			my_dir(args,Inputs,states);
		else if (!strcmp(args[0],"echo"))  // echo命令
			my_echo(args,Inputs,states);
		else if (!strcmp(args[0],"environ"))  // environ命令
		{  
			list_environ( ); 
			if(states[1])// 无效输入重定向
				Error(-3,NULL,Inputs,states,"environ");
			if(args[1])    // pwd后有参数
				Error(-2,args+1,NULL,NULL,"environ"); 
		}
		else if (!strcmp(args[0],"help")||!strcmp(args[0],"?"))   // help命令
		{        
			my_help(args,Outputs,states); 
			if(states[1])    // 在pwd后有参数
				Error(-3,NULL,Inputs,states,args[0]); 
		} 
		else  if (!strcmp(args[0],"pwd"))   // pwd命令		
		{		
			show_pwd(  );
			if(states[1])// 无效输入重定向，例如<m.txt     
				Error(-3,NULL,Inputs,states,"pwd");
			if(args[1])    // pwd后面有参数
				Error(-2,args+1,NULL,NULL,"pwd");               
		}		 
         // 检查外部可执行文件，例如ls    
		else 
		{                            
			strcpy(parent,"parent=") ;        
			strcat(parent, getenv("shell"));                                              
			switch (newpid=fork( ))
			{
				case -1:   
					Error(-9,NULL,NULL,NULL,"fork");
				case 0:   // 在子进程中执行                                                                  
					if(states[1])// 输入重定向
					{	         
						get_fullpath(filepath,Inputs->filename);
						inputfile= freopen(filepath,"r",stdin); // 打开文件 
						if(inputfile==NULL) 
						{    
							Error(-6,NULL,NULL,NULL,Inputs->filename); 
							exit(1); 
						}
					}
					putenv( parent); //                                        
					execvp(args[0],args);                                                    
					Error(-1,args,NULL,NULL,NULL);// 
					if(inputfile)  fclose(inputfile);                               
						exit(0);
				default:
					if(states[0]==0 )
						waitpid(newpid, NULL, WUNTRACED);  
			}	
		}       
		if(outputfile)
		{    
			fclose(outputfile);
			freopen("/dev/tty","w",stdout);
		}         
		if(states[2]>1)// 多于一个输出文件 
		{       
			states[2]--;                       
			my_spawn (args, Inputs, Outputs+1,states) ;
		}
		if(states[0]) exit(0);   
		else 
			return 0; 
}//函数my_spawn( )结束


/*函数Error报告错误*/
int Error(int errortype,char **args,const Redirect *  IOputs,const int *states, char * msg)  
{   
	int i; 
	if(isbat)  // 从批处理文件读入
		fprintf(stderr,"***Line %d of inputfile \"%s\": ",bat_line,batchfile);
	switch(errortype)// 所有命令执行中的错误，分类显示错误类型
	{
		case  1:       
			fprintf(stderr,"Format Error: invlid argument '%s'(Letter %d), without openfile!\n",open,letter);// <  >>  > 
			fprintf(stderr,"Type 'help redirection'  to get help information abbout '<'  '>>' and '>'\n");  
			break;
		case  2:       
			fprintf(stderr,"Format Error(Letter %d): '%s' followed by invlid argument '%c'!\n", letter,open,*msg); // <  >> > 
			fprintf(stderr,"Type 'help redirection'  to get help information abbout '<'  '>>' and '>'\n");  
			break;
		case  0:      
			fprintf(stderr," %s\n",msg);  //
			break;
		case 3:        
			fprintf(stderr,"Sorry: no help information about \"%s\" found in the manual of myshell!\n",msg);
			fprintf(stderr,"Type \"man %s\" to search in the manual of system.\nNote: press the key <Q> to exit the search.\n",msg);
			break;
		case 4:        
			fprintf(stderr,"Note: no argument is needed after \"%s\" , except the background-execute flag '&' .",msg);   // 
			fprintf(stderr,"Type 'help %s'  to get usage abbout '%s' .\n",msg,msg);                                
			break; 
		case 5:       
			fprintf(stderr,"System Note: can not open more than %d files as %s !\n",MAX_OPEN,msg);
			break; 
		case  -1:     
			fprintf(stderr,"System Warning: \"");
			while (*args)
				fprintf(stderr,"%s ",*args++);
			fprintf(stderr,"\b\" is not internal command or executive file!\n"); 
			if(isbat==0&&output_num==0)//
				fprintf(stderr,"Type \"help command\" to see supported internal commands.\n");
			break; 
		case  -2:   
			fprintf(stderr,"Format Warning: invalid  arguments \"" ) ;
			while(*args)
				fprintf(stderr,"%s ",*args++);
			fprintf(stderr,"\b\"  after command \"%s\" ! \n",msg);
			break;
		case  -3:   
			fprintf(stderr,"Invalid  input redirection: ");
			for(i=0;i<states[1];i++)
				fprintf(stderr,"\"<%s\" ",IOputs[i].filename);
			fprintf(stderr,"after \"%s\" !\n",msg);
			break;
		case  -4:   
			fprintf(stderr,"Invalid  output redirection: ");
			for(i=0;i<states[1];i++)
				fprintf(stderr,"\"%s%s\" ",IOputs[i].opentype,IOputs[i].filename);
			fprintf(stderr,"after \"%s\" !\n",msg);
			break;
		case  -5:    
			fprintf(stderr,"Path Error : \"%s\":  not a directory or not exist!\n",msg); 
			break;
		case  -6:   
			fprintf(stderr,"File Error: can not open file \"%s\"  !\n",msg);
			break;
		case -7:   
			fprintf(stderr,"Overflow Error: the assigned dirpath is longer than permitted longth(%d)!\n",MAX_PATH);
			break;
		default:    
			fprintf(stderr,"%s: %s\n", strerror(errno), msg); 
			break;
			abort( ); 
	}     
	return 1; 
} 
    

/*my_delay：用于显示*/
void my_delay(int n)
{ 
	n=n*1000000;
	while(n--) ;
}


/*函数get_fullpath: 获得文件或目录的全路径*/
void get_fullpath(char *fullpath,const char *shortpath)
{       
	int i=0,j=0;   
	fullpath[0]=0; 
	char *old_dir, *current_dir; 
	if(shortpath[0]=='~')// 例如 ~/os
	{
		strcpy(fullpath, getenv("HOME"));
		j=strlen(fullpath);     
		i=1; 
	}	  
	else  if(shortpath[0]=='.'&&shortpath[1]=='.')// 例如 ../os
	{                  
		old_dir=getenv("PWD");
		chdir("..");                
		current_dir=(char *)malloc(MAX_BUFFER);// 分配内存空间
		if(!current_dir)        // 如果内存空间分配失败
			Error(-9,NULL,NULL,NULL,"malloc failed!");      
		getcwd(current_dir,MAX_BUFFER);  // 获取当前工作目录
		strcpy(fullpath, current_dir);
		j=strlen(fullpath);     
		i=2;   
		chdir(old_dir);
	}
	else   if(shortpath[0]=='.')// 例如 ./os 
	{    
		strcpy(fullpath, getenv("PWD"));
		j=strlen(fullpath);     
		i=1; 
	}
	else if(shortpath[0]!='/')//例如 os/project1 
	{       
		strcpy(fullpath, getenv("PWD"));
		strcat(fullpath, "/");
		j=strlen(fullpath);                    
		i=0; 
	}
	strcat(fullpath+j,shortpath+i); 
	return; 
}


/*命令函数：*/

/*函数my_cd：执行命令cd*/
int my_cd (char **args,const Redirect *Inputs,int *states)//
{   
	char dirpath[MAX_PATH], filepath[MAX_PATH], dirname[MAX_PATH];
	char  *current_dir; 
	int i,flag;
	FILE *inputfile;          
	if(states[4]) 
	{
		if(args[1])//无效参数                     
			Error(-2,args+1,NULL,NULL,"cd");                   
		if(--states[1])
			Error(-3,NULL,Inputs+1,states,"cd");
		get_fullpath(filepath,Inputs->filename);
		inputfile=fopen(filepath,"r");  // 打开文件 
		if(inputfile==NULL)// 无法打开  
		{    
			Error(-6,NULL,NULL,states,Inputs->filename);		    
			return -2;   // File Error 
		}    
		fgets(dirname,MAX_PATH,inputfile); 
		fclose(inputfile);         
		args[1] = strtok(dirname," \b\n\r");    
		i=2;  
		while (( args[i] = strtok(NULL," \b\n\r") ) )	i++;                                                        
	}
	else  if (states[1])   // 输入无效命令，例如 cd /home <a.txt或cd <a.txt <b.txt 
		Error(-3,NULL,Inputs,states,"cd");
	if(args[1])   //参数路径给出  
	{                     
	  if(args[2]) //多余一个参数                   
			Error(-2,args+2,NULL,NULL,"cd");         
		get_fullpath(dirpath,args[1]); 						                       
	}
	else 	
	{    
		fprintf(stdout,"%s\n",getenv("PWD"));
		return 0;
	}
	flag=chdir(dirpath); // 改变工作目录
	if(flag) // 如果改变失败					
	{      
		Error(-5,NULL,NULL,states, args[1]); 		
		return -2; 
	}
	 // 如果目录改变成功，修改PWD的值
	current_dir=(char *)malloc(MAX_BUFFER);// 分配内存空间
	if(!current_dir)// 如果内存空间分配失败
		Error(-9,NULL,NULL,states,"malloc failed!");            
	getcwd(current_dir,MAX_BUFFER);  // 获取当前工作目录
	setenv("PWD",current_dir,1);     // 修改PWD的值
	free(current_dir);
	return 0;
}  //函数my_cd结束



/*函数my_clear:执行命令clear */
void my_clear(void)    
 {      // system("clear");	
	pid_t newpid;
	switch (newpid=fork( ))
	{
  		case -1:
			Error(-9,NULL,NULL,NULL,"fork");
		case 0:   // 在子进程中执行
			execlp("clear",NULL,NULL); 
			Error(-9,NULL,NULL,NULL,"execlp"); // 若从exec返回，显示错误 
		default:      
			waitpid(newpid, NULL, WUNTRACED); 
			//fprintf(stderr,"Clear screen!\n"); 
	}
	return;
 } //函数my_clear结束


/*函数my_dir:执行命令dir */
int my_dir(char **args,const Redirect *Inputs, int *states)//
{            
	FILE  *inputfile ;
	pid_t newpid;
	DIR   *pdir;
	int   i; 
	char   filepath[MAX_PATH], dirpath[MAX_PATH], dirname[MAX_PATH];
         /* 处理路径 */      
	if(states[4]) 
	{
		if(args[1])// 无效参数                     
			Error(-2,args+1,NULL,NULL,"dir");                
		if(--states[1])
			Error(-3,NULL,Inputs,states,"dir");
		get_fullpath(filepath,Inputs->filename);
		inputfile=fopen(filepath,"r");  // 打开文件
		if(inputfile==NULL)// 无法打开 
		{    
			Error(-6,NULL,NULL,states,Inputs->filename);		    
			return -2;   // File Error 
		}         		  	                                   
		fgets(dirname,MAX_PATH,inputfile);   //
		fclose(inputfile);      
		args[1] = strtok(dirname," \b\n\r");//                              
		i=2;  
		while (( args[i] = strtok(NULL," \b\n\r") ) )	i++;                                                                     
	}
	else  if (states[1])   // 无效输入，例如cd /home <a.txt   
		Error(-3,NULL,Inputs,states,"dir");
	if(args[1])   //给出了参数路径
	{                     
		if(args[2])   // 大于一个参数                     
			Error(-2,args+2,NULL,NULL,"dir");         
		get_fullpath(dirpath,args[1]);				                       
	 } 
	else strcpy(dirpath, "."); // 仅输入"dir"无参数，代表"dir ."
       /* 以下开始检查路径是否存在 */
		pdir=opendir(dirpath);             
	if(pdir==NULL)	//如果不存在					
	{         
		Error(-5,NULL,NULL,states,args[1]);		
		return -2 ;
	}
        /* 以下开始执行dir指令 */
	switch (newpid=fork( ))
	{
		case -1:
			Error(-9,NULL,NULL,states,"fork");
		case 0:       // 在子进程中执行 				    
			execlp( "ls","ls" ,"-al", dirpath, NULL);     //  ls                                
			Error(-9,NULL,NULL,states,"execlp"); // 若从exec返回，显示错误 		
		default:   waitpid(newpid, NULL, WUNTRACED);
	} 	 
	return 0;
} //my_dir函数结束


/* 函数my_echo:执行echo命令*/
int my_echo (char **args,const Redirect *Inputs,int *states)
 {
	FILE * inputfile; 
	char filepath[MAX_PATH];
	char buf[MAX_BUFFER];
	int j,k;
	if(states[4])  // 输出重定向在args[1]之前,例如echo <a.txt hello   
	{
		if(args[1])    // args[1] is invalid    
			Error(-2,args+1,NULL,NULL,"echo");	                    
		for(j=0;j<states[1];j++) // 例如 echo <a.txt <b.txt 
		{   			   
			get_fullpath(filepath,Inputs[j].filename);
			inputfile=fopen(filepath,"r");
			if(inputfile==NULL) 
			{     
				Error(-6,NULL,NULL,NULL,Inputs[j].filename);				 
				return -2; 
			}
			if(states[2]==0&&output_num==0)// 无输出文件是打开的  
				fprintf(stderr,"The contents of file \"%s\" is as follows:\n",Inputs[j].filename);
			while (!feof(inputfile))    // 打印文件内容  
			{
				if(fgets(buf, MAX_BUFFER, inputfile))  
				fprintf(stdout,"%s",buf); 
			}					   		   
			fclose(inputfile);
			fprintf(stdout,"\n");	
		}  		   
	}
	else 
	{       
		if(states[1])// 无效输入，例如 echo hello <m.txt     
			Error(-3,NULL,Inputs,states,"echo");
		if(args[1])
		{    
			for(k=1;k<states[3]-1;k++)  
				fprintf(stdout,"%s ",args[k]);//fputs(args[k],outputfile);  
			fprintf(stdout,"%s",args[k]);  
		}  
		fprintf(stdout,"\n");           	  			 	                    		
	}    
	return 0;
 } //my_echo函数结束


/*函数list_environ":显示环境变量*/
int list_environ (void) 
{		 
	char ** env = environ;	            
	while(*env) fprintf(stdout,"%s\n",*env++); 	                               
	return 0;        
}        


/*函数show_pwd"：执行pwd命令 */
int show_pwd (void)//  
{	 		               
	fprintf(stdout,"PWD=%s\n",getenv("PWD")); 		                 
	return 0;
}


/*my_help函数:打印关键词；寻找关键词例如 <help  dir> ；输出文件readme直到 '#'符号*/
int my_help(char **args,const Redirect *Outputs,int *states)//
{        
	FILE *readme;
	char buffer[MAX_BUFFER];  
	char keywords [MAX_BUFFER]="<help ";   
	int i,len; 
	for(i=1;args[i];i++)
	{
		strcat(keywords,args[i]);
		strcat(keywords," ");
	}
	len=strlen(keywords);
	keywords[len-1]='>';
	keywords[len]='\0';        
	if(!strcmp(keywords,"<help more>")) // "help more "打印用户手册 
	{
		strcpy(buffer,"more ");
		strcat(buffer,getenv("readme_path"));
		for(i=0;i<states[2];i++)
		{  
			strcat(buffer,Outputs[i].open);
			strcat(buffer,Outputs[i].filename);
		}  
		Execute(buffer);                 
		return 0;  
	}
	readme=fopen(getenv("readme_path"),"r"); // 在运行myshell时，初始化阶段在环境变量里添加了readme_path项  
	while(!feof(readme)&&fgets(buffer,MAX_BUFFER,readme))  // 寻找关键词，如 <help dir>
	{      
		if(strstr(buffer,keywords)) 
		break;
	}   
	while(!feof(readme)&&fgets(buffer,MAX_BUFFER,readme))// 从当前位置打印至'#'
	{
		if(buffer[0]=='#')
			break;
		fputs(buffer,stdout);    // 打印帮助信息 
	}
	if(feof(readme))// 如果没有找到关键词
	{	  
		keywords[len-1]='\0';   //  关键词，例如 <help dir>
		Error(3,NULL,NULL,NULL,&keywords[6]);        
	}
	if(readme) fclose(readme);
	return 0;
}


/*函数my_bat:从批处理文件读取命令并执行*/
int my_bat(char **args,const Redirect *Inputs,const Redirect *Outputs, int *states)
{	  //   fprintf(stderr,"mybat");
	FILE *inputfile;  
	char filepath[MAX_PATH]; 
	int i=0;
	char fullpath_batchfile[MAX_PATH] ;  //  之前批处理文件的fullpath  e.g  执行  myshell a.bat, 在a.bat里有myshell b.bat 
                                                             //  fullpath_batchfile记录a.bat的路径，以防无限次循环   e.g  执行  myshell a.bat, 在a.bat里有myshell a.bat 
	pid_t  newpid; 
	if(isbat)  // if executes from batchfile
		fprintf(stderr,"***Line %d of inputfile \"%s\": ",bat_line,batchfile);//
	if(states[4])  // e.g.  myshell <a.bat      ==>     myshell a.bat	  
	{	  
		if(args[1])// e.g.   myshell <a.bat b.bat
			fprintf(stderr,"Note: can not open more than one inputfile after commnad '%s' .\n",args[0]); 
		args[1]=Inputs->filename;                                  
		--states[1];
		i=1;             
	}
	if(args[1])  // 例如 myshell a.bat    myshell <a.bxt
	{       
		if(states[1]>0)  // 例如 myshell a.txt <b.txt   
			Error(-3,NULL,Inputs+i,states,args[0]);  // 无效输入重定向   
		if(args[2])   // 多于两个输入 例如： myshell  a.txt  b.txt                       
			Error(-2,args+2,NULL,NULL,args[0]);                             
		get_fullpath(filepath,args[1]); 		
		get_fullpath( fullpath_batchfile,batchfile) ;  //  批处理文件的fullpath
		if(isbat && !strcmp(fullpath_batchfile,filepath))// 例如：myshell a.txt, 而a.txt 里有myshell a.txt 语句
		{
			fprintf(stderr,"Warning: commands not execute, it will cause infinite loop!!!\n");             
			return -5; 
		}
		inputfile=fopen(filepath,"r");    // 打开输入重定向文件
  		if(inputfile==NULL)// 无法打开文件
		{         
			Error(-6,NULL,NULL,NULL,args[1]);	
			return -2; 
		} 
		isbat=1; 	 
		strcpy(batchfile,args[1]);  
		fprintf(stderr,"Turn to execute the commands in batch file \"%s\" :\n",batchfile);       	
		if( ! output_num)//  
		my_shell(inputfile,NULL,states);  //my_shell( )函数里从inputfile读命令行，并解析和执行，结果输出到OutPuts
		else //多个输入重定向
			for(i=0;i<states[2];i++) 
			{                                                        
				my_shell(inputfile,Outputs+i,states);
				rewind(inputfile);
				output_num--;
			}
		fclose(inputfile);
		fprintf(stderr,"\nExecution of batch file \"%s\" is finished!\n",batchfile);
		bat_line=isbat=0; 	   
	}
	else   if( output_num) // 例如 myshell >b.txt >n.txt 
	{
		isbat=0;
		switch(newpid=fork( ))
		{   
			case -1:      
				Error(-9,NULL,NULL,states,"fork");
			case 0:      
				fprintf(stderr,"Please type commands(the results will be writen into \"%s\"):\n",Outputs->filename);
				my_shell(stdin,Outputs,states);                                                   
				exit(0);
			default:    waitpid(newpid, NULL, WUNTRACED);  
		}
		if( output_num>1)//多个输出重定向
		{      
			fprintf(stderr,"\n");		          
			output_num--;
			my_bat(args,Inputs,Outputs+1,states);
		}  		     
	}
	else// 仅输入"myshell" : 显示myshell版本 	 
		fprintf(stdout,"myshell    BY -- Xu Xiaoyao\n");		 
	return 0;
}


