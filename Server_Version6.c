#include <stdio.h>              // Para funciones de entrada/salida (printf, perror)
#include <stdlib.h>             // Para funciones generales (exit)
#include <string.h>             // Para funciones de manejo de cadenas (strcpy, strcat, memset, etc.)
#include <unistd.h>             // Para funciones de Unix (close, read, write)
#include <sys/types.h>          // Para tipos de datos relacionados con sockets
#include <netinet/in.h>         // Para la estructura sockaddr_in y constantes de dominio
#include <mysql/mysql.h>        // Libreria para conectarse a MySQL
#include <pthread.h>

#define PORT 9080               // Definimos el puerto en el que el servidor escuchara

// Funcion para mostrar un mensaje de error y terminar el programa
void error(const char *msg) {
    perror(msg);
    exit(1);
}   

int contador;
//Estructura necesaria para acceso excluyente
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int i;
int sockets[100];

typedef struct {
    char nombre [20];
    int socket;
}Conectado;

typedef struct {
    Conectado conectados [100];
    int num;
}ListaConectados;

ListaConectados milista;

typedef struct {
	int id; //La primera partida creada ser� con ID = 3 ya que en nuestra base de datos ya creamos 2 partidas de prueba con ID = 1 y ID = 2 para probar el c�digo en su momento.
	// Podemos a�adir mas atributos a Partida cuando sea necesario
}Partida;

typedef struct {
	Partida partidas [100];
	int numPartidas;
}ListaPartidas;

ListaPartidas milistaP;

int DameSocketUsuario(char* nombreBuscado, int* socketUsuario)
{
	for (int i = 0; i < milista.num; i++)
	{
		if (strcmp(milista.conectados[i].nombre, nombreBuscado) == 0)
		{
			*socketUsuario = milista.conectados[i].socket;
			return 1; // Usuario encontrado
		}
	}
	
	return 0; // Usuario no encontrado
}


int Pon (ListaConectados *lista, char nombre [20], int socket){
    if ( lista ->num == 100)
        return -1;
    else {
        strcpy (lista->conectados[lista->num].nombre, nombre);
        lista->conectados[lista->num].socket = socket;
        lista->num++;
        return 0;
    }
        
    }

int DamePosicion (ListaConectados *lista, char nombre [20]){
    //Devuelve el socket o -1 si no est￡ en la lista
    int i = 0;
    int encontrado = 0;
    while ((i < lista->num) && !encontrado)
    {
        if (strcmp( lista->conectados[i].nombre,nombre) ==0)
            encontrado =1;
        if (!encontrado)
            i=i+1;
    }
    if (encontrado)
        return i; // Devuelve la posición en el array
    else 
        return -1;
}

int Eliminar (ListaConectados *lista, char nombre [20])
{
    int pos = DamePosicion (lista,nombre);
    if (pos==-1)
        return -1;
    else {
        int i;
        for(i=pos; i < lista->num-1; i++)
        {
            lista->conectados[i] = lista ->conectados[i+1];
        }
        lista->num--;
        return 0;
    }
}

void DameConectados (ListaConectados *lista, char conectados[300])
{
	conectados[0] = '\0'; // Inicializa la cadena vacía
	
	for (int i = 0; i < lista->num; i++)
	{
		strcat(conectados, lista->conectados[i].nombre);
		if (i < lista->num - 1)
			strcat(conectados, "/"); // Solo añade "/" si no es el último
	}
}


void *AtenderCliente (void *socket)
{
    int sock_conn;
    int *s;
    s = (int *) socket;
    sock_conn = *s;
    char peticion[512];
    char respuesta[512];
    int ret;
    
    
    MYSQL *conn;
    conn = mysql_init(NULL);                 // Inicializa la conexi￳n a MySQL
    if (conn == NULL) {
        fprintf(stderr, "Error inicialitzant MySQL\n");
        return 1;
    }
    // Se conecta a la base de datos con los par￡metros: host, usuario, contraseￃﾱa y nombre de la BD
    if (mysql_real_connect(conn, "localhost", "root", "mysql", "rummykub", 0, NULL, 0) == NULL) {
        fprintf(stderr, "Error connectant a MySQL: %s\n", mysql_error(conn));
        mysql_close(conn);                   // Cierra la conexi￳n en caso de error
        return 1;
    }
    
    
    //entramos en el bucle para atender las peticiones
    int terminar = 0;
    while (!terminar) 
    {
        // Se lee la petici￳n enviada por el cliente y se guarda en el buffer 'peticion'
        ret = read(sock_conn, peticion, sizeof(peticion));
        if (ret <= 0) { // Si no se recibe nada o se produce un error, salimos del bucle
            break;
        }
        peticion[ret] = '\0';  // Se finaliza la cadena con un caracter nulo
        printf("Peticion: %s\n", peticion);
        
        // Se separa la petici￳n usando "/" como delimitador
        char *p = strtok(peticion, "/");
        int codigo = atoi(p);  // Se convierte el primer token en un n mero que representa el c￳digo de operaci￳n
        // Si el código es 0, se entiende que el cliente finaliza la conexi￳n
		int numForm = 2; 
        if (codigo == 0) // Añadir ELiminar connectado
        { 
            char usuari[50];
            char conectados[300];
            p = strtok(NULL, "/");
            strcpy(usuari, p);
            int eliminar = Eliminar(&milista,usuari);
            if (eliminar== -1)
            {
                printf("El usuari no està connectat.\n");
            }
            else 
            {
                printf ("El usuari s'ha desconnectat.\n");
                // Enviar lista actualizada a todos los clientes
                DameConectados(&milista, conectados);
				char notificacion[400];
				sprintf(notificacion, "7/%s", conectados);
				for (int j = 0; j < milista.num; j++)
				{
					write(milista.conectados[j].socket, notificacion, strlen(notificacion));
				}
            }
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
                        strcpy(respuesta, "3/Usuari registrat amb exit");
                    }
                
                    else {
                        strcpy(respuesta, "3/Error en el registre");
                    }
                
                }
                
                else
                    strcpy(respuesta, "3/Ya existeix aquest nombre d'usuari");
                
                mysql_free_result(result);
            }
            
        }
        
        else if (codigo == 2)
        {
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
                    strcpy(respuesta, "2/No existeix aquest usuari");
                }
                
                else
                {
                    if (strcmp(contrasenya, row[0]) == 0)
                    {
                        int nuevoconn = Pon(&milista,usuari,sock_conn);
                        if ( nuevoconn == -1)
                        { 
                            strcpy(respuesta, "2/Error al iniciar sessió");
                        }
                        else 
                        {
                            sprintf(respuesta, "2/Hola de nou, '%s'!", usuari);
                            // Enviar lista actualizada a todos los clientes
							char conectados[300];
							DameConectados(&milista, conectados);
							
							char notificacion[400];
							sprintf(notificacion, "7/%s", conectados);
							
							printf("DEBUG - Enviando notificación: %s\n", notificacion);  // ← Útil para depurar
							
							for (int j = 0; j < milista.num; j++)
							{
								write(milista.conectados[j].socket, notificacion, strlen(notificacion));
							}
                        }
                    }
                    else
                        sprintf(respuesta, "2/La contrasenya no coincideix amb l'usuari, torna-ho a intentar");
                }
                
                mysql_free_result(result);
            }   
        }
        
        // Código 4: Consulta de puntos totales del jugador
        else if (codigo == 4) 
        { 
            char usuari[50];
            p = strtok(NULL, "/");
            strcpy(usuari, p);  // Se obtiene el nombre del jugador
            char query[256];

            sprintf(query, "SELECT punts_totals FROM jugadors WHERE nom = '%s';", usuari);
    
            if (mysql_query(conn, query) == 0) {
                MYSQL_RES *result = mysql_store_result(conn);
                MYSQL_ROW row = mysql_fetch_row(result);
        
                if (row != NULL) {
                    sprintf(respuesta, "4/Punts totals: %s", row[0] ? row[0] : "0");
                } else {
                    strcpy(respuesta, "4/No existe el jugador.");
                }
        
                mysql_free_result(result);
            } else {
                strcpy(respuesta, "4/Error en la consulta");
            }
        }

        // Código 5: Consulta de puntos de cada jugador en una partida
        else if (codigo == 5) 
        { 
            char id_partida[10];
            p = strtok(NULL, "/");
            strcpy(id_partida, p);  // Se obtiene el ID de la partida

            char query[256];
            sprintf(query, 
                "SELECT nom, punts FROM jugadors, partides, jugador_partida WHERE jugadors.id_jugador = jugador_partida.id_jugador AND partides.id_partida = jugador_partida.id_partida AND partides.id_partida = '%s';", id_partida);

            if (mysql_query(conn, query) == 0) 
            {
                MYSQL_RES *result = mysql_store_result(conn);
                MYSQL_ROW row;
                strcpy(respuesta, "5/Punts per jugador:\n");

                while ((row = mysql_fetch_row(result))) 
                {
                    char linea[100];
                    sprintf(linea, "%s: %s punts\n", row[0], row[1]);
                    strcat(respuesta, linea);
                }

                mysql_free_result(result);
            } 
            else 
            {
                strcpy(respuesta, "5/Error en la consulta");
            }
        }

        // Código 6: Consulta del récord de puntos en una partida
        else if (codigo == 6) 
        {
            char id_partida[10];
            p = strtok(NULL, "/");
            strcpy(id_partida, p);  // Se obtiene el ID de la partida

            char query[256];
            sprintf(query, "SELECT record_punts FROM partides WHERE partides.id_partida = '%s';", id_partida);

            if (mysql_query(conn, query) == 0) 
            {
                MYSQL_RES *result = mysql_store_result(conn);
                MYSQL_ROW row = mysql_fetch_row(result);

                sprintf(respuesta, "6/Record de punts: %s", row[0] ? row[0] : "0");

                mysql_free_result(result);
            } 
            else 
            {
                strcpy(respuesta, "6/Error en la consulta");
            }
        }
		
		
		else if (codigo == 8)
		{
			char anfitrion[20];
			char invitado[20];
			p = strtok(NULL, "/");
			strcpy(anfitrion, p);
			p = strtok(NULL, "/");
			strcpy(invitado, p);
			
			int socketInvitado;
			if (DameSocketUsuario(invitado, &socketInvitado))
			{
				char notificacion[400];
				sprintf(notificacion, "8/%s", anfitrion);
				write(socketInvitado, notificacion, strlen(notificacion));
			}
			else
			{
				printf("El usuario invitado '%s' no esta conectado.\n", invitado);
			}
		}
		
		else if (codigo == 9)
		{
			char anfitrion[20];
			char invitado[20];
			char decision[10];
			p = strtok(NULL, "/");
			strcpy(anfitrion, p);
			p = strtok(NULL, "/");
			strcpy(invitado, p);
			p = strtok(NULL, "/");
			strcpy(decision, p);
			
			int socketAnfitrion;
			if (DameSocketUsuario(anfitrion, &socketAnfitrion))
			{
				char notificacion[50];
				if (strcmp(decision, "ACEPTAR") == 0)
				{
					sprintf(notificacion, "9/%s/ACEPTAR", invitado);
				}
				else if (strcmp(decision, "RECHAZAR") == 0)
				{
					sprintf(notificacion, "9/%s/RECHAZAR", invitado);
				}
				else
				{
					sprintf(notificacion, "9/%s/ERROR", invitado);
				}
				printf("%s\n", decision);
				printf("%s\n", notificacion);
				write(socketAnfitrion, notificacion, strlen(notificacion));
			}
			else
			{
				printf("El usuario '%s', que es el anfitri�n, no esta conectado.\n", anfitrion);
			}
		}
		
		else if (codigo == 10)
		{
			char jugador1[20];
			char jugador2[20];
			char jugador3[20];
			char jugador4[20];
			p = strtok(NULL, "/");
			strcpy(jugador1, p);
			p = strtok(NULL, "/");
			strcpy(jugador2, p);
			p = strtok(NULL, "/");
			strcpy(jugador3, p);
			p = strtok(NULL, "/");
			strcpy(jugador4, p);
			
			char notificacion[50];
			int socketAnfitrion;
			int socketJugador2;
			int socketJugador3;
			int socketJugador4;
			
			if (DameSocketUsuario(jugador1, &socketAnfitrion))
			{
				milistaP.numPartidas++;
				milistaP.partidas[milistaP.numPartidas].id = milistaP.numPartidas; // Asignamos ID nuevo				
				
				sprintf(notificacion, "10/%d/%s/%s/%s/%s/%d", numForm, jugador1, jugador2, jugador3, jugador4, milistaP.partidas[milistaP.numPartidas].id);
				write(socketAnfitrion, notificacion, strlen(notificacion));
				numForm++;
			}
			if (DameSocketUsuario(jugador2, &socketJugador2))
			{
				write(socketJugador2, notificacion, strlen(notificacion));
			}
			if (DameSocketUsuario(jugador3, &socketJugador3))
			{
				write(socketJugador3, notificacion, strlen(notificacion));
			}
			if (DameSocketUsuario(jugador4, &socketJugador4))
			{
				write(socketJugador4, notificacion, strlen(notificacion));
			}
			else
			{
				printf("El usuario '%s', que es el anfitri�n, no esta conectado.\n", jugador1);
			}
		}
		else if (codigo == 11)
		{
			char emisor[20],texto[450];
			//Obtenemos emisor
			p=strtok(NULL,"/");
			strcpy(emisor,p);
			//Obtener el mensaje
			p=strtok(NULL,"");
			strcpy(texto,p);
			char notificacion[512];
			sprintf(notificacion,"11/%s/%s/%d",emisor,texto,milistaP.partidas[milistaP.numPartidas].id);
			//Reenviar a todos los conectados
			for (int j = 0;j<milista.num;j++)
			{
				write(milista.conectados[j].socket,notificacion,strlen(notificacion));
			}
			continue;
		}
        
        if ((codigo != 0) && (strlen(respuesta) != 0))
        {
            printf("Resposta: %s\n", respuesta);
            write(sock_conn, respuesta, strlen(respuesta));
			sprintf(respuesta, "");
        }

        if ((codigo ==2)||(codigo==3)|| (codigo==4) || (codigo==5)||(codigo==6))
		{
			pthread_mutex_lock( &mutex ); //No me interrumpas ahora
			contador = contador +1;
			pthread_mutex_unlock( &mutex); //ya puedes interrumpirme
    
		}
    }
    close(sock_conn);
    mysql_close(conn);
    return NULL;
}
        

int main(int argc, char *argv[]) 
{
	milistaP.numPartidas = 2;
	milistaP.partidas[milistaP.numPartidas].id = 2; //La primera partida creada ser￡ ya con ID = 3 ya que en nuestra base de datos ya creamos 2 partidas de prueba con ID = 1 y ID = 2 para probar el c￳digo en su momento.
	
	
	int sock_conn, sock_listen;          // sock_listen: descriptor para el socket de escucha, sock_conn: descriptor para la conexi￳n del cliente
    struct sockaddr_in serv_adr;             // Estructura para almacenar la direcci￳n del servidor

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
    contador = 0;
    
    pthread_t thread;
    i = 0;
    milista.num = 0; // Inicializar la lista de conectados
    
    while (1) 
    {
        // accept() bloquea y espera hasta que un cliente se conecte.
        sock_conn = accept(sock_listen, NULL, NULL);
        if (sock_conn < 0) {
            perror("Error en accept");
            continue; // Si ocurre un error al aceptar, se sigue al siguiente ciclo
        }
        printf("Connexio rebuda\n");
        sockets[i] = sock_conn;
        
        // Crear thread y decirle lo que tiene que hacer
        pthread_create (&thread, NULL, AtenderCliente,&sockets[i]);
        i=i+1;
    }
    close(sock_listen);
    return 0;
}
