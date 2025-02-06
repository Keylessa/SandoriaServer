-- Creăm baza de date pentru caractere
CREATE DATABASE IF NOT EXISTS characters;
USE characters;

-- Creăm tabela pentru personaje
CREATE TABLE IF NOT EXISTS characters (
    id INT AUTO_INCREMENT PRIMARY KEY,
    accountName VARCHAR(50) NOT NULL,
    charName VARCHAR(50) NOT NULL UNIQUE,
    race VARCHAR(30) NOT NULL,
    class VARCHAR(30) NOT NULL,
    gender ENUM('Male', 'Female') NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Index pentru căutare rapidă după nume de personaj
CREATE INDEX idx_charName ON characters (charName);
