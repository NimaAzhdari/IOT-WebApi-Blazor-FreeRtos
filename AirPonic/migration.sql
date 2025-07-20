CREATE TABLE IF NOT EXISTS `__EFMigrationsHistory` (
    `MigrationId` varchar(150) CHARACTER SET utf8mb4 NOT NULL,
    `ProductVersion` varchar(32) CHARACTER SET utf8mb4 NOT NULL,
    CONSTRAINT `PK___EFMigrationsHistory` PRIMARY KEY (`MigrationId`)
) CHARACTER SET=utf8mb4;

START TRANSACTION;

ALTER DATABASE CHARACTER SET utf8mb4;

CREATE TABLE `MicroData` (
    `id` int NOT NULL AUTO_INCREMENT,
    `Humidity` float NOT NULL,
    `Temperature` float NOT NULL,
    `Light` float NOT NULL,
    `LightIsOn` tinyint(1) NOT NULL,
    `TotalLightTime` float NOT NULL,
    `Date` date NOT NULL,
    `Time` time(6) NOT NULL,
    CONSTRAINT `PK_MicroData` PRIMARY KEY (`id`)
) CHARACTER SET=utf8mb4;

INSERT INTO `__EFMigrationsHistory` (`MigrationId`, `ProductVersion`)
VALUES ('20250424170559_first', '8.0.13');

COMMIT;

START TRANSACTION;

ALTER TABLE `MicroData` DROP COLUMN `Date`;

ALTER TABLE `MicroData` DROP COLUMN `Time`;

ALTER TABLE `MicroData` ADD `DeviceId` char(36) COLLATE ascii_general_ci NOT NULL DEFAULT '00000000-0000-0000-0000-000000000000';

ALTER TABLE `MicroData` ADD `Timestamp` datetime(6) NOT NULL DEFAULT '0001-01-01 00:00:00';

INSERT INTO `__EFMigrationsHistory` (`MigrationId`, `ProductVersion`)
VALUES ('20250427065846_second', '8.0.13');

COMMIT;

START TRANSACTION;

CREATE TABLE `MicroConfig` (
    `id` int NOT NULL AUTO_INCREMENT,
    `DeviceId` char(36) COLLATE ascii_general_ci NOT NULL,
    `ConfigVersion` char(36) COLLATE ascii_general_ci NOT NULL,
    `PUMP_ON_DURATION` int NOT NULL,
    `PUMP_OFF_DURATION` int NOT NULL,
    `SENSOR_CYCLE` int NOT NULL,
    `ssid` longtext CHARACTER SET utf8mb4 NOT NULL,
    `password` longtext CHARACTER SET utf8mb4 NOT NULL,
    `LIGHT_THRESHOLD` int NOT NULL,
    `LIGHT_HOURS_THRESHOLD` int NOT NULL,
    `LIGHT_INCREMENT` float NOT NULL,
    `LastUpdate` datetime NOT NULL,
    CONSTRAINT `PK_MicroConfig` PRIMARY KEY (`id`)
) CHARACTER SET=utf8mb4;

INSERT INTO `__EFMigrationsHistory` (`MigrationId`, `ProductVersion`)
VALUES ('20250504152053_third', '8.0.13');

COMMIT;

