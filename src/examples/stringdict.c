#include <xrhash.h>
#include <xrhash_fast.h>

#include <stdio.h>
#include <string.h>


void free_dict( XRHash * dict );

int main( int argc, char** argv ){
  XRHash * dict = xr_init_hash( xr_hash__strhash, xr_hash__strcmp );

  /* add things to the hash */

  if ( xr_hash_contains( dict, "mary" ) != XRHASH_EXISTS_TRUE )
    printf("does not contain mary\n");

  xr_hash_add( dict, "fred", strdup("my name is fred") );
  xr_hash_add( dict, "mary", strdup("my name is mary") );
  xr_hash_add( dict, "sally", strdup("my name is sally") );
  xr_hash_add( dict, "bob", strdup("my name is bob") );

  if ( xr_hash_contains( dict, "mary" ) == XRHASH_EXISTS_TRUE )
    printf("does contain mary\n");


  /* iterate the hash */
  xrhash_fast_iterator * iter = xr_init_fasthashiterator( dict );

  char * hashkey = NULL;
  do {
    hashkey = (char*) xr_hash_fastiteratekey( iter );
    if ( hashkey != NULL ){
      char * data;
      xr_hash_get( dict, (void*)hashkey, (void**)&data );
      printf("key = %s, value = %s\n", hashkey, data );
    } else {
      break;
    }
  } while ( 1 );
  xr_hash_fastiterator_free( iter );

  free_dict( dict );

  return 0;
}

/* free the strdup'd strings we added at each item value */
void free_dict( XRHash * dict ) {  
  XRHashIter * iter = xr_init_hashiterator( dict );
  void * k = NULL;
  void * data = NULL;
  while ( ( k = xr_hash_iteratekey( iter ) ) != NULL ) {
    if ( xr_hash_get( dict, k, (void**)&data) == XRHASH_EXISTS_TRUE ){
      printf("free %s\n", (char*)k);
      free ( data );
    }
  }
  free(iter);
  xr_hash_free( dict );
}


