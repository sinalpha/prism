#pragma once

#include<fstream>

class ShaderReader{
public:
    ShaderReader(const char* fileName);

    bool getSucceeded() const;
    unsigned char* code() const;
    size_t size() const;

private:

    std::string buffer;
    bool succeeded;


};