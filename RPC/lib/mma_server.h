#ifndef MMA_SERVER_H
#define MMA_SERVER_H

#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>

#include <grpc++/grpc++.h>
#include <grpc++/ext/proto_server_reflection_plugin.h>
#include <grpc++/health_check_service_interface.h>

#ifdef BAZEL_BUILD
#include "proto/mma.grpc.pb.h"
#else
#include "mma.grpc.pb.h"
#endif

#include "memory_manager.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using mma::Mma;
using mma::AllocateRequest;
using mma::AllocateReply;
using mma::FreeRequest;
using mma::ReadRequest;
using mma::ReadReply;
using mma::WriteRequest;
using mma::Null;

// Logic and data behind the server's behavior.

namespace proj4 {

class MmaServiceImpl final : public Mma::Service {
  public:
    Status Allocate(ServerContext* context, const AllocateRequest* request,
                    AllocateReply* reply) override;

    Status Free(ServerContext* context, const FreeRequest* request,
                Null* reply) override;

    Status ReadPage(ServerContext* context, const ReadRequest* request,
                    ReadReply* reply) override;

    Status WritePage(ServerContext* context, const WriteRequest* request,
                     Null* reply) override;
    
    MemoryManager* mma;
    size_t max_vir_page_num;
    std::mutex mux;
};

// setup a server with UnLimited virtual memory space
void RunServerUL(size_t phy_page_num);

// setup a server with Limited virtual memory space
void RunServerL(size_t phy_page_num, size_t max_vir_page_num);

// shutdown the server setup by RunServerUL or RunServerL
void ShutdownServer();

} //namespace proj4

#endif
