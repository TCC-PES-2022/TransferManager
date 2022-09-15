//
// Created by kollins on 05/09/2022.
//

#ifndef ITFTPSERVER_H
#define ITFTPSERVER_H

#include "tftpd_api.h"
#include <string>

/**
 * @brief Enum with possible return from interface functions.
 * Possible return values are:
 * - TFTP_SERVER_OK:                    Operation was successful.
 * - TFTP_SERVER_ERROR:                 Generic error.
 */
enum class TftpServerOperationResult {
    TFTP_SERVER_OK = 0,
    TFTP_SERVER_ERROR
};

/**
 * @brief Enum with possible section status.
 * Possible return values are:
 * - TFTP_SERVER_SECTION_OK:            Section has been successfully completed.
 * - TFTP_SERVER_SECTION_ERROR:         Section has failed.
 * - TFTP_SERVER_SECTION_UNDEFINED:     Section status is undefined.
 */
enum class TftpServerSectionStatus {
    TFTP_SERVER_SECTION_OK = 0,
    TFTP_SERVER_SECTION_ERROR,
    TFTP_SERVER_SECTION_UNDEFINED
};

typedef SectionId TftpSectionId;
class ITFTPSection;

/**
 * @brief Callback for new section. This callback is called when a
 * client requests an operation to be performed.
 *
 * @param[in] sectionHandler the section handler.
 * @param[in] context the user context.
 *
 * @return TFTP_SERVER_OK if success.
 * @return TFTP_SERVER_ERROR otherwise.
 */
typedef TftpServerOperationResult (*sectionStartedCallback) (
        ITFTPSection *sectionHandler,
        void *context
);

/**
* @brief Callback for end section. This callback is called when a
 * client operation is finished.
 *
 * @param[in] sectionHandler the section handler.
 * @param[in] context the user context.
 *
 * @return TFTP_SERVER_OK if success.
 * @return TFTP_SERVER_ERROR otherwise.
 */
typedef TftpServerOperationResult (*sectionFinishedCallback) (
        ITFTPSection *sectionHandler,
        void *context
);

/**
 * @brief Open file callback. This callback is called when
 * the server needs to open a file, so a file pointer must
 * be opened for the filename with the given mode.
 *
 * If you want to use the default open file function, don't
 * register this callback. The server will open the file
 * based on the file name received.
 *
 * If you open the file, you must close it. Use the
 * closeFileCallback() for that.
 *
 * @param[in] sectionHandler the section handler.
 * @param[out] fd the file pointer to open.
 * @param[in] filename the name of the file to open.
 * @param[in] mode the mode to open the file in.
 * @param[in] context the user context.
 *
 * @return TFTP_SERVER_OK if success.
 * @return TFTP_SERVER_ERROR otherwise.
 */
typedef TftpServerOperationResult (*openFileCallback) (
        ITFTPSection *sectionHandler,
        FILE **fd,
        char *filename,
        char* mode,
        void *context
);

/**
 * @brief Close file callback. This callback is called when
 * the server needs to close a file. If you register
 * open_file_callback, register this callback too.
 *
 * @param[in] sectionHandler the section handler.
 * @param[in] fd the file pointer to close.
 * @param[in] context the user context.
 *
 * @return TFTP_SERVER_OK if success.
 * @return TFTP_SERVER_ERROR otherwise.
 */
typedef TftpServerOperationResult (*closeFileCallback) (
        ITFTPSection *sectionHandler,
        FILE *fd,
        void *context
);


/**
 * @brief TFTP server interface.
 */
class ITFTPServer {
public:
    virtual ~ITFTPServer() = default;

    /**
     * @brief Set port to use for TFTP Server communication.
     *
     * @param[in] port the port to use for TFTP Server communication.
     *
     * @return TFTP_SERVER_OK if success.
     * @return TFTP_SERVER_ERROR otherwise.
     */
    virtual TftpServerOperationResult setPort(
            const int port
    ) = 0;

    /**
     * @brief Set timeout. This is the time the server will wait for a client
     * to send a request. If the client doesn't send a request within this time,
     * the server will close the connection. The default is 300 seconds.
     * If you want to disable the timeout, set the timeout to 0.
     *
     * @param[in] timeout the timeout in seconds.
     *
     * @return TFTP_SERVER_OK if success.
     * @return TFTP_SERVER_ERROR otherwise.
     */
    virtual TftpServerOperationResult setTimeout(
            const int timeout
    ) = 0;

    /**
     * @brief Register open file callback.
     *
     * @param[in] handler the pointer to the tftpd handler.
     * @param[in] callback the callback to register.
     * @param[in] context the user context.
     *
     * @return TFTP_SERVER_OK if success.
     * @return TFTP_SERVER_ERROR otherwise.
     */
    virtual TftpServerOperationResult registerOpenFileCallback(
            openFileCallback callback,
            void *context
    ) = 0;

    /**
     * @brief Register close file callback.
     *
     * @param[in] callback the callback to register.
     * @param[in] context the user context.
     *
     * @return TFTP_SERVER_OK if success.
     * @return TFTP_SERVER_ERROR otherwise.
     */
    virtual TftpServerOperationResult registerCloseFileCallback(
            closeFileCallback callback,
            void *context
    ) = 0;

    /**
     * @brief Register section start callback.
     *
     * @param[in] callback the callback to register.
     * @param[in] context the user context.
     *
     * @return TFTP_SERVER_OK if success.
     * @return TFTP_SERVER_ERROR otherwise.
     */
    virtual TftpServerOperationResult registerSectionStartedCallback(
            sectionStartedCallback callback,
            void *context
    ) = 0;

    /**
     * @brief Register section finished callback.
     *
     * @param[in] callback the callback to register.
     * @param[in] context the user context.
     *
     * @return TFTP_SERVER_OK if success.
     * @return TFTP_SERVER_ERROR otherwise.
     */
    virtual TftpServerOperationResult registerSectionFinishedCallback(
            sectionFinishedCallback callback,
            void *context
    ) = 0;

    /**
     * @brief Start the TFTP Server. This is a blocking function.
     * In order to stop be able to stop the server, call this function
     * from another thread and then use the stop_listening() function.
     *
     *
     * @return TFTP_SERVER_OK if success.
     * @return TFTP_SERVER_ERROR otherwise.
     */
    virtual TftpServerOperationResult startListening() = 0;

    /**
     * @brief Request the TFTP Server to stop listening. The success
     * return of this function doesn't mean the server has stopped,
     * it just means that the server will stop listening when it is
     * ready. When the server stops listening, the startListening()
     * function will return.
     *
     *
     * @return TFTP_SERVER_OK if success.
     * @return TFTP_SERVER_ERROR otherwise.
     */
    virtual TftpServerOperationResult stopListening() = 0;

};

/**
 * @brief TFTP section interface.
 */
class ITFTPSection {
public:
    /**
     * @brief Get identifier for the section.
     *
     * @param[out] section_id the port to use for TFTP Server communication.
     *
     * @return TFTP_SERVER_OK if success.
     * @return TFTP_SERVER_ERROR otherwise.
     */
    virtual TftpServerOperationResult getSectionId(
            TftpSectionId *id
    ) = 0;

    /**
     * @brief Get client IP
     *
     * @param[out] ip the client IP.
     *
     * @return TFTP_SERVER_OK if success.
     * @return TFTP_SERVER_ERROR otherwise.
     */
    virtual TftpServerOperationResult getClientIp(
            std::string &ip
    ) = 0;

    /**
     * @brief Get section status. Call this function from the section_finished
     * callback to check if the section was successful.
     *
     * @param[out] status the status of the section.
     *
     * @return TFTP_SERVER_OK if success.
     * @return TFTP_SERVER_ERROR otherwise.
     */
    virtual TftpServerOperationResult getSectionStatus(
            TftpServerSectionStatus *status
    ) = 0;

};

#endif //ITFTPSERVER_H
