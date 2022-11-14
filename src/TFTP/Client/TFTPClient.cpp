//
// Created by kollins on 03/09/2022.
//

#include "TFTPClient.h"

TFTPClient::TFTPClient() {
    if (create_tftp_handler(&clientHandler) != TFTP_OK) {
        clientHandler = nullptr;
        throw "CLIENT HANDLER CREATION FAILED!";
    }

    tftpErrorCtx = nullptr;
    _tftpErrorCallback = nullptr;
    tftpFetchDataReceivedCtx = nullptr;
    _tftpFetchDataReceivedCallback = nullptr;

    register_tftp_error_callback(clientHandler, tftpErrorCbk, this);
    register_tftp_fetch_data_received_callback(clientHandler, tftpFetchDataReceivedCbk, this);
}

TFTPClient::~TFTPClient() {
    if (clientHandler != nullptr) {
        if (destroy_tftp_handler(&clientHandler) != TFTP_OK) {
            //TODO: inform error
        }
    }
}

TftpClientOperationResult TFTPClient::setConnection(
        const char *host,
        const int port
) {
    if (clientHandler == nullptr) {
        return TftpClientOperationResult::TFTP_CLIENT_ERROR;
    }

    TftpOperationResult result = set_connection(
            clientHandler,
            host,
            port
    );

    if (result != TFTP_OK) {
        return TftpClientOperationResult::TFTP_CLIENT_ERROR;
    }

    result = config_tftp(clientHandler);

    return result == TFTP_OK ?
           TftpClientOperationResult::TFTP_CLIENT_OK :
           TftpClientOperationResult::TFTP_CLIENT_ERROR;
}

TftpClientOperationResult TFTPClient::registerTftpErrorCallback(
        tftpErrorCallback callback,
        void *context)
{
    _tftpErrorCallback = callback;
    tftpErrorCtx = context;
    return TftpClientOperationResult::TFTP_CLIENT_OK;
}

TftpClientOperationResult TFTPClient::registerTftpFetchDataReceivedCallback(
        tftpfetchDataReceivedCallback callback,
        void *context)
{
    _tftpFetchDataReceivedCallback = callback;
    tftpFetchDataReceivedCtx = context;
    return TftpClientOperationResult::TFTP_CLIENT_OK;
}

TftpClientOperationResult TFTPClient::sendFile(
        const char *filename,
        FILE *fp
) {
    if (clientHandler == nullptr) {
        return TftpClientOperationResult::TFTP_CLIENT_ERROR;
    }
    return send_file(clientHandler, filename, fp) == TFTP_OK ?
           TftpClientOperationResult::TFTP_CLIENT_OK :
           TftpClientOperationResult::TFTP_CLIENT_ERROR;
}

TftpClientOperationResult TFTPClient::fetchFile(
        const char *filename,
        FILE *fp
) {
    if (clientHandler == nullptr) {
        return TftpClientOperationResult::TFTP_CLIENT_ERROR;
    }
    return fetch_file(clientHandler, filename, fp) == TFTP_OK ?
           TftpClientOperationResult::TFTP_CLIENT_OK :
           TftpClientOperationResult::TFTP_CLIENT_ERROR;
}

TftpOperationResult TFTPClient::tftpErrorCbk (
        short error_code,
        const char *error_message,
        void *context)
{
    if (context != NULL) {
        TFTPClient *client = (TFTPClient *) context;
        if (client->_tftpErrorCallback != nullptr) {
            std::string errorMessage(error_message);
            client->_tftpErrorCallback(error_code, errorMessage,
                                       client->tftpErrorCtx);
            return TFTP_OK;
        }
    }
    return TFTP_ERROR;
}

TftpOperationResult TFTPClient::tftpFetchDataReceivedCbk (
        int data_size,
        void *context)
{
    if (context != NULL) {
        TFTPClient *client = (TFTPClient *) context;
        if (client->_tftpFetchDataReceivedCallback != nullptr) {
            client->_tftpFetchDataReceivedCallback(data_size, 
                                                   client->tftpFetchDataReceivedCtx);
            return TFTP_OK;
        }
    }
    return TFTP_ERROR;
}