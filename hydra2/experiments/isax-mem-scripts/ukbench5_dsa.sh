ID=5

DATASET=ukbench
DATASET_SIZE=1097907
DIMENSION=128
QUERY_SIZE=200
K=20



INDEX=${DATASET}${ID}
BASE=/data/kabir/similarity-search/dataset/${DATASET}/base.bin
QUERY=/data/kabir/similarity-search/dataset/${DATASET}/query.bin
INDEX_FILE=/data/kabir/similarity-search/dsaIndex/${INDEX}/
logFile=dsa_scripts/logs/memory-log-${INDEX}.txt
outputFile=dsa_scripts/logs/memory-log-build${INDEX}.txt

BUFFER_SIZE=9531
LEAF_SIZE=4000
NPROBES=200

cd ..
rm -rf $INDEX_FILE

/usr/bin/time -v bin/dstree --dataset $BASE --dataset-size $DATASET_SIZE --buffer-size $BUFFER_SIZE --leaf-size $LEAF_SIZE --index-path $INDEX_FILE --ascii-input 0 --mode 0 --timeseries-size $DIMENSION &> $outputFile

/usr/bin/time -v bin/dstree --queries $QUERY --queries-size $QUERY_SIZE --buffer-size $BUFFER_SIZE --leaf-size $LEAF_SIZE --index-path $INDEX_FILE --ascii-input 0 --mode 1 --timeseries-size $DIMENSION --k $K --epsilon 0 --delta 0 --nprobes $NPROBES &> $logFile

