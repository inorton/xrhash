#ifndef XRHASH_H
#define XRHASH_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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


/** 
* @brief function to generate a hash code for the object at the given pointer. should return a positive int greater than zero.
*/
typedef int (*hashfn)(void*); 

/** 
* @brief function to compare to objects for equality
*/
typedef int (*cmpfn)(void*,void*);


typedef struct link XRHashLink;
struct link
{
  void       * key;
  void       * value;
  int          hashcode;
  XRHashLink * next;
};


/** 
* @brief xrhash hashtable object
*/
typedef struct xrhash
{
  int hash_generation; /* used to monitor changes in the hash  for iterators */
  hashfn hash;
  cmpfn cmp;
  size_t count;
  size_t maxslots; 
  XRHashLink ** buckets;
} XRHash;


/** 
* @brief xrhash iteration object
*/
typedef struct xrhash_iter
{
  XRHash * xr;
  int hash_generation;
  int current_bucket;
  XRHashLink * next_slot;
} XRHashIter;



/** 
* @brief initialize a xrhash hashtable object
* 
* @param hash callback function for hashcode generation
* @param cmp callback function for equality comparison (eg strcmp)
* 
* @return 
*/
XRHash * xr_init_hash( int (*hash)(void*) , int(*cmp)(void*,void*) );


/** 
* @brief initialize a xrhash hashtable object with a specific number of hash buckets
* 
* @param hash callback function for hashcode generation
* @param cmp callback function for equality comparison (eg strcmp)
* @param len number of buckets to use
* 
* @return 
*/
XRHash * xr_init_hash_len( int (*hash)(void*), int(*cmp)(void*,void*), size_t len );

/** 
* @brief add an object to the given xr hashtable
* 
* @param xr hashtable to add to
* @param key store value with this key
* @param value object to store
* 
* @return XRHASH_ADDED on success, XRHASH_ADDED_ALREADY if the key is already taken or XRHASH_NULL_KEY if key was NULL
*/
int      xr_hash_add( XRHash * xr, void * key, void * value );


/** 
* @brief test if a given key exists in the hashtable
* 
* @param xr hashtable to check
* @param key pointer to key 
* 
* @return XRHASH_EXISTS_TRUE or XRHASH_EXISTS_FALSE 
*/
int      xr_hash_contains( XRHash * xr, void * key );

/** 
* @brief get the value corresponding to key
* 
* @param xr hashtable to access
* @param key key to use
* @param dataout put value (pointer) here
* 
* @returns XRHASH_EXISTS_TRUE or XRHASH_EXISTS_FALSE */
int      xr_hash_get( XRHash * xr, void * key, void **dataout );


/** 
* @brief delete a given key+value from the hashtable
* 
* @param xr hashtable to access
* @param key delete the value with this key
* 
* @return 
*/
int      xr_hash_remove( XRHash * xr, void * key );



/** 
* @brief initialize an object to iterate forwards through keys int the hashtable
* 
* @param xr hashtable to use
* 
* @return initialized iterator object
*/
XRHashIter * xr_init_hashiterator( XRHash * xr );


/** 
* @brief iterate to the next item using a hash iterator
* 
* @param iter iterator to use
* 
* @return pointer to a key used in the iter->xr hashtable
*/
void * xr_hash_iteratekey( XRHashIter * iter );



/** 
* @brief generate a hashcode for a given null terminated string
* 
* @param str string to hash
* 
* @return hashcode > 1 on success, <= 0 on error
*/
int   xr_hash__strhash( void * str );


/** 
* @brief wrapper around strcmp
* 
* @param stra
* @param strb
* 
* @return 0,-1 or 1
*/
int   xr_hash__strcmp( void * stra, void * strb );


#endif
