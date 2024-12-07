#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define size_F 50  // 500MB
#define swap_size 100 // 100MB
int *memory_pages;   // Arreglo para páginas en RAM
int *swap_pages;     // Arreglo para páginas en Swap
int active_processes = 0;

typedef struct {
    pthread_t thread_id;
    int size;
    int num_pages;
    int pos;
} Proceso;

typedef struct {
    int sizeF;
    int sizeV;
    int PageS;
    int Apage;
} Memoria;

Memoria memoria;

// Prototipos
int find_contiguous_space(int num_pages, int total_pages, int *pages);
int allocate_pages_to_process(int process_id, int num_pages, int total_pages, int *pages);
void release_pages(int process_id, int total_pages, int *pages);
void print_memory_pages(int N, int *pages);
void simulate_virtual_access();

// Inicializar RAM y Swap
void initialize_memory_pages(int Npages, int **pages) {
    *pages = malloc(Npages * sizeof(int));
    if (*pages == NULL) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < Npages; i++) {
        (*pages)[i] = -1; // Todas las páginas están libres
    }
}
float Nrand(void);

// Implementación de Nrand
float Nrand(void) {
    float min = 1.5, max = 4.5;
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

int find_contiguous_space(int num_pages, int total_pages, int *pages) {
    int start = -1, count = 0;
    for (int i = 0; i < total_pages; i++) {
        if (pages[i] == -1) {
            if (count == 0) start = i;
            count++;
            if (count == num_pages) return start;
        } else {
            count = 0;
        }
    }
    return -1;
}

int allocate_pages_to_process(int process_id, int num_pages, int total_pages, int *pages) {
    int start = find_contiguous_space(num_pages, total_pages, pages);
    if (start != -1) {
        for (int i = start; i < start + num_pages; i++) {
            pages[i] = process_id;
        }
        return 1;
    }
    return 0;
}

void release_pages(int process_id, int total_pages, int *pages) {
    for (int i = 0; i < total_pages; i++) {
        if (pages[i] == process_id) {
            pages[i] = -1; // Liberar la página
        }
    }
}

void print_memory_pages(int N, int *pages) {
    printf("Estado de las páginas:\n");
    for (int i = 0; i < N; i++) {
        printf("Página %d: %s\n", i, (pages[i] == -1) ? "Libre" : "Ocupada");
    }
}

void simulate_virtual_access() {
    int virtual_address = rand() % (memoria.sizeV * 1024); // Dirección en KB
    int page_number = virtual_address / (memoria.PageS * 1024); // Página virtual
    printf("Acceso a dirección virtual: %d KB, página: %d\n", virtual_address, page_number);

    if (page_number >= memoria.Apage) {
        printf("Page Fault: Página %d no está en RAM. Simulando swap.\n", page_number);
        // Implementar política de reemplazo
        for (int i = 0; i < memoria.Apage; i++) {
            if (memory_pages[i] != -1) {
                printf("Reemplazando página %d.\n", i);
                memory_pages[i] = -1; // Liberar espacio en RAM
                swap_pages[i % swap_size] = page_number; // Mover a swap
                break;
            }
        }
    }
}

void *thread_task(void *arg) {
    Proceso *proceso = (Proceso *)arg;
    printf("Thread ID: %p ejecutando proceso.\n", (void *)proceso->thread_id);
    printf("Tamaño: %d MB, Páginas requeridas: %d\n", proceso->size, proceso->num_pages);

    if (allocate_pages_to_process(proceso->pos, proceso->num_pages, memoria.Apage, memory_pages)) {
        printf("Proceso %d asignado a memoria.\n", proceso->pos);
        print_memory_pages(memoria.Apage, memory_pages);
        sleep(2);
        release_pages(proceso->pos, memoria.Apage, memory_pages);
        printf("Memoria después de liberar las páginas del proceso %d:\n", proceso->pos);
        print_memory_pages(memoria.Apage, memory_pages);
    } else {
        printf("No hay suficiente espacio contiguo para el proceso %d.\n", proceso->pos);
    }

    free(proceso);
    return NULL;
}

int main(void) {
    srand((unsigned int)time(NULL));
    memoria.sizeF = size_F;
    memoria.sizeV = size_F * Nrand();
    memoria.PageS = 10; // 10MB
    memoria.Apage = memoria.sizeF / memoria.PageS;

    initialize_memory_pages(memoria.Apage, &memory_pages);
    initialize_memory_pages(swap_size / memoria.PageS, &swap_pages);

    int countT = 0;
    time_t start_time = time(NULL);

    while (1) {
        time_t current_time = time(NULL);
        if (current_time - start_time >= 30) {
            if ((current_time - start_time) % 5 == 0) {
                // Finalizar un proceso aleatorio
                int random_process = rand() % active_processes + 1;
                printf("Finalizando proceso aleatorio: %d\n", random_process);
                release_pages(random_process, memoria.Apage, memory_pages);
                print_memory_pages(memoria.Apage, memory_pages);

                // Acceso a dirección virtual
                simulate_virtual_access();
            }
        }

        // Crear un nuevo proceso
        countT++;
        Proceso *proceso = malloc(sizeof(Proceso));
        proceso->size = rand() % 100 + 1;
        proceso->num_pages = (proceso->size + memoria.PageS - 1) / memoria.PageS;
        proceso->pos = countT;
        active_processes++;

        pthread_create(&proceso->thread_id, NULL, thread_task, proceso);
        pthread_detach(proceso->thread_id);

        sleep(1);

        if (active_processes > memoria.Apage && active_processes > swap_size / memoria.PageS) {
            printf("Memoria llena. Terminando simulación.\n");
            break;
        }
    }

    free(memory_pages);
    free(swap_pages);
    return 0;
}