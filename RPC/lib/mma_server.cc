#include "mma_server.h"

namespace proj4 {

MmaServiceImpl service;
std::unique_ptr<Server> server;

// Logic and data behind the server's behavior.
Status MmaServiceImpl::Allocate(ServerContext* context, const AllocateRequest* request,
                                AllocateReply* reply) {
    mux.lock();
    size_t page_needed = (request->size() + PageSize - 1) / PageSize;
    if (max_vir_page_num < page_needed) {
        reply->set_success(false);
        mux.unlock();
        return Status::OK;
    }
    max_vir_page_num -= page_needed;
    int array_id = mma->Allocate(request->size());
    reply->set_arrayid(array_id);
    reply->set_success(true);
    mux.unlock();
    return Status::OK;
}

Status MmaServiceImpl::Free(ServerContext* context, const FreeRequest* request,
                            Null* reply) {
    mux.lock();
    max_vir_page_num += (mma->GetArraySize(request->arrayid()) + PageSize - 1) / PageSize;
    mma->Release(request->arrayid());
    mux.unlock();
    return Status::OK;
}

Status MmaServiceImpl::ReadPage(ServerContext* context, const ReadRequest* request,
                                ReadReply* reply) {
    int value = mma->ReadPage(request->arrayid(), request->virtualpageid(),
                              request->offset());
    reply->set_value(value);
    return Status::OK;
}

Status MmaServiceImpl::WritePage(ServerContext* context, const WriteRequest* request,
                                 Null* reply) {
    mma->WritePage(request->arrayid(), request->virtualpageid(),
                   request->offset(), request->value());
    return Status::OK;
}

void RunServerUL(size_t phy_page_num) {
    std::string server_address("0.0.0.0:50051");
    service.mma = new MemoryManager(phy_page_num);
    service.max_vir_page_num = ~0u;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    server = std::move(builder.BuildAndStart());
    // std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

void RunServerL(size_t phy_page_num, size_t max_vir_page_num) {
    std::string server_address("0.0.0.0:50051");
    service.mma = new MemoryManager(phy_page_num);
    service.max_vir_page_num = max_vir_page_num;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    server = std::move(builder.BuildAndStart());
    // std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

void ShutdownServer() {
    delete service.mma;
    server->Shutdown();
}

}
