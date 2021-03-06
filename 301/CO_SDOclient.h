/**
 * CANopen Service Data Object - client protocol.
 *
 * @file        CO_SDOclient.h
 * @ingroup     CO_SDOclient
 * @author      Janez Paternoster
 * @author      Matej Severkar
 * @copyright   2004 - 2020 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef CO_SDO_CLIENT_H
#define CO_SDO_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CO_SDOclient SDO client
 * @ingroup CO_CANopen_301
 * @{
 *
 * CANopen Service Data Object - client protocol (master functionality).
 *
 * @see @ref CO_SDOserver
 */


/**
 * Return values of SDO client functions.
 */
typedef enum{
    /** Transmit buffer is full. Waiting */
    CO_SDOcli_transmittBufferFull       = 4,
    /** Block download is in progress. Sending train of messages */
    CO_SDOcli_blockDownldInProgress     = 3,
    /** Block upload in progress. Receiving train of messages */
    CO_SDOcli_blockUploadInProgress     = 2,
    /** Waiting server response */
    CO_SDOcli_waitingServerResponse     = 1,
    /** Success, end of communication */
    CO_SDOcli_ok_communicationEnd       = 0,
    /** Error in arguments */
    CO_SDOcli_wrongArguments            = -2,
    /** Communication ended with client abort */
    CO_SDOcli_endedWithClientAbort      = -9,
    /** Communication ended with server abort */
    CO_SDOcli_endedWithServerAbort      = -10,
    /** Communication ended with timeout */
    CO_SDOcli_endedWithTimeout          = -11
}CO_SDOclient_return_t;


/**
 * SDO Client Parameter. The same as record from Object dictionary
 * (index 0x1280+).
 */
typedef struct{
    /** Equal to 3 */
    uint8_t             maxSubIndex;
    /** Communication object identifier for client transmission.
     * Meaning of the specific bits:
        - Bit 0...10: 11-bit CAN identifier.
        - Bit 11..30: reserved, set to 0.
        - Bit 31: if 1, SDO client object is not used. */
    uint32_t            COB_IDClientToServer;
    /** Communication object identifier for message received from server.
     * Meaning of the specific bits:
        - Bit 0...10: 11-bit CAN identifier.
        - Bit 11..30: reserved, set to 0.
        - Bit 31: if 1, SDO client object is not used. */
    uint32_t            COB_IDServerToClient;
    /** Node-ID of the SDO server */
    uint8_t             nodeIDOfTheSDOServer;
}CO_SDOclientPar_t;


/**
 * SDO client object
 */
typedef struct{
    /** From CO_SDOclient_init() */
    CO_SDOclientPar_t  *SDOClientPar;
    /** From CO_SDOclient_init() */
    CO_SDO_t           *SDO;
    /** Internal state of the SDO client */
    uint8_t             state;
    /** Pointer to data buffer supplied by user */
    uint8_t            *buffer;
    /** By download application indicates data size in buffer.
    By upload application indicates buffer size */
    uint32_t            bufferSize;
    /** Offset in buffer of next data segment being read/written */
    uint32_t            bufferOffset;
    /** Acknowledgement */
    uint32_t            bufferOffsetACK;
    /** data length to be uploaded in block transfer */
    uint32_t            dataSize;
    /** Data length transferred in block transfer */
    uint32_t            dataSizeTransfered;
    /** Maximum timeout time between request and response in microseconds. */
    uint32_t            SDOtimeoutTime_us;
    /** Half of maximum timeout time between request and response. */
    uint32_t            SDOtimeoutTimeHalf_us;
    /** Timeout timer for SDO communication */
    uint32_t            timeoutTimer;
    /** Timeout timer for SDO block transfer */
    uint32_t            timeoutTimerBLOCK;
    /** Index of current object in Object Dictionary */
    uint16_t            index;
    /** Subindex of current object in Object Dictionary */
    uint8_t             subIndex;
    /** From CO_SDOclient_init() */
    CO_CANmodule_t     *CANdevRx;
    /** From CO_SDOclient_init() */
    uint16_t            CANdevRxIdx;
    /** Indicates, if new SDO message received from CAN bus.
    It is not cleared, until received message is completely processed. */
    volatile void      *CANrxNew;
    /** 8 data bytes of the received message */
    uint8_t             CANrxData[8];
#if ((CO_CONFIG_SDO_CLI) & CO_CONFIG_FLAG_CALLBACK_PRE) || defined CO_DOXYGEN
    /** From CO_SDOclient_initCallbackPre() or NULL */
    void              (*pFunctSignal)(void *object);
    /** From CO_SDOclient_initCallbackPre() or NULL */
    void               *functSignalObject;
#endif
    /** From CO_SDOclient_init() */
    CO_CANmodule_t     *CANdevTx;
    /** CAN transmit buffer inside CANdevTx for CAN tx message */
    CO_CANtx_t         *CANtxBuff;
    /** From CO_SDOclient_init() */
    uint16_t            CANdevTxIdx;
    /** Toggle bit toggled with each subsequent in segmented transfer */
    uint8_t             toggle;
    /** Server threshold for switch back to segmented transfer, if data size is
     * small. Set in CO_SDOclient_init(). Can be changed by application.
     * 0 Disables switching. */
    uint8_t             pst;
    /** Maximum number of segments in one block. Set in CO_SDOclient_init(). Can
    be changed by application to 2 .. 127. */
    uint8_t             block_size_max;
    /** Last sector number */
    uint8_t             block_seqno;
    /** Block size in current transfer */
    uint8_t             block_blksize;
    /** Number of bytes in last segment that do not contain data */
    uint8_t             block_noData;
    /** Server CRC support in block transfer */
    uint8_t             crcEnabled;
    /** Previous value of the COB_IDClientToServer */
    uint32_t            COB_IDClientToServerPrev;
    /** Previous value of the COB_IDServerToClient */
    uint32_t            COB_IDServerToClientPrev;

}CO_SDOclient_t;


/**
 * Initialize SDO client object.
 *
 * Function must be called in the communication reset section.
 *
 * @param SDO_C This object will be initialized.
 * @param SDO SDO server object. It is used in case, if client is accessing
 * object dictionary from its own device. If NULL, it will be ignored.
 * @param SDOClientPar Pointer to _SDO Client Parameter_ record from Object
 * dictionary (index 0x1280+). Will be written.
 * @param CANdevRx CAN device for SDO client reception.
 * @param CANdevRxIdx Index of receive buffer in the above CAN device.
 * @param CANdevTx CAN device for SDO client transmission.
 * @param CANdevTxIdx Index of transmit buffer in the above CAN device.
 *
 * @return #CO_ReturnError_t: CO_ERROR_NO or CO_ERROR_ILLEGAL_ARGUMENT.
 */
CO_ReturnError_t CO_SDOclient_init(
        CO_SDOclient_t         *SDO_C,
        CO_SDO_t               *SDO,
        CO_SDOclientPar_t      *SDOClientPar,
        CO_CANmodule_t         *CANdevRx,
        uint16_t                CANdevRxIdx,
        CO_CANmodule_t         *CANdevTx,
        uint16_t                CANdevTxIdx);


#if ((CO_CONFIG_SDO_CLI) & CO_CONFIG_FLAG_CALLBACK_PRE) || defined CO_DOXYGEN
/**
 * Initialize SDOclient callback function.
 *
 * Function initializes optional callback function, which should immediately
 * start processing of CO_SDOclientDownload() or CO_SDOclientUpload() function.
 * Callback is called after SDOclient message is received from the CAN bus or
 * when new call without delay is necessary (exchange data with own SDO server
 * or SDO block transfer is in progress).
 *
 * @param SDOclient This object.
 * @param object Pointer to object, which will be passed to pFunctSignal(). Can
 * be NULL.
 * @param pFunctSignal Pointer to the callback function. Not called if NULL.
 */
void CO_SDOclient_initCallbackPre(
        CO_SDOclient_t         *SDOclient,
        void                   *object,
        void                  (*pFunctSignal)(void *object));
#endif


/**
 * Setup SDO client object.
 *
 * Function must be called before new SDO communication. If previous SDO
 * communication was with the same node, function does not need to be called.
 *
 * @remark If configuring SDO client from network is required, this function
 * should be set as callback for the corresponding SDO client parameter OD
 * entry.
 *
 * @param SDO_C This object.
 * @param COB_IDClientToServer See CO_SDOclientPar_t. If zero, then
 * nodeIDOfTheSDOServer is used with default COB-ID.
 * @param COB_IDServerToClient See CO_SDOclientPar_t. If zero, then
 * nodeIDOfTheSDOServer is used with default COB-ID.
 * @param nodeIDOfTheSDOServer Node-ID of the SDO server. If zero, SDO client
 * object is not used. If it is the same as node-ID of this node, then data will
 * be exchanged with this node (without CAN communication).
 *
 * @return #CO_SDOclient_return_t
 */
CO_SDOclient_return_t CO_SDOclient_setup(
        CO_SDOclient_t         *SDO_C,
        uint32_t                COB_IDClientToServer,
        uint32_t                COB_IDServerToClient,
        uint8_t                 nodeIDOfTheSDOServer);


/**
 * Initiate SDO download communication.
 *
 * Function initiates SDO download communication with server specified in
 * CO_SDOclient_init() function. Data will be written to remote node.
 * Function is non-blocking.
 *
 * @param SDO_C This object.
 * @param index Index of object in object dictionary in remote node.
 * @param subIndex Subindex of object in object dictionary in remote node.
 * @param dataTx Pointer to data to be written. Data must be valid until end
 * of communication. Note that data are aligned in little-endian
 * format, because CANopen itself uses little-endian. Take care,
 * when using processors with big-endian.
 * @param dataSize Size of data in dataTx.
 * @param SDOtimeoutTime_ms Timeout time for SDO communication in milliseconds.
 * @param blockEnable Try to initiate block transfer.
 * @param [out] timerNext_us info to OS - see CO_process(). Ignored if NULL.
 *
 * @return #CO_SDOclient_return_t
 */
CO_SDOclient_return_t CO_SDOclientDownloadInitiate(
        CO_SDOclient_t         *SDO_C,
        uint16_t                index,
        uint8_t                 subIndex,
        uint8_t                *dataTx,
        uint32_t                dataSize,
        uint16_t                SDOtimeoutTime_ms,
        uint8_t                 blockEnable,
        uint32_t               *timerNext_us);


/**
 * Process SDO download communication.
 *
 * Function must be called cyclically until it returns <=0. It Proceeds SDO
 * download communication initiated with CO_SDOclientDownloadInitiate().
 * Function is non-blocking.
 *
 * @param SDO_C This object.
 * @param timeDifference_us Time difference from previous function call in
 * [microseconds].
 * @param [out] pSDOabortCode Pointer to external variable written by this
 * function in case of error in communication.
 * @param [out] timerNext_us info to OS - see CO_process(). Ignored if NULL.
 *
 * @return #CO_SDOclient_return_t
 */
CO_SDOclient_return_t CO_SDOclientDownload(
        CO_SDOclient_t         *SDO_C,
        uint32_t                timeDifference_us,
        uint32_t               *pSDOabortCode,
        uint32_t               *timerNext_us);


/**
 * Initiate SDO upload communication.
 *
 * Function initiates SDO upload communication with server specified in
 * CO_SDOclient_init() function. Data will be read from remote node.
 * Function is non-blocking.
 *
 * @param SDO_C This object.
 * @param index Index of object in object dictionary in remote node.
 * @param subIndex Subindex of object in object dictionary in remote node.
 * @param dataRx Pointer to data buffer, into which received data will be
 * written. Buffer must be valid until end of communication. Note that data are
 * aligned in little-endian format, because CANopen itself uses little-endian.
 * Take care, when using processors with big-endian.
 * @param dataRxSize Size of dataRx.
 * @param SDOtimeoutTime_ms Timeout time for SDO communication in milliseconds.
 * @param blockEnable Try to initiate block transfer.
 * @param [out] timerNext_us info to OS - see CO_process(). Ignored if NULL.
 *
 * @return #CO_SDOclient_return_t
 */
CO_SDOclient_return_t CO_SDOclientUploadInitiate(
        CO_SDOclient_t         *SDO_C,
        uint16_t                index,
        uint8_t                 subIndex,
        uint8_t                *dataRx,
        uint32_t                dataRxSize,
        uint16_t                SDOtimeoutTime_ms,
        uint8_t                 blockEnable,
        uint32_t               *timerNext_us);


/**
 * Process SDO upload communication.
 *
 * Function must be called cyclically until it returns <=0. It Proceeds SDO
 * upload communication initiated with CO_SDOclientUploadInitiate().
 * Function is non-blocking.
 *
 * @param SDO_C This object.
 * @param timeDifference_us Time difference from previous function call in
 * [microseconds].
 * @param [out] pDataSize pointer to external variable, where size of received
 * data will be written.
 * @param [out] pSDOabortCode Pointer to external variable written by this
 * function in case of error in communication.
 * @param [out] timerNext_us info to OS - see CO_process(). Ignored if NULL.
 *
 * @return #CO_SDOclient_return_t
 */
CO_SDOclient_return_t CO_SDOclientUpload(
        CO_SDOclient_t         *SDO_C,
        uint32_t                timeDifference_us,
        uint32_t               *pDataSize,
        uint32_t               *pSDOabortCode,
        uint32_t               *timerNext_us);


/**
 * Close SDO communication temporary.
 *
 * Function must be called after finish of each SDO client communication cycle.
 * It disables reception of SDO client CAN messages. It is necessary, because
 * CO_SDOclient_receive function may otherwise write into undefined SDO buffer.
 *
 * @param SDO_C This object.
 */
void CO_SDOclientClose(CO_SDOclient_t *SDO_C);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

/** @} */
#endif
