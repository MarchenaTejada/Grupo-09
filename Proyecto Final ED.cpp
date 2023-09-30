#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <windows.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <mysql.h>
#include <mysqld_error.h>
#include <clocale>

using namespace std;
template <typename T>

string to_string(T value) {
    ostringstream os;
    os << value;
    return os.str();
}

//Acceso a la base de datos
char HOST[] = "localhost";
char USER[] = "root";
char PASS[] = "1234";

//Estructuras para listas
struct date {
    int dia;
    int mes;
    int anio;
};

struct tiempo {
    int hora;
    int minuto;
};

struct Vuelo {
    int idVuelo;
    string destino;
    date fecha;
    tiempo hora;
    string clase; // Economica, Ejecutiva, Primera clase
    double precio;
    int aforo;
    Vuelo* sgte;
    Vuelo* ant;
};

struct Pasajero {
    int idPasajero;
    string tipoDoc; //DNI - Carnet de de extranjería
    string nombres;
    string apellidos;
    int edad;
    string telefono;
    struct Pasajero *sgte;
};

//Definiendo tipos de datos
typedef struct Vuelo *TpVuelo;
typedef struct Pasajero *TpPasajero;

struct arbol{
	TpVuelo vuelo;
	struct arbol *izq;
	struct arbol *der;
};

typedef struct arbol *TpArbol;

// CONSTANTES
const char FINCAD = char(0);
const int MAXCAD = 20;
const char SP = ' ';

// TIPOS
typedef char TCadena[MAXCAD + 1]; // MAXCAD caracteres + FINCAD
struct Persona_R {
    TCadena PASS;
};

// CABECERA DE PROCEDIMIENTOS Y FUNCIONES
void Ingresar_PASS(Persona_R &p);
void confirmar_PASS(Persona_R &p);
void getPassword(char* password, int maxLength);
void insertarPersonaTXT(TCadena nombreFichero, Persona_R p);
void EscribePersonaFicheroTXT(ofstream &fichero, Persona_R p);
void verificar_existencia_fichero(TCadena nombreFichero);
void verificar_existencia_de_usuario(TCadena nombreFichero);
void verificar_contrasena(TCadena nombreFichero, Persona_R &password);

//Función para conectar la base de datos 
MYSQL* conectarBaseDatos();
 
//menú de registro o nicio de sesion
 void menuIniciarSesion();
 void selecOpcionInicio(TCadena nombreFichero, Persona_R &password, Persona_R &p, TpPasajero &pasajero, TpVuelo &vuelo, MYSQL* conn);
 
// menu secundario 
void menu();
void selecOpc(TpVuelo &vuelo, TpPasajero &listaPasajero,MYSQL* conn);

// Fuciones de registro de vuelo 
TpVuelo crearNodoVuelo();
void insertarVuelo(TpVuelo &vuelo, TpVuelo nuevo);
void guardarVuelosEnBD(TpVuelo vuelo, MYSQL* conn);
void obtenerVuelos (TpVuelo &vuelo, MYSQL* conn);
void modificarVuelo(int id_vuelo, TpVuelo vuelo, MYSQL* conn);
int contarVuelo(TpVuelo vuelo);
void mostrarVuelosOrdenados(TpVuelo vuelo);
void mostrarVuelo(TpVuelo p);
void intercambiar (int &op1, int &op2);
TpVuelo buscarVuelo(TpVuelo p, int idVuelo);
void actualizarVuelo(TpVuelo vuelo, MYSQL* conn);
//Funciones para el registro de pasajeros

TpPasajero crearNodoPasajero();
void insertarPasajero(TpPasajero &pasajero, TpPasajero nuevo);
string consistenciarDocumento(string opciones, string op1, string op2);
void guardarPasajerosEnBD(TpPasajero pasajero, MYSQL* conn);
void obtenerPasajeros (MYSQL* conn, TpPasajero &pasajero);

//Funciones para realizar una reserva
bool buscarPasajero(TpPasajero pasajero, int idPasajero);
void realizarReservaPorFecha(TpPasajero pasajero, TpVuelo v, MYSQL* conn);
int pantallaReserva ();
void realizarReservaPorFecha(TpPasajero pasajero, TpVuelo v, MYSQL* conn);
void reservarPorId (TpPasajero pasajero, TpVuelo v, MYSQL* conn);
void guardarReserva (int idPasajero, int idVuelo, MYSQL* conn);
TpPasajero merge(TpPasajero izquierda, TpPasajero derecha);
void dividir_lista(TpPasajero pasajero, TpPasajero *mitad1, TpPasajero *mitad2);
void mergeSort(TpPasajero *pasajero);
void consulta_vuelo(TpVuelo vuelo,MYSQL* conn);
void lista_pasajeros(TpPasajero pasajeros);

// PROGRAMA PRINCIPAL
int main() {
    Persona_R p;
    Persona_R password;
    TCadena nombre_usuario_registrado;
    setlocale(LC_CTYPE,"Spanish");
    system("color 71");
    TpVuelo vuelo = NULL;
    TpPasajero pasajero = NULL;
    MYSQL* conn = conectarBaseDatos();
    selecOpcionInicio(nombre_usuario_registrado,password,p, pasajero, vuelo, conn);
    mysql_close(conn);
    return 0;
}


void menuIniciarSesion() {
    cout << "\t\t\t==============================================" << endl;
    cout << "\n\t\t\t      * SISTEMA DE VUELOS AÉREOS *" << endl;
    cout << "\n\t\t\t==============================================" << endl;
    cout << "\t\t1. Iniciar sesión" << endl;
    cout << "\t\t2. Registrarse" << endl;
    cout << "\t\t0. Salir del Programa." << endl;
}

void selecOpcionInicio(TCadena nombreFichero, Persona_R &password, Persona_R &p, TpPasajero &pasajero, TpVuelo &vuelo, MYSQL* conn) {
    TCadena nombre_usuario_registrado;
    int i=0;
    int opc_iniciar;
    do {
    	system("cls");
        menuIniciarSesion();
        cout << "\nDigite una opción:  ";
        cin >> opc_iniciar;
        if ((opc_iniciar < 0) || (opc_iniciar > 2)) {
                system("cls");
                cout << "\n\n\t\t\t | ERROR. OPCIÓN INCORRECTA |\n\n\n\n";
                system("pause");
                system("cls");
        }
        switch (opc_iniciar) {
	        case 1: {
	            cout << "\nDigite su Nombre de Usuario:     ";
	            cin >> nombre_usuario_registrado; // INGRESAR COMO USUARIO
	            verificar_existencia_fichero(nombre_usuario_registrado);
	            verificar_contrasena(nombre_usuario_registrado, password); // confirmar_PASS(password);
	            Sleep(20);
	            cout<<"\n\n\t\tC A R G A N D O";
	           	do{
	           		cout<<"\t.";
	           		Sleep(100);
	           		i++;
			   	}while(i<=5);
	            Sleep(20);
	            system("cls");
	            cout<<"\n\t--------- BIENVENIDO --------- \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"<<endl;
				system("pause");
				selecOpc(vuelo,pasajero,conn);
				
	            break;
	        }
	        case 2: {
	            cout << "\nEscriba su nombre de Usuario:    ";
	            cin >> nombre_usuario_registrado; // REGISTRO
	            verificar_existencia_de_usuario(nombre_usuario_registrado);
	            Ingresar_PASS(p);
	            insertarPersonaTXT(nombre_usuario_registrado, p);
	            system("cls");
	            system("pause");
	            break;
	        }
        }
    } while (opc_iniciar != 0);
}

void menu(){
	system("cls");
	cout << "\t\tSISTEMA DE GESTIÓN DE VIAJES" << endl;
	cout << "\t1. Registrar vuelo." << endl;
	cout << "\t2. Registrar pasajero." << endl;
	cout << "\t3. Realizar reserva de vuelo." << endl;
	cout << "\t4. Mostrar vuelos programados." << endl;
	cout << "\t5. Modificar vuelo." << endl;
	cout << "\t6. Mostrar pasajeros." << endl;
	cout << "\t0. Salir." << endl;
	cout << "Seleccione una opcion: ";
}

void selecOpc(TpVuelo &vuelo, TpPasajero &pasajero, MYSQL* conn ){
	int opcion;
	do{
		obtenerVuelos (vuelo, conn);
		obtenerPasajeros (conn, pasajero);
		menu();
		cin >> opcion;
		system("cls");
		switch(opcion){
			case 1: {
				TpVuelo nuevo = crearNodoVuelo();
				insertarVuelo(vuelo, nuevo);
            	guardarVuelosEnBD(nuevo, conn);
            	cout << "VUELO REGISTRADO" << endl;
				break;}
			case 2:{
				TpPasajero nuevo = NULL;
				nuevo = crearNodoPasajero();
				insertarPasajero(pasajero, nuevo);
                guardarPasajerosEnBD(nuevo,conn);
                cout << "PASAJERO REGISTRADO" << endl;
				break;}
			case 3:{
				int opc = pantallaReserva();
				system ("cls");
				if (opc == 1){
					mostrarVuelosOrdenados(vuelo);
					reservarPorId (pasajero, vuelo, conn);
				}else if (opc == 2)
					realizarReservaPorFecha(pasajero, vuelo, conn);
				break;
			}
			case 4:
				mostrarVuelosOrdenados(vuelo);
				break;
			case 5:
				int idVueloMod;
				mostrarVuelosOrdenados(vuelo);
				cout << "Ingrese el id del vuelo a modificar:\n -> ";
				cin >> idVueloMod;
				modificarVuelo(idVueloMod, vuelo, conn);
				break;
			case 6:
				mostrarVuelosOrdenados(vuelo);
				cout<<"\n";
				consulta_vuelo(vuelo,conn);	
				break;
			case 0:
				cout << "Cerrando sesión :D" << endl;
				break;
			default:
				cout << "Opción no válida." << endl; 
		}
		system("pause");
	}while(opcion != 0);	
}

//Merge

TpPasajero merge(TpPasajero izquierda, TpPasajero derecha) {
    if (izquierda == NULL) {
        return derecha;
    }
    if (derecha == NULL) {
        return izquierda;
    }
    TpPasajero resultado = NULL;
    if (izquierda->apellidos <= derecha->apellidos) {
        resultado = izquierda;
        resultado->sgte = merge(izquierda->sgte, derecha);
    } else {
        resultado = derecha;
        resultado->sgte = merge(izquierda, derecha->sgte);
    }
    return resultado;
}

void dividir_lista(TpPasajero pasajero, TpPasajero *mitad1, TpPasajero *mitad2) {
    TpPasajero punteroMitad = pasajero;
    TpPasajero punteroFinal = pasajero->sgte;
    while (punteroFinal != NULL) {
        punteroFinal = punteroFinal->sgte;
        if (punteroFinal != NULL) {
            punteroMitad = punteroMitad->sgte;
            punteroFinal = punteroFinal->sgte;
        }
    }
    *mitad1 = pasajero;
    *mitad2 = punteroMitad->sgte;
    punteroMitad->sgte = NULL;
}

void mergeSort(TpPasajero *pasajero) {
    if (*pasajero == NULL || (*pasajero)->sgte == NULL) {
        return;
    }
    TpPasajero siguiente = NULL;
    TpPasajero anterior = NULL;
    dividir_lista(*pasajero, &anterior, &siguiente);
    mergeSort(&anterior);
    mergeSort(&siguiente);
    *pasajero = merge(anterior,siguiente);
}

void consulta_vuelo(TpVuelo vuelo, MYSQL* conn){
	MYSQL_RES *res;
    MYSQL_ROW row;
    TpPasajero pasajero = NULL, p = NULL;
    int idVuelo;
    cout << "Ingrese el id del vuelo: ";
    cin >> idVuelo;   
    string consulta = "SELECT p.id_pasajero, p.nombres, p.apellidos, p.edad, p.telefono "
                      "FROM pasajeros p "
                      "JOIN reserva r ON p.id_pasajero = r.id_pasajero "
                      "WHERE r.id_vuelo = "+ to_string(idVuelo);
    if (mysql_query(conn, consulta.c_str())) {
        cout << "Error al ejecutar la consulta: " << mysql_error(conn) << endl;
        return;
    }
    res = mysql_store_result(conn);
    if (mysql_num_rows(res) == 0) {
        cout << "No se encontraron pasajeros para el ID de vuelo proporcionado.\n\n" ;
        mysql_free_result(res);
        return;
    }
    while ((row = mysql_fetch_row(res)) != NULL) {
        Pasajero *nuevo = new Pasajero;
        nuevo->idPasajero = atoi(row[0]);
        nuevo->nombres = row[1];
        nuevo->apellidos = row[2];
        nuevo->edad = atoi(row[3]);
        nuevo->telefono = row[4];
        nuevo->sgte = NULL;        
        if (pasajero == NULL) {
            pasajero = nuevo;
            p = nuevo;
        } else {
            p->sgte = nuevo;
            p = nuevo;
        }
    }
	mergeSort(&pasajero);
	lista_pasajeros(pasajero);	
}

void lista_pasajeros(TpPasajero pasajeros){
	while(pasajeros != NULL){
		cout<<"\nID: "<<pasajeros->idPasajero;
		cout<<"\nApellidos: "<<pasajeros->apellidos;
		cout<<"\nNombres: "<<pasajeros->nombres;
		cout<<"\nEdad: "<<pasajeros->edad;	
		cout<<"\nTelefono: "<<pasajeros->telefono;
		cout<<"\n\n";
		pasajeros=pasajeros->sgte;
	}
}

void obtenerPasajeros (MYSQL* conn, TpPasajero &pasajero){
 		//Reiniciamos lista
 		while (pasajero != NULL) {
	        TpPasajero temp = pasajero;
	        pasajero = pasajero->sgte;
	        delete temp;
    	}
 		pasajero = NULL;
		string consulta = "SELECT * FROM pasajeros";
        int query = mysql_query(conn, consulta.c_str());

        if (query != 0){
          cout << "Ha ocurrido un error al realizar la consulta" << endl;      
        }else{
          	MYSQL_RES * resultado = mysql_store_result(conn);
          	unsigned long filas_afectadas = mysql_num_rows(resultado);
          	MYSQL_ROW fila;
          	for (int x = 0; x < filas_afectadas; x++){
          		TpPasajero nuevo = new(struct Pasajero);
              	fila = mysql_fetch_row(resultado);
              	nuevo->idPasajero = atoi(fila[0]);
              	nuevo->nombres = fila [1];
				nuevo->apellidos = fila[2];
				nuevo->edad = atoi(fila[3]);
				nuevo->telefono = fila[4];
				nuevo->sgte = NULL;
				insertarPasajero(pasajero, nuevo);
          	}
          // Liberar el resultado de la consulta
          mysql_free_result(resultado);
        }
}

void obtenerVuelos (TpVuelo &vuelo, MYSQL* conn){
	//Reiniciamos lista
		while (vuelo != NULL) {
        	TpVuelo temp = vuelo;
        	vuelo = vuelo->sgte;
        	delete temp;
    	}
 		vuelo = NULL;
 		int dia, mes, anio;
 		int hora, minutos;
		string consulta = "SELECT * FROM vuelos";
        int query = mysql_query(conn, consulta.c_str());

        if (query != 0){
          cout << "Ha ocurrido un error al realizar la consulta" << endl;      
        }else{
          	MYSQL_RES * resultado = mysql_store_result(conn);
         	unsigned long filas_afectadas = mysql_num_rows(resultado);
          	MYSQL_ROW fila;
        	for (int x = 0; x < filas_afectadas; x++){
          		TpVuelo nuevo = new(struct Vuelo);
        		fila = mysql_fetch_row(resultado);
        		nuevo->idVuelo = atoi(fila[0]);
            	nuevo->destino = fila [1];
				string fecha = fila[2];
		    	stringstream ss(fecha);
		    	char delimiter = '-';
		
			    string diaStr, mesStr, anioStr;
			    getline(ss, anioStr, delimiter);
			    getline(ss, mesStr, delimiter);
			    getline(ss, diaStr, delimiter);
			
			    stringstream diaSS(diaStr);
			    stringstream mesSS(mesStr);
			    stringstream anioSS(anioStr);
			    diaSS >> dia;
			    mesSS >> mes;
			    anioSS >> anio;
				nuevo->fecha.dia = dia;
	 			nuevo->fecha.mes = mes;
				nuevo->fecha.anio = anio;
				string tiempo = fila[3];
				stringstream sst(tiempo);
			    char delimitador = ':'; 
			
			    string horasStr, minutosStr, segundosStr;
			    getline(sst, horasStr, delimitador);
			    getline(sst, minutosStr, delimitador);
	
			    stringstream horasSS(horasStr);
			    horasSS >> hora;
			
			    stringstream minutosSS(minutosStr);
			    minutosSS >> minutos;
				nuevo->hora.hora = hora; 
				nuevo->hora.minuto = minutos;
				nuevo->clase = fila[4];
				nuevo->precio = atof(fila[5]);
				nuevo->aforo = atoi(fila [6]);
				nuevo->sgte = NULL;
	    		nuevo->ant = NULL;
				insertarVuelo(vuelo, nuevo);
          }
          // Liberar el resultado de la consulta
          mysql_free_result(resultado);
        }
}

// Función para buscar un pasajero por su id en la base de datos
bool buscarPasajero(TpPasajero pasajero, int idPasajero){
	while(pasajero != NULL){
		if (idPasajero == pasajero->idPasajero)
			return true;
		pasajero = pasajero->sgte;
	}
	return false;
}

MYSQL* conectarBaseDatos(){
    MYSQL* conn = mysql_init(NULL);
    if (!mysql_real_connect(conn, HOST, USER, PASS, "db_vuelos", 0, NULL, 0)) {
        cout << "Error al conectar a la base de datos: " << mysql_error(conn) << endl;
        exit(1);
    }
    return conn;
}

TpArbol crearNodo (TpVuelo vuelo){
	TpArbol nuevo = NULL;
	nuevo = new(struct arbol);
	nuevo->vuelo = vuelo;
	nuevo->izq = NULL;
	nuevo->der = NULL;
	return nuevo;
}

void llenarArbol(TpArbol &arbol, TpArbol vuelo){
	TpArbol p = arbol;
	if (arbol == NULL){
		arbol = vuelo;
	}
	else{
		while(p != NULL){
			if (vuelo->vuelo->fecha.dia <= p->vuelo->fecha.dia){
				if (p->izq == NULL){
						p->izq = vuelo;
					break;
				}else{
					p = p->izq;
				}
			}else
				if (p->der == NULL){
						p->der = vuelo;
					break;
				}else{
					p = p->der;
				}	
		}	
	}
}

void filtro (TpVuelo vuelo, TpArbol &arbol, string destino, date fecha){
	TpVuelo p = vuelo; 
	TpVuelo arreglo [10];
	int i = 0;
	TpArbol nodo;
	while (p != NULL){
		if ((p->destino == destino) && (p->fecha.anio == fecha.anio)){
			if ((p->fecha.mes == fecha.mes) && (p->fecha.dia == fecha.dia)){
				nodo = crearNodo (p);
				llenarArbol(arbol, nodo);
			}else if ((p->fecha.mes == fecha.mes) && (p->fecha.dia > (fecha.dia - 5) || p->fecha.dia < (fecha.dia + 5))){
				if (i < 10)
					arreglo[i] = p;
				i++;			
			}
		}
		p = p->sgte;
	}
	i--;
	TpArbol nodo1;
	while (i >= 0){
		nodo1 = crearNodo (arreglo[i]);
		llenarArbol(arbol, nodo1);
		i--;
	}
}

string subCadena(string destino, TpVuelo vuelo) {
    int m = destino.length();
    int maxLen = 0;
    string destinoComun = "";

    while (vuelo != NULL) {
        string destinoVuelo = vuelo->destino;
        int n = destinoVuelo.length();
        vector< vector<int> > dp(m + 1, vector<int>(n + 1, 0));

        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                if (destino[i - 1] == destinoVuelo[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1] + 1;
                    if (dp[i][j] > maxLen) {
                        maxLen = dp[i][j];
                        destinoComun = vuelo->destino;
                    }
                }
            }
        }
        vuelo = vuelo->sgte; 
    }

    return destinoComun;
}

void pantallafiltro (TpVuelo vuelo, TpArbol &arbol){
	string destino, parecido;
	date fecha;
	int opc;
	bool band = true;
	cout << "Buscar por disponibilidad:" << endl;
	cout << "Digite la ciudad de destino:\n -> ";
	cin >> destino;
	parecido = subCadena(destino, vuelo);
	if (parecido != ""){
		band = false;
		cout << "\nQuizás quisiste decir " << parecido << endl << "Selecciona una opcion:" << endl;
		cout << "\t1. Sí\t2. No\n\t ->";
		cin>> opc;
		if (opc == 1){
			destino = parecido;
			band = true;
		}
	}else
		band = false;
	if (band){
		cout << "Seleccione fecha:\n\t- Año (AAAA):\n\t  -> ";
		cin >> fecha.anio;
		cout << "\t- Mes (MM):\n\t  -> ";
		cin >> fecha.mes;
		cout << "\t- Día (dd):\n\t  -> ";
		cin >> fecha.dia;
		filtro (vuelo, arbol, destino, fecha);
	}
	
}

void preOrden(TpArbol arbol) {
	if (arbol != NULL) {
    	cout << "------------------------------------------------------------------------------------------------------------------" << endl;
		mostrarVuelo (arbol->vuelo);
		cout << endl;
		preOrden(arbol->izq);
        preOrden(arbol->der);
    }
}

int pantallaReserva (){
	int opc = 0;
		system ("cls");
		if (opc > 2 || opc < 0)
			cout << "\t\t\tError. Elija una opcion válida por favor." << endl;
		cout << "\t\t\t\tRESERVAR VUELO" << endl;
		cout << "\t 1. Reservar por id." << endl;
		cout << "\t 2. Buscar por destino y fecha, y reservar." << endl;
		cout << "\t 0. Cancelar.\n\tElija una opcion:\n\t ->";
		cin >> opc;
	while (opc > 2 || opc < 0)
	return opc;
}

void realizarReservaPorFecha(TpPasajero pasajero, TpVuelo v, MYSQL* conn) {
    TpVuelo q = v;
    TpArbol arbol = NULL;
	pantallafiltro (v, arbol);
	if (arbol == NULL){
		cout << "\t\t\tNo se han encontrado resultados." << endl;
	}else{
		cout << " RESULTADOS DE BÚSQUEDA:" << endl;
		preOrden(arbol);
		reservarPorId (pasajero, v, conn);
	}
}

void reservarPorId (TpPasajero pasajero, TpVuelo v, MYSQL* conn){
	int idVuelo, idPasajero;
	
	cout << "\n - Introduce el id del vuelo: ";cin >> idVuelo;
	TpVuelo vuelo = buscarVuelo(v, idVuelo);
	
	
		if (vuelo == NULL) {
		    cout << "Vuelo no encontrado" << endl;
		}else{
			if(vuelo->aforo>0){
				cout << "Introduce el documento del pasajero: ";
				cin >> idPasajero;
				if (!buscarPasajero (pasajero, idPasajero)) {
				    cout << "Pasajero no encontrado" << endl;
				}else{
				 	guardarReserva(idPasajero, idVuelo, conn);
					 vuelo->aforo = vuelo->aforo - 1;
				 	actualizarVuelo(vuelo, conn);
				  	cout << "Reserva exitosa." << endl;
				}
			}else{
				cout<<"\nAFORO MAXIMO"<<endl;
			}
		}	
	
}

void guardarReserva (int idPasajero, int idVuelo, MYSQL* conn){
	string query = "INSERT INTO reserva (id_pasajero, id_vuelo) VALUES (";
        query += to_string(idPasajero) + ", ";
        query += to_string(idVuelo) + ")";

        if (mysql_query(conn, query.c_str()) != 0) {
            cout << "Error al ejecutar la consulta: " << mysql_error(conn) << endl;
            exit(1);
        }
}

//Funciones para registrar vuelo
TpVuelo crearNodoVuelo() {
    TpVuelo nuevo = new Vuelo;
    int clase;
    bool diaValido; // Declarar la variable diaValido aquí

    do {
        cout << "\t\tIngresar destino:\n -> ";
        cin >> nuevo->destino;
        cout << "\t\tIngresar año (AAAA):\n -> ";
        cin >> nuevo->fecha.anio;
        cout << "\t\tIngresar mes (MM):\n -> ";
        cin >> nuevo->fecha.mes;
        cout << "\t\tIngresar día (dd):\n -> ";
        cin >> nuevo->fecha.dia;
        cout << "\t\tIngresar hora de salida:\n -> ";
        cin >> nuevo->hora.hora;
        cout << "\t\tIngresar minuto de salida:\n -> ";
        cin >> nuevo->hora.minuto;
        cout << "\t\tSeleccione tipo de vuelo: \n\t0: Economica\n\t1: Ejecutiva\n\t2: Primera clase\n -> ";
        cin >> clase;
        if (clase == 0)
            nuevo->clase = "Economica";
        else if (clase == 1)
            nuevo->clase = "Ejecutiva";
        else if (clase == 2)
            nuevo->clase = "Primera clase";
        cout << "\t\tIngresar precio:\n -> ";
        cin >> nuevo->precio;
        cout << "\t\tIngresar aforo:\n -> ";
        cin >> nuevo->aforo;

        bool esBisiesto = (nuevo->fecha.anio % 4 == 0 && nuevo->fecha.anio % 100 != 0) || nuevo->fecha.anio % 400 == 0;
        diaValido = (nuevo->fecha.dia <= 31 &&
                          ((nuevo->fecha.mes == 1 || nuevo->fecha.mes == 3 || nuevo->fecha.mes == 5 || nuevo->fecha.mes == 7 ||
                            nuevo->fecha.mes == 8 || nuevo->fecha.mes == 10 || nuevo->fecha.mes == 12) && nuevo->fecha.dia <= 31) ||
                          ((nuevo->fecha.mes == 4 || nuevo->fecha.mes == 6 || nuevo->fecha.mes == 9 || nuevo->fecha.mes == 11) && nuevo->fecha.dia <= 30) ||
                          (nuevo->fecha.mes == 2 && (esBisiesto && nuevo->fecha.dia <= 29) || (!esBisiesto && nuevo->fecha.dia <= 28)));

        if (nuevo->fecha.anio < 2023 || nuevo->fecha.mes < 1 || nuevo->fecha.mes > 12 || !diaValido ||
            nuevo->hora.hora < 0 || nuevo->hora.hora > 23 ||
            nuevo->hora.minuto < 0 || nuevo->hora.minuto > 59 ||
            nuevo->precio < 0 || nuevo->aforo < 0) {
            cout << "ERROR AL INGRESAR LOS DATOS, INGRESE LOS DATOS CORRECTAMENTE" << endl;
        }
    } while (nuevo->fecha.anio < 2023 || nuevo->fecha.mes < 1 || nuevo->fecha.mes > 12 || !diaValido ||
             nuevo->hora.hora < 0 || nuevo->hora.hora > 23 ||
             nuevo->hora.minuto < 0 || nuevo->hora.minuto > 59 ||
             nuevo->precio < 0 || nuevo->aforo < 0);

    nuevo->sgte = NULL;
    nuevo->ant = NULL;
    return nuevo;
}

void insertarVuelo(TpVuelo &vuelo, TpVuelo nuevo) {
    TpVuelo p = vuelo;
	if (vuelo == NULL) {
        vuelo = nuevo;
    } else {
        while(p->sgte != NULL)
			p=p->sgte;
		p->sgte = nuevo;
		nuevo->ant = p;
    }
}


void mostrarVuelosOrdenados(TpVuelo vuelo) {
    if (vuelo == NULL) {
        cout << "No hay vuelos registrados." << endl;
        return;
    }
    // Ordenar los vuelos por destino utilizando Bubble Sort
    TpVuelo p, q;
    bool cambios;

    do {
        cambios = false;
        p = vuelo;
        while (p->sgte != NULL){
            q = p->sgte;
            if (p->destino > q->destino) {
                // Intercambiar los destinos
                string tempString = p->destino;
                p->destino = q->destino;
                q->destino = tempString;
                intercambiar (p->fecha.anio, q->fecha.anio);
                intercambiar (p->fecha.mes, q->fecha.mes);
                intercambiar (p->fecha.dia, q->fecha.dia);
                intercambiar (p->hora.hora, q->hora.hora);
                intercambiar (p->hora.minuto, q->hora.minuto);
                tempString = p->clase;
                p->clase = q->clase;
                q->clase = tempString;
                intercambiar (p->aforo, q->aforo);
                intercambiar (p->idVuelo, q->idVuelo);
				double tempPrecio = p->precio;
				p->precio = q->precio;
				q->precio = tempPrecio;
                cambios = true;
            }
            p = p->sgte;
        }
    } while (cambios);
    // Mostrar los vuelos ordenados por destino
    p = vuelo;
    string destino;
    cout << "\t\t\t\tVUELOS PROGRAMADOS" << endl;
    while (p != NULL) {
    	if (p->destino != destino){
    		cout << "\n\n----------------------------------------------- Destino: " << p->destino << " ---------------------------------------------------- " << endl;
			destino = p->destino;
		}
        mostrarVuelo(p);
        p = p->sgte;
        cout << endl;
    }
}

void intercambiar (int &op1, int &op2){
	int temp = op1;
	op1 = op2;
	op2 = temp;
}

void mostrarVuelo(TpVuelo p){
	cout << "\n   ID Vuelo |\tDestino      | \t   Fecha       |     Hora      |   Tipo de Clase  |  Precio  | Asientos disponibles" <<endl;
	cout <<"   "<<p->idVuelo;  
	cout <<"\t         "<<p->destino;
	cout <<"\t        "<<p->fecha.dia << "/" << p->fecha.mes << "/" << p->fecha.anio;
	cout <<"\t   "<<p->hora.hora << ":" << p->hora.minuto;
	cout <<" \t  "<<p->clase;
	cout << "\t\t" << p->precio;	
	cout <<"\t\t "<<p->aforo;    
}

void guardarVuelosEnBD(TpVuelo vuelo, MYSQL* conn) {
        string query = "INSERT INTO vuelos (destino, fecha, hora, tipo_clase, precio, aforo) VALUES (";
        query += "'" + vuelo->destino + "', ";
        query += "'" + to_string(vuelo->fecha.anio) + "-" + to_string(vuelo->fecha.mes) + "-" + to_string(vuelo->fecha.dia) + "', ";
        query += "'" + to_string(vuelo->hora.hora) + ":" + to_string(vuelo->hora.minuto) + "', '";
        query += vuelo->clase + "', ";
        query += to_string(vuelo->precio) + ", ";
        query += to_string(vuelo->aforo) + ")";

        if (mysql_query(conn, query.c_str()) != 0) {
            cout << "Error al ejecutar la consulta: " << mysql_error(conn) << endl;
            exit(1);
        }
}

// Funciones para el registro de pasajeros
string consistenciarDocumento(string opciones, string op1, string op2){ 
	int op;
	do{ 
		cout << "Seleccionar " << opciones << "\n 1) "<<op1 << "\n 2)"<< op2 << "\n -> ";
		cin >> op;
		if (op != 1 && op != 2)
			cout << "Intente de nuevo. Fuera de rango" << endl;
	}while(op != 1 && op != 2);
	if (op == 1)
		return op1;
	else
		return op2;		
}

TpPasajero crearNodoPasajero(){
	TpPasajero nuevo = NULL;
	nuevo = new(struct Pasajero);
	nuevo->tipoDoc = consistenciarDocumento("Tipo de documento: ", "DNI", "Carnet de extranjeria");
	do{
	cout << "\t\tIngresar Documento del pasajero:\n -> ";
    cin >> nuevo->idPasajero;
    if(nuevo->idPasajero<10000000 || nuevo->idPasajero>99999999){
    	cout<<"\n\tCantidad de digitos incorrecto. Vuelva a intentarlo\n"<<endl;
	}
	}while(nuevo->idPasajero<10000000 || nuevo->idPasajero>99999999);
    cout << "\t\tIngresar nombres:\n -> ";cin >> nuevo->nombres;
    fflush(stdin);
    cout << "\t\tIngresar apellidos:\n -> ";cin >> nuevo->apellidos;
    fflush(stdin);
    cout << "\t\tIngresar edad:\n -> ";cin >> nuevo->edad;
    fflush(stdin);
    cout << "\t\tIngresar telefono:\n -> ";cin >> nuevo->telefono;
    fflush(stdin);
	nuevo->sgte = NULL;
	return nuevo;
}

void insertarPasajero(TpPasajero &pasajero, TpPasajero nuevo){ 
	TpPasajero p = pasajero;
	
	if (pasajero == NULL) {
        pasajero = nuevo;
    } else {
        while(p->sgte != NULL)
			p = p->sgte;
		p->sgte= nuevo;
    }
}

void guardarPasajerosEnBD(TpPasajero pasajero, MYSQL* conn) {
    TpPasajero p = pasajero;
        string query = "INSERT INTO pasajeros (id_pasajero, nombres, apellidos, edad, telefono) VALUES (";
        query += to_string(p->idPasajero) + ", ";
        query += "'" + p->nombres + "', ";
        query += "'" + p->apellidos + "', ";
        query += to_string(p->edad) + ", ";
        query += "'" + p->telefono + "')";

        if (mysql_query(conn, query.c_str()) != 0) {
            cout << "Error al ejecutar la consulta: " << mysql_error(conn) << endl;
            exit(1);
        }
}

void modificarVuelo(int id_vuelo, TpVuelo vuelo, MYSQL* conn){
	TpVuelo p;
	bool comp1, comp2;
	system("cls");
	p = buscarVuelo(vuelo, id_vuelo);
	if (p == NULL){
		cout << "\nId no encontrado." << endl;
	}else{
		mostrarVuelo(p);
		cout << "\nDigite los nuevos datos:\n";
		do{
			cout << " - Fecha:\n";
			cout << "\tDia:\n\t -> ";
			cin >> p->fecha.dia;
			cout << "\tMes:\n\t -> ";
			cin >> p->fecha.mes;
			cout << "\tAnio:\n\t -> ";
			cin >> p->fecha.anio;
			comp1 = (p->fecha.dia > 31 || p->fecha.dia < 0) && (p->fecha.mes > 12 || p->fecha.mes < 0) && (p->fecha.anio < 2023);
			if (comp1)
				cout << "\n\t\tDigite una fecha válida por favor." << endl;
		}while(comp1);
		cout << " - Hora de salida:\n";
		do{
			cout << "\tHora:\n\t -> ";
			cin >> p->hora.hora;
			cout << "\tMinuto:\n\t -> ";
			cin >> p->hora.minuto;
			comp2 = (p->hora.hora > 24 || p->hora.hora < 0) && (p->hora.minuto > 60 || p->hora.minuto < 0);
			if (comp2)
				cout << "\n\t\tDigite una hora válida por favor." << endl; 
		}while(comp2);
		cout << " - Precio:\n  ->";
		cin >> p->precio;
		cout << " - Aforo:\n  ->";
		cin >> p->aforo;
		actualizarVuelo(p, conn);
		cout << "Vuelo modificado :D" << endl;
	}
}

TpVuelo buscarVuelo(TpVuelo p, int idVuelo){
	while(p != NULL){
		if (idVuelo == p->idVuelo)
			return p;
		p = p->sgte;
	}
	return NULL;
}

void actualizarVuelo(TpVuelo vuelo, MYSQL* conn) {
        string query = "UPDATE vuelos SET fecha = ";
        query += "'" + to_string(vuelo->fecha.anio) + "-" + to_string(vuelo->fecha.mes) + "-" + to_string(vuelo->fecha.dia) + "', ";
        query += "hora = '" + to_string(vuelo->hora.hora) + ":" + to_string(vuelo->hora.minuto) + "', precio = ";
        query += to_string(vuelo->precio) + ", aforo = ";
        query += to_string(vuelo->aforo) + " WHERE id_vuelo = " + to_string(vuelo->idVuelo);

        if (mysql_query(conn, query.c_str()) != 0) {
            cout << "Error al ejecutar la consulta: " << mysql_error(conn) << endl;
            exit(1);
        }
}

// Funciones para el registro e inicio de sesión

void Ingresar_PASS(Persona_R &p) {
    cout << "Escriba su password: ";
    getPassword(p.PASS, MAXCAD);
    cout << endl;
    cout << "\nSE HA REGISTRADO CORRECTAMENTE" << endl;
}

void getPassword(char* password, int maxLength) {
    int index = 0;
    char ch;
    while (index < maxLength - 1) {
        ch = _getch();
        if (ch == '\r') {
            break; 
        } else if (ch == '\b') {
            if (index > 0) {
                cout << "\b \b";
                index--;
            }
        } else {
            cout << "*";
            password[index] = ch;
            index++;
        }
    }
    password[index] = '\0'; 
}

void confirmar_PASS(Persona_R &password) {
    cout << "Escriba su password: ";
    getPassword(password.PASS, MAXCAD);
}

void insertarPersonaTXT(TCadena nombreFichero, Persona_R p) {
    ofstream out;
    out.open(nombreFichero, ios::app);
    if (out.bad()) {
        out.open(nombreFichero);
    }
    EscribePersonaFicheroTXT(out, p);
    out.close();
}

void EscribePersonaFicheroTXT(ofstream &fichero, Persona_R p) {
    fichero << p.PASS << SP;
}

void verificar_existencia_fichero(TCadena nombreFichero) {
    ifstream archivo(nombreFichero);
    if (!archivo) {
        cout << "\n\tUsuario no registrado\n" << endl;
        system("pause");
        system("cls");
        main();
    } else {
        cout << "\n\tNombre de Usuario CORRECTO\n" << endl;
        
    }
}

void verificar_existencia_de_usuario(TCadena nombreFichero) {
    ifstream archivo(nombreFichero);
    if (!archivo) {
        cout << "\n\tNombre de Usuario disponible\n" << endl;
        
    } else {
        cout << "\n\tNombre de Usuario no disponible. Vuelva a intentarlo" << endl;
        system("pause");
        system("cls");
        main();
    }
}

void verificar_contrasena(TCadena nombreFichero, Persona_R &password) {
    string cadena;
    string contrase;
    do{
	    cout << "Escriba su password: ";
	    getPassword(password.PASS, MAXCAD);
	    ifstream fichero(nombreFichero, ios::in);
		
	    while (!fichero.eof()) {
	        fichero >> cadena;
	    }
    	if (cadena != password.PASS)
    		cout << "\nPassword incorrecta. INTENTELO DE NUEVO "<<endl;
		fichero.close();
	}while (cadena != password.PASS);
    if (cadena == password.PASS) {
        cout << "\n\n\t\tPassword correcto. \n" << endl;
    } 
}
