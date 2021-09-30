#include "ARC.hpp"
#include <cstdio>

int main()
{
    ARC::Cache <int> cache {4};

    size_t req_num = 0, hits = 0;
    scanf ("%zu", &req_num);

    for (size_t i_req = 0; i_req < req_num; i_req++)
    {
        int page = 0;
        scanf ("%d", &page);
        hits += cache.Request (page);
    }

    printf ("%zu\n", hits);
    return 0;
}
