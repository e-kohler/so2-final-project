set -e

make veryclean
make APPLICATION=$1
make APPLICATION=$1 run 