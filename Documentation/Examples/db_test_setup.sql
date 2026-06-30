-- ====================================================================== --
-- ATTENTION!
-- This script wipes all existing data in database
-- Use with caution!
-- ====================================================================== --


-- ====================================================================== --
-- Wipe existing data
DELETE FROM sch_manager.t_model_roles;
DELETE FROM sch_manager.t_backends;

-- ====================================================================== --
-- ROLE 1
INSERT INTO sch_manager.t_model_roles (version, name, type, config) 
VALUES (101, '0x5a65726f207465737420726f6c65', 'planner', '{"sampleval":1}');
INSERT INTO sch_manager.t_backends (
	device, type,
    ip_addr, ip_port,
    model_role, display_name, last_online
    ) VALUES (
	'36a4aba0dcec23e926f0692065524808479c33f4501bae79348f8f005f10b21c', 0,
	'127.0.0.1', 9001,
	(SELECT id FROM sch_manager.t_model_roles ORDER BY id ASC LIMIT 1),
	'0x53616d706c65206261636b656e64', now()
);

INSERT INTO sch_manager.t_model_roles (version, name, type, config) 
VALUES (102, '0x5a65726f207465737420726f6c65', 'planner', '{"updval":2}');
INSERT INTO sch_manager.t_backends (
	device, type,
    ip_addr, ip_port,
    model_role, display_name, last_online
    ) VALUES (
	'ca7cba0dcecb43e926f0692065524808479c33f4501bae79348f8f005f10b21c', 0,
	'127.0.0.1', 9001,
	(SELECT id FROM sch_manager.t_model_roles ORDER BY id ASC LIMIT 1),
	'0x4e6f7465626f6f6b2041', now()
);


-- ====================================================================== --
-- ROLE 2
INSERT INTO sch_manager.t_model_roles (version, name, type, config) 
VALUES (201, '0x466972737420726f6c65', 'coder', '{"tmp":0.3}');
INSERT INTO sch_manager.t_backends (
	device, type,
    ip_addr, ip_port,
    model_role, display_name, last_online
    ) VALUES (
	'ffcdaba0dcec23e926f0692065524808479c33f4501bae79348f8f005f10b21c', 0,
	'127.0.0.1', 9001,
	(SELECT id FROM sch_manager.t_model_roles ORDER BY id ASC LIMIT 1),
	'0x4e6f7465626f6f6b2042', now()
);
INSERT INTO sch_manager.t_backends (
	device, type,
    ip_addr, ip_port,
    model_role, display_name, last_online
    ) VALUES (
	'8892aba0dcec23e926f0692065524808479c33f4501bae79348f8f005f10b21c', 0,
	'127.0.0.1', 9001,
	(SELECT id FROM sch_manager.t_model_roles ORDER BY id ASC LIMIT 1),
	'0x53696e676c6520416e64726f696420646576696365', now()
);

INSERT INTO sch_manager.t_model_roles (version, name, type, config) 
VALUES (202, '0x466972737420726f6c65', 'coder', '{"temperature":0.3}');


-- ====================================================================== --
-- ROLE 3
INSERT INTO sch_manager.t_model_roles (version, name, type, config) 
VALUES (301, '0x5472616e736c61746f72', NULL, '{"special":{"npovt":3}}');
INSERT INTO sch_manager.t_backends (
	device, type,
    ip_addr, ip_port,
    model_role, display_name, last_online
    ) VALUES (
	'987654ba0dcec23e926f069206552488479c33f4501bae79348f8f005f10b21c', 0,
	'127.0.0.1', 9001,
	(SELECT id FROM sch_manager.t_model_roles ORDER BY id ASC LIMIT 1),
	'0x4f6c64207063', now()
);