#include "ARC.hpp"
#include <iostream>

int main()
{
    size_t cache_size = 0;
    std::cin >> cache_size;
    ARC::Cache <int> cache {cache_size};

    size_t req_num = 0, hits = 0;
    std::cin >> req_num;
    // cache.PrintInfo();

    for (size_t i_req = 0; i_req < req_num; i_req++)
    {
        int page = 0;
        std::cin >> page;
        hits += cache.Request (page);
        // cache.PrintInfo();
    }

    std::cout << hits << std::endl;
    // cache.PrintInfo();
    return 0;
}
