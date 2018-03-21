/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.scalar.conf,v 1.5 2002/07/18 14:18:52 dts12 Exp $
 */
/*********************************************************
* Modified  by Deltanetwork  2004.12.25 --- DNI  
* modules for dot1dbridge, base net-snmp-5.0.9
*  auto generated by  mib2c code and I  merge it to dot1dbridge
***********************************************************/
#ifndef DOT1DBRIDGE_SCALAR_H
#define DOT1DBRIDGE_SCALAR_H

#include "dot1dBridge_table.h"


/* here is my own variable and routine */
/*
struct Bridge_Id
{
	unsigned char prio[2];
	unsigned char addr[6];
}Bridge_Id;
*/
/*
typedef struct if_info_S
{
	int ifindex;
	char ifname[8];
}if_info_T;
*/
typedef unsigned int timestr[2];

typedef struct bridgestp_info_S
{
	unsigned char bname[8];
	unsigned char  designated_root[8];
	struct Bridge_Id bridge_id;
	int path_cost;
	
	timestr max_age;
	timestr hello_time;
	timestr forward_delay;
	timestr bridge_max_age;
	timestr bridge_hello_time;
	timestr bridge_forward_delay;
	timestr topology_change_timer;
	int root_port;
	timestr ageing_time;
	timestr gc_interval;
	timestr gc_timer;
	timestr hello_timer;
	timestr tcn_timer;
	int flags;
	int brgstp_priority;
	int brgstp_status;
}
bridgestp_info_T;


/* bridge contrl routine */
int showstp(char * );
int getmacbyifname( char*,char *);
int setAgeingtime(int );
int setBridgeHellotime(int );
int SetStpPriority(int );
int SetSptBridgeMaxAge(unsigned long time );
int SetStpBridgeHelloTime(unsigned long time);
int SetStpBridgeForwardDelay(unsigned long time );
int getstpinfo(char * );


/* 
 * Bridge Parameters  from RFC 1493  
 */
 
#define MAX_BASE_PORT_CIRCUIT		2
#define MAX_MACADDR_SIZE           	6  			/* MIB_MACADDR_SIZE */
#define MAX_BRIDGEID_SIZE         		2  			/* ???  MIB_BRIDGEID_SIZE  */ 
#define MAX_DESIGNPORT_SIZE      	2
#define MAX_PORT_CIRCUIT			2

#define BASE_TYPE_TRANSPARENT_ONLY 	2  		/* dot1dBaseType  */
#define STP_PROTOCOL_SPEC_IEEE8021D 	3 		 /* dot1dStpProtocolSpecification  */
#define STP_Max_Priority		65535;
#define STP_Min_Priority		1;

/* define stp port status */
#define STP_PORT_STATE_DISABLED         	1
#define STP_PORT_STATE_BLOCKING        	2
#define STP_PORT_STATE_LISTENING        	3
#define STP_PORT_STATE_LEARNING         	4
#define STP_PORT_STATE_FORWARDING      5
#define STP_PORT_STATE_BROKEN           	6
#define STP_PORT_STATE_DISCARDING       7

#define STP_PORT_ENABLE_ENABLED         1
#define STP_PORT_ENABLE_DISABLED        2

/*
single OID scalar var decalration 
*/
int
get_newRoot(netsnmp_mib_handler *,
            netsnmp_handler_registration *,
            netsnmp_agent_request_info *,
            netsnmp_request_info *);

int
get_topologyChange(netsnmp_mib_handler *,
                   netsnmp_handler_registration *,
                   netsnmp_agent_request_info *,
                   netsnmp_request_info *);

int	get_dot1dBaseBridgeAddress(netsnmp_mib_handler *,
		netsnmp_handler_registration *,
                netsnmp_agent_request_info *,
                netsnmp_request_info *);
int
get_dot1dBaseNumPorts(netsnmp_mib_handler *,
                      netsnmp_handler_registration *,
                      netsnmp_agent_request_info *,
                      netsnmp_request_info *);
int
get_dot1dBaseType(netsnmp_mib_handler *,
                  netsnmp_handler_registration *,
                  netsnmp_agent_request_info *,
                  netsnmp_request_info *);
int
get_dot1dTpLearnedEntryDiscards(netsnmp_mib_handler *,
                                netsnmp_handler_registration *,
                                netsnmp_agent_request_info *,
                                netsnmp_request_info *);

/*
 * function declarations 
 */
void            init_dot1dBridge_scalar(void);
Netsnmp_Node_Handler do_dot1dStpBridgeHelloTime;
Netsnmp_Node_Handler do_dot1dTpAgingTime;
Netsnmp_Node_Handler get_dot1dStpMaxAge;
Netsnmp_Node_Handler get_dot1dStpDesignatedRoot;
Netsnmp_Node_Handler get_dot1dStpTopChanges;
Netsnmp_Node_Handler do_dot1dStpBridgeForwardDelay;
Netsnmp_Node_Handler get_dot1dStpForwardDelay;
Netsnmp_Node_Handler get_dot1dStpHoldTime;
Netsnmp_Node_Handler get_dot1dBaseType;
Netsnmp_Node_Handler do_dot1dStpPriority;
Netsnmp_Node_Handler do_dot1dStpBridgeMaxAge;
Netsnmp_Node_Handler get_dot1dStpRootCost;
Netsnmp_Node_Handler get_dot1dStpHelloTime;
Netsnmp_Node_Handler get_dot1dBaseNumPorts;
Netsnmp_Node_Handler get_newRoot;
Netsnmp_Node_Handler get_topologyChange;
Netsnmp_Node_Handler get_dot1dStpRootPort;
Netsnmp_Node_Handler get_dot1dStpProtocolSpecification;
Netsnmp_Node_Handler get_dot1dBaseBridgeAddress;
Netsnmp_Node_Handler get_dot1dStpTimeSinceTopologyChange;
Netsnmp_Node_Handler get_dot1dTpLearnedEntryDiscards;

#endif                          /* DOT1DBRIDGE_SCALAR_H */
