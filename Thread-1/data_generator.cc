#include<bits/stdc++.h>
#include<time.h>

#define N 2000000
#define VECTOR_SIZE 16
#define NUM_VECTORS 20
#define NUM_INSTRS 1000

using namespace std;

void create_data(string filename) {
    ofstream outfile(filename);
    for (int i = 0; i < NUM_VECTORS; i++) {
        outfile << double(rand() % N) * 2.0 / N - 1;
        for (int j = 1; j < VECTOR_SIZE; j++) {
            outfile << "," << double(rand() % N) * 2.0 / N - 1;
        }
        outfile << endl;
    }
}

void create_instructions(string filename) {
    ofstream outfile(filename);
    int epoch = -1;
    int num_users = NUM_VECTORS;
    for (int i = 0; i < NUM_INSTRS; i++) {
        int instr_order = rand() % 3;
        outfile << instr_order;
        if (instr_order == 0) {
            bool empty = true;
            do {
                int p = rand() % 20;
                for (int i = 0; i < NUM_VECTORS; i++) {
                    if (rand() % NUM_VECTORS < p) {
                        outfile << " " << i;
                        empty = false;
                    }
                }
            } while (empty);
            num_users++;
        } else if (instr_order == 1) {
            outfile << " " << rand() % num_users;
            outfile << " " << rand() % VECTOR_SIZE;
            outfile << " " << rand() % 2;
            outfile << " " << epoch + 1;
        } else {
            outfile << " " << rand() % num_users;
            outfile << " " << epoch;
            bool empty = true;
            do {
                int p = rand() % 20;
                for (int i = 0; i < NUM_VECTORS; i++) {
                    if (rand() % NUM_VECTORS < p) {
                        outfile << " " << i;
                        empty = false;
                    }
                }
            } while (empty);
        }
        if (rand() % 30 == 0) {
            epoch++;
        }
        outfile << endl;
    }
}

int main() {
    srand(time(0));
    create_data("test_data/large_users.in");
    create_data("test_data/large_items.in");
    create_instructions("test_data/large_instruction.tsv");
}
