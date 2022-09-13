//
// Created by kollins on 05/09/2022.
//

#ifndef TFTPSERVER_H
#define TFTPSERVER_H

#include "ITFTPServer.h"

/**
 * @brief TFTP server implementation.
 */
class TFTPServer : public ITFTPServer {
public:
    TFTPServer();
    virtual ~TFTPServer();

    TftpServerOperationResult setPort(
            const int port
    ) override;

    TftpServerOperationResult setTimeout(
            const int timeout
    ) override;

    TftpServerOperationResult registerOpenFileCallback(
            openFileCallback callback,
            void *context
    ) override;

    TftpServerOperationResult registerCloseFileCallback(
            closeFileCallback callback,
            void *context
    ) override;

    TftpServerOperationResult registerSectionStartedCallback(
            sectionStartedCallback callback,
            void *context
    ) override;

    TftpServerOperationResult registerSectionFinishedCallback(
            sectionFinishedCallback callback,
            void *context
    ) override;

    TftpServerOperationResult startListening() override;

    TftpServerOperationResult stopListening() override;

private:

    static TftpdOperationResult sectionStartedCbk (
            const TftpdSectionHandlerPtr sectionHandler,
            void *context
    );

    static TftpdOperationResult sectionFinishedCbk (
            const TftpdSectionHandlerPtr sectionHandler,
            void *context
    );

    static TftpdOperationResult openFileCbk (
            FILE **fd,
            char *filename,
            char *mode,
            void *context
    );

    static TftpdOperationResult closeFileCbk (
            FILE *fd,
            void *context
    );

    TftpdHandlerPtr serverHandler;

    void *openFileCtx;
    openFileCallback _openFileCallback;

    void *closeFileCtx;
    closeFileCallback _closeFileCallback;

    void *sectionStartedCtx;
    sectionStartedCallback _sectionStartedCallback;

    void *sectionFinishedCtx;
    sectionFinishedCallback _sectionFinishedCallback;
};

/**
 * @brief TFTP section implementation.
 */
class TFTPSection : public ITFTPSection {
public:
    friend TFTPServer;
    ~TFTPSection() = default;

    TftpServerOperationResult getSectionId(
            TftpSectionId *id
    ) override;

    TftpServerOperationResult getClientIp(
            std::string &ip
    ) override;

    TftpServerOperationResult getSectionStatus(
            TftpServerSectionStatus *status
    ) override;

private:
    TFTPSection(const TftpdSectionHandlerPtr sectionHandler);
    TftpdSectionHandlerPtr sectionHandler;
};

#endif //TFTPSERVER_H
