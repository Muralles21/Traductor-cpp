#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <windows.h>

using namespace std;

struct Nodo {
    string palabra_es;
    map<string, string> traducciones;
    Nodo* izquierdo;
    Nodo* derecho;
    int altura;

    Nodo(string es, string it, string fr, string de, string en) {
        palabra_es = es;
        traducciones["Italiano"] = it;
        traducciones["Frances"] = fr;
        traducciones["Aleman"] = de;
        traducciones["Ingles"] = en;
        izquierdo = nullptr;
        derecho = nullptr;
        altura = 1;
    }

    ~Nodo() {
        delete izquierdo;
        delete derecho;
    }
};

vector<string> historial_encriptado;
map<string, int> contadorPalabras;

int altura(Nodo* nodo) {
    if (nodo == nullptr)
        return 0;
    return nodo->altura;
}

int maximo(int a, int b) {
    return (a > b) ? a : b;
}

int obtenerBalance(Nodo* nodo) {
    if (nodo == nullptr)
        return 0;
    return altura(nodo->izquierdo) - altura(nodo->derecho);
}

Nodo* rotacionDerecha(Nodo* y) {
    Nodo* x = y->izquierdo;
    Nodo* T2 = x->derecho;

    x->derecho = y;
    y->izquierdo = T2;

    y->altura = maximo(altura(y->izquierdo), altura(y->derecho)) + 1;
    x->altura = maximo(altura(x->izquierdo), altura(x->derecho)) + 1;

    return x;
}

Nodo* rotacionIzquierda(Nodo* x) {
    Nodo* y = x->derecho;
    Nodo* T2 = y->izquierdo;

    y->izquierdo = x;
    x->derecho = T2;

    x->altura = maximo(altura(x->izquierdo), altura(x->derecho)) + 1;
    y->altura = maximo(altura(y->izquierdo), altura(y->derecho)) + 1;

    return y;
}

Nodo* insertarNodo(Nodo* nodo, string es, string it, string fr, string de, string en, const string& archivo) {
    if (nodo == nullptr) {
        nodo = new Nodo(es, it, fr, de, en);
        ofstream archivo_salida(archivo, ios::app);
        archivo_salida << es << " " << it << " " << fr << " " << de << " " << en << endl;
        archivo_salida.close();
        return nodo;
    }

    if (es < nodo->palabra_es)
        nodo->izquierdo = insertarNodo(nodo->izquierdo, es, it, fr, de, en, archivo);
    else if (es > nodo->palabra_es)
        nodo->derecho = insertarNodo(nodo->derecho, es, it, fr, de, en, archivo);
    else
        return nodo;

    nodo->altura = 1 + maximo(altura(nodo->izquierdo), altura(nodo->derecho));

    int balance = obtenerBalance(nodo);

    if (balance > 1 && es < nodo->izquierdo->palabra_es)
        return rotacionDerecha(nodo);

    if (balance < -1 && es > nodo->derecho->palabra_es)
        return rotacionIzquierda(nodo);

    if (balance > 1 && es > nodo->izquierdo->palabra_es) {
        nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
        return rotacionDerecha(nodo);
    }

    if (balance < -1 && es < nodo->derecho->palabra_es) {
        nodo->derecho = rotacionDerecha(nodo->derecho);
        return rotacionIzquierda(nodo);
    }

    return nodo;
}

Nodo* encontrarNodoMinimo(Nodo* nodo) {
    Nodo* actual = nodo;
    while (actual->izquierdo != nullptr)
        actual = actual->izquierdo;
    return actual;
}

Nodo* eliminarNodo(Nodo* raiz, string palabra_es) {
    if (raiz == nullptr)
        return raiz;

    if (palabra_es < raiz->palabra_es)
        raiz->izquierdo = eliminarNodo(raiz->izquierdo, palabra_es);
    else if (palabra_es > raiz->palabra_es)
        raiz->derecho = eliminarNodo(raiz->derecho, palabra_es);
    else {
        if (raiz->izquierdo == nullptr || raiz->derecho == nullptr) {
            Nodo* temp = raiz->izquierdo ? raiz->izquierdo : raiz->derecho;

            if (temp == nullptr) {
                temp = raiz;
                raiz = nullptr;
            } else
                *raiz = *temp;
            delete temp;
        } else {
            Nodo* temp = encontrarNodoMinimo(raiz->derecho);
            raiz->palabra_es = temp->palabra_es;
            raiz->derecho = eliminarNodo(raiz->derecho, temp->palabra_es);
        }
    }

    if (raiz == nullptr)
        return raiz;

    raiz->altura = 1 + maximo(altura(raiz->izquierdo), altura(raiz->derecho));

    int balance = obtenerBalance(raiz);

    if (balance > 1 && obtenerBalance(raiz->izquierdo) >= 0)
        return rotacionDerecha(raiz);

    if (balance > 1 && obtenerBalance(raiz->izquierdo) < 0) {
        raiz->izquierdo = rotacionIzquierda(raiz->izquierdo);
        return rotacionDerecha(raiz);
    }

    if (balance < -1 && obtenerBalance(raiz->derecho) <= 0)
        return rotacionIzquierda(raiz);

    if (balance < -1 && obtenerBalance(raiz->derecho) > 0) {
        raiz->derecho = rotacionDerecha(raiz->derecho);
        return rotacionIzquierda(raiz);
    }

    return raiz;
}

Nodo* buscarPalabra(Nodo* nodo, const string& palabra_es) {
    if (nodo == nullptr || nodo->palabra_es == palabra_es)
        return nodo;

    if (palabra_es < nodo->palabra_es)
        return buscarPalabra(nodo->izquierdo, palabra_es);
    else
        return buscarPalabra(nodo->derecho, palabra_es);
}

void cargarPalabrasDesdeArchivo(Nodo*& raiz, const string& palabras) {
    ifstream archivo(palabras);
    string es, it, fr, de, en;
    while (archivo >> es >> it >> fr >> de >> en) {
        raiz = insertarNodo(raiz, es, it, fr, de, en, palabras);
    }
}

void reproducirVoz(const string& palabra) {
    string ruta_speak = "C:\\eSpeak\\command_line\\espeak.exe";
    string comando = ruta_speak + " \"" + palabra + "\"";
    system(comando.c_str());
}

void mostrarTraducciones(Nodo* nodo) {
    cout << "Traducciones:" << endl;
    for (const auto& traduccion : nodo->traducciones) {
        cout << traduccion.first << ": " << traduccion.second << endl;
        reproducirVoz(traduccion.second);
    }
}

string encriptarPalabra(const string& palabra) {
    map<char, string> tablaEncriptacion = {{'a', "U1"}, {'e', "U2"}, {'i', "U3"}, {'o', "U4"}, {'u', "U5"},
                                           {'b', "m1"}, {'c', "m2"}, {'d', "m3"}, {'f', "m4"}, {'g', "m5"},
                                           {'h', "m6"}, {'j', "m7"}, {'k', "m8"}, {'l', "m9"}, {'m', "m10"},
                                           {'n', "m11"}, {'p', "m12"}, {'q', "m13"}, {'r', "m14"}, {'s', "m15"},
                                           {'t', "m16"}, {'v', "m17"}, {'w', "m18"}, {'x', "m19"}, {'y', "m20"},
                                           {'z', "m21"}, {'B', "g1"}, {'C', "g2"}, {'D', "g3"}, {'F', "g4"},
                                           {'G', "g5"}, {'H', "g6"}, {'J', "g7"}, {'K', "g8"}, {'L', "g9"},
                                           {'M', "g10"}, {'N', "g11"}, {'P', "g12"}, {'Q', "g13"}, {'R', "g14"},
                                           {'S', "g15"}, {'T', "g16"}, {'V', "g17"}, {'W', "g18"}, {'X', "g19"},
                                           {'Y', "g20"}, {'Z', "g21"}};

    string palabraEncriptada;
    for (char c : palabra) {
        if (tablaEncriptacion.find(c) != tablaEncriptacion.end()) {
            palabraEncriptada += tablaEncriptacion[c];
        } else {
            palabraEncriptada += c;
        }
    }
    return palabraEncriptada;
}

void guardarPalabraEncriptada(const string& palabraOriginal, const string& palabraEncriptada) {
    ofstream archivo("historial_encriptado.txt", ios::app);
    if (archivo.is_open()) {
        archivo << palabraEncriptada << endl;
        archivo.close();
    } else {
        cout << "Error al abrir el archivo para guardar la palabra encriptada." << endl;
    }

    ofstream archivoOriginal("informacion_original.txt", ios::app);
    if (archivoOriginal.is_open()) {
        archivoOriginal << palabraOriginal << endl;
        archivoOriginal.close();
    } else {
        cout << "Error al abrir el archivo para guardar la palabra original." << endl;
    }
}

void visualizarPalabrasEncriptadas(const string& archivo_encriptado) {
    ifstream archivo(archivo_encriptado);
    if (!archivo.is_open()) {
        cout << "Error al abrir el archivo." << endl;
        return;
    }

    string palabra_encriptada;
    while (getline(archivo, palabra_encriptada)) {
        cout << palabra_encriptada << endl;
    }

    archivo.close();
}

void limpiarPantalla() {
    // Comando para limpiar la pantalla dependiendo del sistema operativo
#ifdef _WIN32 // Windows
    system("cls");
#else // Unix/Linux/macOS
    system("clear");
#endif
}

void pausa() {
    // Espera a que el usuario presione cualquier tecla para continuar
    cin.ignore();
    cin.get();
}

void mostrarMenu(Nodo* raiz) {
    int opcion = 0;
    string palabra_es, palabra_it, palabra_fr, palabra_de, palabra_en;
    vector<string> historial;

    do {
    	cout << "------------------------------------" << endl;
        cout << "1. Buscar palabra"<< endl;
        cout << "2. Agregar nueva palabra"<< endl;
        cout << "3. Eliminar palabra"<< endl;
        cout << "4. Ver historial y sugerencias"<< endl;
        cout << "5. Visualizar palabras encriptadas"<< endl;
        cout << "6. Salir"<< endl;
        cout << "------------------------------------" << endl;
        cout << "Ingrese una opcion: ";
        cin >> opcion;
        limpiarPantalla();
        switch (opcion) {
            case 1: {
                cout << "Ingrese la palabra a buscar: ";
                cin >> palabra_es;
                Nodo* encontrado = buscarPalabra(raiz, palabra_es);
                if (encontrado != nullptr) {
                    cout << "Palabra encontrada: " << encontrado->palabra_es << endl;
                    mostrarTraducciones(encontrado);
                    historial.push_back(palabra_es);
                    contadorPalabras[palabra_es]++;
                    string palabra_encriptada = encriptarPalabra(palabra_es);
                    guardarPalabraEncriptada(palabra_es, palabra_encriptada);
                } else {
                    cout << "Palabra no encontrada." << endl;
                }
                cout << "Presiona cualquier tecla para continuar..." << endl;
    			pausa();
    			limpiarPantalla();
                break;
            }
            
            case 2: {
                cout << "Ingrese la palabra para agregar: ";
                cin >> palabra_es;
                cout << "Ingrese la traducción en italiano: ";
                cin >> palabra_it;
                cout << "Ingrese la traducción en frances: ";
                cin >> palabra_fr;
                cout << "Ingrese la traducción en aleman: ";
                cin >> palabra_de;
                cout << "Ingrese la traducción en ingles: ";
                cin >> palabra_en;
                raiz = insertarNodo(raiz, palabra_es, palabra_it, palabra_fr, palabra_de, palabra_en, "palabras.txt");
                historial_encriptado.push_back(encriptarPalabra(palabra_es)); // Encripta y guarda la palabra
                cout << "Presiona cualquier tecla para continuar..." << endl;
    			pausa();
    			limpiarPantalla();
				break;
            }
            
            case 3: {
                cout << "Ingrese la palabra en español a eliminar: ";
                cin >> palabra_es;
                raiz = eliminarNodo(raiz, palabra_es);
                cout << "Presiona cualquier tecla para continuar..." << endl;
    			pausa();
    			limpiarPantalla();
                break;
            }
            
            case 4: {
                cout << "\nHistorial de palabras buscadas:\n";
                for (const auto& palabra : historial) {
                    cout << palabra << endl;
                }
                cout << "Palabras sugeridas (más buscadas):\n";
                vector<pair<string, int>> sugerencias(contadorPalabras.begin(), contadorPalabras.end());
                sort(sugerencias.begin(), sugerencias.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
                    return a.second > b.second;
                });
                int top = min(3, static_cast<int>(sugerencias.size()));
                for (int i = 0; i < top; ++i) {
                    cout << sugerencias[i].first << " (" << sugerencias[i].second << " veces)" << endl;
                }
                cout << "Presiona cualquier tecla para continuar..." << endl;
    			pausa();
    			limpiarPantalla();
                break;
            }
            
            case 5: {
                visualizarPalabrasEncriptadas("historial_encriptado.txt");
                cout << "Presiona cualquier tecla para continuar..." << endl;
    			pausa();
    			limpiarPantalla();
                break;
            }
            
            case 6: {
                cout << "Cerrando Sesion..." << endl;
                break;
            }
            default:
                cout << "Opción no válida. Intente de nuevo." << endl;
        }
    } while (opcion != 6);

    // Guardar el historial encriptado en el archivo
    ofstream archivo_encriptado("historial_encriptado.txt", ios::app);
    for (const string& palabra : historial_encriptado) {
        archivo_encriptado << palabra << endl;
    }
    archivo_encriptado.close();
}

bool iniciarSesion(const string& usuario, const string& contrasena) {
    ifstream archivo("usuarios.txt");
    string u, c;
    while (archivo >> u >> c) {
        if (u == usuario && c == contrasena) {
            return true;
        }
    }
    return false;
}

void registrarUsuario(const string& usuario, const string& contrasena) {
    ofstream archivo("usuarios.txt", ios::app);
    archivo << usuario << " " << contrasena << endl;
    archivo.close();
}

void eliminarUsuario(const string& usuario) {
    ifstream archivo("usuarios.txt");
    ofstream archivo_temp("temp.txt");
    string u, c;
    while (archivo >> u >> c) {
        if (u != usuario) {
            archivo_temp << u << " " << c << endl;
        }
    }
    archivo.close();
    archivo_temp.close();
    remove("usuarios.txt");
    rename("temp.txt", "usuarios.txt");
}

void visualizarUsuarios() {
    ifstream archivo("usuarios.txt");
    string usuario, contrasena;
    cout << "Usuarios registrados:" << endl;
    while (archivo >> usuario >> contrasena) {
        cout << "Usuario: " << usuario << endl;
    }
    archivo.close();
}

void mostrarMenuUsuarios(Nodo* raiz) {
    int opcion = 0;
    string usuario, contrasena;
    do {
    	cout << "------------------------------------" << endl;
        cout << "1. Iniciar sesion"<< endl;
        cout << "2. Registrar usuario"<< endl;
        cout << "3. Eliminar usuario"<< endl;
        cout << "4. Visualizar usuarios registrados"<< endl;
        cout << "5. Salir "<< endl;
        cout << "------------------------------------" << endl;
        cout << "Ingrese una opcion: ";
        cin >> opcion;
        limpiarPantalla();
        switch (opcion) {
            case 1:
                cout << "Ingrese usuario: ";
                cin >> usuario;
                cout << "Ingrese contrasena: ";
                cin >> contrasena;
                if (iniciarSesion(usuario, contrasena)) {
                    cout << "Inicio de sesion exitoso." << endl;
                    mostrarMenu(raiz);
                } else {
                    cout << "Usuario o contrasena incorrectos." << endl;
                }
                break;
                
            case 2:
                cout << "Ingrese usuario: ";
                cin >> usuario;
                cout << "Ingrese contrasena: ";
                cin >> contrasena;
                registrarUsuario(usuario, contrasena);
                cout << "Usuario registrado con exito." << endl;
                break;
                
            case 3:
                cout << "Ingrese el usuario a eliminar: ";
                cin >> usuario;
                eliminarUsuario(usuario);
                cout << "Usuario eliminado con exito." << endl;
                break;
                
            case 4:
                visualizarUsuarios();
                break;
                
            case 5:
                cout << "Saliendo del programa..." << endl;
                break;
            default:
                cout << "Opcion no valida. Intente de nuevo." << endl;
        }
    } while (opcion != 5);
}

void CambiarAtributosArchivo(const string& nombrearchivo) {
    if (!SetFileAttributes(nombrearchivo.c_str(), FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)) {
        cout << "Error al cambiar los atributos del archivo." << endl;
        return;
    }
    cout << "Atributos del archivo cambiados exitosamente." << endl;
}

int main() {
    Nodo* raiz = nullptr;
    cargarPalabrasDesdeArchivo(raiz, "palabras.txt");
    mostrarMenuUsuarios(raiz);
    string nombreArchivo = "usuarios.txt";
    CambiarAtributosArchivo(nombreArchivo);

    delete raiz;
    return 0;
}

