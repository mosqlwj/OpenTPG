//
//  line_reader_t.m
//  BehaviorAnalysis
//
//  Created by baidu on 2020/2/16.
//  Copyright © 2020 baidu. All rights reserved.
//
#import "line_reader.h"
#include <stdio.h>

//  操作系统API
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

//  C标准库
#include <stdlib.h>
#include <string.h>
#include <assert.h>


//  定义一个 ASSERT 宏
#ifndef ASSERT
#define ASSERT assert
#endif//ASSERT


extern int line_reader_init(struct line_reader_t* r, int buf_init_size)
{
        ASSERT(NULL != r);
        ASSERT(buf_init_size >= LINE_READER_BUF_SIZE_MIN);

    //  先统一初始化成 0
    memset(r, 0, sizeof(struct line_reader_t));

    //  分配缓冲区
    char* buf = (char*) malloc((size_t) (buf_init_size));
    if (NULL == buf)
    {
        goto FAIL;
    }

    //  初始化所有缓冲区管理指针
    r->buf = buf;
    r->limit = r->buf + (buf_init_size - sizeof(void*));
    r->line = r->buf;
    r->eol = r->line - 1;   //  这个地方比较特殊，因为当识别到下一行的时候，line总是从eol+1的地方开始计算的
    r->end = r->buf;

    //  其他成员初始化
    r->limit[0] = '\n';
    r->end[0] = '\n';
    r->status = READER_OK;
    r->lino = 0;
    r->fd = -1;

    return 0;

    FAIL:
    line_reader_destroy(r);
    return -1;
}


//extern int  line_reader_open(struct line_reader_t* r, const char* filename)
//{
//    ASSERT(NULL != r);
//    ASSERT(NULL != filename);
//    return (line_reader_bind(r, open(filename, O_RDONLY)));
//}

extern int line_reader_attach(struct line_reader_t* r, int fd)
{
        ASSERT(NULL != r);
        ASSERT(r->fd == -1);
    r->fd = fd;
    if (r->fd == -1)
    {
        return -1;
    }

    //  重置所有动态变化的指针
    r->line = r->buf;
    //r->eol = r->line - 1;   //  这个地方比较特殊，因为当识别到下一行的时候，line总是从eol+1的地方开始计算的
    r->eol = r->line;   //  这个地方比较特殊，因为当识别到下一行的时候，line总是从eol+1的地方开始计算的
    r->end = r->buf;

    //  其他成员初始化
    r->limit[0] = '\n';
    r->end[0] = '\n';
    r->status = READER_OK;
    r->lino = 0;

    return 0;
}


extern void line_reader_detach(struct line_reader_t* r)
{
    if (NULL == r)
    {
        return;
    }

    //  如果文件没关闭就先关闭
    r->fd = -1;

    return;
}


extern void line_reader_destroy(struct line_reader_t* r)
{
    if (NULL == r)
    {
        return;
    }

    if (r->fd != -1)
    {
        close(r->fd);
        r->fd = -1;
    }

    if (NULL != r->buf)
    {
        free(r->buf);
        r->buf = NULL;
    }
}


//  填充操作只发生在 pc 扫描到了 end 这种情况下；
//  此时，缓冲区应该本身就是填满的；如果缓冲区未被填满，那么说明已经到文件结尾了；
//  缓冲区在填充的时候，如果缓冲区中还有有效数据，那么需要先搬移下数据
static char* line_reader_fill(struct line_reader_t* r, char* pc)
{
        ASSERT(NULL != r);
        ASSERT(pc >= r->line);
        ASSERT(pc <= r->end);

    //  计算下数据总长度
    long dataLen = r->end - r->line;

    //  如果缓冲区太小不足以存放当前字符串
    if (r->line == r->buf)
    {
        //  如果 r->end 与 r->limit 相等，才是正常的字符串太长超过当前缓冲区总长度
        if (r->end == r->limit)
        {
            //  创建一块更大的缓冲区，并搬移现有缓冲区中的数据
            long newSize = 2 * (r->limit - r->buf);
            char* newBuf = (char*) malloc((size_t) newSize);
            if (NULL == newBuf)
            {
                r->status = READER_ERROR;
                return pc;
            }
            memcpy(newBuf, r->line, (size_t) dataLen);

            //  修改所有关联指针的到新位置，释放酒缓冲区，保存新缓冲区
            long pcOffset = pc - r->line;
            long eolOffset = r->eol - r->line;
            free(r->buf);
            r->buf = newBuf;
            r->limit = newBuf + (newSize - sizeof(void*));
            r->line = newBuf;
            r->end = r->line + dataLen;
            r->eol = r->line + eolOffset;

            r->limit[0] = '\n';
            r->end[0] = '\n';

            //  计算并返回新pc的位置
            pc = r->line + pcOffset;
        }
    }

    //  如果遇到缓冲区结尾，才需要填充时，说明一行被缓冲区截断了，此时需要考虑挪移一下数据
    long moveOffset = r->line - r->buf; // 移动距离
    if (moveOffset > 0)
    {
        memmove(r->buf, r->line, (size_t) dataLen);
        r->line -= moveOffset;
        r->end -= moveOffset;
        r->eol -= moveOffset;
        r->end[0] = '\n';
        pc -= moveOffset;
    }

    //  尝试读取文件，使用文件中的数据填充腾挪后的空白空间
    long fillSize = read(r->fd, r->end, r->limit - r->end);

    //  如果遇到错误了
    if (fillSize < 0)
    {
        //  这里是遇到错误了
        r->status = READER_ERROR;
        return pc;
    }

    //  如果读到文件结尾了
    if (fillSize == 0)
    {
        //  end自动可以作为最后的\n补偿
        r->status = READER_EOF;
        return pc;
    }

    //  如果读取成功了
    r->end += fillSize;
    r->end[0] = '\n';
    return pc;
}


extern int line_reader_next(struct line_reader_t* r)
{
        ASSERT(NULL != r);

    //  如果上一次读取的时候就已经读取出错或者读完了，那么就不能继续读了
    if (r->status != READER_OK)
    {
        return -1;
    }

    //  从上一个eol开始，找下一个换行的位置
    char* pc = r->eol;

    READER_RETRY:
    if ('\n' != *pc)
    {
        while (*(++pc) != '\n')
        {
        }
    }

    //  检查一下是否到了缓冲区有效数据的尾部，如果到了有效数据的尾部，说明只是遇到了缓冲区不够的情况
    if (pc == r->end)
    {
        pc = line_reader_fill(r, pc);

        //  如果数据填充后，pc与end的相对位置没变，说明文件读取完毕，已经没有数据可以处理了，
        //  但是当前缓冲区中已经存在的数据还需要处理
        if (pc != r->end)
        {
            //  数据填充完毕了，继续找下一个换行的位置
            goto READER_RETRY;
        }
    }

    //  开始进入下一行，考虑到文件往往会有换行符号的问题
    if (r->eol[0] == '\0')
    {
        r->line = r->eol + 1;
    }
    while (r->line[0] == '\r')
    {
        r->line++;
    }
    r->lino++;
    r->eol = pc;
    r->eol[0] = '\0';

    return 0;
}

