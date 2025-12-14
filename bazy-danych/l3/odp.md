# Paweł Smolnicki 283999

## 1

Odp: 

Stosowanie PESELu jako klucza głównego to nie jest najlepszy pomysł, chociażby ze względu na możliwe pomyłki przy jego wprowadzaniu, braku możliwości dodania cudzoziemców bez PESELu, czy też zmiany PESELu przez osobę - chociażby przez teoretyczną zmianę płci (pomijając kwestie prawne). Dlatego zawsze lepszym rozwiązaniem jest użycie sztucznego klucza głównego (niż tzw. 'naturalnych').


```sql
CREATE DATABASE IF NOT EXISTS Lab3;
USE Lab3;

CREATE TABLE Ludzie (
    id INT AUTO_INCREMENT PRIMARY KEY,
    PESEL CHAR(11) NOT NULL UNIQUE,
    imie VARCHAR(30) NOT NULL,
    nazwisko VARCHAR(30) NOT NULL,
    data_urodzenia DATE NOT NULL,
    plec ENUM('K', 'M') NOT NULL,

    -- https://stackoverflow.com/questions/5064977/detect-if-value-is-number-in-mysql
    CONSTRAINT chk_pesel_format CHECK (PESEL REGEXP '^[0-9]{11}$')
);

CREATE TABLE Zawody (
    zawod_id INT AUTO_INCREMENT PRIMARY KEY,
    nazwa VARCHAR(50) NOT NULL,
    pensja_min FLOAT NOT NULL CHECK (pensja_min >= 0),
    pensja_max FLOAT NOT NULL CHECK (pensja_max >= 0),
    CONSTRAINT chk_widelek CHECK (pensja_min < pensja_max)
);

CREATE TABLE Pracownicy (
    pracownik_id INT AUTO_INCREMENT PRIMARY KEY,
    osoba_id INT NOT NULL,
    zawod_id INT NOT NULL,
    pensja FLOAT NOT NULL CHECK (pensja >= 0),
    FOREIGN KEY (osoba_id) REFERENCES Ludzie(id) ON DELETE CASCADE,
    FOREIGN KEY (zawod_id) REFERENCES Zawody(zawod_id)
);

-- Wstawianie:

INSERT INTO Zawody (nazwa, pensja_min, pensja_max) VALUES 
('polityk', 8000, 25000),
('nauczyciel', 3000, 6000),
('lekarz', 6000, 15000),
('informatyk', 5000, 20000);


DELIMITER $$
CREATE PROCEDURE GenerujLudzi()
BEGIN
    DECLARE i INT DEFAULT 0;
    -- 5 niepełnoletnich
    WHILE i < 5 DO
        INSERT INTO Ludzie (PESEL, imie, nazwisko, data_urodzenia, plec) 
        VALUES (CONCAT('072', LPAD(i, 8, '0')), 'Maly', 'Kowalski', '2010-01-01', IF(i%2=0,'M','K'));
        SET i = i + 1;
    END WHILE;
    
    SET i = 0;
    -- 45 dorosłych < 60 lat
    WHILE i < 45 DO
        INSERT INTO Ludzie (PESEL, imie, nazwisko, data_urodzenia, plec) 
        VALUES (CONCAT('800', LPAD(i, 8, '0')), IF(i%2=0,'Adam','Anna'), 'Nowak', '1980-06-15', IF(i%2=0,'M','K'));
        SET i = i + 1;
    END WHILE;

    SET i = 0;
    -- 5 seniorów >= 60 lat
    WHILE i < 5 DO
        INSERT INTO Ludzie (PESEL, imie, nazwisko, data_urodzenia, plec) 
        VALUES (CONCAT('500', LPAD(i, 8, '0')), 'Senior', 'Starszy', '1950-01-01', IF(i%2=0,'M','K'));
        SET i = i + 1;
    END WHILE;
END$$
DELIMITER ;

CALL GenerujLudzi();


DELIMITER $$
CREATE PROCEDURE PrzydzielZawody()
BEGIN
    DECLARE _done INT DEFAULT FALSE;
    DECLARE _id INT;
    DECLARE _data_ur DATE;
    DECLARE _plec ENUM('K', 'M');
    DECLARE _wiek INT;
    DECLARE _zawod_id INT;
    DECLARE _pensja FLOAT;
    DECLARE _zawod_lekarz_id INT;
    
    -- Kursor dla osób pełnoletnich
    DECLARE cur CURSOR FOR 
        SELECT id, data_urodzenia, plec 
        FROM Ludzie 
        WHERE TIMESTAMPDIFF(YEAR, data_urodzenia, CURDATE()) >= 18;
        
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET _done = TRUE;

    OPEN cur;

    SET _zawod_lekarz_id = (SELECT zawod_id FROM Zawody WHERE nazwa = 'lekarz' LIMIT 1);

    read_loop: LOOP
        FETCH cur INTO _id, _data_ur, _plec;
        IF _done THEN
            LEAVE read_loop;
        END IF;

        SET _wiek = TIMESTAMPDIFF(YEAR, _data_ur, CURDATE());

        -- Logika doboru zawodu (losowo 1-4)
        SELECT zawod_id INTO _zawod_id 
        FROM Zawody 
        ORDER BY RAND() 
        LIMIT 1;
        
        -- Sprawdzenie warunków dla lekarza (id=3)
        -- Lekarz M <= 65, Lekarz K <= 60 
        IF _zawod_id = _zawod_lekarz_id THEN
            IF (_plec = 'M' AND _wiek > 65) OR (_plec = 'K' AND _wiek > 60) THEN
                SET _zawod_id = (SELECT zawod_id FROM Zawody WHERE zawod_id != _zawod_lekarz_id ORDER BY RAND() LIMIT 1);
            END IF;
        END IF;

        SELECT pensja_min + (RAND() * (pensja_max - pensja_min)) 
        INTO _pensja 
        FROM Zawody WHERE zawod_id = _zawod_id;

        INSERT INTO Pracownicy (osoba_id, zawod_id, pensja) 
        VALUES (_id, _zawod_id, _pensja);
        
    END LOOP;

    CLOSE cur;
END$$
DELIMITER ;

CALL PrzydzielZawody();
```

## 2

Odp: 
Aktualne indeksy:
- Ludzie: id, PESEL, idx_ludzie_plec_imie
- Pracownicy: pracownik_id, osoba_id, zawod_id, idx_prac
- Zawody: zawod_id

Optymalizator użyje indeksów dla zapytań 1,2,4 i 5.

```sql

CREATE INDEX idx_ludzie_plec_imie ON Ludzie(plec, imie);
CREATE INDEX idx_pracownicy_pensja ON Pracownicy(pensja);

-- 1
EXPLAIN SELECT * FROM Ludzie WHERE plec = 'K' AND imie LIKE 'A%';

-- 2
EXPLAIN SELECT * FROM Ludzie WHERE plec = 'K';

-- 3
EXPLAIN SELECT * FROM Ludzie WHERE imie LIKE 'K%';

-- 4
EXPLAIN SELECT * FROM Pracownicy WHERE pensja < 2000;

-- 5
EXPLAIN SELECT l.imie, l.nazwisko 
FROM Ludzie l
JOIN Pracownicy p ON l.id = p.osoba_id
WHERE l.plec = 'M' AND z.nazwa = 'informatyk' AND p.pensja > 10000;


SHOW INDEX FROM Ludzie;
SHOW INDEX FROM Pracownicy;
SHOW INDEX FROM Zawody;

```

## 3

```sql
DELIMITER $$
CREATE PROCEDURE PodwyzkaDlaZawodu(IN p_nazwa_zawodu VARCHAR(50))
BEGIN
    DECLARE v_zawod_id INT;
    DECLARE v_pensja_max FLOAT;
    DECLARE v_przekroczenie INT DEFAULT 0;

    SELECT zawod_id, pensja_max INTO v_zawod_id, v_pensja_max 
    FROM Zawody WHERE nazwa = p_nazwa_zawodu;

    START TRANSACTION;

    UPDATE Pracownicy 
    SET pensja = pensja * 1.05
    WHERE zawod_id = v_zawod_id;

    SELECT COUNT(*) INTO v_przekroczenie
    FROM Pracownicy
    WHERE zawod_id = v_zawod_id AND pensja > v_pensja_max;

    IF v_przekroczenie > 0 THEN
        ROLLBACK;
    ELSE
        COMMIT;
    END IF;
END$$
DELIMITER ;
```

## 4

```sql
PREPARE stmt_count_women FROM 
'SELECT COUNT(*) as LiczbaKobiet 
 FROM Pracownicy p 
 JOIN Ludzie l ON p.osoba_id = l.id 
 JOIN Zawody z ON p.zawod_id = z.zawod_id 
 WHERE l.plec = "K" AND z.nazwa = ?';

SET @zawod_nazwa = 'lekarz';

EXECUTE stmt_count_women USING @zawod_nazwa;
```

## 5

Odp: Pełny backup bazy danych kopiuje całą bazę danych (w aktualnym stanie),
natomiast różnicowy backup kopiuje jedynie zmiany dokonane od czasu ostatniego pełnego backupu.

```sh
mysqldump -u root -p Lab3 > backup.sql
mysql -u root -p -e "DROP DATABASE Lab3;"
mysql -u root -p -e "CREATE DATABASE Lab3;"
mysql -u root -p Lab3 < backup.sql
```

## 6

WebGoat
SQL Injection (intro)

```sql
-- 2
SELECT department FROM employees WHERE first_name='Bob' and last_name='Franco';

-- 3
UPDATE employees SET department='Sales' WHERE first_name='Tobi' AND last_name='Barnett';

-- 4
ALTER TABLE employees ADD COLUMN phone varchar(20);

-- 5

-- 9
SELECT * FROM user_data WHERE first_name = 'John' and last_name = 'Smith' or '1' = '1';

-- 10
Login: 0
UserId: 1 or '1'='1'

-- 11
Employee Name: 0
TAN: 0' or '1'='1

-- 12


-- 13
Search: '; DROP TABLE access_log; --
```

----------

SQL Injection (advanced)

```sql
-- 3
Name: '; SELECT userid, user_name, password, cookie, null, null, null FROM user_system_data --
```

6:
1. 4
2. 3
3. 2
4. 3
5. 4


----------
SQL Mitigation

5:
```java
Connection conn = DriverManager.getConnection(DBURL, DBUSER, DBPW);
PreparedStatement statement = conn.prepareStatement("SELECT status FROM users WHERE name=? AND mail=?");

statement.setString(1, name);
statement.setString(2, mail);
```

6:
```java
try {
    Connection conn = DriverManager.getConnection(DBURL, DBUSER, DBPW);
    PreparedStatement statement = conn.prepareStatement("SELECT status FROM users WHERE name=? AND mail=?");

    statement.setString(1, name);
    statement.setString(2, mail);

} catch (Exception e) {
    System.out.println("Oops. Something went wrong!");
}
```