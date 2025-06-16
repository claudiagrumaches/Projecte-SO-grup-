> **SO Project:** Rummikub  
> _Per veure cada versió, entra a l'apartat de Tags i Releases._

---

## 👥 Components del grup

- **Laura Guerrero**  
- **Clàudia Grumaches**  
- **Raúl Gómez**  

---

## 📦 Historial de versions

| Versió    | Generada per   | Verificada per | Comunicada per | Enllaç vídeo                                                                  |
|-----------|----------------|----------------|----------------|-------------------------------------------------------------------------------|
| **#1**    | Laura          | Claudia        | Raúl           | https://youtu.be/06qZ5zRJRsw                                                  |
| **#2**    | Claudia        | Raúl           | Laura          | https://youtu.be/HKLj2eBOCGo<br>https://youtu.be/mOatfCI4nR0                  |
| **#3**    | Raúl           | Laura          | Claudia        | https://youtu.be/CCws0cWLQdY                                                  |
| **#4**    | Laura          | Claudia        | Raúl           | https://youtu.be/c753pwfwYhs                                                  |
| **#5**    | Claudia        | Raúl           | Laura          | https://youtu.be/Bki482JGCrI                                                  |
| **Final** | Raúl           | Laura          | Claudia        | https://youtu.be/8aa9kBz50NE?si=1xzGVmPSAZHMDIWC                              |

## PROTOCOL

| Nº | Petició                              | Client → Server                              | Server → Client                   | Descripció                                   |
| -- | ------------------------------------ | -------------------------------------------- | --------------------------------- | --------------------------------------------- |
| 0  | Desconnexió                          | `usuario`                                    | -                                 | Un usuari es desconnecta                      |
| 2  | Login                                | `usuario/contraseña`                         | -                                 | Inici de sessió                               |
| 3  | Registre                             | `usuario/contraseña`                         | -                                 | Creació de compte                             |
| 4  | Consultar punts totals               | `usuario`                                    | -                                 | Retorna punts totals del jugador              |
| 5  | Consultar punts per partida          | `id-partida`                                 | -                                 | Retorna puntuació del jugador a la partida    |
| 6  | Consultar rècord de partida          | `id-partida`                                 | -                                 | Mostra el rècord de la partida                |
| 8  | Invitació                            | `(anfitrión/invitado)`                       | -                                 | L’amfitrió convida a un altre jugador         |
| 9  | Acceptar o rebutjar invitació        | `(invitado/ACEPTAR)` o `(invitado/RECHAZAR)` | -                                 | El jugador accepta o rebutja la invitació     |
| 10 | Iniciar partida                      | `jug1/jug2/jug3/...`                         | 10/jug1/jug2/jug3/jug4/id         | S'inicia la partida                          |
| 11 | Missatge al xat general o de partida | `emisor/texto`                               | 11/emisor/texto/id_partida        | Enviar missatge a través del xat              |
| 12 | Esborrar usuari                      | `usuario/contraseña`                         | -                                 | Elimina l'usuari de la base de dades          |



