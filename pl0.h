/*********************************************************
 *PL-0 Complier .h file
 *FileName: pl0.h
 *Author: John Lee
 *Date: 2013.12.09
 *Version: 2.0
 *Origin: Appendix A PL0 Compiler version 1.0 oct. 1989
 *********************************************************/

#include <stdio.h>

#define norw       16             ///�����ֵĸ���
#define txmax      100            ///���ű���
#define nmax       14             ///�������λ��
#define al         10             ///��ʶ������
#define amax       2047           ///�����ֵ
#define levmax     3              ///�����
#define cxmax      2000           ///ָ����г���

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


///����ö�ٶ���
enum object
{
    constant, variable, proc
};


///������ö�ٶ���
enum fct
{
    lit, opr, lod, sto, cal, Int, jmp, jpc, red, wrt
};


///ָ��ṹ�嶨��
typedef struct
{
    enum fct f;		///������
    long l; 		///���
    long a; 		///��Ե�ַ
} instruction;

/******************************************************************
 *lit 0, a : ȡ����a�ŵ�����ջջ��
 *opr 0, a : ִ�����㣬a��ʾִ�к�������
 *lod l, a : ȡ��������Ե�ַΪa����β�Ϊl���ŵ�����ջջ��
 *sto l, a : ������ջջ�����ݴ����������Ե�ַΪa����β�Ϊl��
 *cal l, a : ���ù��̣����ָ���ַΪa����β�Ϊl��
 *int 0, a : ����ջջ��ָ������a
 *jmp 0, a : ������ת�Ƶ�a
 *jpc 0, a : ����ת�Ƶ�ָ���ַa
 *red 0, a : �����ݲ������������Ե�ַΪa����β�Ϊl��
 *wrt 0, a : ��ջ���������
 *red l, a : �����ݲ������������Ե�ַΪa����β�Ϊl��
 *wrt 0, 0 : ��ջ���������
 *******************************************************************/


char ch;                          ///��ǰ������ַ�
unsigned long long sym;           ///��ǰ����ĵ��ʷ���
char id[al+1];                    ///��ǰ����ı�ʶ��
long num;                         ///��ǰ���������
long cc;                          ///��ǰ��ȡ�ַ��ڸ��е�λ��
long ll;                          ///��ǰԴ�����еĳ���
long kk;                          ///��ǰ��ʶ������
long err;                         ///�������
long cx;                          ///ָ������ָ��

char line[81];                    ///Դ����ǰ��
char a[al+1];                     ///��ǰ������ַ���
instruction code[cxmax+1];        ///ָ�����
char word[norw][al+1];            ///�����ֶ���
unsigned long long wsym[norw];    ///������ֵ����
unsigned long ssym[256];          ///����ֵ���У�ASCII��Ϊ����ֵ��

char mnemonic[10][3+1];           ///����������ַ���

unsigned long long declbegsys;    ///�������ֺϷ�ͷ���ż�
unsigned long long statbegsys;    ///��䲿�ֺϷ�ͷ���ż�
unsigned long long facbegsys;     ///���Ӳ��ֺϷ�ͷ���ż�


///���ű�ṹ�嶨��
struct
{
    char name[al+1];       ///����
    enum object kind;      ///���ࣨconstant/variable/procedure��
    long val;              ///ֵ��������Ϊconstantʱ��
    long level;            ///���
    long addr;             ///��ַ��������Ϊvariable/procedureʱ��
}table[txmax+1];


///Ҫ�����Դ�����ļ������ļ�ָ��
char infilename[80];
FILE* infile;


///�ֳ��������
long dx;		///���ݷ�������
long lev;		///��ǰ�ֳ�����
long tx;		///��ǰ���ű�����


///����ִ�г����õ�������ջ
#define stacksize 50000
long s[stacksize];
