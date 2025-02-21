import socket

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(('localhost', 1234))

player_id = 123
client.send(player_id.to_bytes(4, byteorder='little'))

x = 10
y = 20
client.send(x.to_bytes(4, byteorder='little'))
client.send(y.to_bytes(4, byteorder='little'))
