#include "mma_client.h"
#include "array_list.h"

namespace proj4 {

ArrayList* MmaClient::Allocate(size_t sz) {
    AllocateRequest request;
    AllocateReply reply;
    ClientContext context;

    do {
        request.set_size(sz);
        Status status = stub_->Allocate(&context, request, &reply);
        if (!status.ok()) {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return nullptr;
        }
    // If not allocated, start new rpc after 0.1s to prevent busy-waiting
    } while (!reply.success() && usleep(1000000));
    
    ArrayList* arr = new ArrayList(sz, this, reply.arrayid());
    return arr;
}

void MmaClient::Free(ArrayList* arr) {
    FreeRequest request;
    Null reply;
    ClientContext context;
    request.set_arrayid(arr->array_id);
    Status status = stub_->Free(&context, request, &reply);
    if (!status.ok()) {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
    }
}

int MmaClient::ReadPage(int array_id, int virtual_page_id, int offset) {
    ReadRequest request;
    ReadReply reply;
    ClientContext context;
    request.set_arrayid(array_id);
    request.set_virtualpageid(virtual_page_id);
    request.set_offset(offset);
    Status status = stub_->ReadPage(&context, request, &reply);
    if (!status.ok()) {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return 0;
    }
    return reply.value();
}

void MmaClient::WritePage(int array_id, int virtual_page_id, int offset, int value) {
    WriteRequest request;
    Null reply;
    ClientContext context;
    request.set_arrayid(array_id);
    request.set_virtualpageid(virtual_page_id);
    request.set_offset(offset);
    request.set_value(value);
    Status status = stub_->WritePage(&context, request, &reply);
    if (!status.ok()) {
        std::cout << status.error_code() << ": " << status.error_message()
                  << std::endl;
    }
}

}
