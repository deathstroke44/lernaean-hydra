ID=2
DATASET='sald1m'

DATASET_SIZE=1000000
DIMENSION=128
QUERY_SIZE=100
K=100


INDEX=${DATASET}${ID}
BASE=/data/kabir/similarity-search/dataset/${DATASET}/base.bin
QUERY=/data/kabir/similarity-search/dataset/${DATASET}/query.bin
INDEX_FILE=/data/kabir/similarity-search/vaPlusIndex/${INDEX}/
logFile=vaplus_scripts/logs/${INDEX}.txt
outputFile=vaplus_scripts/logs/build${INDEX}.txt

BUFFER_SIZE=9531
LEAF_SIZE=900
NPROBES=100

cd ..
rm -rf $INDEX_FILE


bin/vaplus --dataset $BASE --dataset-size $DATASET_SIZE --buffer-size $BUFFER_SIZE --leaf-size $LEAF_SIZE --index-path $INDEX_FILE --ascii-input 0 --mode 0 --timeseries-size $DIMENSION > $outputFile

bin/vaplus --queries $QUERY --queries-size $QUERY_SIZE --buffer-size $BUFFER_SIZE --leaf-size $LEAF_SIZE --index-path $INDEX_FILE --ascii-input 0 --mode 1 --timeseries-size $DIMENSION --k $K --epsilon 0 --delta 0 --nprobes $NPROBES > $logFile