import socket
from datetime import datetime
from time import time
from tabulate import tabulate
from rich.table import Table
from rich.console import Console
from math import round


BUFSIZE=1024
def statistics(measurements:list,jitter_measurements,one_time_delay_measurements):
    console=Console()
    print('Measurements')
    print('*'*10)
    table=Table(title="Raw Timers")
    table.add_column("ID",justify="left",style="blue")
    table.add_column("TIME",justify="left",style="blue")
    for index,timer in enumerate(measurements):
        table.add_row(f"Timer{index+1}",f'{timer}')

    table2=Table(title='Intervals')
    table2.add_column("Interval id")
    table2.add_column("Measurement")
    for time_index in range(len(measurements)-1):
        table2.add_row(f'Interval_{time_index+1}',f'{measurements[time_index+1]-measurements[time_index]-1}')

    console.print(table)
    console.print(table2)

    console.print(f'Measurements',style='green')
    console.print(f'Throughput:{round((BUFSIZE*message_count*1e-3)/(measurements[len(measurements)-1]-measurements[0]),3)} Kbps',style='green')
    console.print(f'Jitter:{(sum(jitter_measurements)/len(jitter_measurements))*1e+3} ms',style='green')
    console.print(f'One Way Delay:{(sum(one_time_delay_measurements)/len(one_time_delay_measurements))*1e+3} ms',style='green')


def handshake(ip_address='127.0.0.1',port='8080',dedicated="server",interval=10,experiment_time=60):
    console=Console()

    # Failover
    if dedicated not in ["server","client"]:
        raise f"{dedicated} not client or server"
    

    if dedicated=="server":
        server_socket=socket.socket(family=socket.AF_INET,type=socket.SOCK_STREAM)
        server_socket.bind((ip_address,port))
        server_socket.listen(2)
        conn,address=server_socket.accept()
        print(f'Hostname:{socket.gethostname()}\nHost ip address:{ip_address}')
        print(f'Connection accepted from:{address}')
        
        
        packages=list()
        jitter_measurements=list()
        one_way_delay_measurements=list()
        previous_timer=None
        package_counter=0
        start_time=time()
        step=0

        while True:
            start_transimission_timer=time()
            data=conn.recv(BUFSIZE).decode()
            if not data:
                break
            encoded_response=f'Dit Uoi Datetime:{datetime.strftime("%Y%M%d %H%M%S")}'.encode()
            conn.send(encoded_response)
            end_transmission_timer=time()
            
            # Throughput, jitter, owd
            packages.append(len(data)+len(encoded_response))
            if package_counter>0:
                jitter_measurements.append(end_transmission_timer-previous_timer-(end_transmission_timer-start_transimission_timer))
            previous_timer=end_transmission_timer
            one_way_delay_measurements.append(end_transmission_timer-start_transimission_timer)

            if end_transmission_timer-start_time>experiment_time:
                console.print(f"Terminate handshake with server| Stop criterion:[time={experiment_time}]",style='red')
                break
            
            if (end_transmission_timer-start_time)%interval==0:
                throughput=sum(packages)*1e-3/(time()-start_time)
                jitter=sum(jitter_measurements)*1e-3/len(jitter)
                owd=sum(one_way_delay_measurements)*1e-3/len(one_way_delay_measurements)
                step+=interval

        conn.close()
    
    elif dedicated=='client':
        message_counter=0
        measurements=list()
        if not port.isdigit():
            print(f'You did not provide the right type of element({type(port)}):{port}')
            return
        
        measurements.append(time())
        client_socket=socket.socket(family=socket.AF_INET,type=socket.SOCK_STREAM)
        measurements.append(time())

        measurements.append(time())
        client_socket.connect((ip_address,port))
        measurements.append(time())

        jitter_packages=list()
        one_time_delay_packages=list()
        prev_time=0
        while True:
            message=input('->')
            start_time=time()
            client_socket.send(message.encode())
            data=client_socket.recv(BUFSIZE).decode()
            end_time=time()
            prev_time=end_time
            one_time_delay_packages.append(end_time-start_time)
            message_counter+=1

            print(f'$Received from server:{data}')
            if message.lower().strip()=='exit':
                break

        client_socket.shutdown(True)
        client_socket.close()
        measurements.append(time())

        statistics(measurements,jitter_packages,one_time_delay)

        

if __name__=='__main__':
    handshake()





