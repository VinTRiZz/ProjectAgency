INSERT INTO sch_manager.t_model_roles (version, name, type, config) 
VALUES (101, 'Zero test role', 'primarytype', '{"sampleval":1}');
INSERT INTO sch_manager.t_model_roles (version, name, type, config) 
VALUES (102, 'Zero test role', 'primarytype', '{"updval":2}');

INSERT INTO sch_manager.t_model_roles (version, name, type, config) 
VALUES (201, 'First role', 'secondarytype', '{"tmp":0.3}');
INSERT INTO sch_manager.t_model_roles (version, name, type, config) 
VALUES (202, 'First role', 'secondarytype', '{"temperature":0.3}');

INSERT INTO sch_manager.t_model_roles (version, name, type, config) 
VALUES (301, 'Sample role', NULL, '{"special":{"npovt":3}}');
INSERT INTO sch_manager.t_model_roles (version, name, type, config) 
VALUES (302, 'Sample role', NULL, '{"special":{"npovt":5}}');

SELECT * FROM sch_manager.t_model_roles;