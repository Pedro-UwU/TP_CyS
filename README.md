# TP_CyS

## Integrantes

| Nombre | Legajo | Correo electrónico |
| :--- | ---: | :--- |
| Pedro Jeremías López Guzmán | 60711 | [pelopez@itba.edu.ar](mailto:pelopez@itba.edu.ar) |
| Lucía Digon | 59030 | [ldigon@itba.edu.ar](mailto:ldigon@itba.edu.ar) |
| Eduardo Federico Madero Torres | 59494 | [emadero@itba.edu.ar](mailto:emadero@itba.edu.ar) |
| Martín E. Zahnd | 60401 | [mzahnd@itba.edu.ar](mailto:mzahnd@itba.edu.ar) |

## Ejecución

## Compilación

```bash
make all
```

## Ejecución

### Archivos sin encripción
#### Ocultar mensaje
```bash
./stegobmp -embed -in <archivo_a_ocultar> -p <bmp_portador> -out <bmp_salida> -steg <algoritmo>
```
Donde `<algoritmo>` puede ser cualquiera de los algoritmos de esteganografiado:
`LSB1`, `LSB4`, `LSBI`.

Ejemplo:
```bash
echo "Hello World!" > hide_me.txt
./stegobmp -embed -in "./hide_me.txt" -p "./assets/sample.bmp" -out "./sample_with_message.bmp" -steg LSB1
```

#### Leer mensaje oculto
```bash
./stegobmp -extract -p <bmp_con_mensaje> -out <archivo_escondido> -steg <algoritmo>
```
Donde `<algoritmo>` puede ser cualquiera de los algoritmos de esteganografiado:
`LSB1`, `LSB4`, `LSBI`.

Ejemplo:
```bash
./stegobmp -extract -p "./sample_with_message.bmp" -out "./i_was_hidden" -steg LSB1
# Verificamos que sea idéntico
sha256sum "./hide_me.txt"
sha256sum "./i_was_hidden.txt"
```

### Archivos con encripción
#### Ocultar mensaje
```bash
./stegobmp -embed -in <archivo_a_ocultar> -p <bmp_portador> -out <bmp_salida> -steg <algoritmo> [-a <algoritmo_encripcion>] [-m <modo>] -pass <password>
```
Donde:
- `<algoritmo>` puede ser cualquiera de los algoritmos de esteganografiado:
`LSB1`, `LSB4`, `LSBI`.
- `<algoritmo_encripcion>` puede ser `aes128`, `aes192`, `aes256`, `3des`. Si se omite, se asume `aes128`.
- `<modo>` puede ser `ecb`, `cfb`, `ofb`,`cbc`. Si se omite, se asume `cbc`.
- `<password>` es la clave de encripción.

Ejemplo:
```bash
echo "Hello World!" > hide_me.txt
./stegobmp -embed -in "./hide_me.txt" -p "./assets/sample.bmp" -out "./sample_with_message.bmp" -steg LSB1 -a aes256 -m ofb -pass "Arthur Scherbius"
```

#### Leer mensaje oculto
```bash
./stegobmp -extract -p <bmp_con_mensaje> -out <archivo_escondido> -steg <algoritmo> [-a <algoritmo_encripcion>] [-m <modo>] -pass <password>
```
Donde:
- `<algoritmo>` puede ser cualquiera de los algoritmos de esteganografiado:
`LSB1`, `LSB4`, `LSBI`.
- `<algoritmo_encripcion>` puede ser `aes128`, `aes192`, `aes256`, `3des`. Si se omite, se asume `aes128`.
- `<modo>` puede ser `ecb`, `cfb`, `ofb`,`cbc`. Si se omite, se asume `cbc`.
- `<password>` es la clave de encripción.

Ejemplo:
```bash
./stegobmp -extract -p "./sample_with_message.bmp" -out "./i_was_hidden" -steg LSB1 -a aes256 -m ofb -pass "Arthur Scherbius"
# Verificamos que sea idéntico
sha256sum "./hide_me.txt"
sha256sum "./i_was_hidden.txt"
```

## End to end tests

En una terminal, parados en el directorio raíz del repositorio, ejecutar los siguientes comandos:
```bash
make clean && make && echo "------" && ./test/example_1.sh
```

> Aclaración: el script asume que la utilidad `trash-put` se encuentra instalada.
> Cambiar `RM_COMMAND` en el script `example_1.sh` por `rm` en caso contrario, o instalarla mediante:
> - Ubuntu/Debian: `sudo apt install trash-cli`
> - Arch Linux: `sudo pacman -S trash-cli`


Este test ejecuta todas las combinaciones de algoritmos, modos y algoritmos de esteganografiado.
Es decir, todas las combinaciones de los siguientes tres parámetros:
```
-steg <LSB1 | LSB4 | LSBI>
-a <aes128 | aes192 | aes256 | 3des>
-m <ecb | cfb | ofb | cbc>
```

Notar que estos tests generan una gran cantidad de archivos en el directorio `test/`. 
Son necesarios al menos 5 GiB de espacio libre en disco.

