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
#define MEM_MEM_MSG "MEM MEM TEST"
#define BUFSIZE 100

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

typedef struct {
    SectionId sectionId;
    std::string clientIp;
    int matchSection;
    TftpServerSectionStatus status;
} ClientDiskServerDiskCommunicationContext;

TftpServerOperationResult ClientDiskServerDiskCommunication_sectionStartedCbk (
        ITFTPSection *sectionHandler,
        void *context)
{
    if (context != nullptr) {
        ClientDiskServerDiskCommunicationContext *ctx = (ClientDiskServerDiskCommunicationContext *)context;
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

TftpServerOperationResult ClientDiskServerDiskCommunication_sectionFinishedCbk (
        ITFTPSection *sectionHandler,
        void *context)
{
    if (context != nullptr) {
        ClientDiskServerDiskCommunicationContext *ctx = (ClientDiskServerDiskCommunicationContext *)context;
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

TEST(TFTPClientServer, ClientDiskServerDiskCommunication)
{
    ITFTPServer *server = new TFTPServer();
    ITFTPClient *client = new TFTPClient();
    ClientDiskServerDiskCommunicationContext context;
    context.sectionId = 0;
    context.matchSection = 0;
    context.status = TftpServerSectionStatus::TFTP_SERVER_SECTION_UNDEFINED;

    server->setPort(PORT);
    server->setTimeout(TIMEOUT);
    server->registerSectionStartedCallback(
        ClientDiskServerDiskCommunication_sectionStartedCbk, &context);
    server->registerSectionFinishedCallback(
        ClientDiskServerDiskCommunication_sectionFinishedCbk, &context);

    std::thread serverThread([&](){
        server->startListening();
    });

    client->setConnection("127.0.0.1", PORT);

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

    std::thread serverThread([&](){
        server->startListening();
    });

    client->setConnection("127.0.0.1", PORT);

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

typedef struct {
    SectionId sectionId;
    char buffer[BUFSIZE];
} ClientMemoryServerMemoryCommunicationContext;

TftpServerOperationResult ClientMemoryServerMemoryCommunicationContext_sectionStartedCbk (
        ITFTPSection *sectionHandler,
        void *context)
{
    if (context != nullptr) {
        ClientMemoryServerMemoryCommunicationContext *ctx = (ClientMemoryServerMemoryCommunicationContext *)context;
        SectionId id;
        sectionHandler->getSectionId(&id);
        ctx->sectionId = id;
        return TftpServerOperationResult::TFTP_SERVER_OK;
    }
    return TftpServerOperationResult::TFTP_SERVER_ERROR;
}

TftpServerOperationResult ClientMemoryServerMemoryCommunication_openFileCbk (
        ITFTPSection *sectionHandler,
        FILE **fd,
        char *filename,
        char* mode,
        void *context)
{
    if (context != nullptr) {
        ClientMemoryServerMemoryCommunicationContext *ctx =
                (ClientMemoryServerMemoryCommunicationContext*)context;
        SectionId id;
        sectionHandler->getSectionId(&id);
        if (ctx->sectionId == id) {
            *fd = fmemopen(ctx->buffer, BUFSIZE, mode);
            return TftpServerOperationResult::TFTP_SERVER_OK;
        }
        return TftpServerOperationResult::TFTP_SERVER_OK;
    }
    return TftpServerOperationResult::TFTP_SERVER_ERROR;
}

TftpServerOperationResult ClientMemoryServerMemoryCommunication_closeFileCbk (
        ITFTPSection *sectionHandler,
        FILE *fd,
        void *context)
{
    if (fd != NULL) {
        return fclose(fd) == 0 ? TftpServerOperationResult::TFTP_SERVER_OK :
                                 TftpServerOperationResult::TFTP_SERVER_ERROR;
    }
    return TftpServerOperationResult::TFTP_SERVER_OK;
}

TEST(TFTPClientServer, ClientMemoryServerMemoryCommunication)
{
    ITFTPServer *server = new TFTPServer();
    ITFTPClient *client = new TFTPClient();
    ClientMemoryServerMemoryCommunicationContext context;
    memset(context.buffer, 0, BUFSIZE);

    server->setPort(PORT);
    server->setTimeout(TIMEOUT);
    server->registerSectionStartedCallback(
        ClientMemoryServerMemoryCommunicationContext_sectionStartedCbk, &context);
    server->registerOpenFileCallback(
        ClientMemoryServerMemoryCommunication_openFileCbk, &context);
    server->registerCloseFileCallback(
        ClientMemoryServerMemoryCommunication_closeFileCbk, &context);

    std::thread serverThread([&](){
        server->startListening();
    });

    client->setConnection("127.0.0.1", PORT);

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
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        FAIL() << "clock_gettime() failed";
    }
    ts.tv_sec += TIMEOUT;
    int s = pthread_timedjoin_np(serverThreadID, NULL, &ts);
    delete server;

    if (s != 0) {
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
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        FAIL() << "clock_gettime() failed";
    }
    ts.tv_sec += TIMEOUT;
    int s1 = pthread_timedjoin_np(serverThreadID1, NULL, &ts);
    int s2 = pthread_timedjoin_np(serverThreadID2, NULL, &ts);

    delete server1;
    delete server2;

    if (s1 != 0 || s2 != 0) {
        FAIL() << "pthread_timedjoin_np() failed";
    }
    SUCCEED();
}