#include "Belady.hpp"

int main()
{
    size_t size = 0, N = 0;
    std::cin >> size >> N;

    Belady::Cache <int> cache (size);

    for (size_t i_req = 0; i_req < N; i_req++)
    {
        int key = 0;
        std::cin >> key;
        
        cache.AddRequest (key, i_req);
    }
    cache.Validate();

    size_t num_hits = 0;
    for (size_t i_req = 0; i_req < N; i_req++)
        num_hits += cache.Request (cache.all_requests_[i_req], i_req); 

    std::cout << num_hits << std::endl;
    return 0;
}
