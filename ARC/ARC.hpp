#include <list>
#include <unordered_map>
#include <iterator>
#include <iostream>

namespace ARC
{

const size_t MIN_SIZE = 1; // ToDo: replace it? Do it bigger?

//! https://dbs.uni-leipzig.de/file/ARC.pdf
template <typename PageT, typename KeyT = int>
class Cache 
{
    typedef typename std::pair <KeyT, PageT> PairT; // ToDo: It's OK?
    typedef typename std::list <PairT> :: iterator IterT;

    enum class ListType
    {
        T1 = 0,
        T2 = 1,
        B1 = 2,
        B2 = 3,
        NOT_IN_LISTS = 4
    };

    struct MapT
    {
        IterT iter_;
        ListType list_type_;
    };

    typedef typename std::unordered_map <KeyT, MapT> :: iterator MapIt;

    std::list <PairT> T1_, T2_, B1_, B2_;
    std::unordered_map <KeyT, MapT> hash_table_;
    size_t c_, p_;

    void Replace_P (const MapIt& elem);

    // ToDo: non-const references - is it correct?
    void MoveToOtherList (std::list<PairT>& list_from, MapIt& elem, 
                          std::list<PairT>& list_to, ListType new_place); 
    void MoveToOtherList (std::list <PairT>& list_from, MapIt&& elem, 
                          std::list <PairT>& list_to, ListType new_place);
    
    void DeletePage (const MapIt& elem);
    PairT GetPageFromMemory (const KeyT& page_id); // bad function, crutch

public:
    Cache (size_t size);
    bool Request (const KeyT& page_id);
    void PrintInfo();
    ~Cache (); 
};

template <typename PageT, typename KeyT>
Cache <PageT, KeyT> :: Cache (size_t size):
    c_ ((size < MIN_SIZE) ? MIN_SIZE : size), // ToDo: size of cache == c_ or 2 * c_ ?
    p_ (0)
{

}

template <typename PageT, typename KeyT>
Cache <PageT, KeyT> :: ~Cache()
{
    p_ = 0;
    c_ = 0;
}

template <typename PageT, typename KeyT>
bool Cache <PageT, KeyT> :: Request (const KeyT& page_id)
{
    MapIt elem = hash_table_.find (page_id);

    if (elem == hash_table_.end())
    {
        size_t T1_size = T1_.size();
        size_t L1_size = T1_size + B1_.size();
        size_t L2_size = T2_.size() + B2_.size();

        if (L1_size == c_)
        {
            if (T1_size < c_)
            {
                DeletePage (hash_table_.find (std::prev (B1_.end())->first)); // key of end B1
                Replace_P (elem);
            }
            else
                DeletePage (hash_table_.find (std::prev (T1_.end())->first)); // key of end T1
        }

        else if (L1_size < c_ && L1_size + L2_size >= c_)
        {
            if (L1_size + L2_size == 2 * c_)
                DeletePage (hash_table_.find (std::prev (B2_.end())->first)); // key of end B2
                
            Replace_P (elem);
        }

        T1_.push_front (GetPageFromMemory (page_id));
        hash_table_.insert ({page_id, {T1_.begin(), ListType :: T1}});
        return false;
    }

    switch (elem->second.list_type_)
    {
        case ListType :: T1:
            MoveToOtherList (T1_, elem, T2_, ListType :: T2);
            break;

        case ListType :: T2:
            if (elem->second.iter_ == T2_.begin())
                break;

            MoveToOtherList (T2_, elem, T2_, ListType :: T2);
            break;

        case ListType :: B1:
            p_ = std::min <size_t> (c_, p_ + std::max <size_t> (B2_.size() / B1_.size(), 1lu));
            Replace_P (elem);
            MoveToOtherList (B1_, elem, T2_, ListType :: T2);
            break;

        case ListType :: B2:
            p_ = (size_t) std::max <ssize_t> (0l, (ssize_t) p_ - std::max <size_t> (B1_.size() / B2_.size(), 1lu));
            Replace_P (elem);
            MoveToOtherList (B2_, elem, T2_, ListType :: T2);
            break;    

        case ListType :: NOT_IN_LISTS:
        default:
            break;
    }

    return true;
}

template <typename PageT, typename KeyT>
void Cache <PageT, KeyT> :: Replace_P (const MapIt& elem)
{
    size_t T1_size = T1_.size();

    if (T1_size >= 1 && ((elem != hash_table_.end() && elem->second.list_type_ == ListType::B2 && T1_size == p_) || T1_size > p_))
        MoveToOtherList (T1_, hash_table_.find (std::prev (T1_.end())->first), B1_, ListType :: B1); // key of end T1
    
    else
        MoveToOtherList (T2_, hash_table_.find (std::prev (T2_.end())->first), B2_, ListType :: B2); // key of end T2
}

template <typename PageT, typename KeyT>
typename std::pair <KeyT, PageT> Cache <PageT, KeyT> :: GetPageFromMemory (const KeyT& page_id) // ToDo: How to use PairT
{
    return {page_id, static_cast <PageT> (page_id)};
}

template <typename PageT, typename KeyT>
void Cache <PageT, KeyT> :: MoveToOtherList (std::list <PairT>& list_from, MapIt& elem, 
                                             std::list <PairT>& list_to, ListType new_place)
{
    // std::cout << "Pushing: " << elem->first << std::endl;
    // PrintInfo ();
    list_to.push_front (*(elem->second.iter_));
    list_from.erase  (elem->second.iter_);
    elem->second = {list_to.begin(), new_place};
    return;
}

template <typename PageT, typename KeyT>
void Cache <PageT, KeyT> :: MoveToOtherList (std::list <PairT>& list_from, MapIt&& elem, 
                                             std::list <PairT>& list_to, ListType new_place)
{
    MapIt lvalue_elem = elem;
    MoveToOtherList (list_from, lvalue_elem, list_to, new_place);
    return;
}

template <typename PageT, typename KeyT>
void Cache <PageT, KeyT> :: DeletePage (const MapIt& elem)
{
    switch (elem->second.list_type_)
    {
        // ToDo: copypaste, maybe define?
    
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

        case ListType :: NOT_IN_LISTS:
        default:
            break;
    }

    hash_table_.erase (elem);
    return;
}

template <typename PageT, typename KeyT>
void Cache <PageT, KeyT> :: PrintInfo ()
{
    #define print(list) std::cout << #list " (size = " << list##_.size() << "):\n";             \
                        for (auto i_elem = list##_.begin(); i_elem != list##_.end(); i_elem++)  \
                            std::cout << "    ""Key = " << i_elem->first << "\t""Value = " << i_elem->second << std::endl;

    std::cout << "p_ = " << p_ << "\t""c_ = " << c_ << std::endl;

    print (T1);
    print (T2);
    print (B1);
    print (B2);
    std::cout << std::endl;
    
    #undef print
    return;
}

} // end of namespace ARC
