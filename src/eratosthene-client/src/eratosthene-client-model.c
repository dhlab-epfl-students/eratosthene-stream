/*
 *  eratosthene-suite - geodetic system
 *
 *      Nils Hamel - nils.hamel@bluewin.ch
 *      Copyright (c) 2016 EPFL CDH DHLAB
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

    # include "eratosthene-client-model.h"

/*
    source - constructor/destructor methods
 */

    er_model_t er_model_create( le_size_t er_cells, le_char_t * const er_ip, le_sock_t const er_port ) {

        /* Model variables */
        er_model_t er_model = ER_MODEL_C;

        /* Socket variables */
        le_sock_t er_socket = _LE_SOCK_NULL;

        /* Assign server network configuration */
        er_model.md_svip = er_ip;
        er_model.md_port = er_port;

        /* Establish server connexion */
        if ( ( er_socket = le_client_create( er_ip, er_port ) ) != _LE_SOCK_NULL ) {

            /* Client/server query handshake */
            if ( le_client_handshake_mode( er_socket, LE_NETWORK_MODE_SMOD ) == LE_ERROR_SUCCESS ) {

                /* Read parameter in socket */
                if ( read( er_socket, & ( er_model.md_sdis ), sizeof( le_size_t ) ) != sizeof( le_size_t ) ) {

                    /* Return model */
                    return( er_model );

                }

            } else {

                /* Return model */
                return( er_model );

            }

            /* Close server connexion */
            er_socket = le_client_delete( er_socket );

        }

        /* Establish server connexion */
        if ( ( er_socket = le_client_create( er_ip, er_port ) ) != _LE_SOCK_NULL ) {

            /* Client/server query handshake */
            if ( le_client_handshake_mode( er_socket, LE_NETWORK_MODE_TMOD ) == LE_ERROR_SUCCESS ) {

                /* Read parameter in socket */
                if ( read( er_socket, & ( er_model.md_tdis ), sizeof( le_time_t ) ) != sizeof( le_time_t ) ) {

                    /* Return model */
                    return( er_model );

                }

            } else {

                /* Return model */
                return( er_model );

            }

            /* Close server connexion */
            er_socket = le_client_delete( er_socket );

        }

        /* Allocate cell array memory */
        if ( ( er_model.md_cell = ( er_cell_t * ) malloc( er_cells * sizeof( er_cell_t ) ) ) != NULL ) {

            /* Assign cell array size */
            er_model.md_size = er_cells;

            /* Parsing cells array */
            for ( le_size_t er_parse = 0; er_parse < er_model.md_size; er_parse ++ ) {

                /* Initialise cell */
                ( er_model.md_cell )[er_parse] = er_cell_create();

            }

            /* Return model */
            return( er_model );

        } else {

            /* Return model */
            return( er_model );

        }

    }

    le_void_t er_model_delete( er_model_t * const er_model ) {

        /* Model variables */
        er_model_t er_reset = ER_MODEL_C;
        
        /* Check cells array state */
        if ( er_model->md_size > 0 ) {

            /* Parsing cells array */
            for ( le_size_t er_parse = 0; er_parse < er_model->md_size; er_parse ++ ) {

                /* Delete cell */
                er_cell_delete( ( er_model->md_cell ) + er_parse );

            }

            /* Unallocate cells array memory */
            free( er_model->md_cell );

        }

        /* Clear model fields */
        * ( er_model ) = er_reset;

    }

/*
    source - accessor methods
 */

    le_size_t er_model_get_sdisc( er_model_t const * const er_model ) {

        /* Return model space discretisation */
        return( er_model->md_sdis );

    }

    le_size_t er_model_get_tdisc( er_model_t const * const er_model ) {

        /* Return model time discretisation */
        return( er_model->md_tdis );

    }

/*
    source - mutator methods
 */

    le_void_t er_model_set_update_prepare( er_model_t * const er_model ) {

        /* Parsing model cells */
        for ( le_size_t er_parse = 1; er_parse < er_model->md_size; er_parse ++ ) {

            /* Reset cell flag */
            er_model->md_cell[er_parse].ce_flag = ER_CELL_0;

        }

        /* Reset push index */
        er_model->md_push = 0;

    }

    le_void_t er_model_set_update_model( er_model_t * const er_model, le_time_t const er_time, le_real_t const er_lon, le_real_t const er_lat, le_real_t er_alt ) {

        /* Parsing variables */
        le_size_t er_plon = 0;
        le_size_t er_plat = 0;

        /* Position vector variable */
        le_real_t er_pose[3] = { 0.0, 0.0, er_alt - ER_ERA };

        /* Scale width variables */
        le_real_t er_scale = LE_2P / pow( 2, LE_GEODESY_ASYA - 1 );

        /* Address variables */
        le_address_t er_addr = LE_ADDRESS_C_SIZE( LE_GEODESY_ASYA - 1 );

        /* Assign address time */
        le_address_set_time( & er_addr, er_time );

        /* Parsing neighbour central cells - longitude */
        for ( er_plon = 0; er_plon < 9; er_plon ++ ) {

            /* Parsing neighbour central cells - latitude */
            for ( er_plat = 0; er_plat < 5; er_plat ++ ) {

                /* Compose position vector */
                er_pose[0] = er_lon + er_scale * ( ( le_real_t ) er_plon - 4.0 );
                er_pose[1] = er_lat + er_scale * ( ( le_real_t ) er_plat - 2.0 );

                /* Assign address depth */
                le_address_set_depth( & er_addr, ER_MODEL_DPT );

                /* Assign address size */
                le_address_set_size( & er_addr, LE_GEODESY_ASYA - 1 );

                /* Assign address position */
                le_address_set_pose( & er_addr, er_pose );

                /* Recursive address push */
                er_model_set_update_cells( er_model, & er_addr, er_lon, er_lat, er_alt );

            }

        }

    }

    le_void_t er_model_set_update_cells( er_model_t * const er_model, le_address_t * const er_addr, le_real_t const er_lon, le_real_t const er_lat, le_real_t const er_alt ) {

        /* Parsing variables */
        le_size_t er_parse = 0;

        /* Distance variables */
        le_real_t er_dist = 0.0;

        /* Scale variables */
        le_real_t er_scale = 0.0;

        /* Socket variables */
        le_sock_t er_socket = _LE_SOCK_NULL;

        /* Address size variables */
        le_size_t er_size = le_address_get_size( er_addr );

        /* Compute distance to cell */
        er_dist = er_geodesy_cell( er_addr, er_lon, er_lat, er_alt - ER_ERA );

        /* Compute geodetic scale */
        er_scale = er_geodesy_distance( er_dist, LE_GEODESY_ASYA - 1, er_model->md_sdis - ER_MODEL_DPT );

        /* Check geodetic scale value */
        if ( ( fabs( er_scale - er_size ) < 1 ) || ( er_size == ( er_model->md_sdis - ER_MODEL_DPT ) ) ) {

            /* Set address depth */
            le_address_set_depth( er_addr, ER_MODEL_DPT );

            /* Set cell address */
            er_cell_set_addr2( er_model->md_cell + ( ++ er_model->md_push ), er_addr );

        } else {

            /* Set address depth */
            le_address_set_depth( er_addr, 0 );

            /* Set cell address */
            er_cell_set_addr( er_model->md_cell, er_addr );

            /* Establish server connexion */
            if ( ( er_socket = le_client_create( er_model->md_svip, er_model->md_port ) ) != _LE_SOCK_NULL ) {
                
                /* Update cell through server query */
                er_cell_set_query( er_model->md_cell, er_socket );

                /* Close server connexion */
                er_socket = le_client_delete( er_socket );

                /* Check cell size */
                if ( er_cell_get_size( er_model->md_cell ) > 0 ) {

                    /* Set address depth */
                    le_address_set_depth( er_addr, ER_MODEL_DPT );

                    /* Parsing sub-cells */
                    for ( er_parse = 0; er_parse < _LE_USE_BASE; er_parse ++ ) {

                        /* Set address size */
                        le_address_set_size( er_addr, er_size + 1 );

                        /* Set address digit */
                        le_address_set_digit( er_addr, er_size, er_parse );

                        /* Recursive cell searching */
                        er_model_set_update_cells( er_model, er_addr, er_lon, er_lat, er_alt );

                    }

                }

            }

        }

    }

    le_void_t er_model_set_update_query( er_model_t * const er_model ) {

        /* Parsing variables */
        le_size_t er_parse = 0;

        /* Selected cell variables */
        le_size_t er_found = 0;

        /* Searching variables */
        le_size_t er_search = 0;

        /* Socket variables */
        le_sock_t er_socket = _LE_SOCK_NULL;

        /* Parsing model cells */
        for ( er_parse = 1; er_parse <= er_model->md_push; er_parse ++ ) {

            /* Check pushed address */
            if ( er_cell_get_push( er_model->md_cell + er_parse ) == _LE_TRUE ) {

                /* Reset search */
                er_found = er_model->md_size;

                /* Searching allocation */
                er_search = 1; while ( ( er_search < er_model->md_size ) && ( er_found == er_model->md_size ) ) {

                    /* Compare address */
                    if ( strcmp( ( char * ) er_model->md_cell[er_parse].ce_push, ( char * ) er_model->md_cell[er_search].ce_addr ) == 0 ) {

                        /* Assign found index */
                        er_found = er_search;

                    } else {

                        /* Continue search */
                        er_search ++;

                    }

                }

                /* Check search results */
                if ( er_found != er_model->md_size ) {

                    /* Reset pushed address */
                    er_model->md_cell[er_parse].ce_push[0] = '\0';

                    /* Update cell flag */
                    er_model->md_cell[er_found].ce_flag = ER_CELL_1;

                }

            }

        }

        /* Reset selected cell */
        er_found = 1;

        /* Parsing model cells */
        for ( er_parse = 1; er_parse <= er_model->md_push; er_parse ++ ) {

            /* Check pushed address */
            if ( er_cell_get_push( er_model->md_cell + er_parse ) == _LE_TRUE ) {

                /* Searched unactive cell */
                while ( ( er_cell_get_flag( er_model->md_cell + er_found ) != ER_CELL_0 ) && ( er_found < er_model->md_size ) ) er_found ++;

                /* Check search results */
                if ( er_found != er_model->md_size ) {

                    /* Copy address */
                    strcpy( ( char * ) er_model->md_cell[er_found].ce_addr, ( char * ) er_model->md_cell[er_parse].ce_push );

                    /* Update cell flag */
                    er_model->md_cell[er_found].ce_flag = ER_CELL_1;

                    /* Establish server connexion */
                    if ( ( er_socket = le_client_create( er_model->md_svip, er_model->md_port ) ) != _LE_SOCK_NULL ) {
                        
                        /* Update cell through server query */
                        er_cell_set_query( er_model->md_cell + er_found, er_socket );

                        /* Close server connexion */
                        er_socket = le_client_delete( er_socket );

                    }

                }

                /* Reset pushed address */
                er_model->md_cell[er_parse].ce_push[0] = '\0';

            }

        }

    }

    le_void_t er_model_set_update_destroy( er_model_t * const er_model ) {

        /* Parsing model cells */
        for ( le_size_t er_parse = 1; er_parse < er_model->md_size; er_parse ++ ) {

            /* Check cell flag */
            if ( er_model->md_cell[er_parse].ce_flag == ER_CELL_0 ) {

                /* Clear address */
                er_model->md_cell[er_parse].ce_addr[0] = '\0';

                /* Clear cell content */
                er_model->md_cell[er_parse].ce_size = 0;

            }

        }

    }

/*
    source - model display methods
 */

    le_void_t er_model_display_cell( er_model_t * const er_model ) {

        /* Count variables */
        le_size_t er_count = 0;

        /* Display cells content */
        for ( le_size_t er_parse = 1; er_parse < er_model->md_size; er_parse ++ ) {

            /* Check cell content */
            if ( ( er_count = er_cell_get_size( ( er_model->md_cell ) + er_parse ) ) > 0 ) {

                /* Vertex and color pointer to cell arrays */
                glVertexPointer( 3, ER_MODEL_VA, 0, er_cell_get_pose( ( er_model->md_cell ) + er_parse ) );
                glColorPointer ( 3, ER_MODEL_CA, 0, er_cell_get_data( ( er_model->md_cell ) + er_parse ) );

                /* Display graphical primitives */
                glDrawArrays( GL_POINTS, 0, er_count / 3 );

                /* Devel temporary */
                er_model_devel_display_cell( er_model->md_cell[er_parse].ce_addr );

            }

        }

    }

    le_void_t er_model_display_earth( le_void_t ) {

        /* Earth frame - orientation */
        glRotated( 90.0, 1.0, 0.0, 0.0 );

        /* Earth frame - color */
        glColor3f( 0.3, 0.32, 0.4 );

        /* Earth model variables */
        GLUquadricObj * er_earth = gluNewQuadric();

        /* Configure quadric */
        gluQuadricDrawStyle( er_earth, GLU_LINE );

        /* Draw quadric */
        gluSphere( er_earth, ER_ERA, 360, 180 );

        /* Delete quadric */
        gluDeleteQuadric( er_earth );

    }

/*
    source - development functions
 */

    le_void_t er_model_devel_display_cell( le_char_t const * const er_saddr ) {

        /* Scale step variables */
        le_real_t er_splan = 0.0;
        le_real_t er_salti = 0.0;

        /* Position array variables */
        le_real_t er_pose[24] = {0.0};

        /* Address variables */
        le_address_t er_addr = LE_ADDRESS_C;

        /* Read address from string */
        le_address_cvsa( & er_addr, er_saddr );

        /* Extract position from address */
        le_address_get_pose( & er_addr, er_pose );

        /* Compute parametric cell size */
        er_splan = ( LE_GEODESY_LMAX - LE_GEODESY_LMIN ) / pow( 2, er_addr.as_size );

        /* Compute cartesian cell size */
        er_salti = ( LE_2P * LE_GEODESY_WGS84_A ) / pow( 2, er_addr.as_size );
        
        /* Compute cell parametric boudaries */
        er_pose[ 3] = er_pose[ 0] + er_splan;
        er_pose[ 4] = er_pose[ 1];
        er_pose[ 5] = er_pose[ 2];
        er_pose[ 6] = er_pose[ 0] + er_splan;
        er_pose[ 7] = er_pose[ 1] + er_splan;
        er_pose[ 8] = er_pose[ 2];
        er_pose[ 9] = er_pose[ 0];
        er_pose[10] = er_pose[ 1] + er_splan;
        er_pose[11] = er_pose[ 2];
        er_pose[12] = er_pose[ 0];
        er_pose[13] = er_pose[ 1];
        er_pose[14] = er_pose[ 2] + er_salti;
        er_pose[15] = er_pose[ 3];
        er_pose[16] = er_pose[ 4];
        er_pose[17] = er_pose[ 5] + er_salti;
        er_pose[18] = er_pose[ 6];
        er_pose[19] = er_pose[ 7];
        er_pose[20] = er_pose[ 8] + er_salti;
        er_pose[21] = er_pose[ 9];
        er_pose[22] = er_pose[10];
        er_pose[23] = er_pose[11] + er_salti;

        /* Compute cell cartesian boundaries */
        er_geodesy_cartesian( er_pose, 24 );

        /* Define cell box color */
        glColor3f( 1.0, 1.0, 0.0 );
        
        /* Display cell box */
        glBegin( GL_LINE_LOOP );

            glVertex3d( er_pose[ 0], er_pose[ 1], er_pose[ 2] );
            glVertex3d( er_pose[ 3], er_pose[ 4], er_pose[ 5] );
            glVertex3d( er_pose[ 6], er_pose[ 7], er_pose[ 8] );
            glVertex3d( er_pose[ 9], er_pose[10], er_pose[11] );

        glEnd();

        /* Display cell box */
        glBegin( GL_LINE_LOOP );

            glVertex3d( er_pose[12], er_pose[13], er_pose[14] );
            glVertex3d( er_pose[15], er_pose[16], er_pose[17] );
            glVertex3d( er_pose[18], er_pose[19], er_pose[20] );
            glVertex3d( er_pose[21], er_pose[22], er_pose[23] );

        glEnd();

        /* Display cell box */
        glBegin( GL_LINES );

            glVertex3d( er_pose[ 0], er_pose[ 1], er_pose[ 2] );
            glVertex3d( er_pose[12], er_pose[13], er_pose[14] );
            glVertex3d( er_pose[ 3], er_pose[ 4], er_pose[ 5] );
            glVertex3d( er_pose[15], er_pose[16], er_pose[17] );
            glVertex3d( er_pose[ 6], er_pose[ 7], er_pose[ 8] );
            glVertex3d( er_pose[18], er_pose[19], er_pose[20] );
            glVertex3d( er_pose[ 9], er_pose[10], er_pose[11] );
            glVertex3d( er_pose[21], er_pose[22], er_pose[23] );

        glEnd();

    }

