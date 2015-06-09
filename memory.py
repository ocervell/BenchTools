#!/usr/bin/python
# Parameters: <read/write> <is_random> <block_size> <n_threads>
import sys
import threading
from random import sample
from datetime import datetime
# This limit is to set to get a maximum of 1MB of memory access.
MAX_SIZE = 1024*1024 
TIMING = []
BANDWIDTH = []

class memory(threading.Thread):

    def __init__(self,cmd,is_ran,blks):
        super(memory,self).__init__()
        self.cmd = cmd
        self.ran = int(is_ran)
        self.blks = int(blks)
        if self.blks > MAX_SIZE:
            print 'The maximum block size is 1MB'
            sys.exit(1)
        self.in_memory = ['M']*MAX_SIZE
            
    # This function reads random data (random access) from the array in memory.        
    def memory_read_random(self):
        for var in sample(xrange(MAX_SIZE),self.blks):
            tmp = self.in_memory[var]
    
    # This function writes into random position of the array in memory (random writes).
    def memory_write_random(self):
        tmp = 'Z'
        for var in sample(xrange(MAX_SIZE),self.blks):
            self.in_memory[var] = tmp
    
    # This function reads sequentially from the memory array (sequential access).
    def memory_read_sequential(self):
        for var in xrange(self.blks):
            tmp = self.in_memory[var]
    
    # This function writes sequentially from the memory (sequential writes).
    def memory_write_sequential(self):
        tmp = 'X'
        for var in xrange(self.blks):
            self.in_memory[var] = tmp

    # This function controls the thread execution flow.        
    def run(self):
        if self.ran == 1:
            if self.cmd == 'read':
                r_read_start = datetime.now()
                self.memory_read_random()
                r_read_stop = datetime.now() 
                diff = r_read_stop - r_read_start
                TIMING.append(diff.total_seconds()) 
                BANDWIDTH.append(self.blks/diff.total_seconds())
                print 'Thread execution time:', TIMING[-1]
                print 'Memory Bandwidth for current thread: %f Bytes/Sec' % BANDWIDTH[-1] 
            else:
                r_write_start = datetime.now()
                self.memory_write_random()
                r_write_stop = datetime.now()  
                diff = r_write_stop - r_write_start
                TIMING.append(diff.total_seconds())  
                BANDWIDTH.append(self.blks/diff.total_seconds()) 
                print 'Thread execution time:', TIMING[-1]
                print 'Memory Bandwidth for current thread: %f Bytes/Sec' % BANDWIDTH[-1] 
        else:
            if self.cmd == 'read':
                read_start = datetime.now()
                self.memory_read_sequential()
                read_stop = datetime.now()  
                diff = read_stop - read_start
                TIMING.append(diff.total_seconds())
                BANDWIDTH.append(self.blks/diff.total_seconds()) 
                print 'Thread execution time:', TIMING[-1]
                print 'Memory Bandwidth for current thread: %f Bytes/Sec' % BANDWIDTH[-1] 
            else:       
                write_start = datetime.now()
                self.memory_write_sequential()
                write_stop = datetime.now()  
                diff = write_stop - write_start
                TIMING.append(diff.total_seconds())  
                BANDWIDTH.append(self.blks/diff.total_seconds()) 
                print 'Thread execution time:', TIMING[-1]
                print 'Memory Bandwidth for current thread: %f Bytes/Sec' % BANDWIDTH[-1] 
    
if __name__ == '__main__':
    if len(sys.argv) != 5:
        print 'Usage: memory.py <read/write> <random/not_random> <blocks in bytes> <thread count>'
        sys.exit(1)
    else:
        for i in range(int(sys.argv[4])):
            mem = memory(sys.argv[1].lower(),sys.argv[2],sys.argv[3])
            print '\nThread %d Started' % (i+1)
            mem.start()
            mem.join()

        sum_ = 0
        avg_bw = 0  
        for val in range(1,int(sys.argv[4])+1):
            sum_ += TIMING[-(val)]
            avg_bw += BANDWIDTH[-(val)]

        print '\nTOTAL NUMBER OF THREADS:',sys.argv[4]
        print 'TOTAL TIME TAKEN       :',sum_
        print 'Averange Bandwidth is  : %f Bytes/Sec\n' %  (avg_bw /float(sys.argv[4]))
