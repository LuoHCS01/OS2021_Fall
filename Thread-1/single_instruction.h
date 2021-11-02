#ifndef THREAD_LIB_SINGLE_INSTRUCTION_H
#define THREAD_LIB_SINGLE_INSTRUCTION_H
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

namespace proj1 {
void init_embedding(Instruction, EmbeddingHolder*, EmbeddingHolder*, Embedding*, int);
void update_embedding(Instruction, EmbeddingHolder*, EmbeddingHolder*);
Embedding* recommend(Instruction, EmbeddingHolder*, EmbeddingHolder*);
} // namespace proj1

#endif
