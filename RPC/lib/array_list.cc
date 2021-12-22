#include "array_list.h"
#include "mma_client.h"

namespace proj4 {
    // address translation, return <vid, offset>
    std::pair<int, int> address(int i) {
        std::pair<int, int> ad;
        ad.first = i / PageSize;
        ad.second = i % PageSize;
        return ad;
    }

    ArrayList::ArrayList(size_t sz, MmaClient* cur_mma, int id) {
        size = sz;
        mma = cur_mma;
        array_id = id;
    }

    int ArrayList::Read(unsigned long idx) {
        //read the value in the virtual index of 'idx' from mma's memory space
        std::pair<int, int> ad = address(idx);
        return mma->ReadPage(array_id, ad.first, ad.second);
    }

    void ArrayList::Write(unsigned long idx, int value) {
        //write 'value' in the virtual index of 'idx' into mma's memory space
        std::pair<int, int> ad = address(idx);
        mma->WritePage(array_id, ad.first, ad.second, value);
    }

    ArrayList::~ArrayList() {
        mma->Free(this);
    }
} // namespce: proj3
