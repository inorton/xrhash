#ifndef XRHASH_H
#define XRHASH_H

#include <string.h>
#include <stdlib.h>
#include <errno.h>


#ifndef XRHASH_SLOTS
#define XRHASH_SLOTS 8192
#endif
#define XRHASH_MOD   (XRHASH_SLOTS - 91)

#define XRHASH_HASH_INVALID   -2 /* hashtable not initialized */
#define XRHASH_NULL_KEY      -3 /* tried to insert a null key */
#define XRHASH_HASHCODE_ERROR -4 /* hashfn returned <= 0 */

#define XRHASH_EXISTS_TRUE    0
#define XRHASH_EXISTS_FALSE   1

#define XRHASH_ADDED_ALREADY  1
#define XRHASH_ADDED          0
#define XRHASH_ADD_FAILED    -1

#define XRHASH_REMOVED        0
#define XRHASH_REMOVE_FAILED -1

/* should never return 0, should return -ve on error */
typedef int (*hashfn)(void*); 
typedef int (*cmpfn)(void*,void*);

typedef struct link XRHashLink;
struct link
{
  void       * key;
  void       * value;
  int          hashcode;
  XRHashLink * next;
};

typedef struct xrhash
{
  hashfn hash;
  cmpfn cmp;
  size_t count; 

  XRHashLink * buckets[XRHASH_SLOTS];
} XRHash;


/* create a new empty hash, return NULL on error */
XRHash * xr_init_hash( int (*hash)(void*) , int(*cmp)(void*,void*) );

/* return XRHASH_ADDED on success, else XRHASH_ADD_FAILED */
int      xr_hash_add( XRHash * xr, void * key, void * value );

/* returns XRHASH_EXISTS_TRUE or XRHASH_EXISTS_FALSE */
int      xr_hash_contains( XRHash * xr, void * key );

/* returns XRHASH_EXISTS_TRUE or XRHASH_EXISTS_FALSE */
int      xr_hash_get( XRHash * xr, void * key, void **dataout );

/* returns XRHASH_REMOVED or XRHASH_REMOVE_FAILED */
int      xr_hash_remove( XRHash * xr, void * key );

#endif
