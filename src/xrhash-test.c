#include <stdio.h>
#include "xrhash.h"
#include <assert.h>

#define TEST_STR "foo%d"

int main( int argc, char** argv )
{
  XRHash * xr = xr_init_hash( NULL, NULL );

  int x = 0;
  int contains = -1;
  int datacount = 500000;

  int datalen = 10 + strlen(TEST_STR); /* ten digits */

  char* data_vec = (char*) malloc ( datacount * datalen * sizeof(char) );
  char * newstr = data_vec;
  fprintf(stderr,"test insertion and contains\n");
  while ( x++ < datacount ){
    snprintf(newstr,datalen,TEST_STR,x);
    xr_hash_add( xr, newstr, (void*) x ); /* store value of x as a pointer in $xr{$newstr} */
    contains = xr_hash_contains( xr, newstr );
    assert( contains == XRHASH_EXISTS_TRUE );
    newstr += 4;
  }

  fprintf(stderr,"test get\n");
  newstr = data_vec;
  x = 0;
  while ( x++ < datacount ){
    int got = -1;
    contains = xr_hash_get( xr, newstr, (void**) &got );
    assert( contains == XRHASH_EXISTS_TRUE );
    assert( got == x );
    
    newstr += 4;
  }

  fprintf(stderr,"test remove\n");
  newstr = data_vec;
  x = 0;
  while ( x++ < datacount ){
    contains = xr_hash_remove( xr, newstr );
    assert( contains == XRHASH_REMOVED );
    newstr += 4;
  }

  free( xr );
  free(data_vec);

  return 0;
}
