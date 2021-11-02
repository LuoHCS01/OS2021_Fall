#include "all_instructions.h"

int main(int argc, char *argv[]) {
    proj1::EmbeddingHolder* users = new proj1::EmbeddingHolder("data/q4.in");
    proj1::EmbeddingHolder* items = new proj1::EmbeddingHolder("data/q4.in");
    proj1::Instructions instructions = proj1::read_instructrions("data/q4_instruction.tsv");
    
    {
    proj1::AutoTimer timer("q4");  // using this to print out timing of the block
    // Run all the instructions
    run_all_instructions(instructions, users, items, true);
    }

    // Write the result
    // users->write_to_stdout();
    // items->write_to_stdout();

    return 0;
}
