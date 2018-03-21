#ifndef _PHY_H
#define _PHY_H

#include <config.h>
/*
 * This file defines the interface between MAC and various phy switches.
 */
#define ag7240_unit2name(_unit) _unit ?  "eth1" : "eth0"
extern int  ag7240_miiphy_read(char *devname, uint32_t phaddr,
		uint8_t reg);
extern int  ag7240_miiphy_write(char *devname, uint32_t phaddr,
		uint8_t reg, uint16_t data);

#if defined(CONFIG_HW29764037P8P64) || defined(CONFIG_HW29763948P128P128) || defined(CONFIG_HW29763948P0P128P128P2X2P3X3)
#define phy_reg_read(base, addr, reg)                    \
    ag7240_miiphy_read(ag7240_unit2name(base), addr, reg)
#define phy_reg_write(base, addr, reg, data)                   \
        ag7240_miiphy_write(ag7240_unit2name(base), addr, reg, data)
#else
#define phy_reg_read(base, addr, reg)                    \
    ag7240_miiphy_read(ag7240_unit2name(base), addr, reg);
#define phy_reg_write(base, addr, reg, data)                   \
        ag7240_miiphy_write(ag7240_unit2name(base), addr, reg, data);
#endif

#endif
