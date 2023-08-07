
import socket
import time
import os
from binance.client import Client

# init
api_key = os.environ.get('binance_api')
api_secret = os.environ.get('binance_secret')
client = Client(api_key, api_secret)
 
if __name__ == '__main__':
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(("localhost", 8888))
    server.listen(0)
    connection, address = server.accept()
    print(connection, address)
    print("___________________________________________________")
    while True:
        btc_price = client.get_symbol_ticker(symbol='BTCUSDT')
        # print(btc_price)
        print(btc_price['price'])
        recv_str=connection.recv(1024)
        recv_str=recv_str.decode("utf8")
        if not recv_str:
            break
        print("FROM CPP: ", recv_str) 
        connection.send( bytes(btc_price['price'],encoding="utf8"))
        time.sleep( 0.5 )
 
    connection.close()
    input("enter end")
