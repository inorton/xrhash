#ifndef XRHASH_H
#define XRHASH_H

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define XRHASH_SLOTS 8192
#define XRHASH_MOD   (XRHASH_SLOTS - 91)

#define XRHASH_EXISTS_TRUE    0
#define XRHASH_EXISTS_FALSE   1

#define XRHASH_ADDED_ALREADY 1
#define XRHASH_ADDED         0
#define XRHASH_ADD_FAILED   -1
#define XRHASH_HASH_INVALID -2

/* should never return 0, should return -ve on error */
typedef int (*hashfn)(void*); 
typedef int (*cmpfn)(void*,void*);

typedef struct link XRHashLink;
struct link
{
  void       * data;
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
int      xr_hash_add( XRHash * xr, void * data );

/* returns XRHASH_EXISTS_TRUE or XRHASH_EXISTS_FALSE */
int      xr_hash_contains( XRHash * xr, void * data );


#endif
