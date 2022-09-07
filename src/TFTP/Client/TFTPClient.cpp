//
// Created by kollins on 03/09/2022.
//

#include "TFTPClient.h"

TFTPClient::TFTPClient() {
    if (create_tftp_handler(&clientHandler) != TFTP_OK) {
        clientHandler = nullptr;
        throw "CLIENT HANDLER CREATION FAILED!";
    }
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