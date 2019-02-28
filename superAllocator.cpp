#include <vector>
#include <cstddef>
#include <iostream>
#include <stdexcept>

char data[1024 * 1024];
bool mapping[1024 * 1024];
std::size_t blockSize = 512;

template <typename T>
class SuperAllocator
{

  public:
    using value_type = T;
    value_type *allocate(std::size_t n)
    {
        size_t size = n * sizeof(T);
        if (size % blockSize != 0)
        {
            size = size / blockSize * blockSize + blockSize;
        }
        size_t index = findEmptyBlock(size / blockSize);
        for (auto i = index; i < (index + size); ++i)
        {
            mapping[i] = true;
        }
        return reinterpret_cast<T *>(&data[index]);
    }

    void deallocate(value_type *pointer, std::size_t n)
    {
        size_t index = reinterpret_cast<char *>(pointer) - data;
        for (auto i = index; i < index + n; ++i)
        {
            mapping[i] = false;
        }
    }

    std::size_t findEmptyBlock(std::size_t blockCount)
    {
        int counter = 0;
        for (std::size_t i = 0; i < sizeof(mapping); i += blockSize)
        {
            if (!mapping[i])
            {
                counter++;
                if (counter == blockCount)
                    return i - (blockCount - 1) * blockSize;
            }
        }
        throw std::length_error("not enough memory");
    }
};

int main(int argc, char *argv[])
{
    try
    {
        std::vector<int, SuperAllocator<int>> v;
        v.reserve(512);
        v.push_back(5);
        std::cout << (int)data[0] << (int)data[1] << (int)data[2] << (int)data[3] << std::endl;
        v.push_back(7);
        std::cout << (int)data[4] << (int)data[5] << (int)data[6] << (int)data[7] << std::endl;
        v.push_back(256);
        std::cout << (int)data[8] << (int)data[9] << (int)data[10] << (int)data[11] << std::endl;
        std::cout << v[2] << std::endl;
    }
    catch (std::length_error e)
    {
        std::cout << e.what();
    }
}
