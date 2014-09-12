// ֧���ⲿ����
#include "myshell.h"

int back_bat=0; // ��־���Ǻ�̨���У�����������ֵΪ1��Ч��
int output_num=0; // �����������ض������
char batchfile[MAX_PATH] ; // ��ǰ���������ļ�
int bat_line=0;// ���������������
int isbat=0;// �������־
int letter; // ��my_strtok( )   Error( )��ʹ��
char *open; // ��my_strtok( )   Error( )��ʹ��


/*��������*/ 
/*my_strtok��������1�������������У�������buf[]�У�����args[]*/
/*my_strtok��������2����states[]�м�¼execute,argc�ȵ�*/
/*my_strtok��������3����¼�ض�����������ļ�*/
int my_strtok(char *buf,char **args,int *states,Redirect *Inputs,Redirect *Outputs) // InPut��OutPuts��¼�ض���
{      
	int i,j,n,m, flag,argc,errortype; // flag�ǿհױ�ǣ�argc�ǲ�������(�������ض���ͺ�̨���У�
	char c ; 
	states[0]=states[1]=states[2]=states[3]=states[4]=0; // states[0]�Ǻ�̨���б�־��states[1]�������ض��������states[2]������ض��������
                                                         //states[3]�ǲ��������� states[4]��������ض����ڼ�������֮ǰ��
	errortype=letter=argc=0;
	args[0]=NULL;  
	open=NULL;// open�Ǵ򿪷�ʽ
	flag=1;
	i=m=n=-1;  
	while(buf[++i]&&buf[i]!='#') //ɨ��������,�����û��� '#'����ע��
	{             
		c=buf[i]; 
		switch(c) //  �ַ�����
		{                       
			case '<':  
				letter++;  
				if(flag==0)
				{   
					flag=1;	
					buf[i]='\0';	
				}
				open="<"; // "r" opentype  
				while(buf[++i]==' '|| buf[i]=='\t'); //������������Ŀո�  
				if(buf[i]<32|| buf[i]=='#' ) // <������Ч���������ļ�
				{  
					errortype=Error(1,NULL,NULL,NULL,"<");
					break; 
				}
				else if(buf[i]=='&'|| buf[i]=='<'|| buf[i]=='>'|| buf[i]=='|'|| buf[i]==';') // ������<& <; << <> <|    
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
					open=">>"; // "a"�Ĵ�����
				} 
				else 	
				{    						            
					open=">";  // "w"�Ĵ�����
				}
				while(buf[++i]==' '|| buf[i]=='\t'); // ������������Ŀո�      
				if(buf[i]<32|| buf[i]=='#' )
				{    						
					errortype=Error(1,NULL,NULL,NULL,NULL);
					break; 
				}
				else if(buf[i]=='&'|| buf[i]=='<'|| buf[i]=='>'|| buf[i]=='|' || buf[i]==';') // ������>&  <; >< <|    
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
				while(buf[++i]==' '|| buf[i]=='\t');  // ������������Ŀո�      
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
						if(m==MAX_OPEN)// ���������ض���
						errortype=Error(5,NULL,NULL,NULL,"input");  
						else  if(n==MAX_OPEN)// ��������ض���
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
				if(c=='\\'  &&buf[i+1]==' ')   //  ת���ַ�������"a\ b"���ļ���Ŀ¼�����д���"a b"  
				{     
					buf[i]=' ';
						if( ! isspace(buf[i+2]))
						{     
							j=i+1; 
							while(  buf[++j])  
								buf[j-1]=buf[j]; 
						}                                                                                                                      
				}  	
		} // switch����
	} // loopѭ������
	args[argc]=NULL; // args��NULL���� 
	states[1]=m+1;//   0,12,...,m
	states[2]=n+1;//  0,1,2,...,n
	states[3]=argc;
	if(errortype||(argc==0&&letter))   
 		Error(0,NULL,NULL,NULL,"Warning: nothing will be executed!");     
	return errortype;					
}



/*����my_shell���ӱ�׼����������ļ��ж�ȡ��������ú���Execute(��ִ������*/ 
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
	do    // /* ���ֶ�״ֱ̬��quit�����eof*/
	  {   //�������ļ��ж�ȡ����
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
	    if (fgets(buf, MAX_BUFFER, inputfile))  //������
		{  			 
			bat_line++; 		
			done=Execute(buf); 
			if(done==1) // ִ��quit���� 
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


/*����Execute:�������룬����my_spawnִ������*/ 
int Execute(char *buffer)
{      
	pid_t pid ;
	char *args[MAX_ARGS];  /// ָ��arg�ַ���ָ��
	int error ;
	int states[5]; // states[0]Ϊback exec; states[1]��ʾ�����ļ�������states[2]��ʾ����ļ�����
	                          // states[3]����argc�������ļ���states[4]����argc 
	Redirect  Inputs [MAX_OPEN];  // �����ض���(���10��) 
	Redirect  Outputs[MAX_OPEN];  // ����ض���(���10)

	error=my_strtok(buffer,args,states,Inputs,Outputs);
	if(error||args[0]==NULL) 
		return -1; // ������������ʽ����
	if ( !strcmp(args[0],"quit") || !strcmp(args[0],"exit"))   // quit����
	{
		if(args[1])   // quit������в��� 
			Error(-2,args+1,NULL,NULL,args[0]); ///
		if(output_num>1)// ���� e.g.  myshell a.bat >m.txt >n.txt 						 
		{         
			fprintf(stderr,"Exit!\n");
			return 1; 
		}
		if(isbat)
			fprintf(stderr,"Execution of batch file \"%s\" is finished!\n",batchfile);
		else fprintf(stderr,"Exit MyShell, Goodbye!\n\n"); 
			exit (0); 		               
	}
	else if(states[0])// states[0]==1 ��ִ̨��
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
	else // states[0]==0 ǰִ̨��  
		my_spawn(args,Inputs,Outputs,states);                          
	return 0;     
}


/*����my_spawn��ִ������ */
int my_spawn(char **args, const Redirect *Inputs,const Redirect *Outputs,int *states)
{        
	char filepath[MAX_PATH] , parent[MAX_ARGS] ;   
	FILE * outputfile=NULL,* inputfile;    
	pid_t newpid;
	int flag ;    
	if (!strcmp(args[0],"myshell")||!strcmp(args[0],"shell"))   // myshell���� 
	{
		flag=0; 
		if(isbat)// ���� ��a.bat��ִ��myshell b.txt
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
		if(states[2])// ����ض���ʹ��freopen() 
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
       // ����ڲ����ⲿ���� 	
		if (!strcmp(args[0],"cd")) // cd����				   
			my_cd(args,Inputs,states);	
		else if (!strcmp(args[0],"clr")||!strcmp(args[0],"clear")) // clear����
		{       // system("clear");
			if(output_num==0 ) 
				my_clear( );  // my_clear()�ʹ��exec����clear	                		      		
			if(args[1]|| states[1]||states[2])     //clear������в���
				Error(4,NULL,NULL,NULL,args[0]); 	
		}
		else if (!strcmp(args[0],"dir")) // dir���� 
			my_dir(args,Inputs,states);
		else if (!strcmp(args[0],"echo"))  // echo����
			my_echo(args,Inputs,states);
		else if (!strcmp(args[0],"environ"))  // environ����
		{  
			list_environ( ); 
			if(states[1])// ��Ч�����ض���
				Error(-3,NULL,Inputs,states,"environ");
			if(args[1])    // pwd���в���
				Error(-2,args+1,NULL,NULL,"environ"); 
		}
		else if (!strcmp(args[0],"help")||!strcmp(args[0],"?"))   // help����
		{        
			my_help(args,Outputs,states); 
			if(states[1])    // ��pwd���в���
				Error(-3,NULL,Inputs,states,args[0]); 
		} 
		else  if (!strcmp(args[0],"pwd"))   // pwd����		
		{		
			show_pwd(  );
			if(states[1])// ��Ч�����ض�������<m.txt     
				Error(-3,NULL,Inputs,states,"pwd");
			if(args[1])    // pwd�����в���
				Error(-2,args+1,NULL,NULL,"pwd");               
		}		 
         // ����ⲿ��ִ���ļ�������ls    
		else 
		{                            
			strcpy(parent,"parent=") ;        
			strcat(parent, getenv("shell"));                                              
			switch (newpid=fork( ))
			{
				case -1:   
					Error(-9,NULL,NULL,NULL,"fork");
				case 0:   // ���ӽ�����ִ��                                                                  
					if(states[1])// �����ض���
					{	         
						get_fullpath(filepath,Inputs->filename);
						inputfile= freopen(filepath,"r",stdin); // ���ļ� 
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
		if(states[2]>1)// ����һ������ļ� 
		{       
			states[2]--;                       
			my_spawn (args, Inputs, Outputs+1,states) ;
		}
		if(states[0]) exit(0);   
		else 
			return 0; 
}//����my_spawn( )����


/*����Error�������*/
int Error(int errortype,char **args,const Redirect *  IOputs,const int *states, char * msg)  
{   
	int i; 
	if(isbat)  // ���������ļ�����
		fprintf(stderr,"***Line %d of inputfile \"%s\": ",bat_line,batchfile);
	switch(errortype)// ��������ִ���еĴ��󣬷�����ʾ��������
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
    

/*my_delay��������ʾ*/
void my_delay(int n)
{ 
	n=n*1000000;
	while(n--) ;
}


/*����get_fullpath: ����ļ���Ŀ¼��ȫ·��*/
void get_fullpath(char *fullpath,const char *shortpath)
{       
	int i=0,j=0;   
	fullpath[0]=0; 
	char *old_dir, *current_dir; 
	if(shortpath[0]=='~')// ���� ~/os
	{
		strcpy(fullpath, getenv("HOME"));
		j=strlen(fullpath);     
		i=1; 
	}	  
	else  if(shortpath[0]=='.'&&shortpath[1]=='.')// ���� ../os
	{                  
		old_dir=getenv("PWD");
		chdir("..");                
		current_dir=(char *)malloc(MAX_BUFFER);// �����ڴ�ռ�
		if(!current_dir)        // ����ڴ�ռ����ʧ��
			Error(-9,NULL,NULL,NULL,"malloc failed!");      
		getcwd(current_dir,MAX_BUFFER);  // ��ȡ��ǰ����Ŀ¼
		strcpy(fullpath, current_dir);
		j=strlen(fullpath);     
		i=2;   
		chdir(old_dir);
	}
	else   if(shortpath[0]=='.')// ���� ./os 
	{    
		strcpy(fullpath, getenv("PWD"));
		j=strlen(fullpath);     
		i=1; 
	}
	else if(shortpath[0]!='/')//���� os/project1 
	{       
		strcpy(fullpath, getenv("PWD"));
		strcat(fullpath, "/");
		j=strlen(fullpath);                    
		i=0; 
	}
	strcat(fullpath+j,shortpath+i); 
	return; 
}


/*�������*/

/*����my_cd��ִ������cd*/
int my_cd (char **args,const Redirect *Inputs,int *states)//
{   
	char dirpath[MAX_PATH], filepath[MAX_PATH], dirname[MAX_PATH];
	char  *current_dir; 
	int i,flag;
	FILE *inputfile;          
	if(states[4]) 
	{
		if(args[1])//��Ч����                     
			Error(-2,args+1,NULL,NULL,"cd");                   
		if(--states[1])
			Error(-3,NULL,Inputs+1,states,"cd");
		get_fullpath(filepath,Inputs->filename);
		inputfile=fopen(filepath,"r");  // ���ļ� 
		if(inputfile==NULL)// �޷���  
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
	else  if (states[1])   // ������Ч������� cd /home <a.txt��cd <a.txt <b.txt 
		Error(-3,NULL,Inputs,states,"cd");
	if(args[1])   //����·������  
	{                     
	  if(args[2]) //����һ������                   
			Error(-2,args+2,NULL,NULL,"cd");         
		get_fullpath(dirpath,args[1]); 						                       
	}
	else 	
	{    
		fprintf(stdout,"%s\n",getenv("PWD"));
		return 0;
	}
	flag=chdir(dirpath); // �ı乤��Ŀ¼
	if(flag) // ����ı�ʧ��					
	{      
		Error(-5,NULL,NULL,states, args[1]); 		
		return -2; 
	}
	 // ���Ŀ¼�ı�ɹ����޸�PWD��ֵ
	current_dir=(char *)malloc(MAX_BUFFER);// �����ڴ�ռ�
	if(!current_dir)// ����ڴ�ռ����ʧ��
		Error(-9,NULL,NULL,states,"malloc failed!");            
	getcwd(current_dir,MAX_BUFFER);  // ��ȡ��ǰ����Ŀ¼
	setenv("PWD",current_dir,1);     // �޸�PWD��ֵ
	free(current_dir);
	return 0;
}  //����my_cd����



/*����my_clear:ִ������clear */
void my_clear(void)    
 {      // system("clear");	
	pid_t newpid;
	switch (newpid=fork( ))
	{
  		case -1:
			Error(-9,NULL,NULL,NULL,"fork");
		case 0:   // ���ӽ�����ִ��
			execlp("clear",NULL,NULL); 
			Error(-9,NULL,NULL,NULL,"execlp"); // ����exec���أ���ʾ���� 
		default:      
			waitpid(newpid, NULL, WUNTRACED); 
			//fprintf(stderr,"Clear screen!\n"); 
	}
	return;
 } //����my_clear����


/*����my_dir:ִ������dir */
int my_dir(char **args,const Redirect *Inputs, int *states)//
{            
	FILE  *inputfile ;
	pid_t newpid;
	DIR   *pdir;
	int   i; 
	char   filepath[MAX_PATH], dirpath[MAX_PATH], dirname[MAX_PATH];
         /* ����·�� */      
	if(states[4]) 
	{
		if(args[1])// ��Ч����                     
			Error(-2,args+1,NULL,NULL,"dir");                
		if(--states[1])
			Error(-3,NULL,Inputs,states,"dir");
		get_fullpath(filepath,Inputs->filename);
		inputfile=fopen(filepath,"r");  // ���ļ�
		if(inputfile==NULL)// �޷��� 
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
	else  if (states[1])   // ��Ч���룬����cd /home <a.txt   
		Error(-3,NULL,Inputs,states,"dir");
	if(args[1])   //�����˲���·��
	{                     
		if(args[2])   // ����һ������                     
			Error(-2,args+2,NULL,NULL,"dir");         
		get_fullpath(dirpath,args[1]);				                       
	 } 
	else strcpy(dirpath, "."); // ������"dir"�޲���������"dir ."
       /* ���¿�ʼ���·���Ƿ���� */
		pdir=opendir(dirpath);             
	if(pdir==NULL)	//���������					
	{         
		Error(-5,NULL,NULL,states,args[1]);		
		return -2 ;
	}
        /* ���¿�ʼִ��dirָ�� */
	switch (newpid=fork( ))
	{
		case -1:
			Error(-9,NULL,NULL,states,"fork");
		case 0:       // ���ӽ�����ִ�� 				    
			execlp( "ls","ls" ,"-al", dirpath, NULL);     //  ls                                
			Error(-9,NULL,NULL,states,"execlp"); // ����exec���أ���ʾ���� 		
		default:   waitpid(newpid, NULL, WUNTRACED);
	} 	 
	return 0;
} //my_dir��������


/* ����my_echo:ִ��echo����*/
int my_echo (char **args,const Redirect *Inputs,int *states)
 {
	FILE * inputfile; 
	char filepath[MAX_PATH];
	char buf[MAX_BUFFER];
	int j,k;
	if(states[4])  // ����ض�����args[1]֮ǰ,����echo <a.txt hello   
	{
		if(args[1])    // args[1] is invalid    
			Error(-2,args+1,NULL,NULL,"echo");	                    
		for(j=0;j<states[1];j++) // ���� echo <a.txt <b.txt 
		{   			   
			get_fullpath(filepath,Inputs[j].filename);
			inputfile=fopen(filepath,"r");
			if(inputfile==NULL) 
			{     
				Error(-6,NULL,NULL,NULL,Inputs[j].filename);				 
				return -2; 
			}
			if(states[2]==0&&output_num==0)// ������ļ��Ǵ򿪵�  
				fprintf(stderr,"The contents of file \"%s\" is as follows:\n",Inputs[j].filename);
			while (!feof(inputfile))    // ��ӡ�ļ�����  
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
		if(states[1])// ��Ч���룬���� echo hello <m.txt     
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
 } //my_echo��������


/*����list_environ":��ʾ��������*/
int list_environ (void) 
{		 
	char ** env = environ;	            
	while(*env) fprintf(stdout,"%s\n",*env++); 	                               
	return 0;        
}        


/*����show_pwd"��ִ��pwd���� */
int show_pwd (void)//  
{	 		               
	fprintf(stdout,"PWD=%s\n",getenv("PWD")); 		                 
	return 0;
}


/*my_help����:��ӡ�ؼ��ʣ�Ѱ�ҹؼ������� <help  dir> ������ļ�readmeֱ�� '#'����*/
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
	if(!strcmp(keywords,"<help more>")) // "help more "��ӡ�û��ֲ� 
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
	readme=fopen(getenv("readme_path"),"r"); // ������myshellʱ����ʼ���׶��ڻ��������������readme_path��  
	while(!feof(readme)&&fgets(buffer,MAX_BUFFER,readme))  // Ѱ�ҹؼ��ʣ��� <help dir>
	{      
		if(strstr(buffer,keywords)) 
		break;
	}   
	while(!feof(readme)&&fgets(buffer,MAX_BUFFER,readme))// �ӵ�ǰλ�ô�ӡ��'#'
	{
		if(buffer[0]=='#')
			break;
		fputs(buffer,stdout);    // ��ӡ������Ϣ 
	}
	if(feof(readme))// ���û���ҵ��ؼ���
	{	  
		keywords[len-1]='\0';   //  �ؼ��ʣ����� <help dir>
		Error(3,NULL,NULL,NULL,&keywords[6]);        
	}
	if(readme) fclose(readme);
	return 0;
}


/*����my_bat:���������ļ���ȡ���ִ��*/
int my_bat(char **args,const Redirect *Inputs,const Redirect *Outputs, int *states)
{	  //   fprintf(stderr,"mybat");
	FILE *inputfile;  
	char filepath[MAX_PATH]; 
	int i=0;
	char fullpath_batchfile[MAX_PATH] ;  //  ֮ǰ�������ļ���fullpath  e.g  ִ��  myshell a.bat, ��a.bat����myshell b.bat 
                                                             //  fullpath_batchfile��¼a.bat��·�����Է����޴�ѭ��   e.g  ִ��  myshell a.bat, ��a.bat����myshell a.bat 
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
	if(args[1])  // ���� myshell a.bat    myshell <a.bxt
	{       
		if(states[1]>0)  // ���� myshell a.txt <b.txt   
			Error(-3,NULL,Inputs+i,states,args[0]);  // ��Ч�����ض���   
		if(args[2])   // ������������ ���磺 myshell  a.txt  b.txt                       
			Error(-2,args+2,NULL,NULL,args[0]);                             
		get_fullpath(filepath,args[1]); 		
		get_fullpath( fullpath_batchfile,batchfile) ;  //  �������ļ���fullpath
		if(isbat && !strcmp(fullpath_batchfile,filepath))// ���磺myshell a.txt, ��a.txt ����myshell a.txt ���
		{
			fprintf(stderr,"Warning: commands not execute, it will cause infinite loop!!!\n");             
			return -5; 
		}
		inputfile=fopen(filepath,"r");    // �������ض����ļ�
  		if(inputfile==NULL)// �޷����ļ�
		{         
			Error(-6,NULL,NULL,NULL,args[1]);	
			return -2; 
		} 
		isbat=1; 	 
		strcpy(batchfile,args[1]);  
		fprintf(stderr,"Turn to execute the commands in batch file \"%s\" :\n",batchfile);       	
		if( ! output_num)//  
		my_shell(inputfile,NULL,states);  //my_shell( )�������inputfile�������У���������ִ�У���������OutPuts
		else //��������ض���
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
	else   if( output_num) // ���� myshell >b.txt >n.txt 
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
		if( output_num>1)//�������ض���
		{      
			fprintf(stderr,"\n");		          
			output_num--;
			my_bat(args,Inputs,Outputs+1,states);
		}  		     
	}
	else// ������"myshell" : ��ʾmyshell�汾 	 
		fprintf(stdout,"myshell    BY -- Xu Xiaoyao\n");		 
	return 0;
}


