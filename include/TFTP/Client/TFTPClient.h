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
};

#endif //TFTPCLIENT_H
