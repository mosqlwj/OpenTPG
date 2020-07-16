//
// Created by luolijun on 2020/7/16.
//

#ifndef OPENTPG_LINE_READER_H
#define OPENTPG_LINE_READER_H



//! line-reader 的缓冲区的最小值
#define LINE_READER_BUF_SIZE_MIN   (1024)

enum READER_STATUS
{
    READER_ERROR = -1,
    READER_OK = 0,
    READER_EOF = 0x01,
};

struct line_reader_t
{
    char* line;   //  当前行起始位置
    char* eol;    //  当前行结束位置
    long lino;   //  当前行的行号
    int fd;     //  当前文件的句柄
    int status; //  文件句柄读取状态
    char* buf;    //  数据缓冲区
    char* end;    //  缓冲区有效数据结束位置
    char* limit;  //  缓冲区结束位置
};

//! line_reader_init 初始化
//! line_reader_destroy 销毁
//! line_reader_next 读取下一行
//!
//! 本套接口的基本使用方法比较简单，可以用一句话概括：总是调用 line_reader_next，直到遇到返回值不是READER_OK为止；
//! 下面是按行扫描文件的一个基本实现
//    typedef int (*line_reader_scan_callback)(void* ctx, struct line_reader_t* r);
//    static int   line_reader_scan(const char* file, line_reader_scan_callback callback, void* ctx)
//    {
//        struct line_reader_t r;
//
//        // 初始化
//        int ret = line_reader_init(&r, file, 4096);
//        if (0 != ret) {
//            return -1;
//        }
//
//        // 逐行读取
//        while (0 == line_reader_next(&r)) {
//            int ret = callback(ctx, &r);
//            //  如果用户希望自行中断读取，那么就提前返回
//            if (0 != ret) {
//                break;
//            }
//        }
//
//        // 销毁
//        line_reader_destroy(&r);
//        return 0;
//    }
extern int line_reader_init(struct line_reader_t* r, int buf_init_size);

extern int line_reader_attach(struct line_reader_t* r, int fd);

extern void line_reader_detach(struct line_reader_t* r);

extern void line_reader_destroy(struct line_reader_t* r);

extern int line_reader_next(struct line_reader_t* r);


#endif //OPENTPG_LINE_READER_H
