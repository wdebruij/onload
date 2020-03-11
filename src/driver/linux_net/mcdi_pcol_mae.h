/****************************************************************************
 * Driver for Solarflare network controllers and boards
 * Copyright 2019 Solarflare Communications Inc.
 * Copyright 2019 Xilinx, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, incorporated herein by reference.
 */


#ifndef MCDI_PCOL_MAE_H
#define MCDI_PCOL_MAE_H

/* DEVEL_PCIE_INTERFACE enum: From EF100 onwards, SFC products can have
 * multiple PCIe interfaces. There is a need to refer to interfaces explicitly
 * from drivers (for example, a management driver on one interface
 * administering a function on another interface). This enumeration provides
 * stable identifiers to all interfaces present on a product. Product
 * documentation will specify which interfaces exist and their associated
 * identifier. In general, drivers, should not assign special meanings to
 * specific values. Instead, behaviour should be determined by NIC
 * configuration, which will identify interfaces where appropriate.
 */
/* enum: Primary host interfaces. Typically (i.e. for all known SFC products)
 * the interface exposed on the edge connector (or form factor equivalent).
 */
#define          DEVEL_PCIE_INTERFACE_HOST_PRIMARY 0x1
/* enum: Riverhead and keystone products have a second PCIe interface to which
 * an on-NIC ARM module is expected to be connecte.
 */
#define          DEVEL_PCIE_INTERFACE_NIC_EMBEDDED 0x2
/* enum: For MCDI commands issued over a PCIe interface, this value is
 * translated into the interface over which the command was issued. Not
 * meaningful for other MCDI transports.
 */
#define          DEVEL_PCIE_INTERFACE_CALLER 0xffffffff

/* MAE_FIELD_SUPPORT_STATUS enum */
/* enum: The NIC does not support this field. The driver must ensure that any
 * mask associated with this field in a match rule is zeroed. The NIC may
 * either reject requests with an invalid mask for such a field, or may assume
 * that the mask is zero. (This category only exists to describe behaviour for
 * fields that a newer driver might know about but that older firmware does
 * not. It is recommended that firmware report MAE_FIELD_FIELD_MATCH_NEVER for
 * all match fields defined at the time of its compilation. If a driver see a
 * field support status value that it does not recognise, it must treat that
 * field as thought the field was reported as MAE_FIELD_SUPPORTED_MATCH_NEVER,
 * and must never set a non-zero mask value for this field.
 */
#define          MAE_FIELD_UNSUPPORTED 0x0
/* enum: The NIC supports this field, but cannot use it in a match rule. The
 * driver must ensure that any mask for such a field in a match rule is zeroed.
 * The NIC will reject requests with an invalid mask for such a field.
 */
#define          MAE_FIELD_SUPPORTED_MATCH_NEVER 0x1
/* enum: The NIC supports this field, and must use it in all match rules. The
 * driver must ensure that any mask for such a field is all ones. The NIC will
 * reject requests with an invalid mask for such a field.
 */
#define          MAE_FIELD_SUPPORTED_MATCH_ALWAYS 0x2
/* enum: The NIC supports this field, and may optionally use it in match rules.
 * The driver must ensure that any mask for such a field is either all zeroes
 * or all ones. The NIC will reject requests with an invalid mask for such a
 * field.
 */
#define          MAE_FIELD_SUPPORTED_MATCH_OPTIONAL 0x3
/* enum: The NIC supports this field, and may optionally use it in match rules.
 * The driver must ensure that any mask for such a field is either all zeroes
 * or a consecutive set of ones following by all zeroes (starting from MSB).
 * The NIC will reject requests with an invalid mask for such a field.
 */
#define          MAE_FIELD_SUPPORTED_MATCH_PREFIX 0x4
/* enum: The NIC supports this field, and may optionally use it in match rules.
 * The driver may provide an arbitrary mask for such a field.
 */
#define          MAE_FIELD_SUPPORTED_MATCH_MASK 0x5

/* MAE_CT_VNI_MODE enum: Controls the layout of the VNI input to the conntrack
 * lookup.
 */
/* enum: The VNI input to the conntrack lookup will be zero. */
#define          MAE_CT_VNI_MODE_ZERO 0x0
/* enum: The VNI input to the conntrack lookup will be the VNI field from the
 * packet.
 */
#define          MAE_CT_VNI_MODE_VNI 0x1
/* enum: The VNI input to the conntrack lookup will be the outermost VLAN tag.
 */
#define          MAE_CT_VNI_MODE_1VLAN 0x2
/* enum: The VNI input to the conntrack lookup will be both VLAN tags. */
#define          MAE_CT_VNI_MODE_2VLAN 0x3

/* MAE_FIELD enum: TODO: this enum shares namespace with the support status
 * enum; is that okay? TODO: Can we pollute the MAE_* namespace? Should be have
 * an MCDI prefix too?
 */
#define          MAE_FIELD_INGRESS_PORT 0x0 /* enum */
#define          MAE_FIELD_MARK 0x1 /* enum */
/* enum: Table ID used in action rule. Initially zero, can be changed in action
 * rule response.
 */
#define          MAE_FIELD_RECIRC_ID 0x2
#define          MAE_FIELD_IS_IP_FRAG 0x3 /* enum */
#define          MAE_FIELD_DO_CT 0x4 /* enum */
#define          MAE_FIELD_CT_HIT 0x5 /* enum */
/* enum: Undefined unless CT_HIT=1. */
#define          MAE_FIELD_CT_MARK 0x6
/* enum: Undefined unless DO_CT=1. */
#define          MAE_FIELD_CT_DOMAIN 0x7
/* enum: Undefined unless CT_HIT=1. */
#define          MAE_FIELD_CT_PRIVATE_FLAGS 0x8
#define          MAE_FIELD_ETHER_TYPE 0x21 /* enum */
#define          MAE_FIELD_VLAN0_TCI 0x22 /* enum */
#define          MAE_FIELD_VLAN0_PROTO 0x23 /* enum */
#define          MAE_FIELD_VLAN1_TCI 0x24 /* enum */
#define          MAE_FIELD_VLAN1_PROTO 0x25 /* enum */
/* enum: Inner when encap */
#define          MAE_FIELD_ETH_SADDR 0x28
/* enum: Inner when encap */
#define          MAE_FIELD_ETH_DADDR 0x29
/* enum: Inner when encap TODO: At present, we consider IP4 and IP6 addresses
 * as distinct fields. In an IP4 packet, then IP6 fields will have a value of
 * 'missing' and will therefore never match a rule with a non-zero mask for an
 * IP6 field (and vice-versa). This feels the cleanest, and I can't see a
 * usecase for the case that it cannot handler---I want to match on some bits
 * on an IP address but I don't care what type of address it is. However, if we
 * really do want to change this, we should add *_IP fields which take a well-
 * defined value for both IP4 and IP6 packets. N.B. The above does not require
 * us to have space in the TCAM match/mask for both IP4 and IP6 addresses.
 * They're exclusive, so can be overlaid and the correct semantics can be
 * provided by adding a qualifying have_ip4/have_ip6 bit to the TCAM match.
 * N.B. The above would allow us to provide *_IP4 / *_IP6 and *_IP (as
 * described) semantics simultaneously. The IP addresses would be overload in
 * the TCAM key, and matches would be qualified by a have_ip4/have_ip6/have_ip
 * bit in the TCAM match.
 */
#define          MAE_FIELD_SRC_IP4 0x2a
/* enum: Inner when encap */
#define          MAE_FIELD_SRC_IP6 0x2b
/* enum: Inner when encap */
#define          MAE_FIELD_DST_IP4 0x2c
/* enum: Inner when encap */
#define          MAE_FIELD_DST_IP6 0x2d
/* enum: Inner when encap */
#define          MAE_FIELD_IP_PROTO 0x2e
/* enum: Inner when encap */
#define          MAE_FIELD_IP_TOS 0x2f
/* enum: Inner when encap TODO: How do OVS/DPDK expect to use this field? GFT
 * is only interested in matching on whether the TTL field is equal to 1. I
 * suggest: 1) we create a new field called IP_TTL_IS_ONE (or similar) 2)
 * define the new field as having value, (TTL == 1) ? 1 : 0, and width 1. The
 * semantics implied by this are easy to implement in hardware; we just need a
 * trivial translation between the TTL value parsed from the packet and the
 * value used in the TCAM lookup. We can choose which of IP_TTL and
 * IP_TTL_IS_ONE we implement at a later date, maybe offering both. The choice
 * can be conveyed to drivers using the already defined match field capability
 * discovery mechanism.
 */
#define          MAE_FIELD_IP_TTL 0x30
/* enum: Inner when encap TODO: how this is defined? The raw flags +
 * frag_offset from the packet, or some derived value more amenable to ternary
 * matching? TODO: there was a proposal for driver-allocation fields. The
 * driver would provide some instruction for how to extract given field values,
 * and would be given a field id in return. It could then use that field id in
 * its matches. This feels like it would be extremely hard to implement in
 * hardware, but I mention it for completeness.
 */
#define          MAE_FIELD_IP_FLAGS 0x31
/* enum: Ports (UDP, TCP) Inner when encap */
#define          MAE_FIELD_L4_SPORT 0x32
/* enum: Ports (UDP, TCP) Inner when encap */
#define          MAE_FIELD_L4_DPORT 0x33
/* enum: TCP flags Inner when encap TODO: how is this field defined? How should
 * we match against it? I understand that GFT doesn't want to the normal
 * ternary matching of, pkt_field & key_mask == key_match & key_mask [1] but
 * instead wants matching of, pkt_field & key_mask != 0 || key_mask == 0 [2]
 * There are ways of implementhing these semantics in the hardware with various
 * level of flexibility. Of more immediate relevance is how to express it
 * within the API. I suggest: 1) we define a new field called TCP_FLAGS_ANY or
 * similar. 2) we define a new field support status like
 * MAE_FIELD_SUPPORTED_MATCH_MASK_ANY, although with a more descriptive name.
 * This value indicates that matches on the corresponding field have semantics
 * as per [2] 3) We advertise FIELD_TCP_FLAGS_ANY as having support of
 * MAE_FIELD_SUPPORTED_MATCH_NEVER or MAE_FIELD_SUPPORTED_MATCH_MASK_ANY Notes:
 * o The above suggested change can be made at any time and will be backwards
 * compatible given that the driver is required to supply a mask of zero for
 * fields with a support status of zero, and is deemed to have supplied a mask
 * of zero for fields not included in its rule insertion requests. o Both
 * TCP_FLAGS and TCP_FLAGS_ANY can coexist in the API, with neither, either or
 * both being advertise as supported (though implementing both may be
 * unpalatable in hardware).
 */
#define          MAE_FIELD_TCP_FLAGS 0x34
/* enum: The type of encapsulated used for this packet. Value as per
 * ENCAP_TYPE_*.
 */
#define          MAE_FIELD_ENCAP_TYPE 0x3f
#define          MAE_FIELD_ENCAP_RULE_ID 0x40 /* enum */
/* enum: The ID of the encap match rule that marked this packet as
 * encapsulated. TODO: This field is included because it might allow the use of
 * narrower TCAMs for match-action rules, chaining of the fields already
 * matched on when determining the encapsulation type. It's not clear whether
 * this is useful, and the mixining of IDs with packet data is admittedly a bit
 * gross. That said, we are doing the same with m-port ids...
 */
#define          MAE_FIELD_OUTER_RULE_ID 0x40
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_ETHER_TYPE 0x41
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_VLAN0_TCI 0x42
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_VLAN0_PROTO 0x43
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_VLAN1_TCI 0x44
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_VLAN1_PROTO 0x45
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_ETH_SADDR 0x48
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_ETH_DADDR 0x49
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_SRC_IP4 0x4a
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_SRC_IP6 0x4b
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_DST_IP4 0x4c
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_DST_IP6 0x4d
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_IP_PROTO 0x4e
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_IP_TOS 0x4f
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_IP_TTL 0x50
/* enum: Outer; only present when encap TODO: how this is defined? The raw
 * flags + frag_offset from the packet, or some derived value more amenable to
 * ternary matching?
 */
#define          MAE_FIELD_ENC_IP_FLAGS 0x51
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_L4_SPORT 0x52
/* enum: Outer; only present when encap */
#define          MAE_FIELD_ENC_L4_DPORT 0x53
/* enum: VNI (when VXLAN or GENEVE) VSID (when NVGRE) Outer; only present when
 * encap
 */
#define          MAE_FIELD_ENC_VNET_ID 0x54

/* MAE_MCDI_ENCAP_TYPE enum */
#define          MAE_MCDI_ENCAP_TYPE_NONE 0x0 /* enum */
/* enum: Don't assume enum aligns with support bitmask... */
#define          MAE_MCDI_ENCAP_TYPE_VXLAN 0x1
#define          MAE_MCDI_ENCAP_TYPE_NVGRE 0x2 /* enum */
#define          MAE_MCDI_ENCAP_TYPE_GENEVE 0x3 /* enum */
#define          MAE_MCDI_ENCAP_TYPE_TODO_OTHERS 0x3e7 /* enum */

/* MAE_FIELD_FLAGS structuredef */
#define    MAE_FIELD_FLAGS_LEN 4
#define       MAE_FIELD_FLAGS_FLAT_OFST 0
#define       MAE_FIELD_FLAGS_FLAT_LEN 4
#define        MAE_FIELD_FLAGS_SUPPORT_STATUS_OFST 0
#define        MAE_FIELD_FLAGS_SUPPORT_STATUS_LBN 0
#define        MAE_FIELD_FLAGS_SUPPORT_STATUS_WIDTH 6
#define        MAE_FIELD_FLAGS_MASK_AFFECTS_CLASS_OFST 0
#define        MAE_FIELD_FLAGS_MASK_AFFECTS_CLASS_LBN 6
#define        MAE_FIELD_FLAGS_MASK_AFFECTS_CLASS_WIDTH 1
#define        MAE_FIELD_FLAGS_MATCH_AFFECTS_CLASS_OFST 0
#define        MAE_FIELD_FLAGS_MATCH_AFFECTS_CLASS_LBN 7
#define        MAE_FIELD_FLAGS_MATCH_AFFECTS_CLASS_WIDTH 1
#define       MAE_FIELD_FLAGS_FLAT_LBN 0
#define       MAE_FIELD_FLAGS_FLAT_WIDTH 32

/* MAE_ENC_FIELD_MASK_VALUE_PAIRS structuredef: Mask and value pairs for all
 * fields that it makes sense to use to determine the encapsulation type of a
 * packet. Its intended use is to keep a common packing of fields across
 * multiple MCDI commands, keeping things inherently sychronised and allowing
 * code shared. To use in an MCDI command, the command should end with a
 * variable length byte array populated with this structure. Do not extend this
 * structure. Instead, create _Vx versions with the necessary fields appended.
 * That way, the existing semantics for extending MCDI commands are preserved.
 */
#define    MAE_ENC_FIELD_MASK_VALUE_PAIRS_LEN 156
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_OFST 0
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_LEN 4
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_LBN 0
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_WIDTH 32
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_MASK_OFST 4
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_MASK_LEN 4
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_MASK_LBN 32
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_MASK_WIDTH 32
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_OFST 8
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_LBN 64
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_WIDTH 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_MASK_OFST 10
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_MASK_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_MASK_LBN 80
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_MASK_WIDTH 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_OFST 12
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_LBN 96
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_WIDTH 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_MASK_OFST 14
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_MASK_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_MASK_LBN 112
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_MASK_WIDTH 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_OFST 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_LBN 128
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_WIDTH 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_MASK_OFST 18
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_MASK_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_MASK_LBN 144
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_MASK_WIDTH 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_OFST 20
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_LBN 160
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_WIDTH 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_MASK_OFST 22
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_MASK_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_MASK_LBN 176
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_MASK_WIDTH 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_OFST 24
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_LBN 192
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_WIDTH 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_MASK_OFST 26
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_MASK_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_MASK_LBN 208
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_MASK_WIDTH 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_OFST 28
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_LEN 6
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_LBN 224
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_WIDTH 48
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_MASK_OFST 34
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_MASK_LEN 6
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_MASK_LBN 272
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_MASK_WIDTH 48
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_OFST 40
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_LEN 6
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_LBN 320
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_WIDTH 48
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_MASK_OFST 46
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_MASK_LEN 6
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_MASK_LBN 368
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_MASK_WIDTH 48
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_OFST 52
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_LEN 4
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_LBN 416
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_WIDTH 32
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_MASK_OFST 56
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_MASK_LEN 4
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_MASK_LBN 448
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_MASK_WIDTH 32
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_OFST 60
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_LEN 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_LBN 480
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_WIDTH 128
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_MASK_OFST 76
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_MASK_LEN 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_MASK_LBN 608
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_MASK_WIDTH 128
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_OFST 92
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_LEN 4
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_LBN 736
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_WIDTH 32
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_MASK_OFST 96
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_MASK_LEN 4
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_MASK_LBN 768
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_MASK_WIDTH 32
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_OFST 100
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_LEN 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_LBN 800
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_WIDTH 128
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_MASK_OFST 116
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_MASK_LEN 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_MASK_LBN 928
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_MASK_WIDTH 128
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_OFST 132
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_LEN 1
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_LBN 1056
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_WIDTH 8
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_MASK_OFST 133
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_MASK_LEN 1
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_MASK_LBN 1064
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_MASK_WIDTH 8
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_OFST 134
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_LEN 1
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_LBN 1072
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_WIDTH 8
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_MASK_OFST 135
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_MASK_LEN 1
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_MASK_LBN 1080
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_MASK_WIDTH 8
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_OFST 136
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_LEN 1
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_LBN 1088
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_WIDTH 8
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_MASK_OFST 137
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_MASK_LEN 1
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_MASK_LBN 1096
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_MASK_WIDTH 8
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_OFST 140
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_LEN 4
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_LBN 1120
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_WIDTH 32
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_MASK_OFST 144
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_MASK_LEN 4
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_MASK_LBN 1152
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_MASK_WIDTH 32
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_OFST 148
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_LBN 1184
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_WIDTH 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_MASK_OFST 150
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_MASK_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_MASK_LBN 1200
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_MASK_WIDTH 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_OFST 152
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_LBN 1216
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_WIDTH 16
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_MASK_OFST 154
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_MASK_LEN 2
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_MASK_LBN 1232
#define       MAE_ENC_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_MASK_WIDTH 16

/* MAE_FIELD_MASK_VALUE_PAIRS structuredef: Mask and value pairs for all fields
 * currently defined. Same semantics as MAE_ENC_FIELD_MASK_VALUE_PAIRS.
 */
#define    MAE_FIELD_MASK_VALUE_PAIRS_LEN 344
#define       MAE_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_OFST 0
#define       MAE_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_LBN 0
#define       MAE_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_MASK_OFST 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_MASK_LBN 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_INGRESS_MPORT_SELECTOR_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_MARK_OFST 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_MARK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_MARK_LBN 64
#define       MAE_FIELD_MASK_VALUE_PAIRS_MARK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_MARK_MASK_OFST 12
#define       MAE_FIELD_MASK_VALUE_PAIRS_MARK_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_MARK_MASK_LBN 96
#define       MAE_FIELD_MASK_VALUE_PAIRS_MARK_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETHER_TYPE_BE_OFST 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETHER_TYPE_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETHER_TYPE_BE_LBN 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETHER_TYPE_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETHER_TYPE_BE_MASK_OFST 18
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETHER_TYPE_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETHER_TYPE_BE_MASK_LBN 144
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETHER_TYPE_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_TCI_BE_OFST 20
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_TCI_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_TCI_BE_LBN 160
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_TCI_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_TCI_BE_MASK_OFST 22
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_TCI_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_TCI_BE_MASK_LBN 176
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_TCI_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_PROTO_BE_OFST 24
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_PROTO_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_PROTO_BE_LBN 192
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_PROTO_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_PROTO_BE_MASK_OFST 26
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_PROTO_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_PROTO_BE_MASK_LBN 208
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN0_PROTO_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_TCI_BE_OFST 28
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_TCI_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_TCI_BE_LBN 224
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_TCI_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_TCI_BE_MASK_OFST 30
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_TCI_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_TCI_BE_MASK_LBN 240
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_TCI_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_PROTO_BE_OFST 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_PROTO_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_PROTO_BE_LBN 256
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_PROTO_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_PROTO_BE_MASK_OFST 34
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_PROTO_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_PROTO_BE_MASK_LBN 272
#define       MAE_FIELD_MASK_VALUE_PAIRS_VLAN1_PROTO_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_SADDR_BE_OFST 36
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_SADDR_BE_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_SADDR_BE_LBN 288
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_SADDR_BE_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_SADDR_BE_MASK_OFST 42
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_SADDR_BE_MASK_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_SADDR_BE_MASK_LBN 336
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_SADDR_BE_MASK_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_DADDR_BE_OFST 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_DADDR_BE_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_DADDR_BE_LBN 384
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_DADDR_BE_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_DADDR_BE_MASK_OFST 54
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_DADDR_BE_MASK_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_DADDR_BE_MASK_LBN 432
#define       MAE_FIELD_MASK_VALUE_PAIRS_ETH_DADDR_BE_MASK_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP4_BE_OFST 60
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP4_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP4_BE_LBN 480
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP4_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP4_BE_MASK_OFST 64
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP4_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP4_BE_MASK_LBN 512
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP4_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP6_BE_OFST 68
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP6_BE_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP6_BE_LBN 544
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP6_BE_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP6_BE_MASK_OFST 84
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP6_BE_MASK_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP6_BE_MASK_LBN 672
#define       MAE_FIELD_MASK_VALUE_PAIRS_SRC_IP6_BE_MASK_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP4_BE_OFST 100
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP4_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP4_BE_LBN 800
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP4_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP4_BE_MASK_OFST 104
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP4_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP4_BE_MASK_LBN 832
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP4_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP6_BE_OFST 108
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP6_BE_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP6_BE_LBN 864
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP6_BE_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP6_BE_MASK_OFST 124
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP6_BE_MASK_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP6_BE_MASK_LBN 992
#define       MAE_FIELD_MASK_VALUE_PAIRS_DST_IP6_BE_MASK_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_PROTO_OFST 140
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_PROTO_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_PROTO_LBN 1120
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_PROTO_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_PROTO_MASK_OFST 141
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_PROTO_MASK_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_PROTO_MASK_LBN 1128
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_PROTO_MASK_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TOS_OFST 142
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TOS_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TOS_LBN 1136
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TOS_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TOS_MASK_OFST 143
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TOS_MASK_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TOS_MASK_LBN 1144
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TOS_MASK_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TTL_OFST 144
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TTL_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TTL_LBN 1152
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TTL_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TTL_MASK_OFST 145
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TTL_MASK_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TTL_MASK_LBN 1160
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_TTL_MASK_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_FLAGS_BE_OFST 148
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_FLAGS_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_FLAGS_BE_LBN 1184
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_FLAGS_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_FLAGS_BE_MASK_OFST 152
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_FLAGS_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_FLAGS_BE_MASK_LBN 1216
#define       MAE_FIELD_MASK_VALUE_PAIRS_IP_FLAGS_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_SPORT_BE_OFST 156
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_SPORT_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_SPORT_BE_LBN 1248
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_SPORT_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_SPORT_BE_MASK_OFST 158
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_SPORT_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_SPORT_BE_MASK_LBN 1264
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_SPORT_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_DPORT_BE_OFST 160
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_DPORT_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_DPORT_BE_LBN 1280
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_DPORT_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_DPORT_BE_MASK_OFST 162
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_DPORT_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_DPORT_BE_MASK_LBN 1296
#define       MAE_FIELD_MASK_VALUE_PAIRS_L4_DPORT_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_TCP_FLAGS_BE_OFST 164
#define       MAE_FIELD_MASK_VALUE_PAIRS_TCP_FLAGS_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_TCP_FLAGS_BE_LBN 1312
#define       MAE_FIELD_MASK_VALUE_PAIRS_TCP_FLAGS_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_TCP_FLAGS_BE_MASK_OFST 166
#define       MAE_FIELD_MASK_VALUE_PAIRS_TCP_FLAGS_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_TCP_FLAGS_BE_MASK_LBN 1328
#define       MAE_FIELD_MASK_VALUE_PAIRS_TCP_FLAGS_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_TYPE_ID_OFST 168
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_TYPE_ID_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_TYPE_ID_LBN 1344
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_TYPE_ID_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_TYPE_ID_MASK_OFST 172
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_TYPE_ID_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_TYPE_ID_MASK_LBN 1376
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_TYPE_ID_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_RULE_ID_OFST 176
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_RULE_ID_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_RULE_ID_LBN 1408
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_RULE_ID_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_RULE_ID_MASK_OFST 180
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_RULE_ID_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_RULE_ID_MASK_LBN 1440
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENCAP_RULE_ID_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_OUTER_RULE_ID_OFST 176
#define       MAE_FIELD_MASK_VALUE_PAIRS_OUTER_RULE_ID_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_OUTER_RULE_ID_LBN 1408
#define       MAE_FIELD_MASK_VALUE_PAIRS_OUTER_RULE_ID_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_OUTER_RULE_ID_MASK_OFST 180
#define       MAE_FIELD_MASK_VALUE_PAIRS_OUTER_RULE_ID_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_OUTER_RULE_ID_MASK_LBN 1440
#define       MAE_FIELD_MASK_VALUE_PAIRS_OUTER_RULE_ID_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_OFST 184
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_LBN 1472
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_MASK_OFST 188
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_MASK_LBN 1504
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETHER_TYPE_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_OFST 192
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_LBN 1536
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_MASK_OFST 194
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_MASK_LBN 1552
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_TCI_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_OFST 196
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_LBN 1568
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_MASK_OFST 198
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_MASK_LBN 1584
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN0_PROTO_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_OFST 200
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_LBN 1600
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_MASK_OFST 202
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_MASK_LBN 1616
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_TCI_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_OFST 204
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_LBN 1632
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_MASK_OFST 206
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_MASK_LBN 1648
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VLAN1_PROTO_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_OFST 208
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_LBN 1664
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_MASK_OFST 214
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_MASK_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_MASK_LBN 1712
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_SADDR_BE_MASK_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_OFST 220
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_LBN 1760
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_MASK_OFST 226
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_MASK_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_MASK_LBN 1808
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_ETH_DADDR_BE_MASK_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_OFST 232
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_LBN 1856
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_MASK_OFST 236
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_MASK_LBN 1888
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP4_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_OFST 240
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_LBN 1920
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_MASK_OFST 256
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_MASK_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_MASK_LBN 2048
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_SRC_IP6_BE_MASK_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_OFST 272
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_LBN 2176
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_MASK_OFST 276
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_MASK_LBN 2208
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP4_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_OFST 280
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_LBN 2240
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_MASK_OFST 296
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_MASK_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_MASK_LBN 2368
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_DST_IP6_BE_MASK_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_OFST 312
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_LBN 2496
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_MASK_OFST 313
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_MASK_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_MASK_LBN 2504
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_PROTO_MASK_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_OFST 314
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_LBN 2512
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_MASK_OFST 315
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_MASK_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_MASK_LBN 2520
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TOS_MASK_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_OFST 316
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_LBN 2528
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_MASK_OFST 317
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_MASK_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_MASK_LBN 2536
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_TTL_MASK_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_OFST 320
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_LBN 2560
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_MASK_OFST 324
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_MASK_LBN 2592
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_IP_FLAGS_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_OFST 328
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_LBN 2624
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_MASK_OFST 330
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_MASK_LBN 2640
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_SPORT_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_OFST 332
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_LBN 2656
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_MASK_OFST 334
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_MASK_LBN 2672
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_L4_DPORT_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VNET_ID_BE_OFST 336
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VNET_ID_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VNET_ID_BE_LBN 2688
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VNET_ID_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VNET_ID_BE_MASK_OFST 340
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VNET_ID_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VNET_ID_BE_MASK_LBN 2720
#define       MAE_FIELD_MASK_VALUE_PAIRS_ENC_VNET_ID_BE_MASK_WIDTH 32

/* MAE_FIELD_MASK_VALUE_PAIRS_V2 structuredef */
#define    MAE_FIELD_MASK_VALUE_PAIRS_V2_LEN 372
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_INGRESS_MPORT_SELECTOR_OFST 0
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_INGRESS_MPORT_SELECTOR_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_INGRESS_MPORT_SELECTOR_LBN 0
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_INGRESS_MPORT_SELECTOR_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_INGRESS_MPORT_SELECTOR_MASK_OFST 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_INGRESS_MPORT_SELECTOR_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_INGRESS_MPORT_SELECTOR_MASK_LBN 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_INGRESS_MPORT_SELECTOR_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_MARK_OFST 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_MARK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_MARK_LBN 64
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_MARK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_MARK_MASK_OFST 12
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_MARK_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_MARK_MASK_LBN 96
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_MARK_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETHER_TYPE_BE_OFST 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETHER_TYPE_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETHER_TYPE_BE_LBN 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETHER_TYPE_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETHER_TYPE_BE_MASK_OFST 18
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETHER_TYPE_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETHER_TYPE_BE_MASK_LBN 144
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETHER_TYPE_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_TCI_BE_OFST 20
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_TCI_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_TCI_BE_LBN 160
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_TCI_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_TCI_BE_MASK_OFST 22
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_TCI_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_TCI_BE_MASK_LBN 176
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_TCI_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_PROTO_BE_OFST 24
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_PROTO_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_PROTO_BE_LBN 192
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_PROTO_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_PROTO_BE_MASK_OFST 26
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_PROTO_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_PROTO_BE_MASK_LBN 208
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN0_PROTO_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_TCI_BE_OFST 28
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_TCI_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_TCI_BE_LBN 224
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_TCI_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_TCI_BE_MASK_OFST 30
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_TCI_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_TCI_BE_MASK_LBN 240
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_TCI_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_PROTO_BE_OFST 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_PROTO_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_PROTO_BE_LBN 256
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_PROTO_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_PROTO_BE_MASK_OFST 34
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_PROTO_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_PROTO_BE_MASK_LBN 272
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_VLAN1_PROTO_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_SADDR_BE_OFST 36
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_SADDR_BE_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_SADDR_BE_LBN 288
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_SADDR_BE_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_SADDR_BE_MASK_OFST 42
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_SADDR_BE_MASK_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_SADDR_BE_MASK_LBN 336
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_SADDR_BE_MASK_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_DADDR_BE_OFST 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_DADDR_BE_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_DADDR_BE_LBN 384
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_DADDR_BE_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_DADDR_BE_MASK_OFST 54
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_DADDR_BE_MASK_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_DADDR_BE_MASK_LBN 432
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ETH_DADDR_BE_MASK_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP4_BE_OFST 60
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP4_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP4_BE_LBN 480
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP4_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP4_BE_MASK_OFST 64
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP4_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP4_BE_MASK_LBN 512
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP4_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP6_BE_OFST 68
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP6_BE_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP6_BE_LBN 544
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP6_BE_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP6_BE_MASK_OFST 84
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP6_BE_MASK_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP6_BE_MASK_LBN 672
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_SRC_IP6_BE_MASK_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP4_BE_OFST 100
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP4_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP4_BE_LBN 800
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP4_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP4_BE_MASK_OFST 104
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP4_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP4_BE_MASK_LBN 832
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP4_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP6_BE_OFST 108
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP6_BE_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP6_BE_LBN 864
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP6_BE_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP6_BE_MASK_OFST 124
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP6_BE_MASK_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP6_BE_MASK_LBN 992
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_DST_IP6_BE_MASK_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_PROTO_OFST 140
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_PROTO_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_PROTO_LBN 1120
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_PROTO_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_PROTO_MASK_OFST 141
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_PROTO_MASK_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_PROTO_MASK_LBN 1128
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_PROTO_MASK_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TOS_OFST 142
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TOS_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TOS_LBN 1136
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TOS_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TOS_MASK_OFST 143
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TOS_MASK_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TOS_MASK_LBN 1144
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TOS_MASK_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TTL_OFST 144
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TTL_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TTL_LBN 1152
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TTL_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TTL_MASK_OFST 145
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TTL_MASK_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TTL_MASK_LBN 1160
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_TTL_MASK_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_FLAGS_BE_OFST 148
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_FLAGS_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_FLAGS_BE_LBN 1184
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_FLAGS_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_FLAGS_BE_MASK_OFST 152
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_FLAGS_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_FLAGS_BE_MASK_LBN 1216
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_IP_FLAGS_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_SPORT_BE_OFST 156
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_SPORT_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_SPORT_BE_LBN 1248
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_SPORT_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_SPORT_BE_MASK_OFST 158
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_SPORT_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_SPORT_BE_MASK_LBN 1264
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_SPORT_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_DPORT_BE_OFST 160
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_DPORT_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_DPORT_BE_LBN 1280
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_DPORT_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_DPORT_BE_MASK_OFST 162
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_DPORT_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_DPORT_BE_MASK_LBN 1296
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_L4_DPORT_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_TCP_FLAGS_BE_OFST 164
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_TCP_FLAGS_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_TCP_FLAGS_BE_LBN 1312
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_TCP_FLAGS_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_TCP_FLAGS_BE_MASK_OFST 166
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_TCP_FLAGS_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_TCP_FLAGS_BE_MASK_LBN 1328
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_TCP_FLAGS_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_TYPE_ID_OFST 168
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_TYPE_ID_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_TYPE_ID_LBN 1344
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_TYPE_ID_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_TYPE_ID_MASK_OFST 172
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_TYPE_ID_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_TYPE_ID_MASK_LBN 1376
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_TYPE_ID_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_RULE_ID_OFST 176
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_RULE_ID_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_RULE_ID_LBN 1408
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_RULE_ID_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_RULE_ID_MASK_OFST 180
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_RULE_ID_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_RULE_ID_MASK_LBN 1440
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENCAP_RULE_ID_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_OUTER_RULE_ID_OFST 176
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_OUTER_RULE_ID_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_OUTER_RULE_ID_LBN 1408
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_OUTER_RULE_ID_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_OUTER_RULE_ID_MASK_OFST 180
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_OUTER_RULE_ID_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_OUTER_RULE_ID_MASK_LBN 1440
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_OUTER_RULE_ID_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETHER_TYPE_BE_OFST 184
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETHER_TYPE_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETHER_TYPE_BE_LBN 1472
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETHER_TYPE_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETHER_TYPE_BE_MASK_OFST 188
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETHER_TYPE_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETHER_TYPE_BE_MASK_LBN 1504
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETHER_TYPE_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_TCI_BE_OFST 192
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_TCI_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_TCI_BE_LBN 1536
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_TCI_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_TCI_BE_MASK_OFST 194
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_TCI_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_TCI_BE_MASK_LBN 1552
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_TCI_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_PROTO_BE_OFST 196
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_PROTO_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_PROTO_BE_LBN 1568
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_PROTO_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_PROTO_BE_MASK_OFST 198
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_PROTO_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_PROTO_BE_MASK_LBN 1584
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN0_PROTO_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_TCI_BE_OFST 200
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_TCI_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_TCI_BE_LBN 1600
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_TCI_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_TCI_BE_MASK_OFST 202
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_TCI_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_TCI_BE_MASK_LBN 1616
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_TCI_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_PROTO_BE_OFST 204
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_PROTO_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_PROTO_BE_LBN 1632
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_PROTO_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_PROTO_BE_MASK_OFST 206
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_PROTO_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_PROTO_BE_MASK_LBN 1648
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VLAN1_PROTO_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_SADDR_BE_OFST 208
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_SADDR_BE_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_SADDR_BE_LBN 1664
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_SADDR_BE_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_SADDR_BE_MASK_OFST 214
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_SADDR_BE_MASK_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_SADDR_BE_MASK_LBN 1712
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_SADDR_BE_MASK_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_DADDR_BE_OFST 220
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_DADDR_BE_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_DADDR_BE_LBN 1760
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_DADDR_BE_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_DADDR_BE_MASK_OFST 226
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_DADDR_BE_MASK_LEN 6
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_DADDR_BE_MASK_LBN 1808
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_ETH_DADDR_BE_MASK_WIDTH 48
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP4_BE_OFST 232
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP4_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP4_BE_LBN 1856
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP4_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP4_BE_MASK_OFST 236
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP4_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP4_BE_MASK_LBN 1888
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP4_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP6_BE_OFST 240
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP6_BE_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP6_BE_LBN 1920
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP6_BE_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP6_BE_MASK_OFST 256
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP6_BE_MASK_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP6_BE_MASK_LBN 2048
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_SRC_IP6_BE_MASK_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP4_BE_OFST 272
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP4_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP4_BE_LBN 2176
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP4_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP4_BE_MASK_OFST 276
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP4_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP4_BE_MASK_LBN 2208
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP4_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP6_BE_OFST 280
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP6_BE_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP6_BE_LBN 2240
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP6_BE_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP6_BE_MASK_OFST 296
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP6_BE_MASK_LEN 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP6_BE_MASK_LBN 2368
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_DST_IP6_BE_MASK_WIDTH 128
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_PROTO_OFST 312
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_PROTO_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_PROTO_LBN 2496
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_PROTO_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_PROTO_MASK_OFST 313
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_PROTO_MASK_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_PROTO_MASK_LBN 2504
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_PROTO_MASK_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TOS_OFST 314
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TOS_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TOS_LBN 2512
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TOS_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TOS_MASK_OFST 315
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TOS_MASK_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TOS_MASK_LBN 2520
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TOS_MASK_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TTL_OFST 316
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TTL_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TTL_LBN 2528
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TTL_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TTL_MASK_OFST 317
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TTL_MASK_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TTL_MASK_LBN 2536
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_TTL_MASK_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_FLAGS_BE_OFST 320
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_FLAGS_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_FLAGS_BE_LBN 2560
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_FLAGS_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_FLAGS_BE_MASK_OFST 324
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_FLAGS_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_FLAGS_BE_MASK_LBN 2592
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_IP_FLAGS_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_SPORT_BE_OFST 328
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_SPORT_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_SPORT_BE_LBN 2624
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_SPORT_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_SPORT_BE_MASK_OFST 330
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_SPORT_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_SPORT_BE_MASK_LBN 2640
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_SPORT_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_DPORT_BE_OFST 332
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_DPORT_BE_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_DPORT_BE_LBN 2656
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_DPORT_BE_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_DPORT_BE_MASK_OFST 334
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_DPORT_BE_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_DPORT_BE_MASK_LBN 2672
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_L4_DPORT_BE_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VNET_ID_BE_OFST 336
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VNET_ID_BE_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VNET_ID_BE_LBN 2688
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VNET_ID_BE_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VNET_ID_BE_MASK_OFST 340
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VNET_ID_BE_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VNET_ID_BE_MASK_LBN 2720
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_ENC_VNET_ID_BE_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_FLAGS_OFST 344
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_FLAGS_LEN 4
#define        MAE_FIELD_MASK_VALUE_PAIRS_V2_IS_IP_FRAG_OFST 344
#define        MAE_FIELD_MASK_VALUE_PAIRS_V2_IS_IP_FRAG_LBN 0
#define        MAE_FIELD_MASK_VALUE_PAIRS_V2_IS_IP_FRAG_WIDTH 1
#define        MAE_FIELD_MASK_VALUE_PAIRS_V2_DO_CT_OFST 344
#define        MAE_FIELD_MASK_VALUE_PAIRS_V2_DO_CT_LBN 1
#define        MAE_FIELD_MASK_VALUE_PAIRS_V2_DO_CT_WIDTH 1
#define        MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_HIT_OFST 344
#define        MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_HIT_LBN 2
#define        MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_HIT_WIDTH 1
#define        MAE_FIELD_MASK_VALUE_PAIRS_V2_RSVD_OFST 344
#define        MAE_FIELD_MASK_VALUE_PAIRS_V2_RSVD_LBN 3
#define        MAE_FIELD_MASK_VALUE_PAIRS_V2_RSVD_WIDTH 29
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_FLAGS_LBN 2752
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_FLAGS_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_FLAGS_MASK_OFST 348
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_FLAGS_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_FLAGS_MASK_LBN 2784
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_FLAGS_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_DOMAIN_OFST 352
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_DOMAIN_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_DOMAIN_LBN 2816
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_DOMAIN_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_DOMAIN_MASK_OFST 354
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_DOMAIN_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_DOMAIN_MASK_LBN 2832
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_DOMAIN_MASK_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_MARK_OFST 356
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_MARK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_MARK_LBN 2848
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_MARK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_MARK_MASK_OFST 360
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_MARK_MASK_LEN 4
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_MARK_MASK_LBN 2880
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_MARK_MASK_WIDTH 32
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_PRIVATE_FLAGS_OFST 364
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_PRIVATE_FLAGS_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_PRIVATE_FLAGS_LBN 2912
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_PRIVATE_FLAGS_WIDTH 8
/* Set to zero. */
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RSVD2_OFST 365
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RSVD2_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RSVD2_LBN 2920
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RSVD2_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_PRIVATE_FLAGS_MASK_OFST 366
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_PRIVATE_FLAGS_MASK_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_PRIVATE_FLAGS_MASK_LBN 2928
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_CT_PRIVATE_FLAGS_MASK_WIDTH 8
/* Set to zero. */
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RSVD3_OFST 367
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RSVD3_LEN 1
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RSVD3_LBN 2936
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RSVD3_WIDTH 8
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RECIRC_ID_OFST 368
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RECIRC_ID_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RECIRC_ID_LBN 2944
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RECIRC_ID_WIDTH 16
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RECIRC_ID_MASK_OFST 370
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RECIRC_ID_MASK_LEN 2
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RECIRC_ID_MASK_LBN 2960
#define       MAE_FIELD_MASK_VALUE_PAIRS_V2_RECIRC_ID_MASK_WIDTH 16

/* MAE_MPORT_SELECTOR structuredef: MPORTS are identified by an opaque unsigned
 * integer value (mport_id) that is guaranteed to be representable within
 * 32-bits or within any NIC interface field that needs store the value
 * (whichever is narrowers). TODO: there's a desire to give mport_id some
 * structure such that there's a low index part and a high type part. The
 * driver would be able to request a mask for the index part, and this extract
 * just the index. Moreover, there'd be a guarantee that the index part would
 * be small (maybe best done by making the mask as small as possible) so that
 * the driver could use the index as, well, an index. We'd ensure that it was
 * implicit in the API what the type field represented, such that the driver
 * could avoid collisions when indexing by the index. We'd probably reserve the
 * special types for things that the driver didn't need to index on. There are
 * MCDI commands for obtaining the m-port id for preexisting m-ports, and for
 * creating new m-ports and obtaining their IDs. In order to provide a simpler
 * MAE API, it is not necessary for the driver to look up the mport_id for
 * certain pre-existing mports. Instead, the MAE MCDI interface uses an 'm-port
 * selector', which is a 32-bit value with a well known format that is used to
 * identify an m-port based upon what that m-port is connected to. This
 * structure defines the format of an mport selector. TODO: The MCDI commands
 * for querying/creating mports don't exist yet. Moreover, it's not clear
 * who'll be responsible for creating the m-ports necessary for certain
 * applications (e.g. should the MC or the driver create the m-mports for guest
 * representers? What about the m-ports for guest VNICs?) For prototyping
 * purposes, the driver should just assume that the m-ports necessary for OVS
 * already exist, and should identify them using the appropriate m-port
 * selector value. TODO: is mport_selector a good term? I'm not happy with it.
 */
#define    MAE_MPORT_SELECTOR_LEN 4
/* Used to force the tools to output bitfield-style defines for this structure.
 */
#define       MAE_MPORT_SELECTOR_FLAT_OFST 0
#define       MAE_MPORT_SELECTOR_FLAT_LEN 4
/* enum: An m-port selector value that is guaranteed never to represent a real
 * mport
 */
#define          MAE_MPORT_SELECTOR_NULL 0x0
/* enum: The m-port assigned to the calling client. */
#define          MAE_MPORT_SELECTOR_ASSIGNED 0x1000000
#define        MAE_MPORT_SELECTOR_TYPE_OFST 0
#define        MAE_MPORT_SELECTOR_TYPE_LBN 24
#define        MAE_MPORT_SELECTOR_TYPE_WIDTH 8
/* enum: The MPORT connected to a given physical port */
#define          MAE_MPORT_SELECTOR_TYPE_PPORT 0x2
/* enum: The MPORT assigned to a given PCIe function */
#define          MAE_MPORT_SELECTOR_TYPE_FUNC 0x3
/* enum: An mport_id */
#define          MAE_MPORT_SELECTOR_TYPE_MPORT_ID 0x4
#define        MAE_MPORT_SELECTOR_MPORT_ID_OFST 0
#define        MAE_MPORT_SELECTOR_MPORT_ID_LBN 0
#define        MAE_MPORT_SELECTOR_MPORT_ID_WIDTH 24
#define        MAE_MPORT_SELECTOR_PPORT_ID_OFST 0
#define        MAE_MPORT_SELECTOR_PPORT_ID_LBN 0
#define        MAE_MPORT_SELECTOR_PPORT_ID_WIDTH 4
#define        MAE_MPORT_SELECTOR_FUNC_PF_ID_OFST 0
#define        MAE_MPORT_SELECTOR_FUNC_PF_ID_LBN 16
#define        MAE_MPORT_SELECTOR_FUNC_PF_ID_WIDTH 8
#define        MAE_MPORT_SELECTOR_FUNC_VF_ID_OFST 0
#define        MAE_MPORT_SELECTOR_FUNC_VF_ID_LBN 0
#define        MAE_MPORT_SELECTOR_FUNC_VF_ID_WIDTH 16
/* enum: Used for VF_ID to indicate a physical function. */
#define          MAE_MPORT_SELECTOR_FUNC_VF_ID_NULL 0xffff
/* enum: Used for PF_ID to indicate the physical function of the calling
 * client. - When used by a PF with VF_ID == VF_ID_NULL, the mport selector
 * relates to the calling function. (For clarity, it is recommended that
 * clients use ASSIGNED to achieve this behaviour). - When used by a PF with
 * VF_ID != VF_ID_NULL, the mport selector relates to a VF child of the calling
 * function. - When used by a VF with VF_ID == VF_ID_NULL, the mport selector
 * relates to the PF owning the calling function. - When used by a VF with
 * VF_ID != VF_ID_NULL, the mport selector relates to a sibling VF of the
 * calling function. - Not meaningful used by a client that is not a PCIe
 * function.
 */
#define          MAE_MPORT_SELECTOR_FUNC_PF_ID_CALLER 0xff
#define       MAE_MPORT_SELECTOR_FLAT_LBN 0
#define       MAE_MPORT_SELECTOR_FLAT_WIDTH 32

/* MAE_COUNTER_VALUE structuredef: Counter values as DMAd to the host. */
#define    MAE_COUNTER_VALUE_LEN 16
#define       MAE_COUNTER_VALUE_PACKETS_OFST 0
#define       MAE_COUNTER_VALUE_PACKETS_LEN 8
#define       MAE_COUNTER_VALUE_PACKETS_LO_OFST 0
#define       MAE_COUNTER_VALUE_PACKETS_HI_OFST 4
#define       MAE_COUNTER_VALUE_PACKETS_LBN 0
#define       MAE_COUNTER_VALUE_PACKETS_WIDTH 64
#define       MAE_COUNTER_VALUE_BYTES_OFST 8
#define       MAE_COUNTER_VALUE_BYTES_LEN 8
#define       MAE_COUNTER_VALUE_BYTES_LO_OFST 8
#define       MAE_COUNTER_VALUE_BYTES_HI_OFST 12
#define       MAE_COUNTER_VALUE_BYTES_LBN 64
#define       MAE_COUNTER_VALUE_BYTES_WIDTH 64


/***********************************/
/* MC_CMD_MAE_GET_CAPABILITIES
 * TODO: Format TBD; lots missing. We likely want some bits that say we meet a
 * set capabilities sufficient for, e.g., OVS (where the exact meaning of each
 * bit is in DoxBox somewhere)
 */
#define MC_CMD_MAE_GET_CAPABILITIES 0x140
#undef MC_CMD_0x140_PRIVILEGE_CTG

#define MC_CMD_0x140_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_GET_CAPABILITIES_IN msgrequest */
#define    MC_CMD_MAE_GET_CAPABILITIES_IN_LEN 0

/* MC_CMD_MAE_GET_CAPABILITIES_OUT msgresponse */
#define    MC_CMD_MAE_GET_CAPABILITIES_OUT_LEN 44
/* The number of field IDs that the NIC supports. Any field with a ID greater
 * than or equal to the value returned in this field must be treated as having
 * a support level of MAE_FIELD_UNSUPPORTED in all requests.
 */
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_MATCH_FIELD_COUNT_OFST 0
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_MATCH_FIELD_COUNT_LEN 4
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_TYPES_SUPPORTED_OFST 4
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_TYPES_SUPPORTED_LEN 4
#define        MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_TYPE_VXLAN_OFST 4
#define        MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_TYPE_VXLAN_LBN 0
#define        MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_TYPE_VXLAN_WIDTH 1
#define        MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_TYPE_TODO_OTHERS_OFST 4
#define        MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_TYPE_TODO_OTHERS_LBN 1
#define        MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_TYPE_TODO_OTHERS_WIDTH 1
/* The total number of counters available to allocate. */
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_COUNTERS_OFST 8
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_COUNTERS_LEN 4
/* The total number of counters lists available to allocate. A value of zero
 * indicates that counter lists are not supported by the NIC.
 */
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_COUNTER_LISTS_OFST 12
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_COUNTER_LISTS_LEN 4
/* The total number of encap header structures available to allocate. */
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_HEADER_LIMIT_OFST 16
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_HEADER_LIMIT_LEN 4
/* The total number of pedit structures available to allocate. A value of zero
 * indicates that packet edits are not supported by the NIC.
 */
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_PEDITS_OFST 20
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_PEDITS_LEN 4
/* The total number of action sets available to allocate. */
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ACTION_SETS_OFST 24
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ACTION_SETS_LEN 4
/* The total number of action set lists available to allocate. */
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ACTION_SET_LISTS_OFST 28
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ACTION_SET_LISTS_LEN 4
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_RULES_OFST 32
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_RULES_LEN 4
/* The total number of outer rules available to allocate. */
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_OUTER_RULES_OFST 32
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_OUTER_RULES_LEN 4
/* The total number of action rules available to allocate. */
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ACTION_RULES_OFST 36
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ACTION_RULES_LEN 4
/* The number of priorities available for OUTER_RULE filters. It is invalid to
 * install an OUTER_RULE filter with a priority number >= ENCAP_PRIOS.
 */
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_PRIOS_OFST 36
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ENCAP_PRIOS_LEN 4
/* The number of priorities available for ACTION_RULE filters. It is invalid to
 * install a MATCH_ACTION filter with a priority number >= ACTION_PRIOS.
 */
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ACTION_PRIOS_OFST 40
#define       MC_CMD_MAE_GET_CAPABILITIES_OUT_ACTION_PRIOS_LEN 4


/***********************************/
/* MC_CMD_MAE_GET_ACTION_RULE_CAPS
 * Get a level of support for match fields when used in match-action rules
 */
#define MC_CMD_MAE_GET_ACTION_RULE_CAPS 0x141
#undef MC_CMD_0x141_PRIVILEGE_CTG

#define MC_CMD_0x141_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_GET_ACTION_RULE_CAPS_IN msgrequest */
#define    MC_CMD_MAE_GET_ACTION_RULE_CAPS_IN_LEN 0

/* MC_CMD_MAE_GET_ACTION_RULE_CAPS_OUT msgresponse */
#define    MC_CMD_MAE_GET_ACTION_RULE_CAPS_OUT_LENMIN 0
#define    MC_CMD_MAE_GET_ACTION_RULE_CAPS_OUT_LENMAX 252
#define    MC_CMD_MAE_GET_ACTION_RULE_CAPS_OUT_LENMAX_MCDI2 1020
#define    MC_CMD_MAE_GET_ACTION_RULE_CAPS_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_GET_ACTION_RULE_CAPS_OUT_FIELD_FLAGS_NUM(len) (((len)-0)/4)
/* Array of values indicating the NIC's support for a given field, indexed by
 * field id. The driver must ensure space for
 * MC_CMD_MAE_GET_CAPABILITIES.MATCH_FIELD_COUNT entries in the array..
 */
#define       MC_CMD_MAE_GET_ACTION_RULE_CAPS_OUT_FIELD_FLAGS_OFST 0
#define       MC_CMD_MAE_GET_ACTION_RULE_CAPS_OUT_FIELD_FLAGS_LEN 4
#define       MC_CMD_MAE_GET_ACTION_RULE_CAPS_OUT_FIELD_FLAGS_MINNUM 0
#define       MC_CMD_MAE_GET_ACTION_RULE_CAPS_OUT_FIELD_FLAGS_MAXNUM 63
#define       MC_CMD_MAE_GET_ACTION_RULE_CAPS_OUT_FIELD_FLAGS_MAXNUM_MCDI2 255


/***********************************/
/* MC_CMD_MAE_GET_OUTER_RULE_FIELD_CAPS
 * Get a level of support for fields used in outer rule keys.
 */
#define MC_CMD_MAE_GET_OUTER_RULE_FIELD_CAPS 0x142
#undef MC_CMD_0x142_PRIVILEGE_CTG

#define MC_CMD_0x142_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_GET_OUTER_RULE_CAPS_IN msgrequest */
#define    MC_CMD_MAE_GET_OUTER_RULE_CAPS_IN_LEN 0

/* MC_CMD_MAE_GET_OUTER_RULE_CAPS_OUT msgresponse */
#define    MC_CMD_MAE_GET_OUTER_RULE_CAPS_OUT_LENMIN 0
#define    MC_CMD_MAE_GET_OUTER_RULE_CAPS_OUT_LENMAX 252
#define    MC_CMD_MAE_GET_OUTER_RULE_CAPS_OUT_LENMAX_MCDI2 1020
#define    MC_CMD_MAE_GET_OUTER_RULE_CAPS_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_GET_OUTER_RULE_CAPS_OUT_FIELD_FLAGS_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_GET_ACTION_RULE_CAPS.MAE_FIELD_FLAGS */
#define       MC_CMD_MAE_GET_OUTER_RULE_CAPS_OUT_FIELD_FLAGS_OFST 0
#define       MC_CMD_MAE_GET_OUTER_RULE_CAPS_OUT_FIELD_FLAGS_LEN 4
#define       MC_CMD_MAE_GET_OUTER_RULE_CAPS_OUT_FIELD_FLAGS_MINNUM 0
#define       MC_CMD_MAE_GET_OUTER_RULE_CAPS_OUT_FIELD_FLAGS_MAXNUM 63
#define       MC_CMD_MAE_GET_OUTER_RULE_CAPS_OUT_FIELD_FLAGS_MAXNUM_MCDI2 255


/***********************************/
/* MC_CMD_MAE_COUNTER_ALLOC
 * Allocate match-action-engine counters
 */
#define MC_CMD_MAE_COUNTER_ALLOC 0x143
#undef MC_CMD_0x143_PRIVILEGE_CTG

#define MC_CMD_0x143_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_COUNTER_ALLOC_IN msgrequest */
#define    MC_CMD_MAE_COUNTER_ALLOC_IN_LEN 4
/* The number of counters that the driver would like allocated */
#define       MC_CMD_MAE_COUNTER_ALLOC_IN_REQUESTED_COUNT_OFST 0
#define       MC_CMD_MAE_COUNTER_ALLOC_IN_REQUESTED_COUNT_LEN 4

/* MC_CMD_MAE_COUNTER_ALLOC_OUT msgresponse */
#define    MC_CMD_MAE_COUNTER_ALLOC_OUT_LENMIN 4
#define    MC_CMD_MAE_COUNTER_ALLOC_OUT_LENMAX 252
#define    MC_CMD_MAE_COUNTER_ALLOC_OUT_LENMAX_MCDI2 1020
#define    MC_CMD_MAE_COUNTER_ALLOC_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_COUNTER_ALLOC_OUT_COUNTER_ID_NUM(len) (((len)-0)/4)
/* An array containing the IDs for the counters allocated. The number of
 * counters that the NIC allocated is given implicitly by the size of the
 * response. It is never greater than REQUESTED_COUNT, but may be less. It is
 * never less than 1; failure to allocate a single counter will cause an error
 * to be returned.
 */
#define       MC_CMD_MAE_COUNTER_ALLOC_OUT_COUNTER_ID_OFST 0
#define       MC_CMD_MAE_COUNTER_ALLOC_OUT_COUNTER_ID_LEN 4
#define       MC_CMD_MAE_COUNTER_ALLOC_OUT_COUNTER_ID_MINNUM 1
#define       MC_CMD_MAE_COUNTER_ALLOC_OUT_COUNTER_ID_MAXNUM 63
#define       MC_CMD_MAE_COUNTER_ALLOC_OUT_COUNTER_ID_MAXNUM_MCDI2 255
/* enum: A counter ID that is guaranteed never to represent a real counter */
#define          MC_CMD_MAE_COUNTER_ALLOC_OUT_COUNTER_ID_NULL 0xffffffff


/***********************************/
/* MC_CMD_MAE_COUNTER_FREE
 * Free match-action-engine counters
 */
#define MC_CMD_MAE_COUNTER_FREE 0x144
#undef MC_CMD_0x144_PRIVILEGE_CTG

#define MC_CMD_0x144_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_COUNTER_FREE_IN msgrequest */
#define    MC_CMD_MAE_COUNTER_FREE_IN_LENMIN 4
#define    MC_CMD_MAE_COUNTER_FREE_IN_LENMAX 128
#define    MC_CMD_MAE_COUNTER_FREE_IN_LENMAX_MCDI2 128
#define    MC_CMD_MAE_COUNTER_FREE_IN_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_COUNTER_FREE_IN_FREE_COUNTER_ID_NUM(len) (((len)-0)/4)
/* An array containing the IDs for the counters to be freed. The number of
 * counters to be freed is given implicitly by the size of the request.
 */
#define       MC_CMD_MAE_COUNTER_FREE_IN_FREE_COUNTER_ID_OFST 0
#define       MC_CMD_MAE_COUNTER_FREE_IN_FREE_COUNTER_ID_LEN 4
#define       MC_CMD_MAE_COUNTER_FREE_IN_FREE_COUNTER_ID_MINNUM 1
#define       MC_CMD_MAE_COUNTER_FREE_IN_FREE_COUNTER_ID_MAXNUM 32
#define       MC_CMD_MAE_COUNTER_FREE_IN_FREE_COUNTER_ID_MAXNUM_MCDI2 32

/* MC_CMD_MAE_COUNTER_FREE_OUT msgresponse */
#define    MC_CMD_MAE_COUNTER_FREE_OUT_LENMIN 4
#define    MC_CMD_MAE_COUNTER_FREE_OUT_LENMAX 128
#define    MC_CMD_MAE_COUNTER_FREE_OUT_LENMAX_MCDI2 128
#define    MC_CMD_MAE_COUNTER_FREE_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_COUNTER_FREE_OUT_FREED_COUNTER_ID_NUM(len) (((len)-0)/4)
/* An array containing the IDs for the counters to that were freed. The number
 * of counters that were freed is given implicitly by the size of the response.
 * It is never greater than the number that were requested to be freed, but may
 * be less if counters could not be freed. It is never less than 1; failure to
 * allocate a single counter will cause an error to be returned. N.B. failure
 * to free a counter can only occur on incorrect driver behaviour, so asserting
 * that the expected counters were freed is reasonable. When debugging,
 * attempting to free a single counter at a time will provide a reason for the
 * failure to free said counter.
 */
#define       MC_CMD_MAE_COUNTER_FREE_OUT_FREED_COUNTER_ID_OFST 0
#define       MC_CMD_MAE_COUNTER_FREE_OUT_FREED_COUNTER_ID_LEN 4
#define       MC_CMD_MAE_COUNTER_FREE_OUT_FREED_COUNTER_ID_MINNUM 1
#define       MC_CMD_MAE_COUNTER_FREE_OUT_FREED_COUNTER_ID_MAXNUM 32
#define       MC_CMD_MAE_COUNTER_FREE_OUT_FREED_COUNTER_ID_MAXNUM_MCDI2 32


/***********************************/
/* MC_CMD_MAE_COUNTER_LIST_ALLOC
 * Allocate a list of match-action-engine counters
 */
#define MC_CMD_MAE_COUNTER_LIST_ALLOC 0x145
#undef MC_CMD_0x145_PRIVILEGE_CTG

#define MC_CMD_0x145_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_COUNTER_LIST_ALLOC_IN msgrequest */
#define    MC_CMD_MAE_COUNTER_LIST_ALLOC_IN_LENMIN 4
#define    MC_CMD_MAE_COUNTER_LIST_ALLOC_IN_LENMAX 252
#define    MC_CMD_MAE_COUNTER_LIST_ALLOC_IN_LENMAX_MCDI2 1020
#define    MC_CMD_MAE_COUNTER_LIST_ALLOC_IN_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_COUNTER_LIST_ALLOC_IN_COUNTER_IDS_NUM(len) (((len)-0)/4)
/* An array containing the IDs for the counters that should comprise this
 * counter list.
 */
#define       MC_CMD_MAE_COUNTER_LIST_ALLOC_IN_COUNTER_IDS_OFST 0
#define       MC_CMD_MAE_COUNTER_LIST_ALLOC_IN_COUNTER_IDS_LEN 4
#define       MC_CMD_MAE_COUNTER_LIST_ALLOC_IN_COUNTER_IDS_MINNUM 1
#define       MC_CMD_MAE_COUNTER_LIST_ALLOC_IN_COUNTER_IDS_MAXNUM 63
#define       MC_CMD_MAE_COUNTER_LIST_ALLOC_IN_COUNTER_IDS_MAXNUM_MCDI2 255

/* MC_CMD_MAE_COUNTER_LIST_ALLOC_OUT msgresponse */
#define    MC_CMD_MAE_COUNTER_LIST_ALLOC_OUT_LEN 4
#define       MC_CMD_MAE_COUNTER_LIST_ALLOC_OUT_COUNTER_LIST_ID_OFST 0
#define       MC_CMD_MAE_COUNTER_LIST_ALLOC_OUT_COUNTER_LIST_ID_LEN 4
/* enum: A counter ID that is guaranteed never to represent a real counter */
#define          MC_CMD_MAE_COUNTER_LIST_ALLOC_OUT_COUNTER_LIST_ID_NULL 0xffffffff


/***********************************/
/* MC_CMD_MAE_COUNTER_LIST_FREE
 * Free match-action-engine counter lists
 */
#define MC_CMD_MAE_COUNTER_LIST_FREE 0x146
#undef MC_CMD_0x146_PRIVILEGE_CTG

#define MC_CMD_0x146_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_COUNTER_LIST_FREE_IN msgrequest */
#define    MC_CMD_MAE_COUNTER_LIST_FREE_IN_LENMIN 4
#define    MC_CMD_MAE_COUNTER_LIST_FREE_IN_LENMAX 128
#define    MC_CMD_MAE_COUNTER_LIST_FREE_IN_LENMAX_MCDI2 128
#define    MC_CMD_MAE_COUNTER_LIST_FREE_IN_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_COUNTER_LIST_FREE_IN_FREE_COUNTER_LIST_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_COUNTER_LIST_FREE_IN_FREE_COUNTER_LIST_ID_OFST 0
#define       MC_CMD_MAE_COUNTER_LIST_FREE_IN_FREE_COUNTER_LIST_ID_LEN 4
#define       MC_CMD_MAE_COUNTER_LIST_FREE_IN_FREE_COUNTER_LIST_ID_MINNUM 1
#define       MC_CMD_MAE_COUNTER_LIST_FREE_IN_FREE_COUNTER_LIST_ID_MAXNUM 32
#define       MC_CMD_MAE_COUNTER_LIST_FREE_IN_FREE_COUNTER_LIST_ID_MAXNUM_MCDI2 32

/* MC_CMD_MAE_COUNTER_LIST_FREE_OUT msgresponse */
#define    MC_CMD_MAE_COUNTER_LIST_FREE_OUT_LENMIN 4
#define    MC_CMD_MAE_COUNTER_LIST_FREE_OUT_LENMAX 128
#define    MC_CMD_MAE_COUNTER_LIST_FREE_OUT_LENMAX_MCDI2 128
#define    MC_CMD_MAE_COUNTER_LIST_FREE_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_COUNTER_LIST_FREE_OUT_FREED_COUNTER_LIST_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_COUNTER_LIST_FREE_OUT_FREED_COUNTER_LIST_ID_OFST 0
#define       MC_CMD_MAE_COUNTER_LIST_FREE_OUT_FREED_COUNTER_LIST_ID_LEN 4
#define       MC_CMD_MAE_COUNTER_LIST_FREE_OUT_FREED_COUNTER_LIST_ID_MINNUM 1
#define       MC_CMD_MAE_COUNTER_LIST_FREE_OUT_FREED_COUNTER_LIST_ID_MAXNUM 32
#define       MC_CMD_MAE_COUNTER_LIST_FREE_OUT_FREED_COUNTER_LIST_ID_MAXNUM_MCDI2 32


/***********************************/
/* MC_CMD_MAE_COUNTERS_READ
 * Read all allocated counters. TODO: I know this API is horrid; it's intended
 * to be a quick bodge to be use for C-model development purposes and to be
 * replaced with something saner once the hardware is designed
 */
#define MC_CMD_MAE_COUNTERS_READ 0x147
#undef MC_CMD_0x147_PRIVILEGE_CTG

#define MC_CMD_0x147_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_COUNTERS_READ_IN msgrequest */
#define    MC_CMD_MAE_COUNTERS_READ_IN_LEN 12
/* The number of counters to read. Only countes with ID < MAX_COUNTERS will be
 * read.
 */
#define       MC_CMD_MAE_COUNTERS_READ_IN_MAX_COUNTERS_OFST 0
#define       MC_CMD_MAE_COUNTERS_READ_IN_MAX_COUNTERS_LEN 4
/* The address of a buffer in host memory into which the counters will be
 * DMAed. The buffer must be large enough to contain a table (indexed by
 * COUNTER_ID) containing MAX_COUNTERS rows. Each table row is a pair of 64 bit
 * little endian values containing the packet and byte counts respectively (as
 * per MAE_COUNTER_VALUE).
 */
#define       MC_CMD_MAE_COUNTERS_READ_IN_DMA_ADDR_OFST 4
#define       MC_CMD_MAE_COUNTERS_READ_IN_DMA_ADDR_LEN 8
#define       MC_CMD_MAE_COUNTERS_READ_IN_DMA_ADDR_LO_OFST 4
#define       MC_CMD_MAE_COUNTERS_READ_IN_DMA_ADDR_HI_OFST 8

/* MC_CMD_MAE_COUNTERS_READ_OUT msgresponse: Receipt of a response indicates
 * that DMAing of the counters has completed.
 */
#define    MC_CMD_MAE_COUNTERS_READ_OUT_LEN 0


/***********************************/
/* MC_CMD_MAE_ENCAP_HEADER_ALLOC
 * Allocate encap action metadata
 */
#define MC_CMD_MAE_ENCAP_HEADER_ALLOC 0x148
#undef MC_CMD_0x148_PRIVILEGE_CTG

#define MC_CMD_0x148_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_ENCAP_HEADER_ALLOC_IN msgrequest */
#define    MC_CMD_MAE_ENCAP_HEADER_ALLOC_IN_LENMIN 4
#define    MC_CMD_MAE_ENCAP_HEADER_ALLOC_IN_LENMAX 252
#define    MC_CMD_MAE_ENCAP_HEADER_ALLOC_IN_LENMAX_MCDI2 1020
#define    MC_CMD_MAE_ENCAP_HEADER_ALLOC_IN_LEN(num) (4+1*(num))
#define    MC_CMD_MAE_ENCAP_HEADER_ALLOC_IN_HDR_DATA_NUM(len) (((len)-4)/1)
#define       MC_CMD_MAE_ENCAP_HEADER_ALLOC_IN_ENCAP_TYPE_OFST 0
#define       MC_CMD_MAE_ENCAP_HEADER_ALLOC_IN_ENCAP_TYPE_LEN 4
#define       MC_CMD_MAE_ENCAP_HEADER_ALLOC_IN_HDR_DATA_OFST 4
#define       MC_CMD_MAE_ENCAP_HEADER_ALLOC_IN_HDR_DATA_LEN 1
#define       MC_CMD_MAE_ENCAP_HEADER_ALLOC_IN_HDR_DATA_MINNUM 0
#define       MC_CMD_MAE_ENCAP_HEADER_ALLOC_IN_HDR_DATA_MAXNUM 248
#define       MC_CMD_MAE_ENCAP_HEADER_ALLOC_IN_HDR_DATA_MAXNUM_MCDI2 1016

/* MC_CMD_MAE_ENCAP_HEADER_ALLOC_OUT msgresponse */
#define    MC_CMD_MAE_ENCAP_HEADER_ALLOC_OUT_LEN 4
#define       MC_CMD_MAE_ENCAP_HEADER_ALLOC_OUT_ENCAP_HEADER_ID_OFST 0
#define       MC_CMD_MAE_ENCAP_HEADER_ALLOC_OUT_ENCAP_HEADER_ID_LEN 4
/* enum: An encap metadata ID that is guaranteed never to represent real encap
 * metadata
 */
#define          MC_CMD_MAE_ENCAP_HEADER_ALLOC_OUT_ENCAP_HEADER_ID_NULL 0xffffffff


/***********************************/
/* MC_CMD_MAE_ENCAP_HEADER_UPDATE
 * Update encap action metadata
 */
#define MC_CMD_MAE_ENCAP_HEADER_UPDATE 0x149
#undef MC_CMD_0x149_PRIVILEGE_CTG

#define MC_CMD_0x149_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN msgrequest */
#define    MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_LENMIN 8
#define    MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_LENMAX 252
#define    MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_LENMAX_MCDI2 1020
#define    MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_LEN(num) (8+1*(num))
#define    MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_HDR_DATA_NUM(len) (((len)-8)/1)
#define       MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_EH_ID_OFST 0
#define       MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_EH_ID_LEN 4
#define       MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_ENCAP_TYPE_OFST 4
#define       MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_ENCAP_TYPE_LEN 4
#define       MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_HDR_DATA_OFST 8
#define       MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_HDR_DATA_LEN 1
#define       MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_HDR_DATA_MINNUM 0
#define       MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_HDR_DATA_MAXNUM 244
#define       MC_CMD_MAE_ENCAP_HEADER_UPDATE_IN_HDR_DATA_MAXNUM_MCDI2 1012

/* MC_CMD_MAE_ENCAP_HEADER_UPDATE_OUT msgresponse */
#define    MC_CMD_MAE_ENCAP_HEADER_UPDATE_OUT_LEN 0


/***********************************/
/* MC_CMD_MAE_ENCAP_HEADER_FREE
 * Free encap action metadata
 */
#define MC_CMD_MAE_ENCAP_HEADER_FREE 0x14a
#undef MC_CMD_0x14a_PRIVILEGE_CTG

#define MC_CMD_0x14a_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_ENCAP_HEADER_FREE_IN msgrequest */
#define    MC_CMD_MAE_ENCAP_HEADER_FREE_IN_LENMIN 4
#define    MC_CMD_MAE_ENCAP_HEADER_FREE_IN_LENMAX 128
#define    MC_CMD_MAE_ENCAP_HEADER_FREE_IN_LENMAX_MCDI2 128
#define    MC_CMD_MAE_ENCAP_HEADER_FREE_IN_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_ENCAP_HEADER_FREE_IN_EH_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_ENCAP_HEADER_FREE_IN_EH_ID_OFST 0
#define       MC_CMD_MAE_ENCAP_HEADER_FREE_IN_EH_ID_LEN 4
#define       MC_CMD_MAE_ENCAP_HEADER_FREE_IN_EH_ID_MINNUM 1
#define       MC_CMD_MAE_ENCAP_HEADER_FREE_IN_EH_ID_MAXNUM 32
#define       MC_CMD_MAE_ENCAP_HEADER_FREE_IN_EH_ID_MAXNUM_MCDI2 32

/* MC_CMD_MAE_ENCAP_HEADER_FREE_OUT msgresponse */
#define    MC_CMD_MAE_ENCAP_HEADER_FREE_OUT_LENMIN 4
#define    MC_CMD_MAE_ENCAP_HEADER_FREE_OUT_LENMAX 128
#define    MC_CMD_MAE_ENCAP_HEADER_FREE_OUT_LENMAX_MCDI2 128
#define    MC_CMD_MAE_ENCAP_HEADER_FREE_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_ENCAP_HEADER_FREE_OUT_FREED_EH_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_ENCAP_HEADER_FREE_OUT_FREED_EH_ID_OFST 0
#define       MC_CMD_MAE_ENCAP_HEADER_FREE_OUT_FREED_EH_ID_LEN 4
#define       MC_CMD_MAE_ENCAP_HEADER_FREE_OUT_FREED_EH_ID_MINNUM 1
#define       MC_CMD_MAE_ENCAP_HEADER_FREE_OUT_FREED_EH_ID_MAXNUM 32
#define       MC_CMD_MAE_ENCAP_HEADER_FREE_OUT_FREED_EH_ID_MAXNUM_MCDI2 32


/***********************************/
/* MC_CMD_MAE_PEDIT_ALLOC
 * Allocate a pedit
 */
#define MC_CMD_MAE_PEDIT_ALLOC 0x14b
#undef MC_CMD_0x14b_PRIVILEGE_CTG

#define MC_CMD_0x14b_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_PEDIT_ALLOC_IN msgrequest: TBD */
#define    MC_CMD_MAE_PEDIT_ALLOC_IN_LEN 0

/* MC_CMD_MAE_PEDIT_ALLOC_OUT msgresponse */
#define    MC_CMD_MAE_PEDIT_ALLOC_OUT_LEN 4
#define       MC_CMD_MAE_PEDIT_ALLOC_OUT_PEDIT_ID_OFST 0
#define       MC_CMD_MAE_PEDIT_ALLOC_OUT_PEDIT_ID_LEN 4
/* enum: A pedit ID that is guaranteed never to represent a pedit */
#define          MC_CMD_MAE_PEDIT_ALLOC_OUT_PEDIT_ID_NULL 0xffffffff


/***********************************/
/* MC_CMD_MAE_PEDIT_FREE
 * Free match-action-engine pedits
 */
#define MC_CMD_MAE_PEDIT_FREE 0x14c
#undef MC_CMD_0x14c_PRIVILEGE_CTG

#define MC_CMD_0x14c_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_PEDIT_FREE_IN msgrequest */
#define    MC_CMD_MAE_PEDIT_FREE_IN_LENMIN 4
#define    MC_CMD_MAE_PEDIT_FREE_IN_LENMAX 128
#define    MC_CMD_MAE_PEDIT_FREE_IN_LENMAX_MCDI2 128
#define    MC_CMD_MAE_PEDIT_FREE_IN_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_PEDIT_FREE_IN_FREE_PEDIT_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_PEDIT_FREE_IN_FREE_PEDIT_ID_OFST 0
#define       MC_CMD_MAE_PEDIT_FREE_IN_FREE_PEDIT_ID_LEN 4
#define       MC_CMD_MAE_PEDIT_FREE_IN_FREE_PEDIT_ID_MINNUM 1
#define       MC_CMD_MAE_PEDIT_FREE_IN_FREE_PEDIT_ID_MAXNUM 32
#define       MC_CMD_MAE_PEDIT_FREE_IN_FREE_PEDIT_ID_MAXNUM_MCDI2 32

/* MC_CMD_MAE_PEDIT_FREE_OUT msgresponse */
#define    MC_CMD_MAE_PEDIT_FREE_OUT_LENMIN 4
#define    MC_CMD_MAE_PEDIT_FREE_OUT_LENMAX 128
#define    MC_CMD_MAE_PEDIT_FREE_OUT_LENMAX_MCDI2 128
#define    MC_CMD_MAE_PEDIT_FREE_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_PEDIT_FREE_OUT_FREED_PEDIT_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_PEDIT_FREE_OUT_FREED_PEDIT_ID_OFST 0
#define       MC_CMD_MAE_PEDIT_FREE_OUT_FREED_PEDIT_ID_LEN 4
#define       MC_CMD_MAE_PEDIT_FREE_OUT_FREED_PEDIT_ID_MINNUM 1
#define       MC_CMD_MAE_PEDIT_FREE_OUT_FREED_PEDIT_ID_MAXNUM 32
#define       MC_CMD_MAE_PEDIT_FREE_OUT_FREED_PEDIT_ID_MAXNUM_MCDI2 32


/***********************************/
/* MC_CMD_MAE_ACTION_SET_ALLOC
 */
#define MC_CMD_MAE_ACTION_SET_ALLOC 0x14d
#undef MC_CMD_0x14d_PRIVILEGE_CTG

#define MC_CMD_0x14d_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_ACTION_SET_ALLOC_IN msgrequest */
#define    MC_CMD_MAE_ACTION_SET_ALLOC_IN_LEN 36
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_FLAGS_OFST 0
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_FLAGS_LEN 4
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN_PUSH_OFST 0
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN_PUSH_LBN 0
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN_PUSH_WIDTH 2
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN_POP_OFST 0
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN_POP_LBN 4
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN_POP_WIDTH 2
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_DECAP_OFST 0
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_DECAP_LBN 8
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_DECAP_WIDTH 1
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_MARK_OFST 0
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_MARK_LBN 9
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_MARK_WIDTH 1
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_FLAG_OFST 0
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_FLAG_LBN 10
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_FLAG_WIDTH 1
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_DO_NAT_OFST 0
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_DO_NAT_LBN 11
#define        MC_CMD_MAE_ACTION_SET_ALLOC_IN_DO_NAT_WIDTH 1
/* If VLAN_PUSH >= 1, TCI value to be inserted as outermost VLAN. */
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN0_TCI_BE_OFST 4
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN0_TCI_BE_LEN 2
/* If VLAN_PUSH >= 1, TPID value to be inserted as outermost VLAN. */
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN0_PROTO_BE_OFST 6
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN0_PROTO_BE_LEN 2
/* If VLAN_PUSH == 2, inner TCI value to be inserted. */
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN1_TCI_BE_OFST 8
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN1_TCI_BE_LEN 2
/* If VLAN_PUSH == 2, inner TPID value to be inserted. */
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN1_PROTO_BE_OFST 10
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_VLAN1_PROTO_BE_LEN 2
/* Set to PEDIT_NULL to request no pedit action */
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_PEDIT_ID_OFST 12
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_PEDIT_ID_LEN 4
/* Set to ENCAP_HEADER_ID_NULL to request no encap action */
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_ENCAP_HEADER_ID_OFST 16
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_ENCAP_HEADER_ID_LEN 4
/* An m-port selector identifying the m-port that the modified packet should be
 * delivered to. Set to MPORT_SELECTOR_NULL to request no delivery of the
 * packet.
 */
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_DELIVER_OFST 20
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_DELIVER_LEN 4
/* Set to COUNTER_LIST_ID_NULL to request no counter action. TODO: do we want
 * counters to be associated with an action set? Apparently, both OVS and DPDK
 * associate counters with rules. Associating counters with actions is more
 * flexible (and may be necessary for counters to be meaningful if we ever want
 * to provide the option of counters based upon lengths after modifications).
 * However, it is wasteful if the driver wished to install lots of rules whose
 * actions only differ in which counters they increase.
 */
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_COUNTER_LIST_ID_OFST 24
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_COUNTER_LIST_ID_LEN 4
/* If a driver only wished to update one counter within this action set, then
 * it can supply a COUNTER_ID instead of allocating a single-element counter
 * list. This field should be set to COUNTER_ID_NULL if this behaviour is not
 * required. It is not valid to supply a non-NULL value for both
 * COUNTER_LIST_ID and COUNTER_ID. TODO: I'm unsure about this feature. It's
 * not yet clear how the firmware will internally manage resources. For
 * example, if the driver passed in a counter id, will the firmware then need
 * to internally allocate a single-entry counter list, and then automatically
 * free that list when the referencing action set is freed? Would it be
 * preferrable to only accept a counter list, and push the resource management
 * up to the driver? TODO: I'm also unsure about the way this feature is
 * exposed. In general, this API uses NULL values for ID fields where there
 * should be no linked resource. However, this doesn't naturally describe the
 * situation where, where the two linked resoures are mutually exclusive. The
 * alternative is to have a union of a COUNTER_ID and a COUNTER_LIST_ID, with a
 * separate field indicating what type of ID the field contains. This makes it
 * clear that the two are mutually exclusive, but brings inconsistency to the
 * API. [CJK] Having written the above, my preference is to remove this
 * feature, and require that the driver supply a counter list. This is the
 * simplest option for firmware, and also feels the safest. However, I'm happy
 * to be told that not dealing with the extra resource management in the driver
 * is worth the risk / complexity. If we do retain the current approach of
 * allowing either a counter or a counter list to be specified, I'm ambivalent
 * between the product type vs. tagged union type approaches, and dislike both.
 */
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_COUNTER_ID_OFST 28
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_COUNTER_ID_LEN 4
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_MARK_VALUE_OFST 32
#define       MC_CMD_MAE_ACTION_SET_ALLOC_IN_MARK_VALUE_LEN 4

/* MC_CMD_MAE_ACTION_SET_ALLOC_OUT msgresponse */
#define    MC_CMD_MAE_ACTION_SET_ALLOC_OUT_LEN 4
#define       MC_CMD_MAE_ACTION_SET_ALLOC_OUT_AS_ID_OFST 0
#define       MC_CMD_MAE_ACTION_SET_ALLOC_OUT_AS_ID_LEN 4
/* enum: An action set ID that is guaranteed never to represent an action set
 */
#define          MC_CMD_MAE_ACTION_SET_ALLOC_OUT_ACTION_SET_ID_NULL 0xffffffff


/***********************************/
/* MC_CMD_MAE_ACTION_SET_FREE
 */
#define MC_CMD_MAE_ACTION_SET_FREE 0x14e
#undef MC_CMD_0x14e_PRIVILEGE_CTG

#define MC_CMD_0x14e_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_ACTION_SET_FREE_IN msgrequest */
#define    MC_CMD_MAE_ACTION_SET_FREE_IN_LENMIN 4
#define    MC_CMD_MAE_ACTION_SET_FREE_IN_LENMAX 128
#define    MC_CMD_MAE_ACTION_SET_FREE_IN_LENMAX_MCDI2 128
#define    MC_CMD_MAE_ACTION_SET_FREE_IN_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_ACTION_SET_FREE_IN_AS_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_ACTION_SET_FREE_IN_AS_ID_OFST 0
#define       MC_CMD_MAE_ACTION_SET_FREE_IN_AS_ID_LEN 4
#define       MC_CMD_MAE_ACTION_SET_FREE_IN_AS_ID_MINNUM 1
#define       MC_CMD_MAE_ACTION_SET_FREE_IN_AS_ID_MAXNUM 32
#define       MC_CMD_MAE_ACTION_SET_FREE_IN_AS_ID_MAXNUM_MCDI2 32

/* MC_CMD_MAE_ACTION_SET_FREE_OUT msgresponse */
#define    MC_CMD_MAE_ACTION_SET_FREE_OUT_LENMIN 4
#define    MC_CMD_MAE_ACTION_SET_FREE_OUT_LENMAX 128
#define    MC_CMD_MAE_ACTION_SET_FREE_OUT_LENMAX_MCDI2 128
#define    MC_CMD_MAE_ACTION_SET_FREE_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_ACTION_SET_FREE_OUT_FREED_AS_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_ACTION_SET_FREE_OUT_FREED_AS_ID_OFST 0
#define       MC_CMD_MAE_ACTION_SET_FREE_OUT_FREED_AS_ID_LEN 4
#define       MC_CMD_MAE_ACTION_SET_FREE_OUT_FREED_AS_ID_MINNUM 1
#define       MC_CMD_MAE_ACTION_SET_FREE_OUT_FREED_AS_ID_MAXNUM 32
#define       MC_CMD_MAE_ACTION_SET_FREE_OUT_FREED_AS_ID_MAXNUM_MCDI2 32


/***********************************/
/* MC_CMD_MAE_ACTION_SET_LIST_ALLOC
 * Allocate a redirect list
 */
#define MC_CMD_MAE_ACTION_SET_LIST_ALLOC 0x14f
#undef MC_CMD_0x14f_PRIVILEGE_CTG

#define MC_CMD_0x14f_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_ACTION_SET_LIST_ALLOC_IN msgrequest */
#define    MC_CMD_MAE_ACTION_SET_LIST_ALLOC_IN_LENMIN 4
#define    MC_CMD_MAE_ACTION_SET_LIST_ALLOC_IN_LENMAX 252
#define    MC_CMD_MAE_ACTION_SET_LIST_ALLOC_IN_LENMAX_MCDI2 1020
#define    MC_CMD_MAE_ACTION_SET_LIST_ALLOC_IN_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_ACTION_SET_LIST_ALLOC_IN_AS_IDS_NUM(len) (((len)-0)/4)
#define       MC_CMD_MAE_ACTION_SET_LIST_ALLOC_IN_AS_IDS_OFST 0
#define       MC_CMD_MAE_ACTION_SET_LIST_ALLOC_IN_AS_IDS_LEN 4
#define       MC_CMD_MAE_ACTION_SET_LIST_ALLOC_IN_AS_IDS_MINNUM 1
#define       MC_CMD_MAE_ACTION_SET_LIST_ALLOC_IN_AS_IDS_MAXNUM 63
#define       MC_CMD_MAE_ACTION_SET_LIST_ALLOC_IN_AS_IDS_MAXNUM_MCDI2 255

/* MC_CMD_MAE_ACTION_SET_LIST_ALLOC_OUT msgresponse */
#define    MC_CMD_MAE_ACTION_SET_LIST_ALLOC_OUT_LEN 4
#define       MC_CMD_MAE_ACTION_SET_LIST_ALLOC_OUT_ASL_ID_OFST 0
#define       MC_CMD_MAE_ACTION_SET_LIST_ALLOC_OUT_ASL_ID_LEN 4
/* enum: An action set list ID that is guaranteed never to represent an action
 * set list
 */
#define          MC_CMD_MAE_ACTION_SET_LIST_ALLOC_OUT_ACTION_SET_LIST_ID_NULL 0xffffffff


/***********************************/
/* MC_CMD_MAE_ACTION_SET_LIST_FREE
 * Free match-action-engine redirect_lists
 */
#define MC_CMD_MAE_ACTION_SET_LIST_FREE 0x150
#undef MC_CMD_0x150_PRIVILEGE_CTG

#define MC_CMD_0x150_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_ACTION_SET_LIST_FREE_IN msgrequest */
#define    MC_CMD_MAE_ACTION_SET_LIST_FREE_IN_LENMIN 4
#define    MC_CMD_MAE_ACTION_SET_LIST_FREE_IN_LENMAX 128
#define    MC_CMD_MAE_ACTION_SET_LIST_FREE_IN_LENMAX_MCDI2 128
#define    MC_CMD_MAE_ACTION_SET_LIST_FREE_IN_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_ACTION_SET_LIST_FREE_IN_ASL_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_ACTION_SET_LIST_FREE_IN_ASL_ID_OFST 0
#define       MC_CMD_MAE_ACTION_SET_LIST_FREE_IN_ASL_ID_LEN 4
#define       MC_CMD_MAE_ACTION_SET_LIST_FREE_IN_ASL_ID_MINNUM 1
#define       MC_CMD_MAE_ACTION_SET_LIST_FREE_IN_ASL_ID_MAXNUM 32
#define       MC_CMD_MAE_ACTION_SET_LIST_FREE_IN_ASL_ID_MAXNUM_MCDI2 32

/* MC_CMD_MAE_ACTION_SET_LIST_FREE_OUT msgresponse */
#define    MC_CMD_MAE_ACTION_SET_LIST_FREE_OUT_LENMIN 4
#define    MC_CMD_MAE_ACTION_SET_LIST_FREE_OUT_LENMAX 128
#define    MC_CMD_MAE_ACTION_SET_LIST_FREE_OUT_LENMAX_MCDI2 128
#define    MC_CMD_MAE_ACTION_SET_LIST_FREE_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_ACTION_SET_LIST_FREE_OUT_FREED_ASL_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_ACTION_SET_LIST_FREE_OUT_FREED_ASL_ID_OFST 0
#define       MC_CMD_MAE_ACTION_SET_LIST_FREE_OUT_FREED_ASL_ID_LEN 4
#define       MC_CMD_MAE_ACTION_SET_LIST_FREE_OUT_FREED_ASL_ID_MINNUM 1
#define       MC_CMD_MAE_ACTION_SET_LIST_FREE_OUT_FREED_ASL_ID_MAXNUM 32
#define       MC_CMD_MAE_ACTION_SET_LIST_FREE_OUT_FREED_ASL_ID_MAXNUM_MCDI2 32


/***********************************/
/* MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER
 * Register an outer rule class. For rule allocation to succeed, the rule's
 * class must be supported and allocated (rule allocation includes an implicit
 * attempt to allocate the rule's class, which will fail if the class is not
 * supported), there must be sufficient rule resources, and rule's actions must
 * be supported (which a driver can determine ahead of time). Where rule
 * allocation is only permitted to fail to due lack of rule resources, drivers
 * register the relevent rule resource, guaranteeing that it is supported and
 * remains allocated until unregistered by the driver. The class to be
 * registered is identified by the priority and fields mask/value pairs of an
 * outer rule, those being the properties of an outer rule that can affect the
 * mapping from outer rule to outer rule class. See SF-122526-TC-A for further
 * details.
 */
#define MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER 0x156
#undef MC_CMD_0x156_PRIVILEGE_CTG

#define MC_CMD_0x156_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_IN msgrequest */
#define    MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_IN_LENMIN 4
#define    MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_IN_LENMAX 252
#define    MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_IN_LENMAX_MCDI2 1020
#define    MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_IN_LEN(num) (4+1*(num))
#define    MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_IN_FIELDS_NUM(len) (((len)-4)/1)
#define       MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_IN_PRIO_OFST 0
#define       MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_IN_PRIO_LEN 4
/* Structure of the format MAE_ENC_FIELD_MASK_VALUE_PAIRS */
#define       MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_IN_FIELDS_OFST 4
#define       MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_IN_FIELDS_LEN 1
#define       MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_IN_FIELDS_MINNUM 0
#define       MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_IN_FIELDS_MAXNUM 248
#define       MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_IN_FIELDS_MAXNUM_MCDI2 1016

/* MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_OUT msgresponse */
#define    MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_OUT_LEN 4
#define       MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_OUT_ORC_HANDLE_OFST 0
#define       MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_OUT_ORC_HANDLE_LEN 4
/* enum: An outer rule class handle that is guaranteed never to represent an
 * outer rule class
 */
#define          MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER_OUT_OUTER_RULE_CLASS_HANDLE_NULL 0xffffffff


/***********************************/
/* MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER
 */
#define MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER 0x157
#undef MC_CMD_0x157_PRIVILEGE_CTG

#define MC_CMD_0x157_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_IN msgrequest */
#define    MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_IN_LENMIN 4
#define    MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_IN_LENMAX 128
#define    MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_IN_LENMAX_MCDI2 128
#define    MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_IN_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_IN_ORC_HANDLE_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_IN_ORC_HANDLE_OFST 0
#define       MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_IN_ORC_HANDLE_LEN 4
#define       MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_IN_ORC_HANDLE_MINNUM 1
#define       MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_IN_ORC_HANDLE_MAXNUM 32
#define       MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_IN_ORC_HANDLE_MAXNUM_MCDI2 32

/* MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_OUT msgresponse */
#define    MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_OUT_LENMIN 4
#define    MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_OUT_LENMAX 128
#define    MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_OUT_LENMAX_MCDI2 128
#define    MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_OUT_UNREGISTERED_ORC_HANDLE_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_OUT_UNREGISTERED_ORC_HANDLE_OFST 0
#define       MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_OUT_UNREGISTERED_ORC_HANDLE_LEN 4
#define       MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_OUT_UNREGISTERED_ORC_HANDLE_MINNUM 1
#define       MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_OUT_UNREGISTERED_ORC_HANDLE_MAXNUM 32
#define       MC_CMD_MAE_OUTER_RULE_CLASS_UNREGISTER_OUT_UNREGISTERED_ORC_HANDLE_MAXNUM_MCDI2 32


/***********************************/
/* MC_CMD_MAE_OUTER_RULE_INSERT
 */
#define MC_CMD_MAE_OUTER_RULE_INSERT 0x15a
#undef MC_CMD_0x15a_PRIVILEGE_CTG

#define MC_CMD_0x15a_PRIVILEGE_CTG SRIOV_CTG_ADMIN

/* MC_CMD_MAE_OUTER_RULE_INSERT_IN msgrequest */
#define    MC_CMD_MAE_OUTER_RULE_INSERT_IN_LENMIN 16
#define    MC_CMD_MAE_OUTER_RULE_INSERT_IN_LENMAX 252
#define    MC_CMD_MAE_OUTER_RULE_INSERT_IN_LENMAX_MCDI2 1020
#define    MC_CMD_MAE_OUTER_RULE_INSERT_IN_LEN(num) (16+1*(num))
#define    MC_CMD_MAE_OUTER_RULE_INSERT_IN_FIELD_MATCH_CRITERIA_NUM(len) (((len)-16)/1)
/* Packets matching the rule will be parsed with this encapsulation. */
#define       MC_CMD_MAE_OUTER_RULE_INSERT_IN_ENCAP_TYPE_OFST 0
#define       MC_CMD_MAE_OUTER_RULE_INSERT_IN_ENCAP_TYPE_LEN 4
/*            Enum values, see field(s): */
/*               MAE_MCDI_ENCAP_TYPE */
/* Match priority. Lower values have higher priority. Must be less than
 * MC_CMD_MAE_GET_CAPABILITIES_OUT.ENCAP_PRIOS If a packet matches two filters
 * with equal priority then it is unspecified which takes priority. TODO: some
 * thought may need to go to security if distinct drivers are able to install
 * colliding rules.
 */
#define       MC_CMD_MAE_OUTER_RULE_INSERT_IN_PRIO_OFST 4
#define       MC_CMD_MAE_OUTER_RULE_INSERT_IN_PRIO_LEN 4
#define       MC_CMD_MAE_OUTER_RULE_INSERT_IN_LOOKUP_CONTROL_OFST 8
#define       MC_CMD_MAE_OUTER_RULE_INSERT_IN_LOOKUP_CONTROL_LEN 4
#define        MC_CMD_MAE_OUTER_RULE_INSERT_IN_DO_CT_OFST 8
#define        MC_CMD_MAE_OUTER_RULE_INSERT_IN_DO_CT_LBN 0
#define        MC_CMD_MAE_OUTER_RULE_INSERT_IN_DO_CT_WIDTH 1
#define        MC_CMD_MAE_OUTER_RULE_INSERT_IN_CT_VNI_MODE_OFST 8
#define        MC_CMD_MAE_OUTER_RULE_INSERT_IN_CT_VNI_MODE_LBN 1
#define        MC_CMD_MAE_OUTER_RULE_INSERT_IN_CT_VNI_MODE_WIDTH 2
/*             Enum values, see field(s): */
/*                MAE_CT_VNI_MODE */
#define        MC_CMD_MAE_OUTER_RULE_INSERT_IN_RECIRC_ID_OFST 8
#define        MC_CMD_MAE_OUTER_RULE_INSERT_IN_RECIRC_ID_LBN 8
#define        MC_CMD_MAE_OUTER_RULE_INSERT_IN_RECIRC_ID_WIDTH 8
#define        MC_CMD_MAE_OUTER_RULE_INSERT_IN_CT_DOMAIN_OFST 8
#define        MC_CMD_MAE_OUTER_RULE_INSERT_IN_CT_DOMAIN_LBN 16
#define        MC_CMD_MAE_OUTER_RULE_INSERT_IN_CT_DOMAIN_WIDTH 16
/* Reserved for future use. Must be set to zero. */
#define       MC_CMD_MAE_OUTER_RULE_INSERT_IN_RSVD_OFST 12
#define       MC_CMD_MAE_OUTER_RULE_INSERT_IN_RSVD_LEN 4
/* Structure of the format MAE_ENC_FIELD_MASK_VALUE_PAIRS. */
#define       MC_CMD_MAE_OUTER_RULE_INSERT_IN_FIELD_MATCH_CRITERIA_OFST 16
#define       MC_CMD_MAE_OUTER_RULE_INSERT_IN_FIELD_MATCH_CRITERIA_LEN 1
#define       MC_CMD_MAE_OUTER_RULE_INSERT_IN_FIELD_MATCH_CRITERIA_MINNUM 0
#define       MC_CMD_MAE_OUTER_RULE_INSERT_IN_FIELD_MATCH_CRITERIA_MAXNUM 236
#define       MC_CMD_MAE_OUTER_RULE_INSERT_IN_FIELD_MATCH_CRITERIA_MAXNUM_MCDI2 1004

/* MC_CMD_MAE_OUTER_RULE_INSERT_OUT msgresponse */
#define    MC_CMD_MAE_OUTER_RULE_INSERT_OUT_LEN 4
#define       MC_CMD_MAE_OUTER_RULE_INSERT_OUT_OR_ID_OFST 0
#define       MC_CMD_MAE_OUTER_RULE_INSERT_OUT_OR_ID_LEN 4
/* enum: An outer match ID that is guaranteed never to represent an outer match
 */
#define          MC_CMD_MAE_OUTER_RULE_INSERT_OUT_OUTER_RULE_ID_NULL 0xffffffff


/***********************************/
/* MC_CMD_MAE_OUTER_RULE_REMOVE
 */
#define MC_CMD_MAE_OUTER_RULE_REMOVE 0x15b
#undef MC_CMD_0x15b_PRIVILEGE_CTG

#define MC_CMD_0x15b_PRIVILEGE_CTG SRIOV_CTG_ADMIN

/* MC_CMD_MAE_OUTER_RULE_REMOVE_IN msgrequest */
#define    MC_CMD_MAE_OUTER_RULE_REMOVE_IN_LENMIN 4
#define    MC_CMD_MAE_OUTER_RULE_REMOVE_IN_LENMAX 128
#define    MC_CMD_MAE_OUTER_RULE_REMOVE_IN_LENMAX_MCDI2 128
#define    MC_CMD_MAE_OUTER_RULE_REMOVE_IN_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_OUTER_RULE_REMOVE_IN_OR_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_OUTER_RULE_REMOVE_IN_OR_ID_OFST 0
#define       MC_CMD_MAE_OUTER_RULE_REMOVE_IN_OR_ID_LEN 4
#define       MC_CMD_MAE_OUTER_RULE_REMOVE_IN_OR_ID_MINNUM 1
#define       MC_CMD_MAE_OUTER_RULE_REMOVE_IN_OR_ID_MAXNUM 32
#define       MC_CMD_MAE_OUTER_RULE_REMOVE_IN_OR_ID_MAXNUM_MCDI2 32

/* MC_CMD_MAE_OUTER_RULE_REMOVE_OUT msgresponse */
#define    MC_CMD_MAE_OUTER_RULE_REMOVE_OUT_LENMIN 4
#define    MC_CMD_MAE_OUTER_RULE_REMOVE_OUT_LENMAX 128
#define    MC_CMD_MAE_OUTER_RULE_REMOVE_OUT_LENMAX_MCDI2 128
#define    MC_CMD_MAE_OUTER_RULE_REMOVE_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_OUTER_RULE_REMOVE_OUT_REMOVED_OR_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_OUTER_RULE_REMOVE_OUT_REMOVED_OR_ID_OFST 0
#define       MC_CMD_MAE_OUTER_RULE_REMOVE_OUT_REMOVED_OR_ID_LEN 4
#define       MC_CMD_MAE_OUTER_RULE_REMOVE_OUT_REMOVED_OR_ID_MINNUM 1
#define       MC_CMD_MAE_OUTER_RULE_REMOVE_OUT_REMOVED_OR_ID_MAXNUM 32
#define       MC_CMD_MAE_OUTER_RULE_REMOVE_OUT_REMOVED_OR_ID_MAXNUM_MCDI2 32


/***********************************/
/* MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER
 * Same semantics as MC_CMD_MAE_OUTER_RULE_CLASS_REGISTER
 */
#define MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER 0x158
#undef MC_CMD_0x158_PRIVILEGE_CTG

#define MC_CMD_0x158_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_IN msgrequest */
#define    MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_IN_LENMIN 4
#define    MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_IN_LENMAX 252
#define    MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_IN_LENMAX_MCDI2 1020
#define    MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_IN_LEN(num) (4+1*(num))
#define    MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_IN_FIELDS_NUM(len) (((len)-4)/1)
#define       MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_IN_PRIO_OFST 0
#define       MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_IN_PRIO_LEN 4
/* Structure of the format MAE_FIELD_MASK_VALUE_PAIRS */
#define       MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_IN_FIELDS_OFST 4
#define       MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_IN_FIELDS_LEN 1
#define       MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_IN_FIELDS_MINNUM 0
#define       MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_IN_FIELDS_MAXNUM 248
#define       MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_IN_FIELDS_MAXNUM_MCDI2 1016

/* MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_OUT msgresponse */
#define    MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_OUT_LEN 4
#define       MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_OUT_ARC_HANDLE_OFST 0
#define       MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_OUT_ARC_HANDLE_LEN 4
/* enum: An action rule class handle that is guaranteed never to represent an
 * action rule class
 */
#define          MC_CMD_MAE_ACTION_RULE_CLASS_REGISTER_OUT_ACTION_RULE_CLASS_HANDLE_NULL 0xffffffff


/***********************************/
/* MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER
 */
#define MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER 0x159
#undef MC_CMD_0x159_PRIVILEGE_CTG

#define MC_CMD_0x159_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_IN msgrequest */
#define    MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_IN_LENMIN 4
#define    MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_IN_LENMAX 128
#define    MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_IN_LENMAX_MCDI2 128
#define    MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_IN_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_IN_ARC_HANDLE_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_IN_ARC_HANDLE_OFST 0
#define       MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_IN_ARC_HANDLE_LEN 4
#define       MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_IN_ARC_HANDLE_MINNUM 1
#define       MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_IN_ARC_HANDLE_MAXNUM 32
#define       MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_IN_ARC_HANDLE_MAXNUM_MCDI2 32

/* MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_OUT msgresponse */
#define    MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_OUT_LENMIN 4
#define    MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_OUT_LENMAX 128
#define    MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_OUT_LENMAX_MCDI2 128
#define    MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_OUT_UNREGISTERED_ARC_HANDLE_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_OUT_UNREGISTERED_ARC_HANDLE_OFST 0
#define       MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_OUT_UNREGISTERED_ARC_HANDLE_LEN 4
#define       MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_OUT_UNREGISTERED_ARC_HANDLE_MINNUM 1
#define       MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_OUT_UNREGISTERED_ARC_HANDLE_MAXNUM 32
#define       MC_CMD_MAE_ACTION_RULE_CLASS_UNREGISTER_OUT_UNREGISTERED_ARC_HANDLE_MAXNUM_MCDI2 32

/* MAE_ACTION_RULE_RESPONSE structuredef */
#define    MAE_ACTION_RULE_RESPONSE_LEN 16
#define       MAE_ACTION_RULE_RESPONSE_ASL_ID_OFST 0
#define       MAE_ACTION_RULE_RESPONSE_ASL_ID_LEN 4
#define       MAE_ACTION_RULE_RESPONSE_ASL_ID_LBN 0
#define       MAE_ACTION_RULE_RESPONSE_ASL_ID_WIDTH 32
/* Same semantics as COUNTER_LIST_ID vs. COUNTER_ID. Same concerns apply too.
 */
#define       MAE_ACTION_RULE_RESPONSE_AS_ID_OFST 4
#define       MAE_ACTION_RULE_RESPONSE_AS_ID_LEN 4
#define       MAE_ACTION_RULE_RESPONSE_AS_ID_LBN 32
#define       MAE_ACTION_RULE_RESPONSE_AS_ID_WIDTH 32
/* Controls lookup flow when this rule is hit. See sub-fields for details. More
 * info on the lookup sequence can be found in SF-122976-TC. It is an error to
 * set both DO_CT and DO_RECIRC.
 */
#define       MAE_ACTION_RULE_RESPONSE_LOOKUP_CONTROL_OFST 8
#define       MAE_ACTION_RULE_RESPONSE_LOOKUP_CONTROL_LEN 4
#define        MAE_ACTION_RULE_RESPONSE_DO_CT_OFST 8
#define        MAE_ACTION_RULE_RESPONSE_DO_CT_LBN 0
#define        MAE_ACTION_RULE_RESPONSE_DO_CT_WIDTH 1
#define        MAE_ACTION_RULE_RESPONSE_DO_RECIRC_OFST 8
#define        MAE_ACTION_RULE_RESPONSE_DO_RECIRC_LBN 1
#define        MAE_ACTION_RULE_RESPONSE_DO_RECIRC_WIDTH 1
#define        MAE_ACTION_RULE_RESPONSE_CT_VNI_MODE_OFST 8
#define        MAE_ACTION_RULE_RESPONSE_CT_VNI_MODE_LBN 2
#define        MAE_ACTION_RULE_RESPONSE_CT_VNI_MODE_WIDTH 2
/*             Enum values, see field(s): */
/*                MAE_CT_VNI_MODE */
#define        MAE_ACTION_RULE_RESPONSE_RECIRC_ID_OFST 8
#define        MAE_ACTION_RULE_RESPONSE_RECIRC_ID_LBN 8
#define        MAE_ACTION_RULE_RESPONSE_RECIRC_ID_WIDTH 8
#define        MAE_ACTION_RULE_RESPONSE_CT_DOMAIN_OFST 8
#define        MAE_ACTION_RULE_RESPONSE_CT_DOMAIN_LBN 16
#define        MAE_ACTION_RULE_RESPONSE_CT_DOMAIN_WIDTH 16
#define       MAE_ACTION_RULE_RESPONSE_LOOKUP_CONTROL_LBN 64
#define       MAE_ACTION_RULE_RESPONSE_LOOKUP_CONTROL_WIDTH 32
/* Counter ID to increment if DO_CT or DO_RECIRC is set. Must be set to
 * COUNTER_ID_NULL otherwise.
 */
#define       MAE_ACTION_RULE_RESPONSE_COUNTER_ID_OFST 12
#define       MAE_ACTION_RULE_RESPONSE_COUNTER_ID_LEN 4
#define       MAE_ACTION_RULE_RESPONSE_COUNTER_ID_LBN 96
#define       MAE_ACTION_RULE_RESPONSE_COUNTER_ID_WIDTH 32


/***********************************/
/* MC_CMD_MAE_ACTION_RULE_INSERT
 * Insert a rule specify that packets matching a filter be processed according
 * to a previous allocated action. Masks can be set as indicated by
 * MC_CMD_MAE_GET_MATCH_FIELD_CAPABILITIES TODO: see thoughts in outer_rule
 * about naming.
 */
#define MC_CMD_MAE_ACTION_RULE_INSERT 0x15c
#undef MC_CMD_0x15c_PRIVILEGE_CTG

#define MC_CMD_0x15c_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_ACTION_RULE_INSERT_IN msgrequest */
#define    MC_CMD_MAE_ACTION_RULE_INSERT_IN_LENMIN 28
#define    MC_CMD_MAE_ACTION_RULE_INSERT_IN_LENMAX 252
#define    MC_CMD_MAE_ACTION_RULE_INSERT_IN_LENMAX_MCDI2 1020
#define    MC_CMD_MAE_ACTION_RULE_INSERT_IN_LEN(num) (28+1*(num))
#define    MC_CMD_MAE_ACTION_RULE_INSERT_IN_MATCH_CRITERIA_NUM(len) (((len)-28)/1)
/* See MC_CMD_MAE_OUTER_RULE_REGISTER_IN/PRIO. */
#define       MC_CMD_MAE_ACTION_RULE_INSERT_IN_PRIO_OFST 0
#define       MC_CMD_MAE_ACTION_RULE_INSERT_IN_PRIO_LEN 4
/* Structure of the format MAE_ACTION_RULE_RESPONSE */
#define       MC_CMD_MAE_ACTION_RULE_INSERT_IN_RESPONSE_OFST 4
#define       MC_CMD_MAE_ACTION_RULE_INSERT_IN_RESPONSE_LEN 20
/* Reserved for future use. Must be set to zero. */
#define       MC_CMD_MAE_ACTION_RULE_INSERT_IN_RSVD_OFST 24
#define       MC_CMD_MAE_ACTION_RULE_INSERT_IN_RSVD_LEN 4
/* Structure of the format MAE_FIELD_MASK_VALUE_PAIRS */
#define       MC_CMD_MAE_ACTION_RULE_INSERT_IN_MATCH_CRITERIA_OFST 28
#define       MC_CMD_MAE_ACTION_RULE_INSERT_IN_MATCH_CRITERIA_LEN 1
#define       MC_CMD_MAE_ACTION_RULE_INSERT_IN_MATCH_CRITERIA_MINNUM 0
#define       MC_CMD_MAE_ACTION_RULE_INSERT_IN_MATCH_CRITERIA_MAXNUM 224
#define       MC_CMD_MAE_ACTION_RULE_INSERT_IN_MATCH_CRITERIA_MAXNUM_MCDI2 992

/* MC_CMD_MAE_ACTION_RULE_INSERT_OUT msgresponse */
#define    MC_CMD_MAE_ACTION_RULE_INSERT_OUT_LEN 4
#define       MC_CMD_MAE_ACTION_RULE_INSERT_OUT_AR_ID_OFST 0
#define       MC_CMD_MAE_ACTION_RULE_INSERT_OUT_AR_ID_LEN 4
/* enum: An action rule ID that is guaranteed never to represent an action rule
 */
#define          MC_CMD_MAE_ACTION_RULE_INSERT_OUT_ACTION_RULE_ID_NULL 0xffffffff


/***********************************/
/* MC_CMD_MAE_ACTION_RULE_UPDATE
 * Atomically change the response of an action rule. Firmware may return
 * ENOTSUP, in which case the driver should DELETE/INSERT.
 */
#define MC_CMD_MAE_ACTION_RULE_UPDATE 0x15d
#undef MC_CMD_0x15d_PRIVILEGE_CTG

#define MC_CMD_0x15d_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_ACTION_RULE_UPDATE_IN msgrequest */
#define    MC_CMD_MAE_ACTION_RULE_UPDATE_IN_LEN 24
/* ID of action rule to update */
#define       MC_CMD_MAE_ACTION_RULE_UPDATE_IN_AR_ID_OFST 0
#define       MC_CMD_MAE_ACTION_RULE_UPDATE_IN_AR_ID_LEN 4
/* Structure of the format MAE_ACTION_RULE_RESPONSE */
#define       MC_CMD_MAE_ACTION_RULE_UPDATE_IN_RESPONSE_OFST 4
#define       MC_CMD_MAE_ACTION_RULE_UPDATE_IN_RESPONSE_LEN 20

/* MC_CMD_MAE_ACTION_RULE_UPDATE_OUT msgresponse */
#define    MC_CMD_MAE_ACTION_RULE_UPDATE_OUT_LEN 0


/***********************************/
/* MC_CMD_MAE_ACTION_RULE_DELETE
 */
#define MC_CMD_MAE_ACTION_RULE_DELETE 0x155
#undef MC_CMD_0x155_PRIVILEGE_CTG

#define MC_CMD_0x155_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_ACTION_RULE_DELETE_IN msgrequest */
#define    MC_CMD_MAE_ACTION_RULE_DELETE_IN_LENMIN 4
#define    MC_CMD_MAE_ACTION_RULE_DELETE_IN_LENMAX 128
#define    MC_CMD_MAE_ACTION_RULE_DELETE_IN_LENMAX_MCDI2 128
#define    MC_CMD_MAE_ACTION_RULE_DELETE_IN_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_ACTION_RULE_DELETE_IN_AR_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_ACTION_RULE_DELETE_IN_AR_ID_OFST 0
#define       MC_CMD_MAE_ACTION_RULE_DELETE_IN_AR_ID_LEN 4
#define       MC_CMD_MAE_ACTION_RULE_DELETE_IN_AR_ID_MINNUM 1
#define       MC_CMD_MAE_ACTION_RULE_DELETE_IN_AR_ID_MAXNUM 32
#define       MC_CMD_MAE_ACTION_RULE_DELETE_IN_AR_ID_MAXNUM_MCDI2 32

/* MC_CMD_MAE_ACTION_RULE_DELETE_OUT msgresponse */
#define    MC_CMD_MAE_ACTION_RULE_DELETE_OUT_LENMIN 4
#define    MC_CMD_MAE_ACTION_RULE_DELETE_OUT_LENMAX 128
#define    MC_CMD_MAE_ACTION_RULE_DELETE_OUT_LENMAX_MCDI2 128
#define    MC_CMD_MAE_ACTION_RULE_DELETE_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_ACTION_RULE_DELETE_OUT_DELETED_AR_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_ACTION_RULE_DELETE_OUT_DELETED_AR_ID_OFST 0
#define       MC_CMD_MAE_ACTION_RULE_DELETE_OUT_DELETED_AR_ID_LEN 4
#define       MC_CMD_MAE_ACTION_RULE_DELETE_OUT_DELETED_AR_ID_MINNUM 1
#define       MC_CMD_MAE_ACTION_RULE_DELETE_OUT_DELETED_AR_ID_MAXNUM 32
#define       MC_CMD_MAE_ACTION_RULE_DELETE_OUT_DELETED_AR_ID_MAXNUM_MCDI2 32


/***********************************/
/* MC_CMD_MAE_MPORT_LOOKUP
 */
#define MC_CMD_MAE_MPORT_LOOKUP 0x160
#undef MC_CMD_0x160_PRIVILEGE_CTG

#define MC_CMD_0x160_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_MPORT_LOOKUP_IN msgrequest */
#define    MC_CMD_MAE_MPORT_LOOKUP_IN_LEN 4
#define       MC_CMD_MAE_MPORT_LOOKUP_IN_MPORT_SELECTOR_OFST 0
#define       MC_CMD_MAE_MPORT_LOOKUP_IN_MPORT_SELECTOR_LEN 4

/* MC_CMD_MAE_MPORT_LOOKUP_OUT msgresponse */
#define    MC_CMD_MAE_MPORT_LOOKUP_OUT_LEN 4
#define       MC_CMD_MAE_MPORT_LOOKUP_OUT_MPORT_ID_OFST 0
#define       MC_CMD_MAE_MPORT_LOOKUP_OUT_MPORT_ID_LEN 4


/***********************************/
/* MC_CMD_MAE_MPORT_ALLOC
 * Allocates a m-port, which can subsequently be used in action rules as a
 * match or delivery argument.
 */
#define MC_CMD_MAE_MPORT_ALLOC 0x163
#undef MC_CMD_0x163_PRIVILEGE_CTG

#define MC_CMD_0x163_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_MPORT_ALLOC_IN msgrequest */
#define    MC_CMD_MAE_MPORT_ALLOC_IN_LEN 20
/* The type of m-port to allocate. */
#define       MC_CMD_MAE_MPORT_ALLOC_IN_TYPE_OFST 0
#define       MC_CMD_MAE_MPORT_ALLOC_IN_TYPE_LEN 4
/* enum: Traffic can be sent to this type of m-port using an override
 * descriptor. Traffic received on this type of m-port will go to the VNIC on a
 * nominated m-port, and will be delivered with metadata identifying the alias
 * m-port.
 */
#define          MC_CMD_MAE_MPORT_ALLOC_IN_MPORT_TYPE_ALIAS 0x1
/* enum: This type of m-port has a VNIC attached. Queues can be created on this
 * VNIC by specifying the created m-port as an m-port selector at queue
 * creation time. This type of m-port is not yet supported.
 */
#define          MC_CMD_MAE_MPORT_ALLOC_IN_MPORT_TYPE_VNIC 0x2
/* 128-bit value for use by the driver. */
#define       MC_CMD_MAE_MPORT_ALLOC_IN_UUID_OFST 4
#define       MC_CMD_MAE_MPORT_ALLOC_IN_UUID_LEN 16

/* MC_CMD_MAE_MPORT_ALLOC_ALIAS_IN msgrequest */
#define    MC_CMD_MAE_MPORT_ALLOC_ALIAS_IN_LEN 24
/* The type of m-port to allocate. */
#define       MC_CMD_MAE_MPORT_ALLOC_ALIAS_IN_TYPE_OFST 0
#define       MC_CMD_MAE_MPORT_ALLOC_ALIAS_IN_TYPE_LEN 4
/* enum: Traffic can be sent to this type of m-port using an override
 * descriptor. Traffic received on this type of m-port will go to the VNIC on a
 * nominated m-port, and will be delivered with metadata identifying the alias
 * m-port.
 */
#define          MC_CMD_MAE_MPORT_ALLOC_ALIAS_IN_MPORT_TYPE_ALIAS 0x1
/* enum: This type of m-port has a VNIC attached. Queues can be created on this
 * VNIC by specifying the created m-port as an m-port selector at queue
 * creation time. This type of m-port is not yet supported.
 */
#define          MC_CMD_MAE_MPORT_ALLOC_ALIAS_IN_MPORT_TYPE_VNIC 0x2
/* 128-bit value for use by the driver. */
#define       MC_CMD_MAE_MPORT_ALLOC_ALIAS_IN_UUID_OFST 4
#define       MC_CMD_MAE_MPORT_ALLOC_ALIAS_IN_UUID_LEN 16
/* An m-port selector identifying the VNIC to which traffic should be
 * delivered. This must currently be set to MAE_MPORT_SELECTOR_ASSIGNED (i.e.
 * the m-port assigned to the calling client).
 */
#define       MC_CMD_MAE_MPORT_ALLOC_ALIAS_IN_DELIVER_MPORT_OFST 20
#define       MC_CMD_MAE_MPORT_ALLOC_ALIAS_IN_DELIVER_MPORT_LEN 4

/* MC_CMD_MAE_MPORT_ALLOC_VNIC_IN msgrequest */
#define    MC_CMD_MAE_MPORT_ALLOC_VNIC_IN_LEN 20
/* The type of m-port to allocate. */
#define       MC_CMD_MAE_MPORT_ALLOC_VNIC_IN_TYPE_OFST 0
#define       MC_CMD_MAE_MPORT_ALLOC_VNIC_IN_TYPE_LEN 4
/* enum: Traffic can be sent to this type of m-port using an override
 * descriptor. Traffic received on this type of m-port will go to the VNIC on a
 * nominated m-port, and will be delivered with metadata identifying the alias
 * m-port.
 */
#define          MC_CMD_MAE_MPORT_ALLOC_VNIC_IN_MPORT_TYPE_ALIAS 0x1
/* enum: This type of m-port has a VNIC attached. Queues can be created on this
 * VNIC by specifying the created m-port as an m-port selector at queue
 * creation time. This type of m-port is not yet supported.
 */
#define          MC_CMD_MAE_MPORT_ALLOC_VNIC_IN_MPORT_TYPE_VNIC 0x2
/* 128-bit value for use by the driver. */
#define       MC_CMD_MAE_MPORT_ALLOC_VNIC_IN_UUID_OFST 4
#define       MC_CMD_MAE_MPORT_ALLOC_VNIC_IN_UUID_LEN 16

/* MC_CMD_MAE_MPORT_ALLOC_OUT msgresponse */
#define    MC_CMD_MAE_MPORT_ALLOC_OUT_LEN 4
/* ID of newly-allocated m-port. */
#define       MC_CMD_MAE_MPORT_ALLOC_OUT_MPORT_ID_OFST 0
#define       MC_CMD_MAE_MPORT_ALLOC_OUT_MPORT_ID_LEN 4

/* MC_CMD_MAE_MPORT_ALLOC_ALIAS_OUT msgrequest */
#define    MC_CMD_MAE_MPORT_ALLOC_ALIAS_OUT_LEN 24
/* ID of newly-allocated m-port. */
#define       MC_CMD_MAE_MPORT_ALLOC_ALIAS_OUT_MPORT_ID_OFST 0
#define       MC_CMD_MAE_MPORT_ALLOC_ALIAS_OUT_MPORT_ID_LEN 4
/* A value that will appear in the packet metadata for any packets delivered
 * using an alias type m-port. This value is guaranteed unique on the VNIC
 * being delivered to, and is guaranteed not to exceed the range of values
 * representable in the relevant metadata field.
 */
#define       MC_CMD_MAE_MPORT_ALLOC_ALIAS_OUT_LABEL_OFST 20
#define       MC_CMD_MAE_MPORT_ALLOC_ALIAS_OUT_LABEL_LEN 4

/* MC_CMD_MAE_MPORT_ALLOC_VNIC_OUT msgrequest */
#define    MC_CMD_MAE_MPORT_ALLOC_VNIC_OUT_LEN 4
/* ID of newly-allocated m-port. */
#define       MC_CMD_MAE_MPORT_ALLOC_VNIC_OUT_MPORT_ID_OFST 0
#define       MC_CMD_MAE_MPORT_ALLOC_VNIC_OUT_MPORT_ID_LEN 4


/***********************************/
/* MC_CMD_MAE_MPORT_FREE
 */
#define MC_CMD_MAE_MPORT_FREE 0x164
#undef MC_CMD_0x164_PRIVILEGE_CTG

#define MC_CMD_0x164_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_MPORT_FREE_IN msgrequest */
#define    MC_CMD_MAE_MPORT_FREE_IN_LEN 4
/* MPORT_ID as returned by MC_CMD_MAE_MPORT_ALLOC. */
#define       MC_CMD_MAE_MPORT_FREE_IN_MPORT_ID_OFST 0
#define       MC_CMD_MAE_MPORT_FREE_IN_MPORT_ID_LEN 4

/* MC_CMD_MAE_MPORT_FREE_OUT msgresponse */
#define    MC_CMD_MAE_MPORT_FREE_OUT_LEN 0


/***********************************/
/* MC_CMD_MAE_MPORT_REUUID
 * Replace the UUID for an existing m-port.
 */
#define MC_CMD_MAE_MPORT_REUUID 0x170
#undef MC_CMD_0x170_PRIVILEGE_CTG

#define MC_CMD_0x170_PRIVILEGE_CTG SRIOV_CTG_GENERAL

/* MC_CMD_MAE_MPORT_REUUID_IN msgrequest */
#define    MC_CMD_MAE_MPORT_REUUID_IN_LEN 20
/* MPORT_ID as returned by MC_CMD_MAE_MPORT_ALLOC. */
#define       MC_CMD_MAE_MPORT_REUUID_IN_MPORT_ID_OFST 0
#define       MC_CMD_MAE_MPORT_REUUID_IN_MPORT_ID_LEN 4
/* 128-bit value for use by the driver. */
#define       MC_CMD_MAE_MPORT_REUUID_IN_UUID_OFST 4
#define       MC_CMD_MAE_MPORT_REUUID_IN_UUID_LEN 16

/* MC_CMD_MAE_MPORT_REUUID_OUT msgresponse */
#define    MC_CMD_MAE_MPORT_REUUID_OUT_LEN 0


/***********************************/
/* MC_CMD_MAE_TRACK_CONNECTION
 * Insert an entry into the connection tracking table. The lookup sequence is
 * described in SF-122976-TC.
 */
#define MC_CMD_MAE_TRACK_CONNECTION 0x17a
#undef MC_CMD_0x17a_PRIVILEGE_CTG

#define MC_CMD_0x17a_PRIVILEGE_CTG SRIOV_CTG_ADMIN

/* MC_CMD_MAE_TRACK_CONNECTION_IN msgrequest */
#define    MC_CMD_MAE_TRACK_CONNECTION_IN_LEN 54
/* See following fields. All other bits must be set to zero. */
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_FLAGS_OFST 0
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_FLAGS_LEN 2
#define        MC_CMD_MAE_TRACK_CONNECTION_IN_IS_IPV6_OFST 0
#define        MC_CMD_MAE_TRACK_CONNECTION_IN_IS_IPV6_LBN 0
#define        MC_CMD_MAE_TRACK_CONNECTION_IN_IS_IPV6_WIDTH 1
#define        MC_CMD_MAE_TRACK_CONNECTION_IN_IS_UDP_OFST 0
#define        MC_CMD_MAE_TRACK_CONNECTION_IN_IS_UDP_LBN 1
#define        MC_CMD_MAE_TRACK_CONNECTION_IN_IS_UDP_WIDTH 1
#define        MC_CMD_MAE_TRACK_CONNECTION_IN_NAT_DIR_IS_DST_OFST 0
#define        MC_CMD_MAE_TRACK_CONNECTION_IN_NAT_DIR_IS_DST_LBN 2
#define        MC_CMD_MAE_TRACK_CONNECTION_IN_NAT_DIR_IS_DST_WIDTH 1
#define        MC_CMD_MAE_TRACK_CONNECTION_IN_PRIVATE_FLAGS_OFST 0
#define        MC_CMD_MAE_TRACK_CONNECTION_IN_PRIVATE_FLAGS_LBN 8
#define        MC_CMD_MAE_TRACK_CONNECTION_IN_PRIVATE_FLAGS_WIDTH 8
/* Domain as given when CT was requested. Analogous to ct_zone software field.
 */
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_DOMAIN_OFST 2
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_DOMAIN_LEN 2
/* Source IP address to match. IPv4 should be in first 4 bytes with other bytes
 * zero.
 */
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_SRC_ADDR_OFST 4
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_SRC_ADDR_LEN 16
/* Destination IP address to match. IPv4 should be in first 4 bytes with other
 * bytes zero.
 */
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_DST_ADDR_OFST 20
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_DST_ADDR_LEN 16
/* Source TCP or UDP port to match. */
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_SRC_PORT_OFST 36
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_SRC_PORT_LEN 2
/* Destination TCP or UDP port to match. */
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_DST_PORT_OFST 38
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_DST_PORT_LEN 2
/* Match input. Depending on CT_VNI_MODE, could contain VNI, outer VLAN, both
 * VLANs, or zero.
 */
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_VNI_OR_VLANS_OR_ZERO_OFST 40
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_VNI_OR_VLANS_OR_ZERO_LEN 3
/* Match input. Discriminator for VNI_OR_VLANS_OR_ZERO field. */
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_CT_VNI_MODE_LBN 344
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_CT_VNI_MODE_WIDTH 2
/*            Enum values, see field(s): */
/*               MAE_CT_VNI_MODE */
/* Mark output, will be given to following ACTION_RULE lookup. */
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_CT_MARK_OFST 44
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_CT_MARK_LEN 4
/* If subsequent ACTION_RULE hit enables NAT, this IP will be used. */
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_NAT_IP_OFST 48
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_NAT_IP_LEN 4
/* If subsequent ACTION_RULE hit enables NAT, this port will be used. */
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_NAT_PORT_OFST 52
#define       MC_CMD_MAE_TRACK_CONNECTION_IN_NAT_PORT_LEN 2

/* MC_CMD_MAE_TRACK_CONNECTION_OUT msgresponse */
#define    MC_CMD_MAE_TRACK_CONNECTION_OUT_LEN 4
/* ID to use for deletion. */
#define       MC_CMD_MAE_TRACK_CONNECTION_OUT_CONN_ID_OFST 0
#define       MC_CMD_MAE_TRACK_CONNECTION_OUT_CONN_ID_LEN 4
/* enum: A connection ID that is guaranteed never to represent a connection. */
#define          MC_CMD_MAE_TRACK_CONNECTION_OUT_CONN_ID_NULL 0xffffffff


/***********************************/
/* MC_CMD_MAE_FORGET_CONNECTION
 */
#define MC_CMD_MAE_FORGET_CONNECTION 0x17b
#undef MC_CMD_0x17b_PRIVILEGE_CTG

#define MC_CMD_0x17b_PRIVILEGE_CTG SRIOV_CTG_ADMIN

/* MC_CMD_MAE_FORGET_CONNECTION_IN msgrequest */
#define    MC_CMD_MAE_FORGET_CONNECTION_IN_LENMIN 4
#define    MC_CMD_MAE_FORGET_CONNECTION_IN_LENMAX 128
#define    MC_CMD_MAE_FORGET_CONNECTION_IN_LENMAX_MCDI2 128
#define    MC_CMD_MAE_FORGET_CONNECTION_IN_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_FORGET_CONNECTION_IN_CONN_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_FORGET_CONNECTION_IN_CONN_ID_OFST 0
#define       MC_CMD_MAE_FORGET_CONNECTION_IN_CONN_ID_LEN 4
#define       MC_CMD_MAE_FORGET_CONNECTION_IN_CONN_ID_MINNUM 1
#define       MC_CMD_MAE_FORGET_CONNECTION_IN_CONN_ID_MAXNUM 32
#define       MC_CMD_MAE_FORGET_CONNECTION_IN_CONN_ID_MAXNUM_MCDI2 32

/* MC_CMD_MAE_FORGET_CONNECTION_OUT msgresponse */
#define    MC_CMD_MAE_FORGET_CONNECTION_OUT_LENMIN 4
#define    MC_CMD_MAE_FORGET_CONNECTION_OUT_LENMAX 128
#define    MC_CMD_MAE_FORGET_CONNECTION_OUT_LENMAX_MCDI2 128
#define    MC_CMD_MAE_FORGET_CONNECTION_OUT_LEN(num) (0+4*(num))
#define    MC_CMD_MAE_FORGET_CONNECTION_OUT_REMOVED_CONN_ID_NUM(len) (((len)-0)/4)
/* Same semantics as MC_CMD_MAE_COUNTER_FREE */
#define       MC_CMD_MAE_FORGET_CONNECTION_OUT_REMOVED_CONN_ID_OFST 0
#define       MC_CMD_MAE_FORGET_CONNECTION_OUT_REMOVED_CONN_ID_LEN 4
#define       MC_CMD_MAE_FORGET_CONNECTION_OUT_REMOVED_CONN_ID_MINNUM 1
#define       MC_CMD_MAE_FORGET_CONNECTION_OUT_REMOVED_CONN_ID_MAXNUM 32
#define       MC_CMD_MAE_FORGET_CONNECTION_OUT_REMOVED_CONN_ID_MAXNUM_MCDI2 32


#endif /* MCDI_PCOL_MAE_H */
