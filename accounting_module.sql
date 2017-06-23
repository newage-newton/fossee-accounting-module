-- Adminer 4.2.4 MySQL dump

SET NAMES utf8;
SET time_zone = '+00:00';
SET foreign_key_checks = 0;
SET sql_mode = 'NO_AUTO_VALUE_ON_ZERO';

DROP TABLE IF EXISTS `lab_migration_cheque`;
CREATE TABLE `lab_migration_cheque` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `form_id` int(11) NOT NULL,
  `cheque_number` int(11) NOT NULL,
  `cheque_amount` int(11) NOT NULL,
  `cheque_date` date NOT NULL,
  `cheque_bank` varchar(255) NOT NULL,
  `cheque_status` int(1) NOT NULL COMMENT '0 - Not Dispatched 1 - Dispatched 2 - Received',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `lab_migration_contact_details`;
CREATE TABLE `lab_migration_contact_details` (
  `user_id` int(11) NOT NULL,
  `phone` int(11) NOT NULL,
  `email_id` varchar(255) NOT NULL,
  `address` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `lab_migration_form_post`;
CREATE TABLE `lab_migration_form_post` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `proposal_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `copyright` int(1) NOT NULL DEFAULT '0' COMMENT '0 - Not Sent 1 - Sent',
  `receipt` int(1) NOT NULL DEFAULT '0' COMMENT '0 - Not Sent 1 - Sent',
  `undertaking_teacher` int(1) NOT NULL DEFAULT '0' COMMENT '0 - Not Sent 1 - Sent',
  `undertaking_hod` int(1) NOT NULL DEFAULT '0' COMMENT '0 - Not Sent 1 - Sent',
  `undertaking_principal` int(1) NOT NULL DEFAULT '0' COMMENT '0 - Not Sent 1 - Sent',
  `application` int(1) NOT NULL DEFAULT '0' COMMENT '0 - Not Sent 1 - Sent',
  `post_service` int(11) NOT NULL,
  `tracking_details` varchar(50) NOT NULL,
  `status` int(1) NOT NULL COMMENT '0 - Sent 1 - Received',
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;


DROP TABLE IF EXISTS `postal_services`;
CREATE TABLE `postal_services` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `service_name` varchar(40) NOT NULL,
  `tracking_url` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=latin1;


-- 2017-06-23 09:58:04
