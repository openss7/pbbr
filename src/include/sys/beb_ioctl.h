/*****************************************************************************

 @(#) src/include/sys/beb_ioctl.h

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

#ifndef __BEB_IOCTL_H__
#define __BEB_IOCTL_H__

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

#define BEB_IOC_MAGIC 'B'

#define BEB_OBJ_TYPE_DF		 0	/* default */
#define BEB_OBJ_TYPE_BR		 1	/* bridge (BEB user) */
#define BEB_OBJ_TYPE_DL		 2	/* data link */
#define BEB_OBJ_TYPE_TS		 3	/* tap/tun stream */

#ifdef __KERNEL__
typedef mblk_t *beb_timer_t;
#else				/* __KERNEL__ */
typedef unsigned long beb_timer_t;
#endif				/* __KERNEL__ */

/*
 * Data link options
 */
typedef struct beb_opt_conf_dl {
	unsigned int ifIndex;
	uint64_t delayExceededDiscards;
	uint64_t mtuExceededDiscards;
	uint32_t capabilities;
	uint32_t typeCapabilities;
	uint32_t type;
	bool external;
	uint32_t adminPointToPoint;
	bool operPointToPoint;
	char name[32];
	int nameLen;
} beb_opt_conf_dl_t;

/*
 * tap/tun stream options
 */
typedef struct beb_opt_conf_ts {
} beb_opt_conf_ts_t;

enum ieee8021BridgeBaseComponentType {
	iComponent = 1,
	bComponent = 2,
	cVlanComponent = 3,
	sVlanComponent = 4,
	dBridgeComponent = 5,
	erComponrnt = 6,
	tComponent = 7,
};

enum ieee8021BridgeBaseDeviceCapabilities {
	dot1ExtendedFilteringServices = 0,
	dot1DTrafficClasses = 1,
	dot1QStaticEntryIndividualPOrt = 2,
	dot1QIVLCapable = 3,
	dot1QSVLCapable = 4,
	dot1QHybridCapable = 5,
	dot1QConfigurablePVIDTagging = 6,
	dot1DLocalVLANCapable = 7,
};

/*
 * Bridge options
 */
typedef struct beb_opt_conf_br {
	uint8_t address[6];
	int addressLen;
	int numPorts;
	enum ieee8021BridgeBaseComponentType componentType;
	unsigned int deviceCapablities;
	bool trafficClassesEnabled;
	bool mmrpEnabledStatus;
	unsigned int rowStatus;
} beb_opt_conf_br_t;

/*
 * Default options
 */
typedef struct beb_opt_conf_df {
} beb_opt_conf_df_t;

typedef union beb_option_obj {
	struct beb_opt_conf_dl dl;	/* data link */
	struct beb_opt_conf_ts ts;	/* tap/tun stream */
	struct beb_opt_conf_br br;	/* bridge */
	struct beb_opt_conf_df df;	/* default */
} beb_option_obj_t;

/*
 *  OPTIONS
 */
typedef struct beb_option {
	dl_ulong type;			/* object type */
	dl_ulong id;			/* object id */
	/* followed by object-specific protocol options structure */
	beb_option_obj_t options[0];
} beb_option_t;

#define BEB_IOCGOPTION	_IOWR(	BEB_IOC_MAGIC,	 0,	beb_option_t	)
#define BEB_IOCSOPTION	_IOWR(	BEB_IOC_MAGIC,	 1,	beb_option_t	)

/*
 * Data link configuration
 */
typedef struct beb_conf_dl {
	int muxid;			/* lower multiplexor id */
	dl_ulong brid;			/* bridge id */
} beb_conf_dl_t;

/*
 * Bridge configuration
 */
typedef struct beb_conf_br {
} beb_conf_br_t;

/*
 * TAP/TUN stream configuration
 */
typedef struct beb_conf_ts {
} beb_conf_ts_t;

/*
 * Default configuration
 */
typedef struct beb_conf_df {
} beb_conf_df_t;

typedef union beb_conf_obj {
	struct beb_conf_dl dl;		/* data link */
	struct beb_conf_br br;		/* bridge */
	struct beb_conf_ts ts;		/* tap/tun stream */
	struct beb_conf_df df;		/* default */
} beb_conf_obj_t;

/*
 *  CONFIGURATION
 */
typedef struct beb_config {
	dl_ulong type;			/* object type */
	dl_ulong id;			/* object id */
	dl_ulong cmd;			/* configuration command */
	/* followed by object-specific configuration structure */
	beb_conf_obj_t config[0];
} beb_config_t;

#define BEB_GET		0	/* get configuration */
#define BEB_ADD		1	/* add configuration */
#define BEB_CHA		2	/* cha configuration */
#define BEB_DEL		3	/* del configuraiton */

#define BEB_IOCGCONFIG	_IOWR(	BEB_IOC_MAGIC,	 2,	beb_config_t	)
#define BEB_IOCSCONFIG	_IOWR(	BEB_IOC_MAGIC,	 3,	beb_config_t	)
#define BEB_IOCTCONFIG	_IOWR(	BEB_IOC_MAGIC,	 4,	beb_config_t	)
#define BEB_IOCCCONFIG	_IOWR(	BEB_IOC_MAGIC,	 5,	beb_config_t	)

/*
 *  Data link state
 */
typedef struct beb_timers_dl {
} beb_timers_dl_t;
typedef struct beb_statem_dl {
	struct beb_timers_dl timers;
} beb_statem_dl_t;

/*
 *  Bridge state
 */
typedef struct beb_timers_br {
} beb_timers_br_t;
typedef struct beb_statem_br {
	struct beb_timers_br timers;
} beb_statem_br_t;

/*
 *  Tap/tun state
 */
typedef struct beb_timers_ts {
} beb_timers_ts_t;
typedef struct beb_statem_ts {
	struct beb_timers_ts timers;
} beb_statem_ts_t;

/*
 *  Default state
 */
typedef struct beb_timers_df {
} beb_timers_df_t;
typedef struct beb_statem_df {
	struct beb_timers_df timers;
} beb_statem_df_t;

typedef union beb_statem_obj {
	struct beb_statem_dl dl;	/* data link */
	struct beb_statem_br br;	/* bridge */
	struct beb_statem_ts ts;	/* tap/tun stream */
	struct beb_statem_df df;	/* default */
} beb_statem_obj_t;

/*
 *  STATE
 */
typedef struct beb_statem {
	dl_ulong type;			/* object type */
	dl_ulong id;			/* object id */
	dl_ulong flags;			/* object flags */
	dl_ulong state;			/* object state */
	/* followed by object-specific state structure */
	beb_statem_obj_t statem[0];
} beb_statem_t;

#define BEB_IOCGSTATEM	_IOWR(	BEB_IOC_MAGIC,	 6,	beb_statem_t	)
#define BEB_IOCCMRESET	_IOWR(	BEB_IOC_MAGIC,	 7,	beb_statem_t	)

/*
 * Data link statistics
 */
typedef struct beb_stats_dl {
} beb_stats_dl_t;

/*
 * Bridge statistics
 */
typedef struct beb_stats_br {
} beb_stats_br_t;

/*
 * Tun/tap stream statistics
 */
typedef struct beb_stats_ts {
} beb_stats_ts_t;

/*
 * Default statistics
 */
typedef struct beb_stats_df {
} beb_stats_df_t;

typedef union beb_stats_obj {
	struct beb_stats_dl dl;		/* data link */
	struct beb_stats_br br;		/* bridge */
	struct beb_stats_ts ts;		/* tap/tun stream */
	struct beb_stats_df df;		/* default */
} beb_stats_obj_t;

/*
 *  STATISTICS
 */
typedef struct beb_stats {
	dl_ulong type;			/* object type */
	dl_ulong id;			/* object id */
	dl_ulong header;		/* object stats header */
	/* followed by object-specific statistics structure */
	beb_stats_obj_t stats[0];
} beb_stats_t;

#define BEB_IOCGSTATSP	_IOWR(	BEB_IOC_MAGIC,	 8,	beb_stats_t	)
#define BEB_IOCSSTATSP	_IOWR(	BEB_IOC_MAGIC,	 9,	beb_stats_t	)
#define BEB_IOCGSTATS	_IOWR(	BEB_IOC_MAGIC,	10,	beb_stats_t	)
#define BEB_IOCCSTATS	_IOWR(	BEB_IOC_MAGIC,	11,	beb_stats_t	)

/*
 *  Data link notificiations
 */
typedef struct beb_notify_dl {
	dl_ulong events;
} beb_notify_dl_t;

/*
 *  Bridge notifications
 */
typedef struct beb_notify_br {
	dl_ulong events;
} beb_notify_br_t;

/*
 *  Tap/tun notifications
 */
typedef struct beb_notify_ts {
	dl_ulong events;
} beb_notify_ts_t;

/*
 *  Default notifications
 */
typedef struct beb_notify_df {
	dl_ulong events;
} beb_notify_df_t;

typedef union beb_notify_obj {
	struct beb_notify_dl dl;	/* data link */
	struct beb_notify_br br;	/* bridge */
	struct beb_notify_ts ts;	/* tap/tun stream */
	struct beb_notify_df df;	/* default */
} beb_notify_obj_t;

/*
 *  EVENTS
 */
typedef struct beb_notify {
	dl_ulong type;			/* object type */
	dl_ulong id;			/* object id */
	/* followed by object-specific notification structure */
	beb_notify_obj_t events[0];
} beb_notify_t;

#define BEB_IOCGNOTIFY	_IOWR(	BEB_IOC_MAGIC,	12,	beb_notify_t	)
#define BEB_IOCSNOTIFY	_IOWR(	BEB_IOC_MAGIC,	13,	beb_notify_t	)
#define BEB_IOCCNOTIFY	_IOWR(	BEB_IOC_MAGIC,	14,	beb_notify_t	)

/*
 *  MANAGEMENT
 */
typedef struct beb_mgmt {
	dl_ulong type;			/* object type */
	dl_ulong id;			/* object id */
	dl_ulong cmd;			/* mgmt command */
} beb_mgmt_t;

#define BEB_IOCCMGMT	_IOW(	BEB_IOC_MAGIC,	15,	beb_mgmt_t	)

/*
 *  PASS LOWER
 */
typedef struct beb_pass {
	dl_ulong muxid;			/* mux index of lower DL structure to pass message to */
	dl_ulong type;			/* type of message block */
	dl_ulong band;			/* band of message block */
	dl_ulong ctl_length;		/* length of cntl part */
	dl_ulong dat_length;		/* length of data part */
	/* followed by cntl and data part of message to pass to data link */
} beb_pass_t;

#define BEB_IOCCPASS	_IOW(	BEB_IOC_MAGIC,	16,	beb_pass_t	)

#define BEB_IOC_FIRST	     0
#define BEB_IOC_LAST	    15
#define BEB_IOC_PRIVATE	    32

#endif				/* __BEB_IOCTL_H__ */
