#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RAM_SIZE 16
#define VIRTUAL_MEM_SIZE 32
#define NUM_PROCESSES 4
#define PAGES_PER_PROCESS 4
#define FRAME_SIZE 2
#define MAX_TIME 99999

// Structure to represent a page in memory
typedef struct {
    int process_id;
    int page_num;
    int last_accessed;  // Timestamp when the page was last accessed
} Page;

// Arrays to represent RAM and virtual memory
Page *RAM[RAM_SIZE];
Page *virtual_mem[VIRTUAL_MEM_SIZE];

// Page tables for each process (99 means page is in virtual memory)
int page_table[NUM_PROCESSES][PAGES_PER_PROCESS];

// Time simulation step
int time_step = 0;

// Function to initialize virtual memory
void initialize_virtual_memory() {
    for (int process = 0; process < NUM_PROCESSES; process++) {
        for (int page = 0; page < PAGES_PER_PROCESS; page++) {
            for (int i = 0; i < FRAME_SIZE; i++) {
                int index = process * PAGES_PER_PROCESS * FRAME_SIZE + page * FRAME_SIZE + i;
                virtual_mem[index] = (Page *)malloc(sizeof(Page));
                virtual_mem[index]->process_id = process;
                virtual_mem[index]->page_num = page;
                virtual_mem[index]->last_accessed = 0;  // Initialize last_accessed to 0
            }
            // Initialize page table to 99 (all pages initially in virtual memory)
            page_table[process][page] = 99;
        }
    }
}

// Function to find an empty frame in RAM
int find_empty_frame() {
    for (int i = 0; i < RAM_SIZE; i += FRAME_SIZE) {
        if (RAM[i] == NULL) {
            return i;
        }
    }
    return -1;  // No empty frame
}

// Function to evict the least recently used page of a process
int evict_lru_page(int process_id) {
    int lru_index = -1;
    int lru_time = MAX_TIME;

    // Look for the least recently used page of the same process
    for (int i = 0; i < RAM_SIZE; i += FRAME_SIZE) {
        if (RAM[i] != NULL && RAM[i]->process_id == process_id) {
            if (RAM[i]->last_accessed < lru_time) {
                lru_time = RAM[i]->last_accessed;
                lru_index = i;
            }
        }
    }

    // If no page of the process is found, return -1
    return lru_index;
}

// Function to evict the global least recently used page if no local LRU page is found
int evict_global_lru_page() {
    int lru_index = -1;
    int lru_time = MAX_TIME;

    // Look for the least recently used page across all processes
    for (int i = 0; i < RAM_SIZE; i += FRAME_SIZE) {
        if (RAM[i] != NULL && RAM[i]->last_accessed < lru_time) {
            lru_time = RAM[i]->last_accessed;
            lru_index = i;
        }
    }

    return lru_index;
}

// Function to bring a page from virtual memory to RAM
void bring_page_to_ram(int process_id, int page_num) {
    int frame_index = find_empty_frame();

    if (frame_index == -1) {
        // No empty frame, apply local or global LRU policy
        frame_index = evict_lru_page(process_id);

        if (frame_index == -1) {
            frame_index = evict_global_lru_page();
        }
    }

    // Evict the page and update the page table
    if (RAM[frame_index] != NULL) {
        page_table[RAM[frame_index]->process_id][RAM[frame_index]->page_num] = 99;  // Move evicted page to virtual memory
    }

    // Bring the new page from virtual memory to RAM
    for (int i = 0; i < FRAME_SIZE; i++) {
        int virtual_index = process_id * PAGES_PER_PROCESS * FRAME_SIZE + page_num * FRAME_SIZE + i;
        RAM[frame_index + i] = virtual_mem[virtual_index];
        RAM[frame_index + i]->last_accessed = time_step;
    }

    // Update the page table
    page_table[process_id][page_num] = frame_index / FRAME_SIZE;
}

// Function to simulate the memory accesses from a file
void simulate_memory_access(const char *input_file) {
    FILE *file = fopen(input_file, "r");
    int process_id;

    while (fscanf(file, "%d", &process_id) != EOF) {
        int page_num = time_step % PAGES_PER_PROCESS;  
        bring_page_to_ram(process_id, page_num);
        time_step++;
    }

    fclose(file);
}

// Function to print the page tables and RAM content to an output file
void print_output(const char *output_file) {
    FILE *file = fopen(output_file, "w");

    // Print the page tables for each process
    for (int process = 0; process < NUM_PROCESSES; process++) {
        for (int page = 0; page < PAGES_PER_PROCESS; page++) {
            fprintf(file, "%d", page_table[process][page]);
            if (page < PAGES_PER_PROCESS - 1) {
                fprintf(file, ", ");
            }
        }
        fprintf(file, "\n");
    }

    // Print the RAM content
    for (int i = 0; i < RAM_SIZE; i += FRAME_SIZE) {
        if (RAM[i] != NULL) {
            fprintf(file, "%d, %d, %d; ", RAM[i]->process_id, RAM[i]->page_num, RAM[i]->last_accessed);
            fprintf(file, "%d, %d, %d; ", RAM[i]->process_id, RAM[i]->page_num, RAM[i]->last_accessed);
        } else {
            fprintf(file, "fuckyou; ");
        }
    }

    fclose(file);
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    // Initialize virtual memory
    initialize_virtual_memory();

    // Simulate the memory accesses from input file
    simulate_memory_access(argv[1]);

    // Print the output to the output file
    print_output(argv[2]);

    return 0;
}
