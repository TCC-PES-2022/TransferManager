//
// Created by kollins on 03/09/2022.
//

#ifndef TFTPCLIENT_H
#define TFTPCLIENT_H

#include "ITFTPClient.h"

/**
 * @brief TFTP client implementation.
 */
class TFTPClient : public ITFTPClient {
public:
    TFTPClient();
    virtual ~TFTPClient();

    TftpClientOperationResult setConnection(
            const char *host,
            const int port
    ) override;

    TftpClientOperationResult registerTftpErrorCallback(
            tftpErrorCallback callback,
            void *context
    ) override;

    TftpClientOperationResult registerTftpFetchDataReceivedCallback(
            tftpfetchDataReceivedCallback callback,
            void *context
    ) override;

    TftpClientOperationResult sendFile(
            const char *filename,
            FILE *fp
    ) override;

    TftpClientOperationResult fetchFile(
            const char *filename,
            FILE *fp
    ) override;

private:
    TftpHandlerPtr clientHandler;

    static TftpOperationResult tftpErrorCbk (
            short error_code,
            const char *error_message,
            void *context
    );

    static TftpOperationResult tftpFetchDataReceivedCbk (
            int data_size,
            void *context
    );

    void *tftpErrorCtx;
    tftpErrorCallback _tftpErrorCallback;
    void *tftpFetchDataReceivedCtx;
    tftpfetchDataReceivedCallback _tftpFetchDataReceivedCallback;
};

#endif //TFTPCLIENT_H
