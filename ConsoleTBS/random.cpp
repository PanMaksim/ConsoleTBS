
#include <ctime>
#include <cstdlib>
#include <mutex>

#include "random.h"

int get_random_number(int min, size_t max_)
{
    static std::once_flag flag;
    std::call_once(flag, []() { srand(static_cast<unsigned int>(time(0))); });

    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    return static_cast<int>(rand() * fraction * (max_ - min + 1) + min);
}

int get_random_number(int min, int max_)
{
    static std::once_flag flag;
    std::call_once(flag, []() { srand(static_cast<unsigned int>(time(0))); });

    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    return static_cast<int>(rand() * fraction * (max_ - min + 1) + min);
}
