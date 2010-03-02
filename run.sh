#!/bin/bash


echo "Checking permissions..."
if [ "`id -u`" != "0" ]; then 
	echo "ERROR: Need to be root to run this script. Use 'sudo' command."
	exit
fi

curr_dir=$PWD

# Run first
echo "Running first task..."
./test 20 50 1.txt &
echo "Retrieving first task PID"
pid_1="$pids $!"
echo "PID of first task: $pid_1"

# Run second
echo "Running second task..."
./test 20 50 2.txt &
echo "Retrieving second task PID"
pid_2="$pids $!"
echo "PID of second task: $pid_2"

# Set affinity

echo "Setting affinity: taskset 0x00000001 -p $pid_1"
taskset -p 0x00000001 $pid_1
echo "Setting affinity: taskset 0x00000001 -p $pid_2"
taskset -p 0x00000001 $pid_2

## Doesn't work...
## pids=`ps aux | grep -v PID | awk '{print $2}' | xargs -r`
## for task in $pids; do
## 	taskset -p 0x00000001 $task
## done

#Cleaning
umount ./cgroups
rm -fr ./cgroups


# Cgroups
echo "Mounting cgroups"
mkdir ./cgroups
mount -t cgroup -ocpu none ./cgroups
mkdir ./cgroups/first ./cgroups/second
echo 1024 > ./cgroups/first/cpu.shares
echo 1024 > ./cgroups/second/cpu.shares

echo $pid_1 > ./cgroups/first/tasks
echo $pid_2 > ./cgroups/second/tasks

taskset -p $pid_1
taskset -p $pid_2

wait $pid_1
wait $pid_2

#Cleaning
umount ./cgroups
rm -fr ./cgroups
