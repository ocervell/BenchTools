#!/usr/bin/python
import socket
import sys
from datetime import datetime
import threading
SERVER_HOST = 'localhost'
SERVER_PORT = 3000
SERVER_PORT_U = 3001
BUFFER = 1024

SEND_TIMES = []
RECEIVE_TIMES = []
SEND_TIMES_U = []
RECEIVE_TIMES_U = []

class client(threading.Thread):
    def __init__(self,data_to_send,len_data,cmd):
        super(client,self).__init__()
        self.client = None
        self.server_addr = (SERVER_HOST,SERVER_PORT)
        self.server_addr_u = (SERVER_HOST,SERVER_PORT_U)
        self.packet_size = 65536
        self.data_to_send = data_to_send
        self.len_data = len_data
        self.cmd = cmd
    
    def send_data(self,data):
        try:
            start_packet_time = datetime.now()
            if self.cmd == 'tcp': 
                self.client.sendall(data)
            else:
                if type(self.data_to_send) == list:
                    for i in self.data_to_send:
                        self.client.sendto(self.data_to_send[1],self.server_addr_u)
                else:        
                    self.client.sendto(data,self.server_addr_u) 
            stop_packet_time = datetime.now()
        except Exception as e:
            print 'Could not send the data.!!'
            print e.message
            print '*'*80
            self.client.close()
            return
                
        return (stop_packet_time - start_packet_time)
    
    def receive_data(self):
        try:
            start_packet_time = datetime.now()
            if self.cmd == 'tcp':
                data = self.client.recv(BUFFER) 
            else:
                data = self.client.recvfrom(BUFFER)     
            stop_packet_time = datetime.now()
        except Exception as e:
            print 'Could not receive the data'
            print e.message
            print '*'*80
            self.client.close()
            return
                
        return (data,(stop_packet_time - start_packet_time))  
            
    def process_data(self,data,packet_size):
        if self.cmd == 'tcp':
            try:
                self.client = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
                self.client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                self.client.connect(self.server_addr)
                print 'Sending Data --->'
                send_time = self.send_data(data)
                receive_time = self.receive_data()
                print 'Data Sent..!!!'
                SEND_TIMES.append(send_time)
                RECEIVE_TIMES.append(receive_time)
                self.client.close()
            except Exception as e:
                print 'Cannot connect to the server..!!'
                print 'Check the connection parameters' 
                print e.message
                print '*'*80
                sys.exit(1)

        elif self.cmd == 'udp':
            try:                       
                self.client = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
                self.client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                print 'Sending Data --->'
                send_time = self.send_data(data)
                receive_time = self.receive_data()
                print 'Data Sent..!!!'
                SEND_TIMES_U.append(send_time)
                RECEIVE_TIMES_U.append(receive_time)
                self.client.close()
            except Exception as e:
                print 'Cannot connect to the server..!!'
                print 'Check the connection parameters' 
                print e.message
                print '*'*80
                sys.exit(1)
        if self.cmd == 'udp':           
            print 'Message from the server:', receive_time[0][0]
        else:    
            print 'Message from the server:', receive_time[0]
        print 'The time taken to send data:',send_time
        print 'The time taken to receive data:',receive_time[1]
        if self.cmd == 'udp' and len(self.data_to_send) == 113:
            print 'Packet Size transmitted:',65536
        elif self.cmd == 'udp' and len(self.data_to_send) in  [56,57,55]:
            print 'Packet Size transmitted:',65536/2
        else:    
            print 'Packet Size transmitted:',packet_size
        print 'Send Time in microseconds:',send_time.microseconds
        print 'The Bandwidth for the application is %d MBytes/Sec' % (((self.packet_size/send_time.microseconds)*(pow(10,6)))/(1024 * 1024))
        print '*'*80

    def run(self):
        print '*'*80
        try:
            self.process_data(self.data_to_send,self.len_data)

        except Exception as e:
            print 'Error processing the data..!!'
            print e.message
            sys.exit(1)    

if __name__ == '__main__':
    # The packet size is varied depending on the user selection
    # Current sizes are 1B, 1KB and 64KB
    if len(sys.argv) != 2:
        print 'Invalid Arguments, usage client.py <tcp | udp>'
        sys.exit(1) 
    elif str(sys.argv[1]).lower() not in ['tcp','udp']:
        print 'Invalid Arguments'
        sys.exit(1)
    else:
        cmd = str(sys.argv[1])
              
    packet = ['A','A'*1024,'A'*1024*64]
    possibilities = [1,2,3]
    try:
        while 1:
            print '\nEnter the packet size:'
            print 'Enter 1 for 1B'
            print 'Enter 2 for 1KB'
            print 'Enter 3 for 64KB\n'
            option = raw_input()
            if int(option) not in possibilities:
                print 'Invalid input.!!'
            else:
                while 1:
                    print 'Enter the number of threads (Max of 2 threads is allowed.)'
                    thr = raw_input()
                    if int(thr) not in [1,2]:
                        print "Invalid Input"
                    else:
                        break;    
                break;    
        data_to_send = packet[possibilities.index(int(option))] 
        if cmd == 'udp' and int(option) == 3:
            data_to_send = ['A'*1024*9]*113 # This is enforced to limit the UDP transfer rate.  
        print '\nNumber of Bytes to send: ', len(data_to_send) 
        print 'Number of Threads used: %d \n' % int(thr) 
        if int(thr) == 1 or int(option) == 1:
            cli = client(data_to_send,len(data_to_send),cmd)
            cli.start()
            cli.join()
        elif int(thr) == 2 and int(option) != 1:
            mid = len(data_to_send) /2
            cli1 = client(data_to_send[:mid],len(data_to_send)/2,cmd)
            cli2 = client(data_to_send[mid:],len(data_to_send)/2,cmd)
            cli1.start()
            cli1.join()
            cli2.start()
            cli2.join() 
    except Exception as e:
        print 'Threads could not be started..!!'
        print e.message
    except KeyboardInterrupt:
        print '*'*78
        print '\nKeyboard Interrupt Caught.!'
        print 'Shutting Down Client..!!!'
        print '*'*80
        sys.exit(1)                
