#ifndef SPACEMAP_H
#define SPACEMAP_H

#include <xrhash.h>
#include <stdio.h>
#include <string.h>

/* silly example storing ships in a map space using 'ship names' as the hash keys */

typedef char uint8 ;

typedef struct spaceship {
  char * name;
  uint8  type;
  int x; /* corrdinate x100 */
  int y;

  int vx;
  int vy;

} spaceship;


typedef struct spacemap {
  XRHash * xr;
  int w;
  int h;
  char * selected_ship;
} spacemap;

#define VIPER    1
#define RAIDER   2
#define SHUTTLE  3

int spaceship_id_cmp( void * id_a, void * id_b );
int spaceship_id_hash( void * id );

spacemap * spacemap_init();

spaceship * new_spaceship( char* name, uint8 type, int x, int y );
void        spacemap_add( spacemap * map, spaceship * ship );
spaceship * spacemap_get( spacemap * map, const char* name );

#endif
