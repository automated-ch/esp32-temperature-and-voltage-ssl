--
-- Table structure for table `SENSOR`
--

CREATE TABLE `SENSOR` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `sensor_name` varchar(255) NOT NULL,
  `sensor_location` varchar(45) NOT NULL,
  `sensor_mac` varchar(17) NOT NULL,
  `sensor_group_id` int(11) NOT NULL,
  `sensor_ip` varchar(15) DEFAULT NULL,
  `sensor_connection_type` varchar(8) NOT NULL,
  `last_seen_activity_date` datetime NOT NULL,
  PRIMARY KEY (`id`)
);

--
-- Table structure for table `SENSOR_DATA`
--

CREATE TABLE `SENSOR_DATA` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `double_record` decimal(10,2) DEFAULT NULL,
  `string_record` varchar(45) DEFAULT NULL,
  `fk_sensor` int(11) NOT NULL,
  `int_record` int(11) DEFAULT NULL,
  `record_date` varchar(45) NOT NULL,
  `device_voltage` decimal(5,2) DEFAULT NULL,
  PRIMARY KEY (`id`)
);

--
-- Table structure for table `SENSOR_GROUP`
--

CREATE TABLE `SENSOR_GROUP` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `sensor_group_name` varchar(45) NOT NULL,
  PRIMARY KEY (`id`)
);
