#ifndef XRHASH_FAST_H
#define XRHASH_FAST_H

#include "xrhash.h"

typedef struct xrhash_fast_iterator
{
  XRHashIter * iter;
  void ** cache;
  size_t cache_len;
  size_t cache_index;  
  size_t cache_progress;
} xrhash_fast_iterator;


xrhash_fast_iterator * xr_init_fasthashiterator( XRHash * xr );

void xr_hash_resetfastiterator( xrhash_fast_iterator * fast );

void xr_hash_fastiterator_free( xrhash_fast_iterator * fast );

void * xr_hash_fastiteratekey( xrhash_fast_iterator * fast );

#endif
