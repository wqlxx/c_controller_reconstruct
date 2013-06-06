/*
 * OpenFlow Switch Manager
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
#include "cc_of_msg_send.h"

int
cc_send_hello(sw_info *cc_sw_info) 
{
  int ret;
  buffer *buf;

  buf = cc_create_hello(cc_sw_info->xid_latest);
  cc_sw_info->xid_latest++;
  ret = enqueue_message(cc_sw_info->send_queue, buf);
  if ( ret != CC_E_NONE ) {
    log_info_for_cc( "Send 'hello' to a switch");
    return CC_E_ERR;
  }

  return CC_E_NONE;
}


int
cc_send_echo_request(sw_info* cc_sw_info) 
{
  int ret;
  buffer *buf;

  buf = cc_create_echo_request( cc_sw_info->xid_latest, buf );
  if(buf == NULL)
    return CC_E_ERR;

  cc_sw_info->xid_latest++;
  ret = enqueue_message(cc_sw_info->send_queue, buf);
  if ( ret != CC_E_NONE ) {
    log_info_for_cc( "Send echo request to a switch");
    return CC_E_ERR;
  }

  return CC_E_NONE;

}


int
cc_send_echo_reply(sw_info* cc_sw_info) 
{
  int ret;
  buffer *buf;

  buf = cc_create_echo_reply(cc_sw_info->xid_latest, buf);
  if(buf == NULL)
    return CC_E_ERR;
  cc_sw_info->xid_latest++;

  ret = enqueue_message(cc_sw_info->send_queue, buf);
  if ( ret != CC_E_NONE ) {
    log_info_for_cc( "Send echo reply to a switch");
    return CC_E_ERR;
  }

  return CC_E_NONE;
}


int
cc_send_features_request(sw_info* cc_sw_info)
{
  int ret;
  buffer *buf;
  

  buf = cc_create_features_request( cc_sw_info->xid_latest);
  if(buf == NULL)
    return CC_E_ERR;
  cc_sw_info->xid_latest++;

  ret = enqueue_message(cc_sw_info->send_queue, buf);
  if ( ret == 0 ) {
      log_info_for_cc( "Send echo reply to a switch");
      return CC_E_ERR;
  }

  return CC_E_NONE;
}


int
cc_send_get_config_request(sw_info* cc_sw_info)
{
	int ret;
	buffer *buf;

	buf = cc_create_get_config_request(cc_sw_info->xid_latest);
    cc_sw_info->xid_latest++;
    ret = enqueue_message(cc_sw_info->send_queue, buf);
    if ( ret == 0 ) {
        log_info_for_cc( "Send echo reply to a switch");
        return CC_E_ERR;
    }
    
    return CC_E_NONE;
	
}


int
cc_send_set_config(sw_info *cc_sw_info) 
{
  int ret;
  buffer *buf;
  uint32_t xid;

  xid = cc_generate_xid(&cc_sw_info->xid_latest, cc_sw_info->xid_table_);
  
  // default switch configuration
  uint16_t config_flags = OFPC_FRAG_NORMAL;
  int16_t miss_send_len = USHRT_MAX;

  
  buf = cc_create_set_config( xid, config_flags,
                           miss_send_len );

  ret = enqueue_message(cc_sw_info->send_queue, buf);
  if ( ret == 0 ) {
      log_info_for_cc( "Send echo reply to a switch");
      return CC_E_ERR;
  }

  return CC_E_NONE;

}


int
cc_send_error_msg( sw_info* cc_sw_info, uint16_t type, uint16_t code) 
{
  int ret;
  buffer *buf;
  uint32_t xid;

  buf = cc_create_error( cc_sw_info->xid_latest, type, code, buf);

  ret = enqueue_message(cc_sw_info->send_queue, buf);
  if ( ret == CC_E_NONE ) {
    log_info_for_cc( "Send 'error' to a switch ");
  }

  return ret;
}

/*need to repair*/
int
cc_send_flow_mod(sw_info* cc_sw_info)
{
	buffer* send_buf;
	struct ofp_match match;
	uint64_t cookie;
	uint16_t command;
    uint16_t idle_timeout;
	uint16_t hard_timeout;
    uint16_t priority;
	uint32_t buffer_id;
    uint16_t out_port;
	uint16_t flags;
    openflow_actions *actions; 	
	int ret;
	
	struct ofp_match match = (struct ofp_match*)buf;
	cookie = cc_sw_info->cookie;
	send_buf = cc_create_flow_mod(cc_sw_info->xid_latest, match, cookie, command, idle_timeout,
					hard_timeout,  priority,  buffer_id, out_port,
					flags, actions);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_packet_out");
		return CC_E_ERR;
	}

	ret = enqueue_message(cc_sw_info, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		return CC_E_ERR;
	}
	
	return CC_E_NONE;
}


int
cc_send_packet_out(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	uint32_t xid;
	uint32_t buffer_id;
	uint16_t in_port;
	openflow_actions* actions;
	buffer* data;
	int ret;
	
	send_buf = cc_create_packet_out(cc_sw_info->xid_latest, buffer_id, in_port, actions, data)
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_packet_out");
		return CC_E_ERR;
	}
    cc_sw_info->xid_latest++;

	ret = enqueue_message(cc_sw_info->send_queue, send_buf);
	if( ret == CC_E_ERR )
	{
		log_err_for_cc("cc_send_to_secure_channel");
        free_buffer(send_buf);
		return CC_E_ERR;
	}

	return CC_E_NONE;
}

int
cc_send_port_mod(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t port_no;
	uint8_t hw_addr[OFP_ETH_ALEN];
	uint32_t config;
	uint32_t mask;
	uint32_t advertise;
	
	send_buf = cc_create_port_mod(xid, port_no,  hw_addr, config, mask, advertise);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_packet_out");
		return CC_E_ERR;
	}

	ret = enqueue_message(cc_sw_info->send_queue, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_send_to_secure_channel");
        free_buffer(send_buf);
		return CC_E_ERR;
	}

	return CC_E_NONE;	
}


int
cc_send_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t type;
	uint16_t length;
	uint16_t flags;

	send_buf = cc_create_stats_request(cc_sw_info->xid_latest, type, length, flags);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_packet_out");
		return CC_E_ERR;
	}
    cc_sw_info->xid_latest++;
    
	ret = enqueue_message(cc_sw_info->xid_latest, send_buf);
	if( ret == CC_E_ERR )
	{
		log_err_for_cc("cc_send_to_secure_channel");
        free_buffer(send_buf);
		return CC_E_ERR;
	}

	return CC_E_NONE;		
}


int
cc_send_get_desc_stats(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;

	send_buf = cc_create_desc_stats_request(cc_sw_info->xid_latest, flags);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_packet_out");
		return CC_E_ERR;
	}
    cc_sw_info->xid_latest++;
    
	ret = enqueue_message(cc_sw_info->send_queue, send_buf);
	if( ret == CC_E_ERR )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		free_buffer(send_buf);
		return CC_E_ERR;
	}	
	return CC_E_NONE;
}


int
cc_send_flow_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;
	struct ofp_match match;
	uint8_t table_id;
	uint16_t out_port;
	
	send_buf = cc_create_flow_stats_request(cc_sw_info->xid_latest, flags, match, 
									table_id, out_port);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_packet_out");
		return CC_E_ERR;
	}
    cc_sw_info->xid_latest++;

	ret = enqueue_message(cc_sw_info->send_queue, send_buf);
	if( ret == CC_E_ERR )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		free_buffer(send_buf);
		return CC_E_ERR;
	}	
	
	return CC_E_NONE;	
}


int
cc_send_aggregate_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;
	struct ofp_match match;
	uint8_t table_id;
	uint16_t out_port;

	send_buf = cc_create_aggregate_stats_request(const uint32_t transaction_id,const uint16_t flags,const struct ofp_match match,const uint8_t table_id,const uint16_t out_port);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_aggregate_stats_request");
		return CC_E_ERR;
	}
    cc_sw_info->xid_latest++;

	ret = enqueue_message(cc_sw_info->send_queue, send_buf);
	if( ret == CC_E_ERR )
	{
		log_err_for_cc("cc_send_to_secure_channel");
		free_buffer(send_buf);
		return CC_E_ERR;
	}	
	
	return CC_E_ERR;	
}


int
cc_send_table_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;

	send_buf = cc_create_table_stats_request(xid, flags);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_table_stats_request");
		return CC_E_ERR;
	}
    cc_sw_info->xid_latest++;

	ret = cc_send_to_secure_channel(cc_sw_info, send_buf);
	if( ret == CC_E_ERR )
	{
		log_err_for_cc("cc_send_to_secure_channel");
        free_buffer(send_buf);
		return CC_E_ERR;
	}

	return CC_E_NONE;	
}


int
cc_send_port_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;
	uint16_t port_no;

	send_buf = cc_create_port_stats_request(cc_sw_info->xid_latest, flags, port_no);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_port_stats_request");
		return CC_E_ERR;
	}
    cc_sw_info->xid_latest++;

	ret = enqueue_message(cc_sw_info->send_queue, send_buf);
	if( ret == CC_E_ERR )
	{
		log_err_for_cc("cc_send_to_secure_channel");
        free_buffer(send_buf);
		return CC_E_ERR;
	}

	return CC_E_NONE;	
}


int
cc_send_queue_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;
	uint16_t port_no;
	uint32_t queue_id;

	send_buf = cc_create_queue_stats_request(cc_sw_info->xid_latest, flags, port_no, queue_id);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_send_queue_stats_request");
		return CC_E_ERR;
	}
    cc_sw_info->xid_latest++;
    
	ret = enqueue_message(cc_sw_info->send_queue, send_buf);
	if( ret == CC_E_ERR )
	{
		log_err_for_cc("cc_send_to_secure_channel");
        free_buffer(send_buf);
		return CC_E_ERR;
	}

	return CC_E_NONE;		
}


int
cc_send_vendor_stats_request(sw_info* cc_sw_info, buffer* buf)
{
	buffer* send_buf;
	int ret;
	uint32_t xid;
	uint16_t flags;
	uint32_t vendor;
	buffer body;

	send_buf = cc_create_vendor_stats_request(cc_sw_info->xid_latest, flags, vendor, body);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_vendor_stats_request");
		return CC_E_ERR;
	}
    cc_sw_info->xid_latest++;

	ret = enqueue_message(cc_sw_info->send_queue, send_buf);
	if( ret == CC_E_ERR )
	{
		log_err_for_cc("cc_send_to_secure_channel");
        free_buffer(send_buf);
		return CC_E_ERR;
	}

	return CC_E_NONE;
}

const struct cc_send_ops_s cc_send_ops = {
    .version = CC_OFP_VER_1_0,
    .send_hello                     = cc_send_hello,
    .send_error                     = cc_send_error_msg,
    .send_echo_reply                = cc_send_echo_reply,
    .send_echo_request              = cc_send_echo_request,
    .send_feature_request           = cc_send_features_request,
    .send_get_config_request        = cc_send_get_config_request,
    .send_set_config                = cc_send_set_config,
    .send_packet_out                = cc_send_packet_out,
    .send_flow_stats_request        = cc_send_flow_stats_request,
    .send_get_desc_stats            = cc_send_get_desc_stats,
    .send_aggregate_stats_request   = cc_send_aggregate_stats_request,
    .send_table_stats_request       = cc_send_table_stats_request,
    .send_port_stats_request        = cc_send_port_stats_request,
    .send_queue_stats_request       = cc_send_queue_stats_request,
    .send_vendor_stats_request      = cc_send_vendor_stats_request
}

