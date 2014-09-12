#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <dirent.h> 
#include <ctype.h>

#define MAX_BUFFER 1024  // ��ȡһ������ʱ���1024���ַ�
#define MAX_ARGS 64  // һ�����������64��������������I/O�ض���ͺ�̨���У�
#define NUM 10        // myshell��֧�ֵ��������
#define MAX_OPEN 10  //  ����10��dtdin�ض����ļ���10��stdout�ض����ļ���myshell֧�ֶ��I/O�ض���
#define MAX_PATH 100 //  �ļ�����Ŀ¼������󳤶�

typedef struct//�Զ����ض������ݽṹ
{ 
	char *filename;   // �ض����ļ���
	char opentype[3]; // �ض����ļ��򿪷�ʽ   "a"   "r"   "w" 
        char open[3];  // �ض����ļ��򿪷�ʽ   ">>"   "<"   ">" 
} Redirect; 

extern int errno;        // ϵͳ�������
extern char **environ;   // ������������

/*��������*/
int my_strtok(char *buf,char **args,int *states,Redirect *InPuts,Redirect *OutPuts);//�����У��ַ���������
int Execute(char *buffer);//����ִ��
int my_shell(FILE *inputfile,const Redirect *Outputs,const int *states);//�������
int my_spawn(char **args, const Redirect *Inputs,const Redirect *Outputs,int *states);
void my_delay(int n);           //��ʱ����֤���ӽ��̵��Ⱥ�˳��
void get_fullpath(char *fullpath,const char *shortpath);  //��ȡ�ļ���Ŀ¼������·��
int Error(int errortype,char **args,const Redirect *  IOputs,const int *states, char * msg) ;//������


/*�����*/
int my_cd(char **args,const Redirect *Inputs, int *states);// ִ��cd����
void my_clear(void);// ִ��clear����
int my_dir(char **args,const Redirect *Inputs, int *states);// ִ��dir����
int my_echo(char **args,const Redirect *Inputs,int *states);// ִ��echo����
int list_environ(void);// ִ��environ���� 
int show_pwd(void); // ִ��pwd���� 
int my_help(char **args,const Redirect *Outputs,int *states);// ִ��help���� 
int my_bat(char **args,const Redirect *Inputs,const Redirect *Outputs,int *states); // ִ��myshell����
