#!/usr/bin/env bash

set -e

resultdir=$1
grammar=$2

mkdir -p $resultdir

if [ ! -f $grammar ]; then
    echo "Grammar file $grammar does not exists in $PWD"
    exit -1
fi

function get_sha()
{
        echo `ls --full-time ${grammar} | sha1sum`
}

function get_previous_sha()
{
    if [ ! -f ".FleetLang.g4.timestamp" ]; then
        echo ""
    else
        cat .FleetLang.g4.timestamp
    fi
}

#printf "Current: %s\n" "$(get_sha)"
#printf "Old: %s\n" "$(get_previous_sha)"

exit_code=0
pushd ${resultdir} > /dev/null
if [ "$(get_sha)" != "$(get_previous_sha)" ] ; then
        popd > /dev/null
        java -jar antlr4.jar -Dlanguage=Cpp -no-listener -o ${resultdir} -visitor ${grammar}
        exit_code=$?;
        pushd ${resultdir} > /dev/null
        echo $(get_sha) > .FleetLang.g4.timestamp
        echo ${exit_code} > ./FleetLang.g4.exitcode
else
    exit_code=$(cat ./FleetLang.g4.exitcode)
fi;
popd > /dev/null

if [[ ${exit_code} != 0 ]]; then
    echo "[EEEE] Error updating grammar!"
    exit ${exit_code}
fi



