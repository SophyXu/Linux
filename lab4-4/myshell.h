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

#define MAX_BUFFER 1024  // 读取一行命令时最多1024个字符
#define MAX_ARGS 64  // 一行命令中最多64个参数（不包括I/O重定向和后台运行）
#define NUM 10        // myshell所支持的命令个数
#define MAX_OPEN 10  //  最多打开10个dtdin重定向文件和10个stdout重定向文件（myshell支持多个I/O重定向）
#define MAX_PATH 100 //  文件名或目录名的最大长度

typedef struct//自定义重定向数据结构
{ 
	char *filename;   // 重定向文件名
	char opentype[3]; // 重定向文件打开方式   "a"   "r"   "w" 
        char open[3];  // 重定向文件打开方式   ">>"   "<"   ">" 
} Redirect; 

extern int errno;        // 系统错如变量
extern char **environ;   // 环境变量数组

/*辅助函数*/
int my_strtok(char *buf,char **args,int *states,Redirect *InPuts,Redirect *OutPuts);//命令行（字符串）解析
int Execute(char *buffer);//命令执行
int my_shell(FILE *inputfile,const Redirect *Outputs,const int *states);//命令入口
int my_spawn(char **args, const Redirect *Inputs,const Redirect *Outputs,int *states);
void my_delay(int n);           //延时，保证父子进程的先后顺序
void get_fullpath(char *fullpath,const char *shortpath);  //获取文件或目录的完整路径
int Error(int errortype,char **args,const Redirect *  IOputs,const int *states, char * msg) ;//错误处理


/*命令函数*/
int my_cd(char **args,const Redirect *Inputs, int *states);// 执行cd命令
void my_clear(void);// 执行clear命令
int my_dir(char **args,const Redirect *Inputs, int *states);// 执行dir命令
int my_echo(char **args,const Redirect *Inputs,int *states);// 执行echo命令
int list_environ(void);// 执行environ命令 
int show_pwd(void); // 执行pwd命令 
int my_help(char **args,const Redirect *Outputs,int *states);// 执行help命令 
int my_bat(char **args,const Redirect *Inputs,const Redirect *Outputs,int *states); // 执行myshell命令
