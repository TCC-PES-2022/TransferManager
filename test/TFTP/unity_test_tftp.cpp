#include <gtest/gtest.h>

#include "TFTPClient.h"
#include "TFTPServer.h"

#include <thread>
#include <arpa/inet.h>
#define SOCKADDR_PRINT_ADDR_LEN INET6_ADDRSTRLEN

#define PORT 60907
#define TIMEOUT 5
#define LOCALHOST "127.0.0.1"
#define FILENAME_DISK_DISK_SEND "disk_disk_test_send.txt"
#define FILENAME_DISK_DISK_RECEIVE "disk_disk_test_receive.txt"
#define DISK_DISK_MSG "DISK DISK TEST"
#define FILENAME_MEM_DISK "mem_disk_test.txt"
#define MEM_DISK_MSG "MEM DISK TEST"
#define FILENAME_MEM_MEM "mem_mem_test.txt"
#define FILENAME_MEM_MEM_SERIALIZED "mem_mem_serializer_test.txt"
#define MEM_MEM_MSG "MEM MEM TEST"
#define BUFSIZE 100
#define ERROR_FILE "error_test.txt"
#define ERROR_CODE 0
#define ABORT_TFTP_MSG "ABORT:1003"
#define WAIT_TFTP_MSG "WAIT:1"

typedef struct
{
    SectionId sectionId;
    std::string clientIp;
    int matchSection;
    TftpServerSectionStatus status;
    char buffer[BUFSIZE];
    std::string tftpErrorMsg;
    short tftpErrorCode;
} ClientServerContext;

/*
 *******************************************************************************
 *                             INITIAL CLIENT TEST                             *
 *******************************************************************************
 */
TEST(TFTPClient, ClientCreation)
{
    ITFTPClient *client;
    ASSERT_NO_THROW(client = new TFTPClient());
    delete client;
}

TEST(TFTPClient, ClientSetConnection)
{
    ITFTPClient *client = new TFTPClient();
    TftpClientOperationResult result = client->setConnection(LOCALHOST, PORT);
    ASSERT_EQ(TftpClientOperationResult::TFTP_CLIENT_OK, result);
    delete client;
}

/*
 *******************************************************************************
 *                             INITIAL SERVER TEST                             *
 *******************************************************************************
 */
TEST(TFTPServer, ServerCreation)
{
    ITFTPServer *server;
    ASSERT_NO_THROW(server = new TFTPServer());
    delete server;
}

TEST(TFTPServer, ServerSetPort)
{
    ITFTPServer *server = new TFTPServer();
    TftpServerOperationResult result = server->setPort(PORT);
    ASSERT_EQ(TftpServerOperationResult::TFTP_SERVER_OK, result);
    delete server;
}

TEST(TFTPServer, ServerSetTimeout)
{
    ITFTPServer *server = new TFTPServer();
    TftpServerOperationResult result = server->setTimeout(TIMEOUT);
    ASSERT_EQ(TftpServerOperationResult::TFTP_SERVER_OK, result);
    delete server;
}

TEST(TFTPServer, ServerRegisterOpenCallback)
{
    ITFTPServer *server = new TFTPServer();
    TftpServerOperationResult result = server->registerOpenFileCallback(nullptr, nullptr);
    ASSERT_EQ(TftpServerOperationResult::TFTP_SERVER_OK, result);
    delete server;
}

TEST(TFTPServer, ServerRegisterCloseCallback)
{
    ITFTPServer *server = new TFTPServer();
    TftpServerOperationResult result = server->registerCloseFileCallback(nullptr, nullptr);
    ASSERT_EQ(TftpServerOperationResult::TFTP_SERVER_OK, result);
    delete server;
}

TEST(TFTPServer, ServerRegisterSectionStartedCallback)
{
    ITFTPServer *server = new TFTPServer();
    TftpServerOperationResult result = server->registerSectionStartedCallback(nullptr, nullptr);
    ASSERT_EQ(TftpServerOperationResult::TFTP_SERVER_OK, result);
    delete server;
}

TEST(TFTPServer, ServerRegisterSectionFinishedCallback)
{
    ITFTPServer *server = new TFTPServer();
    TftpServerOperationResult result = server->registerSectionFinishedCallback(nullptr, nullptr);
    ASSERT_EQ(TftpServerOperationResult::TFTP_SERVER_OK, result);
    delete server;
}

/*
 *******************************************************************************
 *                          CLIENT-SERVER COMMUNICATION                        *
 *******************************************************************************
 */

TftpServerOperationResult ClientDiskServerDiskCommunication_sectionStartedCbk(
    ITFTPSection *sectionHandler,
    void *context)
{
    if (context != nullptr)
    {
        ClientServerContext *ctx = (ClientServerContext *)context;
        SectionId id;
        std::string clientIp;
        sectionHandler->getSectionId(&id);
        sectionHandler->getClientIp(clientIp);
        ctx->sectionId = id;
        ctx->clientIp = clientIp;
        return TftpServerOperationResult::TFTP_SERVER_OK;
    }
    return TftpServerOperationResult::TFTP_SERVER_ERROR;
}

TftpServerOperationResult ClientDiskServerDiskCommunication_sectionFinishedCbk(
    ITFTPSection *sectionHandler,
    void *context)
{
    if (context != nullptr)
    {
        ClientServerContext *ctx = (ClientServerContext *)context;
        SectionId id;
        std::string clientIp;
        sectionHandler->getSectionId(&id);
        sectionHandler->getClientIp(clientIp);
        ctx->matchSection = (ctx->sectionId == id && ctx->clientIp == clientIp);
        sectionHandler->getSectionStatus(&ctx->status);
        return TftpServerOperationResult::TFTP_SERVER_OK;
    }
    return TftpServerOperationResult::TFTP_SERVER_ERROR;
}

//TODO: This test is failing on Lubuntu virtual machine. Working on WSL.
//      Investigate.
TEST(TFTPClientServer, DISABLED_ClientDiskServerDiskCommunication)
{
    ITFTPServer *server = new TFTPServer();
    ITFTPClient *client = new TFTPClient();
    ClientServerContext context;
    context.sectionId = 0;
    context.matchSection = 0;
    context.status = TftpServerSectionStatus::TFTP_SERVER_SECTION_UNDEFINED;

    server->setPort(PORT);
    server->setTimeout(TIMEOUT);
    server->registerSectionStartedCallback(
        ClientDiskServerDiskCommunication_sectionStartedCbk, &context);
    server->registerSectionFinishedCallback(
        ClientDiskServerDiskCommunication_sectionFinishedCbk, &context);

    std::thread serverThread([&]()
                             { server->startListening(); });

    client->setConnection(LOCALHOST, PORT);

    FILE *sendFd = fopen(FILENAME_DISK_DISK_SEND, "w");
    fprintf(sendFd, DISK_DISK_MSG);
    fclose(sendFd);

    sendFd = fopen(FILENAME_DISK_DISK_SEND, "r");
    client->sendFile(FILENAME_DISK_DISK_SEND, sendFd);
    fclose(sendFd);

    FILE *receiveFd = fopen(FILENAME_DISK_DISK_RECEIVE, "w");
    client->fetchFile(FILENAME_DISK_DISK_SEND, receiveFd);
    fclose(receiveFd);

    receiveFd = fopen(FILENAME_DISK_DISK_RECEIVE, "r");
    char buffer[BUFSIZE];
    fgets(buffer, BUFSIZE, receiveFd);
    fclose(receiveFd);

    server->stopListening();
    serverThread.join();

    delete server;
    delete client;

    ASSERT_STREQ(buffer, DISK_DISK_MSG);
    ASSERT_EQ(context.matchSection, 1);
    ASSERT_EQ(context.status, TftpServerSectionStatus::TFTP_SERVER_SECTION_OK);
}

TEST(TFTPClientServer, ClientMemoryServerDiskCommunication)
{
    ITFTPServer *server = new TFTPServer();
    ITFTPClient *client = new TFTPClient();

    server->setPort(PORT);
    server->setTimeout(TIMEOUT);

    std::thread serverThread([&]()
                             { server->startListening(); });

    client->setConnection(LOCALHOST, PORT);

    char *sendBuffer = new char[BUFSIZE];
    strcpy(sendBuffer, MEM_DISK_MSG);
    FILE *sendFd = fmemopen(sendBuffer, BUFSIZE, "r");
    client->sendFile(FILENAME_MEM_DISK, sendFd);

    char *receiveBuffer = new char[BUFSIZE];
    FILE *receiveFd = fmemopen(receiveBuffer, BUFSIZE, "w");
    client->fetchFile(FILENAME_MEM_DISK, receiveFd);

    server->stopListening();
    serverThread.join();

    fclose(sendFd);
    fclose(receiveFd);

    delete server;
    delete client;

    ASSERT_STREQ(sendBuffer, receiveBuffer);
    ASSERT_STREQ(sendBuffer, MEM_DISK_MSG);
}

TftpServerOperationResult ClientServerContext_sectionStartedCbk(
    ITFTPSection *sectionHandler,
    void *context)
{
    if (context != nullptr)
    {
        ClientServerContext *ctx = (ClientServerContext *)context;
        SectionId id;
        sectionHandler->getSectionId(&id);
        ctx->sectionId = id;
        return TftpServerOperationResult::TFTP_SERVER_OK;
    }
    return TftpServerOperationResult::TFTP_SERVER_ERROR;
}

TftpServerOperationResult ClientMemoryServerMemoryCommunication_openFileCbk(
    ITFTPSection *sectionHandler,
    FILE **fd,
    char *filename,
    char *mode,
    size_t *fileSize,
    void *context)
{
    if (context != nullptr)
    {
        ClientServerContext *ctx =
            (ClientServerContext *)context;
        SectionId id;
        sectionHandler->getSectionId(&id);
        if (ctx->sectionId == id)
        {
            *fd = fmemopen(ctx->buffer, BUFSIZE, mode);
            if (fileSize != NULL)
            {
                *fileSize = BUFSIZE;
            }
            return TftpServerOperationResult::TFTP_SERVER_OK;
        }
        return TftpServerOperationResult::TFTP_SERVER_OK;
    }
    return TftpServerOperationResult::TFTP_SERVER_ERROR;
}

TftpServerOperationResult ClientMemoryServerMemoryCommunication_closeFileCbk(
    ITFTPSection *sectionHandler,
    FILE *fd,
    void *context)
{
    if (fd != NULL)
    {
        return fclose(fd) == 0 ? TftpServerOperationResult::TFTP_SERVER_OK : TftpServerOperationResult::TFTP_SERVER_ERROR;
    }
    return TftpServerOperationResult::TFTP_SERVER_OK;
}

TEST(TFTPClientServer, ClientMemoryServerMemoryCommunication)
{
    ITFTPServer *server = new TFTPServer();
    ITFTPClient *client = new TFTPClient();
    ClientServerContext context;
    memset(context.buffer, 0, BUFSIZE);

    server->setPort(PORT);
    server->setTimeout(TIMEOUT);
    server->registerSectionStartedCallback(
        ClientServerContext_sectionStartedCbk, &context);
    server->registerOpenFileCallback(
        ClientMemoryServerMemoryCommunication_openFileCbk, &context);
    server->registerCloseFileCallback(
        ClientMemoryServerMemoryCommunication_closeFileCbk, &context);

    std::thread serverThread([&]()
                             { server->startListening(); });

    client->setConnection(LOCALHOST, PORT);

    char *sendBuffer = new char[BUFSIZE];
    strcpy(sendBuffer, MEM_MEM_MSG);
    FILE *sendFd = fmemopen(sendBuffer, BUFSIZE, "r");
    client->sendFile(FILENAME_MEM_MEM, sendFd);

    char *receiveBuffer = new char[BUFSIZE];
    FILE *receiveFd = fmemopen(receiveBuffer, BUFSIZE, "w");
    client->fetchFile(FILENAME_MEM_MEM, receiveFd);

    server->stopListening();
    serverThread.join();

    fclose(sendFd);
    fclose(receiveFd);

    delete server;
    delete client;

    ASSERT_STREQ(sendBuffer, receiveBuffer);
    ASSERT_STREQ(sendBuffer, context.buffer);
    ASSERT_STREQ(sendBuffer, MEM_MEM_MSG);
}

/*
 *******************************************************************************
 *                                    EXTRA                                    *
 *******************************************************************************
 */

TEST(TFTPClientServer, ClientMemoryServerMemoryCommunicationSerialized)
{
    ITFTPServer *server = new TFTPServer();
    ITFTPClient *client = new TFTPClient();
    ClientServerContext context;
    memset(context.buffer, 0, BUFSIZE);

    server->setPort(PORT);
    server->setTimeout(TIMEOUT);
    server->registerSectionStartedCallback(
        ClientServerContext_sectionStartedCbk, &context);
    server->registerOpenFileCallback(
        ClientMemoryServerMemoryCommunication_openFileCbk, &context);
    server->registerCloseFileCallback(
        ClientMemoryServerMemoryCommunication_closeFileCbk, &context);

    std::thread serverThread([&]()
                             { server->startListening(); });

    client->setConnection(LOCALHOST, PORT);

    char *sendBuffer = new char[BUFSIZE];
    memset(sendBuffer, 0, BUFSIZE);

    // Basic TH_Upload_Initialization serialization
    sendBuffer[0] = 0x00;
    sendBuffer[1] = 0x00;
    sendBuffer[2] = 0x00;
    sendBuffer[3] = 0x09;
    sendBuffer[4] = 0x41;
    sendBuffer[5] = 0x34;
    sendBuffer[6] = 0x00;
    sendBuffer[7] = 0x01;
    sendBuffer[8] = 0x00;
    FILE *sendFd = fmemopen(sendBuffer, BUFSIZE, "r");
    client->sendFile(FILENAME_MEM_MEM_SERIALIZED, sendFd);

    char *receiveBuffer = new char[BUFSIZE];
    memset(receiveBuffer, 0, BUFSIZE);
    FILE *receiveFd = fmemopen(receiveBuffer, BUFSIZE, "w");
    client->fetchFile(FILENAME_MEM_MEM_SERIALIZED, receiveFd);

    server->stopListening();
    serverThread.join();

    fclose(sendFd);
    fclose(receiveFd);

    delete server;
    delete client;

    for (int i = 0; i < BUFSIZE; i++)
    {
        ASSERT_EQ(sendBuffer[i], receiveBuffer[i]);
    }
}

TftpServerOperationResult CustomTftpErrorMessageSend_openFileCbk(
    ITFTPSection *sectionHandler,
    FILE **fd,
    char *filename,
    char *mode,
    size_t *fileSize,
    void *context)
{
    (*fd) = NULL;
    std::string errorMsg(ABORT_TFTP_MSG);
    sectionHandler->setErrorMessage(errorMsg);
    return TftpServerOperationResult::TFTP_SERVER_ERROR;
}

TftpClientOperationResult CustomTftpErrorMessage_tftpErrorCbk (
        short error_code,
        std::string &error_message,
        void *context)
{
    if (context != nullptr)
    {
        ClientServerContext *ctx =
            (ClientServerContext *)context;
        ctx->tftpErrorMsg = error_message;
        ctx->tftpErrorCode = error_code;
    }
    return TftpClientOperationResult::TFTP_CLIENT_OK;
}

TEST(TFTPClientServer, CustomTftpErrorMessageSend)
{
    ITFTPServer *server = new TFTPServer();
    ITFTPClient *client = new TFTPClient();
    ClientServerContext context;

    server->setPort(PORT);
    server->setTimeout(TIMEOUT);
    server->registerOpenFileCallback(
        CustomTftpErrorMessageSend_openFileCbk, &context);

    std::thread serverThread([&]()
                             { server->startListening(); });

    client->setConnection(LOCALHOST, PORT);
    client->registerTftpErrorCallback(
        CustomTftpErrorMessage_tftpErrorCbk, &context);

    FILE *sendFd = fopen(ERROR_FILE, "w+");
    if (sendFd == NULL)
    {
        FAIL() << "Error opening file: " << ERROR_FILE;
    }
    ASSERT_EQ(client->sendFile(ERROR_FILE, sendFd),
              TftpClientOperationResult::TFTP_CLIENT_ERROR);

    server->stopListening();
    serverThread.join();

    delete server;
    delete client;

    ASSERT_EQ(context.tftpErrorMsg, ABORT_TFTP_MSG);
    ASSERT_EQ(context.tftpErrorCode, ERROR_CODE);
}

TftpServerOperationResult CustomTftpErrorMessageFetch_openFileCbk(
    ITFTPSection *sectionHandler,
    FILE **fd,
    char *filename,
    char *mode,
    size_t *fileSize,
    void *context)
{
    (*fd) = NULL;
    std::string errorMsg(WAIT_TFTP_MSG);
    sectionHandler->setErrorMessage(errorMsg);
    return TftpServerOperationResult::TFTP_SERVER_ERROR;
}

TEST(TFTPClientServer, CustomTftpErrorMessageFetch)
{
    ITFTPServer *server = new TFTPServer();
    ITFTPClient *client = new TFTPClient();
    ClientServerContext context;

    server->setPort(PORT);
    server->setTimeout(TIMEOUT);
    server->registerOpenFileCallback(
        CustomTftpErrorMessageFetch_openFileCbk, &context);

    std::thread serverThread([&]()
                             { server->startListening(); });

    client->setConnection(LOCALHOST, PORT);
    client->registerTftpErrorCallback(
        CustomTftpErrorMessage_tftpErrorCbk, &context);

    FILE *sendFd = fopen(ERROR_FILE, "r+");
    if (sendFd == NULL)
    {
        FAIL() << "Error opening file: " << ERROR_FILE;
    }
    ASSERT_EQ(client->fetchFile(ERROR_FILE, sendFd),
              TftpClientOperationResult::TFTP_CLIENT_ERROR);

    server->stopListening();
    serverThread.join();

    delete server;
    delete client;

    ASSERT_EQ(context.tftpErrorMsg, WAIT_TFTP_MSG);
    ASSERT_EQ(context.tftpErrorCode, ERROR_CODE);
}

void *serverThread(void *arg)
{
    ITFTPServer *server = (ITFTPServer *)arg;
    server->startListening();
}

TEST(TFTPExtra, StartStopServer)
{
    ITFTPServer *server = new TFTPServer();
    server->setPort(PORT);
    pthread_t serverThreadID;
    pthread_create(&serverThreadID, NULL, serverThread, server);
    sleep(TIMEOUT);
    server->stopListening();
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
    {
        FAIL() << "clock_gettime() failed";
    }
    ts.tv_sec += TIMEOUT;
    int s = pthread_timedjoin_np(serverThreadID, NULL, &ts);
    delete server;

    if (s != 0)
    {
        FAIL() << "pthread_timedjoin_np() failed";
    }
    SUCCEED();
}

TEST(TFTPExtra, TwoServers)
{
    ITFTPServer *server1 = new TFTPServer();
    server1->setPort(PORT);
    pthread_t serverThreadID1;

    ITFTPServer *server2 = new TFTPServer();
    server2->setPort(PORT + 1);
    pthread_t serverThreadID2;

    pthread_create(&serverThreadID1, NULL, serverThread, server1);
    pthread_create(&serverThreadID2, NULL, serverThread, server2);
    sleep(TIMEOUT);
    server1->stopListening();
    server2->stopListening();

    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
    {
        FAIL() << "clock_gettime() failed";
    }
    ts.tv_sec += TIMEOUT;
    int s1 = pthread_timedjoin_np(serverThreadID1, NULL, &ts);
    int s2 = pthread_timedjoin_np(serverThreadID2, NULL, &ts);

    delete server1;
    delete server2;

    if (s1 != 0 || s2 != 0)
    {
        FAIL() << "pthread_timedjoin_np() failed";
    }
    SUCCEED();
}