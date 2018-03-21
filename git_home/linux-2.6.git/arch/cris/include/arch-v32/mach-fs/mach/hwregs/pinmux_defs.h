#ifndef __pinmux_defs_h
#define __pinmux_defs_h

/*
 * This file is autogenerated from
 *   file:           ../../inst/pinmux/rtl/guinness/pinmux_regs.r
 *     id:           pinmux_regs.r,v 1.40 2005/02/09 16:22:59 perz Exp 
 *     last modfied: Mon Apr 11 16:09:11 2005
 * 
 *   by /n/asic/design/tools/rdesc/src/rdes2c --outfile pinmux_defs.h ../../inst/pinmux/rtl/guinness/pinmux_regs.r
 *      id: $Id: //depot/sw/releases/Aquila_9.2.0_U10/linux/kernels/mips-linux-2.6.31/arch/cris/include/arch-v32/mach-fs/mach/hwregs/pinmux_defs.h#1 $
 * Any changes here will be lost.
 *
 * -*- buffer-read-only: t -*-
 */
/* Main access macros */
#ifndef REG_RD
#define REG_RD( scope, inst, reg ) \
  REG_READ( reg_##scope##_##reg, \
            (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_WR
#define REG_WR( scope, inst, reg, val ) \
  REG_WRITE( reg_##scope##_##reg, \
             (inst) + REG_WR_ADDR_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_VECT
#define REG_RD_VECT( scope, inst, reg, index ) \
  REG_READ( reg_##scope##_##reg, \
            (inst) + REG_RD_ADDR_##scope##_##reg + \
	    (index) * STRIDE_##scope##_##reg )
#endif

#ifndef REG_WR_VECT
#define REG_WR_VECT( scope, inst, reg, index, val ) \
  REG_WRITE( reg_##scope##_##reg, \
             (inst) + REG_WR_ADDR_##scope##_##reg + \
	     (index) * STRIDE_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_INT
#define REG_RD_INT( scope, inst, reg ) \
  REG_READ( int, (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_WR_INT
#define REG_WR_INT( scope, inst, reg, val ) \
  REG_WRITE( int, (inst) + REG_WR_ADDR_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_INT_VECT
#define REG_RD_INT_VECT( scope, inst, reg, index ) \
  REG_READ( int, (inst) + REG_RD_ADDR_##scope##_##reg + \
	    (index) * STRIDE_##scope##_##reg )
#endif

#ifndef REG_WR_INT_VECT
#define REG_WR_INT_VECT( scope, inst, reg, index, val ) \
  REG_WRITE( int, (inst) + REG_WR_ADDR_##scope##_##reg + \
	     (index) * STRIDE_##scope##_##reg, (val) )
#endif

#ifndef REG_TYPE_CONV
#define REG_TYPE_CONV( type, orgtype, val ) \
  ( { union { orgtype o; type n; } r; r.o = val; r.n; } )
#endif

#ifndef reg_page_size
#define reg_page_size 8192
#endif

#ifndef REG_ADDR
#define REG_ADDR( scope, inst, reg ) \
  ( (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_ADDR_VECT
#define REG_ADDR_VECT( scope, inst, reg, index ) \
  ( (inst) + REG_RD_ADDR_##scope##_##reg + \
    (index) * STRIDE_##scope##_##reg )
#endif

/* C-code for register scope pinmux */

/* Register rw_pa, scope pinmux, type rw */
typedef struct {
  unsigned int pa0    : 1;
  unsigned int pa1    : 1;
  unsigned int pa2    : 1;
  unsigned int pa3    : 1;
  unsigned int pa4    : 1;
  unsigned int pa5    : 1;
  unsigned int pa6    : 1;
  unsigned int pa7    : 1;
  unsigned int csp2_n : 1;
  unsigned int csp3_n : 1;
  unsigned int csp5_n : 1;
  unsigned int csp6_n : 1;
  unsigned int hsh4   : 1;
  unsigned int hsh5   : 1;
  unsigned int hsh6   : 1;
  unsigned int hsh7   : 1;
  unsigned int dummy1 : 16;
} reg_pinmux_rw_pa;
#define REG_RD_ADDR_pinmux_rw_pa 0
#define REG_WR_ADDR_pinmux_rw_pa 0

/* Register rw_hwprot, scope pinmux, type rw */
typedef struct {
  unsigned int ser1     : 1;
  unsigned int ser2     : 1;
  unsigned int ser3     : 1;
  unsigned int sser0    : 1;
  unsigned int sser1    : 1;
  unsigned int ata0     : 1;
  unsigned int ata1     : 1;
  unsigned int ata2     : 1;
  unsigned int ata3     : 1;
  unsigned int ata      : 1;
  unsigned int eth1     : 1;
  unsigned int eth1_mgm : 1;
  unsigned int timer    : 1;
  unsigned int p21      : 1;
  unsigned int dummy1   : 18;
} reg_pinmux_rw_hwprot;
#define REG_RD_ADDR_pinmux_rw_hwprot 4
#define REG_WR_ADDR_pinmux_rw_hwprot 4

/* Register rw_pb_gio, scope pinmux, type rw */
typedef struct {
  unsigned int pb0  : 1;
  unsigned int pb1  : 1;
  unsigned int pb2  : 1;
  unsigned int pb3  : 1;
  unsigned int pb4  : 1;
  unsigned int pb5  : 1;
  unsigned int pb6  : 1;
  unsigned int pb7  : 1;
  unsigned int pb8  : 1;
  unsigned int pb9  : 1;
  unsigned int pb10 : 1;
  unsigned int pb11 : 1;
  unsigned int pb12 : 1;
  unsigned int pb13 : 1;
  unsigned int pb14 : 1;
  unsigned int pb15 : 1;
  unsigned int pb16 : 1;
  unsigned int pb17 : 1;
  unsigned int dummy1 : 14;
} reg_pinmux_rw_pb_gio;
#define REG_RD_ADDR_pinmux_rw_pb_gio 8
#define REG_WR_ADDR_pinmux_rw_pb_gio 8

/* Register rw_pb_iop, scope pinmux, type rw */
typedef struct {
  unsigned int pb0  : 1;
  unsigned int pb1  : 1;
  unsigned int pb2  : 1;
  unsigned int pb3  : 1;
  unsigned int pb4  : 1;
  unsigned int pb5  : 1;
  unsigned int pb6  : 1;
  unsigned int pb7  : 1;
  unsigned int pb8  : 1;
  unsigned int pb9  : 1;
  unsigned int pb10 : 1;
  unsigned int pb11 : 1;
  unsigned int pb12 : 1;
  unsigned int pb13 : 1;
  unsigned int pb14 : 1;
  unsigned int pb15 : 1;
  unsigned int pb16 : 1;
  unsigned int pb17 : 1;
  unsigned int dummy1 : 14;
} reg_pinmux_rw_pb_iop;
#define REG_RD_ADDR_pinmux_rw_pb_iop 12
#define REG_WR_ADDR_pinmux_rw_pb_iop 12

/* Register rw_pc_gio, scope pinmux, type rw */
typedef struct {
  unsigned int pc0  : 1;
  unsigned int pc1  : 1;
  unsigned int pc2  : 1;
  unsigned int pc3  : 1;
  unsigned int pc4  : 1;
  unsigned int pc5  : 1;
  unsigned int pc6  : 1;
  unsigned int pc7  : 1;
  unsigned int pc8  : 1;
  unsigned int pc9  : 1;
  unsigned int pc10 : 1;
  unsigned int pc11 : 1;
  unsigned int pc12 : 1;
  unsigned int pc13 : 1;
  unsigned int pc14 : 1;
  unsigned int pc15 : 1;
  unsigned int pc16 : 1;
  unsigned int pc17 : 1;
  unsigned int dummy1 : 14;
} reg_pinmux_rw_pc_gio;
#define REG_RD_ADDR_pinmux_rw_pc_gio 16
#define REG_WR_ADDR_pinmux_rw_pc_gio 16

/* Register rw_pc_iop, scope pinmux, type rw */
typedef struct {
  unsigned int pc0  : 1;
  unsigned int pc1  : 1;
  unsigned int pc2  : 1;
  unsigned int pc3  : 1;
  unsigned int pc4  : 1;
  unsigned int pc5  : 1;
  unsigned int pc6  : 1;
  unsigned int pc7  : 1;
  unsigned int pc8  : 1;
  unsigned int pc9  : 1;
  unsigned int pc10 : 1;
  unsigned int pc11 : 1;
  unsigned int pc12 : 1;
  unsigned int pc13 : 1;
  unsigned int pc14 : 1;
  unsigned int pc15 : 1;
  unsigned int pc16 : 1;
  unsigned int pc17 : 1;
  unsigned int dummy1 : 14;
} reg_pinmux_rw_pc_iop;
#define REG_RD_ADDR_pinmux_rw_pc_iop 20
#define REG_WR_ADDR_pinmux_rw_pc_iop 20

/* Register rw_pd_gio, scope pinmux, type rw */
typedef struct {
  unsigned int pd0  : 1;
  unsigned int pd1  : 1;
  unsigned int pd2  : 1;
  unsigned int pd3  : 1;
  unsigned int pd4  : 1;
  unsigned int pd5  : 1;
  unsigned int pd6  : 1;
  unsigned int pd7  : 1;
  unsigned int pd8  : 1;
  unsigned int pd9  : 1;
  unsigned int pd10 : 1;
  unsigned int pd11 : 1;
  unsigned int pd12 : 1;
  unsigned int pd13 : 1;
  unsigned int pd14 : 1;
  unsigned int pd15 : 1;
  unsigned int pd16 : 1;
  unsigned int pd17 : 1;
  unsigned int dummy1 : 14;
} reg_pinmux_rw_pd_gio;
#define REG_RD_ADDR_pinmux_rw_pd_gio 24
#define REG_WR_ADDR_pinmux_rw_pd_gio 24

/* Register rw_pd_iop, scope pinmux, type rw */
typedef struct {
  unsigned int pd0  : 1;
  unsigned int pd1  : 1;
  unsigned int pd2  : 1;
  unsigned int pd3  : 1;
  unsigned int pd4  : 1;
  unsigned int pd5  : 1;
  unsigned int pd6  : 1;
  unsigned int pd7  : 1;
  unsigned int pd8  : 1;
  unsigned int pd9  : 1;
  unsigned int pd10 : 1;
  unsigned int pd11 : 1;
  unsigned int pd12 : 1;
  unsigned int pd13 : 1;
  unsigned int pd14 : 1;
  unsigned int pd15 : 1;
  unsigned int pd16 : 1;
  unsigned int pd17 : 1;
  unsigned int dummy1 : 14;
} reg_pinmux_rw_pd_iop;
#define REG_RD_ADDR_pinmux_rw_pd_iop 28
#define REG_WR_ADDR_pinmux_rw_pd_iop 28

/* Register rw_pe_gio, scope pinmux, type rw */
typedef struct {
  unsigned int pe0  : 1;
  unsigned int pe1  : 1;
  unsigned int pe2  : 1;
  unsigned int pe3  : 1;
  unsigned int pe4  : 1;
  unsigned int pe5  : 1;
  unsigned int pe6  : 1;
  unsigned int pe7  : 1;
  unsigned int pe8  : 1;
  unsigned int pe9  : 1;
  unsigned int pe10 : 1;
  unsigned int pe11 : 1;
  unsigned int pe12 : 1;
  unsigned int pe13 : 1;
  unsigned int pe14 : 1;
  unsigned int pe15 : 1;
  unsigned int pe16 : 1;
  unsigned int pe17 : 1;
  unsigned int dummy1 : 14;
} reg_pinmux_rw_pe_gio;
#define REG_RD_ADDR_pinmux_rw_pe_gio 32
#define REG_WR_ADDR_pinmux_rw_pe_gio 32

/* Register rw_pe_iop, scope pinmux, type rw */
typedef struct {
  unsigned int pe0  : 1;
  unsigned int pe1  : 1;
  unsigned int pe2  : 1;
  unsigned int pe3  : 1;
  unsigned int pe4  : 1;
  unsigned int pe5  : 1;
  unsigned int pe6  : 1;
  unsigned int pe7  : 1;
  unsigned int pe8  : 1;
  unsigned int pe9  : 1;
  unsigned int pe10 : 1;
  unsigned int pe11 : 1;
  unsigned int pe12 : 1;
  unsigned int pe13 : 1;
  unsigned int pe14 : 1;
  unsigned int pe15 : 1;
  unsigned int pe16 : 1;
  unsigned int pe17 : 1;
  unsigned int dummy1 : 14;
} reg_pinmux_rw_pe_iop;
#define REG_RD_ADDR_pinmux_rw_pe_iop 36
#define REG_WR_ADDR_pinmux_rw_pe_iop 36

/* Register rw_usb_phy, scope pinmux, type rw */
typedef struct {
  unsigned int en_usb0 : 1;
  unsigned int en_usb1 : 1;
  unsigned int dummy1  : 30;
} reg_pinmux_rw_usb_phy;
#define REG_RD_ADDR_pinmux_rw_usb_phy 40
#define REG_WR_ADDR_pinmux_rw_usb_phy 40


/* Constants */
enum {
  regk_pinmux_no                           = 0x00000000,
  regk_pinmux_rw_hwprot_default            = 0x00000000,
  regk_pinmux_rw_pa_default                = 0x00000000,
  regk_pinmux_rw_pb_gio_default            = 0x00000000,
  regk_pinmux_rw_pb_iop_default            = 0x00000000,
  regk_pinmux_rw_pc_gio_default            = 0x00000000,
  regk_pinmux_rw_pc_iop_default            = 0x00000000,
  regk_pinmux_rw_pd_gio_default            = 0x00000000,
  regk_pinmux_rw_pd_iop_default            = 0x00000000,
  regk_pinmux_rw_pe_gio_default            = 0x00000000,
  regk_pinmux_rw_pe_iop_default            = 0x00000000,
  regk_pinmux_rw_usb_phy_default           = 0x00000000,
  regk_pinmux_yes                          = 0x00000001
};
#endif /* __pinmux_defs_h */
