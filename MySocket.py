import socket

class MySocket:
    def __init__(self, sock=None):
        if sock is None:
            self.sock = socket.socket(
                socket.AF_INET, socket.SOCK_STREAM)

        else:
            self.sock = sock

    def connect(self, host, port):
        self.sock.connect( (host, port) )

    def send_some(self, msg):
        totalsent = 0
        while totalsent < len(msg):
            sent = self.sock.send(msg[totalsent:])
            if sent == 0:
                raise RuntimeError("socket connection failure")
            totalsent = totalsent + sent

    def recv_some(self):
        chunk = self.sock.recv(2048)
        if chunk == b'':
            raise RuntimeError("socket recv failure")
        return b''.chunk


