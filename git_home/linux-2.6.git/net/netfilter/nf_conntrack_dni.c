#include <linux/types.h>
#include <linux/netfilter.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <linux/stddef.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/jhash.h>
#include <linux/err.h>
#include <linux/percpu.h>
#include <linux/moduleparam.h>
#include <linux/notifier.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/socket.h>
#include <linux/mm.h>
#include <linux/rculist_nulls.h>
#include <linux/in.h>

#include <net/ip.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_nat.h>
#include <net/netfilter/nf_nat_core.h>
#include <net/netfilter/nf_nat.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter/xt_tcpudp.h>

#define MAX_RULES_FOR_TRAFFIC_TO_ROUTER 4
#define NAT_LAN_HASH_SIZE      (256)
#define NAT_LAN_HASH_MASK      (NAT_LAN_HASH_SIZE - 1)

struct list_head *trigger_list_ptr = NULL;
EXPORT_SYMBOL(trigger_list_ptr);

/* Inclusive: host order */
extern u32 lan_ipv4_address;
/* Inclusive: host order */
extern u32 lan_ipv4_mask;
/* Inclusive: host order */
extern u32 wan_ipv4_address;
/* Inclusive: host order */
extern u32 second_wan_ipv4_address;
/* Inclusive: network order */
static u16 wan_traffic_ports[MAX_RULES_FOR_TRAFFIC_TO_ROUTER] = {0};
static int wan_traffic_rules = 0;

/* For count ipv6 in NAT traffic or not */
int sysctl_count_ipv6_in_nat = 0;

struct ipt_natinfo {
	struct xt_entry_target t;
	struct nf_nat_multi_range_compat mr;
};

#define LIST_FIND(head, cmpfn, type, args...)		\
({							\
	const struct list_head *__i, *__j = NULL;	\
							\
	list_for_each(__i, (head))			\
		if (cmpfn((const type)__i , ## args)) {	\
			__j = __i;			\
			break;				\
		}					\
	(type)__j;					\
 })

struct ipt_trigger_ports {
	u_int16_t mport[2];	/* Related destination port range */
	u_int16_t rport[2];	/* Port range to map related destination port range to */
};

struct ipt_trigger
{
	struct list_head list;	/* Trigger list */

	struct timer_list timeout;	/* Timer for list destroying */

	__u32 srcip;		/* Outgoing source address */
	__u16 mproto;	/* Trigger protocol */
	__u16 rproto;		/* Related protocol */

	struct ipt_trigger_ports ports;	/* Trigger and related ports */

	__u16 reply;                 /* Confirm a reply connection */
	__u16 trigger_timeout;
};

#define ASSERT_READ_LOCK(x)

enum ipt_conenat_step
{
	IPT_CONENAT_DNAT = 1,
	IPT_CONENAT_IN = 2,
};
enum ipt_conenat_type
{
	IPT_CONENAT_FULL = 1,
	IPT_CONENAT_RESTRICT = 2,
};

struct ipt_conenat_info
{
	enum ipt_conenat_step step;
	enum ipt_conenat_type type;
};

#define PORT_TCP	IPPROTO_TCP /*  6, 00110 in HEX */
#define PORT_UDP	IPPROTO_UDP /* 17, 10001 in HEX */
#define PORT_BOTH	(PORT_TCP | PORT_UDP) /* 23, 10111 in HEX */

struct high_port_item {
	char *name;

	u16 start, end;

	unsigned int proto;
};

struct high_port_item ports_table[] = {
	{"PPTP", 1723, 1723, PORT_TCP},
	{"L2TP", 1701, 1701, PORT_UDP},
	{"H.323", 1720, 1720, PORT_TCP},
	{"SIP", 5060, 5060, PORT_BOTH},
	{"AIM", 5190, 5190, PORT_BOTH},

	{"NetMeeting", 389, 389, PORT_TCP},
	{"NetMeeting", 522, 522, PORT_TCP},
	{"NetMeeting", 1503, 1503, PORT_TCP},
	{"NetMeeting", 1720, 1720, PORT_TCP},
	{"NetMeeting", 1731, 1731, PORT_TCP},

	{"Yahoo Messenger instant messenging", 5050, 5050, PORT_TCP},
	{"Yahoo Messenger voice chat", 5000, 5010, PORT_BOTH},
	{"Yahoo Messenger video(Webcams)", 5100, 5100, PORT_BOTH},

	{"MSN instant messenging", 1863, 1863, PORT_TCP},
	{"MSN Whiteboard and Application Sharing", 1503, 1503, PORT_BOTH},
	{"MSN voice (computer to phone)", 2001, 2120, PORT_UDP},
	{"MSN voice (computer to phone)", 6801, 6801, PORT_UDP},
	{"MSN voice (computer to phone)", 6901, 6901, PORT_UDP},
	{"MSN file transfers", 6891, 6900, PORT_TCP},
	{"MSN voice communications (computer to computer)", 6901, 6901,
	 PORT_BOTH},

	{"Skype", 80, 80, PORT_TCP},
	{"Skype", 443, 443, PORT_TCP},

	{"Starcraft", 6112, 6112, PORT_BOTH},
	{"Kazaa", 1214, 1214, PORT_TCP},
	{"Gnutella", 6346, 6347, PORT_BOTH},

	{"BitTorrent", 6881, 6889, PORT_TCP},
	{"BitTorrent", 6969, 6969, PORT_TCP},

	{"QQ Server", 4000, 4000, PORT_TCP},
	{"QQ Client", 8000, 8000, PORT_TCP},

	{NULL, 0, 0, 0}		/* The End! */
};

/* Outbound refresh idle time is less than or equal to 10 minutes */
#define HIGH_PRIO_IDLE_TIME            (10 * 60 * HZ)

#define NAT_SESSION_DIFF_NUM   (50)

static int existing_ct_is_router_traffic(struct nf_conn *ct);
static int ct_is_ipv6_traffic(struct nf_conn *ct);

static inline unsigned int nat_lan_hash(__u32 addr)
{
	return ntohl(addr) & NAT_LAN_HASH_MASK;
}

/*
 * This function return the session number of given lan_ip address.
 * The given lan_ip should be in host order endian.
 */
unsigned int number_of_session(struct net *net, u32 lan_ip)
{
	struct nf_conntrack_tuple_hash *h;
	struct hlist_nulls_node *n;
	struct nf_conn *ct = NULL;
	struct nf_conntrack_tuple *orig;
	unsigned int i;
	unsigned int sessions_of_ip = 0;

	rcu_read_lock();
	for (i = 0; i < nf_conntrack_htable_size; i++) {
		hlist_nulls_for_each_entry_rcu(h, n, &net->ct.hash[i], hnnode) {
			/* we only want to deal DIR_ORIGINAL */
			if (NF_CT_DIRECTION(h) == IP_CT_DIR_ORIGINAL) {
				ct = nf_ct_tuplehash_to_ctrack(h);
				orig = &ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple;
				if (ct_is_ipv6_traffic(ct) || existing_ct_is_router_traffic(ct))
					continue;
				if (ntohl(orig->src.u3.ip) == lan_ip ||
				    ntohl(orig->dst.u3.ip) == lan_ip) {
					sessions_of_ip++;
				}
			}
		}
	}
	rcu_read_unlock();

	return sessions_of_ip;
}

/*
 * This function return lan_ip_max (u32 in host endian).
 * And for parameter sessions_of_ip_max(int pointer), it puts the session
 * numbers of lan_ip_max.
 *
 */
u32 get_lan_ip_max_from_conntracks(struct net *net,
				   unsigned int *sessions_of_ip_max)
{
	struct nf_conntrack_tuple_hash *h;
	struct hlist_nulls_node *n;
	struct nf_conn *ct = NULL;
	struct list_head nat_lan[NAT_LAN_HASH_SIZE];
	u32 lan_ip;
	unsigned int hash;
	unsigned int i;
	struct nf_conn_lan *lan, *pos, *lan_max;

	lan = pos = lan_max = NULL;
	hash = 0;

	for (i = 0; i < NAT_LAN_HASH_SIZE; i++) {
		INIT_LIST_HEAD(&nat_lan[i]);
	}

	rcu_read_lock();
	for (i = 0; i < nf_conntrack_htable_size; i++) {
		hlist_nulls_for_each_entry_rcu(h, n, &net->ct.hash[i], hnnode) {
			/* we only want to deal DIR_ORIGINAL */
			if (NF_CT_DIRECTION(h) == IP_CT_DIR_ORIGINAL) {
				ct = nf_ct_tuplehash_to_ctrack(h);
				/* Ignore ipv6's conntrack and router's conntrack in conntrack table. */
				if (ct_is_ipv6_traffic(ct) || existing_ct_is_router_traffic(ct))
					continue;
				/*
				 * SNAT:lan_ip is src ip of ORIGINAL direction
				 * DNAT:lan_ip is src ip of REPLY direction
				 */
				lan_ip = (ct->status & IPS_SRC_NAT) ?
					 ct->tuplehash[IP_CT_DIR_ORIGINAL].
					 tuple.src.u3.ip :
					 ct->tuplehash[IP_CT_DIR_REPLY].
					 tuple.src.u3.ip;

				hash = nat_lan_hash(lan_ip);
				list_for_each_entry(pos, &nat_lan[hash],
						    list) {
					/* Both are network order */
					if (lan_ip == pos->ipaddr) {
						lan = pos;
						break;
					}
				}

				if (lan == NULL) {
					lan = kmem_cache_alloc(
							nf_ct_natlan_cachep,
							GFP_ATOMIC);

					if (lan == NULL)
						return 0;

					lan->ipaddr = lan_ip;
					lan->count = 1;
					list_add(&lan->list, &nat_lan[hash]);
				} else {
					lan->count++;
				}

				if ((lan_max == NULL) ||
				    (lan_max != lan &&
				     lan->count > lan_max->count)) {
					lan_max = lan;
				}
			}
		}
	}
	rcu_read_unlock();

	if(lan_max == NULL){ //Maybe we could not find any candidate conntrack for that.
		*sessions_of_ip_max = 0;
		return 0;
	}

	*sessions_of_ip_max = lan_max->count;

	return ntohl(lan_max->ipaddr);
}

/*
 * This function scan current NAT Table rules, and find rule that its target is
 * "REDIRECT", IP proto is TCP and redirect to port "80" ,"21" or "443".
 * ("21" for FTP Access USB storage, "80" for HTTP Access USB storage or
 * Remote Management and "443" for HTTPS access USB storage.)
 * It would record the port as traffic destined to router port.
 * e.g.
 * REDIRECT   tcp  --  * * 0.0.0.0/0 0.0.0.0/0 tcp dpt:8000 redir ports 80
 * REDIRECT   tcp  --  * * 0.0.0.0/0 0.0.0.0/0 tcp dpt:4430 redir ports 443
 * REDIRECT   tcp  --  * * 0.0.0.0/0 0.0.0.0/0 tcp dpt:2100 redir ports 21
 * REDIRECT   tcp  --  * * 0.0.0.0/0 0.0.0.0/0 tcp dpt:8443 redir ports 80
 *
 * This function would record 8000, 4430, 2100 and 8443 as traffic destined to
 * router ports and save the to wan_traffic_ports.
 */
void get_wan_traffic_destined_to_router_ports(struct net *net)
{
	void *table_base;
	struct ipt_entry *e;
	struct ipt_entry_target *target;
	struct ipt_natinfo *info;
	struct nf_nat_range *r;
	struct ipt_entry_match *match;
	struct xt_tcp *tcp;
	u16 target_port;
	unsigned int off;
	int i,table_size;

	table_size = net->ipv4.nat_table->private->size;
	table_base = net->ipv4.nat_table->private->entries[smp_processor_id()];

	wan_traffic_rules = 0;
	for (i = 0; i < MAX_RULES_FOR_TRAFFIC_TO_ROUTER; i++) {
		wan_traffic_ports[i] = 0;
	}

	/* Iterately check nat rules to acceptable wan traffic ports */

	for (off = 0; off < table_size; off += e->next_offset) {
		e = (struct ipt_entry *)(table_base + off);

		target = ipt_get_target(e);

		if ((strcmp(target->u.kernel.target->name, "REDIRECT") == 0)
		    && (e->ip.proto == IPPROTO_TCP)) {
			info = (const void *)target;
			r = &info->mr.range[0];

			if (r->flags & IP_NAT_RANGE_PROTO_SPECIFIED) {
				target_port = ntohs(r->min.tcp.port);
				if (target_port == 80
				    || target_port == 21
				    || target_port == 443) {
					match = (void *)e +
					    sizeof(struct ipt_entry);
					tcp = (struct xt_tcp *)match->data;

					/* wan_traffic_ports is network order */
					wan_traffic_ports[wan_traffic_rules++]
					    = tcp->dpts[0];
				}
			}
		}
	}

}
EXPORT_SYMBOL(get_wan_traffic_destined_to_router_ports);

/*
 * return value 1: this tuple is initiated from inside (LAN)
 * return value 0: this tuple is initiated from outside (WAN)
 */
int tuple_initiated_from_inside(const struct nf_conntrack_tuple *tuple)
{
	/* wan_ipv4_address is host order
	 * second_wan_ipv4_address is host order
	 * tuple->dst.u3.ip is network order
	 */
	if (ntohl(tuple->dst.u3.ip) == wan_ipv4_address
	    || ntohl(tuple->dst.u3.ip) == second_wan_ipv4_address)
		return 0;
	else
		return 1;
}

/*
 * return value 1: this tuple is to or from router
 * return value 0: this tuple is not to or from router
 */
int tuple_is_router_traffic(const struct nf_conntrack_tuple *tuple)
{
	/* We don't need to take firewall rules into consideration, for we don't need to know whether is high priority or not here. 
	 * So I remove the remote management checking from tuple_is_router_traffic(). By the way, Spec doesn't require us to handle 
	 * remote management rule in NAT management. */
	if(ntohl(tuple->src.u3.ip) == lan_ipv4_address //DUT->LAN
	   || ntohl(tuple->src.u3.ip) == wan_ipv4_address //DUT->WAN
	   || ntohl(tuple->src.u3.ip) == second_wan_ipv4_address //DUT->WAN
	   || ntohl(tuple->dst.u3.ip) == lan_ipv4_address //LAN->DUT
	   || ntohl(tuple->dst.u3.ip) == wan_ipv4_address //WAN->DUT
	   || ntohl(tuple->dst.u3.ip) == second_wan_ipv4_address //WAN->DUT
	   || ntohl(tuple->dst.u3.ip) == INADDR_LOOPBACK) //Loopback traffic
		return 1;

	if(ntohl((tuple->src.u3.ip) & lan_ipv4_mask) != (lan_ipv4_address & lan_ipv4_mask))//Other traffic belongs to Router.
		return 1;
	return 0;
}

/*
 * return value 1: this ct is belng traffic destined/initiated to/from router.
 * return value 0: this ct is not belong to traffic of router.
 */
static int existing_ct_is_router_traffic(struct nf_conn *ct)
{
	/* We can simply use this method to judge whether is existing NAT conntrack or not. */
	if (!ct_is_ipv6_traffic(ct) && ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u3.ip == ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip
	    &&  ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip == ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip)
		/* LAN<->DUT & WAN<->DUT & DUT<->DUT & other router's sessions */
		return 1;
	return 0;
}

/*
 * Return how many conntracks should not be counted in.
 * Per Spec, all traffic (or connections) destined/initiated to/from the router
 * should not be include in NAT sessions.
 */
static int number_of_router_traffic(struct net *net)
{
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct = NULL;
	struct hlist_nulls_node *n;
	unsigned int i;
	int router_traffic_count = 0;

	rcu_read_lock();
	for (i = 0; i < nf_conntrack_htable_size; i++) {
		hlist_nulls_for_each_entry_rcu(h, n, &net->ct.hash[i], hnnode) {
			/* we only want to deal DIR_ORIGINAL */
			if (NF_CT_DIRECTION(h) == IP_CT_DIR_ORIGINAL) {
				ct = nf_ct_tuplehash_to_ctrack(h);
				if (existing_ct_is_router_traffic(ct)) {
					router_traffic_count++;
				}
			}
		}
	}
	rcu_read_unlock();

	return router_traffic_count;
}

/*
 * return value 1: skb is IPv6 packet
 * return value 0: skb is not IPv6 packet
 */
int skb_is_ipv6_packet(struct sk_buff *skb)
{
	/* When Count IPv6 in NAT, we saw IPv6 packet as IPv4 packet for NAT */
	if (sysctl_count_ipv6_in_nat == 1)
		return 0;

	struct iphdr *ip;
	ip = ip_hdr(skb);

	if (ip->version == 6)
		return 1;
	else
		return 0;
}

/*
 * return value 1: this ct is ipv6 traffic
 * return value 0: this ct is not ipv6 traffic
 */
static int ct_is_ipv6_traffic(struct nf_conn *ct)
{
	if (nf_ct_l3num(ct) == AF_INET6)
		return 1;
	else
		return 0;
}

/*
 * Return how many ipv6 conntracks should not be counted in.
 *
 * In nf_connntrack, it's nf_conntrack_count include ipv4 & ipv6 traffic.
 * Per Spec, we are not sure if we should cont ipv6 in NAT traffic now.
 * Using sysctl_count_ipv6_in_nat to control this behavior.
 * Default behavior is "do not count ipv6 in NAT traffic"
 * And this function would return how many ipv6 traffic should be taken out
 * when we check if NAT conntrack is full.
 */
static int number_of_ipv6_traffic(struct net *net)
{
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct = NULL;
	struct hlist_nulls_node *n;
	unsigned int i;
	int ipv6_traffic_count = 0;

	/* We check number of ipv6 conntrack on when support nf_conntrack_ipv6 */
#if defined(CONFIG_NF_CONNTRACK_IPV6) || defined(CONFIG_NF_CONNTRACK_IPV6_MODULE)

	/* When Count IPv6 in NAT, no need to count ipv6 traffic numbers*/
	if (sysctl_count_ipv6_in_nat == 1)
		return 0;

	rcu_read_lock();
	for (i = 0; i < nf_conntrack_htable_size; i++) {
		hlist_nulls_for_each_entry_rcu(h, n, &net->ct.hash[i], hnnode) {
			/* we only want to deal DIR_ORIGINAL */
			if (NF_CT_DIRECTION(h) == IP_CT_DIR_ORIGINAL) {
				ct = nf_ct_tuplehash_to_ctrack(h);
				if (ct_is_ipv6_traffic(ct)) {
					ipv6_traffic_count++;
				}
			}
		}
	}
	rcu_read_unlock();
#endif

	return ipv6_traffic_count;
}

/*
 * return value 1: conntrack is still full after minus all traffic of router itself.
 * return value 0: conntrack is not full after minus all traffic of router itself.
 */
int conntrack_table_really_full(struct net *net)
{
	if (nf_conntrack_max
	    && ((atomic_read(&net->ct.count) - number_of_router_traffic(net)
		 - number_of_ipv6_traffic(net)) > nf_conntrack_max)) {
		return 1;
	} else {
		return 0;
	}
}

static inline int trigger_in_matched(const struct ipt_trigger *i, __u16 proto,
				     __u16 dport)
{
	__u16 rproto = i->rproto ? : proto;
	return
	    rproto == proto &&
	    i->ports.rport[0] <= dport && i->ports.rport[1] >= dport;
}

/*
 * return value 1: given tuple would match one trigger rule.
 * return value 0: given tuple would not match any trigger rule.
 */
static int tuple_matches_trigger_rules(struct nf_conntrack_tuple *orig)
{
	struct ipt_trigger *trig;

	/* Check if the trigger-ed range has already existed in 'trigger_list'. */
	trig = LIST_FIND(trigger_list_ptr,
			 trigger_in_matched,
			 struct ipt_trigger *, orig->dst.protonum,
			 ntohs(orig->dst.u.all));

	if (trig == NULL || trig->srcip == 0)
		return 0;

	return 1;
}

/*
 * If the given tuple "orig" matched any trigger rule, this function returns
 * the corresponded Lan IP of "orig"(u32 in host endian).
 *
 * return value 0: tuple did not match any trigger rule.
 */
static u32 get_lan_ip_from_TRIGGER_target(struct nf_conntrack_tuple *orig)
{
	struct ipt_trigger *trig;

	/* Check if the trigger-ed range has already existed in 'trigger_list'. */
	trig = LIST_FIND(trigger_list_ptr,
			 trigger_in_matched,
			 struct ipt_trigger *, orig->dst.protonum,
			 ntohs(orig->dst.u.all));

	if (trig == NULL || trig->srcip == 0)
		return 0;

	return ntohl(trig->srcip);
}

/*
 * This function should not be generally used.
 * At least, it is for "net-wall" DNAT rules only now.
 * It checks if given tuple could match the given DNAT ipt_entry
 *
 * return value 1:  tuple matched given DNAT ipt_entry.
 *
 * return value 0:  tuple did not match given DNAT ipt_entry.
 */
static int tuple_matches_DNAT_entry(struct ipt_entry *e,
				    struct nf_conntrack_tuple *tuple)
{
	const struct ipt_entry_match *m;
	const struct xt_tcp *tcp;
	const struct xt_udp *udp;

	if (e->ip.proto == tuple->dst.protonum) {
		if (e->ip.proto == IPPROTO_TCP) {
			/* TCP DNAT rules */
			m = (void *)e + sizeof(struct ipt_entry);
			tcp = (struct xt_tcp *)m->data;

			if (htons(tuple->dst.u.tcp.port) == htons(tcp->dpts[0])
			    && !(e->ip.invflags & IPT_INV_PROTO))

				return 1;

		} else if (e->ip.proto == IPPROTO_UDP) {
			/* UDP DNAT rules */
			m = (void *)e + sizeof(struct ipt_entry);
			udp = (struct xt_udp *)m->data;

			if (htons(tuple->dst.u.udp.port) == htons(udp->dpts[0])
			    && !(e->ip.invflags & IPT_INV_PROTO))
				return 1;

		} else if (e->ip.proto == IPPROTO_ICMP) {
			/* In current DMZ rule, it uses "! -p icmp" to match */
			if (e->ip.invflags & IPT_INV_PROTO)
				return 1;
		}
	}

	return 0;
}

/*
 * When tuple is initiated from outside (WAN), scan netfilter NAT table for
 * possible DNAT rules (e.g. Port Forwarding, or UPnP Port Mapping rules.
 *
 * return value 1:  this tuple matched DNAT target rule,
 *	            and lan_ip would save the corresponded LAN IP.
 *
 * return value 0:  this tuple did not match any DNAT target rule,
 *                  and set lan_ip as "0".
 */
static int tuple_matches_DNAT_rules(struct net *net,
				    const struct nf_conntrack_tuple *tuple)
{
	struct xt_table *table;
	struct xt_table_info *private;
	void *table_base;
	struct ipt_entry *e;
	struct ipt_entry_target *target;
	unsigned int off;
	int table_size;

	table = net->ipv4.nat_table;
	private = table->private;
	table_size = private->size;
	table_base = private->entries[0];

	/* Iterately check if tuple could match any DNAT rules from net_wall */
	for (off = 0; off < table_size; off += e->next_offset) {
		e = (struct ipt_entry *)(table_base + off);

		target = ipt_get_target(e);
		if (strcmp(target->u.kernel.target->name, "DNAT") == 0) {
			if (tuple_matches_DNAT_entry(e, tuple) == 1)
				return 1;
		}

	}

	return 0;
}

/*
 * This function is written for net-wall Port Forwarding rules and other DNAT
 * target rules only.
 * For present rules, we do not allocate multi IP or assign proto for DNAT target.
 * We only assign one target IP in DNAT target in net-wall rules.
 *
 * Return the Lan IP address got from known DNAT target rule
 * Storing type u32(in host endian)
 */
static u32 get_lan_ip_from_DNAT_target(struct ipt_entry_target *target)
{
	const struct ipt_natinfo *info = (const void *)target;
	struct nf_nat_range *r = &info->mr.range[0];

	if (r->flags & IP_NAT_RANGE_MAP_IPS)
		return ntohl(r->min_ip);
	else
		return 0;
}

static inline int conenat_port_cmp(const struct nf_conntrack_tuple_hash *i,
				   const u_int16_t portmap,
				   const u_int32_t srcip, const void *targinfo)
{
	struct nf_conn *conntrack = nf_ct_tuplehash_to_ctrack(i);
	const struct ipt_conenat_info *info = targinfo;

	ASSERT_READ_LOCK(&ip_conntrack_lock);

	if (NF_CT_DIRECTION(i) != IP_CT_DIR_REPLY
	    || i->tuple.dst.protonum != IPPROTO_UDP)
		return 0;

	/* The connection SHOULD be initiated from LAN to Internet: SNAT-ed. */
	if (!(conntrack->status & IPS_SRC_NAT))
		return 0;

	if (info->type == IPT_CONENAT_FULL)
		return i->tuple.dst.u.all == portmap;
	else			/* if (info->type == IPT_CONENAT_RESTRICT) */
		return i->tuple.dst.u.all == portmap
		    && i->tuple.src.u3.ip == srcip;
}

static struct nf_conntrack_tuple_hash *conenat_port_used(struct net *net,
							 u_int16_t portmap,
							 u_int32_t srcip,
							 const void *targinfo)
{
	struct nf_conntrack_tuple_hash *h;
	struct hlist_nulls_node *n;
	int i;

	spin_lock_bh(&nf_conntrack_lock);

	for (i = 0; i < nf_conntrack_htable_size; i++) {
		hlist_nulls_for_each_entry(h, n, &net->ct.hash[i], hnnode) {
			if (conenat_port_cmp(h, portmap, srcip, targinfo)) {
				spin_unlock_bh(&nf_conntrack_lock);
				return h;
			}
		}
	}

	spin_unlock_bh(&nf_conntrack_lock);

	return NULL;
}

/*
 * return value 1: given tuple would be allowed by CONENAT.
 * return value 0: given tuple would not be allowed by CONENAT.
 */
static int tuple_allowed_by_CONNAT(struct net *net,
				   struct nf_conntrack_tuple *orig,
				   struct ipt_entry_target *t)
{

	struct nf_conntrack_tuple_hash *h;

	h = conenat_port_used(net, orig->dst.u.all, orig->src.u3.ip, t->data);

	if (h == NULL)
		return 0;
	else
		return 1;
}

/*
 * This function is written for net-wall CONENAT target rule only,
 * it is at net_dnat, PREROUTING of NAT table.
 *
 * Return the Lan IP address got from given CONENAT ipt_entry_target.
 * Storing type u32(in host endian)
 */
static u32 get_lan_ip_from_CONENAT(struct net *net,
				   struct nf_conntrack_tuple *orig,
				   struct ipt_entry_target *t)
{

	struct nf_conntrack_tuple_hash *h;

	h = conenat_port_used(net, orig->dst.u.all, orig->src.u3.ip, t->data);

	if (h == NULL)
		return 0;
	else
		return ntohl(h->tuple.src.u3.ip);
}

static inline struct ipt_entry *
get_entry(void *base, unsigned int offset)
{
	return (struct ipt_entry *)(base + offset);
}

static inline __pure
struct ipt_entry *ipt_next_entry(const struct ipt_entry *entry)
{
	return (void *)entry + entry->next_offset;
}

static inline bool
do_match(struct ipt_entry_match *m, const struct sk_buff *skb,
	 struct xt_match_param *par)
{
	par->match     = m->u.kernel.match;
	par->matchinfo = m->data;

	/* Stop iteration if it doesn't match */
	if (!m->u.kernel.match->match(skb, par))
		return true;
	else
		return false;
}

static inline bool
ip_packet_match(const struct iphdr *ip,
		const char *indev,
		const char *outdev,
		const struct ipt_ip *ipinfo,
		int isfrag)
{
	unsigned long ret;

#define FWINV(bool, invflg) ((bool) ^ !!(ipinfo->invflags & (invflg)))

	if (ipinfo->flags & IPT_F_NO_DEF_MATCH)
		return true;

	if (FWINV((ip->saddr&ipinfo->smsk.s_addr) != ipinfo->src.s_addr,
		  IPT_INV_SRCIP)
	    || FWINV((ip->daddr&ipinfo->dmsk.s_addr) != ipinfo->dst.s_addr,
		     IPT_INV_DSTIP)) {
		return false;
	}

	ret = ifname_compare_aligned(indev, ipinfo->iniface, ipinfo->iniface_mask);

	if (FWINV(ret != 0, IPT_INV_VIA_IN)) {
		return false;
	}

	ret = ifname_compare_aligned(outdev, ipinfo->outiface, ipinfo->outiface_mask);

	if (FWINV(ret != 0, IPT_INV_VIA_OUT)) {
		return false;
	}

	/* Check specific protocol */
	if (ipinfo->proto
	    && FWINV(ip->protocol != ipinfo->proto, IPT_INV_PROTO)) {
		return false;
	}

	/* If we have a fragment rule but the packet is not a fragment
	 * then we return zero */
	if (FWINV((ipinfo->flags&IPT_F_FRAG) && !isfrag, IPT_INV_FRAG)) {
		return false;
	}

#undef FWINV
	return true;
}

/*
 * This function would find the corresponded target entry of given skb from
 * NAT table PREROUTING chain.
 */
static struct ipt_entry_target *
nat_prerouting_final_entry_target_of_skb(struct net *net, struct sk_buff *skb,
					 struct nf_conntrack_tuple *orig)
{
	static const char nulldevname[IFNAMSIZ] __attribute__((aligned(sizeof(long))));
	const struct iphdr *ip;
	u_int16_t datalen;
	bool hotdrop = false;
	/* Initializing verdict to NF_DROP keeps gcc happy. */
	unsigned int verdict = NF_DROP;
	const char *indev, *outdev;
	void *table_base;
	struct ipt_entry *e, *back;
	struct xt_table_info *private;
	struct xt_match_param mtpar;
	struct xt_target_param tgpar;
	struct xt_table *table;
	unsigned int hook;
	struct net_device *in;
	struct net_device *out;
	struct ipt_entry_target *t;

	/* NAT table, PREROUTING chain, skb->dev is arrived on device of skb*/
	table = net->ipv4.nat_table;
	hook = NF_INET_PRE_ROUTING;
	in = skb->dev;
	out = NULL;
	t = NULL;

	/* Initialization */
	ip = ip_hdr(skb);
	datalen = skb->len - ip->ihl * 4;
	indev = in ? in->name : nulldevname;
	outdev = out ? out->name : nulldevname;
	/* We handle fragments by dealing with the first fragment as
	 * if it was a normal packet.  All other fragments are treated
	 * normally, except that they will NEVER match rules that ask
	 * things we don't know, ie. tcp syn flag or ports).  If the
	 * rule is also a fragment-specific rule, non-fragments won't
	 * match it. */
	mtpar.fragoff = ntohs(ip->frag_off) & IP_OFFSET;
	mtpar.thoff   = ip_hdrlen(skb);
	mtpar.hotdrop = &hotdrop;
	mtpar.in      = tgpar.in  = in;
	mtpar.out     = tgpar.out = out;
	mtpar.family  = tgpar.family = NFPROTO_IPV4;
	mtpar.hooknum = tgpar.hooknum = hook;

	xt_info_rdlock_bh();
	private = table->private;
	table_base = private->entries[smp_processor_id()];

	e = get_entry(table_base, private->hook_entry[hook]);

	/* For return from builtin chain */
	back = get_entry(table_base, private->underflow[hook]);

	do {

		if (!ip_packet_match(ip, indev, outdev,
		    &e->ip, mtpar.fragoff) ||
		    IPT_MATCH_ITERATE(e, do_match, skb, &mtpar) != 0) {
			e = ipt_next_entry(e);
			continue;
		}

		t = ipt_get_target(e);

		/* Standard target? */
		if (!t->u.kernel.target->target) {
			int v;

			v = ((struct ipt_standard_target *)t)->verdict;
			if (v < 0) {
				/* Pop from stack? */
				if (v != IPT_RETURN) {
					verdict = (unsigned)(-v) - 1;
					break;
				}
				e = back;
				back = get_entry(table_base, back->comefrom);
				continue;
			}
			if (table_base + v != ipt_next_entry(e)
			    && !(e->ip.flags & IPT_F_GOTO)) {
				/* Save old back ptr in next entry */
				struct ipt_entry *next = ipt_next_entry(e);
				next->comefrom = (void *)back - table_base;
				/* set back pointer to next entry */
				back = next;
			}

			e = get_entry(table_base, v);
			continue;
		}
		/*
		 * All packet would go into TRIGGER target, only some matched.
		 * Those "non-matched" would go continue.
		 */
		if (strcmp(t->u.kernel.target->name, "TRIGGER") == 0) {
			if (tuple_matches_trigger_rules(orig) == 0) {
				e = ipt_next_entry(e);
				continue;
			}
		} else if (strcmp(t->u.kernel.target->name, "CONENAT") == 0) {
			if (tuple_allowed_by_CONNAT(net, orig, t) == 0) {
				e = ipt_next_entry(e);
				continue;
			}
		}

		break;

	} while (!hotdrop);

	xt_info_rdunlock_bh();
	return t;
}

/*
 * This function would get the corresponding Lan IP address from given
 * original tuple.
 *
 * The returned Lan IP address would be stored in u32 type host endian.
 */

u32 lan_ip_of_wan_orig_tuple(struct net *net, struct sk_buff *skb,
			     const struct nf_conntrack_tuple *orig)
{
	struct ipt_entry_target *t;
	struct xt_target *target;
	u32 lan_ip_new = 0;

	t = nat_prerouting_final_entry_target_of_skb(net, skb, orig);
	if (t == NULL)
		return 0;

	target = t->u.kernel.target;

	if (strcmp(target->name, "TRIGGER") == 0)
		lan_ip_new = get_lan_ip_from_TRIGGER_target(orig);
	else if (strcmp(target->name, "CONENAT") == 0)
		lan_ip_new = get_lan_ip_from_CONENAT(net, orig, t);
	else if (strcmp(target->name, "DNAT") == 0)
		lan_ip_new = get_lan_ip_from_DNAT_target(t);
	else
		lan_ip_new = 0;

	return lan_ip_new;
}

/*
 * 1. The session is initiated from either the outside (i.e. from the WAN) or
 *    the inside (i.e. from the LAN) and it matches a Port Forwarding,
 *    Port Triggering, or UPnP Port Mapping rule.
 *
 * return value 1:  This orig tuple matched high-priority rule 1
 * return value 0:  This orig tuple did not match high-priority rule 1
 */
static int
match_port_forward_port_trigger_or_upnp(struct net *net,
					struct nf_conntrack_tuple *orig,
					struct nf_conntrack_tuple *repl)
{
	/* orig tuple is from inside, check via reply tuple */
	if (tuple_initiated_from_inside(orig) == 1) {
	    if (tuple_matches_DNAT_rules(net, repl) == 1
		|| tuple_matches_trigger_rules(repl) == 1) {
		    return 1;
	    }
	} else { /* orig tuple is from outside, check via orig tuple */
	    if (tuple_matches_DNAT_rules(net, orig) == 1
		|| tuple_matches_trigger_rules(orig) == 1) {
		    return 1;
	    }
	}

	return 0;
}

/*
 * 2. The session is initiated from the inside and its layer 4 protocol
 *    (i.e. the protocol above the IP) is not TCP or UDP.
 *
 * return value 1:  This orig tuple matched high-priority rule 2
 * return value 0:  This orig tuple did not match high-priority rule 2
 */
static int tuple_is_not_tcp_or_udp(struct nf_conntrack_tuple *orig)
{
	if (tuple_initiated_from_inside(orig) == 1) {
		if ((orig->dst.protonum != IPPROTO_TCP)
		    && (orig->dst.protonum != IPPROTO_UDP)) {
			return 1;
		}
	}
	return 0;
}

/*
 * 3. The session is initiated from the inside and its destination TCP/UDP port
 *    number is below 1024 (i.e. 1 ~ 1023).
 *
 * return value 1:  This orig tuple matched high-priority rule 3
 * return value 0:  This orig tuple did not match high-priority rule 3
 */
static int tuple_dst_port_is_below_1024(struct nf_conntrack_tuple *orig)
{
	if (tuple_initiated_from_inside(orig) == 1) {
		if (ntohs(orig->dst.u.all) < 1024){
			return 1;
		}
	}
	return 0;
}

/*
 * 4. The session is initiated from the inside and its destination TCP/UDP port
 *    number is one of the applications in the ports_table[]
 *
 * return value 1:  This orig tuple matched high-priority rule 4
 * return value 0:  This orig tuple did not match high-priority rule 4
 */
static int tuple_dst_port_matches_table(struct nf_conntrack_tuple *orig)
{
	u16 port;
	struct high_port_item *item;

	port = ntohs(orig->dst.u.all);
	if (tuple_initiated_from_inside(orig) == 1) {
		for (item = &ports_table[0]; item->name != NULL; item++) {
			if (((orig->dst.protonum & item->proto) != 0) &&
			    (port >= item->start) && (port <= item->end)) {
				return 1;
			}
		}
	}
	return 0;
}

/*
 * return value 1:  This orig tuple matched one high priority rule
 * return value 0:  This orig tuple did not match any high priority rules
 */
static int tuple_is_high_priority(struct net *net,
				  struct nf_conntrack_tuple *orig,
				  struct nf_conntrack_tuple *repl)
{
	if (match_port_forward_port_trigger_or_upnp(net, orig, repl)
	    || tuple_is_not_tcp_or_udp(orig)
	    || tuple_dst_port_is_below_1024(orig)
	    || tuple_dst_port_matches_table(orig)) {
		return 1;
	}

	return 0;
}

/*
 * return value 1:  ct matched one high priority rule
 * return value 0:  ct did not match any high priority rules
 */
static int conntrack_is_high_priority(struct net *net, struct nf_conn *ct)
{
	struct nf_conntrack_tuple *orig, *repl;
	extern unsigned int tcp_timeouts[];

	orig = &ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple;
	repl = &ct->tuplehash[IP_CT_DIR_REPLY].tuple;

	if (tuple_is_high_priority(net, orig, repl) == 1) {
		/*
		 * Only TCP timeout(1800) larger than HIGH_PRIO_IDLE_TIME(600)
		 * So we only check IPPROTO_TCP now.
		 */
		if ((orig->dst.protonum == IPPROTO_TCP)
		    && (tcp_timeouts[TCP_CONNTRACK_ESTABLISHED] -
		    (ct->timeout.expires - jiffies) > HIGH_PRIO_IDLE_TIME)) {
			printk("I am old enough, but I don't know whether I am the oldest!\n");
			return 0;
		}
		return 1;
	}

	return 0;
}

/*
 * This function would try to find a low-priority conntrack belong to lan_ip.
 * return NULL if it fails to find any low-priority conntrack.
 * Or return the low-priority conntrack of lan_ip.
 */
static struct nf_conn *find_lru_conntrack(struct net *net, u32 lan_ip)
{
	struct nf_conntrack_tuple_hash *h;
	struct nf_conn *ct = NULL;
	struct hlist_nulls_node *n;
	unsigned int i;
	/* addr is in host order */
	u32 addr;

	rcu_read_lock();
	for (i = 0; i < nf_conntrack_htable_size; i++) {
		hlist_nulls_for_each_entry_rcu(h, n, &net->ct.hash[i], hnnode) {
			/* we only want to deal DIR_ORIGINAL */
			if (NF_CT_DIRECTION(h) == IP_CT_DIR_ORIGINAL) {
				ct = nf_ct_tuplehash_to_ctrack(h);
				if (ct_is_ipv6_traffic(ct) || existing_ct_is_router_traffic(ct))
					continue;
				addr = ntohl((ct->status & IPS_SRC_NAT) ?
					     ct->tuplehash[IP_CT_DIR_ORIGINAL].
					     tuple.src.u3.ip :
					     ct->tuplehash[IP_CT_DIR_REPLY].
					     tuple.src.u3.ip);
				if (lan_ip == addr &&
				    !conntrack_is_high_priority(net, ct) &&
				    test_bit(IPS_ASSURED_BIT, &ct->status)) {
					rcu_read_unlock();
					printk("Victim conntrack:%u.%u.%u.%u:%u(%u.%u.%u.%u:%u) <==> %u.%u.%u.%u:%u(%u.%u.%u.%u:%u)\n", NIPQUAD(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip), ntohs(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.all), NIPQUAD(ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip), ntohs(ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u.all), NIPQUAD(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u3.ip), ntohs(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.all), NIPQUAD(ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip), ntohs(ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u.all));
					return ct;
				}
			}
		}
	}
	rcu_read_unlock();

	return NULL;
}

/*
 * Find a victim from given Lan IP address lan_ip_new.
 * If we fail to find any lower priority conntrack, return NULL.
 * Return address of victim if we find a victim.
 */
struct nf_conn *find_victim_from_conntracks(struct net *net,
					    struct nf_conntrack_tuple *orig,
					    struct nf_conntrack_tuple *repl,
					    u32 lan_ip_new, u32 lan_ip_max,
					    unsigned int sessions_of_ip_new,
					    unsigned int sessions_of_ip_max)
{
	struct nf_conn *victim = NULL;

	/* Rule 1: If NumNATSessMAX - NumNATSessNEW >= 50, identify the LRU
	 * (least recently used) low-priority session that belongs to LanIPMAX
	 * and replace it with the new session.
	 */
	if ((sessions_of_ip_max - sessions_of_ip_new) >= NAT_SESSION_DIFF_NUM)
		victim = find_lru_conntrack(net, lan_ip_max);

	/* Rule 1 fails, either
	 * NumNATSessMAX - NumNATSessNEW < 50 or
	 * no low priority conntrack belongs to lan_ip_max.
	 */
	if (victim == NULL) {
		if (tuple_is_high_priority(net, orig, repl) == 1)   /* Rule 2 */
			victim = find_lru_conntrack(net, lan_ip_new);
		else	/* Rule 3 */
			victim = NULL;
	}

	return victim;
}
