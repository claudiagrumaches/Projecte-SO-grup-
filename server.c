#include <stdio.h>              // Para funciones de entrada/salida (printf, perror)
#include <stdlib.h>             // Para funciones generales (exit)
#include <string.h>             // Para funciones de manejo de cadenas (strcpy, strcat, memset, etc.)
#include <unistd.h>             // Para funciones de Unix (close, read, write)
#include <sys/types.h>          // Para tipos de datos relacionados con sockets
#include <netinet/in.h>         // Para la estructura sockaddr_in y constantes de dominio
#include <mysql/mysql.h>        // Libreria para conectarse a MySQL

#define PORT 9000               // Definimos el puerto en el que el servidor escuchara

// Funcion para mostrar un mensaje de error y terminar el programa
void error(const char *msg) {
	perror(msg);
	exit(1);
}	
int main(int argc, char *argv[]) {
	int sock_conn, sock_listen, ret;         // sock_listen: descriptor para el socket de escucha, sock_conn: descriptor para la conexi￳n del cliente
	struct sockaddr_in serv_adr;             // Estructura para almacenar la direcci￳n del servidor
	char peticion[512], respuesta[512];      // Buffers para almacenar la peticionn y la respuesta

// CONEXION A LA BASE DE DATOS MySQL

	MYSQL *conn;
	conn = mysql_init(NULL);                 // Inicializa la conexi￳n a MySQL
	if (conn == NULL) {
		fprintf(stderr, "Error inicialitzant MySQL\n");
		return 1;
	}
	// Se conecta a la base de datos con los par￡metros: host, usuario, contraseￃﾱa y nombre de la BD
	if (mysql_real_connect(conn, "localhost", "usuari", "mysql", "rummykub", 0, NULL, 0) == NULL) {
		fprintf(stderr, "Error connectant a MySQL: %s\n", mysql_error(conn));
		mysql_close(conn);                   // Cierra la conexi￳n en caso de error
		return 1;
	}
	// CREACIￓN DEL SOCKET DEL SERVIDOR
	// Se crea un socket para la comunicaci￳n (TCP/IP)
	sock_listen = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_listen < 0)
		error("Error creant socket");
	// CONFIGURACIￓN DE LA DIRECCIￓN DEL SERVIDOR
	// Se limpia la estructura para evitar datos basura
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;                  // Establece que se usar￡ IPv4
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);     // Acepta conexiones de cualquier interfaz de red
	serv_adr.sin_port = htons(PORT);                // Establece el puerto (se convierte al formato de red)
	// ASOCIAR (BIND) EL SOCKET A LA DIRECCIￓN Y PUERTO
	// El bind asocia el socket creado con la direcciￃﾳn y puerto definidos en serv_adr
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		error("Error al bind");
	// PONER EL SOCKET EN MODO ESCUCHA (LISTEN)
	// Permite que el servidor escuche conexiones entrantes. El segundo par￡metro indica el n￺mero m￡ximo de conexiones en cola.
	if (listen(sock_listen, 3) < 0)
		error("Error en el listen");
	
	printf("Servidor escoltant al port %d...\n", PORT);
	// BUCLE INFINITO PARA ACEPTAR CONEXIONES DE CLIENTES
	while (1) {
		// accept() bloquea y espera hasta que un cliente se conecte.
		// No se necesita informaci￳n del cliente (por eso se pasa NULL en el segundo y tercer par￡metro)
		sock_conn = accept(sock_listen, NULL, NULL);
		if (sock_conn < 0) {
			perror("Error en accept");
			continue; // Si ocurre un error al aceptar, se sigue al siguiente ciclo
		}
		printf("Connexi￳ rebuda\n");
		
		// Bucle para procesar peticiones del cliente conectado.
		// La variable 'terminar' se usar￡ para salir del bucle cuando el cliente finalice la comunicaci￳n.
		int terminar = 0;
		while (!terminar) {
			// Se lee la petici￳n enviada por el cliente y se guarda en el buffer 'peticion'
			ret = read(sock_conn, peticion, sizeof(peticion));
			if (ret <= 0) { // Si no se recibe nada o se produce un error, salimos del bucle
				break;
			}
			peticion[ret] = '\0';  // Se finaliza la cadena con un caracter nulo
			printf("Petici￳n: %s\n", peticion);
			
			// Se separa la petici￳n usando "/" como delimitador
			char *p = strtok(peticion, "/");
			int codigo = atoi(p);  // Se convierte el primer token en un n￺mero que representa el c￳digo de operaci￳n
			// Si el c￳digo es 0, se entiende que el cliente finaliza la conexi￳n
			if (codigo == 0) {
				terminar = 1;
			}
			// C￳digo 3: Registro de usuario
			else if (codigo == 3) { 
				char usuari[50], contrasenya[50];
				p = strtok(NULL, "/");
				strcpy(usuari, p);  // Se obtiene el nombre de usuario
				p = strtok(NULL, "/");
				strcpy(contrasenya, p);  // Se obtiene la contrase￱a
				
				char query[256];
				// Se arma la consulta SQL para insertar el nuevo usuario en la tabla 'usuaris'
				sprintf(query, "INSERT INTO usuaris (nom, contrasenya) VALUES ('%s', '%s')", usuari, contrasenya);
				// Se ejecuta la consulta
				if (mysql_query(conn, query) == 0)
					strcpy(respuesta, "Usuari registrat amb ￨xit");
				else
					strcpy(respuesta, "Error en el registre");
			}
			// C￳digo 4: Consulta de puntos totales del jugador
			else if (codigo == 4) { 
				char usuari[50];
				p = strtok(NULL, "/");
				strcpy(usuari, p);  // Se obtiene el nombre del usuario
				char query[256];
				// Se arma la consulta SQL para sumar los puntos del usuario en la tabla 'partides'
				sprintf(query, "SELECT SUM(punts) FROM partides WHERE usuari = '%s'", usuari);
				if (mysql_query(conn, query) == 0) {
					MYSQL_RES *result = mysql_store_result(conn);
					MYSQL_ROW row = mysql_fetch_row(result);
					// Se muestra el total de puntos o "0" si no hay resultados
					sprintf(respuesta, "Punts totals: %s", row[0] ? row[0] : "0");
					mysql_free_result(result);  // Se libera la memoria del resultado
				} else {
					strcpy(respuesta, "Error en la consulta");
				}
			}
			// C￳digo 5: Consulta de puntos de cada jugador en una partida
			else if (codigo == 5) { 
				char id_partida[20];
				p = strtok(NULL, "/");
				strcpy(id_partida, p);  // Se obtiene el ID de la partida
				
				char query[256];
				// Consulta SQL para obtener el nombre del usuario y sus puntos en la partida especificada
				sprintf(query, "SELECT usuari, punts FROM partides WHERE id_partida = '%s'", id_partida);
						if (mysql_query(conn, query) == 0) {
							MYSQL_RES *result = mysql_store_result(conn);
							MYSQL_ROW row;
							strcpy(respuesta, "");  // Se inicializa la respuesta vac￭a
							// Se recorre cada fila del resultado y se van concatenando los datos al buffer 'respuesta'
							while ((row = mysql_fetch_row(result))) {
								char temp[100];
								sprintf(temp, "%s: %s punts\n", row[0], row[1]);
								strcat(respuesta, temp);
							}
							mysql_free_result(result);
				} else {
							strcpy(respuesta, "Error en la consulta");
						}
			}
			// C￳digo 6: Consulta del r￩cord de puntos en una partida
			else if (codigo == 6) { 
				char query[256];
				// Consulta SQL para obtener el usuario y su m￡ximo puntaje registrado
				sprintf(query, "SELECT usuari, MAX(punts) FROM partides");
				
				if (mysql_query(conn, query) == 0) {
					MYSQL_RES *result = mysql_store_result(conn);
					MYSQL_ROW row = mysql_fetch_row(result);
					sprintf(respuesta, "Record de punts: %s amb %s punts", row[0], row[1]);
					mysql_free_result(result);
				} else {
					strcpy(respuesta, "Error en la consulta");
				}
			}
			// Se env￭a la respuesta generada al cliente
			write(sock_conn, respuesta, strlen(respuesta));
		}
		// Se cierra la conexi￳n con el cliente actual
		close(sock_conn);
	}
	
	// Cierre de la conexi￳n a MySQL (nunca se alcanza en este ejemplo por el bucle infinito)
	mysql_close(conn);
	return 0;
}
