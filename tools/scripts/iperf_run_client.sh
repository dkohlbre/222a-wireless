#!/bin/bash
# Where is iperf2?
iperf_path=iperf
#250M files
size=250M
#Server to connect to
server=192.168.5.60
#set to -R to turn on server sends to client
reverse=-R
#default port
port=5001

########End Config###########
if [ $# -lt 1  ]; then
    echo "Usage $0 run_type(can be non-unique) port[if not 5201]"
    exit 1
fi

if [ $# -eq 2 ]; then
    port=$2
fi

mkdir -p results

name=results/$1
i=1
if [[ -e $name.$i ]] ; then
    while [[ -e $name.$i ]] ; do
        let i++
    done
fi
name=$name.$i
if hash unbuffer 2>/dev/null; then
    header=unbuffer
else
    header=
    echo "You don't have unbuffer, but this is running"
fi

name=$name

run="$header $iperf_path -i1 -n $size -c $server -p $port $reverse | tee -a $name"

date | tee $name
echo $run | tee -a $name
eval $run
eval $run
eval $run
eval $run
