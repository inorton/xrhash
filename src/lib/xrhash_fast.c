#include <xrhash.h>
#include <xrhash_fast.h>

xrhash_fast_iterator * xr_init_fasthashiterator( XRHash * xr )
{
  xrhash_fast_iterator * fast = NULL;
  fast = (xrhash_fast_iterator *)malloc(1*sizeof(xrhash_fast_iterator));
  if ( fast == NULL ) return fast;
  fast->iter = xr_init_hashiterator( xr );
  if ( fast->iter != NULL ){
    fast->cache_progress = 0;
    fast->cache_index = 0;
    fast->cache = (void**)calloc( sizeof(void**), fast->iter->xr->count );
    fast->cache_len = fast->iter->xr->count;
  }
  return fast;
}


void xr_hash_resetfastiterator( xrhash_fast_iterator * fast )
{
  fast->cache_index = 0;
  if ( fast->iter->hash_generation != fast->iter->xr->hash_generation ){
    /* hash has changed */
    xr_hash_resetiterator( fast->iter );
    if ( fast->cache_len != fast->iter->xr->count ){
      /* hash size has changed too */
      fast->cache_len = fast->iter->xr->count;
      fast->cache = (void**) realloc( fast->cache, fast->iter->xr->count * sizeof( void** ) );
      fast->cache_progress = 0;
    }  
  } else {
    /* hash is unchanged */
  }
}

void xr_hash_fastiterator_free( xrhash_fast_iterator * fast )
{
  if ( fast != NULL ){
    if ( fast->cache != NULL ){
      free(fast->cache);
      free(fast->iter);
      memset( fast, 0, sizeof(xrhash_fast_iterator) );
    }
    free(fast);
  }
}

void * xr_hash_fastiteratekey( xrhash_fast_iterator * fast )
{
  if ( fast->iter->hash_generation == fast->iter->xr->hash_generation ){
    if ( fast->cache_index < fast->cache_len ){
      if ( fast->cache_index < fast->cache_progress ){
        return fast->cache[fast->cache_index++];
      } else {
        /* iterate and copy key */
        void * key = xr_hash_iteratekey( fast->iter );
        fast->cache[fast->cache_index++] = key;
        fast->cache_progress++;
        return key; 
      }
    }
    return NULL; /* end of hash */
  } else { 
    fprintf(stderr,"hash changed during iteration\n");
    abort();
  }
}


