//
// Created by kollins on 05/09/2022.
//

#include "TFTPServer.h"
#include <arpa/inet.h>

TFTPServer::TFTPServer() {
    if (create_tftpd_handler(&serverHandler) != TFTPD_OK) {
        serverHandler = nullptr;
        throw "SERVER HANDLER CREATION FAILED!";
    }
    
    register_section_started_callback(serverHandler, sectionStartedCbk, this);
    register_open_file_callback(serverHandler, openFileCbk, this);
    register_close_file_callback(serverHandler, closeFileCbk, this);
    register_section_finished_callback(serverHandler, sectionFinishedCbk, this);

    _openFileCallback = nullptr;
    _closeFileCallback = nullptr;
    _sectionStartedCallback = nullptr;
    _sectionFinishedCallback = nullptr;

    openFileCtx = nullptr;
    closeFileCtx = nullptr;
    sectionStartedCtx = nullptr;
    sectionFinishedCtx = nullptr;
}

TFTPServer::~TFTPServer() {
    if (serverHandler != nullptr) {

        register_section_started_callback(serverHandler, NULL, NULL);
        register_open_file_callback(serverHandler, NULL, NULL);
        register_close_file_callback(serverHandler, NULL, NULL);
        register_section_finished_callback(serverHandler, NULL, NULL);        
        
        if (destroy_tftpd_handler(&serverHandler) != TFTPD_OK) {
            //TODO: inform error
        }
    }
}

TftpServerOperationResult TFTPServer::setPort(
        const int port)
{
    if (serverHandler == nullptr) {
        return TftpServerOperationResult::TFTP_SERVER_ERROR;
    }

    return set_port(serverHandler, port) == TFTPD_OK ?
           TftpServerOperationResult::TFTP_SERVER_OK :
           TftpServerOperationResult::TFTP_SERVER_ERROR;
}

TftpServerOperationResult TFTPServer::setTimeout(
        const int timeout)
{
    if (serverHandler == nullptr) {
        return TftpServerOperationResult::TFTP_SERVER_ERROR;
    }

    return set_server_timeout(serverHandler, timeout) == TFTPD_OK ?
           TftpServerOperationResult::TFTP_SERVER_OK :
           TftpServerOperationResult::TFTP_SERVER_ERROR;
}

TftpServerOperationResult TFTPServer::startListening() {
    if (serverHandler == nullptr) {
        return TftpServerOperationResult::TFTP_SERVER_ERROR;
    }

    return start_listening(serverHandler) == TFTPD_OK ?
           TftpServerOperationResult::TFTP_SERVER_OK :
           TftpServerOperationResult::TFTP_SERVER_ERROR;
}

TftpServerOperationResult TFTPServer::stopListening() {
    if (serverHandler == nullptr) {
        return TftpServerOperationResult::TFTP_SERVER_ERROR;
    }

    return stop_listening(serverHandler) == TFTPD_OK ?
           TftpServerOperationResult::TFTP_SERVER_OK :
           TftpServerOperationResult::TFTP_SERVER_ERROR;
}

TftpServerOperationResult TFTPServer::registerOpenFileCallback(
        openFileCallback callback,
        void *context)
{
    _openFileCallback = callback;
    openFileCtx = context;
    return TftpServerOperationResult::TFTP_SERVER_OK;
}

TftpServerOperationResult TFTPServer::registerCloseFileCallback(
        closeFileCallback callback,
        void *context)
{
    _closeFileCallback = callback;
    closeFileCtx = context;
    return TftpServerOperationResult::TFTP_SERVER_OK;
}

TftpServerOperationResult TFTPServer::registerSectionStartedCallback(
        sectionStartedCallback callback,
        void *context)
{
    _sectionStartedCallback = callback;
    sectionStartedCtx = context;
    return TftpServerOperationResult::TFTP_SERVER_OK;
}

TftpServerOperationResult TFTPServer::registerSectionFinishedCallback(
        sectionFinishedCallback callback,
        void *context)
{
    _sectionFinishedCallback = callback;
    sectionFinishedCtx = context;
    return TftpServerOperationResult::TFTP_SERVER_OK;
}

TftpdOperationResult TFTPServer::sectionStartedCbk(
        const TftpdSectionHandlerPtr section_handler,
        void *context)
{
    if (context != NULL) {
        TFTPServer *server = (TFTPServer *) context;
        if (server->_sectionStartedCallback != nullptr) {
            TFTPSection section(section_handler);
            server->_sectionStartedCallback(&section, server->sectionStartedCtx);
            return TFTPD_OK;
        }
    }
    return TFTPD_ERROR;
}

TftpdOperationResult TFTPServer::sectionFinishedCbk(
        const TftpdSectionHandlerPtr section_handler,
        void *context)
{
    if (context != NULL) {
        TFTPServer *server = (TFTPServer *) context;
        if (server->_sectionFinishedCallback != nullptr) {
            TFTPSection section(section_handler);
            server->_sectionFinishedCallback(&section, server->sectionFinishedCtx);
            return TFTPD_OK;
        }
    }
    return TFTPD_ERROR;
}

TftpdOperationResult TFTPServer::openFileCbk(
        const TftpdSectionHandlerPtr section_handler,
        FILE **fd,
        char *filename,
        char* mode,
        void *context)
{
    if (context != NULL) {
        TFTPServer *server = (TFTPServer *) context;
        if (server->_openFileCallback != nullptr) {
            TFTPSection section(section_handler);
            server->_openFileCallback(&section, fd, filename, mode, server->openFileCtx);
            return TFTPD_OK;
        } else {
            *fd = fopen(filename, mode);
            return *fd != NULL ? TFTPD_OK : TFTPD_ERROR;
        }
    }
    return TFTPD_ERROR;
}

TftpdOperationResult TFTPServer::closeFileCbk(
        const TftpdSectionHandlerPtr section_handler,
        FILE *fd,
        void *context)
{
    if (context != NULL) {
        TFTPServer *server = (TFTPServer *) context;
        if (server->_closeFileCallback != nullptr) {
            TFTPSection section(section_handler);
            server->_closeFileCallback(&section, fd, server->closeFileCtx);
            return TFTPD_OK;
        } else {
            return fclose(fd) == 0 ? TFTPD_OK : TFTPD_ERROR;
        }
    }
    return TFTPD_ERROR;
}

TFTPSection::TFTPSection(
        const TftpdSectionHandlerPtr section_handler)
{
    sectionHandler = section_handler;
}

TftpServerOperationResult TFTPSection::getSectionId(
        TftpSectionId *id)
{
    if (sectionHandler == nullptr) {
        return TftpServerOperationResult::TFTP_SERVER_ERROR;
    }

    SectionId sectionId;
    if (get_section_id(sectionHandler, &sectionId) != TFTPD_OK) {
        return TftpServerOperationResult::TFTP_SERVER_ERROR;
    }

    *id = sectionId;
    return TftpServerOperationResult::TFTP_SERVER_OK;
}

TftpServerOperationResult TFTPSection::getClientIp(
        std::string &ip)
{
    if (sectionHandler == nullptr) {
        return TftpServerOperationResult::TFTP_SERVER_ERROR;
    }

    char clientIp[INET6_ADDRSTRLEN];
    if (get_client_ip(sectionHandler, clientIp) != TFTPD_OK) {
        return TftpServerOperationResult::TFTP_SERVER_ERROR;
    }

    ip = clientIp;
    return TftpServerOperationResult::TFTP_SERVER_OK;
}

TftpServerOperationResult TFTPSection::getSectionStatus(
        TftpServerSectionStatus *status)
{
    if (sectionHandler == nullptr) {
        return TftpServerOperationResult::TFTP_SERVER_ERROR;
    }

    TftpdSectionStatus sectionStatus;

    if (get_section_status(sectionHandler, &sectionStatus) != TFTPD_OK) {
        return TftpServerOperationResult::TFTP_SERVER_ERROR;
    }

    switch (sectionStatus) {
        case TFTPD_SECTION_OK:
            *status = TftpServerSectionStatus::TFTP_SERVER_SECTION_OK;
            break;
        case TFTPD_SECTION_ERROR:
            *status = TftpServerSectionStatus::TFTP_SERVER_SECTION_ERROR;
            break;
        default:
            *status = TftpServerSectionStatus::TFTP_SERVER_SECTION_UNDEFINED;
    }

    return TftpServerOperationResult::TFTP_SERVER_OK;
}