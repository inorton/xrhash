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



int xr_hash_add( XRHash * xr, void * data )
{
  int hashcode      = 0;
  int index         = 0;
  XRHashLink * slot = NULL;
  XRHashLink * prev = NULL;
  if ( data == NULL )
    return XRHASH_ADD_FAILED;

  if ( xr == NULL )
    return XRHASH_HASH_INVALID;

  hashcode = (*xr->hash)( data ); 
  index = hashcode;
  while ( index >= XRHASH_SLOTS )
    index = index % XRHASH_MOD; 

  /* new node, first hit */
  if ( xr->buckets[index] == NULL ){
    xr->buckets[index] = (XRHashLink*)malloc(1 * sizeof(XRHashLink));
    slot = xr->buckets[index];
    slot->hashcode = hashcode;
    slot->data = data;
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
    if ( (data == slot->data) || ( (*xr->cmp)(data,slot->data) == 0 ) ){
      /* same object, do nothing */
      return XRHASH_ADDED_ALREADY;
    } else {
      if ( slot->hashcode == 0 ){
        break;
      }
      if ( slot->data == NULL ){
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
  slot->data = data;
  slot->next = NULL; 
  xr->count++;
  return XRHASH_ADDED;
}

