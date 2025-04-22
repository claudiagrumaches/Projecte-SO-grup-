DROP DATABASE IF EXISTS M8_BBDDrummykub;
CREATE DATABASE M8_BBDDrummykub;

USE M8_BBDDrummykub;

-- Tabla de jugadores
CREATE TABLE jugadors (
    id_jugador INT AUTO_INCREMENT PRIMARY KEY,
    nom VARCHAR(50),
    contrasenya VARCHAR(50),
    edat INT,
    punts_totals INT
);

-- Tabla de partidas
CREATE TABLE partides (
    id_partida INT AUTO_INCREMENT PRIMARY KEY,
    record_punts INT,
    punts_totals_partida INT 
);


CREATE TABLE jugador_partida (
    id_jugador INT,   -- Foreign key a jugadors
    id_partida INT,   -- Foreign key a partides
    punts INT,        -- Puntos obtenidos en la partida
    PRIMARY KEY (id_jugador, id_partida), -- Clave primaria compuesta
    FOREIGN KEY (id_jugador) REFERENCES jugadors(id_jugador),
    FOREIGN KEY (id_partida) REFERENCES partides(id_partida)
);

-- Insertar jugadores
INSERT INTO jugadors (nom, contrasenya, edat, punts_totals) 
VALUES 
    ('Raul', '123abc', 20, 1200),
    ('Claudia', 'gato', 20, 950),
    ('Laura', 'atleti', 19, 800);

-- Insertar partidas
INSERT INTO partides (record_punts, punts_totals_partida)  
VALUES 
    (681, 1257), 
    (250, 300);

-- Insertar relación jugadores-partidas
INSERT INTO jugador_partida (id_jugador, id_partida, punts) 
VALUES 
    (1, 1, 120),  
    (2, 1, 251),  
    (3, 2, 300); 
