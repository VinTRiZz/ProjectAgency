-- Main script with user configuration

\echo "Recreating database...";
DROP DATABASE IF EXISTS :CONFIGURE_DBNAME;
CREATE DATABASE :CONFIGURE_DBNAME;

-- Create user (no need in strong password, server has no privilegies)
\echo "Recreating roles and user...";
SET custom.serv_usr = :'CONFIGURE_SERVERUSER';
DO $$
DECLARE
    v_rolename TEXT := current_setting('custom.serv_usr');
BEGIN
    IF EXISTS (SELECT 1 FROM pg_roles WHERE rolname = v_rolename) THEN
        EXECUTE FORMAT ('REASSIGN OWNED BY %I TO postgres', v_rolename);
        EXECUTE FORMAT ('DROP OWNED BY %I', v_rolename);
    END IF;
END
$$;
DROP USER IF EXISTS :CONFIGURE_SERVERUSER;
DROP ROLE IF EXISTS :CONFIGURE_SERVERUSER;
CREATE USER :CONFIGURE_SERVERUSER WITH PASSWORD 'serv_auth_password';

-- Remove server user privilegies
\echo "Revoking privileges of a server...";
REVOKE ALL ON SCHEMA public FROM public;
REVOKE ALL ON DATABASE :CONFIGURE_DBNAME FROM :CONFIGURE_SERVERUSER;

-- Grant basic permissions for working
\echo "Granting usage without editing for server...";
GRANT USAGE ON SCHEMA public TO :CONFIGURE_SERVERUSER;
GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO :CONFIGURE_SERVERUSER;

-- Grant CRUD on common tables
\echo "Grant CRUD on tables in schema for server...";
GRANT SELECT, INSERT, UPDATE, DELETE ON ALL TABLES IN SCHEMA public TO :CONFIGURE_SERVERUSER;
ALTER DEFAULT PRIVILEGES IN SCHEMA public
GRANT SELECT, INSERT, UPDATE, DELETE ON TABLES TO :CONFIGURE_SERVERUSER;
