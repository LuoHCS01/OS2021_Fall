#include "all_instructions.h"

int main(int argc, char *argv[]) {
    proj1::EmbeddingHolder* users = new proj1::EmbeddingHolder("data/q2.in");
    proj1::EmbeddingHolder* items = new proj1::EmbeddingHolder("data/q2.in");
    proj1::Instructions instructions = proj1::read_instructrions("data/q2_instruction.tsv");
    
    {
    proj1::AutoTimer timer("q2");  // using this to print out timing of the block
    // Run all the instructions
    run_all_instructions(instructions, users, items, false);
    }

    // Write the result
    users->write_to_stdout();
    // items->write_to_stdout();

    return 0;
}
