
/*
 *  agent_trap.c: define trap generation routines for mib modules, etc,
 * to use 
 */

#include <net-snmp/net-snmp-config.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_NETDB_H
#include <netdb.h>
#endif
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#if TIME_WITH_SYS_TIME
# ifdef WIN32
#  include <sys/timeb.h>
# else
#  include <sys/time.h>
# endif
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#elif HAVE_WINSOCK_H
#include <winsock.h>
#endif
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#include <net-snmp/utilities.h>

#if HAVE_DMALLOC_H
#include <dmalloc.h>
#endif

#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/agent_trap.h>
#include <net-snmp/agent/snmp_agent.h>
#include <net-snmp/agent/agent_callbacks.h>

#include <net-snmp/agent/mib_module_config.h>

//#define SUPPORT_V3TRAP

struct trap_sink
{
	netsnmp_session *sesp;
	struct trap_sink *next;
	int pdutype;
	int version;
};

struct trap_sink *sinks = NULL;

extern struct timeval starttime;

oid objid_enterprisetrap[] = { NOTIFICATION_MIB };
oid trap_version_id[] = { SYSTEM_MIB };
int enterprisetrap_len;
int trap_version_id_len;

#define SNMPV2_TRAPS_PREFIX	SNMP_OID_SNMPMODULES,1,1,5
oid cold_start_oid[] = { SNMPV2_TRAPS_PREFIX, 1 };	/* SNMPv2-MIB */
oid warm_start_oid[] = { SNMPV2_TRAPS_PREFIX, 2 };	/* SNMPv2-MIB */
oid link_down_oid[] = { SNMPV2_TRAPS_PREFIX, 3 };	/* IF-MIB */
oid link_up_oid[] = { SNMPV2_TRAPS_PREFIX, 4 };	/* IF-MIB */
oid auth_fail_oid[] = { SNMPV2_TRAPS_PREFIX, 5 };	/* SNMPv2-MIB */
oid config_change_oid[]= { 1, 3, 6, 1, 4, 1, 388, 13, 2, 3, 1 }; /*add by josn*/
oid acl_fail_oid[]= {1, 3, 6, 1, 4, 1, 388, 13, 2, 3, 2};		/* add by josn*/

oid eth_acl_oid[]= {1, 3, 6, 1, 4, 1, 388, 13, 2, 3, 3};		/* add by denny*/
oid fw_upgrad_oid[]= {1, 3, 6, 1, 4, 1, 388, 13, 2, 3, 4};		/* add by denny*/
oid config_update_oid[]= {1, 3, 6, 1, 4, 1, 388, 13, 2, 3, 5};		/* add by denny*/
oid text_config_oid[]= {1, 3, 6, 1, 4, 1, 388, 13, 2, 3, 6};		/* add by denny*/
oid time_fail_oid[]= {1, 3, 6, 1, 4, 1, 388, 13, 2, 3, 7};		/* add by denny*/
oid rf_thro_oid[]= {1, 3, 6, 1, 4, 1, 388, 13, 2, 3, 8};		/* add by denny*/
oid rf_retries_oid[]= {1, 3, 6, 1, 4, 1, 388, 13, 2, 3, 9};		/* add by denny*/
oid process_fail_oid[]= {1, 3, 6, 1, 4, 1, 388, 13, 2, 3, 10};		/* add by denny*/

oid egp_xxx_oid[] = { SNMPV2_TRAPS_PREFIX, 99 };	/* ??? */



oid objid_enterprise[] = { 1, 3, 6, 1, 4, 1, 3, 1, 1 };
oid objid_sysdescr[] = { 1, 3, 6, 1, 2, 1, 1, 1, 0 };
oid objid_sysuptime[] = { 1, 3, 6, 1, 2, 1, 1, 3, 0 };
oid objid_snmptrap[] = { 1, 3, 6, 1, 6, 3, 1, 1, 4, 1, 0 };

//ccConfigChange 1.3.6.1.4.1.388.11.2.3.7
oid objid_ccCfgHistChangeTime[] =         {1, 3, 6, 1, 4, 1, 388, 13, 2, 2, 3, 2};
oid objid_ccCfgHistChangeCount[] =        {1, 3, 6, 1, 4, 1, 388, 13, 2, 2, 3, 1};

//ccSnmpAclViolation 1.3.6.1.4.1.388.11.2.3.8
oid objid_ccSnmpLastDeniedIpAddr[] =      {1, 3, 6, 1, 4, 1, 388, 13, 2, 2, 4, 2};
oid objid_ccSnmpAclViolations[] =         {1, 3, 6, 1, 4, 1, 388, 13, 2, 2, 4, 1};


#define SNMPV2_TRAP_OBJS_PREFIX	SNMP_OID_SNMPMODULES,1,1,4
oid snmptrap_oid[] = { SNMPV2_TRAP_OBJS_PREFIX, 1, 0 };
oid snmptrapenterprise_oid[] = { SNMPV2_TRAP_OBJS_PREFIX, 3, 0 };
oid sysuptime_oid[] = { SNMP_OID_MIB2, 1, 3, 0 };
size_t snmptrap_oid_len;
size_t snmptrapenterprise_oid_len;
size_t sysuptime_oid_len;


#define SNMP_AUTHENTICATED_TRAPS_ENABLED	1
#define SNMP_AUTHENTICATED_TRAPS_DISABLED	2

int snmp_enableauthentraps = SNMP_AUTHENTICATED_TRAPS_DISABLED;
int snmp_enableauthentrapsset = 0;
char *snmp_trapcommunity = NULL;


static int my_enableColdStartTrap;
static int my_enableAclFailTrap;
static int my_enableAuthFailTrap;
static int my_enableConfChangeTrap;

static int my_enableEthACLVio;
static int my_enableFwUpgradeFail;
static int my_enableConfUpgradeFail;
static int my_enableInvalidTextConf;
static int my_enableTimeAdoptFail;
static int my_enableRfThresholdThro;
static int my_enableRfAvgRetries;
static int my_enableProcessFail;

extern  my_isSkipCheckACL;
extern my_lastDeniedIpAddr;//add by josn
extern my_aclViolations;//add by josn

unsigned long my_cfgHistChangeCount;

void
read_trap_select_file ()
{
	FILE *fp;
	fp = fopen ("/etc/snmp/trap_select.conf", "r");
	if (fp)
	{
		fscanf (fp, "%d %d %d %d %d %d %d %d %d %d %d %d", &my_enableColdStartTrap,
			&my_enableAclFailTrap, &my_enableAuthFailTrap,
			&my_enableConfChangeTrap,
			&my_enableEthACLVio, &my_enableFwUpgradeFail,
			&my_enableConfUpgradeFail, &my_enableInvalidTextConf,
			&my_enableTimeAdoptFail, &my_enableRfThresholdThro,
			&my_enableRfAvgRetries, &my_enableProcessFail);
		fclose (fp);
	}
	else
	{
		my_enableColdStartTrap = 1;
		my_enableAclFailTrap = 1;
		my_enableAuthFailTrap = 1;
		my_enableConfChangeTrap = 1;

		my_enableEthACLVio = 1;
		my_enableFwUpgradeFail = 1;
		my_enableConfUpgradeFail = 1;
		my_enableInvalidTextConf = 1;
		my_enableTimeAdoptFail = 1;
		my_enableRfThresholdThro = 1;
		my_enableRfAvgRetries = 1;
		my_enableProcessFail = 1;
	}
}

/*
 * Prototypes 
 */
 /*
  * static int create_v1_trap_session (const char *, u_short, const char *);
  * static int create_v2_trap_session (const char *, u_short, const char *);
  * static int create_v2_inform_session (const char *, u_short, const char *);
  * static void free_trap_session (struct trap_sink *sp);
  * static void send_v1_trap (netsnmp_session *, int, int);
  * static void send_v2_trap (netsnmp_session *, int, int, int);
  */


	/*******************
	 *
	 * Trap session handling
	 *
	 *******************/

void
init_traps (void)
{
	enterprisetrap_len = OID_LENGTH (objid_enterprisetrap);
	trap_version_id_len = OID_LENGTH (trap_version_id);
	snmptrap_oid_len = OID_LENGTH (snmptrap_oid);
	snmptrapenterprise_oid_len = OID_LENGTH (snmptrapenterprise_oid);
	sysuptime_oid_len = OID_LENGTH (sysuptime_oid);
}

static void
free_trap_session (struct trap_sink *sp)
{
	snmp_close (sp->sesp);
	free (sp);
}

int
add_trap_session (netsnmp_session * ss, int pdutype, int confirm, int version)
{
	if (snmp_callback_available (SNMP_CALLBACK_APPLICATION,
				     SNMPD_CALLBACK_REGISTER_NOTIFICATIONS) ==
	    SNMPERR_SUCCESS)
	{
		/*
		 * something else wants to handle notification registrations 
		 */
		struct agent_add_trap_args args;
		DEBUGMSGTL (("trap", "adding callback trap sink\n"));
		args.ss = ss;
		args.confirm = confirm;
		snmp_call_callbacks (SNMP_CALLBACK_APPLICATION,
				     SNMPD_CALLBACK_REGISTER_NOTIFICATIONS,
				     (void *) &args);
	}
	else
	{
		/*
		 * no other support exists, handle it ourselves. 
		 */
		struct trap_sink *new_sink;

		DEBUGMSGTL (("trap", "adding internal trap sink\n"));
		new_sink = (struct trap_sink *) malloc (sizeof (*new_sink));
		if (new_sink == NULL)
			return 0;

		new_sink->sesp = ss;
		new_sink->pdutype = pdutype;
		new_sink->version = version;
		new_sink->next = sinks;
		sinks = new_sink;
	}
	return 1;
}

int
remove_trap_session (netsnmp_session * ss)
{
	struct trap_sink *sp = sinks, *prev = 0;

	while (sp)
	{
		if (sp->sesp == ss)
		{
			if (prev)
			{
				prev->next = sp->next;
			}
			else
			{
				sinks = sp->next;
			}
			/*
			 * I don't believe you *really* want to close the session here;
			 * it may still be in use for other purposes.  In particular this
			 * is awkward for AgentX, since we want to call this function
			 * from the session's callback.  Let's just free the trapsink
			 * data structure.  [jbpn]  
			 */
			/*
			 * free_trap_session(sp);  
			 */
			free (sp);
			return 1;
		}
		prev = sp;
		sp = sp->next;
	}
	return 0;
}

int
create_trap_session (char *sink, u_short sinkport,
		     char *com, int version, int pdutype)
{
	netsnmp_session session, *sesp;
	char *peername = NULL;

	if ((peername = malloc (strlen (sink) + 4 + 32)) == NULL)
	{
		return 0;
	}
	else
	{
		snprintf (peername, strlen (sink) + 4 + 32, "udp:%s:%hu",
			  sink, sinkport);
	}

	memset (&session, 0, sizeof (netsnmp_session));
	session.peername = peername;
	session.version = version;
	if (com)
	{
		session.community = (u_char *) com;
		session.community_len = strlen (com);
	}
	sesp = snmp_open (&session);
	free (peername);

	if (sesp)
	{
		return add_trap_session (sesp, pdutype,
					 (pdutype == SNMP_MSG_INFORM),
					 version);
	}

	/*
	 * diagnose snmp_open errors with the input netsnmp_session pointer 
	 */
	snmp_sess_perror ("snmpd: create_trap_session", &session);
	return 0;
}

static int
create_v1_trap_session (char *sink, u_short sinkport, char *com)
{
	return create_trap_session (sink, sinkport, com,
				    SNMP_VERSION_1, SNMP_MSG_TRAP);
}

static int
create_v2_trap_session (char *sink, u_short sinkport, char *com)
{
	return create_trap_session (sink, sinkport, com,
				    SNMP_VERSION_2c, SNMP_MSG_TRAP2);
}

static int
create_v2_inform_session (char *sink, u_short sinkport, char *com)
{
	return create_trap_session (sink, sinkport, com,
				    SNMP_VERSION_2c, SNMP_MSG_INFORM);
}


void
snmpd_free_trapsinks (void)
{
	struct trap_sink *sp = sinks;
	while (sp)
	{
		sinks = sinks->next;
		free_trap_session (sp);
		sp = sinks;
	}
}

	/*******************
	 *
	 * Trap handling
	 *
	 *******************/

void
convert_v2_to_v1 (netsnmp_variable_list * vars, netsnmp_pdu * template_pdu)
{
	netsnmp_variable_list *v, *trap_v = NULL, *ent_v = NULL;
	oid trap_prefix[] = { SNMPV2_TRAPS_PREFIX };
	int len;

	for (v = vars; v; v = v->next_variable)
	{
		if (netsnmp_oid_equals (v->name, v->name_length,
					snmptrap_oid,
					OID_LENGTH (snmptrap_oid)) == 0)
			trap_v = v;
		if (netsnmp_oid_equals (v->name, v->name_length,
					snmptrapenterprise_oid,
					OID_LENGTH (snmptrapenterprise_oid))
		    == 0)
			ent_v = v;
	}

	if (!trap_v)
		return;		/* Can't find v2 snmpTrapOID varbind */

	/*
	 * Is this a 'standard' trap?
	 *  Or at least, does it have the correct prefix?
	 */
	if (netsnmp_oid_equals (trap_v->val.objid, OID_LENGTH (trap_prefix),
				trap_prefix, OID_LENGTH (trap_prefix)) == 0)
	{
		template_pdu->trap_type =
			trap_v->val.objid[OID_LENGTH (trap_prefix)] - 1;
		template_pdu->specific_type = 0;
	}
	else
	{
		len = trap_v->val_len / sizeof (oid);
		template_pdu->trap_type = 6;	/* enterprise specific */
		template_pdu->specific_type = trap_v->val.objid[len - 1];
	}

	/*
	 *  TODO:
	 *    Extract the appropriate enterprise value from 'ent_v'
	 *    Remove uptime/trapOID varbinds from 'vars' list
	 */

}

void
send_enterprise_trap_vars (int trap,
			   int specific,
			   oid * enterprise, int enterprise_length,
			   netsnmp_variable_list * vars)
{
	netsnmp_variable_list uptime_var, snmptrap_var, enterprise_var;
	netsnmp_variable_list lastDeniedIpAddr_var,aclViolations_var;
	netsnmp_variable_list cfgHistChangeTime_var,cfgHistChangeCount_var;
	netsnmp_variable_list *v2_vars, *last_var = NULL;
	netsnmp_pdu *template_pdu;
	u_long uptime;
	in_addr_t *pdu_in_addr_t;
	struct trap_sink *sink;
	oid temp_oid[MAX_OID_LEN];

	if (trap == SNMP_TRAP_COLDSTART && !my_enableColdStartTrap)
		return;
	else if (trap == SNMP_TRAP_AUTHFAIL && !my_enableAuthFailTrap)
		return;
	else if (trap == SNMP_TRAP_ACLFAIL && !my_enableAclFailTrap)
		return;
	else if (trap == SNMP_TRAP_CONFCHANGE && !my_enableConfChangeTrap)
		return;

	else if (trap == SNMP_TRAP_ETHACLVIO && !my_enableEthACLVio)
		return;
	else if (trap == SNMP_TRAP_FWUPGRADEFAIL && !my_enableFwUpgradeFail)
		return;
	else if (trap == SNMP_TRAP_CONFUPGRADEFAIL && !my_enableConfUpgradeFail)
		return;
	else if (trap == SNMP_TRAP_INVALIDTEXTCONF && !my_enableInvalidTextConf)
		return;
	else if (trap == SNMP_TRAP_TIMEADOPTFAIL && !my_enableTimeAdoptFail)
		return;
	else if (trap == SNMP_TRAP_RFTHRESHOLDTHRO && !my_enableRfThresholdThro)
		return;
	else if (trap == SNMP_TRAP_RFAVGRETRIES && !my_enableRfAvgRetries)
		return;
	else if (trap == SNMP_TRAP_PROCESSFAIL && !my_enableProcessFail)
		return;
	else;

	uptime = netsnmp_get_agent_uptime ();

	if(trap == SNMP_TRAP_ACLFAIL && vars==NULL)
	{
		vars=&lastDeniedIpAddr_var;
		memset (&lastDeniedIpAddr_var, 0, sizeof (netsnmp_variable_list));
		snmp_set_var_objid (&lastDeniedIpAddr_var, objid_ccSnmpLastDeniedIpAddr,
				    OID_LENGTH (objid_ccSnmpLastDeniedIpAddr));
		snmp_set_var_value (&lastDeniedIpAddr_var, (u_char *) & my_lastDeniedIpAddr,
				    sizeof (my_lastDeniedIpAddr));
		lastDeniedIpAddr_var.type = ASN_IPADDRESS;
		lastDeniedIpAddr_var.next_variable = &aclViolations_var;
		
		memset (&aclViolations_var, 0, sizeof (netsnmp_variable_list));
		snmp_set_var_objid (&aclViolations_var, objid_ccSnmpAclViolations,
				    OID_LENGTH (objid_ccSnmpAclViolations));
		snmp_set_var_value (&aclViolations_var, (u_char *) & my_aclViolations,
				    sizeof (my_aclViolations));
		aclViolations_var.type = ASN_COUNTER;
		aclViolations_var.next_variable = NULL;
	}
	if(trap == SNMP_TRAP_CONFCHANGE && vars==NULL)
	{
		vars=&cfgHistChangeCount_var;
		memset (&cfgHistChangeCount_var, 0, sizeof (netsnmp_variable_list));
                snmp_set_var_objid (&cfgHistChangeCount_var, objid_ccCfgHistChangeCount,
                                    OID_LENGTH (objid_ccCfgHistChangeCount));
                snmp_set_var_value (&cfgHistChangeCount_var, (u_char *) & my_cfgHistChangeCount,
                                    sizeof (my_cfgHistChangeCount));
                cfgHistChangeCount_var.type = ASN_COUNTER;
		cfgHistChangeCount_var.next_variable = &cfgHistChangeTime_var;

		memset (&cfgHistChangeTime_var, 0, sizeof (netsnmp_variable_list));
		snmp_set_var_objid (&cfgHistChangeTime_var, objid_ccCfgHistChangeTime,
				    OID_LENGTH (objid_ccCfgHistChangeTime));
		snmp_set_var_value (&cfgHistChangeTime_var, (u_char *) & uptime,
				    sizeof (uptime));
		cfgHistChangeTime_var.type = ASN_TIMETICKS;
		cfgHistChangeTime_var.next_variable = NULL;
	}

	/*
	 * Initialise SNMPv2 required variables
	 */
	
	memset (&uptime_var, 0, sizeof (netsnmp_variable_list));
	snmp_set_var_objid (&uptime_var, sysuptime_oid,
			    OID_LENGTH (sysuptime_oid));
	snmp_set_var_value (&uptime_var, (u_char *) & uptime,
			    sizeof (uptime));
	uptime_var.type = ASN_TIMETICKS;
	uptime_var.next_variable = &snmptrap_var;

	memset (&snmptrap_var, 0, sizeof (netsnmp_variable_list));
	snmp_set_var_objid (&snmptrap_var, snmptrap_oid,
			    OID_LENGTH (snmptrap_oid));
	/*
	 * value set later .... 
	 */
	snmptrap_var.type = ASN_OBJECT_ID;
	if (vars)
		snmptrap_var.next_variable = vars;
	else
		snmptrap_var.next_variable = &enterprise_var;

	/*
	 * find end of provided varbind list,
	 * ready to append the enterprise info if necessary 
	 */
	last_var = vars;
	while (last_var && last_var->next_variable)
		last_var = last_var->next_variable;

	memset (&enterprise_var, 0, sizeof (netsnmp_variable_list));
	snmp_set_var_objid (&enterprise_var,
			    snmptrapenterprise_oid,
			    OID_LENGTH (snmptrapenterprise_oid));
	snmp_set_var_value (&enterprise_var, (u_char *) enterprise,
			    enterprise_length * sizeof (oid));
	enterprise_var.type = ASN_OBJECT_ID;
	enterprise_var.next_variable = NULL;

	v2_vars = &uptime_var;

	/*
	 *  Create a template PDU, ready for sending
	 */
	template_pdu = snmp_pdu_create (SNMP_MSG_TRAP);
	if (template_pdu == NULL)
	{
		/*
		 * Free memory if value stored dynamically 
		 */
		snmp_set_var_value (&enterprise_var, NULL, 0);
		return;
	}
	template_pdu->trap_type = trap;
	template_pdu->specific_type = specific;
	if (snmp_clone_mem ((void **) &template_pdu->enterprise,
			    enterprise, enterprise_length * sizeof (oid)))
	{
		snmp_free_pdu (template_pdu);
		snmp_set_var_value (&enterprise_var, NULL, 0);
		return;
	}
	template_pdu->enterprise_length = enterprise_length;
	template_pdu->flags |= UCD_MSG_FLAG_FORCE_PDU_COPY;

	pdu_in_addr_t = (in_addr_t *) template_pdu->agent_addr;
	*pdu_in_addr_t = get_myaddr ();
	template_pdu->time = uptime;

	/*
	 *  Now use the parameters to determine
	 *    which v2 variables are needed,
	 *    and what values they should take.
	 */
	switch (trap)
	{
	case -1:		/*
				 *      SNMPv2 only
				 *  Check to see whether the variables provided
				 *    are sufficient for SNMPv2 notifications
				 */
		if (vars && netsnmp_oid_equals (vars->name, vars->name_length,
						sysuptime_oid,
						OID_LENGTH (sysuptime_oid)) ==
		    0)
			v2_vars = vars;
		else if (vars
			 && netsnmp_oid_equals (vars->name, vars->name_length,
						snmptrap_oid,
						OID_LENGTH (snmptrap_oid)) ==
			 0)
			uptime_var.next_variable = vars;
		else
		{
			/*
			 * Hmmm... we don't seem to have a value - oops! 
			 */
			snmptrap_var.next_variable = vars;
		}
		last_var = NULL;	/* Don't need enterprise info */
		convert_v2_to_v1 (vars, template_pdu);
		break;

		/*
		 * "Standard" SNMPv1 traps 
		 */

	case SNMP_TRAP_COLDSTART:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) cold_start_oid,
				    sizeof (cold_start_oid));
		break;
	case SNMP_TRAP_WARMSTART:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) warm_start_oid,
				    sizeof (warm_start_oid));
		break;
	case SNMP_TRAP_LINKDOWN:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) link_down_oid,
				    sizeof (link_down_oid));
		break;
	case SNMP_TRAP_LINKUP:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) link_up_oid,
				    sizeof (link_up_oid));
		break;
	case SNMP_TRAP_AUTHFAIL:
		if (snmp_enableauthentraps ==
		    SNMP_AUTHENTICATED_TRAPS_DISABLED)
		{
			snmp_free_pdu (template_pdu);
			snmp_set_var_value (&enterprise_var, NULL, 0);
			return;
		}
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) auth_fail_oid,
				    sizeof (auth_fail_oid));
		break;
	case SNMP_TRAP_EGPNEIGHBORLOSS:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) egp_xxx_oid,
				    sizeof (egp_xxx_oid));
		break;

	case SNMP_TRAP_ENTERPRISESPECIFIC:
		memcpy (temp_oid,
			(char *) enterprise,
			(enterprise_length) * sizeof (oid));
		temp_oid[enterprise_length] = 0;
		temp_oid[enterprise_length + 1] = specific;
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) temp_oid,
				    (enterprise_length + 2) * sizeof (oid));
		snmptrap_var.next_variable = vars;
		last_var = NULL;	/* Don't need version info */
		break;
	case SNMP_TRAP_ACLFAIL:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) acl_fail_oid,
				    sizeof (acl_fail_oid));
		break;
	case SNMP_TRAP_CONFCHANGE:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) config_change_oid,
				    sizeof (config_change_oid));
		break;

	case SNMP_TRAP_ETHACLVIO:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) eth_acl_oid,
				    sizeof (eth_acl_oid));
		break;
	case SNMP_TRAP_FWUPGRADEFAIL:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) fw_upgrad_oid,
				    sizeof (fw_upgrad_oid));
		break;
	case SNMP_TRAP_CONFUPGRADEFAIL:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) config_update_oid,
				    sizeof (config_update_oid));
		break;
	case SNMP_TRAP_INVALIDTEXTCONF:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) text_config_oid,
				    sizeof (text_config_oid));
		break;
	case SNMP_TRAP_TIMEADOPTFAIL:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) time_fail_oid,
				    sizeof (time_fail_oid));
		break;
	case SNMP_TRAP_RFTHRESHOLDTHRO:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) rf_thro_oid,
				    sizeof (rf_thro_oid));
		break;
	case SNMP_TRAP_RFAVGRETRIES:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) rf_retries_oid,
				    sizeof (rf_retries_oid));
		break;
	case SNMP_TRAP_PROCESSFAIL:
		snmp_set_var_value (&snmptrap_var,
				    (u_char *) process_fail_oid,
				    sizeof (process_fail_oid));
		break;
	}


	/*
	 *  Now loop through the list of trap sinks,
	 *   sending an appropriately formatted PDU to each
	 */
	for (sink = sinks; sink; sink = sink->next)
	{
		if (sink->version == SNMP_VERSION_1 && trap == -1)
			continue;	/* Skip v1 sinks for v2 only traps */
		template_pdu->command = sink->pdutype;

		if (sink->version != SNMP_VERSION_1)
		{
			template_pdu->variables = v2_vars;
			if (last_var)
				last_var->next_variable = &enterprise_var;
		}
		else
			template_pdu->variables = vars;

		send_trap_to_sess (sink->sesp, template_pdu);

		if (sink->version != SNMP_VERSION_1 && last_var)
			last_var->next_variable = NULL;
	}

	/*
	 * send stuff to registered callbacks 
	 */
	/*
	 * v2 traps/informs 
	 */
	template_pdu->variables = v2_vars;
	if (last_var)
		last_var->next_variable = &enterprise_var;

	snmp_call_callbacks (SNMP_CALLBACK_APPLICATION,
			     SNMPD_CALLBACK_SEND_TRAP2, template_pdu);

	if (last_var)
		last_var->next_variable = NULL;

	/*
	 * v1 traps 
	 */
	template_pdu->command = SNMP_MSG_TRAP;
	template_pdu->variables = vars;

	snmp_call_callbacks (SNMP_CALLBACK_APPLICATION,
			     SNMPD_CALLBACK_SEND_TRAP1, template_pdu);

	/*
	 * Free memory if values stored dynamically 
	 */
	snmp_set_var_value (&enterprise_var, NULL, 0);
	snmp_set_var_value (&snmptrap_var, NULL, 0);
	/*
	 * Ensure we don't free anything we shouldn't 
	 */
	if (last_var)
		last_var->next_variable = NULL;
	template_pdu->variables = NULL;
	snmp_free_pdu (template_pdu);
}

/*
 * send_trap_to_sess: sends a trap to a session but assumes that the
 * pdu is constructed correctly for the session type. 
 */
void
send_trap_to_sess (netsnmp_session * sess, netsnmp_pdu * template_pdu)
{
	netsnmp_pdu *pdu;

	if (!sess || !template_pdu)
		return;

	DEBUGMSGTL (("trap", "sending trap type=%d, version=%d\n",
		     template_pdu->command, sess->version));

	if (sess->version == SNMP_VERSION_1 &&
	    (template_pdu->command == SNMP_MSG_TRAP2 ||
	     template_pdu->command == SNMP_MSG_INFORM))
		return;		/* Skip v1 sinks for v2 only traps */
	template_pdu->version = sess->version;
	pdu = snmp_clone_pdu (template_pdu);
	pdu->sessid = sess->sessid;	/* AgentX only ? */
	if (snmp_send (sess, pdu) == 0)
	{
		snmp_sess_perror ("snmpd: send_trap", sess);
		snmp_free_pdu (pdu);
	}
	else
	{
		snmp_increment_statistic (STAT_SNMPOUTTRAPS);
		snmp_increment_statistic (STAT_SNMPOUTPKTS);
	}
}

void
send_trap_vars (int trap, int specific, netsnmp_variable_list * vars)
{
	if (trap == SNMP_TRAP_ENTERPRISESPECIFIC)
		send_enterprise_trap_vars (trap, specific,
					   objid_enterprisetrap,
					   OID_LENGTH (objid_enterprisetrap),
					   vars);
	else
		send_enterprise_trap_vars (trap, specific, trap_version_id,
					   OID_LENGTH (trap_version_id),
					   vars);
}

void
send_easy_trap (int trap, int specific)
{
	send_trap_vars (trap, specific, NULL);
#ifdef SUPPORT_V3TRAP
	send_v3trap(trap);
#endif
}

void
send_v2trap (netsnmp_variable_list * vars)
{
	send_trap_vars (-1, -1, vars);
}

void
send_trap_pdu (netsnmp_pdu * pdu)
{
	send_trap_vars (-1, -1, pdu->variables);
}



	/*******************
	 *
	 * Config file handling
	 *
	 *******************/

void
snmpd_parse_config_authtrap (const char *token, char *cptr)
{
	int i;

	i = atoi (cptr);
	if (i == 0)
	{
		if (strcmp (cptr, "enable") == 0)
		{
			i = SNMP_AUTHENTICATED_TRAPS_ENABLED;
		}
		else if (strcmp (cptr, "disable") == 0)
		{
			i = SNMP_AUTHENTICATED_TRAPS_DISABLED;
		}
	}
	if (i < 1 || i > 2)
	{
		config_perror ("authtrapenable must be 1 or 2");
	}
	else
	{
		if (strcmp (token, "pauthtrapenable") == 0)
		{
			if (snmp_enableauthentrapsset < 0)
			{
				/*
				 * This is bogus (and shouldn't happen anyway) -- the value
				 * of snmpEnableAuthenTraps.0 is already configured
				 * read-only.  
				 */
				snmp_log (LOG_WARNING,
					  "ignoring attempted override of read-only snmpEnableAuthenTraps.0\n");
				return;
			}
			else
			{
				snmp_enableauthentrapsset++;
			}
		}
		else
		{
			if (snmp_enableauthentrapsset > 0)
			{
				/*
				 * This is bogus (and shouldn't happen anyway) -- we already
				 * read a persistent value of snmpEnableAuthenTraps.0, which
				 * we should ignore in favour of this one.  
				 */
				snmp_log (LOG_WARNING,
					  "ignoring attempted override of read-only snmpEnableAuthenTraps.0\n");
				/*
				 * Fall through and copy in this value.  
				 */
			}
			snmp_enableauthentrapsset = -1;
		}
		snmp_enableauthentraps = i;
	}
}

void
snmpd_parse_config_trapsink (const char *token, char *cptr)
{
	char tmpbuf[1024];
	char *sp, *cp, *pp = NULL;
	u_short sinkport;

	if (!snmp_trapcommunity)
		snmp_trapcommunity = strdup ("public");
	sp = strtok (cptr, " \t\n");
	cp = strtok (NULL, " \t\n");
	if (cp)
		pp = strtok (NULL, " \t\n");
	if (cp && pp)
	{
		sinkport = atoi (pp);
		if ((sinkport < 1) || (sinkport > 0xffff))
		{
			config_perror ("trapsink port out of range");
			sinkport = SNMP_TRAP_PORT;
		}
	}
	else
	{
		sinkport = SNMP_TRAP_PORT;
	}
	if (create_v1_trap_session (sp, sinkport,
				    cp ? cp : snmp_trapcommunity) == 0)
	{
		snprintf (tmpbuf, sizeof (tmpbuf),
			  "cannot create trapsink: %s", cptr);
		tmpbuf[sizeof (tmpbuf) - 1] = '\0';
		config_perror (tmpbuf);
	}
}


void
snmpd_parse_config_trap2sink (const char *word, char *cptr)
{
	char tmpbuf[1024];
	char *sp, *cp, *pp = NULL;
	u_short sinkport;

	if (!snmp_trapcommunity)
		snmp_trapcommunity = strdup ("public");
	sp = strtok (cptr, " \t\n");
	cp = strtok (NULL, " \t\n");
	if (cp)
		pp = strtok (NULL, " \t\n");
	if (cp && pp)
	{
		sinkport = atoi (pp);
		if ((sinkport < 1) || (sinkport > 0xffff))
		{
			config_perror ("trapsink port out of range");
			sinkport = SNMP_TRAP_PORT;
		}
	}
	else
	{
		sinkport = SNMP_TRAP_PORT;
	}
	if (create_v2_trap_session (sp, sinkport,
				    cp ? cp : snmp_trapcommunity) == 0)
	{
		snprintf (tmpbuf, sizeof (tmpbuf),
			  "cannot create trap2sink: %s", cptr);
		tmpbuf[sizeof (tmpbuf) - 1] = '\0';
		config_perror (tmpbuf);
	}
}

void
snmpd_parse_config_informsink (const char *word, char *cptr)
{
	char tmpbuf[1024];
	char *sp, *cp, *pp = NULL;
	u_short sinkport;

	if (!snmp_trapcommunity)
		snmp_trapcommunity = strdup ("public");
	sp = strtok (cptr, " \t\n");
	cp = strtok (NULL, " \t\n");
	if (cp)
		pp = strtok (NULL, " \t\n");
	if (cp && pp)
	{
		sinkport = atoi (pp);
		if ((sinkport < 1) || (sinkport > 0xffff))
		{
			config_perror ("trapsink port out of range");
			sinkport = SNMP_TRAP_PORT;
		}
	}
	else
	{
		sinkport = SNMP_TRAP_PORT;
	}
	if (create_v2_inform_session (sp, sinkport,
				      cp ? cp : snmp_trapcommunity) == 0)
	{
		snprintf (tmpbuf, sizeof (tmpbuf),
			  "cannot create informsink: %s", cptr);
		tmpbuf[sizeof (tmpbuf) - 1] = '\0';
		config_perror (tmpbuf);
	}
}

/*
 * this must be standardized somewhere, right? 
 */
#define MAX_ARGS 128

static int traptype;

static void
trapOptProc (int argc, char *const *argv, int opt)
{
	switch (opt)
	{
	case 'C':
		while (*optarg)
		{
			switch (*optarg++)
			{
			case 'i':
				traptype = SNMP_MSG_INFORM;
				break;
			default:
				config_perror
					("unknown argument passed to -C");
				break;
			}
		}
		break;
	}
}


void
snmpd_parse_config_trapsess (const char *word, char *cptr)
{
	char *argv[MAX_ARGS], *cp = cptr, tmp[SPRINT_MAX_LEN];
	int argn, arg;
	netsnmp_session session, *ss;

	/*
	 * inform or trap?  default to trap 
	 */
	traptype = SNMP_MSG_TRAP2;

	/*
	 * create the argv[] like array 
	 */
	argv[0] = strdup ("snmpd-trapsess");	/* bogus entry for getopt() */
	for (argn = 1; cp && argn < MAX_ARGS; argn++)
	{
		cp = copy_nword (cp, tmp, SPRINT_MAX_LEN);
		argv[argn] = strdup (tmp);
	}

	arg = snmp_parse_args (argn, argv, &session, "C:", trapOptProc);
	ss = snmp_open (&session);

	for (; argn > 0; argn--)
	{
		free (argv[argn - 1]);
	}

	if (!ss)
	{
		config_perror
			("snmpd: failed to parse this line or the remote trap receiver is down.  Possible cause:");
		snmp_sess_perror ("snmpd: snmpd_parse_config_trapsess()",
				  &session);
		return;
	}

	if (ss->version == SNMP_VERSION_1)
	{
		add_trap_session (ss, SNMP_MSG_TRAP, 0, SNMP_VERSION_1);
	}
	else
	{
		add_trap_session (ss, traptype, (traptype == SNMP_MSG_INFORM),
				  ss->version);
	}
}

void
snmpd_parse_config_trapcommunity (const char *word, char *cptr)
{
	if (snmp_trapcommunity != NULL)
	{
		free (snmp_trapcommunity);
	}
	snmp_trapcommunity = (char *) malloc (strlen (cptr) + 1);
	if (snmp_trapcommunity != NULL)
	{
		copy_nword (cptr, snmp_trapcommunity, strlen (cptr) + 1);
	}
}

void
snmpd_free_trapcommunity (void)
{
	if (snmp_trapcommunity)
	{
		free (snmp_trapcommunity);
		snmp_trapcommunity = NULL;
	}
}




int
my_snmp_input (int operation,
	    netsnmp_session * session,
	    int reqid, netsnmp_pdu * pdu, void *magic)
{
	return 1;
}

static int
my_init_session (netsnmp_session * session,
		 char *ip, int port, char *user, char *auth_level,
		 char *password)
{
	snmp_sess_init (session);
	session->version = SNMP_VERSION_3;
	session->securityName = user;
	session->securityNameLen = strlen (user);

	//if (!strcasecmp(optarg, "MD5")) {
	session->securityAuthProto = usmHMACMD5AuthProtocol;
	session->securityAuthProtoLen = USM_AUTH_PROTO_MD5_LEN;

/*
            } else if (!strcasecmp(optarg, "SHA")) {
                session->securityAuthProto = usmHMACSHA1AuthProtocol;
                session->securityAuthProtoLen = USM_AUTH_PROTO_SHA_LEN;
            }
*/
	if (!strcmp (auth_level, "noAuthNoPriv"))
		session->securityLevel = SNMP_SEC_LEVEL_NOAUTH;
	else if (!strcmp (auth_level, "authNoPriv"))
		session->securityLevel = SNMP_SEC_LEVEL_AUTHNOPRIV;
	else if (!strcmp (auth_level, "authPriv"))
		session->securityLevel = SNMP_SEC_LEVEL_AUTHPRIV;
	else
		return -1;
	session->securityAuthKeyLen = USM_AUTH_KU_LEN;
	if (generate_Ku (session->securityAuthProto,
			 session->securityAuthProtoLen,
			 (u_char *) password, strlen (password),
			 session->securityAuthKey,
			 &session->securityAuthKeyLen) != SNMPERR_SUCCESS)
	{
		snmp_perror ("sned v3 trap");
		fprintf (stderr,
			 "Error generating a key (Ku) from the supplied authentication pass phrase. \n");
		return -1;
	}
	session->peername = ip;
	session->remote_port = port;
	session->retries = 0;
	return 0;
}


/*change from snmptrap.c main josn*/
static int
my_send_v3trap (char *ip, int port, char *user, char *auth_level,
		char *password, int trap)
{
	netsnmp_session session, *ss;
	netsnmp_pdu *pdu, *response;
	oid name[MAX_OID_LEN];
	size_t name_length;
	int status;
	int exitval = 0;
	char csysuptime[20];
	u_long sysuptime;
	if(trap!=SNMP_TRAP_COLDSTART
		&&trap!=SNMP_TRAP_AUTHFAIL
		&&trap!=SNMP_TRAP_ACLFAIL
		&&trap!=SNMP_TRAP_CONFCHANGE)
	{
		return -1;
	}
	putenv (strdup ("POSIXLY_CORRECT=1"));
	
	if (trap == SNMP_TRAP_COLDSTART && !my_enableColdStartTrap)
		return;
	else if (trap == SNMP_TRAP_AUTHFAIL && !my_enableAuthFailTrap)
		return;
	else if (trap == SNMP_TRAP_ACLFAIL && !my_enableAclFailTrap)
		return;
	else if (trap == SNMP_TRAP_CONFCHANGE && !my_enableConfChangeTrap)
		return;
	else;

	sysuptime = netsnmp_get_agent_uptime ();
	
	if (my_init_session (&session, ip, port, user, auth_level, password))
	{
		printf("send v3 trap fial: my_init_session fail %s %d\n",__FILE__,__LINE__);
		return -1;
	}
	session.callback = my_snmp_input;
	session.callback_magic = NULL;
	netsnmp_ds_set_int (NETSNMP_DS_LIBRARY_ID,
			    NETSNMP_DS_LIB_DEFAULT_PORT, SNMP_TRAP_PORT);
	ss = snmp_open (&session);
	if (ss == NULL)
	{
		snmp_sess_perror ("send v3 trap fail:snmp_open fail", &session);
	//	printf("send v3 trap fail: can not open session %s %d\n",__FILE__,__LINE__);
		return -1;
	}
	pdu = snmp_pdu_create(SNMP_MSG_TRAP2);

	
	switch(trap)
	{
	case SNMP_TRAP_COLDSTART:
		snmp_pdu_add_variable(pdu, objid_sysuptime, OID_LENGTH (objid_sysuptime),
			ASN_TIMETICKS, (u_char *) & sysuptime, sizeof(long));
		snmp_pdu_add_variable(pdu, snmptrap_oid,   OID_LENGTH (snmptrap_oid),
			ASN_OBJECT_ID, (u_char *) cold_start_oid, sizeof(cold_start_oid));
		snmp_pdu_add_variable(pdu,snmptrapenterprise_oid,OID_LENGTH (snmptrapenterprise_oid),
			ASN_OBJECT_ID,(u_char*) trap_version_id,sizeof(trap_version_id));
		break;
	case  SNMP_TRAP_AUTHFAIL:
		snmp_pdu_add_variable(pdu, objid_sysuptime, OID_LENGTH (objid_sysuptime),
			ASN_TIMETICKS, (u_char *) & sysuptime, sizeof(long));
		snmp_pdu_add_variable(pdu, snmptrap_oid,   OID_LENGTH (snmptrap_oid),
			ASN_OBJECT_ID, (u_char *) auth_fail_oid, sizeof(auth_fail_oid));
		snmp_pdu_add_variable(pdu,snmptrapenterprise_oid,OID_LENGTH (snmptrapenterprise_oid),
			ASN_OBJECT_ID,(u_char*) trap_version_id,sizeof(trap_version_id));
		break;
	case SNMP_TRAP_ACLFAIL:
		snmp_pdu_add_variable(pdu, objid_sysuptime, OID_LENGTH (objid_sysuptime),
			ASN_TIMETICKS, (u_char *) & sysuptime, sizeof(long));
		snmp_pdu_add_variable(pdu, snmptrap_oid,   OID_LENGTH (snmptrap_oid),
			ASN_OBJECT_ID, (u_char *) acl_fail_oid, sizeof(acl_fail_oid));
		snmp_pdu_add_variable(pdu, objid_ccSnmpLastDeniedIpAddr,   OID_LENGTH (objid_ccSnmpLastDeniedIpAddr),
			ASN_IPADDRESS, (u_char *) &my_lastDeniedIpAddr, sizeof(my_lastDeniedIpAddr));
		snmp_pdu_add_variable(pdu, objid_ccSnmpAclViolations,   OID_LENGTH (objid_ccSnmpAclViolations),
			ASN_COUNTER, (u_char *) &my_aclViolations, sizeof(my_aclViolations));
		snmp_pdu_add_variable(pdu,snmptrapenterprise_oid,OID_LENGTH (snmptrapenterprise_oid),
                        ASN_OBJECT_ID,(u_char*) trap_version_id,sizeof(trap_version_id));	
		break;
	case SNMP_TRAP_CONFCHANGE:
		snmp_pdu_add_variable(pdu, objid_sysuptime, OID_LENGTH (objid_sysuptime),
			ASN_TIMETICKS, (u_char *) & sysuptime, sizeof(long));
		snmp_pdu_add_variable(pdu, snmptrap_oid,   OID_LENGTH (snmptrap_oid),
			ASN_OBJECT_ID, (u_char *) config_change_oid, sizeof(config_change_oid));
		snmp_pdu_add_variable(pdu, objid_ccCfgHistChangeTime,   OID_LENGTH (objid_ccCfgHistChangeTime),
			ASN_TIMETICKS, (u_char *) &sysuptime, sizeof(sysuptime));
		snmp_pdu_add_variable(pdu, objid_ccCfgHistChangeCount,   OID_LENGTH (objid_ccCfgHistChangeCount),
			ASN_COUNTER, (u_char *) &my_cfgHistChangeCount, sizeof(my_cfgHistChangeCount));
		snmp_pdu_add_variable(pdu,snmptrapenterprise_oid,OID_LENGTH (snmptrapenterprise_oid),
                        ASN_OBJECT_ID,(u_char*) trap_version_id,sizeof(trap_version_id));
		break;
	default:
		printf("unknow v3 trap type %s %d\n",__FILE__,__LINE__);
		break;
	}
	if(snmp_send (ss, pdu)==0)
	{	
		printf("send v3 trap fail: snmp_send fail %s %d",__FILE__,__LINE__);
		snmp_free_pdu (pdu);
		exitval = 1;
	}
	snmp_close (ss);
	return exitval;
}


void
send_v3trap (int trap)
{
	char *ip = "172.17.149.142";
	int port = 162;
	char *auth_level = "authNoPriv";
	char *user = "josn";
	char *password = "password";
	my_isSkipCheckACL=1;
	my_send_v3trap (ip, port, user, auth_level, password, trap);
	my_isSkipCheckACL=0;
}

