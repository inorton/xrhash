#include "spacemap.h"
#include <string.h>

int spaceship_id_cmp( void * id_a, void * id_b )
{
  return strcmp((char*)id_a,(char*)id_b);
}

/* id is a pointer to spaceship.id */
int spaceship_id_hash( void * id )
{
  /* this hashes strings in a similar way to the mono String.cs class */
  char* str = (char*) id;
  size_t len = strlen(str);
  int hash = 0;
  int c = 0;
  while ( c < len ){
    hash = (hash << 5) - hash + str[c];
    c++;
  }

  return hash;
}

spaceship * new_spaceship( char* name, uint8 type, int x, int y, double head )
{
  spaceship * ship = (spaceship*)malloc(1 * sizeof(spaceship));
  ship->name = strdup( name );
  ship->x = x;
  ship->y = y;
  ship->heading = head;
  return ship;
}

void spacemap_add( spacemap * map, spaceship * ship )
{
  xr_hash_add( map->xr, (void*)ship->name, ship );
}

spaceship * spacemap_get( spacemap * map, const char* name )
{
  spaceship * ret = NULL;
  xr_hash_get( map->xr, (void*) name, (void**) &ret );
  return ret;   
}

