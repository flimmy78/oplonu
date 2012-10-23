/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */

#include "sw.h"
#include "fal_port_ctrl.h"
#include "hsl.h"
#include "f2_phy.h"

PHY_STATUS_INFO_t phyStatus[4] = {{1,1,1,0},{1,1,1,0},{1,1,1,0},{1,1,1,0}};

static a_uint16_t
_phy_reg_read(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint8_t reg)
{
    sw_error_t rv;
    a_uint16_t val;

    HSL_PHY_GET(rv, dev_id, phy_addr, reg, &val);
    if (SW_OK != rv)
         return 0xFFFF;

     return val;
}


static void
_phy_reg_write(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint8_t reg,
                    a_uint16_t val) 
{
    sw_error_t rv;

    HSL_PHY_SET(rv, dev_id, phy_addr, reg, val);
}

#define f2_phy_reg_read _phy_reg_read
#define f2_phy_reg_write _phy_reg_write

/******************************************************************************
*
* f2_phy_debug_write - debug port write 
*
* debug port write 
*/
sw_error_t
f2_phy_debug_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id, 
                                a_uint16_t reg_val)
{
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_ADDRESS, reg_id);
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_DATA, reg_val); 

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_debug_read - debug port read 
*
* debug port read 
*/
a_uint16_t
f2_phy_debug_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t reg_id)
{
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_ADDRESS, reg_id);
    return f2_phy_reg_read(dev_id, phy_id, F2_DEBUG_PORT_DATA); 
}

/******************************************************************************
*
* f2_phy_set_powersave - set power saving status 
*
* set power saving status  
*/
sw_error_t
f2_phy_set_powersave(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
    a_uint16_t phy_data;
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_ADDRESS, 0x29);
    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_DEBUG_PORT_DATA);

    if(enable == A_TRUE) {
        phy_data |= 0x8000;
    } else {
        phy_data &= ~0x8000;
    }
    
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_DATA, phy_data); 

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_get_powersave - get power saving status 
*
* set power saving status  
*/
sw_error_t
f2_phy_get_powersave(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t *enable)
{
    a_uint16_t phy_data;
    *enable = A_FALSE;
    
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_ADDRESS, 0x29);
    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_DEBUG_PORT_DATA);

    if(phy_data & 0x8000)
        *enable =  A_TRUE;

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_set_hibernate - set hibernate status 
*
* set hibernate status 
*/
sw_error_t
f2_phy_set_hibernate(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t enable)
{
    a_uint16_t phy_data;
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_ADDRESS, 0xb);
    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_DEBUG_PORT_DATA);

    if(enable == A_TRUE) {
        phy_data |= 0x8000;
    } else {
        phy_data &= ~0x8000;
    }
    
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_DATA, phy_data); 

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_get_hibernate - get hibernate status 
*
* get hibernate status 
*/
sw_error_t
f2_phy_get_hibernate(a_uint32_t dev_id, a_uint32_t phy_id, a_bool_t *enable)
{
    a_uint16_t phy_data;
    *enable = A_FALSE;
    
    f2_phy_reg_write(dev_id, phy_id, F2_DEBUG_PORT_ADDRESS, 0xb);
    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_DEBUG_PORT_DATA);

    if(phy_data & 0x8000)
        *enable =  A_TRUE;

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_cdt - cable diagnostic test
*
* cable diagnostic test
*/
sw_error_t
f2_phy_cdt(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t mdi_pair,
                fal_cable_status_t *cable_status, a_uint32_t *cable_len) 
{
    a_uint16_t status = 0;
    a_uint16_t ii = 100;

    if(!cable_status || !cable_len) {
        return SW_FAIL;
    }

    if(mdi_pair >= 2) {
        //There are only 4 mdi pairs in 1000BASE-T
        return SW_BAD_PARAM;
    }

    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CDT_CONTROL, (mdi_pair << 8) | 0x0001);

    do {
        aos_mdelay(30);
        status = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CDT_CONTROL);
    } while ((status & 0x0001) && (--ii));

    status = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CDT_STATUS);
    *cable_status = (status & 0x300) >> 8;//(00:normal  01:short 10:opened 11:invalid)

    /*the actual cable length equals to CableDeltaTime * 0.824*/
    a_uint16_t cable_delta_time = status & 0xff;
    *cable_len = (cable_delta_time * 824) /1000;

    /*workaround*/
    if(*cable_len <= 2 && *cable_status == 1)
        *cable_status = 2;
        
    //f2_phy_reg_write(dev_id, phy_id, 0x00, 0x9000);  //Reset the PHY if necessary
    
    return SW_OK;
}

/******************************************************************************
*
* f2_phy_reset_done - reset the phy 
*
* reset the phy
*/
a_bool_t
f2_phy_reset_done(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;
    a_uint16_t ii = 200;

    do {
        phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);
        aos_mdelay(10);
    } while ((!F2_RESET_DONE(phy_data)) && --ii);

    if (ii == 0)
        return A_FALSE;

    return A_TRUE;
}

/******************************************************************************
*
* f2_autoneg_done
*
* f2_autoneg_done
*/
a_bool_t
f2_autoneg_done(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;
    a_uint16_t ii = 200;

    do {
        phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_STATUS);
        aos_mdelay(10);
    } while ((!F2_AUTONEG_DONE(phy_data)) && --ii);

    if (ii == 0)
        return A_FALSE;

    return A_TRUE;
}

/******************************************************************************
*
* f2_phy_Speed_Duplex_Resolved
 - reset the phy 
*
* reset the phy
*/
a_bool_t
f2_phy_speed_duplex_resolved(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;
    a_uint16_t ii = 200;

    do {
        phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_SPEC_STATUS);
        aos_mdelay(10);
    } while ((!F2_SPEED_DUPLEX_RESOVLED(phy_data)) && --ii);

    if (ii == 0)
        return A_FALSE;

    return A_TRUE;
}

/******************************************************************************
*
* f2_phy_reset - reset the phy 
*
* reset the phy
*/
sw_error_t
f2_phy_reset(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);
    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL,
                         phy_data | F2_CTRL_SOFTWARE_RESET);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_off - power off the phy to change its speed
*
* Power off the phy
*/
sw_error_t
f2_phy_poweroff_only(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    f2_phy_reg_write(dev_id, phy_id, 0x1d, 0x3);
    f2_phy_reg_write(dev_id, phy_id, 0x1e, 0x3eff);

	phy_data = F2_CTRL_POWER_DOWN;
    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL, phy_data);

    return SW_OK;
}

sw_error_t
f2_phy_poweroff(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);

	if (!(phy_data & F2_CTRL_POWER_DOWN))
	{
		if (phy_data & F2_CTRL_AUTONEGOTIATION_ENABLE)
		{
			phyStatus[phy_id].autonegotiation = 1;
		}
		else
		{
			phyStatus[phy_id].autonegotiation = 0;
		}

		if (phy_data & F2_CTRL_FULL_DUPLEX)
		{
			phyStatus[phy_id].duplex = 1;
		}
		else
		{
			phyStatus[phy_id].duplex = 0;
		}
			
		if (phy_data & F2_CTRL_SPEED_LSB)
		{
			phyStatus[phy_id].speed = 1;
		}
		else
		{
			phyStatus[phy_id].speed = 0;
		}

		if (phy_data & F2_CTRL_LOOPBACK)
		{
			phyStatus[phy_id].loopback = 1;
		}
		else
		{
			phyStatus[phy_id].loopback = 0;
		}	
	}
	
    f2_phy_reg_write(dev_id, phy_id, 0x1d, 0x3);
    f2_phy_reg_write(dev_id, phy_id, 0x1e, 0x3eff);

	phy_data = F2_CTRL_POWER_DOWN;
    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL, phy_data);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_on - power on the phy after speed changed
*
* Power on the phy
*/
sw_error_t
f2_phy_poweron(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data = 0;
    a_uint16_t phy_data2 = 0;
	__a_uint16_t i;

	if (phyStatus[phy_id].autonegotiation)
	{
		phy_data = 0x1200;
	}
	else
	{
		/* 100M full */
		if ((1 == phyStatus[phy_id].speed) && (1 == phyStatus[phy_id].duplex))
		{
			phy_data = 0xa100;
		}
		/* 100M half */
		if ((1 == phyStatus[phy_id].speed) && (0 == phyStatus[phy_id].duplex))
		{
			phy_data = 0xa000;
		}
		/* 10M full */
		if ((0 == phyStatus[phy_id].speed) && (1 == phyStatus[phy_id].duplex))
		{
			phy_data = 0x8100;
		}
		/* 10M half */
		if ((0 == phyStatus[phy_id].speed) && (0 == phyStatus[phy_id].duplex))
		{
			phy_data = 0x8000;
		}
	}

    if (phyStatus[phy_id].loopback)
	{
		phy_data |= 0x4000;
	}

    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL, phy_data);

    aos_mdelay(200);

#if 0  /* deleted by Gan Zhiheng - 2011/03/22, for but 3410 */
	if (phyStatus[phy_id].autonegotiation)
	{
		/* waiting for auto complete */
        i = 45;
        do {
            phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_STATUS);
            aos_mdelay(10);
        } while ((!F2_AUTONEG_DONE(phy_data)) && --i);
	}

	if (phyStatus[phy_id].loopback)
	{
		phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL); 
		phy_data |= 0x4000;
		f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL, phy_data);		
		aos_mdelay(200);
	}
#endif /* #if 0 */

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_get_ability - get the phy ability
*
* 
*/
sw_error_t
f2_phy_get_ability(a_uint32_t dev_id, a_uint32_t phy_id,
                       a_uint16_t * ability)
{
    a_uint16_t phy_data;

    *ability = 0;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_STATUS);

    if (phy_data & F2_STATUS_AUTONEG_CAPS)
        *ability |= FAL_PHY_AUTONEG_CAPS;

    if (phy_data & F2_STATUS_100T2_HD_CAPS)
        *ability |= FAL_PHY_100T2_HD_CAPS;

    if (phy_data & F2_STATUS_100T2_FD_CAPS)
        *ability |= FAL_PHY_100T2_FD_CAPS;

    if (phy_data & F2_STATUS_10T_HD_CAPS)
        *ability |= FAL_PHY_10T_HD_CAPS;

    if (phy_data & F2_STATUS_10T_FD_CAPS)
        *ability |= FAL_PHY_10T_FD_CAPS;

    if (phy_data & F2_STATUS_100X_HD_CAPS)
        *ability |= FAL_PHY_100X_HD_CAPS;

    if (phy_data & F2_STATUS_100X_FD_CAPS)
        *ability |= FAL_PHY_100X_FD_CAPS;

    if (phy_data & F2_STATUS_100T4_CAPS)
        *ability |= FAL_PHY_100T4_CAPS;

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_get_ability - get the phy ability
*
* 
*/
sw_error_t
f2_phy_get_partner_ability(a_uint32_t dev_id, a_uint32_t phy_id,
                               a_uint16_t * ability)
{
    a_uint16_t phy_data;

    *ability = 0;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_LINK_PARTNER_ABILITY);

    if (phy_data & F2_LINK_10BASETX_HALF_DUPLEX)
        *ability |= FAL_PHY_PART_10T_HD;

    if (phy_data & F2_LINK_10BASETX_FULL_DUPLEX)
        *ability |= FAL_PHY_PART_10T_FD;

    if (phy_data & F2_LINK_100BASETX_HALF_DUPLEX)
        *ability |= FAL_PHY_PART_100TX_HD;

    if (phy_data & F2_LINK_100BASETX_FULL_DUPLEX)
        *ability |= FAL_PHY_PART_100TX_FD;

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_get_power - test to see if the phy power is on
*
* RETURNS:
*    A_TRUE  --> power is on
*    A_FALSE --> power is off
*/
a_bool_t
f2_phy_get_power(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);

    if (!(phy_data & F2_CTRL_POWER_DOWN))
        return A_TRUE;

    return A_FALSE;
}

/******************************************************************************
*
* f2_phy_status - test to see if the specified phy link is alive
*
* RETURNS:
*    A_TRUE  --> link is alive
*    A_FALSE --> link is down
*/
a_bool_t
f2_phy_get_link_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_SPEC_STATUS);

    if (phy_data & 0x400)
        return A_TRUE;

    return A_FALSE;
}

/******************************************************************************
*
* f2_set_autoneg_adv - set the phy autoneg Advertisement
*
*/
sw_error_t
f2_phy_set_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
                           a_uint32_t autoneg)
{
    a_uint16_t phy_data = 0;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_AUTONEG_ADVERT);
    phy_data &= ~F2_ADVERTISE_ALL;
    phy_data &= ~(F2_ADVERTISE_PAUSE | F2_ADVERTISE_ASYM_PAUSE);

    if (autoneg & FAL_PHY_ADV_100TX_FD)
        phy_data |= F2_ADVERTISE_100FULL;

    if (autoneg & FAL_PHY_ADV_100TX_HD)
        phy_data |= F2_ADVERTISE_100HALF;

    if (autoneg & FAL_PHY_ADV_10T_FD)
        phy_data |= F2_ADVERTISE_10FULL;

    if (autoneg & FAL_PHY_ADV_10T_HD)
        phy_data |= F2_ADVERTISE_10HALF;

    if (autoneg & FAL_PHY_ADV_PAUSE)
        phy_data |= F2_ADVERTISE_PAUSE;

    if (autoneg & FAL_PHY_ADV_ASY_PAUSE)
        phy_data |= F2_ADVERTISE_ASYM_PAUSE;

    f2_phy_reg_write(dev_id, phy_id, F2_AUTONEG_ADVERT, phy_data);

    return SW_OK;
}

/******************************************************************************
*
* f2_get_autoneg_adv - get the phy autoneg Advertisement
*
*/
sw_error_t
f2_phy_get_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
                           a_uint32_t * autoneg)
{
    a_uint16_t phy_data = 0;

    *autoneg = 0;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_AUTONEG_ADVERT);

    if (phy_data & F2_ADVERTISE_100FULL)
        *autoneg |= FAL_PHY_ADV_100TX_FD;

    if (phy_data & F2_ADVERTISE_100HALF)
        *autoneg |= FAL_PHY_ADV_100TX_HD;

    if (phy_data & F2_ADVERTISE_10FULL)
        *autoneg |= FAL_PHY_ADV_10T_FD;

    if (phy_data & F2_ADVERTISE_10HALF)
        *autoneg |= FAL_PHY_ADV_10T_HD;

    if (phy_data & F2_ADVERTISE_PAUSE)
        *autoneg |= FAL_PHY_ADV_PAUSE;

    if (phy_data & F2_ADVERTISE_ASYM_PAUSE)
        *autoneg |= FAL_PHY_ADV_ASY_PAUSE;	

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_enable_autonego - power off the phy to change its speed
*
* Power off the phy
*/
a_bool_t
f2_phy_autoneg_status(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);

    if (phy_data & F2_CTRL_AUTONEGOTIATION_ENABLE)
        return A_TRUE;

    return A_FALSE;
}

/******************************************************************************
*
* f2_restart_autoneg - restart the phy autoneg
*
*/
sw_error_t
f2_phy_restart_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data = 0;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);

    phy_data |= F2_CTRL_AUTONEGOTIATION_ENABLE;
    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL,
                         phy_data | F2_CTRL_RESTART_AUTONEGOTIATION);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_enable_autonego - power off the phy to change its speed
*
* Power off the phy
*/
sw_error_t
f2_phy_enable_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data = 0;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);

    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL,
                         phy_data | F2_CTRL_AUTONEGOTIATION_ENABLE);

	phyStatus[phy_id].autonegotiation = 1;

    return SW_OK;
}


/******************************************************************************
*
* f2_phy_disable_autoneg - disable the phy auto-negotiation
*
* Power off the phy
*/
sw_error_t
f2_phy_disable_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data = 0;
	
    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);

    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL,
                         phy_data & ~F2_CTRL_AUTONEGOTIATION_ENABLE);

	phyStatus[phy_id].autonegotiation = 0;

    return SW_OK;
}


/******************************************************************************
*
* f2_phy_get_speed - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*               AG7100_PHY_SPEED_10T, AG7100_PHY_SPEED_100TX;
*               AG7100_PHY_SPEED_1000T;
*/

sw_error_t
f2_phy_get_speed(a_uint32_t dev_id, a_uint32_t phy_id,
                     fal_port_speed_t * speed)
{
    a_uint16_t phy_data;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_SPEC_STATUS);

    //read speed    
    switch (phy_data & F2_STATUS_SPEED_MASK) {
    case F2_STATUS_SPEED_1000MBS:
        *speed = FAL_SPEED_1000;
        break;
    case F2_STATUS_SPEED_100MBS:
        *speed = FAL_SPEED_100;
        break;
    case F2_STATUS_SPEED_10MBS:
        *speed = FAL_SPEED_10;
        break;
    default:
        return SW_READ_ERROR;
    }

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_set_speed - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*               AG7100_PHY_SPEED_10T, AG7100_PHY_SPEED_100TX;
*               AG7100_PHY_SPEED_1000T;
*/
sw_error_t
f2_phy_set_speed(a_uint32_t dev_id, a_uint32_t phy_id,
                     fal_port_speed_t speed)
{
    a_uint16_t phy_data = 0;
    fal_port_duplex_t old_duplex;

    (void)f2_phy_get_duplex(dev_id, phy_id, &old_duplex);

	if (FAL_FULL_DUPLEX == old_duplex)
	{
		phy_data |= F2_CTRL_FULL_DUPLEX;
	}
	
    if (speed == FAL_SPEED_100)
    {
        phy_data |= F2_CTRL_SPEED_100;
    }
    else if (speed == FAL_SPEED_10)
    {
        phy_data |= F2_CTRL_SPEED_10;
    }
    else
    {
        return SW_BAD_PARAM;
    }

	//phy_data |= F2_CTRL_SOFTWARE_RESET; /*fixed bug 385 */

    f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL, phy_data);

    aos_mdelay(300);

    return SW_OK;

}

/******************************************************************************
*
* f2_phy_get_duplex - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*               AG7100_PHY_SPEED_10T, AG7100_PHY_SPEED_100TX;
*               AG7100_PHY_SPEED_1000T;
*/
sw_error_t
f2_phy_get_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
                      fal_port_duplex_t * duplex)
{
    a_uint16_t phy_data;

#if 0
    //a_uint16_t ii = 200;
    a_uint16_t ii = 2;

    if (phy_id >= F2_PHY_MAX)
        return SW_BAD_PARAM;

    do {
        phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_SPEC_STATUS);
        aos_mdelay(10);
    } while ((!(phy_data & F2_STATUS_RESOVLED)) && --ii);

    //read time out    
    if (ii == 0)
        return SW_DISABLE;
#endif

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_SPEC_STATUS);

    //read duplex
    if (phy_data & F2_STATUS_FULL_DUPLEX)
        *duplex = FAL_FULL_DUPLEX;
    else
        *duplex = FAL_HALF_DUPLEX;

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_set_duplex - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*               AG7100_PHY_SPEED_10T, AG7100_PHY_SPEED_100TX;
*               AG7100_PHY_SPEED_1000T;
*/
sw_error_t
f2_phy_set_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
                      fal_port_duplex_t duplex)
{
	a_uint16_t phy_data = 0;
	fal_port_speed_t speed;


	(void)f2_phy_get_speed(dev_id, phy_id, &speed);

	if (FAL_SPEED_100 == speed)
	{
		phy_data |= F2_CTRL_SPEED_100;
	}
	else if (FAL_SPEED_10 == speed)
	{
		phy_data |= F2_CTRL_SPEED_10;
	}
	
	if (duplex == FAL_FULL_DUPLEX)
	{
		phy_data |= F2_CTRL_FULL_DUPLEX;
	}
	else if (duplex == FAL_HALF_DUPLEX)
	{

	}
	else
	{
		return SW_BAD_PARAM;
	}
	
	//phy_data |= F2_CTRL_SOFTWARE_RESET;  /*fixed bug 385 */

	f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL, phy_data);

	aos_mdelay(300);

	return SW_OK;

}

/******************************************************************************
*
* f2_phy_get_lpbk - Get the loopback of phy ports associated with the
* specified device.
*
* RETURNS:
*
*/
sw_error_t
f2_phy_get_lpbk(a_uint32_t dev_id, a_uint32_t phy_id,
                      a_bool_t * enable)
{
    a_uint16_t phy_data;

    phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);

    if (phy_data & F2_CTRL_LOOPBACK)
        *enable = 1;
    else
        *enable = 0;

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_set_lpbk - Determines the loopback of phy ports associated with the
* specified device.
*
* RETURNS:
*
*/
sw_error_t
f2_phy_set_lpbk(a_uint32_t dev_id, a_uint32_t phy_id,
                      a_bool_t enable)
{
	a_uint16_t phy_data = 0;

	phy_data = f2_phy_reg_read(dev_id, phy_id, F2_PHY_CONTROL);

	if (enable)
	{
		phy_data |= F2_CTRL_LOOPBACK;
	}
	else
	{
		phy_data &= ~F2_CTRL_LOOPBACK;
	}

	phyStatus[phy_id].loopback = enable;
	
	f2_phy_reg_write(dev_id, phy_id, F2_PHY_CONTROL, phy_data);

	aos_mdelay(200);

	return SW_OK;

}


/******************************************************************************
*
* f2_phy_get_phy_id - get the phy id
*
*/
static sw_error_t
f2_phy_get_phy_id(a_uint32_t dev_id, int phy_id,
                      a_uint16_t * org_id, a_uint16_t * rev_id)
{
    *org_id = f2_phy_reg_read(dev_id, phy_id, F2_PHY_ID1);
    *rev_id = f2_phy_reg_read(dev_id, phy_id, F2_PHY_ID2);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_init - 
*
*/
a_bool_t
f2_phy_init(a_uint32_t dev_id, int phy_id,
                a_uint16_t org_id, a_uint16_t rev_id)
{
    a_uint16_t org_tmp, rev_tmp;

    (void)f2_phy_get_phy_id(dev_id, phy_id, &org_tmp, &rev_tmp);
    if ((org_id == org_tmp) && (rev_id == rev_tmp))
        return A_TRUE;
    else
        return A_FALSE;
}