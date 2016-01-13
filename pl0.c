/*********************************************************
 *PL-0 Complier .c file
 *FileName: pl0.c
 *Author: John Lee
 *Date: 2013.12.09
 *Version: 2.0
 *Origin: Appendix A PL0 Compiler version 1.0 oct. 1989
 *********************************************************/


#include <stdlib.h>
#include <string.h>
#include "pl0.h"


/***************************************
 *Function: error
 *Description: 出错处理，打印出错代码
 *Input: n为错误编码
 ***************************************/
void error(long n)
{
    long i;
    printf(" ****");            ///打印错误标识
    for (i=1; i<=cc-1; i++)     ///将光标移到出错的字符处
    {
        printf(" ");
    }
    printf("^%2d\n",n);         ///打印错误编码
    err++;                      ///错误数加1
}


/*************************
 *Function: getch
 *Description: 读字符
 *************************/
void getch()
{
    if(cc==ll)                    ///若果当前字符在一行的结尾
    {
        if(feof(infile))          ///如果文件结束，给出程序不完整提示，程序终止
        {
            printf("************************************\n");
            printf("      program incomplete\n");
            printf("************************************\n");
            exit(1);
        }
        ll=0;                     ///置当前行长度为0
        cc=0;                     ///置当前字符位置为0
        printf("%5d ", cx);       ///打印指令索引号

                                  ///将下一行打印并读入line[]中
        while((!feof(infile))&&((ch=getc(infile))!='\n'))
        {
            printf("%c",ch);
            ll=ll+1;
            line[ll]=ch;
        }
        printf("\n");
        ll=ll+1;
        line[ll]=' ';
    }
    cc=cc+1;                      ///取当前位置的下一个字符到ch中
    ch=line[cc];
}


/***************************************
 *Function: getsym
 *Description: 词法分析，读取一个单词
 ***************************************/
void getsym()
{
    long i,j,k;
    while(ch==' '||ch=='\t')                    ///跳过空格和制表符
    {
        getch();
    }
    if(isalpha(ch))                             ///如果当前字符是字母，既sym为标识符或保留字
    {
        k=0;
        do
        {
            if(k<al)                            ///如果单词长度大于最长单词限制自动截断
            {
                a[k]=ch;
                k=k+1;
            }
            getch();
        }while(isalpha(ch)||isdigit(ch));
        if(k>=kk)                               ///k为当前单词长度，kk为上一个单词的长度
        {
            kk=k;
        }
        else                                    ///如果当前单词长度小于上一个单词长度，将a队列短的部分位置字符置空
        {
            do
            {
                kk=kk-1;
                a[kk]=' ';
            }while(k<kk);
        }
        strcpy(id,a);                           ///将当前读入的字符串a赋给当前标识符id

        i=0;                                    ///用二分法查找保留字表
        j=norw-1;
        do
        {
            k=(i+j)/2;
            if(strcmp(id,word[k])<=0)
            {
                j=k-1;
            }
            if(strcmp(id,word[k])>=0)
            {
                i=k+1;
            }
        }while(i<=j);
        if(i-1>j)                               ///如果找到说明为保留字，将值赋给sym
        {
            sym=wsym[k];
        }
        else                                    ///如果没找到，说明sym为标识符
        {
            sym=ident;
        }
    }
    else if(isdigit(ch))                        ///如果当前字符是数字
    {
        k=0;
        num=0;
        sym=number;                             ///将数字类型值赋给sym
        do                                      ///将数字字符串转换成数字赋给num
        {
            num=num*10+(ch-'0');
            k=k+1;
            getch();
        }while(isdigit(ch));
        if(k>nmax)                              ///如果数字位数超过最大位数限制，报错
        {
            error(30);
        }
    }
    else if(ch==':')                            ///判断是 nul 或是 :=
    {
        getch();
        if(ch=='=')
        {
            sym=becomes;
            getch();
        }
        else
        {
            sym=nul;
        }
    }
    else if(ch=='<')                            ///判断是 < 或是 <= 或是 <>
    {
        getch();
        if(ch=='=')
        {
            sym=leq;
            getch();
        }
        else if(ch=='>')
        {
            sym=neq;
            getch();
        }
        else
        {
            sym=lss;
        }
    }
    else if(ch=='>')                            ///判断是 > 或是 >=
    {
        getch();
        if(ch=='=')
        {
            sym=geq;
            getch();
        }
        else
        {
            sym=gtr;
        }
    }
    else                                        ///其它符号
    {
        sym=ssym[(unsigned char)ch];
        getch();
    }
}


/***********************************************
 *Function: gen
 *Description: 生成P-code指令，送入目标程序区
 *Input: x为指令操作码，y为层次差，z为相对地址
 ***********************************************/
void gen(enum fct x, long y, long z)
{
    if(cx>cxmax)                         ///指令索引指针超过最大指令索引指针
    {
        printf("program too long\n");    ///输出错误信息
        exit(1);                         ///程序结束
    }
    code[cx].f=x;                        ///否则将当前指令的操作码、
    code[cx].l=y;                        ///层次差、
    code[cx].a=z;                        ///相对地址赋给code指令队列中cx指针的相应变量
    cx=cx+1;                             ///将指令索引指针加1
}


/*****************************************************************
 *Function: test
 *Description: 测试当前单词符号是否合法
 *Input: fsys1为合法符号集合，fsys2为停止符号集合，n为错误编码
 *****************************************************************/
void test(unsigned long long fsys1, unsigned long long fsys2, long n)
{
    if (!(sym & fsys1))          ///若下一个取来的符号不是该语法合法的后继符号
    {
        error(n);             ///报出错误号为n的错误
        fsys1=fsys1|fsys2;
        while(!(sym & fsys1))    ///程序跳读直到取来的符号属于合法后继符号集合或停止符号集合
        {
            getsym();
        }
    }
}


/******************************************************************
 *Function: enter
 *Description: 登录符号表
 *Input: k为要写入符号表的对象枚举类型（constant/variable/proc）
 ******************************************************************/
void enter(enum object k)
{
    tx=tx+1;                            ///将符号表索引加1
    strcpy(table[tx].name,id);          ///将当前标识符名字和类型赋给符号表中索引位置相应元素
    table[tx].kind=k;
    switch(k)
    {
        case constant:                  ///如果是常量
            if(num>amax)                ///如果数值超限，报错，num置为0
            {
                error(30);
                num = 0;
            }
            table[tx].val=num;          ///数值合法将其赋给val
            break;
        case variable:                  ///如果是变量，将其层次、数据分配索引赋给符号表中的层次、地址
            table[tx].level=lev;
            table[tx].addr=dx;
            dx=dx+1;
            break;
        case proc:                      ///如果是procedure只赋层次值，地址应填入生成的P-code指令序列的入口地址，还不能确定
            table[tx].level=lev;
            break;
    }
}


/************************************************************************************
 *Function: positon
 *Description: 查找标识在符号表中的位置
 *Input: id为要查找的标识符号串
 *Output: 返回该标识符在符号表中的位置，若符号表中无此名字，则返回值0，表示出错
 ************************************************************************************/
long position(char* id)
{
    long i;
    strcpy(table[0].name,id);              ///将要查找的标识符赋给符号表第零元素的名字，这样若符号表中无该标识符能返回0
    i=tx;                                  ///将符号表的最下索引赋给i
    while(strcmp(table[i].name,id)!=0)     ///自下而上比较符号表每一条目的名字与要查找的标识符
    {
        i=i-1;
    }
    return i;                              ///若找到则i为标识符位置，若符号表中无该标识符，则在第零元素找到，返回i值为0
}


/****************************
 *Function: constdeclaration
 *Description: 常量定义处理
 ****************************/
void constdeclaration()
{
    if(sym==ident)                        ///如果当单词是标识符
    {
        getsym();                         ///读一个单词
        if(sym==eql||sym==becomes)        ///如果是 = 或 :=
        {
            if(sym==becomes)              ///如果把 = 错写成 := ，报错
            {
                error(1);
            }
            getsym();                     ///读一个单词，如果是数字将其写入符号表
            if(sym==number)
            {
                enter(constant);
                getsym();                 ///预读一个单词
            }
            else                          ///否则报错
            {
                error(2);
            }
        }
        else                              ///标识符后不是 = ，报错
        {
            error(3);
        }
    }
    else                                  ///如果不是标识符，报错
    {
        error(4);
    }
}


/****************************
 *Function: vardeclaration
 *Description: 变量说明处理
 ****************************/
void vardeclaration()
{
    if(sym==ident)            ///如果当前单词是标识符
    {
        enter(variable);      ///写入符号表
        getsym();             ///预读一个单词
    }
    else
    {
        error(4);             ///否则报错
    }
}


/*********************************************
 *Function: listcode
 *Description: 列出P-code指令清单
 *Input: cx0为要打印的指令段的起始指令索引
 *********************************************/
void listcode(cx0)
{
    long i;
    for(i=cx0; i<=cx-1; i++)         ///循环打印索引号（行号）、操作码、层次、地址
    {
        printf("%10d%5s%3d%5d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
    }
}


/*****************************************************************
 *Function: expression
 *Description: 表达式分析处理函数声明
 *因为factor()、term()、expression()涉及到交叉调用所以要先声明
 *****************************************************************/
void expression(unsigned long long fsys);


/*****************************
 *Function: factor
 *Description: 因子分析处理
 *Input: fsys为停止符号集合
 *****************************/
void factor(unsigned long long fsys)
{
    long i;
    test(facbegsys,fsys,24);                          ///调用test函数判断当前单词是否在factor的合法头符号集中，若不在报错并跳读
    while(sym & facbegsys)                            ///循环读因子
    {
        if(sym==ident)                                ///如果当前单词是标识符
        {
            i=position(id);                           ///调用position函数读取它在符号表中的位置
            if(i==0)                                  ///如果为没声明的标识符，报错
            {
                error(11);
            }
            else                                      ///如果是合法的标识符，根据其类型调用gen函数生成指令
            {
                switch(table[i].kind)
                {
                    case constant:
                        gen(lit,0,table[i].val);
                        break;
                    case variable:
                        gen(lod,lev-table[i].level,table[i].addr);
                        break;
                    case proc:                        ///如果是过程标识，报错
                        error(21);
                        break;
                }
            }
            getsym();                                 ///预读一个单词
        }
        else if(sym==number)                          ///如果当前单词是数字
        {
            if(num>amax)                              ///如果数值超限，报错
            {
                error(30);
                num=0;
            }
            gen(lit,0,num);                           ///生成指令
            getsym();                                 ///预读一个单词
        }
        else if(sym==lparen)                          ///如果是 (
        {
            getsym();
            expression(rparen|fsys);                  ///调用表达式分析处理函数expression
            if(sym==rparen)                           ///如果是 ) ，分析成功，预读单词
            {
                getsym();
            }
            else                                      ///否则报错
            {
                error(22);
            }
        }
        test(fsys,lparen,23);                         ///测试因子后面的符号是否正确
    }
}


/***************************
 *Function: term
 *Description: 项分析处理
 *Input: fsys为停止符号集合
 ***************************/
void term(unsigned long long fsys)
{
    unsigned long mulop;
    factor(fsys|times|slash);          ///调用因子分析处理函数factor
    while(sym==times||sym==slash)      ///如果单词是 * 或 /
    {
        mulop=sym;                     ///将当前单词赋给mulop
        getsym();
        factor(fsys|times|slash);      ///调用因子分析处理函数factor
        if(mulop==times)               ///根据mulop是 * 或 / 调用gen函数生成不同的指令
        {
            gen(opr,0,4);              ///乘
        }
        else
        {
            gen(opr,0,5);              ///除
        }
    }
}


/*************************************
 *Function: expression
 *Description: 表达式分析处理函数体
 *Input: fsys为停止符号集合
 *************************************/
void expression(unsigned long long fsys)
{
    unsigned long long addop;
    if(sym==plus||sym==minus)           ///如果单词为 + 或 -
    {
        addop=sym;                      ///将单词存入addop中
        getsym();
        term(fsys|plus|minus);          ///调用项分析处理函数term
        if(addop==minus)                ///如果addop为 - ，调用gen函数生成负数指令
        {
            gen(opr,0,1);
        }
    }
    else                                ///若不是 + 或 - ，直接调用项分析处理函数term
    {
        term(fsys|plus|minus);
    }
    while(sym==plus||sym==minus)        ///循环读取 +项 或 -项
    {
        addop=sym;
        getsym();
        term(fsys|plus|minus);          ///调用项分析处理函数term
        if(addop==plus)                 ///+项，调用gen函数生成加指令
        {
            gen(opr,0,2);
        }
        else                            ///-项，调用gen函数生成减指令
        {
            gen(opr,0,3);
        }
    }
}


/****************************
 *Function: condition
 *Description: 条件分析处理
 *Input: fsys为停止符号集合
 ****************************/
void condition(unsigned long long fsys)
{
    unsigned long long relop;
    if(sym==oddsym)                                      ///如果单词是 odd
    {
        getsym();
        expression(fsys);                                ///调用表达式分析处理函数expression
        gen(opr,0,6);                                    ///调用gen函数生成指令
    }
    else
    {
        expression(fsys|eql|neq|lss|gtr|leq|geq);        ///调用表达式分析处理函数expression
        if(!(sym&(eql|neq|lss|gtr|leq|geq)))             ///如果下一个单词不是运算关系符，报错
        {
            error(20);
        }
        else                                             ///如果是运算关系符
        {
            relop=sym;                                   ///将运算关系符存入relop中
            getsym();
            expression(fsys);                            ///调用表达式分析处理函数expression
            switch(relop)                                ///根据relop的不同调用gen函数生成相应的指令
            {
                case eql:                                ///=表达式
                    gen(opr,0,8);
                    break;
                case neq:                                ///<>表达式
                    gen(opr,0,9);
                    break;
                case lss:                                ///<表达式
                    gen(opr,0,10);
                    break;
                case geq:                                ///>=表达式
                    gen(opr,0,11);
                    break;
                case gtr:                                ///>表达式
                    gen(opr,0,12);
                    break;
                case leq:                                ///<=表达式
                    gen(opr,0,13);
                    break;
            }
        }
    }
}


/*********************************
 *Function: statement
 *Description: 语句部分分析处理
 *Input: fsys为停止符号集合
 *********************************/
void statement(unsigned long long fsys)
{
    long i,cx1,cx2;
    if(sym==ident)                                  ///如果单词是标识符
    {
        i=position(id);                             ///调用position函数在符号表中查找该标识符
        if(i==0)                                    ///如果没找到报错
        {
            error(11);
        }
        else if(table[i].kind!=variable)            ///如果标识符类型不是变量，报错
        {
            error(12);
            i=0;
        }
        getsym();
        if(sym==becomes)                            ///如果标识符是 := ，读单词
        {
            getsym();
        }
        else                                        ///否则报错
        {
            error(13);
        }
        expression(fsys);                           ///调用表达式分析处理函数expression
        if(i!=0)                                    ///调用gen函数生成指令
        {
            gen(sto,lev-table[i].level,table[i].addr);
        }
    }
    else if(sym==callsym)                           ///如果单词是 call
    {
        getsym();
        if(sym!=ident)                              ///如果单词不是标识符，报错
        {
            error(14);
        }
        else
        {
            i=position(id);                         ///否则调用position函数在符号表中查找该标识符
            if(i==0)                                ///如果未找到，报错
            {
                error(11);
            }
            else if(table[i].kind==proc)            ///如果标识符是过程，调用gen函数生成指令
            {
                gen(cal,lev-table[i].level,table[i].addr);
            }
            else                                    ///如果标识符不是过程类型，报错
            {
                error(15);
            }
            getsym();
        }
    }
    else if(sym==ifsym)                             ///如果单词是 if
    {
        getsym();
        condition(fsys|thensym|dosym);              ///调用条件分析处理函数condition
        if(sym==thensym)                            ///如果单词是 then ，读单词
        {
            getsym();
        }
        else                                        ///否则出错
        {
            error(16);
        }
        cx1=cx;                                     ///用cx1记录当前指令索引
        gen(jpc,0,0);                               ///调用gen函数生成jpc指令，跳转地址先赋为0
        statement(fsys|elsesym);                    ///调用语句分析处理函数statement
        if(sym==elsesym)                            ///如果单词是else
        {
            getsym();                               ///读单词
            cx2=cx;                                 ///用cx2记录jmp指令代码索引
            gen(jmp,0,0);                           ///调用gen函数生成then语句结束后的jmp指令，跳转地址先赋为0
            code[cx1].a=cx;                         ///将上文生成的jpc指令的跳转地址赋为现在的指令索引
            statement(fsys);                        ///调用statement分析处理else对应的语句
            code[cx2].a=cx;                         ///将上文生成的jmp指令的跳转地址赋为现在的指令索引
        }
        else
        {
            code[cx1].a=cx;                         ///将上文生成的jpc指令的跳转地址赋为现在的指令索引
        }
    }
    else if(sym==beginsym)                          ///如果单词是 begin
    {
        getsym();
        statement(fsys|semicolon|endsym);           ///调用语句分析处理函数statement
        while(sym==semicolon||(sym&statbegsys))     ///如果单词是 ; 或在语句合法头符号集中
        {
            if(sym==semicolon)                      ///如果单词是 ; ，读单词
            {
                getsym();
            }
            else
            {
                error(10);                          ///否则报错
            }
            statement(fsys|semicolon|endsym);       ///调用语句分析处理函数statement
        }
        if(sym==endsym)                             ///如果单词是 end ，读单词
        {
            getsym();
        }
        else                                        ///否则报错
        {
            error(17);
        }
    }
    else if(sym==whilesym)                          ///如果单词是 while
    {
        cx1=cx;                                     ///用cx1记录当前指令索引
        getsym();
        condition(fsys|dosym);                      ///调用条件分析处理函数condition
        cx2=cx;                                     ///用cx2记录调用conditon后的指令索引
        gen(jpc,0,0);                               ///调用gen函数生成jpc指令，跳转地址先赋为0
        if(sym==dosym)                              ///如果单词是 do ，读单词
        {
            getsym();
        }
        else                                        ///否则报错
        {
            error(18);
        }
        statement(fsys);                            ///调用语句分析处理函数statement
        gen(jmp,0,cx1);                             ///调用gen函数生成jmp指令，跳转地址为cx1
        code[cx2].a=cx;                             ///将所因为cx2的jmp指令的跳转地址置为当前指令索引
    }
    else if(sym==repeatsym)
    {
        cx1=cx;                                     ///用cx1记录当前指令索引
        getsym();                                   ///读单词
        statement(fsys|semicolon|untilsym);         ///调用语句分析处理函数statement
        while(sym==semicolon||(sym&statbegsys))     ///如果单词是 ; 或在语句合法头符号集中
        {
            if(sym==semicolon)                      ///如果单词是 ; ，读单词
            {
                getsym();
            }
            else
            {
                error(10);                          ///否则报错
            }
            statement(fsys|semicolon|untilsym);     ///调用语句分析处理函数statement
        }
        if(sym==untilsym)                           ///如果单词是 until ，读单词
        {
            getsym();
        }
        else                                        ///否则报错
        {
            error(19);
        }
        condition(fsys);                            ///调用条件分析处理函数condition
        gen(jpc,0,cx1);                             ///调用gen函数生成jpc指令，跳转的地址为cx1记录的指令索引
    }
    else if(sym==readsym)                           ///如果单词是read
    {
        getsym();                                   ///读单词
        if(sym==lparen)                             ///如果是右括号
        {
            do
            {
                getsym();                           ///读单词
                if(sym==ident)                      ///如果是标识符
                {
                    i=position(id);                 ///调用position函数查找标识符在符号表中的位置
                    if(i==0)                        ///如果未声明标识符，报错
                    {
                        error(11);
                    }
                    else
                    {                               ///如果标识符不是变量，报错
                        if(table[i].kind==proc||table[i].kind==constant)
                        {
                            error(12);
                        }
                        else                        ///调用gen函数生成指令
                        {
                            gen(red,lev-table[i].level,table[i].addr);
                        }
                    }
                    getsym();                       ///读单词，如果是 , ，就循环读标识符
                }
            }while(sym==comma);
            if(sym==rparen)                         ///如果是 ) ，读单词
            {
                getsym();
            }
            else                                    ///如果不是 ) ，报错
            {
                error(22);
            }
        }
        else                                        ///如果不是 ( ，报错
        {
            error(40);
        }
    }
    else if(sym==writesym)                          ///如果单词是read
    {
        getsym();                                   ///读单词
        if(sym==lparen)                             ///如果是右括号
        {
            do
            {
                getsym();                           ///读单词
                if(sym==ident)                      ///如果是标识符
                {
                    i=position(id);                 ///调用position函数查找标识符在符号表中的位置
                    if(i==0)                        ///如果未声明标识符，报错
                    {
                        error(11);
                    }
                    else
                    {                               ///如果标识符不是变量，报错
                        if(table[i].kind==proc||table[i].kind==constant)
                        {
                            error(12);
                        }
                        else                        ///调用gen函数生成指令
                        {
                            gen(lod,lev-table[i].level,table[i].addr);
                            gen(wrt,0,0);
                        }
                    }
                    getsym();                       ///读单词，如果是 , ，就循环读标识符
                }
                else if(sym==number)                 ///如果是数字
                {
                    gen(lit,0,num);
                    gen(wrt,0,0);
                    getsym();                       ///读单词，如果是 , ，就循环读标识符
                }
            }while(sym==comma);
            if(sym==rparen)                         ///如果是 ) ，读单词
            {
                getsym();
            }
            else                                    ///如果不是 ) ，报错
            {
                error(22);
            }
        }
        else                                        ///如果不是 ( ，报错
        {
            error(40);
        }
    }
    test(fsys,0,19);                                ///测试语句后面的符号是否正确
}


/*******************************
 *Function: block
 *Description: 分程序分析处理
 *Input: fsys为停止符号集合
 *******************************/
void block(unsigned long long fsys)
{
    long tx0;
    long cx0;
    long tx1;
    long dx1;
    dx=3;
    tx0=tx;                                   ///用tx0记录当前tx值
    table[tx].addr=cx;                        ///在table[tx0].addr中记录当前jmp指令索引
    gen(jmp,0,0);                             ///调用gen函数生成jmp指令，从声明部分跳转到语句部分
    if(lev>levmax)                            ///如果层次超限，报错
    {
        error(32);
    }
    do
    {
        if(sym==constsym)                     ///如果单词是 const
        {
            getsym();                         ///读单词
            do
            {
                constdeclaration();           ///调用常量定义处理函数constdeclareation
                while(sym==comma)             ///如果单词是 , ，循环调用常量定义处理函数
                {
                    getsym();
                    constdeclaration();
                }
                if(sym==semicolon)            ///如果单词是 ; ，读单词
                {
                    getsym();
                }
                else                          ///否则报错
                {
                    error(5);
                }
            }while(sym==ident);               ///如果单词是标识符，循环
        }
        if(sym==varsym)                       ///如果单词是 var
        {
            getsym();                         ///读单词
            do
            {
                vardeclaration();             ///调用变量定义处理函数vardeclareation
                while(sym==comma)             ///如果单词是 , ，循环调用变量定义处理函数
                {
                    getsym();
                    vardeclaration();
                }
                if(sym==semicolon)            ///如果单词是 ; ，读单词
                {
                    getsym();
                }
                else                          ///否则报错
                {
                    error(5);
                }
            }while(sym==ident);               ///如果单词是标识符，循环
        }
        while(sym==procsym)                   ///如果单词是 procedure
        {
            getsym();                         ///读单词
            if(sym==ident)                    ///如果单词是标识符
            {
                enter(proc);                  ///将该过程写入符号表
                getsym();
            }
            else                              ///否则报错
            {
                error(4);
            }
            if(sym==semicolon)                ///如果单词是 ; ，读单词
            {
                getsym();
            }
            else                              ///否则报错
            {
                error(5);
            }
            lev=lev+1;                        ///层次加1
            tx1=tx;                           ///tx1记录当前符号表索引
            dx1=dx;                           ///dx1记录当前数据索引
            block(fsys|semicolon);            ///调用分程序分析处理函数block
            lev=lev-1;                        ///回到原层次
            tx=tx1;                           ///符号表索引返回到调用前的值
            dx=dx1;                           ///数据索引返回到调用前的值
            if(sym==semicolon)                ///如果单词是 ; ，读单词，测试分程序后面的符号是否正确
            {
                getsym();
                test(statbegsys|ident|procsym,fsys,6);
            }
            else                              ///否则报错
            {
                error(5);
            }
        }
        test(statbegsys|ident,declbegsys,7);  ///测试声明语句后面的符号是否正确
    }while(sym&declbegsys);
    code[table[tx0].addr].a=cx;               ///block开头生成的jmp指令的跳转地址赋为statement指令段开始的指令索引
    table[tx0].addr=cx;                       ///block开始生成的符号表addr项赋为statement指令段开始的指令索引
    cx0=cx;
    gen(Int,0,dx);
    statement(fsys|semicolon|endsym);         ///调用语句分析处理函数statement
    gen(opr,0,0);                             ///调用gen函数生成opr指令，功能是return
    test(fsys,0,8);                           ///测试分程序后面的符号是否正确
}


/*******************************************
 *Function: base
 *Description: 通过静态链求出数据区的地址
 *Input: b为基地址，l为层次差
 *******************************************/
long base(long b, long l)
{
    long b1;
    b1=b;
    while(l>0)
    {
        b1=s[b1];
        l=l-1;
    }
    return b1;
}


/**********************************
 *Function: interpret
 *Description: P-code解释执行程序
 **********************************/
void interpret()
{
    long p,b,t;                              ///p为当前读的指令索引，b为基地址，t为数据栈栈顶指针
    instruction i;
    printf("\nSTART PL/0\n");
    t=0;
    b=1;
    p=0;
    s[1]=0;
    s[2]=0;
    s[3]=0;
    do
    {
        i=code[p];                           ///读取一条指令
        p=p+1;                               ///指令索引加1
        switch(i.f)
        {
            case lit:                        ///取常量放到数据栈栈顶
                t=t+1;
                s[t]=i.a;
                break;
            case opr:                        ///指令运算
                switch(i.a)
                {
                    case 0:                  ///返回 return
                        t=b-1;               ///栈顶指针指向该调用过程前一个索引
                        p=s[t+3];            ///返回到call的下一条指令
                        b=s[t+2];            ///b指向上一层基地址
                        break;
                    case 1:                  ///取负 -
                        s[t]=-s[t];
                        break;
                    case 2:                  ///加 +
                        t=t-1;
                        s[t]=s[t]+s[t+1];
                        break;
                    case 3:                  ///减 -
                        t=t-1;
                        s[t]=s[t]-s[t+1];
                        break;
                    case 4:                  ///乘 *
                        t=t-1;
                        s[t]=s[t]*s[t+1];
                        break;
                    case 5:                  ///除 /
                        t=t-1;
                        s[t]=s[t]/s[t+1];
                        break;
                    case 6:                  ///判断奇偶，奇数为1
                        s[t]=s[t]%2;
                        break;
                    case 8:                  ///相等 =
                        t=t-1;
                        s[t]=(s[t]==s[t+1]);
                        break;
                    case 9:                  ///不等 <>
                        t=t-1;
                        s[t]=(s[t]!=s[t+1]);
                        break;
                    case 10:                 ///小于 <
                        t=t-1;
                        s[t]=(s[t]<s[t+1]);
                        break;
                    case 11:                 ///大于等于 >=
                        t=t-1;
                        s[t]=(s[t]>=s[t+1]);
                        break;
                    case 12:                 ///大于 >
                        t=t-1;
                        s[t]=(s[t]>s[t+1]);
                        break;
                    case 13:                 ///小于等于 <=
                        t=t-1;
                        s[t]=(s[t]<=s[t+1]);
                }
                break;
            case lod:                        ///取变量到栈顶
                t=t+1;
                s[t]=s[base(b,i.l)+i.a];
                break;
            case sto:                        ///将栈顶存入变量
                s[base(b,i.l)+i.a]=s[t];
                t=t-1;
                break;
            case cal:                        ///调用过程
                s[t+1]=base(b,i.l);          ///静态链索引
                s[t+2]=b;                    ///动态链索引
                s[t+3]=p;                    ///记录返回指令索引
                b=t+1;                       ///b赋为当前调用过程的基地址
                p=i.a;                       ///将指令索引p赋为调用过程代码段起始代码索引
                break;
            case Int:                        ///数据栈栈顶指针增加
                t=t+i.a;
                break;
            case jmp:                        ///无条件跳转到指令
                p=i.a;
                break;
            case jpc:                        ///条件跳转到指令
                if(s[t]==0)
                {
                    p=i.a;
                }
                t=t-1;
                break;
            case red:
                printf("Input: ");           ///从控制台读数到数据栈的绝对地址
                scanf("%ld",&s[base(b,i.l)+i.a]);
                break;
            case wrt:                        ///输出数据站绝对地址的值到控制台
                printf("%5d\n",s[t]);
        }
    }while(p!=0);
    printf("END PL/0\n");
}


/*************************
 *Function: main
 *Description: 主程序
 *************************/
main()
{
    long i;
    for(i=0; i<256; i++)                    ///将符号值队列ssym置为空
    {
        ssym[i]=nul;
    }

    strcpy(word[0],  "begin     ");         ///初始化保留字队列word
    strcpy(word[1],  "call      ");
    strcpy(word[2],  "const     ");
    strcpy(word[3],  "do        ");
    strcpy(word[4],  "else      ");
    strcpy(word[5],  "end       ");
    strcpy(word[6],  "if        ");
    strcpy(word[7],  "odd       ");
    strcpy(word[8],  "procedure ");
    strcpy(word[9],  "read      ");
    strcpy(word[10], "repeat    ");
    strcpy(word[11], "then      ");
    strcpy(word[12], "until     ");
    strcpy(word[13], "var       ");
    strcpy(word[14], "while     ");
    strcpy(word[15], "write     ");

    wsym[0]=beginsym;                       ///初始化保留字值队列wsym
    wsym[1]=callsym;
    wsym[2]=constsym;
    wsym[3]=dosym;
    wsym[4]=elsesym;
    wsym[5]=endsym;
    wsym[6]=ifsym;
    wsym[7]=oddsym;
    wsym[8]=procsym;
    wsym[9]=readsym;
    wsym[10]=repeatsym;
    wsym[11]=thensym;
    wsym[12]=untilsym;
    wsym[13]=varsym;
    wsym[14]=whilesym;
    wsym[15]=writesym;

    ssym['+']=plus;                         ///初始化符号值队列ssym
    ssym['-']=minus;
    ssym['*']=times;
    ssym['/']=slash;
    ssym['(']=lparen;
    ssym[')']=rparen;
    ssym['=']=eql;
    ssym[',']=comma;
    ssym['.']=period;
    ssym[';']=semicolon;

    strcpy(mnemonic[lit],"LIT");            ///初始化操作符队列
    strcpy(mnemonic[opr],"OPR");
    strcpy(mnemonic[lod],"LOD");
    strcpy(mnemonic[sto],"STO");
    strcpy(mnemonic[cal],"CAL");
    strcpy(mnemonic[Int],"INT");
    strcpy(mnemonic[jmp],"JMP");
    strcpy(mnemonic[jpc],"JPC");
    strcpy(mnemonic[red],"RED");
    strcpy(mnemonic[wrt],"WRT");

    declbegsys=constsym|varsym|procsym;                       ///初始化声明部分合法头符号集
    statbegsys=beginsym|callsym|ifsym|whilesym
                   |repeatsym|readsym|writesym;               ///初始化语句部分合法头符号集
    facbegsys=ident|number|lparen;                            ///初始化因子部分合法头符号集

    printf("Please input source program file name: ");
    scanf("%s",infilename);
    printf("\n");
    if((infile=fopen(infilename,"r"))==NULL)                  ///读源程序文件，如果不能打开给出提示，中断程序
    {
        printf("File %s can't be opened.\n", infilename);
        exit(1);
    }

    err=0;                                      ///初始化变量
    cc=0;
    cx=0;
    ll=0;
    ch=' ';
    kk=al;
    getsym();                                   ///预读单词
    lev=0;
    tx=0;
    block(declbegsys|statbegsys|period);        ///调用分程序分析处理函数block
    if(sym!=period)                             ///如果最后不是 . ，报错
    {
        error(9);
    }

    printf("\nList P-code in order:\n");
    listcode(0);                                ///列出P-code指令

    if(err==0)                                  ///如果编译没有错误，解释执行
    {
        interpret();
    }
    else                                        ///如果编译有错误给出提示，不解释执行
    {
        printf("errors in PL/0 program\n");
    }
    fclose(infile);                             ///关闭文件
}
