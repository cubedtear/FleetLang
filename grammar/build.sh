#!/usr/bin/env bash

function get_sha()
{
        echo `ls --full-time FleetLang.g4 | sha1sum`
}

function get_previous_sha()
{
        echo `cat .FleetLang.g4.timestamp` 2>/dev/null || echo ""
}

#printf "Current: %s\n" "$(get_sha)"
#printf "Old: %s\n" "$(get_previous_sha)"

exit_code=0

if [ "$(get_sha)" != "$(get_previous_sha)" ] ; then
        java -jar antlr4.jar -Dlanguage=Cpp -no-listener -o ../gen -visitor FleetLang.g4
        exit_code=$?;
        echo $(get_sha) > .FleetLang.g4.timestamp
        echo ${exit_code} > ./FleetLang.g4.exitcode
        echo "------ Grammar updated";
else
    exit_code=$(cat ./FleetLang.g4.exitcode)
fi;

if [[ ${exit_code} != 0 ]]; then
    echo "[EEEE] Error updating grammar!"
    exit ${exit_code}
fi



