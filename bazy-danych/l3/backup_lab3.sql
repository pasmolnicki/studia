/*M!999999\- enable the sandbox mode */ 
-- MariaDB dump 10.19  Distrib 10.11.13-MariaDB, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: Lab3
-- ------------------------------------------------------
-- Server version	10.11.13-MariaDB-0ubuntu0.24.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Ludzie`
--

DROP TABLE IF EXISTS `Ludzie`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `Ludzie` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `PESEL` char(11) NOT NULL,
  `imie` varchar(30) NOT NULL,
  `nazwisko` varchar(30) NOT NULL,
  `data_urodzenia` date NOT NULL,
  `plec` enum('K','M') NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `PESEL` (`PESEL`),
  KEY `idx_ludzie_plec_imie` (`plec`,`imie`),
  CONSTRAINT `chk_pesel_format` CHECK (`PESEL` regexp '^[0-9]{11}$')
) ENGINE=InnoDB AUTO_INCREMENT=56 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Ludzie`
--

LOCK TABLES `Ludzie` WRITE;
/*!40000 ALTER TABLE `Ludzie` DISABLE KEYS */;
INSERT INTO `Ludzie` VALUES
(1,'07200000000','Maly','Kowalski','2010-01-01','M'),
(2,'07200000001','Maly','Kowalski','2010-01-01','K'),
(3,'07200000002','Maly','Kowalski','2010-01-01','M'),
(4,'07200000003','Maly','Kowalski','2010-01-01','K'),
(5,'07200000004','Maly','Kowalski','2010-01-01','M'),
(6,'80000000000','Adam','Nowak','1980-06-15','M'),
(7,'80000000001','Anna','Nowak','1980-06-15','K'),
(8,'80000000002','Adam','Nowak','1980-06-15','M'),
(9,'80000000003','Anna','Nowak','1980-06-15','K'),
(10,'80000000004','Adam','Nowak','1980-06-15','M'),
(11,'80000000005','Anna','Nowak','1980-06-15','K'),
(12,'80000000006','Adam','Nowak','1980-06-15','M'),
(13,'80000000007','Anna','Nowak','1980-06-15','K'),
(14,'80000000008','Adam','Nowak','1980-06-15','M'),
(15,'80000000009','Anna','Nowak','1980-06-15','K'),
(16,'80000000010','Adam','Nowak','1980-06-15','M'),
(17,'80000000011','Anna','Nowak','1980-06-15','K'),
(18,'80000000012','Adam','Nowak','1980-06-15','M'),
(19,'80000000013','Anna','Nowak','1980-06-15','K'),
(20,'80000000014','Adam','Nowak','1980-06-15','M'),
(21,'80000000015','Anna','Nowak','1980-06-15','K'),
(22,'80000000016','Adam','Nowak','1980-06-15','M'),
(23,'80000000017','Anna','Nowak','1980-06-15','K'),
(24,'80000000018','Adam','Nowak','1980-06-15','M'),
(25,'80000000019','Anna','Nowak','1980-06-15','K'),
(26,'80000000020','Adam','Nowak','1980-06-15','M'),
(27,'80000000021','Anna','Nowak','1980-06-15','K'),
(28,'80000000022','Adam','Nowak','1980-06-15','M'),
(29,'80000000023','Anna','Nowak','1980-06-15','K'),
(30,'80000000024','Adam','Nowak','1980-06-15','M'),
(31,'80000000025','Anna','Nowak','1980-06-15','K'),
(32,'80000000026','Adam','Nowak','1980-06-15','M'),
(33,'80000000027','Anna','Nowak','1980-06-15','K'),
(34,'80000000028','Adam','Nowak','1980-06-15','M'),
(35,'80000000029','Anna','Nowak','1980-06-15','K'),
(36,'80000000030','Adam','Nowak','1980-06-15','M'),
(37,'80000000031','Anna','Nowak','1980-06-15','K'),
(38,'80000000032','Adam','Nowak','1980-06-15','M'),
(39,'80000000033','Anna','Nowak','1980-06-15','K'),
(40,'80000000034','Adam','Nowak','1980-06-15','M'),
(41,'80000000035','Anna','Nowak','1980-06-15','K'),
(42,'80000000036','Adam','Nowak','1980-06-15','M'),
(43,'80000000037','Anna','Nowak','1980-06-15','K'),
(44,'80000000038','Adam','Nowak','1980-06-15','M'),
(45,'80000000039','Anna','Nowak','1980-06-15','K'),
(46,'80000000040','Adam','Nowak','1980-06-15','M'),
(47,'80000000041','Anna','Nowak','1980-06-15','K'),
(48,'80000000042','Adam','Nowak','1980-06-15','M'),
(49,'80000000043','Anna','Nowak','1980-06-15','K'),
(50,'80000000044','Adam','Nowak','1980-06-15','M'),
(51,'50000000000','Senior','Starszy','1950-01-01','M'),
(52,'50000000001','Senior','Starszy','1950-01-01','K'),
(53,'50000000002','Senior','Starszy','1950-01-01','M'),
(54,'50000000003','Senior','Starszy','1950-01-01','K'),
(55,'50000000004','Senior','Starszy','1950-01-01','M');
/*!40000 ALTER TABLE `Ludzie` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Pracownicy`
--

DROP TABLE IF EXISTS `Pracownicy`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `Pracownicy` (
  `pracownik_id` int(11) NOT NULL AUTO_INCREMENT,
  `osoba_id` int(11) NOT NULL,
  `zawod_id` int(11) NOT NULL,
  `pensja` float NOT NULL CHECK (`pensja` >= 0),
  PRIMARY KEY (`pracownik_id`),
  KEY `osoba_id` (`osoba_id`),
  KEY `zawod_id` (`zawod_id`),
  KEY `idx_pracownicy_pensja` (`pensja`),
  CONSTRAINT `Pracownicy_ibfk_1` FOREIGN KEY (`osoba_id`) REFERENCES `Ludzie` (`id`) ON DELETE CASCADE,
  CONSTRAINT `Pracownicy_ibfk_2` FOREIGN KEY (`zawod_id`) REFERENCES `Zawody` (`zawod_id`)
) ENGINE=InnoDB AUTO_INCREMENT=101 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Pracownicy`
--

LOCK TABLES `Pracownicy` WRITE;
/*!40000 ALTER TABLE `Pracownicy` DISABLE KEYS */;
INSERT INTO `Pracownicy` VALUES
(1,6,2,5983.58),
(2,7,3,12038),
(3,8,4,16941.9),
(4,9,3,6795.46),
(5,10,2,4542.56),
(6,11,3,14766.9),
(7,12,2,5640.51),
(8,13,2,5869.01),
(9,14,4,8733.16),
(10,15,4,6824.59),
(11,16,2,3213.76),
(12,17,2,3777.15),
(13,18,4,9209.93),
(14,19,4,5016.97),
(15,20,1,16424.2),
(16,21,1,14886.3),
(17,22,2,3709.63),
(18,23,3,13637.7),
(19,24,1,24917.3),
(20,25,3,8428.92),
(21,26,2,3514.52),
(22,27,3,13467.6),
(23,28,1,11763),
(24,29,3,12642.6),
(25,30,3,6280.36),
(26,31,1,24137.4),
(27,32,1,20844.6),
(28,33,2,5437.11),
(29,34,4,14524.4),
(30,35,3,12776.7),
(31,36,3,11325.8),
(32,37,1,15932.1),
(33,38,3,10855.1),
(34,39,4,5754.22),
(35,40,2,3300.8),
(36,41,1,15693.5),
(37,42,4,15442.8),
(38,43,1,10627.6),
(39,44,3,12038.9),
(40,45,2,4073.01),
(41,46,2,5270.54),
(42,47,3,7449.37),
(43,48,3,8233.62),
(44,49,1,12522.9),
(45,50,2,5464.17),
(46,51,1,11602.3),
(47,52,2,4900.94),
(48,53,1,10247),
(49,54,1,21745.6),
(50,55,4,7596.53),
(51,6,2,3346.79),
(52,7,4,11644.4),
(53,8,2,3828.86),
(54,9,3,13973.5),
(55,10,4,14560.6),
(56,11,4,17582),
(57,12,2,4033.94),
(58,13,3,10734.4),
(59,14,1,10811.4),
(60,15,2,3317.08),
(61,16,3,12409.2),
(62,17,4,15801.4),
(63,18,4,6371.84),
(64,19,2,5744.6),
(65,20,1,11450.9),
(66,21,1,9804.93),
(67,22,1,12695.7),
(68,23,4,14343.2),
(69,24,2,4892.99),
(70,25,4,10377),
(71,26,1,14882.8),
(72,27,3,6696.14),
(73,28,4,15594.5),
(74,29,2,5989.75),
(75,30,3,8986.9),
(76,31,1,13951.4),
(77,32,3,12528.9),
(78,33,3,14538.2),
(79,34,1,8662.48),
(80,35,4,19215.2),
(81,36,1,14139.6),
(82,37,4,11171.5),
(83,38,4,6583.21),
(84,39,4,15555.6),
(85,40,4,9761.38),
(86,41,1,11440.7),
(87,42,2,4201.63),
(88,43,1,18497.2),
(89,44,2,4351.47),
(90,45,2,4271.76),
(91,46,4,17198.4),
(92,47,2,4500.85),
(93,48,1,13132.3),
(94,49,3,12586.4),
(95,50,3,6742.68),
(96,51,4,18243.3),
(97,52,1,9357.66),
(98,53,1,19102.3),
(99,54,1,23039.6),
(100,55,2,3612.82);
/*!40000 ALTER TABLE `Pracownicy` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Zawody`
--

DROP TABLE IF EXISTS `Zawody`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `Zawody` (
  `zawod_id` int(11) NOT NULL AUTO_INCREMENT,
  `nazwa` varchar(50) NOT NULL,
  `pensja_min` float NOT NULL CHECK (`pensja_min` >= 0),
  `pensja_max` float NOT NULL CHECK (`pensja_max` >= 0),
  PRIMARY KEY (`zawod_id`),
  CONSTRAINT `chk_widelek` CHECK (`pensja_min` < `pensja_max`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Zawody`
--

LOCK TABLES `Zawody` WRITE;
/*!40000 ALTER TABLE `Zawody` DISABLE KEYS */;
INSERT INTO `Zawody` VALUES
(1,'polityk',8000,25000),
(2,'nauczyciel',3000,6000),
(3,'lekarz',6000,15000),
(4,'informatyk',5000,20000);
/*!40000 ALTER TABLE `Zawody` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-12-09 13:07:10
