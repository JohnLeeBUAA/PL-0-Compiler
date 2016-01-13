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
 *Description: ��������ӡ�������
 *Input: nΪ�������
 ***************************************/
void error(long n)
{
    long i;
    printf(" ****");            ///��ӡ�����ʶ
    for (i=1; i<=cc-1; i++)     ///������Ƶ�������ַ���
    {
        printf(" ");
    }
    printf("^%2d\n",n);         ///��ӡ�������
    err++;                      ///��������1
}


/*************************
 *Function: getch
 *Description: ���ַ�
 *************************/
void getch()
{
    if(cc==ll)                    ///������ǰ�ַ���һ�еĽ�β
    {
        if(feof(infile))          ///����ļ���������������������ʾ��������ֹ
        {
            printf("************************************\n");
            printf("      program incomplete\n");
            printf("************************************\n");
            exit(1);
        }
        ll=0;                     ///�õ�ǰ�г���Ϊ0
        cc=0;                     ///�õ�ǰ�ַ�λ��Ϊ0
        printf("%5d ", cx);       ///��ӡָ��������

                                  ///����һ�д�ӡ������line[]��
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
    cc=cc+1;                      ///ȡ��ǰλ�õ���һ���ַ���ch��
    ch=line[cc];
}


/***************************************
 *Function: getsym
 *Description: �ʷ���������ȡһ������
 ***************************************/
void getsym()
{
    long i,j,k;
    while(ch==' '||ch=='\t')                    ///�����ո���Ʊ��
    {
        getch();
    }
    if(isalpha(ch))                             ///�����ǰ�ַ�����ĸ����symΪ��ʶ��������
    {
        k=0;
        do
        {
            if(k<al)                            ///������ʳ��ȴ�������������Զ��ض�
            {
                a[k]=ch;
                k=k+1;
            }
            getch();
        }while(isalpha(ch)||isdigit(ch));
        if(k>=kk)                               ///kΪ��ǰ���ʳ��ȣ�kkΪ��һ�����ʵĳ���
        {
            kk=k;
        }
        else                                    ///�����ǰ���ʳ���С����һ�����ʳ��ȣ���a���ж̵Ĳ���λ���ַ��ÿ�
        {
            do
            {
                kk=kk-1;
                a[kk]=' ';
            }while(k<kk);
        }
        strcpy(id,a);                           ///����ǰ������ַ���a������ǰ��ʶ��id

        i=0;                                    ///�ö��ַ����ұ����ֱ�
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
        if(i-1>j)                               ///����ҵ�˵��Ϊ�����֣���ֵ����sym
        {
            sym=wsym[k];
        }
        else                                    ///���û�ҵ���˵��symΪ��ʶ��
        {
            sym=ident;
        }
    }
    else if(isdigit(ch))                        ///�����ǰ�ַ�������
    {
        k=0;
        num=0;
        sym=number;                             ///����������ֵ����sym
        do                                      ///�������ַ���ת�������ָ���num
        {
            num=num*10+(ch-'0');
            k=k+1;
            getch();
        }while(isdigit(ch));
        if(k>nmax)                              ///�������λ���������λ�����ƣ�����
        {
            error(30);
        }
    }
    else if(ch==':')                            ///�ж��� nul ���� :=
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
    else if(ch=='<')                            ///�ж��� < ���� <= ���� <>
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
    else if(ch=='>')                            ///�ж��� > ���� >=
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
    else                                        ///��������
    {
        sym=ssym[(unsigned char)ch];
        getch();
    }
}


/***********************************************
 *Function: gen
 *Description: ����P-codeָ�����Ŀ�������
 *Input: xΪָ������룬yΪ��βzΪ��Ե�ַ
 ***********************************************/
void gen(enum fct x, long y, long z)
{
    if(cx>cxmax)                         ///ָ������ָ�볬�����ָ������ָ��
    {
        printf("program too long\n");    ///���������Ϣ
        exit(1);                         ///�������
    }
    code[cx].f=x;                        ///���򽫵�ǰָ��Ĳ����롢
    code[cx].l=y;                        ///��β
    code[cx].a=z;                        ///��Ե�ַ����codeָ�������cxָ�����Ӧ����
    cx=cx+1;                             ///��ָ������ָ���1
}


/*****************************************************************
 *Function: test
 *Description: ���Ե�ǰ���ʷ����Ƿ�Ϸ�
 *Input: fsys1Ϊ�Ϸ����ż��ϣ�fsys2Ϊֹͣ���ż��ϣ�nΪ�������
 *****************************************************************/
void test(unsigned long long fsys1, unsigned long long fsys2, long n)
{
    if (!(sym & fsys1))          ///����һ��ȡ���ķ��Ų��Ǹ��﷨�Ϸ��ĺ�̷���
    {
        error(n);             ///���������Ϊn�Ĵ���
        fsys1=fsys1|fsys2;
        while(!(sym & fsys1))    ///��������ֱ��ȡ���ķ������ںϷ���̷��ż��ϻ�ֹͣ���ż���
        {
            getsym();
        }
    }
}


/******************************************************************
 *Function: enter
 *Description: ��¼���ű�
 *Input: kΪҪд����ű�Ķ���ö�����ͣ�constant/variable/proc��
 ******************************************************************/
void enter(enum object k)
{
    tx=tx+1;                            ///�����ű�������1
    strcpy(table[tx].name,id);          ///����ǰ��ʶ�����ֺ����͸������ű�������λ����ӦԪ��
    table[tx].kind=k;
    switch(k)
    {
        case constant:                  ///����ǳ���
            if(num>amax)                ///�����ֵ���ޣ�����num��Ϊ0
            {
                error(30);
                num = 0;
            }
            table[tx].val=num;          ///��ֵ�Ϸ����丳��val
            break;
        case variable:                  ///����Ǳ����������Ρ����ݷ��������������ű��еĲ�Ρ���ַ
            table[tx].level=lev;
            table[tx].addr=dx;
            dx=dx+1;
            break;
        case proc:                      ///�����procedureֻ�����ֵ����ַӦ�������ɵ�P-codeָ�����е���ڵ�ַ��������ȷ��
            table[tx].level=lev;
            break;
    }
}


/************************************************************************************
 *Function: positon
 *Description: ���ұ�ʶ�ڷ��ű��е�λ��
 *Input: idΪҪ���ҵı�ʶ���Ŵ�
 *Output: ���ظñ�ʶ���ڷ��ű��е�λ�ã������ű����޴����֣��򷵻�ֵ0����ʾ����
 ************************************************************************************/
long position(char* id)
{
    long i;
    strcpy(table[0].name,id);              ///��Ҫ���ҵı�ʶ���������ű����Ԫ�ص����֣����������ű����޸ñ�ʶ���ܷ���0
    i=tx;                                  ///�����ű��������������i
    while(strcmp(table[i].name,id)!=0)     ///���¶��ϱȽϷ��ű�ÿһ��Ŀ��������Ҫ���ҵı�ʶ��
    {
        i=i-1;
    }
    return i;                              ///���ҵ���iΪ��ʶ��λ�ã������ű����޸ñ�ʶ�������ڵ���Ԫ���ҵ�������iֵΪ0
}


/****************************
 *Function: constdeclaration
 *Description: �������崦��
 ****************************/
void constdeclaration()
{
    if(sym==ident)                        ///����������Ǳ�ʶ��
    {
        getsym();                         ///��һ������
        if(sym==eql||sym==becomes)        ///����� = �� :=
        {
            if(sym==becomes)              ///����� = ��д�� := ������
            {
                error(1);
            }
            getsym();                     ///��һ�����ʣ���������ֽ���д����ű�
            if(sym==number)
            {
                enter(constant);
                getsym();                 ///Ԥ��һ������
            }
            else                          ///���򱨴�
            {
                error(2);
            }
        }
        else                              ///��ʶ������ = ������
        {
            error(3);
        }
    }
    else                                  ///������Ǳ�ʶ��������
    {
        error(4);
    }
}


/****************************
 *Function: vardeclaration
 *Description: ����˵������
 ****************************/
void vardeclaration()
{
    if(sym==ident)            ///�����ǰ�����Ǳ�ʶ��
    {
        enter(variable);      ///д����ű�
        getsym();             ///Ԥ��һ������
    }
    else
    {
        error(4);             ///���򱨴�
    }
}


/*********************************************
 *Function: listcode
 *Description: �г�P-codeָ���嵥
 *Input: cx0ΪҪ��ӡ��ָ��ε���ʼָ������
 *********************************************/
void listcode(cx0)
{
    long i;
    for(i=cx0; i<=cx-1; i++)         ///ѭ����ӡ�����ţ��кţ��������롢��Ρ���ַ
    {
        printf("%10d%5s%3d%5d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
    }
}


/*****************************************************************
 *Function: expression
 *Description: ���ʽ��������������
 *��Ϊfactor()��term()��expression()�漰�������������Ҫ������
 *****************************************************************/
void expression(unsigned long long fsys);


/*****************************
 *Function: factor
 *Description: ���ӷ�������
 *Input: fsysΪֹͣ���ż���
 *****************************/
void factor(unsigned long long fsys)
{
    long i;
    test(facbegsys,fsys,24);                          ///����test�����жϵ�ǰ�����Ƿ���factor�ĺϷ�ͷ���ż��У������ڱ�������
    while(sym & facbegsys)                            ///ѭ��������
    {
        if(sym==ident)                                ///�����ǰ�����Ǳ�ʶ��
        {
            i=position(id);                           ///����position������ȡ���ڷ��ű��е�λ��
            if(i==0)                                  ///���Ϊû�����ı�ʶ��������
            {
                error(11);
            }
            else                                      ///����ǺϷ��ı�ʶ�������������͵���gen��������ָ��
            {
                switch(table[i].kind)
                {
                    case constant:
                        gen(lit,0,table[i].val);
                        break;
                    case variable:
                        gen(lod,lev-table[i].level,table[i].addr);
                        break;
                    case proc:                        ///����ǹ��̱�ʶ������
                        error(21);
                        break;
                }
            }
            getsym();                                 ///Ԥ��һ������
        }
        else if(sym==number)                          ///�����ǰ����������
        {
            if(num>amax)                              ///�����ֵ���ޣ�����
            {
                error(30);
                num=0;
            }
            gen(lit,0,num);                           ///����ָ��
            getsym();                                 ///Ԥ��һ������
        }
        else if(sym==lparen)                          ///����� (
        {
            getsym();
            expression(rparen|fsys);                  ///���ñ��ʽ����������expression
            if(sym==rparen)                           ///����� ) �������ɹ���Ԥ������
            {
                getsym();
            }
            else                                      ///���򱨴�
            {
                error(22);
            }
        }
        test(fsys,lparen,23);                         ///�������Ӻ���ķ����Ƿ���ȷ
    }
}


/***************************
 *Function: term
 *Description: ���������
 *Input: fsysΪֹͣ���ż���
 ***************************/
void term(unsigned long long fsys)
{
    unsigned long mulop;
    factor(fsys|times|slash);          ///�������ӷ���������factor
    while(sym==times||sym==slash)      ///��������� * �� /
    {
        mulop=sym;                     ///����ǰ���ʸ���mulop
        getsym();
        factor(fsys|times|slash);      ///�������ӷ���������factor
        if(mulop==times)               ///����mulop�� * �� / ����gen�������ɲ�ͬ��ָ��
        {
            gen(opr,0,4);              ///��
        }
        else
        {
            gen(opr,0,5);              ///��
        }
    }
}


/*************************************
 *Function: expression
 *Description: ���ʽ������������
 *Input: fsysΪֹͣ���ż���
 *************************************/
void expression(unsigned long long fsys)
{
    unsigned long long addop;
    if(sym==plus||sym==minus)           ///�������Ϊ + �� -
    {
        addop=sym;                      ///�����ʴ���addop��
        getsym();
        term(fsys|plus|minus);          ///���������������term
        if(addop==minus)                ///���addopΪ - ������gen�������ɸ���ָ��
        {
            gen(opr,0,1);
        }
    }
    else                                ///������ + �� - ��ֱ�ӵ��������������term
    {
        term(fsys|plus|minus);
    }
    while(sym==plus||sym==minus)        ///ѭ����ȡ +�� �� -��
    {
        addop=sym;
        getsym();
        term(fsys|plus|minus);          ///���������������term
        if(addop==plus)                 ///+�����gen�������ɼ�ָ��
        {
            gen(opr,0,2);
        }
        else                            ///-�����gen�������ɼ�ָ��
        {
            gen(opr,0,3);
        }
    }
}


/****************************
 *Function: condition
 *Description: ������������
 *Input: fsysΪֹͣ���ż���
 ****************************/
void condition(unsigned long long fsys)
{
    unsigned long long relop;
    if(sym==oddsym)                                      ///��������� odd
    {
        getsym();
        expression(fsys);                                ///���ñ��ʽ����������expression
        gen(opr,0,6);                                    ///����gen��������ָ��
    }
    else
    {
        expression(fsys|eql|neq|lss|gtr|leq|geq);        ///���ñ��ʽ����������expression
        if(!(sym&(eql|neq|lss|gtr|leq|geq)))             ///�����һ�����ʲ��������ϵ��������
        {
            error(20);
        }
        else                                             ///����������ϵ��
        {
            relop=sym;                                   ///�������ϵ������relop��
            getsym();
            expression(fsys);                            ///���ñ��ʽ����������expression
            switch(relop)                                ///����relop�Ĳ�ͬ����gen����������Ӧ��ָ��
            {
                case eql:                                ///=���ʽ
                    gen(opr,0,8);
                    break;
                case neq:                                ///<>���ʽ
                    gen(opr,0,9);
                    break;
                case lss:                                ///<���ʽ
                    gen(opr,0,10);
                    break;
                case geq:                                ///>=���ʽ
                    gen(opr,0,11);
                    break;
                case gtr:                                ///>���ʽ
                    gen(opr,0,12);
                    break;
                case leq:                                ///<=���ʽ
                    gen(opr,0,13);
                    break;
            }
        }
    }
}


/*********************************
 *Function: statement
 *Description: ��䲿�ַ�������
 *Input: fsysΪֹͣ���ż���
 *********************************/
void statement(unsigned long long fsys)
{
    long i,cx1,cx2;
    if(sym==ident)                                  ///��������Ǳ�ʶ��
    {
        i=position(id);                             ///����position�����ڷ��ű��в��Ҹñ�ʶ��
        if(i==0)                                    ///���û�ҵ�����
        {
            error(11);
        }
        else if(table[i].kind!=variable)            ///�����ʶ�����Ͳ��Ǳ���������
        {
            error(12);
            i=0;
        }
        getsym();
        if(sym==becomes)                            ///�����ʶ���� := ��������
        {
            getsym();
        }
        else                                        ///���򱨴�
        {
            error(13);
        }
        expression(fsys);                           ///���ñ��ʽ����������expression
        if(i!=0)                                    ///����gen��������ָ��
        {
            gen(sto,lev-table[i].level,table[i].addr);
        }
    }
    else if(sym==callsym)                           ///��������� call
    {
        getsym();
        if(sym!=ident)                              ///������ʲ��Ǳ�ʶ��������
        {
            error(14);
        }
        else
        {
            i=position(id);                         ///�������position�����ڷ��ű��в��Ҹñ�ʶ��
            if(i==0)                                ///���δ�ҵ�������
            {
                error(11);
            }
            else if(table[i].kind==proc)            ///�����ʶ���ǹ��̣�����gen��������ָ��
            {
                gen(cal,lev-table[i].level,table[i].addr);
            }
            else                                    ///�����ʶ�����ǹ������ͣ�����
            {
                error(15);
            }
            getsym();
        }
    }
    else if(sym==ifsym)                             ///��������� if
    {
        getsym();
        condition(fsys|thensym|dosym);              ///������������������condition
        if(sym==thensym)                            ///��������� then ��������
        {
            getsym();
        }
        else                                        ///�������
        {
            error(16);
        }
        cx1=cx;                                     ///��cx1��¼��ǰָ������
        gen(jpc,0,0);                               ///����gen��������jpcָ���ת��ַ�ȸ�Ϊ0
        statement(fsys|elsesym);                    ///����������������statement
        if(sym==elsesym)                            ///���������else
        {
            getsym();                               ///������
            cx2=cx;                                 ///��cx2��¼jmpָ���������
            gen(jmp,0,0);                           ///����gen��������then���������jmpָ���ת��ַ�ȸ�Ϊ0
            code[cx1].a=cx;                         ///���������ɵ�jpcָ�����ת��ַ��Ϊ���ڵ�ָ������
            statement(fsys);                        ///����statement��������else��Ӧ�����
            code[cx2].a=cx;                         ///���������ɵ�jmpָ�����ת��ַ��Ϊ���ڵ�ָ������
        }
        else
        {
            code[cx1].a=cx;                         ///���������ɵ�jpcָ�����ת��ַ��Ϊ���ڵ�ָ������
        }
    }
    else if(sym==beginsym)                          ///��������� begin
    {
        getsym();
        statement(fsys|semicolon|endsym);           ///����������������statement
        while(sym==semicolon||(sym&statbegsys))     ///��������� ; �������Ϸ�ͷ���ż���
        {
            if(sym==semicolon)                      ///��������� ; ��������
            {
                getsym();
            }
            else
            {
                error(10);                          ///���򱨴�
            }
            statement(fsys|semicolon|endsym);       ///����������������statement
        }
        if(sym==endsym)                             ///��������� end ��������
        {
            getsym();
        }
        else                                        ///���򱨴�
        {
            error(17);
        }
    }
    else if(sym==whilesym)                          ///��������� while
    {
        cx1=cx;                                     ///��cx1��¼��ǰָ������
        getsym();
        condition(fsys|dosym);                      ///������������������condition
        cx2=cx;                                     ///��cx2��¼����conditon���ָ������
        gen(jpc,0,0);                               ///����gen��������jpcָ���ת��ַ�ȸ�Ϊ0
        if(sym==dosym)                              ///��������� do ��������
        {
            getsym();
        }
        else                                        ///���򱨴�
        {
            error(18);
        }
        statement(fsys);                            ///����������������statement
        gen(jmp,0,cx1);                             ///����gen��������jmpָ���ת��ַΪcx1
        code[cx2].a=cx;                             ///������Ϊcx2��jmpָ�����ת��ַ��Ϊ��ǰָ������
    }
    else if(sym==repeatsym)
    {
        cx1=cx;                                     ///��cx1��¼��ǰָ������
        getsym();                                   ///������
        statement(fsys|semicolon|untilsym);         ///����������������statement
        while(sym==semicolon||(sym&statbegsys))     ///��������� ; �������Ϸ�ͷ���ż���
        {
            if(sym==semicolon)                      ///��������� ; ��������
            {
                getsym();
            }
            else
            {
                error(10);                          ///���򱨴�
            }
            statement(fsys|semicolon|untilsym);     ///����������������statement
        }
        if(sym==untilsym)                           ///��������� until ��������
        {
            getsym();
        }
        else                                        ///���򱨴�
        {
            error(19);
        }
        condition(fsys);                            ///������������������condition
        gen(jpc,0,cx1);                             ///����gen��������jpcָ���ת�ĵ�ַΪcx1��¼��ָ������
    }
    else if(sym==readsym)                           ///���������read
    {
        getsym();                                   ///������
        if(sym==lparen)                             ///�����������
        {
            do
            {
                getsym();                           ///������
                if(sym==ident)                      ///����Ǳ�ʶ��
                {
                    i=position(id);                 ///����position�������ұ�ʶ���ڷ��ű��е�λ��
                    if(i==0)                        ///���δ������ʶ��������
                    {
                        error(11);
                    }
                    else
                    {                               ///�����ʶ�����Ǳ���������
                        if(table[i].kind==proc||table[i].kind==constant)
                        {
                            error(12);
                        }
                        else                        ///����gen��������ָ��
                        {
                            gen(red,lev-table[i].level,table[i].addr);
                        }
                    }
                    getsym();                       ///�����ʣ������ , ����ѭ������ʶ��
                }
            }while(sym==comma);
            if(sym==rparen)                         ///����� ) ��������
            {
                getsym();
            }
            else                                    ///������� ) ������
            {
                error(22);
            }
        }
        else                                        ///������� ( ������
        {
            error(40);
        }
    }
    else if(sym==writesym)                          ///���������read
    {
        getsym();                                   ///������
        if(sym==lparen)                             ///�����������
        {
            do
            {
                getsym();                           ///������
                if(sym==ident)                      ///����Ǳ�ʶ��
                {
                    i=position(id);                 ///����position�������ұ�ʶ���ڷ��ű��е�λ��
                    if(i==0)                        ///���δ������ʶ��������
                    {
                        error(11);
                    }
                    else
                    {                               ///�����ʶ�����Ǳ���������
                        if(table[i].kind==proc||table[i].kind==constant)
                        {
                            error(12);
                        }
                        else                        ///����gen��������ָ��
                        {
                            gen(lod,lev-table[i].level,table[i].addr);
                            gen(wrt,0,0);
                        }
                    }
                    getsym();                       ///�����ʣ������ , ����ѭ������ʶ��
                }
                else if(sym==number)                 ///���������
                {
                    gen(lit,0,num);
                    gen(wrt,0,0);
                    getsym();                       ///�����ʣ������ , ����ѭ������ʶ��
                }
            }while(sym==comma);
            if(sym==rparen)                         ///����� ) ��������
            {
                getsym();
            }
            else                                    ///������� ) ������
            {
                error(22);
            }
        }
        else                                        ///������� ( ������
        {
            error(40);
        }
    }
    test(fsys,0,19);                                ///����������ķ����Ƿ���ȷ
}


/*******************************
 *Function: block
 *Description: �ֳ����������
 *Input: fsysΪֹͣ���ż���
 *******************************/
void block(unsigned long long fsys)
{
    long tx0;
    long cx0;
    long tx1;
    long dx1;
    dx=3;
    tx0=tx;                                   ///��tx0��¼��ǰtxֵ
    table[tx].addr=cx;                        ///��table[tx0].addr�м�¼��ǰjmpָ������
    gen(jmp,0,0);                             ///����gen��������jmpָ�������������ת����䲿��
    if(lev>levmax)                            ///�����γ��ޣ�����
    {
        error(32);
    }
    do
    {
        if(sym==constsym)                     ///��������� const
        {
            getsym();                         ///������
            do
            {
                constdeclaration();           ///���ó������崦����constdeclareation
                while(sym==comma)             ///��������� , ��ѭ�����ó������崦����
                {
                    getsym();
                    constdeclaration();
                }
                if(sym==semicolon)            ///��������� ; ��������
                {
                    getsym();
                }
                else                          ///���򱨴�
                {
                    error(5);
                }
            }while(sym==ident);               ///��������Ǳ�ʶ����ѭ��
        }
        if(sym==varsym)                       ///��������� var
        {
            getsym();                         ///������
            do
            {
                vardeclaration();             ///���ñ������崦����vardeclareation
                while(sym==comma)             ///��������� , ��ѭ�����ñ������崦����
                {
                    getsym();
                    vardeclaration();
                }
                if(sym==semicolon)            ///��������� ; ��������
                {
                    getsym();
                }
                else                          ///���򱨴�
                {
                    error(5);
                }
            }while(sym==ident);               ///��������Ǳ�ʶ����ѭ��
        }
        while(sym==procsym)                   ///��������� procedure
        {
            getsym();                         ///������
            if(sym==ident)                    ///��������Ǳ�ʶ��
            {
                enter(proc);                  ///���ù���д����ű�
                getsym();
            }
            else                              ///���򱨴�
            {
                error(4);
            }
            if(sym==semicolon)                ///��������� ; ��������
            {
                getsym();
            }
            else                              ///���򱨴�
            {
                error(5);
            }
            lev=lev+1;                        ///��μ�1
            tx1=tx;                           ///tx1��¼��ǰ���ű�����
            dx1=dx;                           ///dx1��¼��ǰ��������
            block(fsys|semicolon);            ///���÷ֳ������������block
            lev=lev-1;                        ///�ص�ԭ���
            tx=tx1;                           ///���ű��������ص�����ǰ��ֵ
            dx=dx1;                           ///�����������ص�����ǰ��ֵ
            if(sym==semicolon)                ///��������� ; �������ʣ����Էֳ������ķ����Ƿ���ȷ
            {
                getsym();
                test(statbegsys|ident|procsym,fsys,6);
            }
            else                              ///���򱨴�
            {
                error(5);
            }
        }
        test(statbegsys|ident,declbegsys,7);  ///��������������ķ����Ƿ���ȷ
    }while(sym&declbegsys);
    code[table[tx0].addr].a=cx;               ///block��ͷ���ɵ�jmpָ�����ת��ַ��Ϊstatementָ��ο�ʼ��ָ������
    table[tx0].addr=cx;                       ///block��ʼ���ɵķ��ű�addr�Ϊstatementָ��ο�ʼ��ָ������
    cx0=cx;
    gen(Int,0,dx);
    statement(fsys|semicolon|endsym);         ///����������������statement
    gen(opr,0,0);                             ///����gen��������oprָ�������return
    test(fsys,0,8);                           ///���Էֳ������ķ����Ƿ���ȷ
}


/*******************************************
 *Function: base
 *Description: ͨ����̬������������ĵ�ַ
 *Input: bΪ����ַ��lΪ��β�
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
 *Description: P-code����ִ�г���
 **********************************/
void interpret()
{
    long p,b,t;                              ///pΪ��ǰ����ָ��������bΪ����ַ��tΪ����ջջ��ָ��
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
        i=code[p];                           ///��ȡһ��ָ��
        p=p+1;                               ///ָ��������1
        switch(i.f)
        {
            case lit:                        ///ȡ�����ŵ�����ջջ��
                t=t+1;
                s[t]=i.a;
                break;
            case opr:                        ///ָ������
                switch(i.a)
                {
                    case 0:                  ///���� return
                        t=b-1;               ///ջ��ָ��ָ��õ��ù���ǰһ������
                        p=s[t+3];            ///���ص�call����һ��ָ��
                        b=s[t+2];            ///bָ����һ�����ַ
                        break;
                    case 1:                  ///ȡ�� -
                        s[t]=-s[t];
                        break;
                    case 2:                  ///�� +
                        t=t-1;
                        s[t]=s[t]+s[t+1];
                        break;
                    case 3:                  ///�� -
                        t=t-1;
                        s[t]=s[t]-s[t+1];
                        break;
                    case 4:                  ///�� *
                        t=t-1;
                        s[t]=s[t]*s[t+1];
                        break;
                    case 5:                  ///�� /
                        t=t-1;
                        s[t]=s[t]/s[t+1];
                        break;
                    case 6:                  ///�ж���ż������Ϊ1
                        s[t]=s[t]%2;
                        break;
                    case 8:                  ///��� =
                        t=t-1;
                        s[t]=(s[t]==s[t+1]);
                        break;
                    case 9:                  ///���� <>
                        t=t-1;
                        s[t]=(s[t]!=s[t+1]);
                        break;
                    case 10:                 ///С�� <
                        t=t-1;
                        s[t]=(s[t]<s[t+1]);
                        break;
                    case 11:                 ///���ڵ��� >=
                        t=t-1;
                        s[t]=(s[t]>=s[t+1]);
                        break;
                    case 12:                 ///���� >
                        t=t-1;
                        s[t]=(s[t]>s[t+1]);
                        break;
                    case 13:                 ///С�ڵ��� <=
                        t=t-1;
                        s[t]=(s[t]<=s[t+1]);
                }
                break;
            case lod:                        ///ȡ������ջ��
                t=t+1;
                s[t]=s[base(b,i.l)+i.a];
                break;
            case sto:                        ///��ջ���������
                s[base(b,i.l)+i.a]=s[t];
                t=t-1;
                break;
            case cal:                        ///���ù���
                s[t+1]=base(b,i.l);          ///��̬������
                s[t+2]=b;                    ///��̬������
                s[t+3]=p;                    ///��¼����ָ������
                b=t+1;                       ///b��Ϊ��ǰ���ù��̵Ļ���ַ
                p=i.a;                       ///��ָ������p��Ϊ���ù��̴������ʼ��������
                break;
            case Int:                        ///����ջջ��ָ������
                t=t+i.a;
                break;
            case jmp:                        ///��������ת��ָ��
                p=i.a;
                break;
            case jpc:                        ///������ת��ָ��
                if(s[t]==0)
                {
                    p=i.a;
                }
                t=t-1;
                break;
            case red:
                printf("Input: ");           ///�ӿ���̨����������ջ�ľ��Ե�ַ
                scanf("%ld",&s[base(b,i.l)+i.a]);
                break;
            case wrt:                        ///�������վ���Ե�ַ��ֵ������̨
                printf("%5d\n",s[t]);
        }
    }while(p!=0);
    printf("END PL/0\n");
}


/*************************
 *Function: main
 *Description: ������
 *************************/
main()
{
    long i;
    for(i=0; i<256; i++)                    ///������ֵ����ssym��Ϊ��
    {
        ssym[i]=nul;
    }

    strcpy(word[0],  "begin     ");         ///��ʼ�������ֶ���word
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

    wsym[0]=beginsym;                       ///��ʼ��������ֵ����wsym
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

    ssym['+']=plus;                         ///��ʼ������ֵ����ssym
    ssym['-']=minus;
    ssym['*']=times;
    ssym['/']=slash;
    ssym['(']=lparen;
    ssym[')']=rparen;
    ssym['=']=eql;
    ssym[',']=comma;
    ssym['.']=period;
    ssym[';']=semicolon;

    strcpy(mnemonic[lit],"LIT");            ///��ʼ������������
    strcpy(mnemonic[opr],"OPR");
    strcpy(mnemonic[lod],"LOD");
    strcpy(mnemonic[sto],"STO");
    strcpy(mnemonic[cal],"CAL");
    strcpy(mnemonic[Int],"INT");
    strcpy(mnemonic[jmp],"JMP");
    strcpy(mnemonic[jpc],"JPC");
    strcpy(mnemonic[red],"RED");
    strcpy(mnemonic[wrt],"WRT");

    declbegsys=constsym|varsym|procsym;                       ///��ʼ���������ֺϷ�ͷ���ż�
    statbegsys=beginsym|callsym|ifsym|whilesym
                   |repeatsym|readsym|writesym;               ///��ʼ����䲿�ֺϷ�ͷ���ż�
    facbegsys=ident|number|lparen;                            ///��ʼ�����Ӳ��ֺϷ�ͷ���ż�

    printf("Please input source program file name: ");
    scanf("%s",infilename);
    printf("\n");
    if((infile=fopen(infilename,"r"))==NULL)                  ///��Դ�����ļ���������ܴ򿪸�����ʾ���жϳ���
    {
        printf("File %s can't be opened.\n", infilename);
        exit(1);
    }

    err=0;                                      ///��ʼ������
    cc=0;
    cx=0;
    ll=0;
    ch=' ';
    kk=al;
    getsym();                                   ///Ԥ������
    lev=0;
    tx=0;
    block(declbegsys|statbegsys|period);        ///���÷ֳ������������block
    if(sym!=period)                             ///�������� . ������
    {
        error(9);
    }

    printf("\nList P-code in order:\n");
    listcode(0);                                ///�г�P-codeָ��

    if(err==0)                                  ///�������û�д��󣬽���ִ��
    {
        interpret();
    }
    else                                        ///��������д��������ʾ��������ִ��
    {
        printf("errors in PL/0 program\n");
    }
    fclose(infile);                             ///�ر��ļ�
}
