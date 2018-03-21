/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.iterate.conf,v 5.5.2.1 2003/07/02 17:36:07 hardaker Exp $
 */
#ifndef IEEE802DOT11_TABLE_H
#define IEEE802DOT11_TABLE_H

/*
 * function declarations 
 */
void            init_ieee802dot11_table(void);
void            initialize_table_dot11WEPKeyMappingsTable(void);
Netsnmp_Node_Handler dot11WEPKeyMappingsTable_handler;

Netsnmp_First_Data_Point dot11WEPKeyMappingsTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11WEPKeyMappingsTable_get_next_data_point;
void            initialize_table_dot11PhyFHSSTable(void);
Netsnmp_Node_Handler dot11PhyFHSSTable_handler;

Netsnmp_First_Data_Point dot11PhyFHSSTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11PhyFHSSTable_get_next_data_point;
void            initialize_table_dot11PhyDSSSTable(void);
Netsnmp_Node_Handler dot11PhyDSSSTable_handler;

Netsnmp_First_Data_Point dot11PhyDSSSTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11PhyDSSSTable_get_next_data_point;
void            initialize_table_dot11ResourceInfoTable(void);
Netsnmp_Node_Handler dot11ResourceInfoTable_handler;

Netsnmp_First_Data_Point dot11ResourceInfoTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11ResourceInfoTable_get_next_data_point;
void            initialize_table_dot11PhyOperationTable(void);
Netsnmp_Node_Handler dot11PhyOperationTable_handler;

Netsnmp_First_Data_Point dot11PhyOperationTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11PhyOperationTable_get_next_data_point;
void            initialize_table_dot11StationConfigTable(void);
Netsnmp_Node_Handler dot11StationConfigTable_handler;

Netsnmp_First_Data_Point dot11StationConfigTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11StationConfigTable_get_next_data_point;
void            initialize_table_dot11PhyAntennaTable(void);
Netsnmp_Node_Handler dot11PhyAntennaTable_handler;

Netsnmp_First_Data_Point dot11PhyAntennaTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11PhyAntennaTable_get_next_data_point;
void            initialize_table_dot11CountersTable(void);
Netsnmp_Node_Handler dot11CountersTable_handler;

Netsnmp_First_Data_Point dot11CountersTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11CountersTable_get_next_data_point;
void            initialize_table_dot11AntennasListTable(void);
Netsnmp_Node_Handler dot11AntennasListTable_handler;

Netsnmp_First_Data_Point dot11AntennasListTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11AntennasListTable_get_next_data_point;
void            initialize_table_dot11PhyTxPowerTable(void);
Netsnmp_Node_Handler dot11PhyTxPowerTable_handler;

Netsnmp_First_Data_Point dot11PhyTxPowerTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11PhyTxPowerTable_get_next_data_point;
void            initialize_table_dot11GroupAddressesTable(void);
Netsnmp_Node_Handler dot11GroupAddressesTable_handler;

Netsnmp_First_Data_Point dot11GroupAddressesTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11GroupAddressesTable_get_next_data_point;
void            initialize_table_dot11SupportedDataRatesRxTable(void);
Netsnmp_Node_Handler dot11SupportedDataRatesRxTable_handler;

Netsnmp_First_Data_Point
    dot11SupportedDataRatesRxTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11SupportedDataRatesRxTable_get_next_data_point;
void            initialize_table_dot11AuthenticationAlgorithmsTable(void);
Netsnmp_Node_Handler dot11AuthenticationAlgorithmsTable_handler;

Netsnmp_First_Data_Point
    dot11AuthenticationAlgorithmsTable_get_first_data_point;
Netsnmp_Next_Data_Point
    dot11AuthenticationAlgorithmsTable_get_next_data_point;
void            initialize_table_dot11PhyIRTable(void);
Netsnmp_Node_Handler dot11PhyIRTable_handler;

Netsnmp_First_Data_Point dot11PhyIRTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11PhyIRTable_get_next_data_point;
void            initialize_table_dot11PrivacyTable(void);
Netsnmp_Node_Handler dot11PrivacyTable_handler;

Netsnmp_First_Data_Point dot11PrivacyTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11PrivacyTable_get_next_data_point;
void            initialize_table_dot11WEPDefaultKeysTable(void);
Netsnmp_Node_Handler dot11WEPDefaultKeysTable_handler;

Netsnmp_First_Data_Point dot11WEPDefaultKeysTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11WEPDefaultKeysTable_get_next_data_point;
void            initialize_table_dot11SupportedDataRatesTxTable(void);
Netsnmp_Node_Handler dot11SupportedDataRatesTxTable_handler;

Netsnmp_First_Data_Point
    dot11SupportedDataRatesTxTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11SupportedDataRatesTxTable_get_next_data_point;
void            initialize_table_dot11OperationTable(void);
Netsnmp_Node_Handler dot11OperationTable_handler;

Netsnmp_First_Data_Point dot11OperationTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11OperationTable_get_next_data_point;
void            initialize_table_dot11RegDomainsSupportedTable(void);
Netsnmp_Node_Handler dot11RegDomainsSupportedTable_handler;

Netsnmp_First_Data_Point
    dot11RegDomainsSupportedTable_get_first_data_point;
Netsnmp_Next_Data_Point dot11RegDomainsSupportedTable_get_next_data_point;

#ifndef MY_DEBUG
#define MY_DEBUG
#define MYSTART(s)      printf("\n***** start  ***** %s %s %d\n",s,__FILE__,__LINE__);
#define MYEND(s)        printf("\n*****  end   ***** %s %s %d\n",s,__FILE__,__LINE__);
#define MYMARK(s)       printf("\n***** mark   ***** %s %s %d\n",s,__FILE__,__LINE__);
#define MYIN()          printf("\n***** in     ***** %s %s %d\n",__FUNCTION__,__FILE__,__LINE__);
#define MYOUT()          printf("\n***** out   ***** %s %s %d\n",__FUNCTION__,__FILE__,__LINE__);
#endif
/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11WEPKEYMAPPINGSTABLE_IFINDEX_MIN	1
#define DOT11WEPKEYMAPPINGSTABLE_IFINDEX_MAX	2
#define DOT11WEPKEYMAPPINGSTABLE_INDEX_MIN	1
#define DOT11WEPKEYMAPPINGSTABLE_INDEX_MAX	3
/*
 * column number definitions for table dot11WEPKeyMappingsTable 
 */
#define COLUMN_DOT11WEPKEYMAPPINGINDEX		1
#define COLUMN_DOT11WEPKEYMAPPINGADDRESS		2
#define COLUMN_DOT11WEPKEYMAPPINGWEPON		3
#define COLUMN_DOT11WEPKEYMAPPINGVALUE		4
#define COLUMN_DOT11WEPKEYMAPPINGSTATUS		5

/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11PHYFHSSTABLE_IFINDEX_MIN	1
#define DOT11PHYFHSSTABLE_IFINDEX_MAX	2
/*
 * column number definitions for table dot11PhyFHSSTable 
 */
#define COLUMN_DOT11HOPTIME		1
#define COLUMN_DOT11CURRENTCHANNELNUMBER		2
#define COLUMN_DOT11MAXDWELLTIME		3
#define COLUMN_DOT11CURRENTDWELLTIME		4
#define COLUMN_DOT11CURRENTSET		5
#define COLUMN_DOT11CURRENTPATTERN		6
#define COLUMN_DOT11CURRENTINDEX		7

/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11PHYDSSSTABLE_IFINDEX_MIN	1
#define DOT11PHYDSSSTABLE_IFINDEX_MAX	2
/*
 * column number definitions for table dot11PhyDSSSTable 
 */
#define COLUMN_DOT11CURRENTCHANNEL		1
#define COLUMN_DOT11CCAMODESUPPORTED		2
#define COLUMN_DOT11CURRENTCCAMODE		3
#define COLUMN_DOT11EDTHRESHOLD		4

/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11RESOURCEINFOTABLE_IFINDEX_MIN	1
#define DOT11RESOURCEINFOTABLE_IFINDEX_MAX	2
/*
 * column number definitions for table dot11ResourceInfoTable 
 */
#define COLUMN_DOT11MANUFACTUREROUI		1
#define COLUMN_DOT11MANUFACTURERNAME		2
#define COLUMN_DOT11MANUFACTURERPRODUCTNAME		3
#define COLUMN_DOT11MANUFACTURERPRODUCTVERSION		4
/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11PHYOPERATIONTABLE_IFINDEX_MIN	1
#define DOT11PHYOPERATIONTABLE_IFINDEX_MAX	2
/*
 * column number definitions for table dot11PhyOperationTable 
 */
#define COLUMN_DOT11PHYTYPE		1
#define COLUMN_DOT11CURRENTREGDOMAIN		2
#define COLUMN_DOT11TEMPTYPE		3
/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11STATIONCONFIGTABLE_IFINDEX_MIN	1
#define DOT11STATIONCONFIGTABLE_IFINDEX_MAX	2
/*
 * column number definitions for table dot11StationConfigTable 
 */
#define COLUMN_DOT11STATIONID		1
#define COLUMN_DOT11MEDIUMOCCUPANCYLIMIT		2
#define COLUMN_DOT11CFPOLLABLE		3
#define COLUMN_DOT11CFPPERIOD		4
#define COLUMN_DOT11CFPMAXDURATION		5
#define COLUMN_DOT11AUTHENTICATIONRESPONSETIMEOUT		6
#define COLUMN_DOT11PRIVACYOPTIONIMPLEMENTED		7
#define COLUMN_DOT11POWERMANAGEMENTMODE		8
#define COLUMN_DOT11DESIREDSSID		9
#define COLUMN_DOT11DESIREDBSSTYPE		10
#define COLUMN_DOT11OPERATIONALRATESET		11
#define COLUMN_DOT11BEACONPERIOD		12
#define COLUMN_DOT11DTIMPERIOD		13
#define COLUMN_DOT11ASSOCIATIONRESPONSETIMEOUT		14
#define COLUMN_DOT11DISASSOCIATEREASON		15
#define COLUMN_DOT11DISASSOCIATESTATION		16
#define COLUMN_DOT11DEAUTHENTICATEREASON		17
#define COLUMN_DOT11DEAUTHENTICATESTATION		18
#define COLUMN_DOT11AUTHENTICATEFAILSTATUS		19
#define COLUMN_DOT11AUTHENTICATEFAILSTATION		20
/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11PHYANTENNATABLE_IFINDEX_MIN	1
#define DOT11PHYANTENNATABLE_IFINDEX_MAX	2
/*
 * column number definitions for table dot11PhyAntennaTable 
 */
#define COLUMN_DOT11CURRENTTXANTENNA		1
#define COLUMN_DOT11DIVERSITYSUPPORT		2
#define COLUMN_DOT11CURRENTRXANTENNA		3


/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11COUNTERSTABLE_IFINDEX_MIN		1
#define DOT11COUNTERSTABLE_IFINDEX_MAX	2
/*
 * column number definitions for table dot11CountersTable 
 */
#define COLUMN_DOT11TRANSMITTEDFRAGMENTCOUNT		1
#define COLUMN_DOT11MULTICASTTRANSMITTEDFRAMECOUNT		2
#define COLUMN_DOT11FAILEDCOUNT		3
#define COLUMN_DOT11RETRYCOUNT		4
#define COLUMN_DOT11MULTIPLERETRYCOUNT		5
#define COLUMN_DOT11FRAMEDUPLICATECOUNT		6
#define COLUMN_DOT11RTSSUCCESSCOUNT		7
#define COLUMN_DOT11RTSFAILURECOUNT		8
#define COLUMN_DOT11ACKFAILURECOUNT		9
#define COLUMN_DOT11RECEIVEDFRAGMENTCOUNT		10
#define COLUMN_DOT11MULTICASTRECEIVEDFRAMECOUNT		11
#define COLUMN_DOT11FCSERRORCOUNT		12
#define COLUMN_DOT11TRANSMITTEDFRAMECOUNT		13
#define COLUMN_DOT11WEPUNDECRYPTABLECOUNT		14



/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11ANTENNASLISTTABLE_IFINDEX_MIN 1
#define DOT11ANTENNASLISTTABLE_IFINDEX_MAX	2
#define DOT11ANTENNASLISTTABLE_INDEX_MIN 	1
#define DOT11ANTENNASLISTTABLE_INDEX_MAX	2
/*
 * column number definitions for table dot11AntennasListTable 
 */
#define COLUMN_DOT11ANTENNALISTINDEX		1
#define COLUMN_DOT11SUPPORTEDTXANTENNA		2
#define COLUMN_DOT11SUPPORTEDRXANTENNA		3
#define COLUMN_DOT11DIVERSITYSELECTIONRX		4


/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11PHYTXPOWERTABLE_IFINDEX_MIN	1
#define DOT11PHYTXPOWERTABLE_IFINDEX_MAX	2
/*
 * column number definitions for table dot11PhyTxPowerTable 
 */
#define COLUMN_DOT11NUMBERSUPPORTEDPOWERLEVELS		1
#define COLUMN_DOT11TXPOWERLEVEL1		2
#define COLUMN_DOT11TXPOWERLEVEL2		3
#define COLUMN_DOT11TXPOWERLEVEL3		4
#define COLUMN_DOT11TXPOWERLEVEL4		5
#define COLUMN_DOT11TXPOWERLEVEL5		6
#define COLUMN_DOT11TXPOWERLEVEL6		7
#define COLUMN_DOT11TXPOWERLEVEL7		8
#define COLUMN_DOT11TXPOWERLEVEL8		9
#define COLUMN_DOT11CURRENTTXPOWERLEVEL		10

/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11GROUPADDRESSESTABLE_IFINDEX_MIN	1
#define DOT11GROUPADDRESSESTABLE_IFINDEX_MAX	2
#define DOT11GROUPADDRESSESTABLE_INDEX_MIN	1
#define DOT11GROUPADDRESSESTABLE_INDEX_MAX	3
/*
 * column number definitions for table dot11GroupAddressesTable 
 */
#define COLUMN_DOT11GROUPADDRESSESINDEX		1
#define COLUMN_DOT11ADDRESS		2
#define COLUMN_DOT11GROUPADDRESSESSTATUS		3


/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11SUPPORTEDDATARATESRXTABLE_IFINDEX_MIN	1
#define DOT11SUPPORTEDDATARATESRXTABLE_IFINDEX_MAX	2
#define DOT11SUPPORTEDDATARATESRXTABLE_INDEX_MIN	1
#define DOT11SUPPORTEDDATARATESRXTABLE_INDEX_MAX	4
/*
 * column number definitions for table dot11SupportedDataRatesRxTable 
 */
#define COLUMN_DOT11SUPPORTEDDATARATESRXINDEX		1
#define COLUMN_DOT11SUPPORTEDDATARATESRXVALUE		2


/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11AUTHENTICATIONALGORITHMSTABLE_IFINDEX_MIN	1
#define DOT11AUTHENTICATIONALGORITHMSTABLE_IFINDEX_MAX	2
#define DOT11AUTHENTICATIONALGORITHMSTABLE_INDEX_MIN	1
#define DOT11AUTHENTICATIONALGORITHMSTABLE_INDEX_MAX	2
/*
 * column number definitions for table dot11AuthenticationAlgorithmsTable 
 */
#define COLUMN_DOT11AUTHENTICATIONALGORITHMSINDEX		1
#define COLUMN_DOT11AUTHENTICATIONALGORITHM		2
#define COLUMN_DOT11AUTHENTICATIONALGORITHMSENABLE		3


/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11PHYIRTABLE_IFINDEX_MIN	1
#define DOT11PHYIRTABLE_IFINDEX_MAX	2
/*
 * column number definitions for table dot11PhyIRTable 
 */
#define COLUMN_DOT11CCAWATCHDOGTIMERMAX		1
#define COLUMN_DOT11CCAWATCHDOGCOUNTMAX		2
#define COLUMN_DOT11CCAWATCHDOGTIMERMIN		3
#define COLUMN_DOT11CCAWATCHDOGCOUNTMIN		4


/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11PRIVACYTABLE_IFINDEX_MIN	1
#define DOT11PRIVACYTABLE_IFINDEX_MAX	2
/*
 * column number definitions for table dot11PrivacyTable 
 */
#define COLUMN_DOT11PRIVACYINVOKED		1
#define COLUMN_DOT11WEPDEFAULTKEYID		2
#define COLUMN_DOT11WEPKEYMAPPINGLENGTH		3
#define COLUMN_DOT11EXCLUDEUNENCRYPTED		4
#define COLUMN_DOT11WEPICVERRORCOUNT		5
#define COLUMN_DOT11WEPEXCLUDEDCOUNT		6


/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11WEPDEFAULTKEYSTABLE_IFINDEX_MIN	1
#define DOT11WEPDEFAULTKEYSTABLE_IFINDEX_MAX	2
#define DOT11WEPDEFAULTKEYSTABLE_INDEX_MIN	1
#define DOT11WEPDEFAULTKEYSTABLE_INDEX_MAX	4
/*
 * column number definitions for table dot11WEPDefaultKeysTable 
 */
#define COLUMN_DOT11WEPDEFAULTKEYINDEX		1
#define COLUMN_DOT11WEPDEFAULTKEYVALUE		2


/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11SUPPORTEDDATARATESTXTABLE_IFINDEX_MIN	1
#define DOT11SUPPORTEDDATARATESTXTABLE_IFINDEX_MAX	2
#define DOT11SUPPORTEDDATARATESTXTABLE_INDEX_MIN	1
#define DOT11SUPPORTEDDATARATESTXTABLE_INDEX_MAX	4
/*
 * column number definitions for table dot11SupportedDataRatesTxTable 
 */
#define COLUMN_DOT11SUPPORTEDDATARATESTXINDEX		1
#define COLUMN_DOT11SUPPORTEDDATARATESTXVALUE		2


/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11OPERATIONTABLE_IFINDEX_MIN	1
#define DOT11OPERATIONTABLE_IFINDEX_MAX	2
/*
 * column number definitions for table dot11OperationTable 
 */
#define COLUMN_DOT11MACADDRESS		1
#define COLUMN_DOT11RTSTHRESHOLD		2
#define COLUMN_DOT11SHORTRETRYLIMIT		3
#define COLUMN_DOT11LONGRETRYLIMIT		4
#define COLUMN_DOT11FRAGMENTATIONTHRESHOLD		5
#define COLUMN_DOT11MAXTRANSMITMSDULIFETIME		6
#define COLUMN_DOT11MAXRECEIVELIFETIME		7
#define COLUMN_DOT11MANUFACTURERID		8
#define COLUMN_DOT11PRODUCTID		9


/*json
*defien ifIndex range and  index range ,if no specify, default to 3
*/
#define DOT11REGDOMAINSSUPPORTEDTABLE_IFINDEX_MIN	1
#define DOT11REGDOMAINSSUPPORTEDTABLE_IFINDEX_MAX	2
#define DOT11REGDOMAINSSUPPORTEDTABLE_INDEX_MIN 	1
#define DOT11REGDOMAINSSUPPORTEDTABLE_INDEX_MAX	4
/*
 * column number definitions for table dot11RegDomainsSupportedTable 
 */
#define COLUMN_DOT11REGDOMAINSSUPPORTINDEX		1
#define COLUMN_DOT11REGDOMAINSSUPPORTVALUE		2
#endif                          /* IEEE802DOT11_TABLE_H */
