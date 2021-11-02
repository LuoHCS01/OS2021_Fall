#ifndef THREAD_LIB_ALL_INSTRUCTIONS_H
#define THREAD_LIB_ALL_INSTRUCTIONS_H
#include <algorithm>
#include <chrono>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include "lib/embedding.h" 
#include "lib/instruction.h"
#include "lib/model.h" 
#include "lib/utils.h"
#include "single_instruction.h"

void run_all_instructions(proj1::Instructions, proj1::EmbeddingHolder*, proj1::EmbeddingHolder*, bool);

#endif
