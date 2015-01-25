/*****************************************************************************

 @(#) src/drivers/beb.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

#ident "@(#) src/drivers/beb.c (" PACKAGE_ENVR " " PACKAGE_DATE ")"

static char const ident[] = "src/drivers/beb.c (" PACKAGE_ENVR " " PACKAGE_DATE ")";

/*
 * This is a drivers for Provider Backbone Edge Bridging (BEB).
 */

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <ss7/os8/compat.h>
#include <sys/dlpi.h>

#define BEB_DESCRIP	"Backbone Edge Bridge (BEB) for IEEE 802.1ah STREAMS Driver"
#define BEB_EXTRA	"Part of the OpenSS7 INET Stack for LInux Fast-STREAMS"
#define BEB_REVISION	"OpenSS7 src/drivers/beb.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define BEB_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define BEB_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define BEB_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define BEB_LICENSE	"GPL"
#define BEB_BANNER	BEB_DESCRIP	"\n" \
			BEB_EXTRA	"\n" \
			BEB_REVISION	"\n" \
			BEB_COPYRIGHT	"\n" \
			BEB_DEVICE	"\n" \
			BEB_CONTACT	"\n"
#define BEB_SPLASH	BEB_DESCRIP	" - " \
			BEB_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(BEB_CONTACT);
MODULE_SUPPORTED_DEVICE(BEB_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(BEB_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_BEB_MODID));
MODULE_ALIAS("streams-driver-beb");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_BEB_MAJOR));
MODULE_ALIAS("/dev/streams/beb");
MODULE_ALIAS("/dev/streams/beb/*");
MODULE_ALIAS("/dev/streams/clone/beb");
MODULE_ALIAS("streams-beb");
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR) "-" __stringify(BEB_CMAJOR_0));
//MODULE_ALIAS("devname::beb");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define BEB_DRV_ID		CONFIG_STREAMS_BEB_MODID
#define BEB_DRV_NAME		CONFIG_STREAMS_BEB_NAME
#define BEB_CMAJORS		CONFIG_STREAMS_BEB_NMAJORS
#define BEB_CMAJOR_0		CONFIG_STREAMS_BEB_MAJOR
#define BEB_UNITS		CONFIG_STREAMS_BEB_NMINORS

#ifndef BEB_DRV_NAME
#define BEB_DRV_NAME	"beb"
#endif				/* BEB_DRV_NAME */

#ifndef BEB_DRV_ID
#define BEB_DRV_ID	0
#endif				/* BEB_DRV_ID */

/*
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
 */

#define DRV_ID		BEB_DRV_ID
#define DRV_NAME	BEB_DRV_NAME
#ifdef MODULE
#define DRV_BANNER	BEB_BANNER
#else				/* MODULE */
#define DRV_BANNER	BEB_SPLASH
#endif				/* MODULE */

static struct module_info beb_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat beb_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat beb_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  =========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  =========================================================================
 */

struct beb {
	queue_t *wq;
	queue_t *rq;
};

#define PRIV(q)		((struct beb *)(q)->q_ptr)
#define BEB_PRIV(q)	PRIV(q)

static kmem_cachep_t beb_priv_cachep = NULL;

#define STRLOGNO	0	/* log streams errors */
#define STRLOGIO	1	/* log streams input-output */
#define STRLOGST	2	/* log streams state transitions */
#define STRLOGTO	3	/* log streams timeouts */
#define STRLOGRX	4	/* log streams primitives received */
#define STRLOGTX	5	/* log streams primitives issued */
#define STRLOGTE	6	/* log streams timer events */
#define STRLOGDA	7	/* log streams data */

#if	defined DEFINE_RWLOCK
static DEFINE_RWLOCK(beb_drv_lock);
#elif	defined __RW_LOCK_UNLOCKED
static rwlock_t beb_drv_lock = __RW_LOCK_UNLOCKED(beb_drv_lock);
#elif	defined RW_LOCK_UNLOCKED
static rwlock_t beb_drv_lock = RW_LOCK_UNLOCKED;
#else
#error cannot initialize read-write locks
#endif

/**
 * beb_iocname: display BEB ioctl command name
 * @cmd: ioctl command
 */
static const char *
beb_iocname(int cmd)
{
	switch (_IOC_NR(cmd)) {
	case _IOC_NR(BEB_IOCGOPTIONS):
		return ("BEB_IOCGOPTIONS");
	case _IOC_NR(BEB_IOCSOPTIONS):
		return ("BEB_IOCSOPTIONS");
	case _IOC_NR(BEB_IOCGCONFIG):
		return ("BEB_IOCGCONFIG");
	case _IOC_NR(BEB_IOCSCONFIG):
		return ("BEB_IOCSCONFIG");
	case _IOC_NR(BEB_IOCTCONFIG):
		return ("BEB_IOCTCONFIG");
	case _IOC_NR(BEB_IOCCCONFIG):
		return ("BEB_IOCCCONFIG");
	case _IOC_NR(BEB_IOCGSTATEM):
		return ("BEB_IOCGSTATEM");
	case _IOC_NR(BEB_IOCCMRESET):
		return ("BEB_IOCCMRESET");
	case _IOC_NR(BEB_IOCGSTATSP):
		return ("BEB_IOCGSTATSP");
	case _IOC_NR(BEB_IOCSSTATSP):
		return ("BEB_IOCSSTATSP");
	case _IOC_NR(BEB_IOCGSTATS):
		return ("BEB_IOCGSTATS");
	case _IOC_NR(BEB_IOCCSTATS):
		return ("BEB_IOCCSTATS");
	case _IOC_NR(BEB_IOCGNOTIFY):
		return ("BEB_IOCGNOTIFY");
	case _IOC_NR(BEB_IOCSNOTIFY):
		return ("BEB_IOCSNOTIFY");
	case _IOC_NR(BEB_IOCCNOTIFY):
		return ("BEB_IOCCNOTIFY");
	default:
		return ("(unknown ioctl)");
	}
}

/**
 * dl_primname: - display DL primitive name
 * @prim: the primitive to display
 */
static const char *
dl_primname(dl_ulong prim)
{
	switch (prim) {
	case DL_INFO_REQ:
		return ("DL_INFO_REQ");
	case DL_INFO_ACK:
		return ("DL_INFO_ACK");
	case DL_ATTACH_REQ:
		return ("DL_ATTACH_REQ");
	case DL_DETACH_REQ:
		return ("DL_DETACH_REQ");
	case DL_BIND_REQ:
		return ("DL_BIND_REQ");
	case DL_BIND_ACK:
		return ("DL_BIND_ACK");
	case DL_UNBIND_REQ:
		return ("DL_UNBIND_REQ");
	case DL_OK_ACK:
		return ("DL_OK_ACK");
	case DL_ERROR_ACK:
		return ("DL_ERROR_ACK");
	case DL_SUBS_BIND_REQ:
		return ("DL_SUBS_BIND_REQ");
	case DL_SUBS_BIND_ACK:
		return ("DL_SUBS_BIND_ACK");
	case DL_SUBS_UNBIND_REQ:
		return ("DL_SUBS_UNBIND_REQ");
	case DL_ENABMULTI_REQ:
		return ("DL_ENABMULTI_REQ");
	case DL_DISABMULTI_REQ:
		return ("DL_DISABMULTI_REQ");
	case DL_PROMISCON_REQ:
		return ("DL_PROMISCON_REQ");
	case DL_PROMISCOFF_REQ:
		return ("DL_PROMISCOFF_REQ");
	case DL_UNITDATA_REQ:
		return ("DL_UNITDATA_REQ");
	case DL_UNITDATA_IND:
		return ("DL_UNITDATA_IND");
	case DL_UDERROR_IND:
		return ("DL_UDERROR_IND");
	case DL_UDQOS_REQ:
		return ("DL_UDQOS_REQ");
	case DL_CONNECT_REQ:
		return ("DL_CONNECT_REQ");
	case DL_CONNECT_IND:
		return ("DL_CONNECT_IND");
	case DL_CONNECT_RES:
		return ("DL_CONNECT_RES");
	case DL_CONNECT_CON:
		return ("DL_CONNECT_CON");
	case DL_TOKEN_REQ:
		return ("DL_TOKEN_REQ");
	case DL_TOKEN_ACK:
		return ("DL_TOKEN_ACK");
	case DL_DISCONNECT_REQ:
		return ("DL_DISCONNECT_REQ");
	case DL_DISCONNECT_IND:
		return ("DL_DISCONNECT_IND");
	case DL_RESET_REQ:
		return ("DL_RESET_REQ");
	case DL_RESET_IND:
		return ("DL_RESET_IND");
	case DL_RESET_RES:
		return ("DL_RESET_RES");
	case DL_RESET_CON:
		return ("DL_RESET_CON");
	case DL_DATA_ACK_REQ:
		return ("DL_DATA_ACK_REQ");
	case DL_DATA_ACK_IND:
		return ("DL_DATA_ACK_IND");
	case DL_DATA_ACK_STATUS_IND:
		return ("DL_DATA_ACK_STATUS_IND");
	case DL_REPLY_REQ:
		return ("DL_REPLY_REQ");
	case DL_REPLY_IND:
		return ("DL_REPLY_IND");
	case DL_REPLY_STATUS_IND:
		return ("DL_REPLY_STATUS_IND");
	case DL_REPLY_UPDATE_REQ:
		return ("DL_REPLY_UPDATE_REQ");
	case DL_REPLY_UPDATE_STATUS_IND:
		return ("DL_REPLY_UPDATE_STATUS_IND");
	case DL_XID_REQ:
		return ("DL_XID_REQ");
	case DL_XID_IND:
		return ("DL_XID_IND");
	case DL_XID_RES:
		return ("DL_XID_RES");
	case DL_XID_CON:
		return ("DL_XID_CON");
	case DL_TEST_REQ:
		return ("DL_TEST_REQ");
	case DL_TEST_IND:
		return ("DL_TEST_IND");
	case DL_TEST_RES:
		return ("DL_TEST_RES");
	case DL_TEST_CON:
		return ("DL_TEST_CON");
	case DL_PHYS_ADDR_REQ:
		return ("DL_PHYS_ADDR_REQ");
	case DL_PHYS_ADDR_ACK:
		return ("DL_PHYS_ADDR_ACK");
	case DL_SET_PHYS_ADDR_REQ:
		return ("DL_SET_PHYS_ADDR_REQ");
	case DL_GET_STATISTICS_REQ:
		return ("DL_GET_STATISTICS_REQ");
	case DL_GET_STATISTICS_ACK:
		return ("DL_GET_STATISTICS_ACK");
	default:
		return ("(unknown primitive)");
	}
}

/*
 *  =========================================================================
 *
 *  STATE TRANSITIONS
 *
 *  =========================================================================
 */

static inline fastcall dl_ulong
dl_get_i_state(struct dl *dl)
{
	return (dl->state.i_state);
}

static inline fastcall dl_ulong
dl_set_i_state(struct dl *dl, dl_ulong newstate)
{
	dl_ulong oldstate = dl_get_i_state(dl);

	if (newstate != oldstate) {
		dl->state.i_state = newstate;
		mi_strlog(dl->wq, STRLOGST, SL_TRACE, "%s <- %s", dl_i_state_name(newstate),
			  dl_i_state_name(oldstate));
	}
	return (newstate);
}

static inline fastcall dl_ulong
dl_save_i_state(struct dl *dl)
{
	return ((dl->oldstate.i_state = dl_get_i_state(dl)));
}

static inline fastcall dl_ulong
dl_restore_i_state(struct dl *dl)
{
	return (dl_set_i_state(dl, dl->oldstate.i_state));
}

/* ====================================================== */
/* ====================================================== */
/* ====================================================== */

/*
 *  -------------------------------------------------------------------------
 *
 *  IO Controls
 *
 *  -------------------------------------------------------------------------
 */

static int
beb_do_options_br(struct beb_option *p, struct br *br, int cmd)
{
	struct beb_opt_conf_br *c = (typeof(c)) (p + 1);

	if (!br)
		return (-ESRCH);
	p->id = br->id;
	if (cmd == BEB_CHA)
		br->config = *c;
	*c = br->config;
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_do_options_dl(struct beb_option *p, struct dl *dl, int cmd)
{
	struct beb_opt_conf_dl *c = (typeof(c)) (p + 1);

	if (!dl)
		return (-ESRCH);
	p->id = dl->id;
	if (cmd == BEB_CHA)
		dl->config = *c;
	*c = dl->config;
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_do_options_df(struct beb_option *p, struct df *df, int cmd)
{
	struct beb_opt_conf_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
#ifdef FIXME
	p->id = df->id;
#endif
	if (cmd == BEB_CHA)
		df->config = *c;
	*c = df->config;
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_do_options(mblk_t *dp, int cmd)
{
	struct beb_option *p = (typeof(p)) dp->b_rptr;
	int rtn;

	switch (cmd) {
	case BEB_GET:		/* get */
	case BEB_SET:		/* set */
		switch (p->type) {
		case BEB_OBJ_TYPE_BR:	/* bridge */
			rtn = beb_do_options_br(p, br_lookup(p->id), cmd);
			break;
		case BEB_OBJ_TYPE_DL:	/* data link */
			rtn = beb_do_options_br(p, dl_lookup(p->id), cmd);
			break;
		case BEB_OBJ_TYPE_DF:	/* defaults */
			rtn = beb_do_options_br(p, df_lookup(p->id), cmd);
			break;
		default:
			return (EINVAL);
		}
		if (rtn < 0)
			return (-rtn);
		dp->b_wptr = dp->b_rptr + rtn;
		return (0);
	default:
		return (EINVAL);
	}
}

static int
beb_get_options(mblk_t *dp)
{
	return beb_do_options(dp, BEB_GET);
}

static int
beb_set_options(mblk_t *dp)
{
	return beb_do_options(db, BEB_CHA);
}

static int
beb_get_statem_br(struct beb_statem *p, struct br *br)
{
	struct beb_statem_br *c = (typeof(c)) (p + 1);

	if (!br)
		return (-ESRCH);
	p->id = br->id;
	p->flags = br->flags;
	p->state = br->state;
	/* TODO: add timers and stuff */
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_get_statem_dl(struct beb_statem *p, struct dl *dl)
{
	struct beb_statem_dl *c = (typeof(c)) (p + 1);

	if (!dl)
		return (-ESRCH);
	p->id = dl->id;
	p->flags = dl->flags;
	p->state = dl->state;
	/* TODO: add timers and stuff */
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_get_statem_df(struct beb_statem *p, struct df *df)
{
	struct beb_statem_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
	p->id = df->id;
	p->flags = df->flags;
	p->state = df->state;
	/* TODO: add timers and stuff */
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_get_statem(mblk_t *dp)
{
	struct beb_statem *p = (typeof(p)) dp->b_rptr;
	int rtn;

	switch (p->type) {
	case BEB_OBJ_TYPE_BR:
		rtn = beb_get_statem_br(p, br_lookup(p->id));
		break;
	case BEB_OBJ_TYPE_DL:
		rtn = beb_get_statem_dl(p, dl_lookup(p->id));
		break;
	case BEB_OBJ_TYPE_DF:
		rtn = beb_get_statem_df(p, df_lookup(p->id));
		break;
	default:
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}

static int
beb_get_stats_br(struct beb_stats *p, struct br *br, bool clear)
{
	struct beb_stats_br *c = (typeof(c)) (p + 1);

	if (!br)
		return (-ESRCH);
	p->id = br->id;
	*c = br->stats;
	if (clear)
		bzero(&br->stats, sizeof(br->stats));
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_get_stats_dl(struct beb_stats *p, struct dl *dl, bool clear)
{
	struct beb_stats_dl *c = (typeof(c)) (p + 1);

	if (!dl)
		return (-ESRCH);
	p->id = dl->id;
	*c = dl->stats;
	if (clear)
		bzero(&dl->stats, sizeof(dl->stats));
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_get_stats_df(struct beb_stats *p, struct df *df, bool clear)
{
	struct beb_stats_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
	p->id = df->id;
	*c = df->stats;
	if (clear)
		bzero(&df->stats, sizeof(df->stats));
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_get_stats(mblk_t *dp, const bool clear)
{
	struct beb_stats *p = (typeof(p)) dp->b_rptr;
	int rtn;

	p->header = (dl_ulong) drv_hztomsec(jiffies);
	switch (p->type) {
	case BEB_OBJ_TYPE_BR:
		rtn = beb_get_stats_br(p, br_lookup(p->id), clear);
		break;
	case BEB_OBJ_TYPE_DL:
		rtn = beb_get_stats_dl(p, dl_lookup(p->id), clear);
		break;
	case BEB_OBJ_TYPE_DF:
		rtn = beb_get_stats_df(p, df_lookup(p->id), clear);
		break;
	default:
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}

static int
beb_do_notify_br(struct beb_notify *p, struct br *br, int size, int cmd)
{
	struct beb_notify_br *c = (typeof(c)) (p + 1);

	if (!br)
		return (-ESRCH);
	p->id = br->id;
	switch (cmd) {
	case BEB_ADD:
		br->notify.events |= c->events;
		break;
	case BEB_DEL:
		br->notify.events &= ~c->events;
		break;
	}
	c->events = br->notify.events;
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_do_notify_dl(struct beb_notify *p, struct dl *dl, int size, int cmd)
{
	struct beb_notify_dl *c = (typeof(c)) (p + 1);

	if (!dl)
		return (-ESRCH);
	p->id = dl->id;
	switch (cmd) {
	case BEB_ADD:
		dl->notify.events |= c->events;
		break;
	case BEB_DEL:
		dl->notify.events &= ~c->events;
		break;
	}
	c->events = dl->notify.events;
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_do_notify_df(struct beb_notify *p, struct df *df, int size, int cmd)
{
	struct beb_notify_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
	p->id = df->id;
	switch (cmd) {
	case BEB_ADD:
		df->notify.events |= c->events;
		break;
	case BEB_DEL:
		df->notify.events &= ~c->events;
		break;
	}
	c->events = df->notify.events;
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_do_notify(mblk_t *dp, int size, int cmd)
{
	struct beb_notify *p = (typeof(p)) dp->b_rptr;
	int rtn;

	switch (cmd) {
	case BEB_GET:		/* get */
	case BEB_ADD:		/* add */
	case BEB_DEL:		/* del */
		break;
	default:
		return (EFAULT);
	}
	switch (p->type) {
	case BEB_OBJ_TYPE_BR:
		rtn = beb_do_notify_br(p, br_lookup(p->id), size, cmd);
		break;
	case BEB_OBJ_TYPE_DL:
		rtn = beb_do_notify_dl(p, dl_lookup(p->id), size, cmd);
		break;
	case BEB_OBJ_TYPE_DF:
		rtn = beb_do_notify_df(p, df_lookup(p->id), size, cmd);
		break;
	default:
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}

static int
beb_do_statsp_br(struct beb_stats *p, struct br *br, int cmd)
{
	if (!br)
		return (-ESRCH);
	p->id = br->id;
	switch (cmd) {
	case BEB_GET:		/* get */
		*c = br->statsp;
		break;
	case BEB_CHA:		/* set */
		br->statsp = *c;
		break;
	}
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_do_statsp_dl(struct beb_stats *p, struct dl *dl, int cmd)
{
	if (!dl)
		return (-ESRCH);
	p->id = dl->id;
	switch (cmd) {
	case BEB_GET:		/* get */
		*c = dl->statsp;
		break;
	case BEB_CHA:		/* set */
		dl->statsp = *c;
		break;
	}
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_do_statsp_df(struct beb_stats *p, struct df *df, int cmd)
{
	if (!df)
		return (-ESRCH);
	p->id = df->id;
	switch (cmd) {
	case BEB_GET:		/* get */
		*c = df->statsp;
		break;
	case BEB_CHA:		/* set */
		df->statsp = *c;
		break;
	}
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_do_statsp(mblk_t *dp, int cmd)
{
	struct beb_stats *p = (typeof(p)) dp->b_rptr;
	int rtn;

	switch (cmd) {
	case BEB_GET:		/* get */
	case BEB_CHA:		/* set */
		break;
	default:
		return (EFAULT);
	}
	switch (p->type) {
	case BEB_OBJ_TYPE_BR:
		rtn = beb_do_statsp_br(p, br_lookup(p->id), cmd);
		break;
	case BEB_OBJ_TYPE_DL:
		rtn = beb_do_statsp_dl(p, dl_lookup(p->id), cmd);
		break;
	case BEB_OBJ_TYPE_DF:
		rtn = beb_do_statsp_df(p, df_lookup(p->id), cmd);
		break;
	default:
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}

/*
 *  LIST Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
/** beb_lst_br: - list bridges
  * @p: configuration header
  * @br: id lookup bridge structure
  *
  * Lists the data link associated with a specified bridge, or if no bridge is specified,
  * lists all bridge identifiers.
  */
static int
beb_lst_br(struct beb_config *p, struct br *br)
{
	struct beb_config *o = p;
	struct dl *dl;
	int num = p->cmd;

	if (br) {
		o++;
		/* write list of data links */
		for (dl = br->dl.list; dl && num > 0; dl = dl->br.next, num--, o++) {
			o->type = BEB_OBJ_TYPE_DL;
			o->id = dl->id;
			o->cmd = BEB_GET;
		}
	} else {
		num++;
		/* write out list of bridges */
		for (br = master.br.list; br && num > 0; br = br->next, num--, o++) {
			o->type = BEB_OBJ_TYPE_BR;
			o->id = br->id;
			o->cmd = BEB_GET;
		}
	}
	if (num > 0) {
		/* end list with zero object type */
		o->type = 0;
		o->id = 0;
		o->cmd = 0;
	}
	return ((caddr_t) o - (caddr_t) p);
}

/** beb_lst_dl: - list data links
  * @p: configuration header
  * @dl: id lookup of data link structure
  *
  * Lists nothing associated with a specified data link, or if no data link is specified,
  * lists all data link identifiers.
  */
static int
beb_lst_dl(struct beb_config *p, struct dl *dl)
{
	struct beb_config *o = p;
	int num = p->cmd;

	if (dl) {
		o++;
	} else {
		num++;
		/* write list of data links */
		for (dl = master.dl.list; dl && num > 0; dl = dl->next, num--, o++) {
			o->type = BEB_OBJ_TYPE_DL;
			o->id = dl->id;
			o->cmd = BEB_GET;
		}
	}
	if (num > 0) {
		/* end list with zero object type */
		o->type = 0;
		o->id = 0;
		o->cmd = 0;
	}
	return ((caddr_t) o - (caddr_t) p);
}

/** beb_lst_df: - list default
  * @p: configuration header
  * @df: id lookup of default structure
  *
  * Lists the bridge identifiers associated with the default, or if no default is
  * specified, lists the default identifier.
  */
static int
beb_lst_df(struct beb_config *p, struct df *df)
{
	struct beb_config *o = p;
	struct br *br;
	int num = p->cmd;

	if (df) {
		o++;
		/* write list of bridge identifiers */
		for (br = df->br.list; br && num > 0; br = br->next, num--, o++) {
			o->type = BEB_OBJ_TYPE_BR;
			o->id = br->id;
			o->cmd = BEB_GET;
		}
	} else {
		num++;
		/* write list of default objects */
		df = &master;
		o->type = BEB_OBJ_TYPE_DF;
		o->id = 0;
		o->cmd = BEB_GET;
		num--;
		o++;
	}
	if (num > 0) {
		/* end list with zero object type */
		o->type = 0;
		o->id = 0;
		o->cmd = 0;
	}
	return ((caddr_t) o - (caddr_t) p);
}

/** beb_lst_conf: - list configuration
  * @dp: data block to fill
  *
  * Lists the information associated with the configuration header structure contained in
  * the data buffer.
  */
static inline int
beb_lst_conf(mblk_t *dp)
{
	struct beb_config *p = (typeof(p)) dp->b_rptr;
	int rtn;

	switch (p->type) {
	case BEB_OBJ_TYPE_BR:
		rtn = beb_lst_br(p, br_lookup(p->id));
		break;
	case BEB_OBJ_TYPE_DL:
		rtn = beb_lst_dl(p, dl_lookup(p->id));
		break;
	case BEB_OBJ_TYPE_DF:
		rtn = beb_lst_df(p, df_lookup(p->id));
		break;
	default:
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}

/*
 *  GET Object Configuration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static int
beb_get_br(struct beb_config *p, struct br *br)
{
	struct beb_conf_br *c = (typeof(c)) (p + 1);

	if (!br)
		return (-ESRCH);
	/* TODO: add some information to c */
	p->id = br->id;
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_get_dl(struct beb_config *p, struct dl *dl)
{
	struct beb_conf_dl *c = (typeof(c)) (p + 1);

	if (!dl)
		return (-ESRCH);
	/* TODO: add some information to c */
	p->id = dl->id;
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_get_df(struct beb_config *p, struct df *df)
{
	struct beb_conf_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
	/* TODO: add some information to c */
	p->id = df->id;
	return (sizeof(*p) + sizeof(*c));
}

static int
beb_get_conf(mblk_t *dp)
{
	struct beb_config *p = (typeof(p)) dp->b_rptr;
	int rtn;

	p->cmd = BEB_GET;
	switch (p->type) {
	case BEB_OBJ_TYPE_BR:
		rtn = beb_get_br(p, br_lookup(p->id));
		break;
	case BEB_OBJ_TYPE_DL:
		rtn = beb_get_dl(p, dl_lookup(p->id));
		break;
	case BEB_OBJ_TYPE_DF:
		rtn = beb_get_df(p, df_lookup(p->id));
		break;
	default:
		rare();
		return (EINVAL);
	}
	if (rtn < 0)
		return (rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}

/*
 *  ADD Object
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static int
beb_add_br(struct beb_config *p, struct br *br, const bool force, const bool test)
{
	struct beb_conf_br *c = (typeof(c)) (p + 1);

	if (br)
		return (-EEXIST);
	if (!force) {
		/* network entity title must not be assigned yet */
		/* TODO: check this */
	}
	if (!test) {
		if (!(br = beb_alloc_br(br_get_id(p->id), c)))
			return (-ENOMEM);
		p->id = br->id;
	}
	return (sizeof(*p));
}

static int
beb_add_dl(struct beb_config *p, struct dl *dl, const bool force, const bool test)
{
	struct br *br;
	struct beb_conf_dl *c = (typeof(c)) (p + 1);

	if (dl)
		return (-EEXIST);
	/* bridge must already exist */
	if (!(br = br_lookup(c->brid)))
		return (-ESRCH);
	/* hardware address must be unique, but don't check */
	/* TODO */
	/* data link must be linked */
	if (!(dl = dl_lookup_mux(c->muxid)))
		return (-ESRCH);
	if (!test) {
		if (!(sl = beb_alloc_dl(dl_get_id(p->id), dl, br, c)))
			return (-ENOMEM);
		p->id = dl->id;
	}
	return (sizeof(*p));
}

static int
beb_add_df(struct beb_config *p, const bool force, const bool test)
{
	switch (p->type) {
	case BEB_OBJ_TYPE_BR:
		return beb_add_br(p, br_lookup(p->id), force, test);
	case BEB_OBJ_TYPE_DL:
		return beb_add_dl(p, dl_lookup(p->id), force, test);
	case BEB_OBJ_TYPE_DF:
		return beb_add_df(p, df_lookup(p->id), force, test);
	default:
		rare();
		return (-EINVAL);
	}
}

/*
 *  CHA Object
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static int
beb_cha_br(struct beb_config *p, struct br *br, const bool force, const bool test)
{
	struct beb_conf_br *c = (typeof(c)) (p + 1);

	if (!br)
		return (-ESRCH);
	if (!force) {
		/* TODO: check if bridge is being reconfigured in use */
	}
	if (!test) {
		/* TODO: actually change settings of bridge */
	}
	return (0);
}

static int
beb_cha_dl(struct beb_config *p, struct dl *dl, const bool force, const bool test)
{
	struct dl *d;
	struct beb_conf_dl *c = (typeof(c)) (p + 1);

	if (!dl)
		return (-ESRCH);
	if (c->muxid && c->muxid != dl->index)
		return (-EINVAL);
	if (c->brid && c->brid != dl->br.br->id)
		return (-EINVAL);
	if (!force) {
		/* TODO: check if data link is being reconfigured in use */
	}
	if (!test) {
		/* TODO: actually change settings of data link */
	}
	return (0);
}

static int
beb_cha_df(struct beb_config *p, struct df *df, const bool force, const bool test)
{
	struct beb_conf_df *c = (typeof(c)) (p + 1);

	if (!df)
		return (-ESRCH);
	if (!force) {
	}
	if (!test) {
	}
	return (0);
}

static int
beb_cha_conf(struct beb_config *p, const bool force, const bool test)
{
	switch (p->type) {
	case BEB_OBJ_TYPE_BR:
		return beb_cha_br(p, br_lookup(p->id), force, test);
	case BEB_OBJ_TYPE_DL:
		return beb_cha_dl(p, dl_lookup(p->id), force, test);
	case BEB_OBJ_TYPE_DF:
		return beb_cha_df(p, df_lookup(p->id), force, test);
	default:
		rare();
		return (-EINVAL);
	}
}

/*
 *  DEL Object
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
static int
beb_del_br(struct beb_config *p, struct br *br, const bool force, const bool test)
{
	if (!br)
		return (-ESRCH);
	if (!force) {
		/* check if bound to internal data structures */
	}
	if (!test) {
		beb_free_br(br);
	}
	return (0);
}

static int
beb_del_dl(struct beb_config *p, struct dl *dl, const bool force, const bool test)
{
	if (!dl)
		return (-ESRCH);
	if (!force) {
		/* check if bound to internal data structures */
	}
	if (!test) {
		beb_free_dl(dl);
	}
	return (0);
}

static int
beb_del_df(struct beb_config *p, struct df *df, const bool force, const bool test)
{
	if (!df)
		return (-ESRCH);
	if (!force) {
		return (-EBUSY);
	}
	if (!test) {
		/* can't delete default */
	}
	return (0);
}

static int
beb_del_conf(struct beb_config *p, const bool force, const bool test)
{
	switch (p->type) {
	case BEB_OBJ_TYPE_BR:
		return beb_del_br(p, br_lookup(p->id), force, test);
	case BEB_OBJ_TYPE_DL:
		return beb_del_dl(p, dl_lookup(p->id), force, test);
	case BEB_OBJ_TYPE_DF:
		return beb_del_df(p, df_lookup(p->id), force, test);
	default:
		rare();
		return (-EINVAL);
	}
}

static int
beb_do_conf(mblk_t *dp, const bool force, const bool test)
{
	struct beb_config *p = (typeof(p)) dp->b_rptr;
	int rtn;

	switch (p->cmd) {
	case BEB_ADD:
		rtn = beb_add_conf(p, force, test);
		break;
	case BEB_CHA:
		rtn = beb_cha_conf(p, force, test);
		break;
	case BEB_DEL:
		rtn = beb_del_conf(p, force, test);
		break;
	default:
		return (EINVAL);
	}
	if (rtn < 0)
		return (-rtn);
	dp->b_wptr = dp->b_rptr + rtn;
	return (0);
}

static int
beb_set_conf(mblk_t *dp)
{
	return beb_do_conf(dp, false, false);
}

static int
beb_test_conf(mblk_t *dp)
{
	return beb_do_conf(dp, false, true);
}

static int
beb_commit_conf(mblk_t *dp)
{
	return beb_do_conf(dp, true, false);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
/**
  * beb_i_link: - perform I_LINK operation
  * @q: active queue (upper write queue)
  * @beb: MTP private structure
  * @mp: the M_IOCTL message
  *
  * The driver supports I_LINK operations; however, any DL Stream that is linked with an
  * I_LINK operation can only be managed by the control Stream linking the lower Stream
  * and cannot be shared across other upper Streams unless configured against a bridge id.
  *
  * Note that if this is not the first DL linked and there are running Users, this DL
  * will not be available to them until it is conifgured and brought to the active state.
  * If this is the first DL, there cannot be running users.
  */
static int
beb_i_link(queue_t *q, struct beb *beb, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	lmi_info_req_t *p;
	unsigned long flags;
	struct br *br;
	struct dl *dl = NULL;
	mblk_t *rp = NULL;
	int err;

	if (!(rp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		err = -ENOBUFS;
		goto error;
	}
	if (!(dl = beb_alloc_link(l->l_qtop, l->l_index, ioc->ioc_cr, 0))) {
		err = ENOMEM;
		goto error;
	}

	write_lock_irqsave(&beb_mux_lock, flags);

	if (!(br = beb->br.loc)) {
		write_unlock_irqrestore(&beb_mux_lock, flags);
		err = ENXIO;
		goto error;
	}
	if (!br_trylock(q, br)) {
		err = -EDEADLK;
		write_unlock_irqrestore(&beb_mux_lock, flags);
		goto error;
	}
	/* Note that there can only be one layer management Stream per bridge.  For temporary
	   links, that must be the same layer management Stream used to create the bridge. */
	if (br->beb.lm != beb) {
		err = EPERM;
		br_unlock(br);
		write_unlock_irqrestore(&beb_mux_lock, flags);
		goto error;
	}

	if ((dl->br.next = br->dl.list))
		dl->br.next->br.prev = &dl->br.next;
	dl->br.prev = &br->dl.list;
	dl->br.br = br;
	br->dl.list = dl;

	mi_attach(l->l_qtop, (caddr_t) dl);
	br_unlock(br);
	write_unlock_irqrestore(&beb_mux_lock, flags);

	mi_copy_done(q, mp, 0);

	DB_TYPE(rp) = M_PCPROTO;
	p = (typeof(p)) rp->b_rptr;
	p->lmi_primitive = LMI_INFO_REQ;
	rp->b_wptr = rp->b_rptr + sizeof(*p);
	putnext(dl->wq, rp);	/* immediate info request */

	return (0);
      error:
	if (rp)
		freeb(rp);
	if (dl) {
		beb_free_link(dl);
	}
	if (err >= 0) {
		mi_copy_done(q, mp, err);
		return (0);
	}
	return (err);
}

static int
beb_i_plink(queue_t *q, struct beb *beb, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct dl *dl;
	int err = EPERM;

#ifdef HAVE_KMEMB_STRUCT_CRED_UID_VAL
	if (ioc->ioc_cr->cr_uid.val == 0)
#else
	if (ioc->ioc_cr->cr_uid == 0)
#endif
	{
		err = ENOMEM;
		if ((dl = beb_alloc_link(l->l_qbot, l->l_index, ioc->ioc_cr, 0)))
			err = 0;
	}
	mi_copy_done(q, mp, err);
	return (0);
}

static int
beb_i_unlink(queue_t *q, struct beb *beb, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct dl *dl;

	dl = DL_PRIV(l->l_qtop);
	beb_free_link(dl);
	mi_copy_done(q, mp, 0);
	return (0);
}

static int
beb_i_punlink(queue_t *q, struct beb *beb, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct dl *dl;
	int err = EPERM;

#ifdef HAVE_KMEMB_STRUCT_CRED_UID_VAL
	if (ioc->ioc_cr->cr_uid.val == 0)
#else
	if (ioc->ioc_cr->cr_uid == 0)
#endif
	{
		dl = DL_PRIV(l->l_qtop);
		beb_free_link(dl);
		err = 0;
	}
	mi_copy_done(q, mp, err);
	return (0);
}

static int
beb_i_ioctl(queue_t *q, struct beb *beb, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(I_LINK):
		return beb_i_link(q, beb, mp);
	case _IOC_NR(I_PLINK):
		return beb_i_plink(q, beb, mp);
	case _IOC_NR(I_UNLINK):
		return beb_i_unlink(q, beb, mp);
	case _IOC_NR(I_PUNLINK):
		return beb_i_punlink(q, beb, mp);
	}
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

/**
 * beb_ioctl: - process M_IOCTL message
 * @q: active queue (upper write queue)
 * @beb: private structure
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the input-output control operation.  Step 1 consists of copying in the
 * necessary prefix structure that identifies the object type and id being managed.
 */
static int
beb_ioctl(queue_t *q, struct beb *beb, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(BEB_IOCGOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCGOPTION)");
		size = sizeof(struct beb_option);
		break;
	case _IOC_NR(BEB_IOCSOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCSOPTION)");
		size = sizeof(struct beb_option);
		break;
	case _IOC_NR(BEB_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCSCONFIG)");
		size = sizeof(struct beb_config);
		break;
	case _IOC_NR(BEB_IOCGCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCGCONFIG)");
		size = sizeof(struct beb_config);
		break;
	case _IOC_NR(BEB_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCTCONFIG)");
		size = sizeof(struct beb_config);
		break;
	case _IOC_NR(BEB_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCCCONFIG)");
		size = sizeof(struct beb_config);
		break;
#ifdef BEB_IOCLCONFIG
	case _IOC_NR(BEB_IOCLCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCLCONFIG)");
		size = sizeof(struct beb_config);
		break;
#endif				/* BEB_IOCLCONFIG */
	case _IOC_NR(BEB_IOCGSTATEM):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCGSTATEM)");
		size = sizeof(struct beb_statem);
		break;
	case _IOC_NR(BEB_IOCCMRESET):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCCMRESET)");
		size = sizeof(struct beb_statem);
		break;
	case _IOC_NR(BEB_IOCGSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCGSTATSP)");
		size = sizeof(struct beb_stats);
		break;
	case _IOC_NR(BEB_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCSSTATSP)");
		size = sizeof(struct beb_stats);
		break;
	case _IOC_NR(BEB_IOCGSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCGSTATS)");
		size = sizeof(struct beb_stats);
		break;
	case _IOC_NR(BEB_IOCCSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCCSTATS)");
		size = sizeof(struct beb_stats);
		break;
	case _IOC_NR(BEB_IOCGNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCGNOTIFY)");
		size = sizeof(struct beb_notify);
		break;
	case _IOC_NR(BEB_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCSNOTIFY)");
		size = sizeof(struct beb_notify);
		break;
	case _IOC_NR(BEB_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCCNOTIFY)");
		size = sizeof(struct beb_notify);
		break;
	case _IOC_NR(BEB_IOCCMGMT):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCCMGMT)");
		size = sizeof(struct beb_mgmt);
		break;
	case _IOC_NR(BEB_IOCCPASS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(BEB_IOCCPASS)");
		size = sizeof(struct beb_pass);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}

/* this structure is just to determine the maximum size of an ioctl */
union beb_ioctls {
	struct {
		struct beb_option option;
		union beb_option_obj obj;
	} opt_conf;
	struct {
		struct beb_config config;
		union beb_conf_obj obj;
	} conf;
	struct {
		struct beb_statem statem;
		union beb_statem_obj obj;
	} statem;
	struct {
		struct beb_stats stats;
		union beb_stats_obj obj;
	} stats;
	struct {
		struct beb_notify notify;
		union beb_notify_obj obj;
	} notify;
	struct beb_mgmt mgmt;
	struct beb_pass pass;
};

/** beb_copyin: - process M_IOCDATA message
  * @q: active queue (upper write queue)
  * @beb: BEB private structure
  * @mp: the M_IOCDATA message
  * @dp: data part
  *
  * This is step 2 of the input-output control operation.  Step 2 consists of copying out
  * for GET operations, and processing an additional copy in operation of object specific
  * information for SET operations.
  */
static int
beb_copying(queue_t *q, struct beb *beb, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *bp = NULL;
	int size = -1;
	int err = 0;

	switch (_IOC_NR(cp->cp_cmd)) {
		int len;

	case _IOC_NR(BEB_IOCGOPTIONS):
	case _IOC_NR(BEB_IOCGCONFIG):
	case _IOC_NR(BEB_IOCGSTATEM):
	case _IOC_NR(BEB_IOCGSTATSP):
	case _IOC_NR(BEB_IOCGSTATS):
	case _IOC_NR(BEB_IOCCSTATS):
	case _IOC_NR(BEB_IOCGNOTIFY):
		len = sizeof(union beb_ioctls);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(MBLKL(dp), len));
		break;
#ifdef BEB_IOCLCONFIG
	case _IOC_NR(BEB_IOCLCONFIG):
	{
		struct beb_config *p = (typeof(p)) dp->b_rptr;

		len = (p->cmd + 1) * sizeof(*p);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(MBLKL(dp), len));
		break;
	}
#endif				/* BEB_IOCLCONFIG */
	}
	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(BEB_IOCGOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCGOPTION)");
		if (bp == NULL)
			goto enobufs;
		err = beb_get_options(bp);
		break;
	case _IOC_NR(BEB_IOCSOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCSOPTION)");
		err = beb_size_opt_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(BEB_IOCGCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCGCONFIG)");
		if (bp == NULL)
			goto enobufs;
		err = beb_get_conf(bp);
		break;
	case _IOC_NR(BEB_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCSCONFIG)");
		err = beb_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(BEB_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCTCONFIG)");
		err = beb_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(BEB_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCCCONFIG)");
		err = beb_size_conf(dp->b_rptr, &size);
		break;
#ifdef BEB_IOCLCONFIG
	case _IOC_NR(BEB_IOCLCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCLCONFIG)");
		if (bp == NULL)
			goto enobufs;
		err = beb_lst_conf(bp);
		break;
#endif				/* BEB_IOCLCONFIG */
	case _IOC_NR(BEB_IOCGSTATEM):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCGSTATEM)");
		if (bp == NULL)
			goto enobufs;
		err = beb_get_statem(bp);
		break;
	case _IOC_NR(BEB_IOCCMRESET):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCCMRESET)");
		err = EOPNOTSUPP; /* later */ ;
		break;
	case _IOC_NR(BEB_IOCGSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCGSTATSP)");
		if (bp == NULL)
			goto enobufs;
		err = beb_do_statsp(bp, BEB_GET);
		break;
	case _IOC_NR(BEB_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCSSTATSP)");
		err = beb_size_stats(dp->b_rptr, &size);
		break;
	case _IOC_NR(BEB_IOCGSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCGSTATS)");
		if (bp == NULL)
			goto enobufs;
		err = beb_get_stats(bp, false);
		break;
	case _IOC_NR(BEB_IOCCSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCCSTATS)");
		if (bp == NULL)
			goto enobufs;
		err = beb_get_stats(bp, true);
		break;
	case _IOC_NR(BEB_IOCGNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCCSTATS)");
		if (bp == NULL)
			goto enobufs;
		err = beb_do_notify(bp, 0, BEB_GET);
		break;
	case _IOC_NR(BEB_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCSNOTIFY)");
		err = beb_size_notify(dp->b_rptr, &size);
		break;
	case _IOC_NR(BEB_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCCNOTIFY)");
		err = beb_size_notify(dp->b_rptr, &size);
		break;
	case _IOC_NR(BEB_IOCCMGMT):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCCMGMT)");
		err = beb_action(q, dp);
		break;
	case _IOC_NR(BEB_IOCCPASS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCCPASS)");
		break;
	default:
		err = EOPNOTSUPP;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin_n(q, mp, 0, size);
	}
	return (0);
      enobufs:
	err = ENOBUFS;
	goto done;
}

/** beb_copyin2: - process M_IOCDATA message
  * @q: active queue (upper write queue)
  * @beb: BEB private structure
  * @mp: the M_IOCDATA message
  * @dp: data part
  *
  * Step 3 of the input-output control operation is an optional step that is used for SET
  * operations.  After the second copyin we now have the object specific structure that
  * was passed by the user and can complete the SET operation.  All SET operations also
  * include a last copyout step that copies out the information actually set (e.g. the
  * assigned id on an add operation).
  */
static int
beb_copyin2(queue_t *q, struct beb *beb, mblk_t *mp, mblk_t *dp)
{
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR((struct copyresp *) mp->b_rptr)->cp_cmd) {
	case _IOC_NR(BEB_IOCGOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCGOPTION)");
		err = EPROTO;
		break;
	case _IOC_NR(BEB_IOCSOPTION):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCSOPTION)");
		err = beb_set_options(bp);
		break;
	case _IOC_NR(BEB_IOCGCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCGCONFIG)");
		err = EPROTO;
		break;
	case _IOC_NR(BEB_IOCSCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCSCONFIG)");
		err = beb_set_conf(bp);
		break;
	case _IOC_NR(BEB_IOCTCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCTCONFIG)");
		err = beb_test_conf(bp);
		break;
	case _IOC_NR(BEB_IOCCCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCCCONFIG)");
		err = beb_commit_conf(bp);
		break;
#ifdef BEB_IOCLCONFIG
	case _IOC_NR(BEB_IOCLCONFIG):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCLCONFIG)");
		err = EPROTO;
		break;
#endif				/* BEB_IOCLCONFIG */
	case _IOC_NR(BEB_IOCGSTATEM):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCGSTATEM)");
		err = EPROTO;
		break;
	case _IOC_NR(BEB_IOCCMRESET):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCCMRESET)");
		err = EPROTO;
		break;
	case _IOC_NR(BEB_IOCGSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCGSTATSP)");
		err = EPROTO;
		break;
	case _IOC_NR(BEB_IOCSSTATSP):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCSSTATSP)");
		err = beb_do_statsp(bp, BEB_CHA);
		break;
	case _IOC_NR(BEB_IOCGSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCGSTATS)");
		err = EPROTO;
		break;
	case _IOC_NR(BEB_IOCCSTATS):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCCSTATS)");
		err = EPROTO;
		break;
	case _IOC_NR(BEB_IOCGNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCGNOTIFY)");
		err = EPROTO;
		break;
	case _IOC_NR(BEB_IOCSNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCSNOTIFY)");
		err = beb_do_notify(bp, 0, BEB_ADD);
		break;
	case _IOC_NR(BEB_IOCCNOTIFY):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCCNOTIFY)");
		err = beb_do_notify(bp, 0, BEB_DEL);
		break;
	case _IOC_NR(BEB_IOCCMGMT):
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCCMGMT)");
		err = EPROTO;
		break;
	case _IOC_NR(BEB_IOCCPASS):
	{
		struct beb_pass *p = (typeof(p)) dp->b_rptr;

		mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(BEB_IOCCPASS)");
		break;
	}
	default:
		err = EOPNOTSUPP;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
      enobufs:
	err = ENOBUFS;
	goto done;
}

/** beb_copyout: - process M_IOCDATA message
  * @q: active queue (upper write queue)
  * @beb: BEB private structure
  * @mp: the M_IOCDATA message
  * @dp: data part
  *
  * Step 4 and the final step of the input-output control operation is a final copyout
  * step.
  */
static int
beb_copyout(queue_t *q, struct beb *beb, mblk_t *mp, mblk_t *dp)
{
	mi_copyout(q, mp);
	return (0);
}

/** beb_w_ioctl: - process M_IOCTL message
  * @q: active queue (upper write queue)
  * @mp: the M_IOCTL message
  */
static int
beb_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct beb *beb;
	int err = 0;

	if (!mp->b_cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	if (!(beb = beb_acquire(q)))
		return (-EAGAIN);
	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case __SID:
		err = beb_i_ioctl(q, beb, mp);
		break;
	case BEB_IOC_MAGIC:
		err = beb_ioctl(q, beb, mp);
		break;
	default:
		mi_copy_done(q, mp, EINVAL);
		break;
	}
	beb_release(beb);
	return (err);
}

/** beb_w_iocdata: - process M_IOCDATA message
  * @q: active queue (upper write queue)
  * @mp: the M_IOCDATA message
  */
static int
beb_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct beb *beb;
	int err = 0;
	mblk_t *dp;

	if (!(beb = beb_acquire(q)))
		return (-EAGAIN);
	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case BEB_IOC_MAGIC:
			err = beb_copyin(q, beb, mp, dp);
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	case MI_COPY_CASE(MI_COPY_IN, 2):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case BEB_IOC_MAGIC:
			err = beb_copyin2(q, beb, mp, dp);
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case BEB_IOC_MAGIC:
			err = beb_copyout(q, beb, mp, dp);
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
	default:
		mi_copy_done(q, mp, EPROTO);
		break;
	}
	beb_release(beb);
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
static noinline fastcall int
tun_w_proto(queue_t *q, mblk_t *mp)
{
	switch (*(dl_ulong *) mp->b_wptr) {
	case DL_BIND_REQ:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> DL_BIND_REQ");
		return beb_bind_req(q, mp);
	default:
		mi_strlog(q, STRLOGTX, SL_TRACE, "-> DL_????_???");
		return beb_error_ack(q, BEB_PRIV(q), mp, *(dl_ulong *) mp->b_rptr, -EOPNOTSUPP);
	}
}

static inline fastcall int
beb_w_proto(queue_t *q, mblk_t *mp)
{
	struct beb *beb;
	int err = -EAGAIN;

	if (likely((beb = beb_acquire(q)) != NULL)) {
		uint oldstate = beb_get_state(beb);

		if ((err = tun_w_proto(q, mp)))
			beb_set_state(beb, oldstate);
		beb_release(beb);
	}
	return (err < 0 ? err : 0);
}

static int
dlpi_r_proto(queue_t *q, mblk_t *mp)
{
	switch (*(dl_ulong *) mp->b_wptr) {
	case DL_DATA_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "DL_DATA_IND [%u] <-",
			  (uint) msgdsize(mp->b_cont));
		return dl_data_ind(q, mp);
	case DL_OK_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_OK_ACK <-");
		return dl_ok_ack(q, mp);
	case DL_ERROR_ACK:
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_ERROR_ACK <-");
		return dl_error_ack(q, mp);
	default:
		/* reject what we don't recognize */
		mi_strlog(q, STRLOGRX, SL_TRACE, "DL_????_??? <-");
		return EOPNOTSUPP;
	}
}

static int
dl_r_proto(queue_t *q, mblk_t *mp)
{
	struct dl *dl;
	int err = -EAGAIN;

	if ((dl = dl_acquire(q))) {
		uint oldstate = dl_get_i_state(dl);

		if ((err = dlpi_r_proto(q, mp)))
			dl_set_state(q, dl, oldstate);
		dl_release(dl);
	}
	return (err < 0 ? err : 0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
static inline fastcall int
beb_w_data(queue_t *q, mblk_t *mp)
{
	struct beb *beb;
	int err = -EAGAIN;

	if (likely((beb = beb_acquire(q)) != NULL)) {
		err = beb_data(q, beb, mp);
		beb_release(beb);
	}
	return (err < 0 ? err : 0);
}

static int
dl_r_data(queue_t *q, mblk_t *mp)
{
	struct dl *dl;
	int err = -EAGAIN;

	if (likely((dl = dl_acquire(q)) != NULL)) {
		err = dl_data(q, dl, mp);
		dl_release(dl);
	}
	return (err);

}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_SIG, M_PCSIG Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
do_timeout(queue_t *q, mblk_t *mp)
{
	struct beb_timer *t = (typeof(t)) mp->b_rptr;
	int rtn;

	switch (t->timer) {
	case t1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "t1 expiry at %lu", jiffies);
		return cb_t1_timeout(q, t->cb);
/** MORE **/
/** .... **/
/** MORE **/
	default:
		return (0);
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_RSE, M_PCRSE Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
beb_w_rse(queue_t *q, mblk_t *mp)
{
	return (EPROTO);
}

static int
dl_r_rse(queue_t *q, mblk_t *mp)
{
	return (EPROTO);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Unknown STREAMS Message Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
beb_w_unknown(queue_t *q, mblk_t *mp)
{
	return (EOPNOTSUPP);
}

static int
dl_r_unknown(queue_t *q, mblk_t *mp)
{
	return (EOPNOTSUPP);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_FLUSH Handling
 *
 *  -------------------------------------------------------------------------
 */
static int
beb_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

static int
dl_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHR;
	}
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(WR(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR Handling
 *
 *  -------------------------------------------------------------------------
 *  A hangup from below indicates that a data link has failed badly.  Move link to the
 *  out-of-service state, notify management, and perform normal data link failure actions.
 */
static int
dl_r_error(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);

	dl_set_i_state(dl, LMI_UNUSABLE);
	fixme(("Complete this function\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_HANGUP Handling
 *
 *  -------------------------------------------------------------------------
 *  A hangup from below indicates that a data link has failed badly.  Move link to the
 *  out-of-service state, notify management, and perform normal data link failure actions.
 */
static int
dl_r_hangup(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);

	dl_set_i_state(dl, LMI_UNUSABLE);
	fixme(("Complete this function\n"));
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */

static noinline fastcall
beb_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return beb_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return beb_w_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return beb_w_sig(q, mp);
	case M_FLUSH:
		return beb_w_flush(q, mp);
	case M_IOCTL:
		return beb_w_ioctl(q, mp);
	case M_IOCDATA:
		return beb_w_iocdata(q, mp);
	case M_RSE:
	case M_PCRSE:
		return beb_w_rse(q, mp);
	default:
		return beb_w_unknown(q, mp);
	}
}

static noinline fastcall int
dl_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return dl_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_r_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return dl_r_sig(q, mp);
	case M_FLUSH:
		return dl_r_flush(q, mp);
	case M_ERROR:
		return dl_r_error(q, mp);
	case M_HANGUP:
		return dl_r_hangup(q, mp);
#if 0
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return dl_r_copy(q, mp);
#endif
	case M_RSE:
	case M_PCRSE:
		return dl_r_rse(q, mp);
	default:
		return dl_r_unknown(q, mp);
	}
}

static inline fastcall int
beb_w_msg(queue_t *q, mblk_t *mp)
{
	/* TODO: write a fast path */
	return beb_w_msg_slow(q, mp);
}

static inline fastcall int
dl_r_msg(queue_t *q, mblk_t *mp)
{
	/* TODO: write a fast path */
	return dl_r_msg_slow(q, mp);
}

/*
 *  QUEUE PUT and SERVICE procedures
 *  =========================================================================
 *  Canonical STREAMS multiplexing driver processing.
 */
static streamscall __hot_put int
beb_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || beb_w_msg(q, mp))
		putq(q, mp);
	return (0);
}

static streamscall __hot_out int
beb_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (beb_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall __hot_get int
beb_rsrv(queue_t *q)
{
	/* FIXME: backenable across the mux */
	return (0);
}

static streamscall __unlikely int
beb_rput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "beb_rput() called");
	putnext(q, mp);
	return (0);
}

static streamscall __unlikely int
dl_wput(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "dl_wput() called");
	putnext(q, mp);
	return (0);
}

static streamscall __hot_out int
dl_wsrv(queue_t *q)
{
	/* FIXME: backenable across the mux */
	return (0);
}

static streamscall __hot_get int
dl_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (dl_r_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall __hot_in int
dl_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || dl_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */

#define BEB_C_MINOR	0	/* "beb" device */
#define BEB_N_MINOR	1	/* "nit" device */
#define BEB_B_MINOR	2	/* "bpf" device */
#define BEB_T_MINOR	3	/* "tap" device */
#define BEB_U_MINOR	4	/* "tun" device */
#define BEB_F_MINOR	5	/* first free minor */

#define CMINOR_FIRST	 0
#define CMINOR_LAST	10
#define CMINOR_FREE	11

/** beb_qopen: - each open of a stream
  * @q: read queue of freshly allocated queue pair
  * @devp: pointer to device number being opened
  * @oflags: flags to open(2s)
  * @sflag: STREAMS flag
  * @crp: credentials of opening process
  */
static streamscall int
beb_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	minor_t bminor = getminor(*devp);
	psw_t flags;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if (sflag != CLONEOPEN)
		return (ENXIO);
	if (CMINOR_FIRST > bminor || bminr > CMINOR_LAST)
		return (ENXIO);
	*devp = makedevice(getmajor(*devp), CMINOR_FREE);
	if (!mi_set_sth_lowat(q, 0))
		return (ENOBUFS);
	if (!mi_set_sth_hiwat(q, SHEADHIWAT >> 1))
		return (ENOBUFS);
	write_lock_irqsave(&beb_list_lock, flags);
	err = mi_open_comm_cache(&beb_opens, beb_priv_cachep, q, devp, oflags, sflag, crp);
	write_unlock_irqrestore(&beb_list_lock, flags);
	if (err)
		return (err);
	beb_init_priv(q, bminor);
	qprocson(q);
	return (0);
}

/** beb_qclose: - last close of a stream
  * @q: queue pair
  * @oflags: open flags
  * @crp: credentials of closing process
  */
static streamscall __unlikely int
beb_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct beb *beb = BEB_PRIV(q);
	caddr_t ptr = (caddr_t) beb;
	psw_t flags;
	int err;

	qprocsoff(q);
	write_lock_irqsave(&beb_list_lock, flags);
	mi_acquire_sleep_nosignal(ptr, &ptr, &beb_list_lock, &flags);
	beb_term_priv(beb);
	mi_release(ptr);
	err = mi_close_comm_cache(&beb_opens, beb_priv_cachep, q);
	write_unlock_irqrestore(&beb_list_lock, flags);
	return (err);
}

/**
 * beb_qopen: - STREAMS driver open routine
 * @q: read queue of newly created queue pair
 * @devp: pointer to device number associated with stream
 * @oflags: flags to the open(2s) call
 * @sflag: STREAMS flag
 * @crp: pointer to the credentials of the opening process
 *
 * When a Stream is opened on the driver it corresponds to a BR device associated with the
 * driver.  The BR is determined from the minor device opened.  All minor devices corresponding
 * to BR are clone or auto-clone devices.  There may be several DL lower streams for each BR.  If
 * an BR structure has not been allocated for the corresponding minor device number, we allocate
 * one.  When a DL stream is I_LINK'ed under the driver, it is associated with the BR structure.
 * A tunnel structure is allocated and associated with each upper stream.
 *
 * This driver cannot be pushed as a module.
 *
 * (cminor == 0) && (sflag == DRVOPEN)
 *	When minor device number 0 is opened with DRVOPEN (non-clone), a control Stream is opened.
 *	If a control Stream has already been opened, the open is refused.  The @sflag is changed
 *	from %DRVOPEN to %CLONEOPEN and a new minor device number above NUM_BR is assigned.  This
 *	uses the autocloning features of Linux Fast-STREAMS.  This corresponds to the
 *	/dev/stream/beb/mgr minor device node.
 *
 * (cminor == 0) && (sflag == CLONEOPEN)
 *	This is a normal clone open using the clone(4) driver.  A disassociated user Stream is
 *	opened.  A new unique minor device number above NUM_BR is assigned.  This corresponds to the
 *	/dev/streams/clone/beb clone device node.
 *
 * (1 <= cminor && cminor <= NUM_BR)
 *	This is a normal non-clone open.  Where the minor device number is between 1 and NUM_BR, an
 *	associated user Stream is opened.  If there is no SP structure to associate, one is created
 *	with default values (and associated with the default BR).  A new minor device number above
 *	NUM_BR is assigned.  This uses the autocloning features of Linux Fast-STREAMS.  This
 *	corresponds to the /dev/streams/beb/NNNN minor device node where NNNN is the minor device
 *	number.
 */
#ifndef NUM_BR
#define NUM_BR	16
#endif
static caddr_t beb_opens = NULL;
static struct beb *mgr_ctrl = NULL;

DECLARE_WAIT_QUEUE_HEAD(beb_waitq);
static streamscall int
beb_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	psw_t flags;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct beb *beb;
	int err;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		mi_strlog(q, 0, SL_ERROR, "cannot push as module");
		return (ENXIO);
	}
	if (cminor > NUM_BR) {
		mi_strlog(q, 0, SL_ERROR, "cannot open dynamic minor device number");
		return (ENXIO);
	}
	if (cmajor != BEB_CMAJOR_0 || cminor >= BEB_CMINOR_FREE) {
		mi_strlog(q, 0, SL_ERROR, "cannot open cloned minors");
		return (ENXIO);
	}
	if (!(beb = beb_alloc_priv(q, devp, crp, cminor)))
		return (ENOMEM);
	*devp = makedevice(cmajor, NUM_BR + 1);
	/* start assigning minors at NUM_BR + 1 */

	write_lock_irqsave(&beb_mux_lock, flags);
	if (cminor == 0) {
		/* When a zero minor device number was opened, the Stream is either a clone open or
		   an attempt to open the master control Stream.  The difference is whether the
		   @sflag was %DRVOPEN or %CLONEOPEN. */
		if (sflag == DRVOPEN) {
			/* When the master control Stream is opened, another master control Stream
			   must no yet exist.  If this is the only master control Stream, then it
			   is created. */
			if (mgr_ctrl != NULL) {
				write_unlock_irqrestore(&beb_mux_lock, flags);
				beb_free_priv(beb);
				return (ENXIO);
			}
		}
		if ((err = mi_open_link(&beb_opens, (caddr_t) beb, devp, oflags, CLONEOPEN, crp))) {
			write_unlock_irqrestore(&beb_mux_lock, flags);
			beb_free_priv(beb);
			return (ENXIO);
		}
		if (sflag == DRVOPEN)
			mgr_ctrl = beb;
		/* Both master control Streams and clone user Streams are disassociated with any
		   specific BEB.  Master control Streams are never associated with a specific BEB.
		   User Streams are associated with a BEB using the id in the address to the bind
		   primitive, or when a DL Stream is temporarily linked under the driver using the
		   I_LINK input-output control. */
	} else {
		DECLARE_WAITQUEUE(wait, current);
		struct br *br;

		/* When a non-zero minor device number was opened, the Stream is automatically
		   associated with the BR to which the minor device number corresponds.  It cannot
		   be disassociated except when it is closed. */
		if (!(br = br_lookup(cminor))) {
			write_unlock_irqrestore(&beb_mux_lock, flags);
			beb_free_priv(beb);
			return (ENXIO);
		}
		/* Locking: need to wait until a lock on the BR structure can be acquired, or a
		   signal is received, or the BR structure is deallocated.  If the lock can be
		   acquired, associate the User Stream with the BR structure; in all other cases,
		   return an error.  Note that it is a likely event that the lock can be acquired
		   without waiting. */
		err = 0;
		add_wait_queue(&beb_waitq, &wait);
		spin_lock(&br->sq.lock);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (signal_pending(current)) {
				err = EINTR;
				spin_unlock(&br->sq.lock);
				break;
			}
			if (br->sq.users != 0) {
				spin_unlock(&br->sq.lock);
				write_unlock_irqrestore(&beb_mux_lock, flags);
				if (!(br = br_lookup(cminor))) {
					err = ENXIO;
					break;
				}
				spin_lock(&br->sq.lock);
				continue;
			}
			err = mi_open_link(&beb_opens, (caddr_t) beb, devp, oflags, CLONEOPEN, crp);
			if (err == 0)
				beb_attach(beb, br, 0);
			spin_unlock(&br->sq.lock);
			break;
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&beb_waitq, &wait);
		if (err) {
			write_unlock_irqrestore(&beb_mux_lock, flags);
			beb_free_priv(beb);
			return (err);
		}
	}
	write_unlock_irqrestore(&beb_mux_lock, flags);

	mi_attach(q, (caddr_t) beb);
	qprocson(q);
	return (0);
}

/**
 * beb_qclose: = STREAMS driver close routine
 * @q: queue pair
 * @oflags: flags to the open(2s) call
 * @crp: pointer to the credentials of the closing process
 */
static streamscall int
beb_qclose(queue_t *q, int flag, cred_t *crp)
{
	struct beb *beb = BEB_PRIV(q);
	psw_t flags;
	struct br *br;

	qprocsoff(q);
	mi_detach(q, (caddr_t) beb);
	write_lock_irqsave(&beb_mux_lock, flags);
	if ((br = beb->br.loc)) {
		DECLARE_WAITQUEUE(wait, current);

		/* Locking: need to wait until an SP lock can be acquired, or the SP structure is
		   deallocated.  If a lock can be acquired, the closing Stream is disassociated
		   with the SP; otherwise, if the SP structure is deallocated, there is no further
		   need to disassociate. Note that it is a likely event that the lock can be
		   acquired without waiting. */
		add_wait_queue(&beb_waitq, &wait);
		spin_lock(&br->sq.lock);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (br->sq.users == 0) {
				beb_detach(beb);
				spin_unlock(&br->sq.lock);
				break;
			}
			spin_unlock(&br->sq.lock);
			write_unlock_irqrestore(&beb_mux_lock, flags);
			schedule();
			write_lock_irqsave(&beb_mux_lock, flags);
			if (!(br = beb->br.loc))
				break;
			spin_lock(&br->sq.lock);
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&beb_waitq, &wait);
	}
	mi_close_unlink(&beb_opens, (caddr_t) beb);
	write_unlock_irqrestore(&beb_mux_lock, flags);
	beb_free_priv(beb);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
 */

static struct module_stat beb_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat beb_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct module_info beb_winfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME "-wr",
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_info beb_rinfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME "-rd",
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_info dl_winfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME "-muxw",
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_info dl_rinfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME "-muxr",
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
}:

static struct qinit beb_rinit = {
	.qi_putp = beb_rput,
	.qi_srvp = beb_rsrv,
	.qi_qopen = beb_qopen,
	.qi_qclose = beb_qclose,
	.qi_minfo = &beb_rinfo,
	.qi_mstat = &beb_rstat,
};

static struct qinit beb_winit = {
	.qi_putp = beb_wput,
	.qi_srvp = beb_wsrv,
	.qi_minfo = &beb_winfo,
	.qi_mstat = &beb_wstat,
};

static struct qinit dl_rinit = {
	.qi_putp = dl_rput,
	.qi_srvp = dl_rsrv,
	.qi_minfo = &dl_rinfo,
	.qi_mstat = &dl_rstat,
};

static struct qinit dl_winit = {
	.qi_putp = dl_wput,
	.qi_srvp = dl_wsrv,
	.qi_minfo = &dl_winfo,
	.qi_mstat = &dl_stat,
};

static struct streamtab bebinfo = {
	.st_rdinit = &beb_rinit,
	.st_wrinit = &beb_winit,
	.st_muxrinit = &dl_rinit,
	.st_muxwinit = &dl_winit,
};

#ifdef LINUX
/*
 *  =========================================================================
 *
 *  LINUX INITIALIZATION
 *
 *  =========================================================================
 */

unsigned short modid = DRV_ID;

#ifdef module_param
module_param(modid, ushort, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for BEB driver.  (0 for allocation.)");

unsigned short major = BEB_CMAJOR_0;

#ifdef module_param
module_param(major, ushort, 0444);
#else				/* module_param */
MODULE_PARM(major, "h");
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major number for BEB driver.  (0 for allocation.)");

static struct cdevsw beb_cdev = {
	.d_name = DRV_NAME,
	.d_str = &bebinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

static __init int
bebinit(void)
{
	int err;

	if (!(beb_priv_cachep = kmem_create_cache("beb_priv_cachep", mi_open_size(sizeof(struct
											 beb)), 0,
						  SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_WARN, "%s: could not allocate beb_priv_cachep\n", DRV_NAME);
		return (-ENOMEM);
	}
	if ((err = register_netdevice_notifier(&dl_notifier))) {
		cmn_err(CE_WARN, "%s: could not register netdevice notifier, err = %d\n", DRV_NAME,
			-err);
		kmem_cache_destroy(beb_priv_cachep);
		return (err);
	}
	if ((err = register_strdev(&beb_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register major %d, err = %d\n", DRV_NAME,
			(int) major, -err);
		unregister_netdevice_notifier(&dl_notifier);
		kmem_cache_destroy(beb_priv_cachep);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}

static __exit void
bebterminate(void)
{
	int err = 0;

	if ((err = unregister_strdev(&beb_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister major %d, err = %d\n", DRV_NAME, (int)
			major, -err);
	}
	if ((err = unregister_netdevice_notifier(&dl_notifier))) {
		cmn_err(CE_WARN, "%s: could not unregister netdevie notifier, err = %d\n", DRV_NAME,
			-err);
	}
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
	if (kmem_cache_destroy(beb_priv_cachep)) {
		cmn_err(CE_WARN, "%s: could not destroy beb_priv_cachep\n", DRV_NAME);
	}
#else
	kmem_cache_destroy(beb_priv_cachep);
#endif
	return (void) (err);
}

module_init(bebinit);
module_exit(bebterminate);

#endif				/* LINUX */
