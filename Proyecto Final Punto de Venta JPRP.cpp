#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

///////////////////////Estructuras///////////////////////

typedef struct {
	int id;
	char *nombre;
	float precio;
	int stockActual;
} Producto; // Estructura de los productos


typedef struct {
	Producto *productos;
	int cantidad;
	int capacidad;
} lineaVenta; // Estructura del carrito

typedef struct {
	Producto *productos;
	int cantidadProductos;
	int capacidad;
} Inventario; // Estructura para el inventario

///////////////////////Disenio de la interfaz///////////////////////
void limpiar() {
	//en algunos compiladores el comando para limpiar la pantalla varia entre cls y clear asi que en esta funcion se ejecutan los dos para asegurar que funcione
	system("clear");
	system("cls");
}
void marco1() {
	printf("{[]======{}=====[]====={}======[]}\n");
}

void marco2() {
	printf(" ||===()===  [] () []  ===()===||\n");
}

void espacio() {
	printf(" ||                            ||\n");
}

void selector() {
	printf("\n>>========>     ");
}

void marcoGrande() {
	printf("{[]========{}=======[]=======()=======[]====={}========[]}\n");
}

void espacioGrande() {
	printf(" ||                                                    ||\n");
}

void marco2Grande() {
	printf(" ||========()======{}===[]  ()  []==={}======()========||\n");
}

///////////////////////Funciones///////////////////////

/////////// Funciones auxiliares


Producto* crearProducto(int id, const char *nombre, float precio, int cantidad) {

	Producto *nuevo=(Producto*)malloc(sizeof(Producto));
	if (nuevo==NULL) {
		printf("Error: No se pudo asignar memoria para el producto\n");
		return NULL;
	}

	nuevo->id=id;
	nuevo->nombre=(char*)malloc(strlen(nombre)+1);
	if (nuevo->nombre==NULL) {
		free(nuevo);
		return NULL;
	}

	strcpy(nuevo->nombre,nombre);
	nuevo->precio=precio;
	nuevo->stockActual=cantidad;

	return nuevo;
}

void liberarProducto(Producto *producto) {
	if (producto != NULL) {
		free(producto->nombre);
		free(producto);
	}
}

Inventario* iniciarInventario(int capacidadInicial) {
	Inventario *inv=(Inventario*)malloc(sizeof(Inventario));
	if (inv==NULL) return NULL;

	inv->productos=(Producto*)malloc(sizeof(Producto)*capacidadInicial);
	if (inv->productos==NULL) {
		free(inv);
		return NULL;
	}

	inv->cantidadProductos=0;
	inv->capacidad=capacidadInicial;
	return inv;
}

void liberarInventario(Inventario *inv) {
	if (inv!=NULL) {
		int i;
		for (i=0; i<inv->cantidadProductos; i++) {
			liberarProducto(&inv->productos[i]);
		}
		free(inv->productos);
		free(inv);
	}
}

lineaVenta* iniciarCarrito() {
	lineaVenta *carrito=(lineaVenta*)malloc(sizeof(lineaVenta));
	if (carrito==NULL) return NULL;

	carrito->capacidad=10;
	carrito->productos=(Producto*)malloc(sizeof(Producto)*carrito->capacidad);
	if (carrito->productos==NULL) {
		free(carrito);
		return NULL;
	}

	carrito->cantidad = 0;
	return carrito;
}

void liberarCarrito(lineaVenta *carrito) {
	if (carrito!=NULL) {
		int i;
		for (i=0; i<carrito->cantidad; i++) {
			liberarProducto(&carrito->productos[i]);
		}
		free(carrito->productos);
		free(carrito);
	}
}

/////////// Funciones de redimensionamiento

int redInv(Inventario *inv, int nuevoValor) {
	Producto *nuevosProductos=(Producto*)realloc(inv->productos,sizeof(Producto)*nuevoValor);
	if (nuevosProductos==NULL) return 0;

	inv->productos=nuevosProductos;
	inv->capacidad=nuevoValor;
	return 1;
}

int redLinea(lineaVenta *carrito, int nuevoValor) {
	Producto *nuevosProductos=(Producto*)realloc(carrito->productos, sizeof(Producto)*nuevoValor);
	if (nuevosProductos==NULL) return 0;

	carrito->productos=nuevosProductos;
	carrito->capacidad=nuevoValor;
	return 1;
}

/////////// Funciones princiales

void aniadirProducto(Inventario *inv, int id, const char *nombre, float precio, int cantidad) {

	if (inv->cantidadProductos>=inv->capacidad) {
		if (!redInv(inv,inv->capacidad*2)) {
			printf("Error: No se pudo expandir el inventario\n");
			return;
		}
	}//para redimensionar en caso de ser necesario


	Producto *nuevo=crearProducto(id,nombre,precio,cantidad);
	if (nuevo!=NULL) {
		inv->productos[inv->cantidadProductos]=*nuevo;
		inv->cantidadProductos++;
		printf(" || Producto agregado con exito||\n");
	}//crea el nuevo producto
}

void imprInv(Inventario *inv) {
	marcoGrande();
	printf(" ||======()===={}===[]  Inventario  []==={}====()======||\n");
	espacioGrande();
	printf(" || ID   Nombre                Precio          Cantidad||\n");
	int i;
	for (i = 0; i < inv->cantidadProductos; i++) {
		printf(" || %d   %s               %f              %d||\n",
		       inv->productos[i].id,
		       inv->productos[i].nombre,
		       inv->productos[i].precio,
		       inv->productos[i].stockActual);
	}
	espacioGrande();
	marco2Grande();
	marcoGrande();
}

Producto* BuscarProducto(Inventario *inv, int id) {
	int i;
	for (i=0; i<inv->cantidadProductos; i++) {
		if (inv->productos[i].id==id) {
			return &inv->productos[i];
		}
	}
	return NULL;
}

void aniadirAlCarrito(lineaVenta *carrito, Inventario *inv, int id, int cantidad) {
	Producto *producto=BuscarProducto(inv, id);
	if (producto==NULL) {
		printf("Error: Producto no encontrado\n");
		return;
	}

	if (producto->stockActual<cantidad) {
		printf("Error: Stock insuficiente. Solo hay %d unidades\n",producto->stockActual);
		return;
	}

	if (carrito->cantidad>=carrito->capacidad) {
		if (!redLinea(carrito,carrito->capacidad*2)) {
			printf("Error: No se pudo expandir el carrito\n");
			return;
		}
	}

	Producto *itemCarrito=crearProducto(producto->id,producto->nombre,producto->precio,cantidad);
	if (itemCarrito!=NULL) {
		carrito->productos[carrito->cantidad]=*itemCarrito;
		carrito->cantidad++;


		producto->stockActual-=cantidad; // Actualiza el stock

		printf("Agregado al carrito: %d x %s\n",cantidad,producto->nombre);
	}
}

void imprCarr(lineaVenta *carrito) {
	limpiar();
	marcoGrande();
	printf(" ||======()===={}===[]   Carrito    []==={}====()======||\n");
	espacioGrande();
	if (carrito->cantidad==0) {
		printf(" ||               El carrito esta vacio                ||\n");
		espacioGrande();
		marco2Grande();
		marcoGrande();
		return;
	}

	float total=0;
	printf(" || ID   Nombre       Precio      Cantidad     Subtotal||\n");
	int i;
	for (i=0; i<carrito->cantidad; i++) {
		float subtotal=carrito->productos[i].precio*carrito->productos[i].stockActual;
		total += subtotal;
		printf(" || %d    %s      %f   %d            %f ||\n",
		       carrito->productos[i].id,
		       carrito->productos[i].nombre,
		       carrito->productos[i].precio,
		       carrito->productos[i].stockActual,
		       subtotal);
	}
	printf(" ||Total:            $%f                          ||\n",total);
	marco2Grande();
	marcoGrande();
}

void vender(lineaVenta *carrito) {
	if (carrito->cantidad == 0) {
		marcoGrande();
		printf(" ||         Error: El carrito esta vacio               ||\n");
		marco2Grande();
		marcoGrande();

		return;
	}
	marcoGrande();
	printf("\n ||======()===={}===[] Vendiendo []==={}====()======||\n\n");
	imprCarr(carrito);

	// Limpiar el carrito
	int i;
	for (i = 0; i < carrito->cantidad; i++) {
		liberarProducto(&carrito->productos[i]);
	}
	carrito->cantidad = 0;

	printf("Venta procesada exitosamente!\n");
}

void vaciarCarro(lineaVenta *carrito, Inventario *inv) {

	int i;
	for (i = 0; i < carrito->cantidad; i++) {
		Producto *producto_inv = BuscarProducto(inv, carrito->productos[i].id);
		if (producto_inv != NULL) {
			producto_inv->stockActual += carrito->productos[i].stockActual;
		}
	}// Para devoolver los productos al inventario


	for (i=0; i< carrito->cantidad; i++) {
		liberarProducto(&carrito->productos[i]);
	}
	carrito->cantidad= 0;
	marcoGrande();
	printf(" || Carrito vaciado y productos devueltos al inventario||\n");
	marco2Grande();
	marcoGrande();
}

///////////  misc

void menu() {
	marco1();
	printf(" ||===()=== Bienvenido ===()===||\n");
	espacio();
	printf(" ||  Seleccione una opcion:    ||\n");
	espacio();
	printf(" ||      1 - Inventario        ||\n");
	printf(" ||      2 - Carrito           ||\n");
	printf(" ||    Cualquier num - Salir   ||\n");
	espacio();
	marco2();
	marco1();
	selector();
}

int main() {
	Inventario *inventario = iniciarInventario(10);
	lineaVenta *carrito = iniciarCarrito();

	aniadirProducto(inventario, 1,"Agua 1L",15,40);
	aniadirProducto(inventario, 2,"Coca Taparrosca",17,30);
	aniadirProducto(inventario, 3,"Kleenex",20,15);
	aniadirProducto(inventario, 4,"Doritos",14,33);

	int id, cantidad;
	float precio;
	char nombre[50];
	limpiar();

	int opcion,opcion2,i;
	do {
		menu();

		scanf("%d", &opcion);

		switch (opcion) {
		case 1:
			limpiar();
			marco1();
			printf(" ||  Seleccione una opcion:    ||\n");
			espacio();
			printf(" ||  1 - Mostrar inventario    ||\n");
			printf(" ||  2 - Agregar producto      ||\n");
			printf(" ||  Cualquier num - Cancelar  ||\n");
			espacio();
			marco2();
			marco1();
			selector();
			scanf(" %d",&opcion2);
			switch(opcion2) {
			case 1:
				limpiar();
				imprInv(inventario);
				printf("\n Escriba cualquier numero para continuar");
				selector();
				scanf(" %d",&opcion2);
				limpiar();
				break;
			case 2:
				limpiar();
				marco1();
				printf(" ||    Introduzca los datos    ||\n");
				espacio();
				marco2();
				marco1();
				printf("         ID del producto: \n");
				selector();
				scanf("%d", &id);
				printf("      Nombre del producto: ");
				selector();
				scanf("%s", nombre);
				printf("       Precio del producto: \n");
				selector();
				scanf("%f", &precio);
				printf("        Stock del producto \n ");
				selector();
				scanf("%d", &cantidad);
				limpiar();
				marco1();
				aniadirProducto(inventario, id, nombre, precio, cantidad);
				marco2();
				marco1();
				printf("\n Escriba cualquier numero para continuar");
				selector();
				scanf(" %d",&opcion2);
				limpiar();
				break;
			default:
				limpiar();
				break;
			}
			break;

		case 2: {
			limpiar();
			marco1();
			printf(" ||  Seleccione una opcion:    ||\n");
			espacio();
			printf(" ||   1 - Agregar al carrito   ||\n");
			printf(" ||   2 - Mostrar carrito      ||\n");
			printf(" ||   3 - Vaciar carrito       ||\n");
			printf(" ||   4 - Procesar venta       ||\n");
			printf(" ||  Cualquier num - Cancelar  ||\n");
			espacio();
			marco2();
			marco1();
			selector();
			scanf(" %d",&opcion2);
			switch(opcion2) {
			case 1:
				limpiar();
				marco1();
				printf(" ||    Introduzca los datos    ||\n");
				espacio();
				marco2();
				marco1();
				printf("       ID del producto: \n");
				selector();
				scanf("%d",&id);
				printf("       Cantidad del producto: \n");
				selector();
				scanf("%d",&cantidad);
				aniadirAlCarrito(carrito,inventario,id,cantidad);
				limpiar();
				break;
			case 2:
				imprCarr(carrito);
				int xd;
				printf("Inserte cualquier numero para continuar\n");
				selector();
				scanf("%d",&opcion2);
				limpiar();
				break;
			case 3:
				limpiar();
				vaciarCarro(carrito,inventario);
				printf("Inserte cualquier numero para continuar\n");
				selector();
				scanf("%d",&opcion2);
				limpiar();
				break;
			case 4:
				limpiar();
				vender(carrito);
				printf("Inserte cualquier numero para continuar\n");
				selector();
				scanf("%d",&opcion2);
				limpiar();
				break;
			}
			break;
		}

		default:
			opcion=3;
			break;
		}
	} while (opcion != 3);

	return 0;
}
