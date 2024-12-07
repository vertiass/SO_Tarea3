# Tarea 3 Sistemas Operativos #

## Enunciado ##

Uno de los conceptos clave que hemos estudiado este semestre es la paginación, una técnica utilizada por los sistemas operativos para gestionar la memoria de una máquina. Esta técnica permite dividir la memoria en pequeños bloques llamados páginas, lo que facilita la asignación de memoria a los procesos de manera eficiente. En esta tarea, se llevará a cabo la simulación de este mecanismo siguiendo las siguientes especificaciones:

1. Se deberá ingresar por input el tamaño de la memoria física (en MB). El tamaño de la memoria virtual corresponderá a un valor (aleatorio), de 1.5 a 4.5 veces la memoria física. Además, se deberá también ingresar el tamaño de cada página.

2. Se deberán crear (solo a nivel lógico) procesos de distinto tamaño, esto cada dos segundos. El tamaño de estos procesos deberá definirse dentro de un rango establecido como parámetro del programa, y cada proceso utilizará una serie de páginas de la memoria. Si no hay memoria RAM suficiente, las páginas asociadas al proceso deberán ser alojadas en la memoria swap. En caso de que no quede más memoria swap disponible el programa deberá terminar. Debe existir en todo momento seguimiento a las páginas asociadas a los procesos y su ubicación en la memoria física.

3. A partir de los primeros treinta segundos de ejecución del programa, deberán comenzar a ocurrir dos cosas de forma periódica. En primer lugar, cada cinco segundos, un proceso aleatorio deberá finalizar su ejecución. En segundo lugar, y también cada 5 segundos, se deberá simular el acceso a una dirección virtual, generada también de manera aleatoria, y mostrada en pantalla. El programa deberá localizar la página correspondiente, y en caso de no encontrarla en RAM se deberá generar un page fault. Se deberá simular el proceso de swap, para lo cual se podrá emplear cualquier política de reemplazo de páginas, que deberá ser explicitada claramente.

4. En caso de que no exista memoria disponible en swap o en la ram, se deberá finalizar el proceso de simulación y terminar el programa.


## Implementación ##

El programa simula la gestión de memoria física y virtual mediante un algoritmo de reemplazo de páginas Least Recently Used (LRU), que permite mantener un seguimiento dinámico del uso de memoria, optimizando la asignación de recursos. El proceso inicia con la configuración de memoria por parte del usuario, quien define el tamaño de memoria física y el tamaño de página, generándose automáticamente una memoria virtual entre 1.5 y 4.5 veces el tamaño de la memoria física. Durante la ejecución, el programa crea procesos cada dos segundos con tamaños aleatorios, asignándoles páginas en memoria RAM y, si es necesario, en memoria swap, implementando un mecanismo de page fault que permite intercambiar páginas cuando la RAM se encuentra saturada.

La política de reemplazo LRU garantiza que las páginas menos recientemente utilizadas sean candidatas para ser movidas a memoria swap, manteniendo en RAM las páginas con accesos más recientes. El programa monitorea constantemente el estado de la memoria, mostrando información detallada sobre el uso de páginas en RAM y swap, y termina su ejecución cuando no existen recursos de memoria disponibles, proporcionando un mensaje descriptivo del estado final del sistema.

Para la correr del código, ejecutar lo siguiente:

1. Compilar el código

```bash
gcc -o simulacion_memoria simulacion_memoria.c
```

2. Ejecutar el programa

```bash
./simulacion_memoria
```

### Datos de Prueba ###
1. Tamaño Pequeño
  - Memoria física: 100 MB
  - Tamaño de página: 4 MB
  - Memoria virtual esperada: 150 - 450 MB
2. Tamaño Mediano
  - Memoria física: 256 MB
  - Tamaño de página: 8 MB
  - Memoria virtual esperada: 384 - 1152 MB
3. Tamaño Grande
  - Memoria física: 512 MB
  - Tamaño de página: 16 MB
  - Memoria virtual esperada: 768 - 2304 MB
