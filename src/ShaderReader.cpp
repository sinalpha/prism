#include "ShaderReader.h"

ShaderReader::ShaderReader(const char* fileName) : succeeded{ false }
{
    std::ifstream ifs(fileName, std::ios::binary);
    if (ifs.fail()) {
        return;
    }
    succeeded = true;
    std::istreambuf_iterator<char> first(ifs);
    std::istreambuf_iterator<char> last;
    buffer.assign(first, last);
    ifs.close();
}

bool ShaderReader::getSucceeded() const
{
    return succeeded;
}

unsigned char* ShaderReader::code() const
{
    char* p = const_cast<char*>(buffer.data());
    return reinterpret_cast<unsigned char*>(p);
}

size_t ShaderReader::size() const
{
    return buffer.size();
}