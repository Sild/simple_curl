#/bin/bash -xe

DATA_PATH_UTF="./tests/data/test_cp1251_utf.data"
DATA_PATH_CP="./tests/data/test_cp1251_cp.data"
echo "" >> ${DATA_PATH_UTF}
for i in $(seq 1 50000); do
    echo "somerandomtextwith${i}" >> ${DATA_PATH_UTF}
done
iconv -f UTF-8 -t CP1252 ${DATA_PATH_UTF} > ${DATA_PATH_CP}

make sgrep
./sgrep ${DATA_PATH_CP} "*10*"