import socket
import sys
import time

msg_len = 2048
server_listen_max = 10

def close_socket(socket_fd):
    if socket_fd is not None:
        socket_fd.close()

def Tcp_client(ip, port, msg):
    try:
        tc_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        tc_sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        tc_addr = (ip, int(port))
        tc_sock.connect(tc_addr)
        while 1:
            tc_sock.send(msg.encode('utf-8'))
            resp = tc_sock.recv(msg_len)
        print("接收到服务端消息：{}".format(resp.decode('utf-8')))
    except Exception as e:
        print("异常：{}".format(e))
    finally:
        close_socket(tc_sock)

def Tcp_server(ip, port):
    try:
        ts_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        ts_addr = (ip, int(port))
        ts_sock.bind(ts_addr)
        ts_sock.listen(server_listen_max)
        print("服务端正在监听")
        while True:
            c_sock, addr = ts_sock.accept()
            print(str(addr) + " 已连接到服务端")
            data = c_sock.recv(msg_len)
            if not data:
                print("接收到错误的消息")
                break
            #print("服务端成功接收到数据：{}".format(data.decode('utf-8')))
            resp = "服务端成功接收到数据：{}".format(data.decode('utf-8'))
            print(resp)
            c_sock.send(resp.encode('utf-8'))
            #time.sleep(1)
    except Exception as e:
        print("异常：{}".format(e))
    finally:
        close_socket(ts_sock)

def Udp_server(ip, port):
    try:
        us_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        us_sock.bind((ip, int(port)))
        while True:
            data, addr = us_sock.recvfrom(msg_len)
            #print("收到消息：{}".format(data.decode('utf-8')))
            resp = "已收到消息：{}".format(data.decode('utf-8'))
            print(resp)
            us_sock.sendto(resp.encode('utf-8'), addr)
            time.sleep(1)
    except Exception as e:
        print("异常：{}".format(e))
    finally:
        close_socket(us_sock)

def Udp_client(ip, port, msg):
    try:
        uc_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        uc_sock.sendto(msg.encode('utf-8'), (ip, int(port)))
        print("已发送消息：{}".format(msg))
        uc_sock.settimeout(5)
        data, addr = uc_sock.recvfrom(msg_len)
        print("收到消息：{}".format(data.decode('utf-8')))
    except Exception as e:
        print("异常：{}".format(e))
    finally:
        close_socket(uc_sock)

def usage():
    print("Usage：")
    print("TCP server python3 pycs.py -ts ip port")
    print("TCP client python3 pycs.py -tc ip port msg")
    print("UDP server python3 pycs.py -us ip port")
    print("UDP client python3 pycs.py -uc ip port msg")

if __name__ == '__main__':
    argc = len(sys.argv)
    print("argc = {}".format(argc))
    for i in range(argc):
        print("{}: {}".format(i, sys.argv))
    if len(sys.argv) == 2 and sys.argv[1] == "-h":
        usage()
    elif len(sys.argv) == 4 and sys.argv[1] == "-ts":
        Tcp_server(sys.argv[2], sys.argv[3])
    elif len(sys.argv) == 5 and sys.argv[1] == "-tc":
        Tcp_client(sys.argv[2], sys.argv[3], sys.argv[4])
    elif len(sys.argv) == 4 and sys.argv[1] == "-us":
        Udp_server(sys.argv[2], sys.argv[3])
    elif len(sys.argv) == 5 and sys.argv[1] == "-uc":
        Udp_client(sys.argv[2], sys.argv[3], sys.argv[4])
    else:
        usage()
