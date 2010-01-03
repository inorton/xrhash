#include <stdio.h>
#include "xrhash.h"

int main( int argc, char** argv )
{
  XRHash * xr = xr_init_hash( NULL, NULL );

  int x = 0;
  while ( x++ < 1000000 ){
    char * newstr = (char*) malloc( 10 * sizeof(char) );
    xr_hash_add( xr, newstr );

  }


  return 0;
}
