-- ============================================== --
-- ================== Common ==================== --
-- ============================================== --

-- Main schema, others will be backups
CREATE SCHEMA IF NOT EXISTS sch_manager;
COMMENT ON SCHEMA sch_manager IS 'Primary schema of AIBackend application';

-- Grant permissions for schema
GRANT USAGE ON SCHEMA sch_manager TO :CONFIGURE_SERVERUSER;
GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA sch_manager TO :CONFIGURE_SERVERUSER;
GRANT SELECT, INSERT, UPDATE, DELETE ON ALL TABLES IN SCHEMA sch_manager TO :CONFIGURE_SERVERUSER;
ALTER DEFAULT PRIVILEGES IN SCHEMA sch_manager
GRANT SELECT, INSERT, UPDATE, DELETE ON TABLES TO :CONFIGURE_SERVERUSER;

-- Drop existing (if version is downgraded?)
DROP VIEW sch_manager.view_backend_common;
DROP TABLE sch_manager.t_backends;
DROP TABLE sch_manager.t_model_roles;

-- ============================================== --
-- ============== Tables setup ================== --
-- ============================================== --

-- Table for roles data saving
CREATE TABLE sch_manager.t_model_roles (
	id 			BIGSERIAL PRIMARY KEY,
	version		INT NOT NULL DEFAULT 100 CHECK (version > 0 AND version < 1000), -- Version of role, 100 is 1.0.0
	name 		TEXT NOT NULL DEFAULT 'Default role name', 	-- Display name
	type		TEXT, 										-- Type of role, like "coder" or "planner"
	config 		JSON NOT NULL,								-- Configuration with values contain *.mf file in Ollama

	CONSTRAINT uniq_name_with_version UNIQUE (name, version)
);
COMMENT ON TABLE sch_manager.t_model_roles IS 'AI configurations';
GRANT INSERT, DELETE, SELECT, UPDATE ON TABLE sch_manager.t_model_roles TO "server";


-- Backends metadata
CREATE TABLE sch_manager.t_backends (
	device			VARCHAR(65) NOT NULL PRIMARY KEY,	-- SHA-256 sum of device identification data
	type			INT NOT NULL DEFAULT 0,				-- Type of a backend (PC, Android, etc.)
	ip_addr			TEXT NOT NULL DEFAULT '0.0.0.0',
	ip_port			INT CHECK (ip_port >= 0 AND ip_port <= 65535) NOT NULL,
	model_role		BIGINT,

	-- Displayable info
	display_name 	TEXT NOT NULL,
	last_online		TIMESTAMP(0), -- If NULL then is online now

	-- Constraints
	FOREIGN KEY (model_role) REFERENCES sch_manager.t_model_roles (id) ON DELETE SET NULL
);
COMMENT ON TABLE sch_manager.t_backends IS 'AI backends';
GRANT INSERT, DELETE, SELECT, UPDATE ON TABLE sch_manager.t_backends TO "server";

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
			sch_manager.t_backends AS b
		ORDER BY "name" ASC;
GRANT SELECT ON sch_manager.view_backend_common TO "server";
COMMENT ON VIEW sch_manager.view_backend_common IS 'Common data for control panel interface';
