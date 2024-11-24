ID=2
DATASET='imageNet'

DATASET_SIZE=2340373
DIMENSION=150
QUERY_SIZE=200
K=20


INDEX=ISAX-${DATASET}${ID}
BASE=/data/kabir/similarity-search/dataset/${DATASET}/base.bin
QUERY=/data/kabir/similarity-search/dataset/${DATASET}/query.bin
INDEX_FILE=/data/kabir/similarity-search/isaxIndex/${INDEX}/
logFile=memory-logs/isax-memory-log-${INDEX}.txt
outputFile=memory-logs/isax-memory-log-build-${INDEX}.txt

BUFFER_SIZE=9531
LEAF_SIZE=20000
NPROBES=100
SAX_CARDINALITY=4
INITIAL_LBL_SIZE=100000
FLUSH_LIMIT=1000000
mkdir /data/kabir/similarity-search/isaxIndex/
cd ..
rm -rf $INDEX_FILE


/usr/bin/time -v bin/isax2plus --dataset $BASE --dataset-size $DATASET_SIZE --flush-limit $FLUSH_LIMIT --leaf-size $LEAF_SIZE --index-path $INDEX_FILE --sax-cardinality $SAX_CARDINALITY --initial-lbl-size $INITIAL_LBL_SIZE --timeseries-size $DIMENSION &> $outputFile

/usr/bin/time -v bin/isax2plus --queries $QUERY --queries-size $QUERY_SIZE --dataset-size $DATASET_SIZE --dataset $BASE --index-path $INDEX_FILE --flush-limit $FLUSH_LIMIT --leaf-size  $LEAF_SIZE --sax-cardinality $SAX_CARDINALITY --initial-lbl-size $INITIAL_LBL_SIZE --timeseries-size $DIMENSION --k $K  --use-index &> $logFile