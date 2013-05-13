/*
 * Utility functions.
 *
 * Author: qiang wang <wqlxx@yahoo.com.cn>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef UTILITY_H
#define UTILITY_H


#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include "openflow.h"
#include "cc_log.h"
#include "cc_error.h"


extern void ( *die )( const char *format, ... );

unsigned int hash_core( const void *key, int size );

uint32_t compare_string( const void *x, const void *y );
unsigned int hash_string( const void *key );

uint32_t compare_mac( const void *x, const void *y );
unsigned int hash_mac( const void *mac );
uint64_t mac_to_uint64( const uint8_t *mac );

uint32_t compare_uint32( const void *x, const void *y );
unsigned int hash_uint32( const void *key );

uint32_t compare_datapath_id( const void *x, const void *y );
unsigned int hash_datapath_id( const void *key );

uint32_t string_to_datapath_id( const char *str, uint64_t *datapath_id );

uint32_t match_to_string( const struct ofp_match *match, char *str, size_t size );
uint32_t wildcards_to_string( uint32_t wildcards, char *str, size_t size );
uint32_t phy_port_to_string( const struct ofp_phy_port *phy_port, char *str, size_t size );
uint32_t actions_to_string( const struct ofp_action_header *actions, uint16_t actions_length, char *str, size_t str_length );

uint16_t get_checksum( uint16_t *pos, uint32_t size );


#endif // UTILITY_H


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */


