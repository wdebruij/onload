/****************************************************************************
 * Driver for Solarflare network controllers and boards
 * Copyright 2019 Solarflare Communications Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, incorporated herein by reference.
 */

#include "net_driver.h"

#ifndef EFX_TC_H
#define EFX_TC_H

#if !defined(EFX_USE_KCOMPAT) || defined(EFX_TC_OFFLOAD)
#include <linux/mutex.h>
#include <net/pkt_cls.h>
#include <net/tc_act/tc_tunnel_key.h>
#include <net/tc_act/tc_pedit.h>
#include "ef100_rep.h"

/* Using a single 4kB page for the counters DMA means limit of 256 counters.
 * XXX This is unsuitable for production, but so is MC_CMD_MAE_COUNTERS_READ.
 */
#define EFX_TC_MAX_COUNTER	256
struct efx_tc_counter {
	u64 packets, bytes;
	u64 old_packets, old_bytes; /* Values last time passed to userspace */
	/* jiffies of the last time we saw packets increase */
	unsigned long touched;
	/* owners of corresponding count actions
	 * must be last, see efx_tc_flower_allocate_counter()
	 */
	struct list_head users;
};

#define EFX_TC_MAX_ENCAP_HDR	128 /* made-up for now, fw will decide */
struct efx_tc_encap_action {
	enum efx_encap_type type;
	struct ip_tunnel_key key; /* 52 bytes */
	u32 dest_mport; /* is copied into struct efx_tc_action_set */
	u8 encap_hdr_len;
	bool n_valid;
	u8 encap_hdr[EFX_TC_MAX_ENCAP_HDR];
	struct efx_neigh_binder *neigh;
	struct list_head list; /* entry on neigh->users list */
	struct list_head users; /* action sets using this encap_md */
	struct rhash_head linkage; /* efx->tc_encap_ht */
	refcount_t ref;
	u32 fw_id; /* index of this entry in firmware encap table */
};

/* Multiple pedit actions are represented by means of a linked list of entries
 * in the pedit_actions table (in the driver, efx->tc_pedit_ht).
 */
struct efx_tc_pedit_action {
	u16 hdr_type; /* enum pedit_header_type; */
	u8 cmd; /* enum pedit_cmd; */
	/* 8 bits hole */
	struct tc_pedit_key key; /* 24 bytes */
	struct efx_tc_pedit_action *next;
	struct rhash_head linkage;
	refcount_t ref;
	u32 fw_id; /* index of this entry in firmware pedit table */
};

/* TODO flip this around when upstream has action_cookie */
#if defined(EFX_USE_KCOMPAT) && defined(EFX_HAVE_TC_ACTION_COOKIE)
struct efx_tc_counter_index {
	unsigned long cookie;
	struct rhash_head linkage;
	refcount_t ref;
	u32 fw_id;
};
#else
struct efx_tc_counter_index {
	u32 fw_id;
};
#endif

/* Driver-internal numbering scheme for vports.  See efx_tc_flower_lookup_dev() */
#define EFX_VPORT_PF		0
#define EFX_VPORT_VF_OFFSET	1

struct efx_tc_action_set {
	u16 vlan_push:2;
	u16 vlan_pop:2;
	u16 decap:1;
	u16 deliver:1;
	__be16 vlan_tci[2]; /* TCIs for vlan_push */
	__be16 vlan_proto[2]; /* Ethertypes for vlan_push */
	struct efx_tc_counter_index *count;
#if defined(EFX_USE_KCOMPAT) && !defined(EFX_HAVE_TC_FLOW_OFFLOAD)
	int count_action_idx;
#endif
	u32 dest_mport;
	struct efx_tc_encap_action *encap_md; /* entry in tc_encap_ht table */
	/* pedit is not currently supported, so this will always be NULL */
	struct efx_tc_pedit_action *pedit_md; /* entry in tc_pedit_ht table */
	struct list_head encap_user; /* entry on encap_md->users list */
	struct list_head count_user; /* entry on counter->users list, if encap */
	struct efx_tc_action_set_list *user; /* Only populated if encap_md */
	u32 fw_id; /* index of this entry in firmware actions table */
	struct list_head list;
};

struct efx_tc_match_fields {
	/* L1, I guess? */
	u32 ingress_port;
	/* L2 (inner when encap) */
	__be16 eth_proto;
	__be16 vlan_tci[2], vlan_proto[2];
	u8 eth_saddr[ETH_ALEN], eth_daddr[ETH_ALEN];
	/* L3 (when IP) */
	u8 ip_proto, ip_tos, ip_ttl;
	__be32 src_ip, dst_ip;
	struct in6_addr src_ip6, dst_ip6;
	/* L4 */
	__be16 l4_sport, l4_dport; /* Ports (UDP, TCP) */
	__be16 tcp_flags;
	/* Encap.  The following are *outer* fields.  Note that there are no
	 * outer eth (L2) fields; this is because TC doesn't have them.
	 */
	__be32 enc_src_ip, enc_dst_ip;
	struct in6_addr enc_src_ip6, enc_dst_ip6;
	u8 enc_ip_tos, enc_ip_ttl;
	__be16 enc_sport, enc_dport;
	__be32 enc_keyid; /* e.g. VNI, VSID */
};

static inline bool efx_tc_match_is_encap(const struct efx_tc_match_fields *mask)
{
	return mask->enc_src_ip || mask->enc_dst_ip ||
	       !ipv6_addr_any(&mask->enc_src_ip6) ||
	       !ipv6_addr_any(&mask->enc_dst_ip6) || mask->enc_ip_tos ||
	       mask->enc_ip_ttl || mask->enc_sport || mask->enc_dport;
}

struct efx_tc_encap_match {
	__be32 src_ip, dst_ip;
	struct in6_addr src_ip6, dst_ip6;
	__be16 udp_dport;
	u16 tun_type; /* enum efx_encap_type */
	struct rhash_head linkage;
	refcount_t ref;
	u32 fw_id; /* index of this entry in firmware encap match table */
};

struct efx_tc_match {
	struct efx_tc_match_fields value;
	struct efx_tc_match_fields mask;
	struct efx_tc_encap_match *encap;
};

struct efx_tc_action_set_list {
	struct list_head list;
	u32 fw_id;
};

enum efx_tc_default_rules { /* named by ingress port */
	EFX_TC_DFLT_PF,
	EFX_TC_DFLT_WIRE,
	EFX_TC_DFLT_VF_BASE,
	EFX_TC_DFLT_VF_REP_BASE
};

#define	EFX_TC_DFLT_VF(_vf)	(EFX_TC_DFLT_VF_BASE + (_vf) * 2)
#define	EFX_TC_DFLT_VF_REP(_vf)	(EFX_TC_DFLT_VF_REP_BASE + (_vf) * 2)
/* In principle up to 255 VFs are possible; the last one is #254 */
#define EFX_TC_DFLT__MAX	EFX_TC_DFLT_VF(255)

struct efx_tc_flow_rule {
	unsigned long cookie;
	struct efx_tc_match match;
	struct efx_tc_action_set_list acts;
	enum efx_tc_default_rules fallback; /* what to use when unready? */
	struct rhash_head linkage;
	u32 fw_id;
};

enum efx_tc_rule_prios {
	EFX_TC_PRIO_TC, /* Rule inserted by TC */
	EFX_TC_PRIO_DFLT, /* Default switch rule; one of efx_tc_default_rules */
	EFX_TC_PRIO__NUM
};

/**
 * struct efx_tc_state - control plane data for TC offload
 *
 * @caps: MAE capabilities reported by MCDI
 * @block_list: List of &struct efx_tc_block_binding
 * @mutex: Used to serialise operations on TC hashtables
 * @counter_ht: Hashtable of TC counter index mappings
 * @encap_ht: Hashtable of TC encap actions
 * @pedit_ht: Hashtable of TC pedit actions
 * @match_action_ht: Hashtable of TC match-action rules
 * @neigh_ht: Hashtable of neighbour watches (&struct efx_neigh_binder)
 * @dflt_rules: Match-action rules for default switching; at priority
 *	%EFX_TC_PRIO_DFLT, and indexed by &enum efx_tc_default_rules.
 *	Also used for fallback actions when actual action isn't ready
 * @counters: last DMA results from MC_CMD_MAE_COUNTERS_READ
 * @counter_buffer: DMA mapping for MC_CMD_MAE_COUNTERS_READ
 * @up: have TC datastructures been set up?
 */
struct efx_tc_state {
	struct mae_caps *caps;
	struct list_head block_list;
	struct mutex mutex;
	struct rhashtable counter_ht;
	struct rhashtable encap_ht;
	struct rhashtable pedit_ht;
	struct rhashtable encap_match_ht;
	struct rhashtable match_action_ht;
	struct rhashtable neigh_ht;
	struct efx_tc_flow_rule *dflt_rules;
	struct efx_tc_counter counters[EFX_TC_MAX_COUNTER];
	struct efx_buffer counter_buffer;
	bool up;
};

int efx_tc_configure_default_rule(struct efx_nic *efx,
				  enum efx_tc_default_rules dflt);
void efx_tc_deconfigure_default_rule(struct efx_nic *efx,
				     enum efx_tc_default_rules dflt);
int efx_tc_flower(struct efx_nic *efx, struct net_device *net_dev,
		  struct flow_cls_offload *tc, struct efx_vfrep *efv);
int efx_tc_setup_block(struct net_device *net_dev, struct efx_nic *efx,
		       struct flow_block_offload *tcb, struct efx_vfrep *efv);
int efx_setup_tc(struct net_device *net_dev, enum tc_setup_type type,
		 void *type_data);
int efx_tc_start_stats(struct efx_nic *efx);
void efx_tc_stop_stats(struct efx_nic *efx);

#else /* EFX_TC_OFFLOAD */

struct efx_tc_action_set {
	u16 vlan_push:2;
	u16 vlan_pop:2;
	u16 decap:1;
	u16 deliver:1;
	__be16 vlan_tci[2]; /* TCIs for vlan_push */
	__be16 vlan_proto[2]; /* Ethertypes for vlan_push */
	struct efx_tc_counter_index *count;
	u32 dest_mport;
	u32 fw_id; /* index of this entry in firmware actions table */
	struct list_head list;
};

struct efx_tc_match_fields {
	/* L1, I guess? */
	u32 ingress_port;
};

struct efx_tc_encap_match {};
struct efx_tc_encap_action {};

struct efx_tc_match {
	struct efx_tc_match_fields value;
	struct efx_tc_match_fields mask;
	struct efx_tc_encap_match *encap;
};

struct efx_tc_action_set_list {
	struct list_head list;
	u32 fw_id;
};

enum efx_tc_default_rules { /* named by ingress port */
	EFX_TC_DFLT_PF,
	EFX_TC_DFLT_WIRE,
	/* No rules for VFs and vfreps; if we don't have TC offload then we
	 * just don't create them.
	 */
	EFX_TC_DFLT__MAX
};

struct efx_tc_flow_rule {
	struct efx_tc_match match;
	struct efx_tc_action_set_list acts;
	u32 fw_id;
};

enum efx_tc_rule_prios {
	EFX_TC_PRIO_DFLT, /* Default switch rule; one of efx_tc_default_rules */
	EFX_TC_PRIO__NUM
};

struct efx_tc_state {
	struct mae_caps *caps;
	struct efx_tc_flow_rule *dflt_rules;
};

#endif /* EFX_TC_OFFLOAD */

int efx_init_tc(struct efx_nic *efx);
void efx_fini_tc(struct efx_nic *efx);

int efx_init_struct_tc(struct efx_nic *efx);
void efx_fini_struct_tc(struct efx_nic *efx);

int efx_tc_netdev_event(struct efx_nic *efx, unsigned long event,
			struct net_device *net_dev);
int efx_tc_netevent_event(struct efx_nic *efx, unsigned long event,
			  void *ptr);

#endif /* EFX_TC_H */
