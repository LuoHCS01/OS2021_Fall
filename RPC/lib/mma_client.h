#ifndef MMA_CLIENT_H
#define MMA_CLIENT_H

#include <memory>
#include <cstdlib>

#include <grpc++/grpc++.h>

#ifdef BAZEL_BUILD
#include "proto/mma.grpc.pb.h"
#else
#include "mma.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using mma::Mma;
using mma::AllocateRequest;
using mma::AllocateReply;
using mma::FreeRequest;
using mma::ReadRequest;
using mma::ReadReply;
using mma::WriteRequest;
using mma::Null;

namespace proj4 {

class ArrayList;

class MmaClient {
  public:
    MmaClient(std::shared_ptr<Channel> channel)
      : stub_(Mma::NewStub(channel)) {}
    
    ArrayList* Allocate(size_t);
    void Free(ArrayList*);

    int ReadPage(int array_id, int virtual_page_id, int offset);
    void WritePage(int array_id, int virtual_page_id, int offset, int value);

  private:
    std::unique_ptr<Mma::Stub> stub_;
};

} //namespace proj4

#endif
