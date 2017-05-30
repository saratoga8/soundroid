/**
 * @file
 * The SDP service of blue tooth connection
 *
 **
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Daniel Haimov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "service.h"
#include <string.h>
#include <errno.h>

/**
 * Set the ID for general SDP service
 * @return The SDP record instance
 */
sdp_record_t* setGeneralServiceID()
{
    uint32_t svc_uuid_int[] = { 0x00000000,0x00000000,0x00000000,0x3233 };
    uuid_t svc_uuid;
    sdp_record_t *record = sdp_record_alloc();
    
    sdp_uuid128_create( &svc_uuid, &svc_uuid_int );
    sdp_set_service_id( record, svc_uuid );
    sdp_list_t service_class = {NULL, &svc_uuid};
    
    sdp_set_service_classes( record, &service_class);

    return record;
}

/**
 * Set the given SDP record browsalbe
 * @param record The SDP record
 */
void setServiceRecordBrowsable(sdp_record_t *record)
{
    uuid_t root_uuid;
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    sdp_list_t *root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups( record, root_list );
    sdp_list_free( root_list, 0 );
}

/**
 * Set L2cap info
 * @param proto_list The SDP list
 */
void setL2CapInfo(sdp_list_t *proto_list)
{
    uuid_t l2cap_uuid;
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    sdp_list_t *l2cap_list = sdp_list_append( 0, &l2cap_uuid );
    proto_list = sdp_list_append( 0, l2cap_list );
    
    sdp_list_free( l2cap_list, 0 );    
}

/**
 * Set RFCOMM info
 * @param proto_list The SDP list
 */
void setRfcommInfo(sdp_list_t *proto_list)
{
    uuid_t rfcomm_uuid;
    uint8_t rfcomm_channel = RFCCOMM_CHANEL_NUM;
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    sdp_data_t *channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
    sdp_list_t *rfcomm_list = sdp_list_append( 0, &rfcomm_uuid );
    
    sdp_list_append( rfcomm_list, channel );
    sdp_list_append( proto_list, rfcomm_list );
    
    sdp_list_free( rfcomm_list, 0 );
    sdp_data_free( channel );
}

/**
 * Add the sdp protocol info to the SDP record
 * @param record The SDP record
 * @param proto_list The SDP protocol list
 */
void addProtoInfoToRecord(sdp_record_t *record, sdp_list_t *proto_list)
{
    sdp_list_t *access_proto_list = sdp_list_append( 0, proto_list );
    sdp_set_access_protos( record, access_proto_list );
    
    sdp_list_free( access_proto_list, 0 );
}

/**
 * Register SDP service
 * @return SDP session pointer
 */
sdp_session_t *register_service()
{
    const char *service_name = "Soundroid bluetooth daemon";
    const char *service_dsc  = "Remote control of desktop sound from Android app";
    const char *service_prov = "Soundroid desktop daemon";

    sdp_record_t *record = setGeneralServiceID();
    
    setServiceRecordBrowsable(record);
    sdp_list_t *proto_list = 0;
    setL2CapInfo(proto_list);
    setRfcommInfo(proto_list);
    addProtoInfoToRecord(record, proto_list);
    
    sdp_list_free(proto_list, 0 );
    sdp_set_info_attr(record, service_name, service_prov, service_dsc);                      // set the name, provider, and description

    sdp_session_t *session = sdp_connect( BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY );     // connect to the local SDP server, register the service record, and disconnect
    if(sdp_record_register(session, record, 0) != 0)
	{
	    session = NULL;
	    printf("ERROR: Can't register the service record: %s", strerror(errno));
	}

    sdp_record_free(record);
    
    return session;
}
