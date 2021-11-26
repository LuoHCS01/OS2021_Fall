#include "array_list.h"

#include "memory_manager.h"

namespace proj3 {

    // address translation, return <vid, offset>
    std::pair<int, int> address(int i) {
        std::pair<int, int> ad;
        ad.first = i / PageSize;
        ad.second = i % PageSize;
        return ad;
    }

    ArrayList::ArrayList(size_t sz, MemoryManager* cur_mma, int id){
        size = sz;
        mma = cur_mma;
        array_id = id;
    }
    int ArrayList::Read (unsigned long idx){
        //read the value in the virtual index of 'idx' from mma's memory space
        std::pair<int, int> ad = address(idx);
        return mma->ReadPage(array_id, ad.first, ad.second);
    }
    void ArrayList::Write (unsigned long idx, int value){
        //write 'value' in the virtual index of 'idx' into mma's memory space
        std::pair<int, int> ad = address(idx);
        mma->WritePage(array_id, ad.first, ad.second, value);
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    ArrayList::~ArrayList(){
    }
} // namespce: proj3
