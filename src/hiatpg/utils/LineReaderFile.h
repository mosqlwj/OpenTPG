//
// Created by luolijun on 2020/7/16.
//

#ifndef OPENTPG_LINEREADERFILE_H
#define OPENTPG_LINEREADERFILE_H


#include "HIATPG.h"

#include "line_reader.h"

#include <cstdio>



class LineReaderFile : public Reader
{
public:
    LineReaderFile();

    virtual ~LineReaderFile() override;

    int64_t Open(const std::string& filename);

public:
    virtual int ReadLine(const uint8_t*& data, int64_t& len) override;

private:
    line_reader_t lineReader;

    FILE* file;
};



#endif //OPENTPG_LINEREADERFILE_H
