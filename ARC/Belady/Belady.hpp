#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cassert>

namespace Belady
{

const size_t MIN_CACHE_SIZE = 2;

template <typename KeyT>
class Cache
{
    size_t size_;

    struct page_info
    {
        using vec_type = std::vector <size_t>;
        vec_type page_req_; // warranty: sorted
        typename vec_type::iterator next_req_; 

        size_t GetNextRequest (size_t req_now)
        {
            typename vec_type::iterator vec_end = page_req_.end();

            while (next_req_ != vec_end && *next_req_ < req_now)
                ++next_req_;

            return (next_req_ != vec_end) ? *next_req_ : SIZE_MAX;
        }
    };

    std::unordered_set <KeyT> cache_content_;
    std::unordered_map <KeyT, page_info> map_pages_;

    using CacheIt = typename std::unordered_set <KeyT> :: iterator;
    using MapIt = typename std::unordered_map <KeyT, page_info> :: iterator;

    void DeleteFU (size_t next_req); // the Farthest Used

    public:

    std::vector <KeyT> all_requests_;

    Cache (size_t size):
        size_ ((size > MIN_CACHE_SIZE) ? size : MIN_CACHE_SIZE)
    {}

    void AddRequest (const KeyT& key, size_t pos_now);
    void Validate();
    bool Request (const KeyT& key, size_t next_req);
};

template <typename KeyT>
void Cache <KeyT> :: AddRequest (const KeyT& key, size_t pos_now)
{
    all_requests_.push_back (key);

    MapIt elem = map_pages_.find (key);
    if (elem == map_pages_.end())
        elem = map_pages_.insert ({key, {}}).first;

    elem->second.page_req_.push_back (pos_now);
    return;
}

template <typename KeyT>
void Cache <KeyT> :: Validate()
{
    for (MapIt i_key = map_pages_.begin(), end_key = map_pages_.end(); i_key != end_key; ++i_key)
        i_key->second.next_req_ = i_key->second.page_req_.begin();
}

template <typename KeyT>
bool Cache <KeyT> :: Request (const KeyT& key, size_t req_now)
{
    CacheIt elem = cache_content_.find (key);
    
    if (elem != cache_content_.end())
        return true;

    if (cache_content_.size() == size_)
        DeleteFU (req_now);

    cache_content_.insert (key);
    return false;
}

template <typename KeyT>
void Cache <KeyT> :: DeleteFU (size_t req_now)
{
    CacheIt elem_max{};
    size_t max_next_req = 0;

    for (CacheIt i_elem = cache_content_.begin(), iter_end = cache_content_.end(); i_elem != iter_end; ++i_elem)
    {
        MapIt elem_in_map = map_pages_.find (*i_elem);
        /* #ifdef DMAKE_BUILD_TYPE=Debug
            assert (elem_in_map);
        #endif ToDo: how to fix warning: extra tokens at end of #ifdef directive ?*/

        size_t elem_next_req = elem_in_map->second.GetNextRequest (req_now);

        if (elem_next_req > max_next_req)
        {
            max_next_req = elem_next_req;
            elem_max = i_elem;

            if (elem_next_req == SIZE_MAX)
                break;
        }
    }
    cache_content_.erase (elem_max);
    return;
}

} // end of namespace Belady
