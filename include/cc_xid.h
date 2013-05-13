/*
 * OpenFlow Switch Manager
 *
 * Author: Kazushi SUGYO
 *
 * Copyright (C) 2008-2012 NEC Corporation
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


#ifndef CC_XID_H
#define CC_XID_H 1

#include <sys/times.h>
#include <stdint.h>
#include <stdio.h>

#include "cc_log.h"
#include "cc_error.h"
#include "cc_hash_table.h"
#include "cc_single_link.h"
#include "cc_utility.h"
#include "openflow.h"

struct xid_entry {
  uint32_t xid;
  struct timeval tv;
};
typedef struct xid_entry xid_entry;


struct xid_table_ {
  hash_table *xid_hash;
  list_element* xid_entry_list;
};
typedef struct xid_table_ xid_table;

uint32_t cc_generate_xid(uint32_t* xid_latest, xid_table *xid_table_ );

xid_entry* cc_allocate_xid_entry( uint32_t* xid , xid_table *xid_table_ );

void cc_free_xid_entry(xid_entry *);

void cc_init_xid_table( xid_table** xid_table_);

void cc_finalize_xid_table( xid_table* xid_table_ );

uint32_t cc_insert_xid_entry( xid_table* xid_table_ ,uint32_t* xid_latest);

int cc_delete_xid_entry(xid_table* xid_table_, xid_entry *delete_entry);

xid_entry *cc_lookup_xid_entry( xid_table* xid_table_, uint32_t xid);

void cc_dump_xid_entry(xid_entry *entry);

void cc_dump_xid_table( xid_table* xid_table_ );


#endif // CC_XID_H


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */


