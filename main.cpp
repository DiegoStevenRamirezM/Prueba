#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Función para convertir un carácter a binario (del sexto código)
void caracterABinario(unsigned char c, char binario[8]) {
    for (int i = 7; i >= 0; --i) {
        binario[i] = (c % 2) + '0';
        c /= 2;
    }
}

// Función para invertir todos los bits de un bloque (Método 1)
void invertirBloque(char bloque[], int tamano) {
    for (int i = 0; i < tamano; i++) {
        bloque[i] = (bloque[i] == '1') ? '0' : '1';
    }
}

// Función para invertir cada 2 bits (Método 1, invertir el segundo bit de cada par)
void invertirCadaDos(char bloque[], int tamano) {
    for (int i = 1; i < tamano; i += 2) { // Solo posiciones 2, 4, ...
        bloque[i] = (bloque[i] == '1') ? '0' : '1';
    }
}

// Función para invertir cada 3 bits (Método 1, invertir el tercer bit de cada grupo de 3)
void invertirCadaTres(char bloque[], int tamano) {
    for (int i = 2; i < tamano; i += 3) { // Solo posiciones 3, 6, ...
        bloque[i] = (bloque[i] == '1') ? '0' : '1';
    }
}

// Función para desplazar los bits en un bloque (Método 2)
void desplazarBloque(char bloque[], int tamano) {
    if (tamano <= 1) return;
    char temp = bloque[tamano - 1]; // Guardar el último bit
    for (int i = tamano - 1; i > 0; i--) {
        bloque[i] = bloque[i - 1]; // Desplazar hacia la derecha
    }
    bloque[0] = temp; // El primer bit es el último original
}

int main() {
    char nombreArchivoEntrada[100], nombreArchivoSalida[100];
    int tamanoBloque, metodo;
    string textoEntrada;

    // Instrucciones iniciales
    cout << "Ingrese el nombre del archivo fuente (ej. miarchivo.txt): ";
    cin >> nombreArchivoEntrada;
    cin.ignore(); // Limpiar el buffer para getline
    cout << "Ingrese el texto para el archivo (ej. AbCd): ";
    getline(cin, textoEntrada);

    // Crear el archivo de entrada como texto
    ofstream crearArchivo(nombreArchivoEntrada);
    if (!crearArchivo) {
        cerr << "Error al crear el archivo de entrada.\n";
        return 1;
    }
    crearArchivo << textoEntrada; // Escribir solo el texto
    crearArchivo.close();
    cout << "Archivo de entrada '" << nombreArchivoEntrada << "' creado con exito.\n";

    // Leer parámetros adicionales
    cout << "Nombre del archivo de salida (ej. salida.bin): ";
    cin >> nombreArchivoSalida;
    cout << "Tamano del bloque (n): ";
    cin >> tamanoBloque;
    cout << "Metodo de codificacion (1 o 2): ";
    cin >> metodo;

    // Validar el método
    if (metodo != 1 && metodo != 2) {
        cerr << "Metodo invalido. Use 1 o 2.\n";
        return 1;
    }

    // Abrir el archivo de entrada en modo binario
    ifstream archivoEntrada(nombreArchivoEntrada, ios::binary);
    if (!archivoEntrada) {
        cerr << "Error: No se pudo abrir el archivo de entrada.\n";
        return 1;
    }

    // Usar la longitud del texto ingresado
    int tamanoArchivo = textoEntrada.length();
    archivoEntrada.clear();
    archivoEntrada.seekg(0, ios::beg);

    // Arreglo para los bits
    char bitsArchivo[10000];
    int tamanoBitsArchivo = 0;

    // Convertir el archivo a binario
    char caracter;
    int bytesLeidos = 0;
    while (archivoEntrada.get(caracter) && bytesLeidos < tamanoArchivo) {
        char bits[8];
        caracterABinario(caracter, bits);
        for (int i = 0; i < 8; i++) {
            bitsArchivo[tamanoBitsArchivo++] = bits[i];
        }
        bytesLeidos++;
    }
    archivoEntrada.close();

    // Arreglo para los bits codificados
    char bitsCodificados[10000];
    int tamanoBitsCodificados = 0;

    // Procesar bloques de n bits
    for (int i = 0; i < tamanoBitsArchivo; i += tamanoBloque) {
        char bloque[100];
        int tamanoBloqueActual = 0;

        // Copiar hasta n bits
        for (int j = 0; j < tamanoBloque && i + j < tamanoBitsArchivo; j++) {
            bloque[j] = bitsArchivo[i + j];
            tamanoBloqueActual++;
        }

        if (metodo == 1) {
            // Metodo 1
            if (i == 0) {
                invertirBloque(bloque, tamanoBloqueActual);
            } else {
                // Contar 1s y 0s en el bloque anterior (sin codificar)
                int unos = 0, ceros = 0;
                for (int j = i - tamanoBloque; j < i && j < tamanoBitsArchivo; j++) {
                    if (bitsArchivo[j] == '1') unos++;
                    else ceros++;
                }
                if (unos == ceros) {
                    invertirBloque(bloque, tamanoBloqueActual);
                } else if (ceros > unos) {
                    invertirCadaDos(bloque, tamanoBloqueActual);
                } else {
                    invertirCadaTres(bloque, tamanoBloqueActual);
                }
            }
        } else {
            // Método 2
            desplazarBloque(bloque, tamanoBloqueActual);
        }

        // Agregar bloque codificado
        for (int j = 0; j < tamanoBloqueActual; j++) {
            bitsCodificados[tamanoBitsCodificados++] = bloque[j];
        }
    }

    // Mostrar los bits codificados en la consola
    cout << "Bits codificados: ";
    for (int i = 0; i < tamanoBitsCodificados; i++) {
        cout << bitsCodificados[i];
    }
    cout << "\n";

    // Crear el archivo binario de salida
    ofstream archivoSalida(nombreArchivoSalida, ios::binary);
    if (!archivoSalida) {
        cerr << "Error al abrir el archivo de salida.\n";
        return 1;
    }

    // Convertir bits codificados a bytes
    for (int i = 0; i < tamanoBitsCodificados; i += 8) {
        unsigned char byte = 0;
        for (int j = 0; j < 8 && i + j < tamanoBitsCodificados; j++) {
            if (bitsCodificados[i + j] == '1') {
                byte |= (1 << (7 - j));
            }
        }
        archivoSalida.put(byte);
    }
    archivoSalida.close();

    cout << "Archivo codificado generado con exito en '" << nombreArchivoSalida << "'.\n";
    return 0;
}
