/*
 * cc_of_msg_recv functions.
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

#include "cc_of_msg_recv.h"

#if 0//move to cc_of_msg_create.c
int
cc_insert_to_send_queue(message_queue *smq, buffer* buf)
{
	int ret;
	buffer* msg;

	if( smq == NULL )
		smq = create_message_queue();

	ret = enqueue_message(smq, buf);

	return ret;
}


int
cc_insert_to_app_queue(message_queue *asq, buffer* buf)
{
	int ret;
	buffer* msg;

    /*now the app is not implement,so here we directly free the buffer*/
	free_buffer(buf);
	return CC_E_ERR;
}
#endif

char*
cc_dump_flow(struct flow* flow,uint32_t wildcards)
{
	char *str;

	uint32_t nw_dst_mask, nw_src_mask;
    uint32_t ip_mask;

    wildcards = ntohl(wildcards);
    ip_mask = ((wildcards & OFPFW_NW_DST_MASK) >> OFPFW_NW_DST_SHIFT);
    nw_dst_mask = ip_mask >= 32 ? 0 :
                           make_inet_mask(32-ip_mask);

    ip_mask = ((wildcards & OFPFW_NW_SRC_MASK) >> OFPFW_NW_SRC_SHIFT);
    nw_src_mask = ip_mask >= 32 ? 0 :
                           make_inet_mask(32-ip_mask);

	if(wildcards&OFPFW_ALL)
	{
			sprintf(str,"in_port is %d ,dl_vlan is %d, dl_src is %d.%d.%d.%d.%d.%d \
						dl_dst is %d.%d.%d.%d.%d.%d ,dl_type is 0x%x ,nw_proto is %d ,\
						nw_proto is %d ,tp_src is %d ,tp_dst is %d ,dl_vlan_pcp is %d \
						nw_tos is %d\n",flow->in_port,flow->dl_vlan,flow->dl_src[0],flow->dl_src[1],
						flow->dl_src[2],flow->dl_src[3],flow->dl_src[4],flow->dl_src[5],flow->dl_dst[0],
						flow->dl_dst[1],flow->dl_dst[2],flow->dl_dst[3],flow->dl_dst[4],flow->dl_dst[5],
						flow->dl_type,flow->nw_proto,flow->tp_src,flow->tp_dst,flow->dl_vlan_pcp,flow->nw_tos);
			printf(str);
	}
	else
	{
		if(wildcards&OFPFW_IN_PORT)
		{
			sprintf(str,"in_port is %d ",flow->in_port);
		}

		if(wildcards&OFPFW_DL_VLAN)
		{
			sprintf(str,"dl_vlan is %d",flow->dl_vlan);
		}

		if(wildcards&OFPFW_DL_SRC)
		{
			sprintf(str,"dl_src is %d.%d.%d.%d.%d.%d ",flow->dl_src[0]
														,flow->dl_src[1]
														,flow->dl_src[2]
														,flow->dl_src[3]
														,flow->dl_src[4]
														,flow->dl_src[5]);
		}

		if(wildcards&OFPFW_DL_DST)
		{
			sprintf(str,"dl_dst is %d.%d.%d.%d.%d.%d ",flow->dl_dst[0]
														,flow->dl_dst[1]
														,flow->dl_dst[2]
														,flow->dl_dst[3]
														,flow->dl_dst[4]
														,flow->dl_dst[5]);
		}

		if(wildcards&OFPFW_DL_TYPE)
		{
			sprintf(str,"dl_type is 0x%x ",flow->dl_type);
		}

		if(wildcards&OFPFW_NW_PROTO)
		{
			sprintf(str,"nw_proto is %d ",flow->nw_proto);
		}

		if(wildcards&OFPFW_TP_SRC)
		{
			sprintf(str,"tp_src is %d ",flow->tp_src);
		}
		
		if(wildcards&OFPFW_TP_DST)
		{
			sprintf(str,"tp_dst is %d ",flow->tp_dst);
		}
		
		if(wildcards&OFPFW_DL_VLAN_PCP)
		{
			sprintf(str,"dl_vlan_pcp is %d ",flow->dl_vlan_pcp);
		}
		
		if(wildcards&OFPFW_NW_TOS)
		{
			sprintf(str,"nw_tos is %d ",flow->nw_tos);
		}

		printf(str);
	}

}

/**/
static int
cc_recv_hello_msg(trans_info_t *tit)
{
	int ret;
	buffer* recv_data;
	buffer* send_buf;
	struct ofp_hello oh;
    
	recv_data = tit->recv_buf->data;
	validate_hello(recv_data);
	if( ret < 0 )
	{
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	log_info_for_cc("recv a hello message");
	oh = (struct ofp_hello*)recv_data;
    *(tit->xid_latest) = ntohl(oh->header.xid) + 1;
    
	send_buf = cc_create_features_request(*(tit->xid_latest));
	if(send_buf == NULL)
	{
		log_err_for_cc("cc_insert_to_send_queue error!");		
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	
	ret = cc_insert_to_send_queue(tit->send_queue, send_buf);
	if(ret < 0)
	{
		log_err_for_cc("cc_insert_to_send_queue error!");
		free_buffer(send_buf);		
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	free_buffer(recv_data);
	return ret;
}


static int
cc_recv_err_msg(trans_info_t *tit)
{
	buffer* recv_data;

	recv_data = tit->recv_buf;	
	struct ofp_error_msg *ofp_err;
	struct flow err_flow;
	char *str;
	struct ofp_flow_mod *ofm = (void*)(ofp_err->data);
	uint32_t wildcards = ofm->match.wildcards;
	int ret;
	
	ret = validate_error(recv_data);
	if( ret < 0 )
	{
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	log_info_for_cc("recv a error message");
	ofp_err = (struct ofp_error_msg*)(recv_data->data);
    *(tit->xid_latest) = htonl(ofp_err->header.xid) + 1;

	switch(ntohs(ofp_err->type))
	{
		case OFPET_HELLO_FAILED:         /* Hello protocol failed. */
    	case OFPET_BAD_REQUEST:         /* Request was not understood. */
    	case OFPET_BAD_ACTION:           /* Error in action description. */
    	case OFPET_PORT_MOD_FAILED:      /* Port mod request failed. */
    	case OFPET_QUEUE_OP_FAILED: 
			break;
		case OFPET_FLOW_MOD_FAILED:      /* Problem modifying flow entry. */
			{
				err_flow.in_port = ofm->match.in_port;
           		memcpy(err_flow.dl_src, ofm->match.dl_src, sizeof ofm->match.dl_src);
            	memcpy(err_flow.dl_dst, ofm->match.dl_dst, sizeof ofm->match.dl_dst);
            	err_flow.dl_vlan = ofm->match.dl_vlan;
            	err_flow.dl_type = ofm->match.dl_type;
            	err_flow.dl_vlan_pcp = ofm->match.dl_vlan_pcp;
            	err_flow.nw_src = ofm->match.nw_src;
            	err_flow.nw_dst = ofm->match.nw_dst;
            	err_flow.nw_proto = ofm->match.nw_proto;
            	err_flow.tp_src = ofm->match.tp_src;
            	err_flow.tp_dst = ofm->match.tp_dst;

				cc_dump_flow(&err_flow,wildcards);
				break;
			}
		default:
				break;			
	}
	free_buffer(recv_data);
	return CC_E_NONE;
}


static int
cc_recv_echo_request(trans_info_t *tit)
{
	buffer* recv_data;
	uint32_t xid;
	uint16_t length;
	int ret;
	buffer* send_buf;
	struct ofp_header *header;

	recv_data = tit->recv_buf;	
	ret = validate_echo_request(recv_data);
	if( ret < 0 )
	{
		free_buffer(recv_data);
		return CC_E_ERR;
	}

	header = (struct ofp_header *)(recv_data->data);
	xid = header->xid;
	length = header->length;
    *(tit->xid_latest) = ntohl(xid) + 1;
	log_info_for_cc("recv a echo request message");

	//*xid_latest = xid;// here we need to fix the bug
	//cc_lookup_xid_entry(xid_table_, xid);
	/*HERE we do nothing about the xid, because the request need a reply with same xid in reply*/
    
	send_buf = cc_create_echo_reply(xid, recv_data);
	if(send_buf == NULL)
	{
		log_err_for_cc("cc_insert_to_send_queue error!");		
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	
	ret = cc_insert_to_send_queue(tit->send_queue, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_to_send_queue error!");
		free_buffer(send_buf);		
		free_buffer(recv_data);
		return CC_E_ERR;
	}	
	free_buffer(recv_data);
	return CC_E_NONE;//here the xid should be correct!
}


static int
cc_recv_echo_reply(trans_info_t *tit)
{
	int ret;
	buffer* send_buf;
	uint32_t xid;
	buffer* recv_data;
	xid_entry *recv_xid;
    struct ofp_header *oer;

	recv_data = tit->recv_buf;	
	ret = validate_echo_reply(recv_data);
	if(ret < 0)
	{
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	log_info_for_cc("recv a echo reply message");
    
 	struct ofp_header* header = (struct ofp_header*)(recv_data->data);	   
    oer = (struct ofp_header*)(recv_data->data);
    xid = ntohl(oer->header.xid);
    if(!(recv_xid = cc_lookup_xid_entry(tit->xid_table_, xid))){
        free_buffer(recv_data);
        return CC_E_ERR;
    }
	CC_ERROR_RETURN(cc_delete_xid_entry(tit->xid_table_, recv_xid));
	*(tit->xid_latest) = xid + 1;

	send_buf = cc_create_echo_request(tit->xid_latest, recv_data);
	if(send_buf == NULL)
	{
		log_err_for_cc("cc_insert_to_send_queue error!");			
		free_buffer(recv_data);
		return CC_E_ERR;
	}
		
	ret = cc_insert_to_send_queue(tit->send_queue, send_buf);
	if(ret < 0)
	{
		log_err_for_cc("cc_insert_to_send_queue error!");
		free_buffer(send_buf);			
		free_buffer(recv_data);
		return CC_E_ERR;
	}	 
    free_buffer(recv_data);
	
	return CC_E_NONE;
}


static int
cc_recv_vendor(trans_info_t *tit)
{
	int ret;
	buffer* recv_data;
	struct ofp_vendor_header *ovh;

	recv_data = tit->recv_buf;	
	ret = validate_vendor(recv_data);
	if(ret < 0)
	{
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	log_info_for_cc("recv a vendor msg");
	ovh = (struct ofp_vendor_header*)(recv_data->data);
	*(tit->xid_latest) = ntohl(ovh->header.xid) + 1;
    
	ret = cc_insert_to_app_queue(tit->app_queue, recv_data);
	if(ret < 0)
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(recv_data);
		return CC_E_ERR;
	}

	return CC_E_NONE;
}


static int
cc_recv_get_config_reply(trans_info_t *tit)
{
	
	/* DO NOTHING*/
	int ret;
	buffer* send_buf = NULL;
	buffer* recv_data;
	struct ofp_queue_get_config_reply *oqgcr;
	xid_entry *recv_xid;

	recv_data = tit->recv_buf;	
	ret = validate_get_config_reply(recv_data);
	if(ret < 0)
	{
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	log_info_for_cc("recv a get config reply");
    
    oqgcr = (struct ofp_queue_get_config_reply *)(recv_data->data);
    if(!(recv_xid = cc_lookup_xid_entry(tit->xid_table_, oqgcr->header.xid))){
        free_buffer(recv_data);
        return CC_E_ERR;
    }
	CC_ERROR_RETURN(cc_delete_xid_entry(tit->xid_table_, recv_xid));
   	*(tit->xid_latest) = ntohl(oqgcr->header.xid) + 1;

	send_buf = cc_create_echo_request(*(tit->xid_latest), recv_data);
	if(send_buf == NULL)
	{
		log_err_for_cc("cc_create_echo_request error!");
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	
	ret = cc_insert_to_send_queue(tit->send_queue, send_buf);
	if(ret < 0)
	{
		log_err_for_cc("cc_insert_echo_request error!");
		free_buffer(send_buf);		
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	free_buffer(recv_data);
	return CC_E_NONE;
	
}


static int
cc_recv_flow_removed(trans_info_t *tit)
{

	int ret;
	buffer* recv_data;
	struct ofp_flow_removed *ofr;

	recv_data = tit->recv_buf;
	ret = validate_flow_mod(recv_data);
	if(ret < 0)
	{
		free_buffer(recv_data);
		return CC_E_ERR;
	}
    
    ofr = (struct ofp_flow_removed*)(recv_data->data);
	*(tit->xid_latest) = ntohl(ofr->header.xid) + 1;
	log_info_for_cc("recv a flow removed msg");
    
	ret = cc_insert_to_app_queue(tit->app_queue, recv_data);
	if(ret == CC_E_ERR)
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	
	return CC_E_NONE;
}


static int
cc_recv_barrier_reply(trans_info_t *tit)
{
	int ret;
	buffer* recv_data;
    struct ofp_header *obr;
	xid_entry *recv_xid;
    uint32_t xid;
    
	recv_data = tit->recv_buf;		
	ret = validate_barrier_reply(recv_data);
	if(ret < 0)
	{
		free_buffer(recv_data);
		return CC_E_ERR;
	}
    
    obr = (struct ofp_header*)(recv_data->data);
    xid = ntohl(obr->header.xid);
    if(!(recv_xid = cc_lookup_xid_entry(tit->xid_table_, xid))){
        free_buffer(recv_data);
        return CC_E_ERR;
    }
	CC_ERROR_RETURN(cc_delete_xid_entry(tit->xid_table_, recv_xid));
	*(tit->xid_latest) = xid + 1;
	log_info_for_cc("recv a barrier reply");
    
	ret = cc_insert_to_app_queue(tit->app_queue, recv_data);
	if(ret == CC_E_ERR)
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	
	return CC_E_NONE;
}


static int
cc_recv_features_reply(trans_info_t *tit)
{
	int ret;
	size_t n_ports;
	struct ofp_switch_features *feat_rep;
	buffer* recv_data;
	xid_entry *recv_xid;
    uint32_t xid;
    
	recv_data = tit->recv_buf;
	ret = validate_error(recv_data);
	if( ret < 0 )
	{
		free_buffer(recv_data);
		return CC_E_ERR;
	}

	log_info_for_cc("get the feature reply from the switch");
	feat_rep = (struct ofp_switch_features)(recv_data->data);
    xid = ntohl(feat_rep->header.xid);
    if(!(recv_xid = cc_lookup_xid_entry(tit->xid_table_, xid))){
        free_buffer(recv_data);
        return CC_E_ERR;
    }
	CC_ERROR_RETURN(cc_delete_xid_entry(tit->xid_table_, recv_xid));   
	*(tit->xid_latest) = xid + 1;

    tit->sw_info->dpid			= ntohll(feat_rep->datapath_id);
	tit->sw_info->version     	= ntohl(feat_rep->header.version);
	tit->sw_info->n_buffers 	= ntohs(feat_rep->n_buffers);
	tit->sw_info->actions   	= ntohl(feat_rep->actions);
	tit->sw_info->capabilities  = ntohl(feat_rep->capabilities);
	
	n_ports = ((ntohs(feat_rep->header.length) - offsetof(struct ofp_switch_features, ports))/ sizeof *feat_rep->ports);
	if(n_ports > CC_MAX_PORT)
	{
		log_err_for_cc("the port num of switch is out of range of the max mount");
		n_ports = n_ports < CC_MAX_PORT ? n_ports : CC_MAX_PORT; 
	}
/*
	int i;
	for(i = 0; i < n_ports; i++)
	{
		cc_process_phy_port(cc_sw_info,&feat_rep->ports[i],OFPPR_ADD);
	}
*/
	buffer* send_buf;
	send_buf = cc_create_set_config(*(ort->xid_latest), CC_DEFAULT_CONFIG_FLAGS, CC_DEFAULT_MISS_SEND_LEN);
	if( send_buf == NULL )
	{
		log_err_for_cc("cc_create_set_config error!");		
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	ret = cc_insert_to_send_queue(ort->smq, send_buf);
	if( ret < 0 )
	{
		log_err_for_cc("cc_create_echo_request error!");
		free_buffer(send_buf);		
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	
	return CC_E_NONE;
}

static int
cc_recv_packet_in(trans_info_t *tit)
{
	int ret;
	buffer* recv_data;
	struct ofp_packet_in *opi;
    
	recv_data = tit->recv_buf;	
	ret = validate_packet_in(recv_data);
	if( ret < 0 )
	{
		free_buffer(recv_data);
		return CC_E_ERR;
	}
    opi = (struct ofp_packet_in*)(recv_data->data);
    *(tit->xid_latest) = ntohl(opi->header.xid) + 1;

	log_info_for_cc("recv packet-in msg from switch");
	ret = cc_insert_to_app_queue(tit->app_queue, recv_data);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(recv_data);
		return CC_E_ERR;
	}

	return CC_E_NONE;
}

static int
cc_recv_port_status(trans_info_t *tit)
{
	int ret;
	buffer* recv_data;
	struct ofp_port_status *ops;

	recv_data = tit->recv_buf;		
	ret = validate_port_status(recv_data);
	if(ret < 0)
	{
		free_buffer(recv_data);
		return CC_E_ERR;
	}
    ops = (struct ofp_port_status *)(recv_data->data);
    *(tit->xid_latest) = ntohl(ops->header.xid) + 1;
	
	log_info_for_cc("recv a port status");
	ret = cc_insert_to_app_queue(tit->app_queue, recv_data);
	if(ret < 0)
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(recv_data);
		return CC_E_ERR;
	}
	
	return CC_E_NONE;
}


static int
cc_recv_stats_reply(trans_info_t *tit)
{
	int ret;
	buffer* recv_data;
	struct ofp_stats_reply *osr;
	xid_entry *recv_xid;
	uint32_t xid;
    
	recv_data = tit->recv_buf;	
	ret = validate_stats_reply(recv_data);
	if( ret < 0 )
	{
		free_buffer(recv_data);
		return CC_E_ERR;
	}
    
    osr = (struct ofp_stats_reply*)(recv_data->data);
    xid = ntohl(osr->header.xid);
    if(!(recv_xid = cc_lookup_xid_entry(tit->xid_table_, xid))){
        free_buffer(recv_data);
        return CC_E_ERR;
    }
	CC_ERROR_RETURN(cc_delete_xid_entry(tit->xid_table_, recv_xid));
	*(tit->xid_latest) = xid + 1;
    
	log_info_for_cc("recv a stats reply");
	ret = cc_insert_to_app_queue(tit->app_queue, recv_data);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(recv_data);
		return CC_E_ERR;
	}

	return CC_E_NONE;
}


static int
cc_recv_flow_stats_reply(trans_info_t *tit)
{
	int ret;
	buffer* recv_data;
	struct ofp_stats_reply *osr;
    xid_entry *recv_xid;
	uint32_t xid;

	recv_data = tit->recv_buf;	
	ret = validate_flow_stats_reply(recv_data);
	if(ret < 0)
	{
		free_buffer(recv_data);
		return CC_E_ERR;
	}
 
	osr = (struct ofp_stats_reply*)(recv_data->data);
    xid = ntohl(osr->header.xid);
    if(!(recv_xid = cc_lookup_xid_entry(tit->xid_table_, xid))){
        free_buffer(recv_data);
        return CC_E_ERR;
    }
	CC_ERROR_RETURN(cc_delete_xid_entry(tit->xid_table_, recv_xid));
	*(tit->xid_latest) = xid + 1;
    
	log_info_for_cc("recv a flow stats reply");
	ret = cc_insert_to_app_queue(tit->app_queue, recv_data);
	if( ret < 0 )
	{
		log_err_for_cc("cc_insert_to_app_queue error!");
		free_buffer(recv_data);
		return CC_E_ERR;
	}

	return CC_E_NONE;
}


const struct cc_recv_ops_s cc_recv_ops = {
        .version                = CC_OFP_VER_1_0,
    	.recv_hello 			= cc_recv_hello_msg,
        .recv_error 			= cc_recv_err_msg,
        .recv_echo_request 		= cc_recv_echo_request,
        .recv_echo_reply 		= cc_recv_echo_reply,
        .recv_vendor 			= cc_recv_vendor,
        .recv_get_config_reply 	= cc_recv_get_config_reply,
        .recv_flow_rm 			= cc_recv_flow_removed,
        .recv_barrier_reply 	= cc_recv_barrier_reply,
        .recv_packet_in 		= cc_recv_packet_in,
        .recv_port_status 		= cc_recv_port_status,
        .recv_flow_stats_reply 	= cc_recv_port_status
};

/*

uint32_t
search_port(void *data, void *user_data)
{
	struct ofp_phy_port *tmp_a = (struct ofp_phy_port *)data;
    struct ofp_phy_port *tmp_b = (struct ofp_phy_port *)user_data;
	if(tmp_a->port_no == tmp_b->port_no)
  		return CC_E_NONE;
	else 
        return CC_E_ERR;
}


int
cc_process_phy_port(each_sw *sw_info, struct ofp_phy_port* port, uint8_t reason)
{
	
	switch(reason){
			
		case OFPPR_DELETE:
            find_list_custom(sw_info->ports, search_port, port)
			delete_element(sw_info->ports, port);

		case OFPPR_MODIFY:
		case OFPPR_ADD:
            insert_in_front(sw_info->ports, port);
		default:
			return CC_E_ERR;
	}
	return CC_E_NONE;			
}
*/

