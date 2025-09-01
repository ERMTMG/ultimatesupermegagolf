#include<vector>
#include<utility.h>
#include<algorithm>
#include<stdexcept>

/*
    Map-like data structure that preserves the order of insertion of pairs. Wrapper over std::vector<std::pair<Key, T>>.
*/
namespace util{
    template<class Key, class T, class IgnoredLess = std::less<Key>,
             class Allocator = std::allocator<std::pair<const Key, T>>>
    struct ordered_map : std::vector<std::pair<const Key, T>, Allocator> {
        using key_type = const Key;
        using iterator_type = typename std::vector<std::pair<const Key, T>, Allocator>::iterator;
        using const_iterator_type = typename std::vector<std::pair<const Key, T>, Allocator>::const_iterator;

        template<class... Args>
        inline void emplace(Args&&... args){
            this->emplace_back(std::forward<Args>(args)...);
        }

        inline iterator_type find(const key_type& key){
            return std::find_if(this->begin(), this->end(), [&key](const std::pair<const Key, T>& p) -> bool {
                return p.first == key;
            });
        }

        inline const_iterator_type find(const key_type& key) const {
            return std::find_if(this->cbegin(), this->cend(), [&key](const std::pair<const Key, T>& p) -> bool {
                return p.first == key;
            });
        }

        inline T& at(const key_type& key){
            iterator_type iter = this->find(key);
            if(iter == this->end()){
                throw new std::out_of_range("key not found");
            }
            return iter->second;
        }

        inline const T& at(const key_type& key) const {
            const_iterator_type iter = this->find(key);
            if(iter == this->end()){
                throw new std::out_of_range("key not found");
            }
            return iter->second;
        }

        inline T& operator[](const key_type& key){
            iterator_type iter = this->find(key);
            if(iter == this->end()){
                std::pair<const Key, T>& new_pair = this->emplace_back(key, T{});
                return new_pair.second;
            } else {
                return iter->second;
            }
        }
    };
}
