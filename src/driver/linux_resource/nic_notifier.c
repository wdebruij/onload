/* SPDX-License-Identifier: GPL-2.0 */
/* X-SPDX-Copyright-Text: (c) Copyright 2020 Xilinx, Inc. */


#include <linux/rtnetlink.h>
#include <ci/efhw/nic.h>
#include <ci/efrm/efrm_client.h>
#include <ci/efrm/efrm_filter.h>
#include <ci/efrm/nic_notifier.h>
#include <ci/efrm/nic_table.h>
#include "linux_resource_internal.h"


static struct efrm_nic_notifier *registered_notifier;

static int efrm_netdev_event(struct notifier_block *this,
			     unsigned long event, void *ptr);

static struct notifier_block efrm_netdev_notifier = {
	.notifier_call = efrm_netdev_event,
};

struct nic_dev {
  struct efhw_nic* nic;
  struct list_head list;
};


void efrm_notify_for_each_nic(bool probe)
{
  struct efhw_nic* nic;
  int nic_index;

  struct nic_dev* nic_dev;
  struct nic_dev* temp;
  struct list_head nics;

  INIT_LIST_HEAD(&nics);

  ASSERT_RTNL();

  /* Obtain a list of devs that we need to notify. This is a two stage
   * process, because we can't do the notify itself with the nic table lock
   * held.
   */
  spin_lock_bh(&efrm_nic_tablep->lock);
  EFRM_FOR_EACH_NIC(nic_index, nic) {
    nic_dev = kzalloc(sizeof(*nic_dev), GFP_ATOMIC);

    if( nic_dev ) {
      nic_dev->nic = nic;
      list_add(&nic_dev->list, &nics);
    }
    else {
      EFRM_ERR("Failed to allocate for notify of %s", dev_name(nic->dev));
    }
  }
  spin_unlock_bh(&efrm_nic_tablep->lock);
 
  list_for_each_entry_safe_reverse(nic_dev, temp, &nics, list) {
    if( probe && registered_notifier ) {
      struct net_device* net_dev = efhw_nic_get_net_dev(nic_dev->nic);
      if( net_dev ) {
        registered_notifier->probe(nic_dev->nic, net_dev);
        dev_put(net_dev);
      }
    }
    else if( registered_notifier ) {
      registered_notifier->remove(nic_dev->nic);
    }

    list_del(&nic_dev->list);
    kfree(nic_dev);
  }
}

void efrm_register_nic_notifier(struct efrm_nic_notifier* notifier)
{
  EFRM_ASSERT(!registered_notifier);

  /* We need the rtnl lock here to avoid the list of nics changing while we're
   * doing the actual notify, and to avoid a double notification if a nic
   * appears between setting the notifier and notifying existing nics.
   */
  rtnl_lock();
  registered_notifier = notifier;

  efrm_notify_for_each_nic(true);
  rtnl_unlock();
}
EXPORT_SYMBOL(efrm_register_nic_notifier);


void efrm_unregister_nic_notifier(struct efrm_nic_notifier* notifier)
{
  EFRM_ASSERT(registered_notifier == notifier);

  rtnl_lock();
  efrm_notify_for_each_nic(false);

  registered_notifier = NULL;
  rtnl_unlock();
}
EXPORT_SYMBOL(efrm_unregister_nic_notifier);


void efrm_notify_nic_probe(const struct efhw_nic* nic,
                           const struct net_device *net_dev)
{
  /* We don't expect probe of a device that doesn't have an associated
   * net_device to start with. */
  EFRM_ASSERT(net_dev);

  if( registered_notifier )
    registered_notifier->probe(nic, net_dev);
}


void efrm_notify_nic_remove(const struct efhw_nic* nic)
{
  if( registered_notifier )
    registered_notifier->remove(nic);
}


static int efrm_netdev_event(struct notifier_block *this,
                             unsigned long event, void *ptr)
{
  struct net_device *net_dev = netdev_notifier_info_to_dev(ptr);
  struct efhw_nic *nic;
  int new_mtu;

  if (event == NETDEV_CHANGEMTU) {
    new_mtu = net_dev->mtu + ETH_HLEN; /* ? + ETH_VLAN_HLEN */

    nic = efhw_nic_find(net_dev, 0, NIC_FLAG_LLCT);
    if (nic) {
      EFRM_TRACE("%s: old=%d new=%d", __func__, nic->mtu, new_mtu);
      nic->mtu = new_mtu;
    }
    nic = efhw_nic_find(net_dev, NIC_FLAG_LLCT, 0);
    if (nic)
      nic->mtu = new_mtu;
  }
  if (event == NETDEV_CHANGENAME) {
    nic = efhw_nic_find(net_dev, 0, 0);
    if (nic)
      efrm_filter_rename(nic, net_dev);
  }

  return NOTIFY_DONE;
}

int efrm_register_netdev_notifier(void)
{
	return register_netdevice_notifier(&efrm_netdev_notifier);
}

void efrm_unregister_netdev_notifier(void)
{
	unregister_netdevice_notifier(&efrm_netdev_notifier);
}
