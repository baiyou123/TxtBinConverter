#include "file_convert.h"
#include <QDebug>
#include<stdio.h>
#include<string.h>
#include<cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <QVariant>
#include <utils.h>

#define FILE_FD_ERR (1)
#define FILE_FMT_ERR (2)

enum CONVERT_TYPE{
    CONVERT_TXT2BIN = 1,
    CONVERT_BIN2TXT,
};

static int allow2hex(char c)
{
    int mode = 0;
    if(c >= '0' && c<= '9')
    {
        mode = 1;
    }
    else if(c >= 'A' && c<= 'F')
    {
        mode = 2;
    }
    else if(c >= 'a' && c<= 'f')
    {
        mode = 3;
    }
    return mode;
}

static char char2hex(char hc,char lc)
{
    int hex = -1;
    int mode;

    mode = allow2hex(hc);
    if(mode)
    {
        switch(mode)
        {
            case 1:
                hex = hc - '0';
                break;
            case 2:
                hex = hc - 'A' + 0xA;
                break;
            case 3:
                hex = hc - 'a' + 0xA;
                break;
            default:
                break;
        }
    }
    else
    {
        return -1;
    }

    hex = hex << 4;

    mode = allow2hex(lc);
    if(mode)
    {
        switch(mode)
        {
            case 1:
                hex |= lc - '0';
                break;
            case 2:
                hex |= lc - 'A' + 0xA;
                break;
            case 3:
                hex |= lc - 'a' + 0xA;
                break;
            default:
                break;
        }
    }
    else
    {
        return -1;
    }

    return hex;
}

int FileConvert::txt2bin(FILE* fpin,FILE* fpout, long size)
{
    char c,c1;
    char hex;
    long bytes_count = 0;

    while((c=fgetc(fpin)) != EOF && bytes_count++ <= size)
    {
        process = bytes_count*100 / size;

        if(c == ',' || c == ' ' || c == '\r' || c == '\n' || c == '{' || c == '}')
        {
            continue;
        }

        if(c == '0')        //跳过0x
        {
            if((c1 = fgetc(fpin)) != EOF)
            {
                bytes_count++;
                if(c1 == 'x' || c1 == 'X')
                {
                    continue;
                }
                else
                {
                    if(!allow2hex(c1))
                    {
                        return -FILE_FMT_ERR;
                    }
                    hex = char2hex(c,c1);
                    if(fputc(hex,fpout) == EOF)
                    {
                        return -FILE_FD_ERR;
                    }
                    continue;
                }

             }
             else
             {
                return -FILE_FMT_ERR;
             }
         }

        if(!allow2hex(c))   //过滤无效符号
        {
            printf("### c:%c\n",c);
            return -FILE_FMT_ERR;
        }
        else
        {
            if((c1 = fgetc(fpin)) != EOF)
            {
                bytes_count++;
                if(!allow2hex(c1))
                {
                    return -FILE_FMT_ERR;
                }
                hex = char2hex(c,c1);
                if(fputc(hex,fpout) == EOF)
                {
                    return -FILE_FD_ERR;
                }
            }
            else
            {
                return -FILE_FMT_ERR;
            }
         }
    }

    return 0;
}

int FileConvert::bin2txt(FILE* fpin,FILE* fpout, long size)
{
    int hex;
    char c,c1;
    long bytes_count = 0;
    unsigned char buffer[8];

    memset(buffer, 0x0, sizeof(buffer));
    buffer[0] = '0';
    buffer[1] = 'x';
    //buffer[2] = 0x0;
    //buffer[3] = 0x0;
    buffer[4] = ',';
    buffer[5] = ' ';

    buffer[6] = 0x0d;
    buffer[7] = 0x0a;

    //注意这里的hex为int类型，至于原因参考（fgetc与EOF的错综复杂关系http://emb.hqyj.com/Column/Column576.htm）
    while((hex=fgetc(fpin)) != EOF && bytes_count++ <= size)
    {
        process = bytes_count*100 / size;

        c = hex & 0x0f;
        c1 = (hex >> 4) & 0x0f;

        if(c >= 0x0 && c <= 0x9)    // 0x0 ~ 0x9
        {
            c = '0' + c;
        } else {    //0xa ~ 0xf
            c = c - 0xa + 'a';
        }

        if(c1 >= 0x0 && c1 <= 0x9)    // 0x0 ~ 0x9
        {
            c1 = '0' + c1;
        } else {    //0xa ~ 0xf
            c1 = c1 - 0xa + 'a';
        }

        buffer[2] = c1;
        buffer[3] = c;

        if (fwrite(buffer, 1, 6, fpout) != 6) {
            return -FILE_FD_ERR;
        }

        if((bytes_count % 16) == 0) {
            if (fwrite(buffer+6, 1, 2, fpout) != 2) {
                return -FILE_FD_ERR;
            }
        }
    }

    return 0;
}


FileConvert::FileConvert(QObject *parent) : QObject(parent)
{
    fileName = "";
    process = 0;
}

void FileConvert::doConvert(int type)
{
    char input[4096];
    char output[4096];
    char tmp[4096];
    std::string str;
    FILE* fp;
    FILE* outfp;
    //struct stat statbuf;
    long size;
    int ret;

    memset(input, 0, sizeof(input));
    memset(output, 0, sizeof(output));
    memset(tmp, 0, sizeof(tmp));

    str = fileName.toStdString();
    strcpy(tmp, str.c_str());
    if(strlen(tmp) == 0) {
        qDebug()<<"fileName is Empty";
        return;
    }
    memmove(input, tmp + strlen("file:///"), strlen(tmp) - strlen("file:///"));
    qDebug()<<"fileName:"<<fileName<<", input:"<<input;

    fp = fopen(input,"rb+");
    if(fp == NULL)
    {
        qDebug()<<"open failed "<<input;
        return;
    }

    //stat(input, &statbuf);
    //size = statbuf.st_size;
    fseek(fp,0L,SEEK_END);
    size=ftell(fp);
    fseek(fp,0L,SEEK_SET);

    qDebug()<<"file size "<<size;

    sprintf(output, "%s%s", input, (type == CONVERT_TXT2BIN) ? ".bin" : ".txt");
    outfp = fopen(output,"wb+");
    if(outfp == NULL)
    {
        qDebug()<<"open failed "<<output;
        fclose(fp);
        return;
    }

    process = 0;

    if(type == CONVERT_TXT2BIN) {
        ret = txt2bin(fp, outfp, size);
    }
    else {//if(type == CONVERT_BIN2TXT) {
        ret = bin2txt(fp, outfp, size);
    }

    if(ret == -FILE_FD_ERR) {
        QVariant msg = "文件读写失败";
        showMessage(msg);
    }
    else if(ret == -FILE_FMT_ERR) {
        QVariant msg = "文件格式错误";
        showMessage(msg);
    }
    else {
        QVariant msg = "完成";
        showMessage(msg);
    }

    fclose(fp);
    fclose(outfp);
    return;
}
