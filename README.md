# pbbr (Provider Backbone Bridge)

This package is an implementation of Provider Backbone Bridging (PBB) as seppecified
in IEEE 802.1ah and IEEE 802.1Q (2011).  It provides a separate 'bridge' device for
the Linux Kernel that can act as a PBB-BEB and/or PBB-TE backbone bridge.

---
**_Objectives_**
---

The objective is to create a bridge device for Linux that provides an SPBM
Level-1/Level-2 bridge for the Linux kernel.
The primary uses are as follows:

1. Using the Linux Kernel as a PBB-TE provider-bridge (Level-2 Intermediate System).<br><br>
In the provider-bridge role, the bridge acts as a PBB-TE bridge for adding
PBB participating nodes into the backbone network, or to interconnect clouds using
WAN interfaces as PBB provider ports.

2. Using QEMU/KVM for virtualization of network interface devices.<br><br>
In the virtualization role, the bridge acts as a PBB-BEB bridge that enslaves Host
network interfaces as provider ports and TAP devices for virtio as customer
ports.  This enables automatic full-mesh resilient network connections for
virtualized networks in the cloud.  My primary purpose is for enabling
resilient networking for NFVI.

