ID=9

DATASET=imageNet
DATASET_SIZE=2340373
DIMENSION=150
QUERY_SIZE=200
K=20



INDEX=DSA-${DATASET}${ID}
BASE=/data/kabir/similarity-search/dataset/${DATASET}/base.bin
QUERY=/data/kabir/similarity-search/dataset/${DATASET}/query.bin
INDEX_FILE=/data/kabir/similarity-search/dsaIndex/${INDEX}/
logFile=memory-logs/dsa-memory-log-${INDEX}.txt
outputFile=memory-logs/dsa-memory-log-build-${INDEX}.txt

BUFFER_SIZE=9531
LEAF_SIZE=150
NPROBES=100

cd ..
rm -rf $INDEX_FILE

/usr/bin/time -v bin/dstree --dataset $BASE --dataset-size $DATASET_SIZE --buffer-size $BUFFER_SIZE --leaf-size $LEAF_SIZE --index-path $INDEX_FILE --ascii-input 0 --mode 0 --timeseries-size $DIMENSION &> $outputFile

/usr/bin/time -v bin/dstree --queries $QUERY --queries-size $QUERY_SIZE --buffer-size $BUFFER_SIZE --leaf-size $LEAF_SIZE --index-path $INDEX_FILE --ascii-input 0 --mode 1 --timeseries-size $DIMENSION --k $K --epsilon 0 --delta 0 --nprobes $NPROBES &> $logFile
