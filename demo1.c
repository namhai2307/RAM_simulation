#include <stdio.h>
#include <stdlib.h>

#define RAM_SIZE 16
#define VIRTUAL_MEM_SIZE 32
#define PAGE_SIZE 2
#define NUM_PROCESSES 4
#define PAGES_PER_PROCESS 4

typedef struct {
    int process_id;
    int page_num;
    int last_accessed;
} memory;

memory *RAM[RAM_SIZE];
memory *virtual_memory[VIRTUAL_MEM_SIZE];
int page_tables[NUM_PROCESSES][PAGES_PER_PROCESS];

int time_step = 0;

void initialize_virtual_memory() {
    for (int process = 0; process < NUM_PROCESSES; ++process) {
        for (int page = 0; page < PAGES_PER_PROCESS; ++page) {
            memory *page_entry = (memory *) malloc(sizeof(memory));
            page_entry->process_id = process;
            page_entry->page_num = page;
            page_entry->last_accessed = 0;
            virtual_memory[process * PAGES_PER_PROCESS + page] = page_entry;
        }
    }
}

void print_page_tables() {
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        for (int j = 0; j < PAGES_PER_PROCESS; ++j) {
            printf("%d ", page_tables[i][j]);
        }
        printf("\n");
    }
}

void print_ram() {
    for (int i = 0; i < RAM_SIZE; i += 2) {
        if (RAM[i]) {
            printf("%d,%d,%d; ", RAM[i]->process_id, RAM[i]->page_num, RAM[i]->last_accessed);
        } else {
            printf("empty; ");
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    FILE *in_file = fopen(argv[1], "r");
    if (in_file == NULL) {
        fprintf(stderr, "!Error: Cannot open file %s\n", argv[1]);
        return 1;
    }

    // Initialize virtual memory and page tables
    initialize_virtual_memory();

    // TODO: Implement simulation logic here, including LRU algorithm

    // Print results
    print_page_tables();
    print_ram();

    return 0;
}
