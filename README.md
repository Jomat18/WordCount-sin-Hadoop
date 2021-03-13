Implementación del problema de WordCount en multithread
=========================

Resolviendo el desafío de contar la frecuencia de palabras en un archivo enorme.

Suponga que tiene un archivo de texto de 100 GB y una maquina Linux con 7 GB de RAM y 4 núcleos.
Hacer un programa que genere un archivo que enumere la frecuencia de todas las palabras
en el archivo (palabra, frecuencia).

NOTA: Considere que el archivo puede no caber en la memoria.

Esta solución utiliza una estructura de datos unordered_map y múltiples POSIX Threads para realizar la tarea de conteo de palabras. 

Los pasos realizados para la solución son los siguientes:

1) Calcular el numero de Bloques iguales. Se considero el tamaño de los Bloques iniciales en 1GB. Cabe mencionar que el tamaño del Bloque se recalculara en las iteraciones debido a que se corre el riesgo de dividir una palabra, para lo cual nos iremos desplazando hasta encontrar un espacio en blanco, un salto de linea o un simbolo de puntuación.
2) Calcular el numero de iteraciones de 4 threads cada una
3) Para cada Thead extendemos el tamaño del Bloque como se menciono antes para luego:
- Leer el Bloque 
- Extraer solo las palabras obviando simbolos de puntuacion, numeros, etc.
- Guardar las palabras y sus frecuencias en un hash table (unordered_map) del Thread
- Escribir las palabras y sus frecuencias en un hash table global, liberando la memoria del hash table del Thread
4) Finalmente escribir los resultados en result.txt

-------------------------------------------

## Resultados con un archivo de 7.1 GB:

![Resultados](test_1.png?raw=true)

## Resultados con un archivo de 10.6 GB tomandose en calcular la frecuencia de palabras un tiempo de 1 minuto con 59 segundos:

![Resultados](test_2.png?raw=true)

Observación: Se puede mejorar el tiempo y el almacenaniento en memoria usando una estructura de datos "Trie"