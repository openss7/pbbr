/*****************************************************************************

 @(#) src/snmp/lldpv2xdot1mib.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 *****************************************************************************/

#ifndef __LOCAL_LLDPV2XDOT1MIB_H__
#define __LOCAL_LLDPV2XDOT1MIB_H__

/*
 * This file was generated by mib2c and is intended for use as a mib module
 * for the ucd-snmp snmpd agent.
 */

extern const char sa_program[];
extern int sa_fclose;			/* default close files between requests */
extern int sa_changed;			/* indication to reread MIB configuration */
extern int sa_stats_refresh;		/* indications that statistics, the mib or its tables need to be refreshed */
extern int sa_request;			/* request number for per-request actions */

/* our storage structure(s) */
struct lldpV2Xdot1MIB_data {
	struct lldpV2Xdot1MIB_data *lldpV2Xdot1MIB_old;
	uint lldpV2Xdot1MIB_rsvs;
	uint lldpV2Xdot1MIB_tsts;
	uint lldpV2Xdot1MIB_sets;
	uint lldpV2Xdot1MIB_request;
};
struct lldpV2Xdot1ConfigPortVlanTable_data {
	struct lldpV2Xdot1ConfigPortVlanTable_data *lldpV2Xdot1ConfigPortVlanTable_old;
	uint lldpV2Xdot1ConfigPortVlanTable_rsvs;
	uint lldpV2Xdot1ConfigPortVlanTable_tsts;
	uint lldpV2Xdot1ConfigPortVlanTable_sets;
	uint lldpV2Xdot1ConfigPortVlanTable_request;
	uint lldpV2Xdot1ConfigPortVlanTable_refs;
	uint lldpV2Xdot1ConfigPortVlanTable_id;
	long lldpV2PortConfigIfIndex;	/* NoAccess */
	ulong lldpV2PortConfigDestAddressIndex;	/* NoAccess */
	long lldpV2Xdot1ConfigPortVlanTxEnable;	/* ReadWrite */
};
struct lldpV2Xdot1ConfigVlanNameTable_data {
	struct lldpV2Xdot1ConfigVlanNameTable_data *lldpV2Xdot1ConfigVlanNameTable_old;
	uint lldpV2Xdot1ConfigVlanNameTable_rsvs;
	uint lldpV2Xdot1ConfigVlanNameTable_tsts;
	uint lldpV2Xdot1ConfigVlanNameTable_sets;
	uint lldpV2Xdot1ConfigVlanNameTable_request;
	uint lldpV2Xdot1ConfigVlanNameTable_refs;
	uint lldpV2Xdot1ConfigVlanNameTable_id;
	long lldpV2LocPortIfIndex;	/* NoAccess */
	long lldpV2Xdot1LocVlanId;	/* NoAccess */
	long lldpV2Xdot1ConfigVlanNameTxEnable;	/* ReadWrite */
};
struct lldpV2Xdot1ConfigProtoVlanTable_data {
	struct lldpV2Xdot1ConfigProtoVlanTable_data *lldpV2Xdot1ConfigProtoVlanTable_old;
	uint lldpV2Xdot1ConfigProtoVlanTable_rsvs;
	uint lldpV2Xdot1ConfigProtoVlanTable_tsts;
	uint lldpV2Xdot1ConfigProtoVlanTable_sets;
	uint lldpV2Xdot1ConfigProtoVlanTable_request;
	uint lldpV2Xdot1ConfigProtoVlanTable_refs;
	uint lldpV2Xdot1ConfigProtoVlanTable_id;
	long lldpV2LocPortIfIndex;	/* NoAccess */
	ulong lldpV2Xdot1LocProtoVlanId;	/* NoAccess */
	long lldpV2Xdot1ConfigProtoVlanTxEnable;	/* ReadWrite */
};
struct lldpV2Xdot1ConfigProtocolTable_data {
	struct lldpV2Xdot1ConfigProtocolTable_data *lldpV2Xdot1ConfigProtocolTable_old;
	uint lldpV2Xdot1ConfigProtocolTable_rsvs;
	uint lldpV2Xdot1ConfigProtocolTable_tsts;
	uint lldpV2Xdot1ConfigProtocolTable_sets;
	uint lldpV2Xdot1ConfigProtocolTable_request;
	uint lldpV2Xdot1ConfigProtocolTable_refs;
	uint lldpV2Xdot1ConfigProtocolTable_id;
	long lldpV2LocPortIfIndex;	/* NoAccess */
	ulong lldpV2Xdot1LocProtocolIndex;	/* NoAccess */
	long lldpV2Xdot1ConfigProtocolTxEnable;	/* ReadWrite */
};
struct lldpV2Xdot1ConfigVidUsageDigestTable_data {
	struct lldpV2Xdot1ConfigVidUsageDigestTable_data *lldpV2Xdot1ConfigVidUsageDigestTable_old;
	uint lldpV2Xdot1ConfigVidUsageDigestTable_rsvs;
	uint lldpV2Xdot1ConfigVidUsageDigestTable_tsts;
	uint lldpV2Xdot1ConfigVidUsageDigestTable_sets;
	uint lldpV2Xdot1ConfigVidUsageDigestTable_request;
	uint lldpV2Xdot1ConfigVidUsageDigestTable_refs;
	uint lldpV2Xdot1ConfigVidUsageDigestTable_id;
	long lldpV2LocPortIfIndex;	/* NoAccess */
	long lldpV2Xdot1ConfigVidUsageDigestTxEnable;	/* ReadWrite */
};
struct lldpV2Xdot1ConfigManVidTable_data {
	struct lldpV2Xdot1ConfigManVidTable_data *lldpV2Xdot1ConfigManVidTable_old;
	uint lldpV2Xdot1ConfigManVidTable_rsvs;
	uint lldpV2Xdot1ConfigManVidTable_tsts;
	uint lldpV2Xdot1ConfigManVidTable_sets;
	uint lldpV2Xdot1ConfigManVidTable_request;
	uint lldpV2Xdot1ConfigManVidTable_refs;
	uint lldpV2Xdot1ConfigManVidTable_id;
	long lldpV2LocPortIfIndex;	/* NoAccess */
	long lldpV2Xdot1ConfigManVidTxEnable;	/* ReadWrite */
};
struct lldpV2Xdot1LocTable_data {
	struct lldpV2Xdot1LocTable_data *lldpV2Xdot1LocTable_old;
	uint lldpV2Xdot1LocTable_rsvs;
	uint lldpV2Xdot1LocTable_tsts;
	uint lldpV2Xdot1LocTable_sets;
	uint lldpV2Xdot1LocTable_request;
	uint lldpV2Xdot1LocTable_refs;
	uint lldpV2Xdot1LocTable_id;
	long lldpV2LocPortIfIndex;	/* NoAccess */
	ulong lldpV2Xdot1LocPortVlanId;	/* ReadOnly */
};
struct lldpV2Xdot1LocProtoVlanTable_data {
	struct lldpV2Xdot1LocProtoVlanTable_data *lldpV2Xdot1LocProtoVlanTable_old;
	uint lldpV2Xdot1LocProtoVlanTable_rsvs;
	uint lldpV2Xdot1LocProtoVlanTable_tsts;
	uint lldpV2Xdot1LocProtoVlanTable_sets;
	uint lldpV2Xdot1LocProtoVlanTable_request;
	uint lldpV2Xdot1LocProtoVlanTable_refs;
	uint lldpV2Xdot1LocProtoVlanTable_id;
	long lldpV2LocPortIfIndex;	/* NoAccess */
	ulong lldpV2Xdot1LocProtoVlanId;	/* NoAccess */
	long lldpV2Xdot1LocProtoVlanSupported;	/* ReadOnly */
	long lldpV2Xdot1LocProtoVlanEnabled;	/* ReadOnly */
};
struct lldpV2Xdot1LocVlanNameTable_data {
	struct lldpV2Xdot1LocVlanNameTable_data *lldpV2Xdot1LocVlanNameTable_old;
	uint lldpV2Xdot1LocVlanNameTable_rsvs;
	uint lldpV2Xdot1LocVlanNameTable_tsts;
	uint lldpV2Xdot1LocVlanNameTable_sets;
	uint lldpV2Xdot1LocVlanNameTable_request;
	uint lldpV2Xdot1LocVlanNameTable_refs;
	uint lldpV2Xdot1LocVlanNameTable_id;
	long lldpV2LocPortIfIndex;	/* NoAccess */
	long lldpV2Xdot1LocVlanId;	/* NoAccess */
	uint8_t *lldpV2Xdot1LocVlanName;	/* ReadOnly */
	size_t lldpV2Xdot1LocVlanNameLen;
};
struct lldpV2Xdot1LocProtocolTable_data {
	struct lldpV2Xdot1LocProtocolTable_data *lldpV2Xdot1LocProtocolTable_old;
	uint lldpV2Xdot1LocProtocolTable_rsvs;
	uint lldpV2Xdot1LocProtocolTable_tsts;
	uint lldpV2Xdot1LocProtocolTable_sets;
	uint lldpV2Xdot1LocProtocolTable_request;
	uint lldpV2Xdot1LocProtocolTable_refs;
	uint lldpV2Xdot1LocProtocolTable_id;
	long lldpV2LocPortIfIndex;	/* NoAccess */
	ulong lldpV2Xdot1LocProtocolIndex;	/* NoAccess */
	uint8_t *lldpV2Xdot1LocProtocolId;	/* ReadOnly */
	size_t lldpV2Xdot1LocProtocolIdLen;
};
struct lldpV2Xdot1LocVidUsageDigestTable_data {
	struct lldpV2Xdot1LocVidUsageDigestTable_data *lldpV2Xdot1LocVidUsageDigestTable_old;
	uint lldpV2Xdot1LocVidUsageDigestTable_rsvs;
	uint lldpV2Xdot1LocVidUsageDigestTable_tsts;
	uint lldpV2Xdot1LocVidUsageDigestTable_sets;
	uint lldpV2Xdot1LocVidUsageDigestTable_request;
	uint lldpV2Xdot1LocVidUsageDigestTable_refs;
	uint lldpV2Xdot1LocVidUsageDigestTable_id;
	long lldpV2LocPortIfIndex;	/* NoAccess */
	ulong lldpV2Xdot1LocVidUsageDigest;	/* ReadOnly */
};
struct lldpV2Xdot1LocManVidTable_data {
	struct lldpV2Xdot1LocManVidTable_data *lldpV2Xdot1LocManVidTable_old;
	uint lldpV2Xdot1LocManVidTable_rsvs;
	uint lldpV2Xdot1LocManVidTable_tsts;
	uint lldpV2Xdot1LocManVidTable_sets;
	uint lldpV2Xdot1LocManVidTable_request;
	uint lldpV2Xdot1LocManVidTable_refs;
	uint lldpV2Xdot1LocManVidTable_id;
	long lldpV2LocPortIfIndex;	/* NoAccess */
	ulong lldpV2Xdot1LocManVid;	/* ReadOnly */
};
struct lldpV2Xdot1LocLinkAggTable_data {
	struct lldpV2Xdot1LocLinkAggTable_data *lldpV2Xdot1LocLinkAggTable_old;
	uint lldpV2Xdot1LocLinkAggTable_rsvs;
	uint lldpV2Xdot1LocLinkAggTable_tsts;
	uint lldpV2Xdot1LocLinkAggTable_sets;
	uint lldpV2Xdot1LocLinkAggTable_request;
	uint lldpV2Xdot1LocLinkAggTable_refs;
	uint lldpV2Xdot1LocLinkAggTable_id;
	long lldpV2LocPortIfIndex;	/* NoAccess */
	uint8_t *lldpV2Xdot1LocLinkAggStatus;	/* ReadOnly */
	size_t lldpV2Xdot1LocLinkAggStatusLen;
	ulong lldpV2Xdot1LocLinkAggPortId;	/* ReadOnly */
};
struct lldpV2Xdot1RemTable_data {
	struct lldpV2Xdot1RemTable_data *lldpV2Xdot1RemTable_old;
	uint lldpV2Xdot1RemTable_rsvs;
	uint lldpV2Xdot1RemTable_tsts;
	uint lldpV2Xdot1RemTable_sets;
	uint lldpV2Xdot1RemTable_request;
	uint lldpV2Xdot1RemTable_refs;
	uint lldpV2Xdot1RemTable_id;
	long lldpV2RemTimeMark;		/* NoAccess */
	long lldpV2RemLocalIfIndex;	/* NoAccess */
	ulong lldpV2RemLocalDestMACAddress;	/* NoAccess */
	ulong lldpV2RemIndex;		/* NoAccess */
	ulong lldpV2Xdot1RemPortVlanId;	/* ReadOnly */
};
struct lldpV2Xdot1RemProtoVlanTable_data {
	struct lldpV2Xdot1RemProtoVlanTable_data *lldpV2Xdot1RemProtoVlanTable_old;
	uint lldpV2Xdot1RemProtoVlanTable_rsvs;
	uint lldpV2Xdot1RemProtoVlanTable_tsts;
	uint lldpV2Xdot1RemProtoVlanTable_sets;
	uint lldpV2Xdot1RemProtoVlanTable_request;
	uint lldpV2Xdot1RemProtoVlanTable_refs;
	uint lldpV2Xdot1RemProtoVlanTable_id;
	long lldpV2RemTimeMark;		/* NoAccess */
	long lldpV2RemLocalIfIndex;	/* NoAccess */
	ulong lldpV2RemLocalDestMACAddress;	/* NoAccess */
	ulong lldpV2RemIndex;		/* NoAccess */
	ulong lldpV2Xdot1RemProtoVlanId;	/* NoAccess */
	long lldpV2Xdot1RemProtoVlanSupported;	/* ReadOnly */
	long lldpV2Xdot1RemProtoVlanEnabled;	/* ReadOnly */
};
struct lldpV2Xdot1RemVlanNameTable_data {
	struct lldpV2Xdot1RemVlanNameTable_data *lldpV2Xdot1RemVlanNameTable_old;
	uint lldpV2Xdot1RemVlanNameTable_rsvs;
	uint lldpV2Xdot1RemVlanNameTable_tsts;
	uint lldpV2Xdot1RemVlanNameTable_sets;
	uint lldpV2Xdot1RemVlanNameTable_request;
	uint lldpV2Xdot1RemVlanNameTable_refs;
	uint lldpV2Xdot1RemVlanNameTable_id;
	long lldpV2RemTimeMark;		/* NoAccess */
	long lldpV2RemLocalIfIndex;	/* NoAccess */
	ulong lldpV2RemLocalDestMACAddress;	/* NoAccess */
	ulong lldpV2RemIndex;		/* NoAccess */
	long lldpV2Xdot1RemVlanId;	/* NoAccess */
	uint8_t *lldpV2Xdot1RemVlanName;	/* ReadOnly */
	size_t lldpV2Xdot1RemVlanNameLen;
};
struct lldpV2Xdot1RemProtocolTable_data {
	struct lldpV2Xdot1RemProtocolTable_data *lldpV2Xdot1RemProtocolTable_old;
	uint lldpV2Xdot1RemProtocolTable_rsvs;
	uint lldpV2Xdot1RemProtocolTable_tsts;
	uint lldpV2Xdot1RemProtocolTable_sets;
	uint lldpV2Xdot1RemProtocolTable_request;
	uint lldpV2Xdot1RemProtocolTable_refs;
	uint lldpV2Xdot1RemProtocolTable_id;
	long lldpV2RemTimeMark;		/* NoAccess */
	long lldpV2RemLocalIfIndex;	/* NoAccess */
	ulong lldpV2RemLocalDestMACAddress;	/* NoAccess */
	ulong lldpV2RemIndex;		/* NoAccess */
	ulong lldpV2Xdot1RemProtocolIndex;	/* NoAccess */
	uint8_t *lldpV2Xdot1RemProtocolId;	/* ReadOnly */
	size_t lldpV2Xdot1RemProtocolIdLen;
};
struct lldpV2Xdot1RemVidUsageDigestTable_data {
	struct lldpV2Xdot1RemVidUsageDigestTable_data *lldpV2Xdot1RemVidUsageDigestTable_old;
	uint lldpV2Xdot1RemVidUsageDigestTable_rsvs;
	uint lldpV2Xdot1RemVidUsageDigestTable_tsts;
	uint lldpV2Xdot1RemVidUsageDigestTable_sets;
	uint lldpV2Xdot1RemVidUsageDigestTable_request;
	uint lldpV2Xdot1RemVidUsageDigestTable_refs;
	uint lldpV2Xdot1RemVidUsageDigestTable_id;
	long lldpV2RemTimeMark;		/* NoAccess */
	long lldpV2RemLocalIfIndex;	/* NoAccess */
	ulong lldpV2RemLocalDestMACAddress;	/* NoAccess */
	ulong lldpV2Xdot1RemVidUsageDigest;	/* ReadOnly */
};
struct lldpV2Xdot1RemManVidTable_data {
	struct lldpV2Xdot1RemManVidTable_data *lldpV2Xdot1RemManVidTable_old;
	uint lldpV2Xdot1RemManVidTable_rsvs;
	uint lldpV2Xdot1RemManVidTable_tsts;
	uint lldpV2Xdot1RemManVidTable_sets;
	uint lldpV2Xdot1RemManVidTable_request;
	uint lldpV2Xdot1RemManVidTable_refs;
	uint lldpV2Xdot1RemManVidTable_id;
	long lldpV2RemTimeMark;		/* NoAccess */
	long lldpV2RemLocalIfIndex;	/* NoAccess */
	ulong lldpV2RemLocalDestMACAddress;	/* NoAccess */
	ulong lldpV2Xdot1RemManVid;	/* ReadOnly */
};
struct lldpV2Xdot1RemLinkAggTable_data {
	struct lldpV2Xdot1RemLinkAggTable_data *lldpV2Xdot1RemLinkAggTable_old;
	uint lldpV2Xdot1RemLinkAggTable_rsvs;
	uint lldpV2Xdot1RemLinkAggTable_tsts;
	uint lldpV2Xdot1RemLinkAggTable_sets;
	uint lldpV2Xdot1RemLinkAggTable_request;
	uint lldpV2Xdot1RemLinkAggTable_refs;
	uint lldpV2Xdot1RemLinkAggTable_id;
	long lldpV2RemTimeMark;		/* NoAccess */
	long lldpV2RemLocalIfIndex;	/* NoAccess */
	ulong lldpV2RemLocalDestMACAddress;	/* NoAccess */
	ulong lldpV2RemIndex;		/* NoAccess */
	uint8_t *lldpV2Xdot1RemLinkAggStatus;	/* ReadOnly */
	size_t lldpV2Xdot1RemLinkAggStatusLen;
	ulong lldpV2Xdot1RemLinkAggPortId;	/* ReadOnly */
};

/* storage declarations */
extern struct lldpV2Xdot1MIB_data *lldpV2Xdot1MIBStorage;
extern struct header_complex_index *lldpV2Xdot1ConfigPortVlanTableStorage;
extern struct header_complex_index *lldpV2Xdot1ConfigVlanNameTableStorage;
extern struct header_complex_index *lldpV2Xdot1ConfigProtoVlanTableStorage;
extern struct header_complex_index *lldpV2Xdot1ConfigProtocolTableStorage;
extern struct header_complex_index *lldpV2Xdot1ConfigVidUsageDigestTableStorage;
extern struct header_complex_index *lldpV2Xdot1ConfigManVidTableStorage;
extern struct header_complex_index *lldpV2Xdot1LocTableStorage;
extern struct header_complex_index *lldpV2Xdot1LocProtoVlanTableStorage;
extern struct header_complex_index *lldpV2Xdot1LocVlanNameTableStorage;
extern struct header_complex_index *lldpV2Xdot1LocProtocolTableStorage;
extern struct header_complex_index *lldpV2Xdot1LocVidUsageDigestTableStorage;
extern struct header_complex_index *lldpV2Xdot1LocManVidTableStorage;
extern struct header_complex_index *lldpV2Xdot1LocLinkAggTableStorage;
extern struct header_complex_index *lldpV2Xdot1RemTableStorage;
extern struct header_complex_index *lldpV2Xdot1RemProtoVlanTableStorage;
extern struct header_complex_index *lldpV2Xdot1RemVlanNameTableStorage;
extern struct header_complex_index *lldpV2Xdot1RemProtocolTableStorage;
extern struct header_complex_index *lldpV2Xdot1RemVidUsageDigestTableStorage;
extern struct header_complex_index *lldpV2Xdot1RemManVidTableStorage;
extern struct header_complex_index *lldpV2Xdot1RemLinkAggTableStorage;

/* enum definitions from the covered mib sections */

#define LLDPV2XDOT1CONFIGPORTVLANTXENABLE_TRUE   1
#define LLDPV2XDOT1CONFIGPORTVLANTXENABLE_FALSE  2

#define LLDPV2XDOT1CONFIGVLANNAMETXENABLE_TRUE   1
#define LLDPV2XDOT1CONFIGVLANNAMETXENABLE_FALSE  2

#define LLDPV2XDOT1CONFIGPROTOVLANTXENABLE_TRUE  1
#define LLDPV2XDOT1CONFIGPROTOVLANTXENABLE_FALSE 2

#define LLDPV2XDOT1CONFIGPROTOCOLTXENABLE_TRUE   1
#define LLDPV2XDOT1CONFIGPROTOCOLTXENABLE_FALSE  2

#define LLDPV2XDOT1CONFIGVIDUSAGEDIGESTTXENABLE_TRUE 1
#define LLDPV2XDOT1CONFIGVIDUSAGEDIGESTTXENABLE_FALSE 2

#define LLDPV2XDOT1CONFIGMANVIDTXENABLE_TRUE     1
#define LLDPV2XDOT1CONFIGMANVIDTXENABLE_FALSE    2

#define LLDPV2XDOT1LOCPROTOVLANSUPPORTED_TRUE    1
#define LLDPV2XDOT1LOCPROTOVLANSUPPORTED_FALSE   2

#define LLDPV2XDOT1LOCPROTOVLANENABLED_TRUE      1
#define LLDPV2XDOT1LOCPROTOVLANENABLED_FALSE     2

#define LLDPV2XDOT1LOCLINKAGGSTATUS_AGGCAPABLE   0
#define LLDPV2XDOT1LOCLINKAGGSTATUS_AGGENABLED   1

#define LLDPV2XDOT1REMPROTOVLANSUPPORTED_TRUE    1
#define LLDPV2XDOT1REMPROTOVLANSUPPORTED_FALSE   2

#define LLDPV2XDOT1REMPROTOVLANENABLED_TRUE      1
#define LLDPV2XDOT1REMPROTOVLANENABLED_FALSE     2

#define LLDPV2XDOT1REMLINKAGGSTATUS_AGGCAPABLE   0
#define LLDPV2XDOT1REMLINKAGGSTATUS_AGGENABLED   1

/* notifications */

/* scalars accessible only for notify */

/* object id definitions */
extern oid lldpV2Xdot1TxRxCompliance_oid[14];
extern oid lldpV2Xdot1TxCompliance_oid[14];
extern oid lldpV2Xdot1RxCompliance_oid[14];
extern oid lldpV2Xdot1ConfigGroup_oid[14];
extern oid lldpV2Xdot1LocSysGroup_oid[14];
extern oid lldpV2Xdot1RemSysGroup_oid[14];

/* function prototypes */
/* trap function prototypes */

/* variable function prototypes */
void init_lldpV2Xdot1MIB(void);
void deinit_lldpV2Xdot1MIB(void);
int term_lldpV2Xdot1MIB(int majorID, int minorID, void *serverarg, void *clientarg);
FindVarMethod var_lldpV2Xdot1MIB;
struct lldpV2Xdot1MIB_data *lldpV2Xdot1MIB_create(void);
struct lldpV2Xdot1MIB_data *lldpV2Xdot1MIB_duplicate(struct lldpV2Xdot1MIB_data *);
int lldpV2Xdot1MIB_destroy(struct lldpV2Xdot1MIB_data **);
int lldpV2Xdot1MIB_add(struct lldpV2Xdot1MIB_data *);
void parse_lldpV2Xdot1MIB(const char *, char *);
SNMPCallback store_lldpV2Xdot1MIB;
void refresh_lldpV2Xdot1MIB(int);
FindVarMethod var_lldpV2Xdot1ConfigPortVlanTable;
struct lldpV2Xdot1ConfigPortVlanTable_data *lldpV2Xdot1ConfigPortVlanTable_create(void);
struct lldpV2Xdot1ConfigPortVlanTable_data *lldpV2Xdot1ConfigPortVlanTable_duplicate(struct lldpV2Xdot1ConfigPortVlanTable_data *);
int lldpV2Xdot1ConfigPortVlanTable_destroy(struct lldpV2Xdot1ConfigPortVlanTable_data **);
int lldpV2Xdot1ConfigPortVlanTable_add(struct lldpV2Xdot1ConfigPortVlanTable_data *);
int lldpV2Xdot1ConfigPortVlanTable_del(struct lldpV2Xdot1ConfigPortVlanTable_data *);
void parse_lldpV2Xdot1ConfigPortVlanTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1ConfigPortVlanTable;
void refresh_lldpV2Xdot1ConfigPortVlanTable(int);
FindVarMethod var_lldpV2Xdot1ConfigVlanNameTable;
struct lldpV2Xdot1ConfigVlanNameTable_data *lldpV2Xdot1ConfigVlanNameTable_create(void);
struct lldpV2Xdot1ConfigVlanNameTable_data *lldpV2Xdot1ConfigVlanNameTable_duplicate(struct lldpV2Xdot1ConfigVlanNameTable_data *);
int lldpV2Xdot1ConfigVlanNameTable_destroy(struct lldpV2Xdot1ConfigVlanNameTable_data **);
int lldpV2Xdot1ConfigVlanNameTable_add(struct lldpV2Xdot1ConfigVlanNameTable_data *);
int lldpV2Xdot1ConfigVlanNameTable_del(struct lldpV2Xdot1ConfigVlanNameTable_data *);
void parse_lldpV2Xdot1ConfigVlanNameTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1ConfigVlanNameTable;
void refresh_lldpV2Xdot1ConfigVlanNameTable(int);
FindVarMethod var_lldpV2Xdot1ConfigProtoVlanTable;
struct lldpV2Xdot1ConfigProtoVlanTable_data *lldpV2Xdot1ConfigProtoVlanTable_create(void);
struct lldpV2Xdot1ConfigProtoVlanTable_data *lldpV2Xdot1ConfigProtoVlanTable_duplicate(struct lldpV2Xdot1ConfigProtoVlanTable_data *);
int lldpV2Xdot1ConfigProtoVlanTable_destroy(struct lldpV2Xdot1ConfigProtoVlanTable_data **);
int lldpV2Xdot1ConfigProtoVlanTable_add(struct lldpV2Xdot1ConfigProtoVlanTable_data *);
int lldpV2Xdot1ConfigProtoVlanTable_del(struct lldpV2Xdot1ConfigProtoVlanTable_data *);
void parse_lldpV2Xdot1ConfigProtoVlanTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1ConfigProtoVlanTable;
void refresh_lldpV2Xdot1ConfigProtoVlanTable(int);
FindVarMethod var_lldpV2Xdot1ConfigProtocolTable;
struct lldpV2Xdot1ConfigProtocolTable_data *lldpV2Xdot1ConfigProtocolTable_create(void);
struct lldpV2Xdot1ConfigProtocolTable_data *lldpV2Xdot1ConfigProtocolTable_duplicate(struct lldpV2Xdot1ConfigProtocolTable_data *);
int lldpV2Xdot1ConfigProtocolTable_destroy(struct lldpV2Xdot1ConfigProtocolTable_data **);
int lldpV2Xdot1ConfigProtocolTable_add(struct lldpV2Xdot1ConfigProtocolTable_data *);
int lldpV2Xdot1ConfigProtocolTable_del(struct lldpV2Xdot1ConfigProtocolTable_data *);
void parse_lldpV2Xdot1ConfigProtocolTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1ConfigProtocolTable;
void refresh_lldpV2Xdot1ConfigProtocolTable(int);
FindVarMethod var_lldpV2Xdot1ConfigVidUsageDigestTable;
struct lldpV2Xdot1ConfigVidUsageDigestTable_data *lldpV2Xdot1ConfigVidUsageDigestTable_create(void);
struct lldpV2Xdot1ConfigVidUsageDigestTable_data *lldpV2Xdot1ConfigVidUsageDigestTable_duplicate(struct lldpV2Xdot1ConfigVidUsageDigestTable_data *);
int lldpV2Xdot1ConfigVidUsageDigestTable_destroy(struct lldpV2Xdot1ConfigVidUsageDigestTable_data **);
int lldpV2Xdot1ConfigVidUsageDigestTable_add(struct lldpV2Xdot1ConfigVidUsageDigestTable_data *);
int lldpV2Xdot1ConfigVidUsageDigestTable_del(struct lldpV2Xdot1ConfigVidUsageDigestTable_data *);
void parse_lldpV2Xdot1ConfigVidUsageDigestTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1ConfigVidUsageDigestTable;
void refresh_lldpV2Xdot1ConfigVidUsageDigestTable(int);
FindVarMethod var_lldpV2Xdot1ConfigManVidTable;
struct lldpV2Xdot1ConfigManVidTable_data *lldpV2Xdot1ConfigManVidTable_create(void);
struct lldpV2Xdot1ConfigManVidTable_data *lldpV2Xdot1ConfigManVidTable_duplicate(struct lldpV2Xdot1ConfigManVidTable_data *);
int lldpV2Xdot1ConfigManVidTable_destroy(struct lldpV2Xdot1ConfigManVidTable_data **);
int lldpV2Xdot1ConfigManVidTable_add(struct lldpV2Xdot1ConfigManVidTable_data *);
int lldpV2Xdot1ConfigManVidTable_del(struct lldpV2Xdot1ConfigManVidTable_data *);
void parse_lldpV2Xdot1ConfigManVidTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1ConfigManVidTable;
void refresh_lldpV2Xdot1ConfigManVidTable(int);
FindVarMethod var_lldpV2Xdot1LocTable;
struct lldpV2Xdot1LocTable_data *lldpV2Xdot1LocTable_create(void);
struct lldpV2Xdot1LocTable_data *lldpV2Xdot1LocTable_duplicate(struct lldpV2Xdot1LocTable_data *);
int lldpV2Xdot1LocTable_destroy(struct lldpV2Xdot1LocTable_data **);
int lldpV2Xdot1LocTable_add(struct lldpV2Xdot1LocTable_data *);
int lldpV2Xdot1LocTable_del(struct lldpV2Xdot1LocTable_data *);
void parse_lldpV2Xdot1LocTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1LocTable;
void refresh_lldpV2Xdot1LocTable(int);
FindVarMethod var_lldpV2Xdot1LocProtoVlanTable;
struct lldpV2Xdot1LocProtoVlanTable_data *lldpV2Xdot1LocProtoVlanTable_create(void);
struct lldpV2Xdot1LocProtoVlanTable_data *lldpV2Xdot1LocProtoVlanTable_duplicate(struct lldpV2Xdot1LocProtoVlanTable_data *);
int lldpV2Xdot1LocProtoVlanTable_destroy(struct lldpV2Xdot1LocProtoVlanTable_data **);
int lldpV2Xdot1LocProtoVlanTable_add(struct lldpV2Xdot1LocProtoVlanTable_data *);
int lldpV2Xdot1LocProtoVlanTable_del(struct lldpV2Xdot1LocProtoVlanTable_data *);
void parse_lldpV2Xdot1LocProtoVlanTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1LocProtoVlanTable;
void refresh_lldpV2Xdot1LocProtoVlanTable(int);
FindVarMethod var_lldpV2Xdot1LocVlanNameTable;
struct lldpV2Xdot1LocVlanNameTable_data *lldpV2Xdot1LocVlanNameTable_create(void);
struct lldpV2Xdot1LocVlanNameTable_data *lldpV2Xdot1LocVlanNameTable_duplicate(struct lldpV2Xdot1LocVlanNameTable_data *);
int lldpV2Xdot1LocVlanNameTable_destroy(struct lldpV2Xdot1LocVlanNameTable_data **);
int lldpV2Xdot1LocVlanNameTable_add(struct lldpV2Xdot1LocVlanNameTable_data *);
int lldpV2Xdot1LocVlanNameTable_del(struct lldpV2Xdot1LocVlanNameTable_data *);
void parse_lldpV2Xdot1LocVlanNameTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1LocVlanNameTable;
void refresh_lldpV2Xdot1LocVlanNameTable(int);
FindVarMethod var_lldpV2Xdot1LocProtocolTable;
struct lldpV2Xdot1LocProtocolTable_data *lldpV2Xdot1LocProtocolTable_create(void);
struct lldpV2Xdot1LocProtocolTable_data *lldpV2Xdot1LocProtocolTable_duplicate(struct lldpV2Xdot1LocProtocolTable_data *);
int lldpV2Xdot1LocProtocolTable_destroy(struct lldpV2Xdot1LocProtocolTable_data **);
int lldpV2Xdot1LocProtocolTable_add(struct lldpV2Xdot1LocProtocolTable_data *);
int lldpV2Xdot1LocProtocolTable_del(struct lldpV2Xdot1LocProtocolTable_data *);
void parse_lldpV2Xdot1LocProtocolTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1LocProtocolTable;
void refresh_lldpV2Xdot1LocProtocolTable(int);
FindVarMethod var_lldpV2Xdot1LocVidUsageDigestTable;
struct lldpV2Xdot1LocVidUsageDigestTable_data *lldpV2Xdot1LocVidUsageDigestTable_create(void);
struct lldpV2Xdot1LocVidUsageDigestTable_data *lldpV2Xdot1LocVidUsageDigestTable_duplicate(struct lldpV2Xdot1LocVidUsageDigestTable_data *);
int lldpV2Xdot1LocVidUsageDigestTable_destroy(struct lldpV2Xdot1LocVidUsageDigestTable_data **);
int lldpV2Xdot1LocVidUsageDigestTable_add(struct lldpV2Xdot1LocVidUsageDigestTable_data *);
int lldpV2Xdot1LocVidUsageDigestTable_del(struct lldpV2Xdot1LocVidUsageDigestTable_data *);
void parse_lldpV2Xdot1LocVidUsageDigestTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1LocVidUsageDigestTable;
void refresh_lldpV2Xdot1LocVidUsageDigestTable(int);
FindVarMethod var_lldpV2Xdot1LocManVidTable;
struct lldpV2Xdot1LocManVidTable_data *lldpV2Xdot1LocManVidTable_create(void);
struct lldpV2Xdot1LocManVidTable_data *lldpV2Xdot1LocManVidTable_duplicate(struct lldpV2Xdot1LocManVidTable_data *);
int lldpV2Xdot1LocManVidTable_destroy(struct lldpV2Xdot1LocManVidTable_data **);
int lldpV2Xdot1LocManVidTable_add(struct lldpV2Xdot1LocManVidTable_data *);
int lldpV2Xdot1LocManVidTable_del(struct lldpV2Xdot1LocManVidTable_data *);
void parse_lldpV2Xdot1LocManVidTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1LocManVidTable;
void refresh_lldpV2Xdot1LocManVidTable(int);
FindVarMethod var_lldpV2Xdot1LocLinkAggTable;
struct lldpV2Xdot1LocLinkAggTable_data *lldpV2Xdot1LocLinkAggTable_create(void);
struct lldpV2Xdot1LocLinkAggTable_data *lldpV2Xdot1LocLinkAggTable_duplicate(struct lldpV2Xdot1LocLinkAggTable_data *);
int lldpV2Xdot1LocLinkAggTable_destroy(struct lldpV2Xdot1LocLinkAggTable_data **);
int lldpV2Xdot1LocLinkAggTable_add(struct lldpV2Xdot1LocLinkAggTable_data *);
int lldpV2Xdot1LocLinkAggTable_del(struct lldpV2Xdot1LocLinkAggTable_data *);
void parse_lldpV2Xdot1LocLinkAggTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1LocLinkAggTable;
void refresh_lldpV2Xdot1LocLinkAggTable(int);
FindVarMethod var_lldpV2Xdot1RemTable;
struct lldpV2Xdot1RemTable_data *lldpV2Xdot1RemTable_create(void);
struct lldpV2Xdot1RemTable_data *lldpV2Xdot1RemTable_duplicate(struct lldpV2Xdot1RemTable_data *);
int lldpV2Xdot1RemTable_destroy(struct lldpV2Xdot1RemTable_data **);
int lldpV2Xdot1RemTable_add(struct lldpV2Xdot1RemTable_data *);
int lldpV2Xdot1RemTable_del(struct lldpV2Xdot1RemTable_data *);
void parse_lldpV2Xdot1RemTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1RemTable;
void refresh_lldpV2Xdot1RemTable(int);
FindVarMethod var_lldpV2Xdot1RemProtoVlanTable;
struct lldpV2Xdot1RemProtoVlanTable_data *lldpV2Xdot1RemProtoVlanTable_create(void);
struct lldpV2Xdot1RemProtoVlanTable_data *lldpV2Xdot1RemProtoVlanTable_duplicate(struct lldpV2Xdot1RemProtoVlanTable_data *);
int lldpV2Xdot1RemProtoVlanTable_destroy(struct lldpV2Xdot1RemProtoVlanTable_data **);
int lldpV2Xdot1RemProtoVlanTable_add(struct lldpV2Xdot1RemProtoVlanTable_data *);
int lldpV2Xdot1RemProtoVlanTable_del(struct lldpV2Xdot1RemProtoVlanTable_data *);
void parse_lldpV2Xdot1RemProtoVlanTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1RemProtoVlanTable;
void refresh_lldpV2Xdot1RemProtoVlanTable(int);
FindVarMethod var_lldpV2Xdot1RemVlanNameTable;
struct lldpV2Xdot1RemVlanNameTable_data *lldpV2Xdot1RemVlanNameTable_create(void);
struct lldpV2Xdot1RemVlanNameTable_data *lldpV2Xdot1RemVlanNameTable_duplicate(struct lldpV2Xdot1RemVlanNameTable_data *);
int lldpV2Xdot1RemVlanNameTable_destroy(struct lldpV2Xdot1RemVlanNameTable_data **);
int lldpV2Xdot1RemVlanNameTable_add(struct lldpV2Xdot1RemVlanNameTable_data *);
int lldpV2Xdot1RemVlanNameTable_del(struct lldpV2Xdot1RemVlanNameTable_data *);
void parse_lldpV2Xdot1RemVlanNameTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1RemVlanNameTable;
void refresh_lldpV2Xdot1RemVlanNameTable(int);
FindVarMethod var_lldpV2Xdot1RemProtocolTable;
struct lldpV2Xdot1RemProtocolTable_data *lldpV2Xdot1RemProtocolTable_create(void);
struct lldpV2Xdot1RemProtocolTable_data *lldpV2Xdot1RemProtocolTable_duplicate(struct lldpV2Xdot1RemProtocolTable_data *);
int lldpV2Xdot1RemProtocolTable_destroy(struct lldpV2Xdot1RemProtocolTable_data **);
int lldpV2Xdot1RemProtocolTable_add(struct lldpV2Xdot1RemProtocolTable_data *);
int lldpV2Xdot1RemProtocolTable_del(struct lldpV2Xdot1RemProtocolTable_data *);
void parse_lldpV2Xdot1RemProtocolTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1RemProtocolTable;
void refresh_lldpV2Xdot1RemProtocolTable(int);
FindVarMethod var_lldpV2Xdot1RemVidUsageDigestTable;
struct lldpV2Xdot1RemVidUsageDigestTable_data *lldpV2Xdot1RemVidUsageDigestTable_create(void);
struct lldpV2Xdot1RemVidUsageDigestTable_data *lldpV2Xdot1RemVidUsageDigestTable_duplicate(struct lldpV2Xdot1RemVidUsageDigestTable_data *);
int lldpV2Xdot1RemVidUsageDigestTable_destroy(struct lldpV2Xdot1RemVidUsageDigestTable_data **);
int lldpV2Xdot1RemVidUsageDigestTable_add(struct lldpV2Xdot1RemVidUsageDigestTable_data *);
int lldpV2Xdot1RemVidUsageDigestTable_del(struct lldpV2Xdot1RemVidUsageDigestTable_data *);
void parse_lldpV2Xdot1RemVidUsageDigestTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1RemVidUsageDigestTable;
void refresh_lldpV2Xdot1RemVidUsageDigestTable(int);
FindVarMethod var_lldpV2Xdot1RemManVidTable;
struct lldpV2Xdot1RemManVidTable_data *lldpV2Xdot1RemManVidTable_create(void);
struct lldpV2Xdot1RemManVidTable_data *lldpV2Xdot1RemManVidTable_duplicate(struct lldpV2Xdot1RemManVidTable_data *);
int lldpV2Xdot1RemManVidTable_destroy(struct lldpV2Xdot1RemManVidTable_data **);
int lldpV2Xdot1RemManVidTable_add(struct lldpV2Xdot1RemManVidTable_data *);
int lldpV2Xdot1RemManVidTable_del(struct lldpV2Xdot1RemManVidTable_data *);
void parse_lldpV2Xdot1RemManVidTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1RemManVidTable;
void refresh_lldpV2Xdot1RemManVidTable(int);
FindVarMethod var_lldpV2Xdot1RemLinkAggTable;
struct lldpV2Xdot1RemLinkAggTable_data *lldpV2Xdot1RemLinkAggTable_create(void);
struct lldpV2Xdot1RemLinkAggTable_data *lldpV2Xdot1RemLinkAggTable_duplicate(struct lldpV2Xdot1RemLinkAggTable_data *);
int lldpV2Xdot1RemLinkAggTable_destroy(struct lldpV2Xdot1RemLinkAggTable_data **);
int lldpV2Xdot1RemLinkAggTable_add(struct lldpV2Xdot1RemLinkAggTable_data *);
int lldpV2Xdot1RemLinkAggTable_del(struct lldpV2Xdot1RemLinkAggTable_data *);
void parse_lldpV2Xdot1RemLinkAggTable(const char *, char *);
SNMPCallback store_lldpV2Xdot1RemLinkAggTable;
void refresh_lldpV2Xdot1RemLinkAggTable(int);

WriteMethod write_lldpV2Xdot1ConfigPortVlanTxEnable;
WriteMethod write_lldpV2Xdot1ConfigVlanNameTxEnable;
WriteMethod write_lldpV2Xdot1ConfigProtoVlanTxEnable;
WriteMethod write_lldpV2Xdot1ConfigProtocolTxEnable;
WriteMethod write_lldpV2Xdot1ConfigVidUsageDigestTxEnable;
WriteMethod write_lldpV2Xdot1ConfigManVidTxEnable;
#endif				/* __LOCAL_LLDPV2XDOT1MIB_H__ */
