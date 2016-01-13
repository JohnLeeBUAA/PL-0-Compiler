/*********************************************************
 *PL-0 Complier .h file
 *FileName: pl0.h
 *Author: John Lee
 *Date: 2013.12.09
 *Version: 2.0
 *Origin: Appendix A PL0 Compiler version 1.0 oct. 1989
 *********************************************************/

#include <stdio.h>

#define norw       16             ///保留字的个数
#define txmax      100            ///符号表长度
#define nmax       14             ///数字最大位数
#define al         10             ///标识符长度
#define amax       2047           ///最大数值
#define levmax     3              ///最大层次
#define cxmax      2000           ///指令队列长度

#define nul	       0x1
#define ident      0x2
#define number     0x4
#define plus       0x8
#define minus      0x10
#define times      0x20
#define slash      0x40
#define oddsym     0x80
#define eql        0x100
#define neq        0x200
#define lss        0x400
#define leq        0x800
#define gtr        0x1000
#define geq        0x2000
#define lparen     0x4000
#define rparen     0x8000
#define comma      0x10000
#define semicolon  0x20000
#define period     0x40000
#define becomes    0x80000
#define beginsym   0x100000
#define endsym     0x200000
#define ifsym      0x400000
#define thensym    0x800000
#define whilesym   0x1000000
#define dosym      0x2000000
#define callsym    0x4000000
#define constsym   0x8000000
#define varsym     0x10000000
#define procsym    0x20000000
#define elsesym    0x40000000
#define repeatsym  0x80000000
#define untilsym   0x100000000
#define readsym    0x200000000
#define writesym   0x400000000


///对象枚举定义
enum object
{
    constant, variable, proc
};


///操作码枚举定义
enum fct
{
    lit, opr, lod, sto, cal, Int, jmp, jpc, red, wrt
};


///指令结构体定义
typedef struct
{
    enum fct f;		///操作码
    long l; 		///层次
    long a; 		///相对地址
} instruction;

/******************************************************************
 *lit 0, a : 取常量a放到数据栈栈顶
 *opr 0, a : 执行运算，a表示执行何种运算
 *lod l, a : 取变量（相对地址为a，层次差为l）放到数据栈栈顶
 *sto l, a : 将数据栈栈顶内容存入变量（相对地址为a，层次差为l）
 *cal l, a : 调用过程（入口指令地址为a，层次差为l）
 *int 0, a : 数据栈栈顶指针增加a
 *jmp 0, a : 无条件转移到a
 *jpc 0, a : 条件转移到指令地址a
 *red 0, a : 读数据并存入变量（相对地址为a，层次差为l）
 *wrt 0, a : 将栈顶内容输出
 *red l, a : 读数据并存入变量（相对地址为a，层次差为l）
 *wrt 0, 0 : 将栈顶内容输出
 *******************************************************************/


char ch;                          ///当前读入的字符
unsigned long long sym;           ///当前读入的单词符号
char id[al+1];                    ///当前读入的标识符
long num;                         ///当前读入的数字
long cc;                          ///当前所取字符在该行的位置
long ll;                          ///当前源程序行的长度
long kk;                          ///当前标识符长度
long err;                         ///错误个数
long cx;                          ///指令索引指针

char line[81];                    ///源程序当前行
char a[al+1];                     ///当前读入的字符串
instruction code[cxmax+1];        ///指令队列
char word[norw][al+1];            ///保留字队列
unsigned long long wsym[norw];    ///保留字值队列
unsigned long ssym[256];          ///符号值队列（ASCII码为索引值）

char mnemonic[10][3+1];           ///储存操作码字符串

unsigned long long declbegsys;    ///声明部分合法头符号集
unsigned long long statbegsys;    ///语句部分合法头符号集
unsigned long long facbegsys;     ///因子部分合法头符号集


///符号表结构体定义
struct
{
    char name[al+1];       ///名字
    enum object kind;      ///种类（constant/variable/procedure）
    long val;              ///值（当种类为constant时）
    long level;            ///层次
    long addr;             ///地址（当种类为variable/procedure时）
}table[txmax+1];


///要读入的源程序文件名和文件指针
char infilename[80];
FILE* infile;


///分程序处理变量
long dx;		///数据分配索引
long lev;		///当前分程序层次
long tx;		///当前符号表索引


///解释执行程序用到的数据栈
#define stacksize 50000
long s[stacksize];
