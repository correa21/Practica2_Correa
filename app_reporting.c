/*
* Copyright 2016-2017 NXP
* All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/*!=============================================================================
\file       app_reporting.c
\brief      Base Device application - reporting functionality
==============================================================================*/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <jendefs.h>
#include <string.h>
#include "NVM_Interface.h"
#include "dbg.h"
#include "app_zcl_cfg.h"
#include "PDM.h"
#include "PDM_IDs.h"
#include "app_common.h"
#include "zcl_options.h"
#include "zcl_common.h"
#include "app_reporting.h"
#ifdef CLD_GROUPS
#include "Groups.h"
#include "OnOff.h"
#include "Groups_internal.h"
#include "TemperatureMeasurement.h"
#endif
#include "FunctionLib.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#ifdef DEBUG_REPORT
    #define TRACE_REPORT   TRUE
#else
    #define TRACE_REPORT   FALSE
#endif
/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/


/*There are just two attributes at this point - OnOff and CurrentLevel */

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/


/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/*Just Two reports for time being*/
PUBLIC tsReports asSavedReports[NUMBER_OF_REPORTS];

/* register reports to nvm\pdm */
NVM_RegisterDataSet(asSavedReports, 1, sizeof(asSavedReports), PDM_ID_APP_REPORTS, gNVM_MirroredInRam_c);

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/


/* define the default reports */
tsReports asDefaultReports[NUMBER_OF_REPORTS] = \
{\
    {GENERAL_CLUSTER_ID_ONOFF, {0, E_ZCL_BOOL, E_CLD_ONOFF_ATTR_ID_ONOFF, MIN_REPORT_INTERVAL,MAX_REPORT_INTERVAL,0,{0}}},
	/*Report for temperature added*/
	{MEASUREMENT_AND_SENSING_CLUSTER_ID_TEMPERATURE_MEASUREMENT, {0, E_ZCL_INT16, E_CLD_TEMPMEAS_ATTR_ID_MEASURED_VALUE, MIN_REPORT_INTERVAL,MAX_REPORT_INTERVAL,0,{0}}}
};


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/


/****************************************************************************
 *
 * NAME: eRestoreReports
 *
 * DESCRIPTION:
 * Loads the reporting information from the EEPROM/PDM
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC PDM_teStatus eRestoreReports( void )
{
    /* Restore any report data that is previously saved to flash */
    uint16 u16ByteRead;
    PDM_teStatus eStatusReportReload = PDM_eReadDataFromRecord(PDM_ID_APP_REPORTS,
                                                              asSavedReports,
                                                              sizeof(asSavedReports),
                                                              &u16ByteRead);

    DBG_vPrintf(TRACE_REPORT,"eStatusReportReload = %d\r\n", eStatusReportReload);
    /* Restore any application data previously saved to flash */

    return  (eStatusReportReload);
}

/****************************************************************************
 *
 * NAME: vMakeSupportedAttributesReportable
 *
 * DESCRIPTION:
 * Makes the attributes reportable for On Off and Level control
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vMakeSupportedAttributesReportable(void)
{
    uint16 u16AttributeEnum;
    uint16 u16ClusterId;
    int i;


    tsZCL_AttributeReportingConfigurationRecord*    psAttributeReportingConfigurationRecord;

    DBG_vPrintf(TRACE_REPORT, "MAKE Reportable ep %d\r\n", ROUTER_APPLICATION_ENDPOINT);

    //for(i=0; i<NUMBER_OF_REPORTS; i++)
    {
        u16AttributeEnum = asSavedReports[0].sAttributeReportingConfigurationRecord.u16AttributeEnum;
        u16ClusterId = asSavedReports[0].u16ClusterID;
        psAttributeReportingConfigurationRecord = &(asSavedReports[0].sAttributeReportingConfigurationRecord);
        DBG_vPrintf(TRACE_REPORT, "Cluster %04x Attribute %04x Min %d Max %d IntV %d Direct %d Change %d\r\n",
                u16ClusterId,
                u16AttributeEnum,
                asSavedReports[0].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
                asSavedReports[0].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
                asSavedReports[0].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
                asSavedReports[0].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
                asSavedReports[0].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zint8ReportableChange);
        eZCL_SetReportableFlag( ROUTER_APPLICATION_ENDPOINT, u16ClusterId, TRUE, FALSE, u16AttributeEnum);
        eZCL_CreateLocalReport( ROUTER_APPLICATION_ENDPOINT, u16ClusterId, 0, TRUE, psAttributeReportingConfigurationRecord);
        u16AttributeEnum = asSavedReports[1].sAttributeReportingConfigurationRecord.u16AttributeEnum;
                u16ClusterId = asSavedReports[1].u16ClusterID;
                psAttributeReportingConfigurationRecord = &(asSavedReports[1].sAttributeReportingConfigurationRecord);
                DBG_vPrintf(TRACE_REPORT, "Cluster %04x Attribute %04x Min %d Max %d IntV %d Direct %d Change %d\r\n",
                        u16ClusterId,
                        u16AttributeEnum,
                        asSavedReports[1].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
                        asSavedReports[1].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
                        asSavedReports[1].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
                        asSavedReports[1].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
                        asSavedReports[1].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zint8ReportableChange);
                eZCL_SetReportableFlag( ROUTER_TEMP_APPLICATION_ENDPOINT, u16ClusterId, TRUE, FALSE, u16AttributeEnum);
                eZCL_CreateLocalReport( ROUTER_TEMP_APPLICATION_ENDPOINT, u16ClusterId, 0, TRUE, psAttributeReportingConfigurationRecord);
    }
}

/****************************************************************************
 *
 * NAME: vLoadDefaultConfigForReportable
 *
 * DESCRIPTION:
 * Loads a default configuration
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/

PUBLIC void vLoadDefaultConfigForReportable(void)
{
    FLib_MemSet(asSavedReports, 0 ,sizeof(asSavedReports));
    int i;
    for (i=0; i<NUMBER_OF_REPORTS; i++)
    {
        asSavedReports[i] = asDefaultReports[i];
    }

#if TRACE_REPORT

    DBG_vPrintf(TRACE_REPORT,"\r\nLoaded Defaults Records \r\n");
    for(i=0; i <NUMBER_OF_REPORTS; i++)
    {
        DBG_vPrintf(TRACE_REPORT,"Cluster %04x Type %d Attr %04x Min %d Max %d IntV %d Direct %d Change %d\r\n",
                asSavedReports[i].u16                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          ID,
                asSavedReports[i].sAttributeReportingConfigurationRecord.eAttributeDataType,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16AttributeEnum,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
                asSavedReports[i].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
                asSavedReports[i].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zuint8ReportableChange);
    }
#endif


    /*Save this Records*/
    PDM_eSaveRecordData(PDM_ID_APP_REPORTS,
                        asSavedReports,
                        sizeof(asSavedReports));
}


/****************************************************************************
 *
 * NAME: vSaveReportableRecord
 *
 * DESCRIPTION:
 * Loads a default configuration
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vSaveReportableRecord(  uint16 u16ClusterID,
                                    tsZCL_AttributeReportingConfigurationRecord* psAttributeReportingConfigurationRecord)
{
    int iIndex;

    if (u16ClusterID == GENERAL_CLUSTER_ID_ONOFF)
    {
        iIndex = REPORT_ONOFF_SLOT;
        DBG_vPrintf(TRACE_REPORT, "Save to report %d\r\n", iIndex);

        /*For CurrentLevel attribute in LevelControl Cluster*/
        asSavedReports[iIndex].u16ClusterID=u16ClusterID;
        FLib_MemCpy( &(asSavedReports[iIndex].sAttributeReportingConfigurationRecord),
                psAttributeReportingConfigurationRecord,
                sizeof(tsZCL_AttributeReportingConfigurationRecord) );

        DBG_vPrintf(TRACE_REPORT,"Cluster %04x Type %d Attrib %04x Min %d Max %d IntV %d Direction %d Change %d\r\n",
                asSavedReports[iIndex].u16ClusterID,
                asSavedReports[iIndex].sAttributeReportingConfigurationRecord.eAttributeDataType,
                asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16AttributeEnum,
                asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
                asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
                asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
                asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
                asSavedReports[iIndex].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zuint8ReportableChange );

        /*Save this Records*/
        PDM_eSaveRecordData(PDM_ID_APP_REPORTS,
                            asSavedReports,
                            sizeof(asSavedReports));
    }
    if (u16ClusterID == MEASUREMENT_AND_SENSING_CLUSTER_ID_TEMPERATURE_MEASUREMENT)
        {
            iIndex = REPORT_TEMP_SLOT;
            DBG_vPrintf(TRACE_REPORT, "Save to report %d\r\n", iIndex);

            /*For CurrentLevel attribute in LevelControl Cluster*/
            asSavedReports[iIndex].u16ClusterID=u16ClusterID;
            FLib_MemCpy( &(asSavedReports[iIndex].sAttributeReportingConfigurationRecord),
                    psAttributeReportingConfigurationRecord,
                    sizeof(tsZCL_AttributeReportingConfigurationRecord) );

            DBG_vPrintf(TRACE_REPORT,"Cluster %04x Type %d Attrib %04x Min %d Max %d IntV %d Direction %d Change %d\r\n",
                    asSavedReports[iIndex].u16ClusterID,
                    asSavedReports[iIndex].sAttributeReportingConfigurationRecord.eAttributeDataType,
                    asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16AttributeEnum,
                    asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
                    asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
                    asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
                    asSavedReports[iIndex].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
                    asSavedReports[iIndex].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zuint8ReportableChange );

            /*Save this Records*/
            PDM_eSaveRecordData(PDM_ID_APP_REPORTS,
                                asSavedReports,
                                sizeof(asSavedReports));
        }



}

PRIVATE uint8 u8GetRecordIndex( uint16 u16ClusterID,
		                uint16 u16AttributeEnum)
{
	uint8 u8Index = 0xFF;

    if (u16ClusterID == GENERAL_CLUSTER_ID_ONOFF)
    {
        u8Index = REPORT_ONOFF_SLOT;
    }
    else  if (u16ClusterID == MEASUREMENT_AND_SENSING_CLUSTER_ID_TEMPERATURE_MEASUREMENT)
    {
    	u8Index = REPORT_TEMP_SLOT;
    }
    else
    {
    	u8Index = 0xFF;
    }

	return u8Index;
}

PUBLIC void vRestoreDefaultRecord(  uint8 u8EndPointID,
				    uint16 u16ClusterID,
				    tsZCL_AttributeReportingConfigurationRecord* psAttributeReportingConfigurationRecord)
{
    uint8 u8Index = u8GetRecordIndex(u16ClusterID,psAttributeReportingConfigurationRecord->u16AttributeEnum);

    if(u8Index == 0xFF)
    	return;

    eZCL_CreateLocalReport( u8EndPointID, u16ClusterID, 0, TRUE, &(asDefaultReports[u8Index].sAttributeReportingConfigurationRecord));

    DBG_vPrintf(TRACE_REPORT, "Save to report %d\n", u8Index);

    FLib_MemCpy( &(asSavedReports[u8Index].sAttributeReportingConfigurationRecord),
    		&(asDefaultReports[u8Index].sAttributeReportingConfigurationRecord),
            sizeof(tsZCL_AttributeReportingConfigurationRecord) );

    DBG_vPrintf(TRACE_REPORT,"Cluster %04x Type %d Attrib %04x Min %d Max %d IntV %d Direction %d Change %d\n",
            asSavedReports[u8Index].u16ClusterID,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.eAttributeDataType,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u16AttributeEnum,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u16MinimumReportingInterval,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u16MaximumReportingInterval,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u16TimeoutPeriodField,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.u8DirectionIsReceived,
            asSavedReports[u8Index].sAttributeReportingConfigurationRecord.uAttributeReportableChange.zuint8ReportableChange    );

    /*Save this Records*/
    PDM_eSaveRecordData(PDM_ID_APP_REPORTS,
                        asSavedReports,
                        sizeof(asSavedReports));

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
