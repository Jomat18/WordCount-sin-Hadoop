Implementación del problema de WordCount en multithread, siguiendo la idea de Hadoop Map Reduce
=========================

Resolviendo el desafío de contar la frecuencia de palabras en un archivo enorme.

Suponga que tiene un archivo de texto de 100 GB y una maquina Linux con 7 GB de RAM y 4 núcleos.
Hacer un programa que genere un archivo que enumere la frecuencia de todas las palabras
en el archivo (palabra, frecuencia).

NOTA: Considere que el archivo puede no caber en la memoria.

Esta solución utiliza una estructura de datos unordered_map y múltiples POSIX Threads para realizar la tarea de conteo de palabras. 

Los pasos realizados para la solución son los siguientes:

1) Calcular el numero de Bloques iguales, en este caso se considero el tamaño del Bloque de 1GB. Cabe mencionar que el tamaño del Bloque se recalculara debido a que puede dividir una palabra, para lo cual se ira moviendo hasta encontrar un espacio en blanco, salto de linea o simbolo de puntuación.
2) Calcular el numero de iteraciones de 4 threads cada una:
3) Para cada Thead extender el tamaño del Bloque por si se cae en medio de una palabra, para luego:
- Leer el Bloque 
- Extraer solo las palabras.
- Guardar las palabras y sus frecuencias en un hash table (unordered_map) del Thread
- Escribir las palabras y sus frecuencias en un hash table global, liberando la memoria del hash table del Thread
4) Escribir los resultados en result.txt

-------------------------------------------

## Resultados de 7,1 GB:

![Resultados](test_1.png?raw=true)

## Resultados de 10,6 GB:

![Resultados](test_2.png?raw=true)