#pragma once

#include <map>


typedef std::map<unsigned int, bool>  THitCache;
typedef THitCache::iterator  THitCacheIter;


//
// TODO: add cache size limit
//

template<typename C, typename K> bool find_cache(C &cache, K &key) {
    return cache.count(key) > 0;
}

