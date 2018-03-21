/*
* Reserved by DNI
* modules for comcast
*/

#include <stdio.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "comcast.h"

char *module_file = "/module_name";
static char sysinfo[128];

/*Get Sysdescr to identify the device*/
int
get_SysDescriptionOfRouter(netsnmp_mib_handler *handler,
                            netsnmp_handler_registration *reginfo,
                            netsnmp_agent_request_info *reqinfo,
                            netsnmp_request_info *requests)
{
	FILE *fp;
	int n = 0;

	switch (reqinfo->mode) {
		case MODE_GET:
                	fp=fopen(module_file , "r");
                	if(fp)
                	{
                        	int c;
                        	int i;
                        	i = 0;
                        	while((c=fgetc(fp)) != EOF)
                                	sysinfo[i++] = c;
                        	n = i;
                        	fclose(fp);
                	}
                	else
                	{
                        	strcpy(sysinfo,"unknow");
                        	n = 6;
                	}
                	snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                        	(u_char *)sysinfo, n-1);// 0~(n-1)
                	break;
        	default:
                	printf("unknow mode\n");
        	return SNMP_ERR_GENERR;
        }
        return SNMP_ERR_NOERROR;
}

/*Recieve the set request to reboot the router*/
int
do_reboot_router(netsnmp_mib_handler *handler,
                  netsnmp_handler_registration *reginfo,
                  netsnmp_agent_request_info *reqinfo,
                  netsnmp_request_info *requests)
{
	netsnmp_variable_list *var;
	var=requests->requestvb;

	char *info="use to reboot the router(snmpset with type=integer value=1)";

	switch (reqinfo->mode) {
		case MODE_GET:
			snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                                (u_char *)info,strlen(info));
			break;
		case MODE_SET_RESERVE1:
			if (var->type!=ASN_INTEGER)
				netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_WRONGTYPE);
			else if(var->val_len!=sizeof(int))
				netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_WRONGLENGTH);
			else if(*var->val.integer!=1)
				netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_WRONGVALUE);
			else
			;
			break;
		case MODE_SET_RESERVE2:
			break;
		case MODE_SET_FREE:
			break;
		case MODE_SET_ACTION:
			system("/sbin/reboot");
			return SNMP_ERR_NOERROR;
		case MODE_SET_COMMIT:
			break;
		case MODE_SET_UNDO:
			break;
		default:
			printf("unknow mode\n");
		return SNMP_ERR_GENERR;
	}
	return SNMP_ERR_NOERROR;
}

/*Receive the set request to reset router to default*/
int
do_reset_to_default(netsnmp_mib_handler *handler,
                  netsnmp_handler_registration *reginfo,
                  netsnmp_agent_request_info *reqinfo,
                  netsnmp_request_info *requests)
{
        netsnmp_variable_list *var;
        var=requests->requestvb;

        char *info="use to reset to default(snmpset with type=integer value=1)";

        switch (reqinfo->mode) {
                case MODE_GET:
                        snmp_set_var_typed_value(requests->requestvb, ASN_OCTET_STR,
                                (u_char *)info,strlen(info));
                        break;
                case MODE_SET_RESERVE1:
                        if (var->type!=ASN_INTEGER)
                                netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_WRONGTYPE);
                        else if(var->val_len!=sizeof(int))
                                netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_WRONGLENGTH);
                        else if(*var->val.integer!=1)
                                netsnmp_set_request_error(reqinfo, requests,SNMP_ERR_WRONGVALUE);
                        else
                        ;
                        break;
                case MODE_SET_RESERVE2:
                        break;
                case MODE_SET_FREE:
                        break;
                case MODE_SET_ACTION:
                        printf("Began to restore user configuration!\n");
                        system("/bin/config default");
                        system("/sbin/reboot");
                        break;
                case MODE_SET_COMMIT:
                        break;
                case MODE_SET_UNDO:
                        break;
                default:
                        printf("unknow mode\n");
                return SNMP_ERR_GENERR;
        }
        return SNMP_ERR_NOERROR;
}


/* Initializes the sys_description module*/
void
init_sys_description()
{
	static oid SysDescription_oid[] = { 1, 3, 6, 1, 4, 1, 4526, 12, 1, 1 };

	DEBUGMSGTL(("SysDescriptionOfRouter", "Initializing\n"));

	netsnmp_register_read_only_instance(netsnmp_create_handler_registration
                                            ("SysDescriptionOfRouter",
                                             get_SysDescriptionOfRouter,
                                             SysDescription_oid,
                                             OID_LENGTH(SysDescription_oid),
                                             HANDLER_CAN_RONLY));
}

/* Initializes the sys_description module */
void
init_reboot_router()
{
	static oid RebootRouter_oid[] = { 1, 3, 6, 1, 4, 1, 4526, 12, 1, 2 };

	DEBUGMSGTL(("RebootRouter", "Initializing\n"));

	netsnmp_register_instance(netsnmp_create_handler_registration
                                        ("RebootRouter", do_reboot_router,
                                         RebootRouter_oid,
                                         OID_LENGTH(RebootRouter_oid),
                                         HANDLER_CAN_RWRITE));
}

/* Initializes the reset to factory default module*/
void
init_reset_to_factory_defaults()
{
	static oid ResetToDefault_oid[] = { 1, 3, 6, 1, 4, 1, 4526, 12, 1, 3 };

	DEBUGMSGTL(("ResetToDefault", "Initializing\n"));

	netsnmp_register_instance(netsnmp_create_handler_registration
                                        ("ResetToDefault", do_reset_to_default,
                                         ResetToDefault_oid,
                                         OID_LENGTH(ResetToDefault_oid),
                                         HANDLER_CAN_RWRITE));
}

/* Initializes the comcast module */
void
init_comcast(void)
{
	printf("Initilized comcast modules\n");
	//init_sys_description();
	init_reboot_router();
	init_reset_to_factory_defaults();
}

