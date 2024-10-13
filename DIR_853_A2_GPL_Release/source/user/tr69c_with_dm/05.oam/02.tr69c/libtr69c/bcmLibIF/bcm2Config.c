/***********************************************************************
 *
 *  Copyright (c) 2013  Broadcom Corporation
 *  All Rights Reserved
 *
<:label-BRCM:2013:proprietary:standard

 This program is the proprietary software of Broadcom Corporation and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
 *
 ************************************************************************/

#ifdef DMP_DEVICE2_BASELINE_1
#include "cms.h"
#include "cms_mdm.h"
#include "cms_obj.h"
#include "cms_phl.h"
#include "cms_util.h"
#include "cms_core.h"
#include "qdm_ipintf.h"

#include "bcmConfig.h"
#include "bcmWrapper.h"
#include "../inc/appdefs.h"
#include "../main/informer_public.h"

// these global variables are defined in main.c
extern ACSState acsState;
extern UBOOL8 loggingSOAP; 
extern int g_TR069WANIPChanged;
extern void *tmrHandle;

extern void changeNameSpaceCwmpVersionURL(int version);
extern void setsaveConfigFlag(UBOOL8 flagh); /* in RPCState.h */

CmsRet getLanIPAddressInfo_dev2(char **addr, char **subnetmask)
{
//   char ifName[CMS_IFNAME_LENGTH]={0};
//   char ipAddress[CMS_IPADDR_LENGTH]={0};
//   char subnetMask[CMS_IPADDR_LENGTH]={0};
   CmsRet ret = CMSRET_OBJECT_NOT_FOUND;
#if 0
   if (qdmIpIntf_getDefaultLanIntfNameLocked_dev2(ifName) == CMSRET_SUCCESS &&
       qdmIpIntf_getIpv4AddrAndSubnetByNameLocked_dev2(ifName, ipAddress, subnetMask) == CMSRET_SUCCESS)
   {
      if (addr)
      {
         *addr = cmsMem_strdup(ipAddress);
      }

      if (subnetmask)
      {
         *subnetmask = cmsMem_strdup(subnetMask);
      }

      ret = CMSRET_SUCCESS;
   }
#endif
   return ret;
}


char *getFullPathToIpvxAddrLocked_dev2(UINT32 ipvx, const char *ipAddr)
{

//   MdmPathDescriptor pathDesc=EMPTY_PATH_DESCRIPTOR;
   char *fullPath=NULL;
//   UBOOL8 found = FALSE;
//   CmsRet ret = CMSRET_SUCCESS;
#if 0

   if (IS_EMPTY_STRING(ipAddr))
   {
      cmsLog_error("ipAddr is empty, just return");
      return NULL;
   }

   cmsLog_notice("Looking for fullpath to %s", ipAddr);

   /*
    * Handle special case of loopback addr first.  This is only used by
    * tr69c unittest code.  Since we don't create an object in the
    * Data Model for the loopback interface, just use the first LAN (br0)
    * interface.  The unittest code does not check if it points to a
    * correct location.  It is only looking for a string in the response.
    */
#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      if (!cmsUtl_strcmp(ipAddr, "::1"))
      {
         fullPath = cmsMem_strdup("Device.IP.Interface.1.IPv6Address.1");
         return fullPath;
      }
   }
#endif

   if ((ipvx & CMS_AF_SELECT_IPV4) &&
       !cmsUtl_strcmp(ipAddr, "127.0.0.1"))
   {
      fullPath = cmsMem_strdup("Device.IP.Interface.1.IPv4Address.1");
      return fullPath;
   }


#ifdef DMP_X_BROADCOM_COM_DEV2_IPV6_1
   if (ipvx & CMS_AF_SELECT_IPV6)
   {
      Dev2Ipv6AddressObject *ipv6AddrObj = NULL;
      InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;

      while (!found &&
             (ret = cmsObj_getNext(MDMOID_DEV2_IPV6_ADDRESS, &iidStack,
                               (void **) &ipv6AddrObj)) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(ipv6AddrObj->IPAddress, ipAddr))
         {
            found = TRUE;
            pathDesc.oid = MDMOID_DEV2_IPV6_ADDRESS;
            pathDesc.iidStack = iidStack;
            sprintf(pathDesc.paramName, "IPAddress");
         }

         cmsObj_free((void **) &ipv6AddrObj);
      }
   }
#endif

   if (ipvx & CMS_AF_SELECT_IPV4)
   {
      Dev2Ipv4AddressObject *ipv4AddrObj = NULL;
      InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;

      while (!found &&
             (ret = cmsObj_getNext(MDMOID_DEV2_IPV4_ADDRESS, &iidStack,
                               (void **) &ipv4AddrObj)) == CMSRET_SUCCESS)
      {
         if (!cmsUtl_strcmp(ipv4AddrObj->IPAddress, ipAddr))
         {
            found = TRUE;
            pathDesc.oid = MDMOID_DEV2_IPV4_ADDRESS;
            pathDesc.iidStack = iidStack;
            sprintf(pathDesc.paramName, "IPAddress");
         }

         cmsObj_free((void **) &ipv4AddrObj);
      }
   }

   if (found)
   {
      ret = cmsMdm_pathDescriptorToFullPath(&pathDesc, &fullPath);
      if (ret != CMSRET_SUCCESS)
      {
         cmsLog_error("could not convert pathDesc to fullPath");
      }
      else
      {
         cmsLog_debug("mapped %s to %s", ipAddr, fullPath);
      }
   }
   else
   {
      /*
       * In Hybrid mode, we might call this _dev2 function to look for IPv6
       * address first.  It is OK if we don't find it here because it may
       * be an IPv4 addr which is stored in the TR98 data model.
       */
      cmsLog_debug("Could not find %s in (dev2) data model", ipAddr);
   }
#endif
   return fullPath;
}

      
void clearModemConnectionURL_dev2(void)
{
   Dev2ManagementServerObject *obj=NULL;
   CmsRet ret;

   ret = cmsPhl_getAcsCfg(&obj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"could not get management server object!");
   }
   else 
   {
      CMSMEM_FREE_BUF_AND_NULL_PTR(obj->connectionRequestURL);

      if ((ret = cmsPhl_setAcsCfg(obj)) != CMSRET_SUCCESS)
      {
         cmsLog_error(TR069_ALARM_PROCESS_ERROR,"set of connectionRequestURL to NULL failed, ret=%d", ret);
      }

      cmsPhl_free((void **) &obj,1);
   }

}

UBOOL8 isTimeSynchronized_dev2(void)
{
   UBOOL8 synchronized = FALSE;

#ifdef DMP_DEVICE2_TIME_1
   Dev2TimeObject *timeObj=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   if ((ret = cmsPhl_getTimeObj(&timeObj)) != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"get of DEV2_TIME object failed, ret=%d", ret);
   }
   else
   {
      if (timeObj->enable == TRUE &&
          cmsUtl_strcmp(timeObj->status, MDMVS_SYNCHRONIZED) == 0)
      {
         synchronized = TRUE;
      }

      cmsPhl_free((void **) &timeObj,1);
   }

#endif /* DMP_DEVICE2_TIME_1 */
   
   return synchronized;
}

/* Build a list of parameters that need to be set defaultActiveNotification */
void setDefaultActiveNotificationLocked_dev2(PhlSetParamAttr_t **pSetParmAttrList, int *numEntries)
{
#if 0
#ifdef TR181_IMPLEMENTATION
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject *ipConnObj = NULL;
   WanPppConnObject *pppConnObj = NULL;
   PhlSetParamAttr_t *pList, *pSetParamAttr;
   int num = 1;   /* ConnectionRequestURL counted */


   /* There are 2 parameters that needs to be forced to Active Notification
    * TR181-2-6-0.xml specification, Forced Default Active Notification parameters.
    * Device.DeviceInfo.SoftwareVersion
    * Device.DeviceInfo.ProvisioningCode
    * And Default Active Notification Parameters are:
    * Device.ManagementServer.ConnectionRequestURL
    * Device.SoftwareModules.ExecutionUnit.{i}.Status
    * Device.FAP.GPS.LastScanTime
    */
   while (cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConnObj) == CMSRET_SUCCESS)
   {
      num++;
      cmsObj_free((void **) &ipConnObj);
   }
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConnObj) == CMSRET_SUCCESS)
   {
      num++;
      cmsObj_free((void **) &pppConnObj);      
   }

   pList = cmsMem_alloc(num * sizeof(PhlSetParamAttr_t),ALLOC_ZEROIZE);
   if (pList == NULL)
   {
      cmsLck_releaseLock();
      return;
   }
   *numEntries = num;
   pSetParamAttr = pList;

   /* first, ConnectionRequestURL */
   cmsMdm_fullPathToPathDescriptor("InternetGatewayDevice.ManagementServer.ConnectionRequestURL",
                                   &(pSetParamAttr->pathDesc));
   pSetParamAttr->attributes.notificationChange = 1;
   pSetParamAttr->attributes.notification = NDA_TR69_ACTIVE_NOTIFICATION;
   pSetParamAttr->attributes.accessBitMask = NDA_ACCESS_TR69C;
   pSetParamAttr++;

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConnObj)) == CMSRET_SUCCESS)
   {
      pSetParamAttr->pathDesc.oid = MDMOID_WAN_IP_CONN;
      pSetParamAttr->pathDesc.iidStack = iidStack;
      sprintf(pSetParamAttr->pathDesc.paramName, "ExternalIPAddress");
      pSetParamAttr->attributes.notificationChange = 1;
      pSetParamAttr->attributes.notification = NDA_TR69_ACTIVE_NOTIFICATION;
      pSetParamAttr->attributes.accessBitMask = NDA_ACCESS_TR69C;
      pSetParamAttr++;
      cmsObj_free((void **) &ipConnObj);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConnObj)) == CMSRET_SUCCESS)
   {
      pSetParamAttr->pathDesc.oid = MDMOID_WAN_PPP_CONN;
      pSetParamAttr->pathDesc.iidStack = iidStack;
      sprintf(pSetParamAttr->pathDesc.paramName, "ExternalIPAddress");
      pSetParamAttr->attributes.notificationChange = 1;
      pSetParamAttr->attributes.notification = NDA_TR69_ACTIVE_NOTIFICATION;
      pSetParamAttr->attributes.accessBitMask = NDA_ACCESS_TR69C;
      pSetParamAttr++;
      cmsObj_free((void **) &pppConnObj);
   }
   *pSetParmAttrList = pList;

#else
   InstanceIdStack iidStack=EMPTY_INSTANCE_ID_STACK;
   WanIpConnObject *ipConnObj = NULL;
   WanPppConnObject *pppConnObj = NULL;
   PhlSetParamAttr_t *pList, *pSetParamAttr;
   int num = 1;   /* ConnectionRequestURL counted */

   /* There are 2 parameters that needs to be forced to Active Notification
    * TR181-2-6-0.xml specification, Forced Default Active Notification parameters.
    * Device.DeviceInfo.SoftwareVersion
    * Device.DeviceInfo.ProvisioningCode
    * And Default Active Notification Parameters are:
    * Device.ManagementServer.ConnectionRequestURL
    * Device.SoftwareModules.ExecutionUnit.{i}.Status
    * Device.FAP.GPS.LastScanTime
    */
   while (cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConnObj) == CMSRET_SUCCESS)
   {
      num++;
      cmsObj_free((void **) &ipConnObj);
   }
   INIT_INSTANCE_ID_STACK(&iidStack);
   while (cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConnObj) == CMSRET_SUCCESS)
   {
      num++;
      cmsObj_free((void **) &pppConnObj);      
   }

   pList = cmsMem_alloc(num * sizeof(PhlSetParamAttr_t),ALLOC_ZEROIZE);
   if (pList == NULL)
   {
      cmsLck_releaseLock();
      return;
   }
   *numEntries = num;
   pSetParamAttr = pList;

   /* first, ConnectionRequestURL */
   cmsMdm_fullPathToPathDescriptor("InternetGatewayDevice.ManagementServer.ConnectionRequestURL",
                                   &(pSetParamAttr->pathDesc));
   pSetParamAttr->attributes.notificationChange = 1;
   pSetParamAttr->attributes.notification = NDA_TR69_ACTIVE_NOTIFICATION;
   pSetParamAttr->attributes.accessBitMask = NDA_ACCESS_TR69C;
   pSetParamAttr++;

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((cmsObj_getNext(MDMOID_WAN_IP_CONN, &iidStack, (void **) &ipConnObj)) == CMSRET_SUCCESS)
   {
      pSetParamAttr->pathDesc.oid = MDMOID_WAN_IP_CONN;
      pSetParamAttr->pathDesc.iidStack = iidStack;
      sprintf(pSetParamAttr->pathDesc.paramName, "ExternalIPAddress");
      pSetParamAttr->attributes.notificationChange = 1;
      pSetParamAttr->attributes.notification = NDA_TR69_ACTIVE_NOTIFICATION;
      pSetParamAttr->attributes.accessBitMask = NDA_ACCESS_TR69C;
      pSetParamAttr++;
      cmsObj_free((void **) &ipConnObj);
   }

   INIT_INSTANCE_ID_STACK(&iidStack);
   while ((cmsObj_getNext(MDMOID_WAN_PPP_CONN, &iidStack, (void **) &pppConnObj)) == CMSRET_SUCCESS)
   {
      pSetParamAttr->pathDesc.oid = MDMOID_WAN_PPP_CONN;
      pSetParamAttr->pathDesc.iidStack = iidStack;
      sprintf(pSetParamAttr->pathDesc.paramName, "ExternalIPAddress");
      pSetParamAttr->attributes.notificationChange = 1;
      pSetParamAttr->attributes.notification = NDA_TR69_ACTIVE_NOTIFICATION;
      pSetParamAttr->attributes.accessBitMask = NDA_ACCESS_TR69C;
      pSetParamAttr++;
      cmsObj_free((void **) &pppConnObj);
   }
   *pSetParmAttrList = pList;

#endif
#endif
}

UBOOL8 checkWanDslDiagnosticObjID_dev2(MdmPathDescriptor *pathDesc)
{
   pathDesc = pathDesc;  // to avoid compile error for LINUX_DESKTOP
#ifdef TR181_IMPLEMENTATATION
   if (pathDesc->oid == MDMOID_WAN_DSL_DIAG)
      return TRUE;
   else
      return FALSE;
#else
   return TRUE;
#endif
}

CmsRet getSessionRetryParameters_dev2(int *waitInterval, int *multiplier)
{
   Dev2ManagementServerObject *obj=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   ret = cmsPhl_getAcsCfg(&obj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"could not get management server object!");
   }
   else 
   {
      *waitInterval = obj->CWMPRetryMinimumWaitInterval;
      *multiplier = obj->CWMPRetryIntervalMultiplier;
      cmsPhl_free((void **) &obj,1);
   }

   return (ret);
}

CmsRet setMgmtServerParameterKeyLocked_dev2(const char *paramKey)
{
   /* TR181 Forced Active Notification Parameters */
   /* Device.DeviceInfo.SoftwareVersion */
   /* Device.DeviceInfo.ProvisioningCode */
   /* TR181 Default Active Notification Parameters */
   /* Device.ManagementServer.ConnectionRequestURL */
   /* Device.SoftwareModules.ExecutionUnits.{i}.Status */
   /* Device.FAP.GPS.LastScanTime */
   Dev2ManagementServerObject *obj=NULL;
   CmsRet ret = CMSRET_SUCCESS;

   ret = cmsPhl_getAcsCfg(&obj);
   if (ret != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"could not get management server object!");
      return (ret);
   }

   CMSMEM_REPLACE_STRING(obj->parameterKey, paramKey);

   if ((ret = cmsPhl_setAcsCfg(obj)) != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"set of paramKey %s failed, ret=%d", paramKey, ret);
   }

   cmsPhl_free((void **) &obj,1);

   return (ret);
}

/* Copy settings from the MDM into acsState.
 * As a side effect, global variable.
 */
void updateTr69cCfgInfo_dev2(void)
{
   UrlProto urlProto;
//   char *urlAddr, *urlPath;
   UINT16 urlPort=0;
   Dev2ManagementServerObject *acsCfg = NULL;
   Tr69cCfgObject *tr69cCfg = NULL;
   UBOOL8 connReqURLchanged = FALSE;
   CmsRet ret;
   UBOOL8 sendInformNeeded = FALSE;

   cmsLog_debug("Entered ");

   /*
    * Fill in our deviceInfo params only if has not been done before.
    * Once we've filled it in, no need to do it again.  It will not
    * change while the system is still up.
    */
   if (acsState.manufacturer == NULL)
   {
      Dev2DeviceInfoObject *deviceInfoObj=NULL;

      if ((ret = cmsPhl_getDeviceInfo(&deviceInfoObj)) != CMSRET_SUCCESS)
      {
         cmsLog_error(TR069_ALARM_PROCESS_ERROR,"could not get DEV2_DEVICE_INFO object!, ret=%d", ret);
      }
      else
      {
         cmsLog_debug("get DEV2_DEVICE_INFO object");
         cmsLog_debug("%s/%s/%s/%s", deviceInfoObj->manufacturer, deviceInfoObj->manufacturerOUI, deviceInfoObj->productClass, deviceInfoObj->serialNumber);

         CMSMEM_REPLACE_STRING(acsState.manufacturer, deviceInfoObj->manufacturer);
         CMSMEM_REPLACE_STRING(acsState.manufacturerOUI, deviceInfoObj->manufacturerOUI);
         CMSMEM_REPLACE_STRING(acsState.productClass, deviceInfoObj->productClass);
         CMSMEM_REPLACE_STRING(acsState.serialNumber, deviceInfoObj->serialNumber);
         snprintf(acsState.SelfDefinedPrefix,sizeof(acsState.SelfDefinedPrefix),"X_%s_",deviceInfoObj->manufacturerOUI);

         cmsPhl_free((void **) &deviceInfoObj,1);
      }
   }


   /*
    * get tr69c config object
    */
   if ((ret = cmsPhl_getTr69cCfg((void *) &tr69cCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"get of TR69C_CFG failed, ret=%d", ret);
   }
   else
   {
      loggingSOAP = tr69cCfg->loggingSOAP;
      acsState.noneConnReqAuth = (tr69cCfg->connectionRequestAuthentication == TRUE) ? 0 : 1;
      acsState.enableSelfdefinedEventCode = tr69cCfg->enableSelfdefinedEventCode;
      cmsLog_notice("acsState.enableSelfdefinedEventCode=%d",acsState.enableSelfdefinedEventCode);

      /* load tls config */
      acsState.tlsCaCertRequired = tr69cCfg->CaCertRequired;
      CMSMEM_REPLACE_STRING(acsState.tlsCiphers, tr69cCfg->Ciphers);
      CMSMEM_REPLACE_STRING(acsState.tlsCertDir, tr69cCfg->CertDir);
      CMSMEM_REPLACE_STRING(acsState.tlsClientCertFile, tr69cCfg->ClientCertFile);
      CMSMEM_REPLACE_STRING(acsState.tlsClientPrivateKeyFile, tr69cCfg->ClientPrivateKeyFile);
      CMSMEM_REPLACE_STRING(acsState.tlsCaCertFile, tr69cCfg->CaCertFile);

      cmsPhl_free((void **) &tr69cCfg,1);
   }

   /*
    * Get managment server object.
    */
   if ((ret = cmsPhl_getAcsCfg((void *) &acsCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"get of DEV2_MANAGEMENT_SERVER failed, ret=%d", ret);
      return;
   }

   // ACS URL
   /* case 1) : tr69c is alive, if URL is changed, we need to update acsState.acsURL, and reset retryCount
    *              to 0.
    *              Because lastConnectedURL can't be changed before cpe received InformResponse from ACS
    *              successfully, we can't reset retryCount at case 3.
    *
    * case 2) : tr69c is exit normally(such as we only have periodic inform, no retry, and out of select time
    *              out) or cpe just boot, when tr69c is launch again for some reason(value change, periodicinform), 
    *		     we need to initialize acsState.acsURL.
    *
    * case 3) : we will add  INFORM_EVENT_BOOTSTRAP in this case, because acsState.acsURL will lose
    *               its value when tr69c exit.(we will set the value of acsCfg->lastConnectedURL when we
    *               receive Informresponse)
    *
    */
   if(acsCfg->URL != NULL)
   {
      /*case 1: tr69c is alive, */
      if(acsState.acsURL != NULL)
      {
         if (cmsUtl_strcmp(acsState.acsURL, acsCfg->URL) != 0)
         {
            CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.acsURL); 
            acsState.acsURL = cmsMem_strdup(acsCfg->URL);
            acsState.retryCount = 0; //reset retryCount when ACS URL is changed.
            /*acsState.cwmpVersion = CWMP_VERSION_1_2;*/
            changeNameSpaceCwmpVersionURL(acsState.cwmpVersion);
            cmsLog_debug("acsURL changed, add eIEBootStrap inform event");
         }
      }
      else /*case 2 : cpe just boot or tr69c exit normally*/
      {
         acsState.acsURL = cmsMem_strdup(acsCfg->URL);
      }
      cmsLog_debug("acsState.acsURL=%s", acsState.acsURL);
   
      /*case 3 */
      if ((acsCfg->lastConnectedURL == NULL) ||
          (cmsUtl_strcmp(acsCfg->lastConnectedURL, acsCfg->URL) != 0))
      {
         addInformEventToList(INFORM_EVENT_BOOTSTRAP);
         addInformEventToList(INFORM_EVENT_BOOT);
         cmsLog_notice("setting acsURL for the first time, add eIEBootStrap event to inform list. lastConnectedURL=%s, acsCfg->URL=%s", 
         acsCfg->lastConnectedURL, acsCfg->URL);

         if (!isAcsConnected())
         {
            sendInformNeeded = TRUE;
         }

         /* when bootstrap is sent, parameters on table 5 of TR98 specification
          * needs to be reset back to default Active Notification.
          */
         // TODO: make setDefaultActiveNotification work with Interface stack
         //setDefaultActiveNotification();
      }
   }

   // ACS username
   if (acsState.acsUser!= NULL && acsCfg->username != NULL)
   {
      if (cmsUtl_strcmp(acsState.acsUser, acsCfg->username) != 0)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.acsUser); 
         acsState.acsUser = cmsMem_strdup(acsCfg->username);
      }
   }
   else if (acsCfg->username != NULL)
   {
      acsState.acsUser = cmsMem_strdup(acsCfg->username);
   }

   // ACS password
   if (acsState.acsPwd != NULL && acsCfg->password != NULL)
   {
      if (cmsUtl_strcmp(acsState.acsPwd, acsCfg->password) != 0)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.acsPwd); 
         acsState.acsPwd = cmsMem_strdup(acsCfg->password);
      }
   }
   else if (acsCfg->password != NULL)
   {
      acsState.acsPwd = cmsMem_strdup(acsCfg->password);
   }

   // update differentiated services field...
   acsState.diffServ = acsCfg->diffServ;

   // load tls configuration, but tls config only work when power on...
   acsState.tlsVersion = acsCfg->tlsVersion;
   CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.tlsAuthenticationMethod); 
   acsState.tlsAuthenticationMethod = cmsMem_strdup(acsCfg->tlsAuthenticationMethod);
   CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.tlsPeerAuthenticationMethod); 
   acsState.tlsPeerAuthenticationMethod = cmsMem_strdup(acsCfg->tlsPeerAuthenticationMethod);

   // connectionRequestURL
   if (acsState.connReqURL != NULL && acsCfg->connectionRequestURL != NULL)
   {
      if (cmsUtl_strcmp(acsState.connReqURL, acsCfg->connectionRequestURL) != 0)
      {
         CMSMEM_REPLACE_STRING(acsState.connReqURL, acsCfg->connectionRequestURL);
         connReqURLchanged = TRUE;
      }
      if ((acsState.connReqIpAddr != NULL) && (acsState.connReqIpAddrFullPath == NULL))
      {
         /* if a path had not been built because External IP address was not up
            due to layer 2 link down.  Try to build it.
         */
         connReqURLchanged = TRUE;
      }
   }
   else if (acsCfg->connectionRequestURL != NULL)
   {
      acsState.connReqURL = cmsMem_strdup(acsCfg->connectionRequestURL);
      connReqURLchanged = TRUE;
   }

   // connReqIpAddr, connReqIfNameFullPath, connReqPath
   if (connReqURLchanged)
   {
      /*
       * ConnectionRequestURL has changed or has been set to the acsState
       * for the first time.  Update the 3 other variables associated with
       * connReqURL.
       */
      CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.connReqIpAddr);
      CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.connReqIpAddrFullPath);
      CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.connReqPath);

      g_TR069WANIPChanged = 1;
      /*
       * parseUrl should always succeed since our own STL handler function
       * built this URL.  Note this algorithm assumes the IP address portion
       * is always in dotted decimal format, not a DNS name.  I think this is
       * a safe assumption.
       */
      cmsUtl_parseUrl(acsCfg->connectionRequestURL, &urlProto, &acsState.connReqIpAddr, &urlPort, &acsState.connReqPath);
      cmsLog_notice("connReqURL=%s ==> connReqIPAddr=%s connReqPath=%s",
                   acsCfg->connectionRequestURL,
                   acsState.connReqIpAddr, acsState.connReqPath);

   }

   // connectionRequestUsername
   if (acsState.connReqUser != NULL && acsCfg->connectionRequestUsername != NULL)
   {
      if (cmsUtl_strcmp(acsState.connReqUser, acsCfg->connectionRequestUsername) != 0)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.connReqUser); 
         acsState.connReqUser = cmsMem_strdup(acsCfg->connectionRequestUsername);
      }
   }
   else if (acsCfg->connectionRequestUsername != NULL)
   {
      acsState.connReqUser = cmsMem_strdup(acsCfg->connectionRequestUsername);
   }

   // connectionRequestPassword
   if (acsState.connReqPwd != NULL && acsCfg->connectionRequestPassword != NULL)
   {
      if (cmsUtl_strcmp(acsState.connReqPwd, acsCfg->connectionRequestPassword) != 0)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(acsState.connReqPwd); 
         acsState.connReqPwd = cmsMem_strdup(acsCfg->connectionRequestPassword);
      }
   }
   else if (acsCfg->connectionRequestPassword != NULL)
   {
      acsState.connReqPwd = cmsMem_strdup(acsCfg->connectionRequestPassword);
   }

   // boundIfName, it should never be NULL
   if (cmsUtl_strcmp(acsState.boundIfName, acsCfg->X_BROADCOM_COM_BoundIfName) != 0)
   {
      CMSMEM_REPLACE_STRING(acsState.boundIfName, acsCfg->X_BROADCOM_COM_BoundIfName);
   }

   // Periodic Inform Interval
   if (acsState.informInterval != (SINT32)acsCfg->periodicInformInterval)
   {
      acsState.informInterval = acsCfg->periodicInformInterval;
      if (acsState.informEnable == TRUE)
      {
         cancelPeriodicInform();
         resetPeriodicInform(acsCfg->periodicInformInterval);
      }
   }

   // Periodic Inform Enable
   if (acsState.informEnable != acsCfg->periodicInformEnable)
   {
      acsState.informEnable = acsCfg->periodicInformEnable;
      if (acsState.informEnable == TRUE)
      {
         resetPeriodicInform(acsCfg->periodicInformInterval);
         cmsLog_debug("periodic inform is now enabled, add event INFORM_EVENT_PERIODIC");
      }
      else
      {
         cancelPeriodicInform();
      }
   }

   cmsPhl_free((void **)&acsCfg,1);

   if(sendInformNeeded)
   {
      cmsTmr_cancel(tmrHandle, sendInform, NULL);
      cmsTmr_set(tmrHandle, sendInform, NULL, 0, "bootstrap_sendInform");
   }
}

/*qingpu : save URL to LastConnectedURL if necessary---move from rpcState.c*/
void saveLastConnectedURL_dev2(void)
{
//   InstanceIdStack iidStack = EMPTY_INSTANCE_ID_STACK;
   Dev2ManagementServerObject *acsCfg = NULL;
   UBOOL8 lastconnectedURLchanged = FALSE;
   CmsRet ret;
   
   /*
    * Get managment server object.
    */
   if ((ret = cmsPhl_getAcsCfg((void *) &acsCfg)) != CMSRET_SUCCESS)
   {
      cmsLog_error(TR069_ALARM_PROCESS_ERROR,"get of DEV2_MANAGEMENT_SERVER failed, ret=%d", ret);
      return;
   }

   if(acsCfg->URL != NULL)
   {
      if (acsCfg->lastConnectedURL == NULL){
         acsCfg->lastConnectedURL = cmsMem_strdup(acsCfg->URL);
         lastconnectedURLchanged = TRUE;
      }
      else if(strcmp(acsCfg->lastConnectedURL, acsCfg->URL) != 0)
      {
         CMSMEM_FREE_BUF_AND_NULL_PTR(acsCfg->lastConnectedURL); 
         acsCfg->lastConnectedURL = cmsMem_strdup(acsCfg->URL);
         lastconnectedURLchanged = TRUE;
      }

      cmsLog_debug("saving URL to lastConnectedURL. lastConnectedURL=%s, acsCfg->URL=%s", 
      acsCfg->lastConnectedURL, acsCfg->URL);

      if(lastconnectedURLchanged)
      {
         ret=cmsPhl_setAcsCfg(acsCfg);
         if(ret != CMSRET_SUCCESS)
         {
            cmsLog_error(TR069_ALARM_PROCESS_ERROR,"cmsObj_set MDMOID_DEV2_MANAGEMENT_SERVER failed ret=%d", ret);
         }
         else
            setsaveConfigFlag(TRUE);
      }
   }

   cmsPhl_free((void **) &acsCfg,1);
}


#endif /* DMP_DEVICE2_BASELINE_1 */
