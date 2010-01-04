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

inline int xr__get_hashcode( XRHash * xr, void * key )
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

inline int xr__get_index( XRHash * xr, int hashcode )
{
  int index      = 0;
  if ( hashcode <= 0 )
    return hashcode;

  if ( xr == NULL )
    return XRHASH_HASH_INVALID;

  index = hashcode;
  while ( index >= XRHASH_SLOTS )
    index = index % XRHASH_MOD; 

  return index;
}

XRHash * xr_init_hash( hashfn hash , cmpfn cmp )
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
  }
  return table;
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
    slot = xr->buckets[index];
    slot->hashcode = hashcode;
    slot->key  = key;
    slot->value = value;
    slot->next = NULL; 
    xr->count++;
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
      if ( slot->hashcode == 0 ){
        break;
      }
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
  slot->hashcode = hashcode;
  slot->key = key;
  slot->value = value;
  slot->next = NULL; 
  xr->count++;
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

