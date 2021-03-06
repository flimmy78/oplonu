/*
=============================================================================
     Header Name: dal_mac.h

     General Description:
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 zzhu 				   2007/11/13		Initial Version	
----------------   ------------  ----------------------------------------------
*/
#ifndef DAL_MAC_H
#define DAL_MAC_H


#include <opl_driver.h>
#include <opconn_api.h>

#define AGE_TIME			(0XFFFFF)
#define DAL_MAC_TOTAL_NUM	2048
#define DAL_MAX_PORT_LEARN_LIMIT_NUM  1024

#ifndef INOUT
#define IN      /* consider as comments near 'input' parameters */
#define OUT     /* consider as comments near 'output' parameters */
#define INOUT   /* consider as comments near 'input/output' parameters */
#endif

#define DAL_MAC_MAX_LPORT_NUM (11)
#define DAL_MAC_MAX_PHYPORT_NUM (10)

#define DAL_MAC_DEFAULT_AGING_TIME_IN_SEC (1200)

typedef enum tagFDB_MAC_TYPE_E
{
	FDB_MAC_INVALID,
	FDB_MAC_DYNAMIC,
	FDB_MAC_STATIC,
	FDB_MAC_DISCARD,
	FDB_MAC_TYPE_MAX
}FDB_MAC_TYPE_E;

typedef struct tagFDB_MAC_ENTRY_INFO_S
{
	UINT16 usType;
	UINT16 usPortBitmap;
	UINT16 usVlanId;
	UINT8 aucMacAddress[6];
}FDB_MAC_ENTRY_INFO_S;

typedef struct tagFDB_MAC_DROP_INFO_S
{
	UINT8	type;	/*0: unused; 1: used */
	UINT8	mac[6];
}FDB_MAC_DROP_INFO_s;


/*******************************************************************************
* dalArlAgeTimeSet
*
* DESCRIPTION:
*  		this function is used to set the arl age time interval
*
*	INPUTS:
*			ageingTime:
*
*	OUTPUTS:
*		     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlAgeTimeSet(UINT32 ageingTime);
/*******************************************************************************
* dalArlAgeTimeGet
*
* DESCRIPTION:
*  		this function is used to get the arl age time interval
*
*	INPUTS:
*
*	OUTPUTS:
*		ageingTime	     
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlAgeTimeGet(UINT32 *ageingTime);
/*******************************************************************************
* dalArlMacAdd
*
* DESCRIPTION:
*  		this function is used to add a port-mac map to arl table.the mac address may be unicast or 
*	multicast.
*
*	INPUTS:
*		portId: 		portId
*		macAddress:	mac
*		vlanIndex:	vlanIndex
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacAdd(UINT8 portNum,UINT8 *macAddress,UINT16 vlanIndex);

/*******************************************************************************
* dalArlMacDel
*
* DESCRIPTION:
*  		this function is used to del a port-mac map from arl table.the mac address may be unicast or 
*	multicast.if the macAddress is a unicast mac address,then remove the mac from arl,or only remove 
*	the portVec from the arl records.
*
*	INPUTS:
*		portId: 		portId
*		macAddress:	mac
*		vlanIndex:	vlanIndex
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacDel(UINT8 portNum,UINT8 *macAddress,UINT16 vlanIndex);

/*******************************************************************************
* dalArlMacRemove
*
* DESCRIPTION:
*  		this function is used to remove a mac address from the arl table of the vlanIndex.
*
*	INPUTS:
*		macAddress:	mac
*		vlanIndex:	vlanIndex
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacRemove(UINT8 *macAddress,UINT16 vlanIndex);

/*******************************************************************************
* dalArlFlushDynamic
*
* DESCRIPTION:
*  		this function is used to flush all unicast mac address learn by the arl.
*
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlFlushDynamic(void);

/*******************************************************************************
* dalArlFlushAll
*
* DESCRIPTION:
*  		this function is used to flush all mac address from arl table.
*
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlFlushAll(void);

/*******************************************************************************
* dalArlLearnEnSet
*
* DESCRIPTION:
*  		this function is used to enable or disable arl leanning function
*
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlLearnEnSet(UINT8 enable);

/*******************************************************************************
* dalArlLearnEnGet
*
* DESCRIPTION:
*  		this function is used to get the arl learning function status
*
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlLearnEnGet(UINT8 *enable);

/*******************************************************************************
* dalArlSoftLearnEnSet
*
* DESCRIPTION:
*  		this function is used to enable or disable arl leanning function
*
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlSoftLearnEnSet(UINT8 enable);

/*******************************************************************************
* dalArlSoftLearnEnGet
*
* DESCRIPTION:
*  		this function is used to get the arl learning function status
*
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlSoftLearnEnGet(UINT8 *enable);

/*******************************************************************************
* dalArlAgeEnableSet
*
* DESCRIPTION:
*  		this function is used to enable or disable arl age function
*
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlAgeEnableSet(UINT8 enable);

/*******************************************************************************
* dalArlSoftLearnEnGet
*
* DESCRIPTION:
*  		this function is used to get the arl age function status
*
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlAgeEnableGet(UINT8 *enable);


/*******************************************************************************
* dalArlMacNumLimitSet
*
* DESCRIPTION:
*  		this function is used to set num of arl mac limit
*
*	INPUTS:
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK if the access is success.
* 			OPL_ERROR if the access is failed.
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacNumLimitSet(UINT32 portId, UINT8 enable, UINT32 num);

/* begin added by jiangmingli, 2008-08-27 */
/*******************************************************************************
* dalArlMultiPortMacAdd
*
* DESCRIPTION:
*  		this function is used to add an atu entry with multiple ports
*	if the atu entry exists already, add the portlist to this atu entry
*	if the atu entry not exists, add atu entry with the portlist
*
*	INPUTS:
*		uiPortNum	Port number for Port List
*		auiPortlist	the Port List associated with the mac
*		macAddress	the mac address of atu entry
*		usVlanId		Vlanid associated with the mac
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK 		success
* 			OPL_ERROR 	failed
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMultiPortMacAdd
(
	UINT32 uiPortNum,
	UINT32 *auiPortlist,
	UINT8 *aucMacAddress,
	UINT16 usVlanId
);

/*******************************************************************************
* dalArlMultiPortMacDel
*
* DESCRIPTION:
*  		this function is used to delete multiple ports from the desired atu entry
*	if the atu entry exists, delete the portlist of this atu entry
*	if the mac not exists, just return OK
*
*	INPUTS:
*		uiPortNum	Port num for Port List
*		auiPortlist	the Port List associated with the mac
*		macAddress	the mac address needed to be added
*		usVlanId		Vlanid associated with the mac
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK 		success
* 			OPL_ERROR 	failed
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMultiPortMacDel
(
	UINT32 uiPortNum,
	UINT32 *auiPortlist,
	UINT8 *aucMacAddress,
	UINT16 usVlanId
);

/*******************************************************************************
* dalArlMacPortClear
*
* DESCRIPTION:
*  		this function is used to set the portlist of the desired atu entry 0
*	if the atu entry exists, clear the portlist of this atu entry
*	if the mac not exists, just return OK
*
*	INPUTS:
*		macAddress	the mac address needed to be added
*		usVlanId		Vlanid associated with the mac
*		
*	OUTPUTS:
*		
* 	RETURNS:
* 			OPL_OK 		success
* 			OPL_ERROR 	failed
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacPortClear(UINT8 *aucMacAddress, UINT16 usVlanId);

/*******************************************************************************
* dalArlMacEntryShowOne
*
* DESCRIPTION:
*		show the information of desired mac entry
*		if the mac entry not exists, this function will print "no such mac entry"
*	INPUTS:
*		lFd				WriteFd of CLI Env
*		aucMacAddress	the mac address needed to delete
*		usType			FDB_MAC_STATIC	for static
*						FDB_MAC_DYNAMIC for dynamic
*						FDB_MAC_TYPE_MAX for not care
*		uiPortNum	Port number for Port List 
*					0 for not care
*		auiPortlist	the Port List associated with the mac
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_SHOW_ONE_ERROR	show one mac entry error
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacEntryShowOne
(
	IN INT32 lFd,
	IN UINT8 *aucMacAddress,
	IN UINT16 usType,
	IN UINT32 uiPortNum,
	IN UINT32 *auiPortlist
);

/*******************************************************************************
* dalArlMacEntryShowAll
*
* DESCRIPTION:
*		show the information of all mac entry
*	INPUTS:
*		lFd				WriteFd of CLI Env
*		usType			FDB_MAC_STATIC	for static
*						FDB_MAC_DYNAMIC for dynamic
*						FDB_MAC_TYPE_MAX for not care
*		uiPortNum	Port number for Port List 
*					0 for not care
*		auiPortlist	the Port List associated with the mac
*	OUTPUTS:
*		
* 	RETURNS:
* 		NO_ERROR					success
* 		FDB_MAC_SHOW_ONE_ERROR	show one mac entry error
* 	SEE ALSO: 
*/
OPL_STATUS dalArlMacEntryShowAll
(
	IN INT32 lFd,
	IN UINT16 usType,
	IN UINT32 uiPortNum,
	IN UINT32 *auiPortlist
);
/* end added by jiangmingli, 2008-08-27 */

typedef struct s_macentry
{
  int type;
  int port_vec;
  UINT8 mac[6];
}sn_macentry;

OPL_STATUS dalArlMacEntryNumGet(UINT32 filter, UINT32 page_num, 
    UINT32 port_id, UINT32* ptotal_num);

OPL_STATUS dalArlMacEntryShowWeb(UINT32 filter, UINT32 page_num, 
    UINT32 port_id, UINT32* ptotal_num, UINT8 *pdata);

OPL_STATUS dalArlMgmtMacRdt2Cpu(UINT8 * oldMac, UINT8 * newMac);

OPL_STATUS dalArlFlushAllByPort(UINT8 portId);

#endif

