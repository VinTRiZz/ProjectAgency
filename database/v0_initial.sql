-- ============================================== --
-- ================== Common ==================== --
-- ============================================== --

-- Main schema, others will be backups
CREATE SCHEMA IF NOT EXISTS sch_manager;
COMMENT ON SCHEMA sch_manager IS 'Primary schema of AIBackend application';


-- ============================================== --
-- ============== Tables setup ================== --
-- ============================================== --

-- Table for roles data saving
CREATE TABLE sch_manager.t_model_roles (
	id 			BIGSERIAL PRIMARY KEY,
	version		INT NOT NULL DEFAULT 100, 					-- Version of role, 100 is 1.0.0
	name 		TEXT NOT NULL DEFAULT 'Default role name', 	-- Display name
	type		TEXT, 										-- Type of role, like "Coder" or "Planner"

	-- Configuration must be specified or not, in case of "thinking" mode or other extras
	config 		JSON  CHECK (config IS NOT NULL AND config_special IS NULL),
	config_special	JSON CHECK (config IS NULL AND config_special IS NOT NULL)
);
COMMENT ON TABLE sch_manager.t_model_roles IS 'AI configurations';
GRANT INSERT, DELETE, SELECT, UPDATE ON TABLE sch_manager.t_model_roles TO "server";


-- Backends metadata
CREATE TABLE sch_manager.t_backends (
	device			VARCHAR(64) NOT NULL PRIMARY KEY,	-- SHA-256 sum of device identification data
	type			INT NOT NULL DEFAULT 0,				-- Type of a backend (PC, Android, etc.)
	ip_addr			TEXT NOT NULL DEFAULT '0.0.0.0',
	ip_port			INT CHECK (ip_port >= 0 AND ip_port <= 65535) NOT NULL,
	model_role		BIGINT,

	-- Displayable info
	display_name 	TEXT NOT NULL DEFAULT 'AI Backend',
	last_online		TIMESTAMP(2) NOT NULL,

	-- Constraints
	FOREIGN KEY (model_role) REFERENCES sch_manager.t_model_roles (id)
);
COMMENT ON TABLE sch_manager.t_backends IS 'AI backends';
GRANT INSERT, DELETE, SELECT, UPDATE ON TABLE sch_manager.t_backends TO "server";


-- Event history of a devices. Must be harvested every second by AIManager
CREATE TABLE sch_manager.t_backends_history (
	id BIGSERIAL NOT NULL PRIMARY KEY,

	level		INTEGER CHECK (level > 0 AND level < 4) NOT NULL DEFAULT 0, -- 3 +debug, 2 +ok, 1 +warn, 0 err
	message 	TEXT,
	sender		VARCHAR(64), 			-- If NULL then sender is AIBackend
	type		VARCHAR(32) NOT NULL	-- Type of an event, such as "Prompt" or "Power"
);
COMMENT ON TABLE sch_manager.t_backends IS 'Events history';
GRANT INSERT, DELETE, SELECT ON TABLE sch_manager.t_backends TO "server";


-- ============================================== --
-- =============== Views setup ================== --
-- ============================================== --

-- Backend info to work from control panel
CREATE OR REPLACE VIEW sch_manager.view_backend_common AS
	SELECT 
		b.device 		AS "device",
		b.display_name 	AS "name",
		mr.name 		AS "role",
		b.last_online	AS "last_online"
		FROM
			sch_manager.t_model_roles AS mr,
			sch_manager.t_backends AS b;
GRANT SELECT ON sch_manager.view_backend_common TO "server";
COMMENT ON VIEW sch_manager.view_backend_common IS 'Common data for control panel interface';
