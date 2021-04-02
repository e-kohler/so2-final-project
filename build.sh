set -e

make clean
make APPLICATION=$1
make APPLICATION=$1 run 