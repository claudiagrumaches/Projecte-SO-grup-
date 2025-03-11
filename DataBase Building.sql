DROP DATABASE IF EXISTS rummykub;
CREATE DATABASE rummykub;

USE rummykub;

CREATE TABLE jugadors (
id_jugador INT AUTO_INCREMENT  PRIMARY KEY,
nom VARCHAR(50),
edat INT,
nacionalitat VARCHAR(50),
punts_totals INT
);

CREATE TABLE partides (
    id_partida INT AUTO_INCREMENT PRIMARY KEY,
    record_punts INT,
    punts_totals INT
);

CREATE TABLE jugador_partida (
    id INT AUTO_INCREMENT PRIMARY KEY,  -- Clau primària única
    id_partida INT,                      -- Foreign key cap a la taula de partides
    punts INT,                           -- Punts del jugador en la partida
    FOREIGN KEY (id_partida) REFERENCES partides(id_partida) ON DELETE CASCADE
);
-- Afegir jugadors
INSERT INTO jugadors (nom, edat, nacionalitat, punts_totals) 
VALUES 
    ('Raúl', 20, 'Espanyol', 1200),
    ('Claudia', 20, 'Espanyola', 950),
    ('Laura', 19, 'Francesa', 800);

-- Afegir partides
INSERT INTO partides (record_punts, punts_totals) 
VALUES 
    (681, 250),
    (250, 300);

-- Crear la taula d'unió
INSERT INTO jugador_partida (id_jugador, punts) 
VALUES 
    (1, 120),  -- Raúl ha jugat, però no sabem en quina partida
    (2, 251),  -- Claudia ha jugat, però tampoc sabem la partida
    (3, 300);  -- Laura ha jugat, sense indicar la partida
