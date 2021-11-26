#include "array_list.h"
#include "memory_manager.h"

namespace proj3 {
    // we store the page into disk by one page per file, named <holder>_<vid>.in
    std::string disk_filename(int holder, int vid) {
        std::string filename = "disk/" + std::to_string(holder) + "_" + std::to_string(vid) + ".in";
        return filename;
    }

    // implementation of bitmap
    void modify(unsigned int* free_list, unsigned int p, unsigned int v) {
        // free_list[p] = v
        if (v) {
            free_list[p >> 5] |= 1u << (p & 31);
        } else {
            free_list[p >> 5] &= ~(1u << (p & 31));
        }
    }

    unsigned int find(unsigned int* free_list) {
        // find the first p with free_list[p] = 1
        // if there is nothing available, return sz
        for (int i = 0; ; i++) {
            if (free_list[i]) {
                return (i << 5) + __builtin_ctz(free_list[i]);
            }
        }
    }

    PageFrame::PageFrame() {
        for (int i = 0; i < PageSize; i++) {
            mem[i] = 0;
        }
    }

    int& PageFrame::operator[](unsigned long idx) {
        // each page should provide random access like an array
        return mem[idx];
    }

    void PageFrame::WriteDisk(std::string filename) {
        // write page content into disk files
        std::ofstream oFile;
        oFile.open(filename);
        for (int i = 0; i < PageSize; i++) {
            oFile << mem[i];
            if (i != PageSize - 1) oFile << std::endl;
        }
        oFile.close();
    }

    void PageFrame::ReadDisk(std::string filename) {
        // read page content from disk files
        std::ifstream inFile;
        inFile.open(filename);
        std::string value;
        int i = 0;
        if (inFile.is_open()) {
            while (std::getline(inFile, value)) {
                mem[i] = std::atoi(value.c_str());
                i++;
            }
        }
        inFile.close();
    }

    PageInfo::PageInfo() {
        holder = -1;
        virtual_page_id = -1;
    }

    void PageInfo::SetInfo(int cur_holder, int cur_vid) {
        // modify the page states
        // you can add extra parameters if needed
        holder = cur_holder;
        virtual_page_id = cur_vid;
        used = false;
    }

    void PageInfo::ClearInfo() {
        // clear the page states
        // you can add extra parameters if needed
        holder = -1;
        virtual_page_id = -1;
        used = false;
    }

    int PageInfo::GetHolder() {
        return holder;
    }
    
    int PageInfo::GetVid() {
        return virtual_page_id;
    }

    void PageInfo::SetUsed(bool state) {
        used = state;
    }
    
    bool PageInfo::GetUsed() {
        return used;
    }

    MemoryManager::MemoryManager(size_t sz) {
        // mma should build its memory space with given space size
        // you should not allocate larger space than 'sz' (the number of physical pages) 
        
        // --------------- Replace Policy -----------------
        // replace_policy = Random;
        // replace_policy = FIFO;
        replace_policy = ClockLRU;
        // ------------------------------------------------

        next_array_id = 0;
        ptr_page = 0;
        mma_sz = sz;
        mem = new PageFrame[sz];
        page_info = new PageInfo[sz];
        free_list = new unsigned int[(sz >> 5) + 1];   // 1 for free
        for (size_t i = 0; i < (sz >> 5) + 1; i++) {
            free_list[i] = ~0u;
        }
        system("mkdir disk");
    }

    MemoryManager::~MemoryManager() {
        delete []mem;
        delete []page_info;
        delete []free_list;
        system("rm -rf disk");
    }

    void MemoryManager::PageOut(int physical_page_id) {
        // swap out the physical page with the indx of 'physical_page_id out' into a disk file
        int holder = page_info[physical_page_id].GetHolder();
        int vid = page_info[physical_page_id].GetVid();
        if (holder != -1) {
            mem[physical_page_id].WriteDisk(disk_filename(holder, vid));
        }
        page_info[physical_page_id].ClearInfo();
        page_map[holder][vid] = -1;
    }

    void MemoryManager::PageIn(int array_id, int virtual_page_id, int physical_page_id) {
        // swap the target page from the disk file into a physical page with the index of 'physical_page_id out'
        mem[physical_page_id] = PageFrame();
        mem[physical_page_id].ReadDisk(disk_filename(array_id, virtual_page_id));
        page_info[physical_page_id].SetInfo(array_id, virtual_page_id);
        page_map[array_id][virtual_page_id] = physical_page_id;
    }

    void MemoryManager::PageReplace(int array_id, int virtual_page_id) {
        // implement your page replacement policy here
        unsigned int f = find(free_list);
        if (f < mma_sz) {
            PageIn(array_id, virtual_page_id, f);
            modify(free_list, f, 0);
        } else {
            if (replace_policy == Random) {
                int page_out_id = rand() % mma_sz;
                PageOut(page_out_id);
                PageIn(array_id, virtual_page_id, page_out_id);
            }
            if (replace_policy == FIFO) {
                PageOut(ptr_page);
                PageIn(array_id, virtual_page_id, ptr_page);
                ptr_page = (ptr_page + 1) % mma_sz;
            }
            if (replace_policy == ClockLRU) {
                while (page_info[ptr_page].GetUsed()) {
                    page_info[ptr_page].SetUsed(false);
                    ptr_page = (ptr_page + 1) % mma_sz;
                }
                PageOut(ptr_page);
                PageIn(array_id, virtual_page_id, ptr_page);
                ptr_page = (ptr_page + 1) % mma_sz;
            }
        }
    }

    int MemoryManager::ReadPage(int array_id, int virtual_page_id, int offset) {
        // for arrayList of 'array_id', return the target value on its virtual space
        mux.lock();
        // std::cout << "Array " << array_id << " read in vid " << virtual_page_id << " offset " << offset << std::endl;
        int physical_page_id = page_map[array_id][virtual_page_id];
        if (physical_page_id == -1) {
            PageReplace(array_id, virtual_page_id);
        } else if (page_info[physical_page_id].GetHolder() != array_id || page_info[physical_page_id].GetVid() != virtual_page_id) {
            PageReplace(array_id, virtual_page_id);
        } else {
            page_info[physical_page_id].SetUsed(true);
        }
        physical_page_id = page_map[array_id][virtual_page_id];
        int result = mem[physical_page_id][offset];
        mux.unlock();
        return result;
    }

    void MemoryManager::WritePage(int array_id, int virtual_page_id, int offset, int value) {
        // for arrayList of 'array_id', write 'value' into the target position on its virtual space
        mux.lock();
        // std::cout << "Array " << array_id << " write " << value << " to vid " << virtual_page_id << " offset " << offset << std::endl;
        int physical_page_id = page_map[array_id][virtual_page_id];
        if (physical_page_id == -1) {
            PageReplace(array_id, virtual_page_id);
        } else if (page_info[physical_page_id].GetHolder() != array_id || page_info[physical_page_id].GetVid() != virtual_page_id) {
            PageReplace(array_id, virtual_page_id);
        } else {
            page_info[physical_page_id].SetUsed(true);
        }
        physical_page_id = page_map[array_id][virtual_page_id];
        mem[physical_page_id][offset] = value;
        mux.unlock();
    }

    ArrayList* MemoryManager::Allocate(size_t sz) {
        // when an application requires for memory, create an ArrayList and record mappings from its virtual memory space to the physical memory space
        mux.lock();
        // std::cout << "Allocate memory space for array id " << next_array_id << " with size " << sz << std::endl;
        ArrayList* arr = new ArrayList(sz, this, next_array_id);
        std::map<int, int> array_page_map;
        // not allocated physical page index is -1
        for (size_t i = 0; i < (sz + PageSize - 1) / PageSize; i++) {
            array_page_map[i] = -1;
        }
        page_map[next_array_id] = array_page_map;
        next_array_id++;
        mux.unlock();
        return arr;
    }

    void MemoryManager::Release(ArrayList* arr) {
        // an application will call release() function when destroying its arrayList
        // release the virtual space of the arrayList and erase the corresponding mappings
        mux.lock();
        for (size_t i = 0; i < mma_sz; i++) {
            if (page_info[i].GetHolder() == arr->array_id) {
                page_info[i].ClearInfo();
                mem[i] = PageFrame();
                modify(free_list, i, 1);
            }
        }
        page_map.erase(arr->array_id);
        mux.unlock();
        for (size_t i = 0; i < (arr->size + PageSize - 1) / PageSize; i++) {
            std::remove(disk_filename(arr->array_id, i).c_str());
        }
    }
} // namespce: proj3
