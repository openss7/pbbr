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
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
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
