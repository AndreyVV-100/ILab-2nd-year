#include <list>
#include <unordered_map>
#include <iterator>
#include <iostream>
#include <string>

namespace ARC
{

const size_t MIN_CACHE_SIZE = 10;

//! https://dbs.uni-leipzig.de/file/ARC.pdf
template <typename PageT, typename KeyT = int>
class Cache 
{
    using PairT = typename std::pair <KeyT, PageT>;
    using IterT = typename std::list <PairT> :: iterator;

    enum class ListType
    {
        T1 = 0,
        T2 = 1,
        B1 = 2,
        B2 = 3
    };

    struct MapT
    {
        IterT iter_;
        ListType list_type_;
    };

    using MapIt = typename std::unordered_map <KeyT, MapT> :: iterator;
    using ListT = typename std::list <PairT>;

    ListT T1_, T2_, B1_, B2_;
    std::unordered_map <KeyT, MapT> hash_table_;
    size_t c_, p_;
    
    void InsertPage (const KeyT&  page_id);
    void  AdaptPage (MapIt& elem);
    void DeletePage (const MapIt& elem);
    PairT GetPageFromMemory (const KeyT& page_id) const; // bad function, crutch

    void MoveToOtherList (ListT& list_from, MapIt& elem, 
                          ListT& list_to, const ListType new_place); 
    void MoveFromEnd (ListT& list_from,
                      ListT& list_to, const ListType new_place);

    void Replace_P (const MapIt& elem);

    // print functions

    void PrintList (const ListT& list, const std::string& list_name) const;
    // void PrintUnorderedMap() const; --- is it necessary?

    public:

    Cache (size_t size);
    bool Request (const KeyT& page_id);
    void PrintInfo() const;
    ~Cache (); 
};

template <typename PageT, typename KeyT>
Cache <PageT, KeyT> :: Cache (size_t size):
    c_ ((size < MIN_CACHE_SIZE) ? MIN_CACHE_SIZE / 2 : (size + 1) / 2),
    p_ (0)
{

}

template <typename PageT, typename KeyT>
Cache <PageT, KeyT> :: ~Cache()
{
    // Nothing here
}

template <typename PageT, typename KeyT>
bool Cache <PageT, KeyT> :: Request (const KeyT& page_id)
{
    MapIt elem = hash_table_.find (page_id);

    if (elem == hash_table_.end())
    {
        InsertPage (page_id);
        return false;
    }

    AdaptPage (elem);
    return true;
}

template <typename PageT, typename KeyT>
void Cache <PageT, KeyT> :: InsertPage (const KeyT& page_id)
{
    size_t T1_size = T1_.size();
    size_t L1_size = T1_size + B1_.size();
    size_t L2_size = T2_.size() + B2_.size();

    if (L1_size == c_)
    {
        if (T1_size < c_)
        {
            DeletePage (hash_table_.find (std::prev (B1_.end())->first)); // key of end B1
            Replace_P (hash_table_.end());
        }
        else
            DeletePage (hash_table_.find (std::prev (T1_.end())->first)); // key of end T1
    }

    else if (L1_size < c_ && L1_size + L2_size >= c_)
    {
        if (L1_size + L2_size == 2 * c_)
            DeletePage (hash_table_.find (std::prev (B2_.end())->first)); // key of end B2
            
        Replace_P (hash_table_.end());
    }

    T1_.push_front (GetPageFromMemory (page_id));
    hash_table_.insert ({page_id, {T1_.begin(), ListType :: T1}});
    return;
}

template <typename PageT, typename KeyT>
void Cache <PageT, KeyT> :: AdaptPage (MapIt& elem)
{
    switch (elem->second.list_type_)
    {
        case ListType :: T1:
            MoveToOtherList (T1_, elem, T2_, ListType :: T2);
            return;

        case ListType :: T2:
            if (elem->second.iter_ == T2_.begin())
                return;

            MoveToOtherList (T2_, elem, T2_, ListType :: T2);
            return;

        case ListType :: B1:
            p_ = std::min <size_t> (c_, p_ + std::max <size_t> (B2_.size() / B1_.size(), 1lu));
            Replace_P (elem);
            MoveToOtherList (B1_, elem, T2_, ListType :: T2);
            return;

        case ListType :: B2:
            p_ = (size_t) std::max <ssize_t> (0l, (ssize_t) p_ - std::max <size_t> (B1_.size() / B2_.size(), 1lu));
            Replace_P (elem);
            MoveToOtherList (B2_, elem, T2_, ListType :: T2);
            return;    

        default:
            std::cout << "Warning: bad list type.\n";
            return;
    }
}

template <typename PageT, typename KeyT>
void Cache <PageT, KeyT> :: DeletePage (const MapIt& elem)
{
    switch (elem->second.list_type_)
    {
        case ListType :: T1:
            T1_.erase (elem->second.iter_);
            break;

        case ListType :: T2:
            T2_.erase (elem->second.iter_);
            break;

        case ListType :: B1:
            B1_.erase (elem->second.iter_);
            break;

        case ListType :: B2:
            B2_.erase (elem->second.iter_);
            break;

        default:
            std::cout << "Warning: bad list type.\n";
            break;
    }

    hash_table_.erase (elem);
    return;
}

template <typename PageT, typename KeyT>
typename std::pair <KeyT, PageT> Cache <PageT, KeyT> :: GetPageFromMemory (const KeyT& page_id) const // ToDo: How to use PairT
{
    return {page_id, static_cast <PageT> (page_id)};
}

template <typename PageT, typename KeyT>
void Cache <PageT, KeyT> :: MoveToOtherList (ListT& list_from, MapIt& elem, 
                                             ListT& list_to, const ListType new_place)
{
    // std::cout << "Pushing: " << elem->first << std::endl;
    // PrintInfo ();
    list_to.push_front (*(elem->second.iter_));
    list_from.erase  (elem->second.iter_);
    elem->second = {list_to.begin(), new_place};
    return;
}

template <typename PageT, typename KeyT>
void Cache <PageT, KeyT> :: MoveFromEnd (ListT& list_from,
                                         ListT& list_to, const ListType new_place)
{
    MapIt elem = hash_table_.find (std::prev (list_from.end())->first);
    MoveToOtherList (list_from, elem, list_to, new_place);
    return;
}

template <typename PageT, typename KeyT>
void Cache <PageT, KeyT> :: Replace_P (const MapIt& elem)
{
    size_t T1_size = T1_.size();

    if (T1_size >= 1 && ((elem != hash_table_.end() && elem->second.list_type_ == ListType::B2 && T1_size == p_) || T1_size > p_))
        MoveFromEnd (T1_, B1_, ListType :: B1); // key of end T1
    
    else
        MoveFromEnd (T2_, B2_, ListType :: B2); // key of end T2
}

template <typename PageT, typename KeyT>
void Cache <PageT, KeyT> :: PrintInfo () const
{
    std::cout << "p_ = " << p_ << "\t""c_ = " << c_ << std::endl;

    PrintList (T1_, "T1");
    PrintList (T2_, "T2");
    PrintList (B1_, "B1");
    PrintList (B2_, "B2");

    std::cout << std::endl;
    return;
}

template <typename PageT, typename KeyT>
void Cache <PageT, KeyT> :: PrintList (const ListT& list, const std::string& list_name) const
{
    std::cout << list_name << " (size = " << list.size() << "):\n";
    for (auto i_elem = list.begin(); i_elem != list.end(); i_elem++)  \
        std::cout << "    ""Key = " << i_elem->first << "\t""Value = " << i_elem->second << std::endl;
}

} // end of namespace ARC
