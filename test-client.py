import socket
import time
import multiprocessing


def client():
    s = socket.socket()
    host = socket.gethostname()
    port = 8080
    s.connect((host, port))
    cnt = 0
    while True:
        s.send(b'Hello server!')
        s.recv(1024)
        time.sleep(0.01)
        cnt += 1
        if cnt > 2000:
            s.close()
            break


if __name__ == '__main__':
    p_list = []
    for i in range(1200):
        p = multiprocessing.Process(target=client)
        p_list.append(p)
        p.start()
    for p in p_list:
        p.join()
    print('end')
