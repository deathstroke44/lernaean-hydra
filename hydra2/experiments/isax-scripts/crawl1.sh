ID=1
DATASET='crawl'

DATASET_SIZE=1989995
DIMENSION=300
QUERY_SIZE=10000
K=100


INDEX=${DATASET}${ID}
BASE=/data/kabir/similarity-search/dataset/${DATASET}/base.bin
QUERY=/data/kabir/similarity-search/dataset/${DATASET}/query.bin
INDEX_FILE=/data/kabir/similarity-search/isaxIndex/${INDEX}/
logFile=isax-scripts/logs/${INDEX}.txt
outputFile=isax-scripts/logs/build${INDEX}.txt

BUFFER_SIZE=9531
LEAF_SIZE=20000
NPROBES=100
SAX_CARDINALITY=8
INITIAL_LBL_SIZE=100000
FLUSH_LIMIT=1000000
mkdir /data/kabir/similarity-search/isaxIndex/
cd ..


bin/isax2plus --dataset $BASE --dataset-size $DATASET_SIZE --flush-limit $FLUSH_LIMIT --leaf-size $LEAF_SIZE --index-path $INDEX_FILE --sax-cardinality $SAX_CARDINALITY --initial-lbl-size $INITIAL_LBL_SIZE $DIMENSION > $outputFile

bin/isax2plus --queries $QUERY --queries-size $QUERY_SIZE --dataset-size $DATASET_SIZE --dataset $BASE --index-path $INDEX_FILE --flush-limit $FLUSH_LIMIT --leaf-size  $LEAF_SIZE --sax-cardinality $SAX_CARDINALITY --initial-lbl-size $INITIAL_LBL_SIZE --timeseries-size --mode 1 $DIMENSION --k $K  --use-index > $logFile