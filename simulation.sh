hash=$1;
version=$2;
mode=$3;

cd NeMo-KV/$hash/$version;
make clean; make;
cd ../../..;
sh NeMo-KV/$hash/$version/simulation.sh $mode;
