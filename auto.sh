NOW=$(date +"%m-%d-%Y.%H.%M.%S")
TIME="5 4 3 2 1"
REPORT_FILE_CPU="report_CPU."$NOW".txt"
REPORT_FILE_GPU="report_GPU."$NOW".txt"
REPORT_FILE_MEMORY="report_MEMORY."$NOW".txt"
REPORT_FILE_DISK="report_DISK."$NOW".txt"

NB_THREADS="1 2 4"
NB_THREADS_MEM="1 2"
BLOCKSIZE="1 1000 1000000"
BLOCKSIZE_MEM="1 1000 999999"
MODE="ws wr rs rr"
MODE_MEM="s r"
MODE_NET="tcp udp"
echo ""
echo "==================================================================================================="
echo "BENCHMARKING PROGRAM "
echo "	This shell script will automatically benchmark your CPU, MEMORY, DISK and NETWORK."
echo "  You will have to run GPU separately (see readme.txt for instructions)."
echo "	A report file for each component will be generated, under the form report_COMPONENT.TIME"
echo "	THIS MIGHT TAKE A WHILE. You can grab a coffee and watch the news."
echo "==================================================================================================="
echo ""
for i in $TIME; do
	echo "Starting in $i"
	sleep 1	
done
echo ""
echo "Starting ..."
sleep 2
echo ""
echo ""
echo "Removing previous remaining temp files and reports ..."
rm temp* report_* 2> errors.txt

echo "done."
echo ""

echo "Compiling all files ..."
	make -f make_cpu
	gcc -pthread memory.c -o memory
	gcc -pthread disk.c -o disk
echo "done."
echo ""


echo "======= CPU BENCHMARKING ======"
echo""
echo ""
for threads in $NB_THREADS; do
	echo "Number of threads : $threads"
	./cpu $threads >> $REPORT_FILE_CPU
echo "done."
done
echo "CPU benchmarking ended successfullyi, file $REPORT_FILE_CPU generated"

sleep 1

echo ""
echo "======= MEMORY BENCHMARKING ======="
echo ""

echo ""
echo "Benchmarking ..."
echo ""
for mode in $MODE_MEM; do
	echo "Mode: $mode"
	for blocksize in $BLOCKSIZE_MEM; do
		echo "    Blocksize: $blocksize Bytes";
		for threads in $NB_THREADS_MEM; do
			echo "     Number of threads: $threads"
			sleep 2 
			./memory $mode $blocksize $threads >> $REPORT_FILE_MEMORY
		done
		echo "     done."
	done
	echo "    done."
done
echo ""
echo "Removing temporary files ..."
echo "done."
echo ""
echo "Memory benchmarking ended successfully."
echo ""

sleep 1

echo "======= DISK BENCHMARKING ======="
echo ""
echo "Benchmarking ..."
echo ""
for mode in $MODE; do
	echo "Mode: $mode"
	for blocksize in $BLOCKSIZE; do
		echo "    Blocksize: $blocksize Bytes";
		for threads in $NB_THREADS; do
			echo "     Number of threads: $threads"
			sleep 2 
			./disk $mode $blocksize $threads >> $REPORT_FILE_DISK
			rm temp*
		done
		echo "     done."
	done
	echo "    done."
done
echo ""
echo "Removing temporary files ..."
echo "done."
echo ""
echo "Disk benchmarking ended successfully, file $REPORT_FILE_DISK"

sleep 1

echo "======= NETWORK BENCHMARKING ======="
echo ""
echo "Benchmarking ..."
echo ""
echo "TCP TEST"
echo "PLEASE OPEN A NEW TERMINAL AND RUN python client.py tcp FROM THE SRC DIRECTORY ..."
echo ""
	python server.py tcp
echo ""
echo ""
echo "PRESS ENTER TO CONTINUE"
echo ""
read enter
echo "UDP TEST"
echo "PLEASE OPEN A NEW TERMINAL AND RUN python client.py udp FROM THE SRC DIRECTORY ..."
echo ""
	python server.py udp
echo ""
echo "PRESS ENTER TO CONTINUE"

echo "Deleting remaining files ..."
rm error.txt 2>l.txt
rm l.txt

