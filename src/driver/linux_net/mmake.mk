# SPDX-License-Identifier: GPL-2.0
# X-SPDX-Copyright-Text: (c) 2005-2019 Solarflare Communications Inc

SUBDIRS := util

ifeq ($(DRIVER),1)
  ifeq ($(MMAKE_LIBERAL),1)
    NOWERROR := 1
  endif

  include $(TOPPATH)/$(CURRENT)/Makefile

  ../linux/sfc.ko: modules
	cp -f sfc.ko $@

  ../linux/sfc_ef100.ko: modules
	cp -f sfc_ef100.ko $@

  ../linux/sfc_driverlink.ko: modules
	cp -f sfc_driverlink.ko $@

  all: ../linux/sfc.ko ../linux/sfc_ef100.ko ../linux/sfc_driverlink.ko
endif
