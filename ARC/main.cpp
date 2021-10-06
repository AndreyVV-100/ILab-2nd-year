#include "ARC.hpp"
#include <cstdio>

int main()
{
    size_t cache_size = 0;
    scanf ("%zu", &cache_size);
    ARC::Cache <int> cache {2 * cache_size};

    size_t req_num = 0, hits = 0;
    scanf ("%zu", &req_num);
    // cache.PrintInfo();

    for (size_t i_req = 0; i_req < req_num; i_req++)
    {
        int page = 0;
        scanf ("%d", &page);
        hits += cache.Request (page);
        // cache.PrintInfo();
    }

    printf ("%zu\n", hits);
    // cache.PrintInfo();
    return 0;
}
