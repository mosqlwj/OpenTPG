//
// Created by luolijun on 2020/7/16.
//

#include "LineReaderFile.h"


LineReaderFile::LineReaderFile()
{
    line_reader_init(&lineReader, 4096);
    file = nullptr;
}

LineReaderFile::~LineReaderFile()
{
    if (nullptr != file)
    {
        fclose(file);

        line_reader_detach(&lineReader);
    }


    line_reader_destroy(&lineReader);
}

int64_t LineReaderFile::Open(const std::string& filename)
{
    file = fopen(filename.c_str(), "r");
    if (nullptr == file)
    {
        return -1;
    }

    line_reader_attach(&lineReader, fileno(file));

    return 0;
}


int LineReaderFile::ReadLine(const uint8_t*& data, int64_t& len)
{
    int ret = line_reader_next(&lineReader);
    if (0 == ret) {
        return 0;
    }

    if (lineReader.status == READER_EOF) {
        return 1;
    }

    return -1;
}