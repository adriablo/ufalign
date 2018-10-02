#pragma once

// https://seqan.readthedocs.io/en/master/
// https://github.com/seqan/seqan
#include <seqan/index.h>
#include <seqan/seq_io.h>

using namespace seqan;

// 'extern template'  is supposed to speed up compilation w/ templates:
// extern template class FMIndexConfig<void, uint32_t>;
// extern template class Index<String<Dna5>, FMIndex<void, TConfig> >;

// https://seqan.readthedocs.io/en/master/Tutorial/DataStructures/Indices/FMIndex.html

// 'slow' config
// typedef FMIndexConfig<void, uint32_t>  TMyConfig;
// typedef FastFMIndexConfig<void, uint32_t>  TMyFastConfig;
// typedef Index<String<Dna5>, FMIndex<void, TMyConfig> > TMyFMIndex;

// fast! config
typedef FastFMIndexConfig<void, uint32_t, 2, 1> TMyFastConfig;
typedef Index<String<Dna5>, FMIndex<void, TMyFastConfig> > TMyFMIndex;

typedef Finder<TMyFMIndex> TMyFinder;

typedef Infix<String<Dna5> >::Type TMyInfix;

typedef String<char, CStyle> TCSString;
