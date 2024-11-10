#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>

#define MIN_PROCESS_SIZE 5    // Minimum process size in MB
#define MAX_PROCESS_SIZE 50   // Maximum process size in MB
#define MAX_PROCESSES 100     // Maximum number of processes

// Estructura para una página
typedef struct {
    int process_id;      // ID del proceso al que pertenece
    int page_number;     // Número de página dentro del proceso
    int in_ram;         // 1 si está en RAM, 0 si está en swap
    time_t last_access; // Último acceso a la página
} Page;

// Estructura para un proceso
typedef struct {
    int id;             // ID único del proceso
    int size;           // Tamaño en MB
    int num_pages;      // Número de páginas que usa
    int *page_table;    // Tabla de páginas (índices en el array global de páginas)
    time_t creation_time; // Tiempo de creación
    int active;         // 1 si está activo, 0 si no
} Process;

// Variables globales
int physical_memory_size;    // Tamaño de la memoria física en MB
int virtual_memory_size;     // Tamaño de la memoria virtual en MB
int page_size;              // Tamaño de página en MB
int total_pages;            // Número total de páginas disponibles
int ram_pages;              // Número de páginas en RAM
int swap_pages;             // Número de páginas en swap
Page *pages;                // Array global de páginas
Process processes[MAX_PROCESSES];  // Array de procesos
int num_processes = 0;      // Contador de procesos
int program_time = 0;       // Tiempo transcurrido en segundos
int should_exit = 0;        // Flag para terminar el programa

// Prototipos de funciones
void init_memory(void);
void create_process(void);
void terminate_random_process(void);
void access_random_address(void);
void print_memory_status(void);
int allocate_pages(Process *process);
void free_process_pages(Process *process);
int swap_page_to_ram(void);
void timer_handler(int signum);

// Inicialización de la memoria
void init_memory(void) {
    ram_pages = physical_memory_size / page_size;
    swap_pages = (virtual_memory_size - physical_memory_size) / page_size;
    total_pages = ram_pages + swap_pages;
    
    pages = calloc(total_pages, sizeof(Page));
    for (int i = 0; i < total_pages; i++) {
        pages[i].process_id = -1;
        pages[i].in_ram = (i < ram_pages) ? 1 : 0;
    }
}

// Creación de un nuevo proceso
void create_process(void) {
    if (num_processes >= MAX_PROCESSES) return;
    
    int size = (rand() % (MAX_PROCESS_SIZE - MIN_PROCESS_SIZE + 1)) + MIN_PROCESS_SIZE;
    int num_pages_needed = (size + page_size - 1) / page_size;
    
    Process *new_process = &processes[num_processes];
    new_process->id = num_processes;
    new_process->size = size;
    new_process->num_pages = num_pages_needed;
    new_process->page_table = calloc(num_pages_needed, sizeof(int));
    new_process->creation_time = time(NULL);
    new_process->active = 1;
    
    if (allocate_pages(new_process)) {
        printf("\nNuevo proceso creado - ID: %d, Tamaño: %d MB, Páginas: %d\n",
               new_process->id, size, num_pages_needed);
        num_processes++;
    } else {
        printf("\nNo hay suficiente memoria para crear proceso de %d MB\n", size);
        free(new_process->page_table);
        should_exit = 1;
    }
}

// Algoritmo LRU para reemplazo de páginas
int get_lru_page(void) {
    time_t oldest_time = time(NULL);
    int oldest_page = -1;
    
    for (int i = 0; i < ram_pages; i++) {
        if (pages[i].process_id != -1 && pages[i].last_access < oldest_time) {
            oldest_time = pages[i].last_access;
            oldest_page = i;
        }
    }
    
    return oldest_page;
}

// Intercambio de página a RAM
int swap_page_to_ram(void) {
    int lru_page = get_lru_page();
    if (lru_page == -1) return -1;
    
    // Buscar espacio en swap
    for (int i = ram_pages; i < total_pages; i++) {
        if (pages[i].process_id == -1) {
            // Intercambiar páginas
            Page temp = pages[lru_page];
            pages[lru_page] = pages[i];
            pages[i] = temp;
            
            pages[lru_page].in_ram = 1;
            pages[i].in_ram = 0;
            
            printf("Page fault: Página %d del proceso %d movida a swap\n",
                   pages[i].page_number, pages[i].process_id);
            return lru_page;
        }
    }
    
    return -1;
}

// Asignación de páginas a un proceso
int allocate_pages(Process *process) {
    int pages_allocated = 0;
    
    // Primero intentar asignar en RAM
    for (int i = 0; i < ram_pages && pages_allocated < process->num_pages; i++) {
        if (pages[i].process_id == -1) {
            pages[i].process_id = process->id;
            pages[i].page_number = pages_allocated;
            pages[i].in_ram = 1;
            pages[i].last_access = time(NULL);
            process->page_table[pages_allocated] = i;
            pages_allocated++;
        }
    }
    
    // Si no hay suficiente espacio en RAM, usar swap
    for (int i = ram_pages; pages_allocated < process->num_pages; i++) {
        if (i >= total_pages) return 0;  // No hay suficiente memoria
        
        if (pages[i].process_id == -1) {
            pages[i].process_id = process->id;
            pages[i].page_number = pages_allocated;
            pages[i].in_ram = 0;
            pages[i].last_access = time(NULL);
            process->page_table[pages_allocated] = i;
            pages_allocated++;
        }
    }
    
    return 1;
}

// Liberar páginas de un proceso
void free_process_pages(Process *process) {
    for (int i = 0; i < process->num_pages; i++) {
        int page_index = process->page_table[i];
        pages[page_index].process_id = -1;
        pages[page_index].page_number = -1;
    }
    free(process->page_table);
    process->active = 0;
}

// Terminación de un proceso aleatorio
void terminate_random_process(void) {
    int active_processes = 0;
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].active) active_processes++;
    }
    
    if (active_processes == 0) return;
    
    int target = rand() % active_processes;
    int current = 0;
    
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].active) {
            if (current == target) {
                printf("\nTerminando proceso %d\n", processes[i].id);
                free_process_pages(&processes[i]);
                break;
            }
            current++;
        }
    }
}

// Acceso a dirección virtual aleatoria
void access_random_address(void) {
    if (num_processes == 0) return;
    
    int process_id = rand() % num_processes;
    while (!processes[process_id].active) {
        process_id = rand() % num_processes;
    }
    
    Process *process = &processes[process_id];
    int page_number = rand() % process->num_pages;
    int page_index = process->page_table[page_number];
    
    printf("\nAcceso a página %d del proceso %d\n", page_number, process_id);
    
    if (!pages[page_index].in_ram) {
        printf("Page fault! Página no está en RAM\n");
        int new_ram_location = swap_page_to_ram();
        if (new_ram_location == -1) {
            printf("Error: No se puede realizar swap\n");
            should_exit = 1;
            return;
        }
        
        // Actualizar la tabla de páginas
        pages[new_ram_location] = pages[page_index];
        pages[new_ram_location].in_ram = 1;
        process->page_table[page_number] = new_ram_location;
    }
    
    pages[page_index].last_access = time(NULL);
}

// Manejador del timer
void timer_handler(int signum) {
    program_time++;
    
    if (program_time % 2 == 0) {
        create_process();
    }
    
    if (program_time >= 30) {
        if (program_time % 5 == 0) {
            terminate_random_process();
            access_random_address();
        }
    }
    
    print_memory_status();
}

// Imprimir estado de la memoria
void print_memory_status(void) {
    int ram_used = 0, swap_used = 0;
    
    for (int i = 0; i < ram_pages; i++) {
        if (pages[i].process_id != -1) ram_used++;
    }
    
    for (int i = ram_pages; i < total_pages; i++) {
        if (pages[i].process_id != -1) swap_used++;
    }
    
    printf("\n=== Estado de la Memoria ===\n");
    printf("RAM: %d/%d páginas usadas\n", ram_used, ram_pages);
    printf("Swap: %d/%d páginas usadas\n", swap_used, swap_pages);
    printf("Procesos activos: %d\n", num_processes);
}

int main(void) {
    srand(time(NULL));
    
    printf("Ingrese el tamaño de la memoria física (MB): ");
    scanf("%d", &physical_memory_size);
    
    // Memoria virtual entre 1.5 y 4.5 veces la física
    float multiplier = (rand() % 30 + 15) / 10.0;  // 1.5 a 4.5
    virtual_memory_size = (int)(physical_memory_size * multiplier);
    
    printf("Ingrese el tamaño de página (MB): ");
    scanf("%d", &page_size);
    
    printf("\nMemoria virtual: %d MB (%.1fx la memoria física)\n", 
           virtual_memory_size, multiplier);
    
    init_memory();
    
    // Configurar timer para ejecutar cada segundo
    struct itimerval timer;
    signal(SIGALRM, timer_handler);
    
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
    
    setitimer(ITIMER_REAL, &timer, NULL);
    
    // Mantener el programa corriendo hasta que deba terminar
    while (!should_exit) {
        sleep(1);
    }
    
    printf("\nSimulación terminada\n");
    free(pages);
    return 0;
}