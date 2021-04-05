set -e

make veryclean
make $2 APPLICATION=$1
make APPLICATION=$1 run 
