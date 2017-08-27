#!/usr/bin/env bash

get_sha()
{
        echo `ls --full-time FleetLang.g4 | sha1sum`
}

get_previous_sha()
{
        echo `cat .FleetLang.g4.timestamp`
}

if [ "$(get_sha)" != "$(get_previous_sha)" ] ; then
        java -jar antlr4.jar -Dlanguage=Cpp -no-listener -o ../gen -visitor FleetLang.g4 && echo "------ Grammar updated" || echo "[EEEE] Error updating grammar!"
        echo $(get_sha) > .FleetLang.g4.timestamp
fi;



