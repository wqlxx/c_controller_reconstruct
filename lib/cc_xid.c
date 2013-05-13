/*
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


#include <assert.h>
#include <string.h>

#include "cc_xid.h"


uint32_t
cc_generate_xid(uint32_t* xid_latest, xid_table *xid_table_)
{
  uint32_t initial_value = ( *xid_latest != UINT32_MAX ) ? ++(*xid_latest) : 0;
/*modify by wangq 20130409*/
  assert(xid_table_ != NULL); 
  while ( cc_lookup_xid_entry( xid_table_, initial_value) != NULL ) {
    if ( *xid_latest != UINT32_MAX ) {
      (*xid_latest)++;
    }
    else {
      *xid_latest = 0;
    }
    if ( initial_value == *xid_latest ) {
      log_err_for_cc( "Failed to generate transaction id value." );
      *xid_latest = 0;
      break;
    }
  }

  return *xid_latest;
}


xid_entry*
cc_allocate_xid_entry(uint32_t* xid_latest, xid_table* xid_table_) {
  xid_entry *new_entry;

  new_entry = (xid_entry*)malloc( sizeof( xid_entry ) );
  new_entry->xid = cc_generate_xid(xid_latest, xid_table_);
  gettimeofday(&(new_entry->tv),NULL);  

  return new_entry;
}


 void
cc_free_xid_entry( xid_entry *free_entry ) 
{
	free( free_entry );
}


 void
cc_free_xid_entry_(void* data, void* user_data)
{
	free(data);
}


void
cc_init_xid_table(xid_table** xid_table_ ) {

  log_debug_for_cc("cc_init_xid_table_");
  //memset( &cc_sw_info->xid_table_, 0, sizeof( xid_table ) );
  (*xid_table_) = (xid_table*)malloc(sizeof(xid_table));
  (*xid_table_)->xid_hash = create_hash( NULL, NULL );/*modified by wangq 20130409*/
 // list_element* header = (list_element*)malloc(sizeof(struct list_element));
  (*xid_table_)->xid_entry_list = NULL;
  return ;
}

void
cc_finalize_xid_table(xid_table* xid_table_ )
{
	iterate_list(xid_table_->xid_entry_list, cc_free_xid_entry_, NULL);
  	delete_hash(xid_table_->xid_hash);
	delete_list( xid_table_->xid_entry_list);
}


uint32_t
cc_insert_xid_entry(xid_table* xid_table_ ,uint32_t* xid_latest) {
	xid_entry *new_entry;
	int ret;
 	log_info_for_cc("insert xid entry");

	new_entry = cc_allocate_xid_entry(xid_latest, xid_table_);
	list_element* new_list_e = malloc(sizeof(list_element));
	if( xid_table_->xid_entry_list == NULL )
	{
		new_list_e->data = (void*)new_entry;
		new_list_e->next = NULL;
		xid_table_->xid_entry_list = new_list_e;
	}else{ 
		ret = append_to_tail( &(xid_table_->xid_entry_list), new_entry);
		if( ret < 0 )
		{
			log_err_for_cc("append_to_tail fail");
			return CC_ERROR;
		}
	}

	insert_hash_entry(xid_table_->xid_hash, &(new_entry->xid), new_entry);

 	return new_entry->xid;
}

uint32_t
compare_xid_entry(const void* entry, const void* find_entry)
{
	xid_entry *entry_ = (xid_entry*)entry;
	xid_entry *find_entry_ = (xid_entry*)find_entry;

	if( entry_->xid != find_entry_->xid )
		return CC_ERROR;

	return CC_SUCCESS;
}


int
cc_delete_xid_entry( xid_table* xid_table_, xid_entry *delete_entry ) 
{

  	log_info_for_cc("delete xid entry");
  	xid_entry *deleted = delete_hash_entry(xid_table_->xid_hash, &delete_entry->xid );

  	if ( deleted == NULL ) {
    	log_err_for_cc( "Failed to delete xid entry ");
    	cc_free_xid_entry( ( xid_entry * ) delete_entry );
		return CC_ERROR;
 	}

	
	deleted = find_list_custom(xid_table_->xid_entry_list, compare_xid_entry, delete_entry);
	delete_element(&(xid_table_->xid_entry_list), deleted);
 	cc_free_xid_entry( deleted );

	return CC_SUCCESS;
}


xid_entry*
cc_lookup_xid_entry( xid_table* xid_table_, uint32_t xid )
{
 xid_entry* find_entry;
  if( (find_entry =  (xid_entry*)lookup_hash_entry( xid_table_->xid_hash, &xid ))  == NULL )
  	return find_entry;
;
}


 void
cc_dump_xid_entry( xid_entry *entry ) {
/*
  log_info_for_cc( "xid = %#" PRIx32 ", original_xid = %#" PRIx32 ", service_name = %s, index = %d",
        entry->xid, entry->original_xid, entry->service_name, entry->index );
*/
}


void
cc_dump_xid_table(xid_table* xid_table_) {
  hash_iterator iter;
  hash_entry *e;

  log_info_for_cc( "#### XID TABLE ####" );
  init_hash_iterator( xid_table_->xid_hash, &iter );
  while ( ( e = iterate_hash_next( &iter ) ) != NULL ) {
    cc_dump_xid_entry( e->value );
  }
  log_info_for_cc( "#### END ####" );
}


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */

