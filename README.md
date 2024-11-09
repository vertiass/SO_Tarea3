# Tarea 3 Sistemas Operativos #

## Enunciado ##

Uno de los conceptos clave que hemos estudiado este semestre es la paginación, una técnica utilizada por los sistemas operativos para gestionar la memoria de una máquina. Esta técnica permite dividir la memoria en pequeños bloques llamados páginas, lo que facilita la asignación de memoria a los procesos de manera eficiente. En esta tarea, se llevará a cabo la simulación de este mecanismo siguiendo las siguientes especificaciones:

1. Se deberá ingresar por input el tamaño de la memoria física (en MB). El tamaño de la memoria virtual corresponderá a un valor (aleatorio), de 1.5 a 4.5 veces la memoria física. Además, se deberá también ingresar el tamaño de cada página.

2. Se deberán crear (solo a nivel lógico) procesos de distinto tamaño, esto cada dos segundos. El tamaño de estos procesos deberá definirse dentro de un rango establecido como parámetro del programa, y cada proceso utilizará una serie de páginas de la memoria. Si no hay memoria RAM suficiente, las páginas asociadas al proceso deberán ser alojadas en la memoria swap. En caso de que no quede más memoria swap disponible el programa deberá terminar. Debe existir en todo momento seguimiento a las páginas asociadas a los procesos y su ubicación en la memoria física.

3. A partir de los primeros treinta segundos de ejecución del programa, deberán comenzar a ocurrir dos cosas de forma periódica. En primer lugar, cada cinco segundos, un proceso aleatorio deberá finalizar su ejecución. En segundo lugar, y también cada 5 segundos, se deberá simular el acceso a una dirección virtual, generada también de manera aleatoria, y mostrada en pantalla. El programa deberá localizar la página correspondiente, y en caso de no encontrarla en RAM se deberá generar un page fault. Se deberá simular el proceso de swap, para lo cual se podrá emplear cualquier política de reemplazo de páginas, que deberá ser explicitada claramente.

4. En caso de que no exista memoria disponible en swap o en la ram, se deberá finalizar el proceso de simulación y terminar el programa.
