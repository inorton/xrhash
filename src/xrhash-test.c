#include <stdio.h>
#include "xrhash.h"
#include <assert.h>

#define TEST_STR "foo%d"

int main( int argc, char** argv )
{
  XRHash * xr = xr_init_hash( NULL, NULL );

  int x = 0;
  int contains = -1;
  int datacount = 1000000;

  int datalen = 10 + strlen(TEST_STR); /* ten digits */

  char* data_vec = (char*) malloc ( datacount * datalen * sizeof(char) );
  char * newstr = data_vec;
  while ( x++ < datacount ){
    snprintf(newstr,datalen,TEST_STR,x);
    xr_hash_add( xr, newstr );
    contains = xr_hash_contains( xr, newstr );
    assert( contains == XRHASH_EXISTS_TRUE );

    xr_hash_remove( xr, newstr );

    newstr += 4;
  }

  free( xr );

  return 0;
}
