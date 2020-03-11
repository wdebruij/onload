# SPDX-License-Identifier: GPL-2.0
# X-SPDX-Copyright-Text: (c) Solarflare Communications Inc
############################
# 
# EtherFabric linux kernel drivers 
#
#	sfc_char
#
############################


CHAR_SRCS	:= char_driver.c mmap.c efch_manager.c resource_table.c \
		mmap_iopage.c vi_resource.c vi_resource_mmap.c \
		vi_resource_event.c vi_set.c filter_list.c \
		efch_memreg.c efch_pd.c pio.c efch_license.c efch_capabilities.c

CHAR_TARGET		:= sfc_char.o
CHAR_TARGET_SRCS	:= $(CHAR_SRCS)
TARGETS			:= $(CHAR_TARGET)


######################################################################
# Autogenerated header for checking user/kernel interface consistency.
#
_EFCH_INTF_HDRS	:= ci/efch/op_types.h
EFCH_INTF_HDRS	:= $(_EFCH_INTF_HDRS:%=$(SRCPATH)/include/%)

ifdef MMAKE_USE_KBUILD
objd	:= $(obj)/
else
objd	:=
endif

$(objd)efch_intf_ver.h: $(EFCH_INTF_HDRS)
	@echo "  GENERATE $@"
	@md5=$$(cat $(EFCH_INTF_HDRS) | grep -v '^[ *]\*' | \
		md5sum | sed 's/ .*//'); \
	echo "#define EFCH_INTF_VER  \"$$md5\"" >"$@"

$(objd)efch_manager.o: $(objd)efch_intf_ver.h


######################################################
# linux kbuild support
#

KBUILD_EXTRA_SYMBOLS := $(BUILDPATH)/driver/linux_resource/Module.symvers

all: $(KBUILD_EXTRA_SYMBOLS)
	$(MAKE) $(MMAKE_KBUILD_ARGS) M=$(CURDIR)
	cp -f sfc_char.ko $(BUILDPATH)/driver/linux
ifndef CI_FROM_DRIVER
	$(warning "Due to build order sfc.ko may be out-of-date. Please build in driver/linux_net")
endif

clean:
	@$(MakeClean)
	rm -rf *.ko Module.symvers .*.cmd


ifdef MMAKE_IN_KBUILD
LIB_OBJS := $(LIB_SRCS:%.c=%.o)

obj-m := $(CHAR_TARGET) 

sfc_char-objs := $(CHAR_TARGET_SRCS:%.c=%.o) \
		$(BUILD)/lib/citools/citools_lib.o \
		$(BUILD)/lib/ciul/ci_ul_lib.o

endif # MMAKE_IN_KBUILD
