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
enum class TftpClientOperationResult
{
        TFTP_CLIENT_OK = 0,
        TFTP_CLIENT_ERROR
};

/**
 * @brief Enum with possible TFTP options.
 * Possible return values are:
 * - TFTP_CLIENT_BLOCKSIZE_OPTION:      Block size option.
 * - TFTP_CLIENT_PORT_OPTION:           Port option.
 */
enum class TftpClientOption {
    TFTP_CLIENT_BLOCKSIZE_OPTION,
    TFTP_CLIENT_PORT_OPTION
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
typedef TftpClientOperationResult (*tftpErrorCallback)(
    short error_code,
    std::string &error_message,
    void *context);

/**
 * @brief TFTP data received callback. This callback is called when the client
 * receives data from the server related to the current fetch operation.
 *
 * @param[in]   dataSize        Size of the data received.
 * @param[in]   context         Context passed to the callback.
 *
 * @return TFTP_CLIENT_OK if success.
 * @return TFTP_CLIENT_ERROR otherwise.
 */
typedef TftpClientOperationResult (*tftpfetchDataReceivedCallback)(
    int dataSize,
    void *context);

/**
 * @brief Option accepted callback. This callback is called when
 * the server accepts an option from the client. Some options may allow 
 * alternate values to be proposed, so make sure to check the value of the
 * option.
 *
 * @param[in] option the option received.
 * @param[in] value the value of the option.
 * @param[in] context the user context.
 *
 * @return TFTP_CLIENT_OK if success.
 * @return TFTP_CLIENT_ERROR otherwise.
 */
typedef TftpOperationResult (*tftpOptionAcceptedCallback) (
        std::string &option,
        std::string &value,
        void *context
);

/**
 * @brief TFTP client interface.
 */
class ITFTPClient
{
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
            const int port) = 0;

        /**
         * @brief Set options for TFTP connection
         *
         * @param[in] option the option to set.
         * @param[in] value the option value.
         *
         * @return TFTP_CLIENT_OK if success.
         * @return TFTP_CLIENT_ERROR otherwise.
         */
        virtual TftpClientOperationResult setTftpOption(
            TftpClientOption option,
            std::string value) = 0;

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
            void *context) = 0;

        /**
         * @brief Register TFTP fetch data received callback
         *
         * @param[in] callback the callback to register.
         * @param[in] context the user context.
         *
         * @return TFTP_CLIENT_OK if success.
         * @return TFTP_CLIENT_ERROR otherwise.
         */
        virtual TftpClientOperationResult registerTftpFetchDataReceivedCallback(
            tftpfetchDataReceivedCallback callback,
            void *context) = 0;

        /**
         * @brief Register TFTP option accepted callback
         *
         * @param[in] callback the callback to register.
         * @param[in] context the user context.
         *
         * @return TFTP_CLIENT_OK if success.
         * @return TFTP_CLIENT_ERROR otherwise.
         */
        virtual TftpClientOperationResult registerTftpOptionAcceptedCallback(
            tftpOptionAcceptedCallback callback,
            void *context) = 0;

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
            FILE *fp) = 0;

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
            FILE *fp) = 0;
};

#endif // ITFTPCLIENT_H
