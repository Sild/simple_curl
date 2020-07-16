#!/bin/bash
RUN_COUNT=99999
APP="./cmake-build-debug/data_check_stable_algo"
APP="./cmake-build-debug/data_check"
ARGS="--gen-th-count=19 --block-count=1570 --block-size=40 --calc-th-count=34"
#ARGS="--gen-th-count=5 --block-count=47 --calc-th-count=9 --block-size=150"
#ARGS="--gen-th-count=5 --block-count=13 --calc-th-count=2 --block-size=150"
#ARGS="--gen-th-count=2 --block-count=2 --calc-th-count=2 --block-size=15"
OUTPUT_DIR="output_log"

mkdir -p ${OUTPUT_DIR}
rm -rf ${OUTPUT_DIR}/*.log

for i in $(seq 1 ${RUN_COUNT}); do
    if (( $i % 500 == 0 )); then
        echo "cleaning directory for next logs ..."
        rm -rf ${OUTPUT_DIR}/*.log
        echo "cleaned."
    fi
    echo "${i} starting with args='${ARGS}'..."
    logfile=${OUTPUT_DIR}/${i}.log
    echo "RUN: '${APP} ${ARGS}'" > ${logfile}
    ${APP} ${ARGS} >> ${logfile} 2>&1 || exit 1
    echo "${i} done."
    echo ""
done;
