#include "xrhash.h"

int xr__hash_is_pointer( void * ptr )
{
  int ret = 0;
  memcpy(&ret,ptr,sizeof(int));
  return ret;
}

int xr__cmp_pointers( void * a, void * b )
{
  if ( a > b ){
    return -1;
  } else if ( a < b ){
    return 1;
  } 
  return 0;
}

static inline int xr__get_hashcode( XRHash * xr, void * key )
{
  int ret = 0;
  if ( key == NULL )
    return XRHASH_NULL_KEY;

  if ( xr == NULL )
    return XRHASH_HASH_INVALID;

  ret = (*xr->hash)( key ); 
  
  if ( ret <= 0 )
    return XRHASH_HASHCODE_ERROR;
  return ret;
}
int xr_get_hashcode( XRHash * xr, void * key )
{
  return xr__get_hashcode( xr, key );
}

static inline int xr__get_index( XRHash * xr, int hashcode )
{
  int index      = 0;
  int pindex     = 0;
  if ( hashcode <= 0 )
    return hashcode;

  if ( xr == NULL )
    return XRHASH_HASH_INVALID;

  index = hashcode;
  while ( index >= xr->maxslots ){
    pindex = index;
    index = index % ( xr->maxslots - xr->index_mod_magic );
    if ( index < 1 ){
      index = (index * -1);  
    }
    if ( index == 0 ){
      index = 1 + ( pindex - ( pindex / 3 ));
    }
  }
  return index;
}
int xr_get_index( XRHash * xr, int hashcode )
{
  return xr__get_index( xr, hashcode );
}


XRHash * xr_init_hash( hashfn hash , cmpfn cmp )
{
  return xr_init_hash_len( hash, cmp, XRHASH_SLOTS );
}


XRHash * xr_init_hash_len( hashfn hash , cmpfn cmp, size_t len )
{
  XRHash * table = NULL;

  if ( ( hash == NULL ) || ( cmp == NULL ) ){
    /* no hasher or comparitor supplied! just work on pointers */
    hash = &xr__hash_is_pointer;
    cmp  = &xr__cmp_pointers;
  }

  table = (XRHash*) malloc(1 * sizeof(XRHash));
  if ( table != NULL ){
    memset( table, 0, sizeof(XRHash));
    table->hash = hash;
    table->cmp  = cmp;
    table->count = 0;
    table->maxslots = len;
    table->hash_generation = 0;
    table->index_mod_magic = ( len > 512 ) ? 91 : 3 ;
    table->buckets = (XRHashLink**)calloc(len,sizeof(XRHashLink*));
  }
  return table;
}

void xr_hash_free( XRHash * xr )
{
  if ( xr == NULL ) return;
  XRHashLink * slot = xr->buckets[0];
  int bucket = 0;
  while ( slot != NULL ){
    XRHashLink * nextslot = slot->next;
    if ( nextslot == NULL ){
      if ( (++bucket) < ( xr->maxslots ) ){
        nextslot = xr->buckets[bucket];
      }
    } else {
      if ( slot != xr->buckets[bucket] ){
        slot->next = NULL;
        free( slot );
      }
    }
    slot = nextslot;
  }
  free(xr->buckets);
  free(xr);
}

int xr_hash_add( XRHash * xr, void * key, void * value )
{
  XRHashLink * slot = NULL;
  XRHashLink * prev = NULL; 

  int hashcode      = xr__get_hashcode( xr, key ); 
  int index         = xr__get_index(xr, hashcode);

  if ( index <= 0 ) return index; /* one of above failed */
 
  /* new node, first hit */
  if ( xr->buckets[index] == NULL ){
    xr->buckets[index] = (XRHashLink*)malloc(1 * sizeof(XRHashLink));
    xr->touched_indexes++;
    slot = xr->buckets[index];
    slot->key  = key;
    slot->value = value;
    slot->next = NULL; 
    xr->count++;
    xr->hash_generation++;
    return XRHASH_ADDED;
  } else {
    slot = xr->buckets[index];
    if ( slot == NULL ){
      errno = ENOMEM;
      return XRHASH_ADD_FAILED;
    }
  }

   /* collision, add a link */
  while ( slot != NULL ){
    if ( (*xr->cmp)(key,slot->key) == 0 ){
      /* same object, do nothing */
      return XRHASH_ADDED_ALREADY;
    } else {
      if ( slot->key == NULL ){
        break; /* use slot */
      }
      /* check next slot */
      prev = slot;
      slot = slot->next;
    }
  }
  /* if slot is null, create a new link */
  if ( slot == NULL ){
    prev->next = (XRHashLink*)malloc(1 * sizeof(XRHashLink));
    slot = prev->next;
    if ( slot == NULL ){
      errno = ENOMEM;
      return XRHASH_ADD_FAILED;
    } 
  }
  slot->key = key;
  slot->value = value;
  slot->next = NULL; 
  xr->count++;
  xr->hash_generation++;
  return XRHASH_ADDED;
}

int      xr_hash_contains( XRHash * xr, void * key )
{
  XRHashLink * slot = NULL;

  int hashcode = xr__get_hashcode( xr, key );
  int index = 0;
  if ( hashcode <= 0 ) return hashcode; /* error */
  index = xr__get_index( xr, hashcode );
  if ( index < 0 ) return index; /* error */

  slot = xr->buckets[index];

  if ( slot == NULL )
    return XRHASH_EXISTS_FALSE;

  while ( slot != NULL )
  { 
    int comp_res = (*xr->cmp)(key, slot->key);
    if ( comp_res == 0 ){
      return XRHASH_EXISTS_TRUE;
    }
    slot = slot->next;
  }
  return XRHASH_EXISTS_FALSE;
}

int      xr_hash_remove( XRHash * xr, void * key )
{
  XRHashLink * slot = NULL;
  XRHashLink * prev = NULL;
  int hashcode =  xr__get_hashcode(xr,key);
  int index    =  xr__get_index(xr, hashcode);

  if ( index <= 0 ) return index; /* one of above failed */
  if ( xr->buckets[index] == NULL )
    return XRHASH_REMOVED; /* not in hash */

  slot = xr->buckets[index];

  /* iterate slots until we find our match */ 
  while ( slot != NULL ){
    if ( (*xr->cmp)(key,slot->key) == 0 ) {
      /* found object - remove it */
      break;
    } else {
      prev = slot;
      slot = slot->next;
    }
  }

  if ( slot != NULL ){ /* remove this slot */
    if ( prev == NULL ){
      /* remove first link in this bucket */
      xr->buckets[index] = slot->next;
    } else {
      /* remove this link */
      prev->next = slot->next;
    }
    xr->count--;
    xr->hash_generation++;
    slot->key = NULL;
    slot->value = NULL;
    slot->next = NULL;
    free(slot);
  } 

  /* if slot == NULL, hashcode matched but the object was 
   * not in the hash */

  return XRHASH_REMOVED;
}

int      xr_hash_get( XRHash * xr, void * key, void **dataout )
{
  XRHashLink * slot = NULL;

  int hashcode = xr__get_hashcode( xr, key );
  int index = 0;
  if ( hashcode <= 0 ) return hashcode; /* error */
  index = xr__get_index( xr, hashcode );
  if ( index < 0 ) return index; /* error */

  slot = xr->buckets[index];

  if ( slot == NULL )
    return XRHASH_EXISTS_FALSE;

  while ( slot != NULL )
  { 
    int comp_res = (*xr->cmp)(key, slot->key);
    if ( comp_res == 0 ){
      *dataout = slot->value;
      return XRHASH_EXISTS_TRUE;
    }
    slot = slot->next;
  }
  return XRHASH_EXISTS_FALSE;
}


XRHashIter * xr_init_hashiterator( XRHash * xr )
{
  XRHashIter * iter = (XRHashIter*)malloc(1*sizeof(XRHashIter));
  if ( iter == NULL ){
    errno = ENOMEM;
  } else {
    iter->xr = xr;
    xr_hash_resetiterator( iter );
  }
  return iter;
}

void xr_hash_resetiterator( XRHashIter * iter )
{
  iter->hash_generation = iter->xr->hash_generation;
  iter->current_bucket = 0;
  iter->next_slot = iter->xr->buckets[0];

}

void * xr_hash_iteratekey( XRHashIter * iter )
{
  void * key = NULL;
  if ( iter->xr->hash_generation != iter->hash_generation ){
    fprintf(stderr,"hash changed during iteration\n");
    abort();
/*    return NULL; */
  }
  if ( iter->next_slot != NULL ){ /* iterate through links in the current bucket */
    key = iter->next_slot->key;
    iter->next_slot = iter->next_slot->next;
  } else { /* no more links here, move to next bucket */
    do { 
      if ( ++iter->current_bucket >= iter->xr->maxslots )
          return NULL; /* no more filled buckets, end of iterations */
    } while ( iter->xr->buckets[iter->current_bucket] == NULL );

    /* reached the end of the hash */
    if ( iter->current_bucket >= iter->xr->maxslots )
      return NULL; /* end of iterations */

    /* now pointing at the next slot */
    iter->next_slot = iter->xr->buckets[iter->current_bucket];
    key = iter->next_slot->key;
    iter->next_slot = iter->next_slot->next;
  }
  return key;
}


int   xr_hash__strhash( void * instr )
{
  /* this hashes strings in a similar way to the mono String.cs class */
  char* str = (char*) instr;
  size_t len = strlen(str);
  int hash = 0;
  int c = 0;
  if ( ( len % 4 ) == 0 ){
    while ( c < (len/4) ){
      hash = (hash << 5) - hash + ((int*)instr)[c++];
    }
  } else {
    while ( c < len ){
      hash = (hash << 5) - hash + str[c];
      c++;
    }
  }
  while ( hash < 1 )
    hash = 1 + ( hash * -1 );

  return hash;

}

int   xr_hash__strcmp( void * stra, void * strb )
{
  return strcmp((char*)stra,(char*)strb);
}

