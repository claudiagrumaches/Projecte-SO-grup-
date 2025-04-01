#include <stdio.h>              // Para funciones de entrada/salida (printf, perror)
#include <stdlib.h>             // Para funciones generales (exit)
#include <string.h>             // Para funciones de manejo de cadenas (strcpy, strcat, memset, etc.)
#include <unistd.h>             // Para funciones de Unix (close, read, write)
#include <sys/types.h>          // Para tipos de datos relacionados con sockets
#include <netinet/in.h>         // Para la estructura sockaddr_in y constantes de dominio
#include <mysql/mysql.h>        // Libreria para conectarse a MySQL

#define PORT 9080               // Definimos el puerto en el que el servidor escuchara

// Funcion para mostrar un mensaje de error y terminar el programa
void error(const char *msg) {
	perror(msg);
	exit(1);
}	
int main(int argc, char *argv[]) 
{
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
	if (mysql_real_connect(conn, "localhost", "usuari", "rummykub", "rummykub", 0, NULL, 0) == NULL) {
		fprintf(stderr, "Error connectant a MySQL: %s\n", mysql_error(conn));
		mysql_close(conn);                   // Cierra la conexi￳n en caso de error
		return 1;
	}
	// CREACIￓN DEL SOCKET DEL SERVIDOR
	// Se crea un socket para la comunicaciónn (TCP/IP)
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
	// El bind asocia el socket creado con la direcciónn y puerto definidos en serv_adr
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		error("Error al bind");
	// PONER EL SOCKET EN MODO ESCUCHA (LISTEN)
	// Permite que el servidor escuche conexiones entrantes. El segundo par￡metro indica el n mero m￡ximo de conexiones en cola.
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
		printf("Connexió rebuda\n");
		
		// Bucle para procesar peticiones del cliente conectado.
		// La variable 'terminar' se usar￡ para salir del bucle cuando el cliente finalice la comunicaci￳n.
		int terminar = 0;
		while (!terminar) 
	{
			// Se lee la petici￳n enviada por el cliente y se guarda en el buffer 'peticion'
			ret = read(sock_conn, peticion, sizeof(peticion));
			if (ret <= 0) { // Si no se recibe nada o se produce un error, salimos del bucle
				break;
			}
			peticion[ret] = '\0';  // Se finaliza la cadena con un caracter nulo
			printf("Petici￳n: %s\n", peticion);
			
			// Se separa la petici￳n usando "/" como delimitador
			char *p = strtok(peticion, "/");
			int codigo = atoi(p);  // Se convierte el primer token en un n mero que representa el c￳digo de operaci￳n
			// Si el código es 0, se entiende que el cliente finaliza la conexi￳n
			if (codigo == 0) {
				terminar = 1;
			}
			// Código 3: Registro de usuario
			else if (codigo == 3) { 
				char usuari[50], contrasenya[50];
				p = strtok(NULL, "/");
				strcpy(usuari, p);  // Se obtiene el nombre de usuario
				p = strtok(NULL, "/");
				strcpy(contrasenya, p);  // Se obtiene la contrase￱a
				char queryQ[256];
				sprintf(queryQ, "SELECT contrasenya FROM jugadors WHERE jugadors.nom = '%s';", usuari);
				if (mysql_query(conn, queryQ) == 0) {
					MYSQL_RES *result = mysql_store_result(conn);
					MYSQL_ROW row = mysql_fetch_row(result);
					if (row == NULL){
						char query[256];
				    // Se arma la consulta SQL para insertar el nuevo usuario en la tabla 'usuaris'
					    sprintf(query, "INSERT INTO jugadors (nom, contrasenya) VALUES ('%s', '%s');", usuari, contrasenya);
					// Se ejecuta la consulta
				        if (mysql_query(conn, query) == 0){
						    strcpy(respuesta, "Usuari registrat amb exit");
					    }
					
				        else {
						   strcpy(respuesta, "Error en el registre");
					    }
					
					}
					
					else
						strcpy(respuesta, "Ya existeix aquest nombre d'usuari");
					
					mysql_free_result(result);
				}
				
			}
			
			else if (codigo == 2) {
				char usuari[50], contrasenya[50];
				p = strtok(NULL, "/");
				strcpy(usuari, p);  // Se obtiene el nombre de usuario
				p = strtok(NULL, "/");
				strcpy(contrasenya, p);  // Se obtiene la contrase￱a
				char query[256];
				sprintf(query, "SELECT contrasenya FROM jugadors WHERE jugadors.nom = '%s';", usuari);
				if (mysql_query(conn, query) == 0) {
					MYSQL_RES *result = mysql_store_result(conn);
					MYSQL_ROW row = mysql_fetch_row(result);
					if (row == NULL)
					{
						strcpy(respuesta, "No existeix aquest usuari");
					}
					
					else
					{
						if (contrasenya == "%s", row[0])
							sprintf(respuesta, "Hola de nou, '%s'!", usuari);
						else
							sprintf(respuesta, "La contrasenya no coincideix amb l'usuari, torna-ho a intentar");
					}
					
					mysql_free_result(result);
			}
			}
			// Código 4: Consulta de puntos totales del jugador
			else if (codigo == 4) { 
				char usuari[50];
				p = strtok(NULL, "/");
				strcpy(usuari, p);  // Se obtiene el ID de la partida
				char query[256];
				// Se arma la consulta SQL para sumar los puntos del usuario en la tabla 'partides'
				sprintf(query, "SELECT punts_totals FROM jugadors WHERE nom = '%s';", usuari);
				if (mysql_query(conn, query) == 0) {
					MYSQL_RES *result = mysql_store_result(conn);
					MYSQL_ROW row = mysql_fetch_row(result);
					if (row != NULL)
					{
					    sprintf(respuesta, "Punts totals: %s", row[0] ? row[0] : "0");
					    
					}
					else
					{
						sprintf(respuesta, "No hay datos del usuario");
					}
				
					mysql_free_result(result);  // Se libera la memoria del resultado
					// Se muestra el total de puntos o "0" si no hay resultados
				}
				
				else
				{
					strcpy(respuesta, "Error en la consulta");
				}
			}
			// Código 5: Consulta de puntos de cada jugador en una partida
			else if (codigo == 5) 
		{ 
			char id_partida[10];
			p = strtok(NULL, "/");
			strcpy(id_partida, p);  // Se obtiene el ID de la partida
    
			char query[256];
			//consulta SQL, obteniendo el nombre del jugador con una subconsulta
			sprintf(query, 
			"SELECT nom, punts FROM jugadors, partides, jugador_partida WHERE jugadors.id_jugador = jugador_partida.id_jugador AND partides.id_partida = jugador_partida.id_partida AND partides.id_partida = '%s';", id_partida);
    
			if (mysql_query(conn, query) == 0) 
			{
				MYSQL_RES *result = mysql_store_result(conn);
				MYSQL_ROW row;
				strcpy(respuesta, "Punts per jugador:\n");
        
				while ((row = mysql_fetch_row(result))) 
				{
					char linea[100];
					sprintf(linea, "%s: %s punts\n", row[0], row[1]);
					strcat(respuesta, linea);
				}
        
				mysql_free_result(result);  // Se libera la memoria del resultado
			} 		
				else 
				{
					strcpy(respuesta, "Error en la consulta");
				}
		}

			// Código 6: Consulta del récord de puntos en una partida
			else if (codigo == 6) 
		{ 
			char id_partida[10];
			p = strtok(NULL, "/");
			strcpy(id_partida, p);  // Se obtiene el ID de la partida
			char query[256];
		// la consulta SQL para obtener el récord de puntos en una partida
			sprintf(query, "SELECT record_punts FROM partides WHERE partides.id_partida = '%s';", id_partida);
    
			if (mysql_query(conn, query) == 0) 
			{
			MYSQL_RES *result = mysql_store_result(conn);
			MYSQL_ROW row = mysql_fetch_row(result);
			// Se muestra el récord de puntos o "0" si no hay resultados
			sprintf(respuesta, "Record de punts: %s", row[0] ? row[0] : "0");
			mysql_free_result(result);  // Se libera la memoria del resultado
			} 
		else {
			strcpy(respuesta, "Error en la consulta");
			}
		}
	
 
			// Se envia la respuesta generada al cliente
			write(sock_conn, respuesta, strlen(respuesta));
			printf("Resposta: %s\n", respuesta);
		}

	}
		// Se cierra la conexi￳n con el cliente actual
	close(sock_conn);
	
	
	// Cierre de la conexión a MySQL (nunca se alcanza en este ejemplo por el bucle infinito)
mysql_close(conn);
return 0;
}
