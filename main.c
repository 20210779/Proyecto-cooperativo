/*
 * Simulador de navegación web en consola
 * ---------------------------------------
 * Usa pilas dinámicas (listas enlazadas que se comportan como pila):
 *   - pila "atrás":    páginas que se pueden volver a visitar con Atrás
 *   - pila "adelante": páginas a las que se puede ir con Adelante
 * Además mantiene un historial (lista enlazada) con todas las páginas
 * visitadas durante la sesión, en orden cronológico.
 *
 * Compilar:  gcc -Wall -Wextra -o navegador navegador.c
 * Ejecutar:  ./navegador   (en Windows: navegador.exe)
 */

#include <stdio.h>
#include <stdlib.h>   /* malloc, free */
#include <string.h>   /* strcpy, strncpy, strlen */

#define MAX_URL 100

/* ------------------------------------------------------------------ */
/* ESTRUCTURAS                                                         */
/* ------------------------------------------------------------------ */

/* Nodo genérico: guarda una URL y apunta al siguiente nodo.
 * Se usa tanto en las pilas como en el historial. */
typedef struct Nodo {
    char url[MAX_URL];
    struct Nodo *siguiente;
} Nodo;

/* Pila dinámica: solo necesita saber quién es el tope.
 * push = insertar al inicio, pop = quitar del inicio. */
typedef struct {
    Nodo *tope;
    int tamano;
} Pila;

/* Historial: lista enlazada con inicio y fin (para insertar al final rápido) */
typedef struct {
    Nodo *inicio;
    Nodo *fin;
    int total;
} Historial;

/* Todo el estado del navegador junto */
typedef struct {
    Pila atras;
    Pila adelante;
    Historial historial;
    char actual[MAX_URL];
    int hayActual;          /* 0 = todavía no se ha visitado ninguna página */
} Navegador;

/* ------------------------------------------------------------------ */
/* OPERACIONES DE LA PILA                                              */
/* ------------------------------------------------------------------ */

void pila_init(Pila *p) {
    p->tope = NULL;
    p->tamano = 0;
}

int pila_vacia(const Pila *p) {
    return p->tope == NULL;
}

/* Apila una URL. Devuelve 1 si salió bien, 0 si no hubo memoria. */
int pila_push(Pila *p, const char *url) {
    Nodo *nuevo = (Nodo *)malloc(sizeof(Nodo));
    if (nuevo == NULL) {
        return 0;
    }
    strncpy(nuevo->url, url, MAX_URL - 1);
    nuevo->url[MAX_URL - 1] = '\0';
    nuevo->siguiente = p->tope;   /* el nuevo apunta al antiguo tope */
    p->tope = nuevo;              /* el nuevo pasa a ser el tope     */
    p->tamano++;
    return 1;
}

/* Desapila y copia la URL en 'destino'. Devuelve 1 si había algo, 0 si estaba vacía. */
int pila_pop(Pila *p, char *destino) {
    Nodo *aux;
    if (pila_vacia(p)) {
        return 0;
    }
    aux = p->tope;
    strcpy(destino, aux->url);
    p->tope = aux->siguiente;     /* el tope baja un nodo */
    free(aux);                    /* liberamos el nodo retirado */
    p->tamano--;
    return 1;
}

void pila_vaciar(Pila *p) {
    char descarte[MAX_URL];
    while (pila_pop(p, descarte)) {
        /* pop libera cada nodo */
    }
}

/* Muestra la pila desde el tope hacia abajo, sin modificarla */
void pila_mostrar(const Pila *p, const char *titulo) {
    const Nodo *aux = p->tope;
    int i = 1;
    printf("  %s (%d):\n", titulo, p->tamano);
    if (aux == NULL) {
        printf("    (vacia)\n");
        return;
    }
    while (aux != NULL) {
        printf("    %d. %s%s\n", i, aux->url, (i == 1) ? "   <- tope" : "");
        aux = aux->siguiente;
        i++;
    }
}

/* ------------------------------------------------------------------ */
/* HISTORIAL                                                           */
/* ------------------------------------------------------------------ */

void historial_init(Historial *h) {
    h->inicio = NULL;
    h->fin = NULL;
    h->total = 0;
}

int historial_agregar(Historial *h, const char *url) {
    Nodo *nuevo = (Nodo *)malloc(sizeof(Nodo));
    if (nuevo == NULL) {
        return 0;
    }
    strncpy(nuevo->url, url, MAX_URL - 1);
    nuevo->url[MAX_URL - 1] = '\0';
    nuevo->siguiente = NULL;

    if (h->inicio == NULL) {
        h->inicio = nuevo;
    } else {
        h->fin->siguiente = nuevo;
    }
    h->fin = nuevo;
    h->total++;
    return 1;
}

void historial_liberar(Historial *h) {
    Nodo *aux;
    while (h->inicio != NULL) {
        aux = h->inicio;
        h->inicio = aux->siguiente;
        free(aux);
    }
    h->fin = NULL;
    h->total = 0;
}

/* ------------------------------------------------------------------ */
/* OPERACIONES DEL NAVEGADOR                                           */
/* ------------------------------------------------------------------ */

void navegador_init(Navegador *nav) {
    pila_init(&nav->atras);
    pila_init(&nav->adelante);
    historial_init(&nav->historial);
    nav->actual[0] = '\0';
    nav->hayActual = 0;
}

/* Visitar una página nueva:
 *  1) la página actual (si existe) se guarda en la pila "atrás"
 *  2) la pila "adelante" se vacía (como en un navegador real)
 *  3) la nueva página pasa a ser la actual y se registra en el historial */
void navegador_visitar(Navegador *nav, const char *url) {
    if (nav->hayActual) {
        if (!pila_push(&nav->atras, nav->actual)) {
            printf("Error: no hay memoria disponible.\n");
            return;
        }
    }
    pila_vaciar(&nav->adelante);

    strncpy(nav->actual, url, MAX_URL - 1);
    nav->actual[MAX_URL - 1] = '\0';
    nav->hayActual = 1;

    if (!historial_agregar(&nav->historial, url)) {
        printf("Aviso: no se pudo registrar en el historial (sin memoria).\n");
    }
    printf("Visitando: %s\n", nav->actual);
}

/* Atrás: la actual va a "adelante", y la que sale de "atrás" pasa a ser la actual */
void navegador_atras(Navegador *nav) {
    char anterior[MAX_URL];

    if (pila_vacia(&nav->atras)) {
        printf("No hay paginas anteriores.\n");
        return;
    }
    if (!pila_push(&nav->adelante, nav->actual)) {
        printf("Error: no hay memoria disponible.\n");
        return;
    }
    pila_pop(&nav->atras, anterior);
    strcpy(nav->actual, anterior);
    printf("Volviste a: %s\n", nav->actual);
}

/* Adelante: la actual va a "atrás", y la que sale de "adelante" pasa a ser la actual */
void navegador_adelante(Navegador *nav) {
    char siguiente[MAX_URL];

    if (pila_vacia(&nav->adelante)) {
        printf("No hay paginas siguientes.\n");
        return;
    }
    if (!pila_push(&nav->atras, nav->actual)) {
        printf("Error: no hay memoria disponible.\n");
        return;
    }
    pila_pop(&nav->adelante, siguiente);
    strcpy(nav->actual, siguiente);
    printf("Avanzaste a: %s\n", nav->actual);
}

void navegador_mostrar_estado(const Navegador *nav) {
    printf("\n--- ESTADO ACTUAL ---\n");
    if (nav->hayActual) {
        printf("  Pagina actual: %s\n", nav->actual);
    } else {
        printf("  Pagina actual: (ninguna, aun no has visitado nada)\n");
    }
    pila_mostrar(&nav->atras, "Pila ATRAS");
    pila_mostrar(&nav->adelante, "Pila ADELANTE");
}

void navegador_mostrar_historial(const Navegador *nav) {
    const Nodo *aux = nav->historial.inicio;
    int i = 1;

    printf("\n--- HISTORIAL (%d paginas visitadas) ---\n", nav->historial.total);
    if (aux == NULL) {
        printf("  (vacio)\n");
        return;
    }
    while (aux != NULL) {
        printf("  %d. %s\n", i, aux->url);
        aux = aux->siguiente;
        i++;
    }
}

/* Libera TODA la memoria dinámica antes de salir */
void navegador_destruir(Navegador *nav) {
    pila_vaciar(&nav->atras);
    pila_vaciar(&nav->adelante);
    historial_liberar(&nav->historial);
}

/* ------------------------------------------------------------------ */
/* ENTRADA / MENÚ                                                      */
/* ------------------------------------------------------------------ */

/* Lee una línea completa (con espacios) y le quita el '\n' final */
void leer_linea(char *buffer, int tam) {
    if (fgets(buffer, tam, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        /* la línea era más larga que el buffer: descartamos el resto */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
}

int leer_opcion(void) {
    char linea[20];
    int op;
    leer_linea(linea, sizeof(linea));
    if (sscanf(linea, "%d", &op) != 1) {
        return -1;
    }
    return op;
}

void mostrar_menu(void) {
    printf("\n===== SIMULADOR DE NAVEGACION WEB =====\n");
    printf(" 1. Visitar una pagina nueva\n");
    printf(" 2. Atras\n");
    printf(" 3. Adelante\n");
    printf(" 4. Ver estado (pagina actual y pilas)\n");
    printf(" 5. Ver historial\n");
    printf(" 0. Salir\n");
    printf("Opcion: ");
}

int main(void) {
    Navegador nav;
    char url[MAX_URL];
    int opcion;

    navegador_init(&nav);

    do {
        mostrar_menu();
        opcion = leer_opcion();

        switch (opcion) {
            case 1:
                printf("URL a visitar: ");
                leer_linea(url, sizeof(url));
                if (url[0] == '\0') {
                    printf("La URL no puede estar vacia.\n");
                } else {
                    navegador_visitar(&nav, url);
                }
                break;
            case 2:
                navegador_atras(&nav);
                break;
            case 3:
                navegador_adelante(&nav);
                break;
            case 4:
                navegador_mostrar_estado(&nav);
                break;
            case 5:
                navegador_mostrar_historial(&nav);
                break;
            case 0:
                printf("Cerrando navegador...\n");
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while (opcion != 0);

    navegador_destruir(&nav);
    return 0;
}