-- ============================================== --
-- ================= Functions ================== --
-- ============================================== --

-- Online time changing from C++ using timestamps
CREATE OR REPLACE FUNCTION sch_manager.f_set_online_time(
	p_backend_id 	sch_manager.t_backends.device%TYPE,
	p_time_utc 		INT8 -- same thing as int64_t
) 
RETURNS VOID
AS $$ BEGIN
	UPDATE sch_manager.t_backends 
	SET last_online = to_timestamp(p_time_utc) 
	WHERE device = p_backend_id;
END $$ LANGUAGE plpgsql;

-- Getting last online time as a timestamp
CREATE OR REPLACE FUNCTION sch_manager.f_get_online_time(
	p_backend_id 	sch_manager.t_backends.device%TYPE
) 
RETURNS BIGINT
AS $$ 
BEGIN
	RETURN (
		SELECT EXTRACT(EPOCH FROM (last_online AT TIME ZONE 'UTC'))::BIGINT
		FROM sch_manager.t_backends 
		WHERE device = p_backend_id 
	);
END $$ LANGUAGE plpgsql;