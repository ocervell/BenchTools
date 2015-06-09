
========================================== BENCHMARKING ============================================

File readme.txt
Here you will find all instructions on how to run either a full benchmark or separate entities.

====================================================================================================
>>> Run a full benchmark <<<
====================================================================================================

bash auto.sh

Reports will be generated in the current folder for each test, under the form report_TEST_TIME
TEST is CPU, MEMORY, DISK or NETWORK. GPU is not included, you'll have to run it manually.
TIME is the current time to the second

The shell script will automatically compile the files. You don't have to do it manually.

It will then automatically run all the experiments. 

IMPORTANT : Near the end of the benchmark, the network benchmark needs your input
You need to :
	1. Open a new terminal
	2. go to the  Prog1_Cervello_Madhavan_Rena/src  directory
	3. Enter the command line  "python client.py tcp"
	4. Choose between the three file sizes proposed (1B, 1kB, 64kB).
	5. After it's done, hit CTRL + C (Windows/Linux) or CMD + C (Mac) to exit the network and go
	   to the next test, which is UDP
	6. Repeat the previous operation for UDP (with for 3. the command line "python client udp" 
	   this time) and the program should finish successfully after that.

====================================================================================================
>>> Separate compilation and run instruction <<<
====================================================================================================

----------------------------------------------------------------------------------------------------
CPU
	make -f make_cpu

	./cpu <nb_threads> 

nb_threads : Specify any number of threads you want to be run


----------------------------------------------------------------------------------------------------
GPU
	Make sure you have a GPU and all the libraries needed

	bash gpu_script.sh

	./gpu <blocksize><nb_threads 

nb_threads : Specify any number of threads you want to be run


----------------------------------------------------------------------------------------------------
MEMORY

	gcc -pthread memory.c -o memory

	./memory <mode><blocksize><nb_threads>

		mode 	   : Specify the mode you want among : s, r
	     			 s = sequential
	     			 r = random
		blocksize  : Specify any blocksize you want (in bytes).
		nb_threads : Specify any number of threads you want to be run

----------------------------------------------------------------------------------------------------
DISK

	gcc -pthread disk.c -o disk
	
	./disk <mode><blocksize><nb_threads>

		mode	   : Specify the mode you want among ws, wr, rs, rr.
	     			 w = write
	      			 r = read
	      			 s = sequential
	     			 r = random
		blocksize  : Specify any blocksize you want (in bytes). 1MB = 1000000B
		nb_threads : Specify any number of threads you want to be run

----------------------------------------------------------------------------------------------------
NETWORK

TCP
	python server.py tcp

	> Open a new terminal

	> Go to the src directory

	python client.py tcp

UDP
	python server.py udp

	> Open a new terminal

	> Go to the src directory

	python client.py udp


====================================================================================================
>>> Additional information <<<
====================================================================================================

The CPU module will run 3 tests in a row whereas the others only generate one test (so you would
have to runthem more than once if you want more test to be done).