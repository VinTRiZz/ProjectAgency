#! /bin/bash

# Check args
if [[ "$1" == "" || "$2" == "" || "$3" == "" || "$4" == "" ]]; then
    echo "Usage: setup_database.sh [DB HOST] [DB PORT] [DB ADMIN USER] [DB DATABASE]"
    exit 1
fi

# Get password of a admin user to execute with no asking
read -s -p "Enter password: " PGPASSWORD
export PGPASSWORD

# Common variables
LOCAL_hostname=$1
LOCAL_port=$2
LOCAL_adminUser=$3
LOCAL_database=$4

# Execution environment
LOCAL_psqlEnv="-v ON_ERROR_STOP=1 -h $LOCAL_hostname -p $LOCAL_port -U $LOCAL_adminUser"

# Execute configuration scripts
LOCAL_scriptDir=$(realpath $(dirname "$0"))
for envScript in "$LOCAL_scriptDir"/*.sql; do
    if [ "$envScript" == "$LOCAL_scriptDir/0.sql" ]; then
        psql $LOCAL_psqlEnv -v CONFIGURE_SERVERUSER="server" -v CONFIGURE_DBNAME="$LOCAL_database" -f "$envScript"
        continue;
    fi

    echo "Executing script: " $envScript
    psql $LOCAL_psqlEnv -v CONFIGURE_SERVERUSER="server" -v CONFIGURE_DBNAME="$LOCAL_database" -d "$LOCAL_database" -f "$envScript"

    if [ $? != 0 ]; then
        echo "[ FAIL ] Failed to execute version script"
        exit 1
    fi
done
