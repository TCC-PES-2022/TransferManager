//
// Created by kollins on 03/09/2022.
//

#ifndef ITFTPCLIENT_H
#define ITFTPCLIENT_H

#include "tftp_api.h"
#include <string>

/**
 * @brief Enum with possible return from interface functions.
 * Possible return values are:
 * - TFTP_CLIENT_OK:                    Operation was successful.
 * - TFTP_CLIENT_ERROR:                 Generic error.
 */
enum class TftpClientOperationResult {
    TFTP_CLIENT_OK = 0,
    TFTP_CLIENT_ERROR
};

/**
 * @brief TFTP error callback. This callback is called when the client
 * receives an error from the server.
 *
 * @param[in]   error_code      Error code.
 * @param[in]   error_msg       Error message.
 * @param[in]   context         Context passed to the callback.
 *
 * @return TFTP_CLIENT_OK if success.
 * @return TFTP_CLIENT_ERROR otherwise.
 */
typedef TftpClientOperationResult (*tftpErrorCallback) (
        short error_code,
        std::string &error_message,
        void *context
);

/**
 * @brief TFTP client interface.
 */
class ITFTPClient {
public:
    virtual ~ITFTPClient() = default;
    
    /**
     * @brief Set host and port for TFTP connection
     * @param[in] host the host to connect to.
     * @param[in] port the port to connect to.
     *
     * @return TFTP_CLIENT_OK if success.
     * @return TFTP_CLIENT_ERROR otherwise.
     */
    virtual TftpClientOperationResult setConnection(
            const char *host,
            const int port
    ) = 0;

    /**
     * @brief Register TFTP error callback
     *
     * @param[in] callback the callback to register.
     * @param[in] context the user context.
     *
     * @return TFTP_CLIENT_OK if success.
     * @return TFTP_CLIENT_ERROR otherwise.
     */
    virtual TftpClientOperationResult registerTftpErrorCallback(
            tftpErrorCallback callback,
            void *context
    ) = 0;

    /**
     * @brief Send a file through TFTP.
     *
     * @param[in] filename the name of the file to send.
     * This is the name the server will save the file as.
     * @param[in] fp the file pointer to send.
     *
     * @return TFTP_CLIENT_OK if success.
     * @return TFTP_CLIENT_ERROR otherwise.
     */
    virtual TftpClientOperationResult sendFile(
            const char *filename,
            FILE *fp
    ) = 0;

    /**
     * @brief Fetches a file through TFTP.
     *
     * @param[in] filename the name of the file to fetch.
     * @param[in] fp the file pointer to receive data.
     *
     * @return TFTP_CLIENT_OK if success.
     * @return TFTP_CLIENT_ERROR otherwise.
     */
    virtual TftpClientOperationResult fetchFile(
            const char *filename,
            FILE *fp
    ) = 0;
};

#endif //ITFTPCLIENT_H
