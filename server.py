#!/usr/bin/python
import socket
import sys
import threading
import time
import datetime
HOSTNAME = 'localhost'
PORT = 3000             # Random port 
PORT_U = 3001
BUFFER = 655363

class multiple_clients(threading.Thread):
    def __init__(self,client):
        super(multiple_clients,self).__init__()
        self.client = client

    def receive_data(self):
        try:
            data = self.client.recv(BUFFER)
            return 'Got the message, Thanks..!!'
        except Exception as e:    
            self.client.close()
            return  'Unable to receive the message..!!'

    def send_data(self,data):
        try:
            self.client.send(data)
        except Exception as e:
            self.client.send('Unable to send the data, Check the connection')
            self.client.close()
            return

    def run(self):
        try:
            client_data = self.receive_data()
            if client_data:
                self.client.send(client_data)
        finally:
            pass        
            #self.client.close()      
            
class server_class(threading.Thread):
    def __init__(self,cmd):
        super(server_class,self).__init__()
        self.cmd = cmd
        self.server = None
        self.threads_ = []
         
    def process_data(self):
        client_connection = None               
        message = 'A'*1024 
        print '*'*80
        print 'Server is now running on port %d' % PORT
        print '*'*80

        while True:
            message_ = ''
            try:
                if self.cmd == 'tcp':
                    self.server = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
                    self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                    self.server.bind((HOSTNAME,PORT))
                    self.server.listen(10)  # Listen upto 10 connections before droping them (queue).
                    client_connection,client_addr = self.server.accept()
                    if client_connection:
                        print '\nConnection Received from: %s on port: %d' % (client_addr[0],client_addr[1])
                        server_receive_start = datetime.datetime.now()
                        message_ += client_connection.recv(BUFFER)
                        server_receive_stop = datetime.datetime.now()
                        client_connection.sendall('Got the message, Thanks..!!')
                        time_diff = server_receive_stop - server_receive_start
                        print 'The size of the packet received is %d Bytes' % len(message_)
                        print 'The Bandwidth for the server application is %d MBytes/Sec' % (((len(message)/time_diff.microseconds)*(pow(10,6)))/(1024 * 1024))
                        client_connection.close()

                elif self.cmd  == 'udp':
                    self.server = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
                    self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                    self.server.bind((HOSTNAME,PORT_U))
                    # Wait for client connection somehow.
                    server_receive_start = datetime.datetime.now() 
                    message_,client_connection = self.server.recvfrom(BUFFER)
                    server_receive_stop = datetime.datetime.now()
                    self.server.sendto('Got the message, Thanks..!!',client_connection)
                    time_diff = server_receive_stop - server_receive_start
                    print 'The size of the packet received is %d Bytes' % len(message_)
                    print 'The Bandwidth for the server application is %d MBytes/Sec' % (((len(message)/time_diff.microseconds)*(pow(10,6)))/(1024 * 1024))
                    self.server.close()
                else:
                    print 'Command not valid..!!'
                    sys.exit(1)
                
                    #multiple_cli = multiple_clients(client_connection)
                    #multiple_cli.setDaemon(True)
                    #thread_ = multiple_cli.start()
                    #self.threads_.append(thread_)
                        
            except Exception as e:
                print '*'*80
                print 'Processing Error..!!'
                print e.message
                print '\nShutting down..!!'
                sys.exit(1)
                raise
                 
            finally:
                self.server.close() 
                #print '*'*80
    def close(self):
        self.server.close()
        
    def run(self):
        self.process_data()

                
if __name__ == '__main__':

    print '*'*80
    print 'Starting Server Daemon..!!'

    if len(sys.argv) != 2:
        print 'Invalid Arguments, usage client.py <tcp | udp>'
        sys.exit(1) 
    elif str(sys.argv[1]).lower() not in ['tcp','udp']:
        print 'Invalid Arguments'
        sys.exit(1)
    else:
        cmd = str(sys.argv[1])         
    try:
        server = server_class(cmd) 
        server.setDaemon(True)
        server.start()
    except Exception as e:
        print 'Server Stopped'
        print e.message
        print '*'*80
        sys.exit(1)

    try:
        while 1:
            time.sleep(1)
    except Exception as e:
        print e.message
    except KeyboardInterrupt:
        print '*'*78
        print '\nKeyboard Interrupt Caught.!'
        print 'Shutting Down Server..!!!'
        print '*'*80
        sys.exit(1)

    finally:
        server.close()             
