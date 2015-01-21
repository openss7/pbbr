# PBBR (Provider Backbone Bridge)

This package is an implementation of Provider Backbone Bridging (PBB) as seppecified
in IEEE 802.1ah and IEEE 802.1Q (2011).  It provides a separate 'bridge' device for
the Linux Kernel that can act as a PBB-BEB and/or PBB-TE backbone bridge.

## Objectives

The objective of PBBR is to create a bridge device for Linux that provides an SPBM
Level-1/Level-2 bridge for the Linux kernel.
The primary uses are as follows:

1. Using the Linux Kernel as a PBB-TE provider-bridge (Level-2 Intermediate System).

  In the provider-bridge role, the bridge acts as a PBB-TE bridge for adding PBB
  participating nodes into the backbone network, or to interconnect clouds using
  WAN interfaces as PBB provider ports.

2. Using QEMU/KVM for virtualization of network interface devices.

  In the virtualization role, the bridge acts as a PBB-BEB bridge that enslaves
  Host network interfaces as provider ports and TAP devices for **`virtio`** as
  customer ports.  This enables automatic full-mesh resilient network
  connections for virtualized networks in the cloud.  My primary purpose is for
  enabling resilient networking for NFVI.

PBBR is intended to replace Open vSwitch for full-mesh networking between VNF
instances in NFVI.

## Advantages

The advantages of the PBB approach are as follows:

* Resilience
* Security
* Migration
* Virtual Instance Identification
* Performance
* Management
* Service Assurance

These advantages are detailed in the sections that follow:

### Resilience

All other existing approaches in the Linux kernel utilize STP for bridging.
Various forms of STP have resilience issues and provide for the possibility of
back holes or significant network degradation dues to broadcast/multicast loops.

SPB (Shortest Path Bridging) using the IS-IS link-state protocol is far more
resilient to network component and path outages than is STP and MSTP, and
exhibits faster convergence than STP.  When **`tun/tap`** ports are utilized on a
BEB bridge in the Host system, there is no longer a need for "learning"
customer MACs on these **`virtio`** ports.  This means that network convergence is
faster, for both startup and shutdown of **`virtio`** NIC instances.

### Security

Other approaches are normal **`br`** and **`tap`**,  **`macvtap`**, **`gretap`**,
and **`vxlan`**.  (The **`macvtap`** in **`bridge`** mode is similar to the **`br`**
and **`tap`** approach, however, setup is easier.)
The problem with bridge- or vepa-mode approaches is that any virtual mac interface
can send packets to any other mac in the cloud, with the attendant security
ramifications.
**`gre`** and **`vxlan`** tunnels achieve the separation between clients
necessary for security, but are quite complex in implementation which leads to
other possible security exposure.

On the other hand, PBB approaches achieve fully transparent separation between
clients (using service identifier) without complex configuration.

### Migration

Some approaches using Open vSwitch allow for migration of VM images between
hosts.  However, they require complex coordination between vSwitch instances and
do no exhibit fast convergence.

Utilizing the IS-IS SPB protocol and short-circuiting "learning" at the customer
port, it is possible to achieve migration change-over times consistent with the
underlying convergence of the SPB protocol.  This can easily achieve migration
times of less than 100 milliseconds, or faster on high performance hosts.  It is
also possible to have VNFs able to perform C-MAC take-over that will also
converge in the same interval and without disrupting layer-3 traffic.

Utilizing a **`tun/tap`** **`virtio`** interface to a PBB backbone edge bridge
internal to the Linux kernel precludes the need for "learning" of customer MACs.
IS-IS (SPBM) ES updates can be performed as soon as the customer **`tun/tap`**
interface is brought up or when it is closed.  Migration of customer MACs to
another host is handled quickly and automatically.  Because each BEB has a full
network view of the entire customer networks, it is also possible to detect
customer MAC conflicts at the time that the **`tun/tap`** interface is
configured.

Allowing a VM guest to add or remove MACs from the **`virtio`** interface can
permit for MAC take-over to enable fault-tolerance of VNFs.

### Virtual Instance Identification

Utilizing Open vSwitch and similar approaches, it is difficult to identify VM
guest instances.  Utilizing PBB, VM guest instances can be identified by a
primary SID (service identifier) and C-MAC (customer MAC) combination.  It is
possible to identify the location of particular VM guest instance by hooking
into the IS-IS SPBM protocol or by utilizing C-LAN approaches such as LLDP.

### Port Aggregation

Port aggregation utilizing the **`macvtap`** approach relies on coordination
between the Host and external bridges.  Port aggregation with GRE and VxLAN
approaches is complex and inefficient.

Utilizing SPBM and ECF at the BEB in the Host achieves port aggregation and load
balancing that is completely transparent to non-participating backbone bridges.

### Full Mesh

Full mesh networking can be achieved using **`macvtap`** approaches; however, it
comes with attendant security issues.  Full mesh can be achieved utilizing GRE
or VxLAN tunnels without the attendant security risks of bridge-mode approaches;
however, there are attendant performance, resilience and management complexity
associated with these approaches.

PBB achieves full mesh networking with resilience, performance and security, and
provides single-point provisioning for greatly reduced complexity.

### Traffic Engineering

Traffic engineering with PBB utilizes traditional models, both at the backbone
level and at the service instance level.  Standard link traffic control can be
managed both for backbone traffic as well as for service instance traffic.

Traffic control utilizing standard Linux host approaches is difficult when using
GRE or VxLAN tunnels.

### Monitoring

Monitoring of failures with PBB utilizes traditional and standard approaches.
On the other hand, GRE tunnels, VxLAN and Open vSwitch approaches make failure
monitoring difficult.

### Data Path Performance

Bridge-mode approaches have good data path performance (as good as a Linux
kernel bridge).  GRE and VxLAN approaches have the data path performance hit of
additional encapsulation and the use of the Layer-3 or 4 stack, as well as the
performance hit of an internal socket interface to the Layer-3 or 4 stack.

PBB-BEB is a bridge and can achieve the same data path performance as any other
Linux kernel bridge.  STREAMS implementation can achieve performance gains above
and beyond that of the native Linux kernel bridges due to instruction cache
bursting.

## Implementation

The Linux kernel native implementation consists of the following elements:

1. An implementation of a customer port.

  A customer port (C-VLAN) is similar to a VLAN virtual port (in fact is more
  similar to a VLAN in VLAN virtual port), except that, instead of being
  instantiated based on VID, it is based on SID.  The SID is a 24-bit service
  instance identifier, that identifies the customer network associated with the
  customer port.  The lower interface acts as a provider port; the upper
  interface, a customer port.  The basic kernel VLAN implementation is extended
  to add a new virtual interface type for the customer port.  Just as with VLAN
  virtual interfaces, the MAC address is shared with the provider port.

2. An implementation of the SPBM (IS-IS).

  For a customer port, the SPBM protocol is associated with the lower provider
  port under the customer port.  The MAC address of the provider port acts as a
  NET (Network Entity Title) for a Level-1/2 intermediate system in the IS-IS
  SPBM protocol.  This network entity acts as a BEB (backbone edge bridge).  All
  BEBs participate in the IS-IS SPBM protocol.

  The upper customer port acts as a normal Ethernet link and participates in the
  STP protocol when combined with a normal customer bridge (a normal **`br`** bridge
  in the Linux kernel).

3. An implementation of a **`pbbclan`** driver.

  The **`pbbclan`** driver is similar to the **`macvlan`** driver in that it
  combines a virtual interface with a bridge over a customer port lower device.
  In fact, it may be possible to simply modify the **`macvlan`** driver for
  direct use over C-VLAN ports.

4. An implementation of a **`pbbctap`** driver.

  The **`pcbctap`** driver is similar to the **`macvtap`** driver in that it
  combines a **`tun/tap`** interface with a bridge over a customer port lower
  device.  In fact, it may be possible to simply modify the **`macvtap`** driver
  for direct use of C-VLAN ports.

