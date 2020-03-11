# SPDX-License-Identifier: GPL-2.0
# X-SPDX-Copyright-Text: (c) Solarflare Communications Inc
TARGET		:= $(CIIP_LIB)
MMAKE_TYPE	:= LIB

LIB_SRCS	:=		\
		netif.c		\
		iptimer.c	\
		netif_event.c	\
		netif_tx.c	\
		netif_table.c	\
		netif_table_ip6.c	\
		netif_pkt.c	\
		tcp_misc.c	\
		tcp_rx.c	\
		tcp_sleep.c	\
		tcp_synrecv.c	\
		tcp_tx.c	\
		tcp_tx_reformat.c \
		tcp_timer.c	\
		tcp_close.c	\
		tcp_init_shared.c \
		pmtu.c		\
		ip_tx.c		\
		udp.c		\
		udp_rx.c	\
		udp_connect.c	\
		udp_misc.c	\
		icmp_send.c	\
		tcp_stats.c	\
		netif_stats.c	\
		tcp_send.c	\
		tcp_recv.c	\
		netif_debug.c	\
		tcp_debug.c	\
		csum_copy_iovec_setlen.c \
		cplane_ops.c	\
		netif_init.c	\
		tcp_connect.c	\
		waitable.c	\
		socket.c	\
		ip_cmsg.c	\
		eplock_slow.c	\
		udp_recv.c	\
		udp_send.c	\
		os_sock.c	\
		pkt_filler.c	\
		pio_buddy.c	\
		pipe.c		\
		common_sockopts.c \
		tcp_sockopts.c	\
		tcp_syncookie.c	\
		active_wild.c	\
		pkt_checksum.c

ifneq ($(DRIVER),1)
LIB_SRCS	+=		\
		tcp_ioctl.c	\
		init.c		\
		udp_sockopts.c	\
		udp_ioctl.c	\
		signal.c	\
		common_ioctl.c  \
                efabcfg.c       \
		save_fd.c	\
		tcp_helper.c	\
		syscall.c	\
		per_thread.c	\
		rwlock.c
endif

ifeq ($(DRIVER),1)
LIB_SRCS	+=              \
		tcp_driver_ops.c
endif

ifneq ($(strip $(CI_PREBUILT_IPDRV)),)
CI_SEPARATE_IP_BUILD := 1
endif
ifneq ($(strip $(CI_SEPARATE_IP_BUILD)),)
CPPFLAGS += -DCI_SEPARATE_IP_BUILD -DCI_DEPENDENT_DRIVER
endif

ifndef MMAKE_NO_RULES

MMAKE_OBJ_PREFIX := ci_ip_
LIB_OBJS	:= $(LIB_SRCS:%.c=$(MMAKE_OBJ_PREFIX)%.o)


ALL		:= $(TARGET)


all: $(ALL)

lib: $(TARGET)

clean:
	@$(MakeClean)
	rm -f uk_intf_ver.h

$(LIB_OBJS): $(CP_INTF_VER_HDR)

$(TARGET): $(LIB_OBJS)
	$(MMakeLinkStaticLib)


ifeq ($(LINUX),1)
# gcc v4 in FC4 incorrectly complains about uninitialised variables, so we
# switch off the test for this file
$(MMAKE_OBJ_PREFIX)udp_recv.o: cwarnings += -Wno-uninitialized
endif

endif

MMAKE_CFLAGS += $(CP_INTF_VER_CFLAGS)

######################################################################
# Autogenerated header for checking user/kernel consistency.
#
_UK_INTF_HDRS	:= ci/internal/ip_shared_types.h	\
		ci/internal/transport_config_opt.h      \
		$(TRANSPORT_CONFIG_OPT_HDR)		\
		ci/internal/opts_netif_def.h		\
		ci/internal/tcp_stats_count_def.h	\
		ci/internal/tcp_ext_stats_count_def.h	\
		ci/internal/ni_dllist.h                 \
		ci/internal/ni_dllist_tmpl_instantiate.h \
		onload/common.h				\
		onload/primitive_types.h		\
		onload/ioctl.h				\
		onload/ioctl_base.h			\
		onload/ioctl_dshm.h			\
		cplane/ioctl.h				\
		onload/dshm.h				\
		onload/mmap.h				\
		onload/mmap_base.h			\
		cplane/mmap.h

UK_INTF_HDRS	:= $(_UK_INTF_HDRS:%=$(SRCPATH)/include/%)

ifdef MMAKE_USE_KBUILD
objd	:= $(obj)/
else
objd	:=
endif

$(objd)uk_intf_ver.h: $(UK_INTF_HDRS)
	@echo "  GENERATE $@"
	@md5=$$(cat $(UK_INTF_HDRS) | md5sum | sed 's/ .*//'); \
	echo "#define OO_UK_INTF_VER  \"$$md5\"" >"$@"

$(objd)$(MMAKE_OBJ_PREFIX)netif_init.o: $(objd)uk_intf_ver.h
$(objd)$(MMAKE_OBJ_PREFIX)netif_debug.o: $(objd)uk_intf_ver.h


######################################################
# linux kbuild support
#
ifdef MMAKE_USE_KBUILD

all:
	 $(MAKE) $(MMAKE_KBUILD_ARGS) KBUILD_EXTMOD=$(BUILDPATH)/lib/transport/ip
	 $(LD) -r $(LIB_SRCS:%.c=%.o) -o ci_ip_lib.o
clean:
	@$(MakeClean)
	rm -f uk_intf_ver.h ci_ip_lib.o
endif

ifdef MMAKE_IN_KBUILD
LIB_OBJS := $(LIB_SRCS:%.c=%.o)
obj-y    := $(LIB_OBJS)
endif


echo_LIB_SRCS:
	@echo $(LIB_SRCS)
