import os
import sys
import math
import csv
import json
import pandas as pd
import talib as ta
import numpy as np
import matplotlib.pyplot as plt


from binance.client import Client
from binance.websockets import BinanceSocketManager
from twisted.internet import reactor
from time import sleep
from datetime import datetime

def btc_trade_history(msg):
	''' define how to process incoming WebSocket messages '''
	if msg['e'] != 'error':
		print(msg['c'])
		btc_price['last'] = msg['c']
		btc_price['bid'] = msg['b']
		btc_price['last'] = msg['a']
	else:
		btc_price['error'] = True

def generateStochasticRSI(close_array, timeperiod):    
    rsi = ta.RSI(close_array, timeperiod=timeperiod)													# 1) ilk aşama rsi değerini hesaplıyoruz.    
    rsi = rsi[~np.isnan(rsi)]																			# 2) ikinci aşamada rsi arrayinden sıfırları kaldırıyoruz.    
    stochrsif, stochrsis = ta.STOCH(rsi, rsi, rsi, fastk_period=14, slowk_period=3, slowd_period=3)		# 3) üçüncü aşamada ise ta-lib stoch metodunu uyguluyoruz.
    return stochrsif, stochrsis


# init
#api_key = os.environ.get('binance_api')
#api_secret = os.environ.get('binance_secret')
api_key = os.environ.get('D4TVZinBoJQ6yYuqAVsFtI5vrlV8hK4onnnoHc2Lq2GATokyBuCLr977YCFV9eHU')
api_secret = os.environ.get('o01XJ6fC5rt5EnPfp0rgrv6yCG79Ys6UA6M3uWZqmXaV4tCyYFZgBzCrmerKFpGU')

client = Client(api_key, api_secret)
btc_price = {'error':False}

pair = 'BTCUSDT'
interval = '1d'
limit = 1000
# valid intervals - 1m, 3m, 5m, 15m, 30m, 1h, 2h, 4h, 6h, 8h, 12h, 1d, 3d, 1w, 1M
## main



timestamp = client._get_earliest_valid_timestamp(pair, interval)		# get timestamp of earliest date data is available
bars = client.get_historical_klines(pair, interval, timestamp, limit=limit)		# request historical candle (or klines) data
print(timestamp)
#print(bars)

# option 1 - save to file using json method - this will retain Python format (list of lists)
with open('btc_bars.json', 'w') as e: 
	json.dump(bars, e)
# option 2 - save as CSV file using the csv writer library
with open('btc_bars.csv', 'w', newline='') as f:
	wr = csv.writer(f)
	for line in bars:
		wr.writerow(line)
# option 3 - save as CSV file without using a library. Shorten to just date, open, high, low, close
with open('btc_bars2.csv', 'w') as d:
	for line in bars:
		d.write(f'{line[0]}, {line[1]}, {line[2]}, {line[3]}, {line[4]}\n')

# delete unwanted data - just keep date, open, high, low, close
for line in bars:
	del line[5:]

# option 4 - create a Pandas DataFrame and export to CSV
btc_df = pd.DataFrame(bars, columns=['date', 'open', 'high', 'low', 'close'])
btc_df.set_index('date', inplace=True)
print(btc_df.head())
# export DataFrame to csv
btc_df.to_csv('btc_bars3.csv')

##################################################################################################
btc_df = pd.read_csv('btc_bars3.csv', index_col=0)# load DataFrame
btc_df.index = pd.to_datetime(btc_df.index, unit='ms')# btc_df.set_index('date', inplace=True)
data=btc_df.close


mean = data.tail(20).mean()	# calculate just the last value for the 20 moving average
max_val = data['2020'].max()	# get the highest closing price in 2020

min, max = ta.MINMAX(data, timeperiod=20)


upperband, middleband, lowerband = ta.BBANDS(data, timeperiod=5, nbdevup=2, nbdevdn=2, matype=0)
macd, macdsignal, macdhist       = ta.MACD(data, fastperiod=12, slowperiod=26, signalperiod=9)
fastk, fastd = ta.STOCHRSI(data, timeperiod=14, fastk_period=3, fastd_period=3, fastd_matype=0)
ma = ta.MA(data, timeperiod=20, matype=0)
sma = ta.SMA(data, timeperiod=20)
ema = ta.EMA(data, timeperiod=20)
rsi =ta.RSI(data, timeperiod=14)

btc_df['ma'] = ma
btc_df['sma'] = sma
btc_df['ema'] = ema
btc_df['rsi'] = rsi

print('---------------------------')
print('---------------------------')
print('---------------------------')
print('---------------------------')
print('---------------------------')
print('---------------------------')
print(btc_df.tail())
print('---------------------------')
print(mean)
print('MAX ',max_val)
print('RSI ',rsi[-1])
print('MACD ',macd[-1])
##################################################################################################
start = datetime(2018, 1, 1)
end = datetime.now();

##################################################################################################
#"""
klines = client.get_klines(symbol=pair, interval=interval, limit=limit)
open_time = [int(entry[0]) for entry in klines]
close = [float(entry[4]) for entry in klines]
close_array = np.asarray(close)
new_time = [datetime.fromtimestamp(time / 1000) for time in open_time]
new_time_x = [date.strftime("%d-%m-%y") for date in new_time]
stochasticRsiF, stochasticRsiS = generateStochasticRSI(close_array, timeperiod=14)

plt.figure(figsize=(11, 6))
plt.plot(new_time_x[114:], stochasticRsiF[100:], label='StochRSI fast')
plt.plot(new_time_x[114:], stochasticRsiS[100:], label='StochRSI slow')
plt.xticks(rotation=90, fontsize=5)
plt.title("Stochastic RSI Plot for BTC/USDT (1d)")
plt.xlabel("Open Time")
plt.ylabel("Value")
plt.legend()
plt.show()
#"""
# init and start the WebSocket
bsm = BinanceSocketManager(client)
conn_key = bsm.start_symbol_ticker_socket(pair, btc_trade_history)
bsm.start()

# put script to sleep to allow WebSocket to run for a while
sleep(20)
bsm.stop_socket(conn_key)
reactor.stop()
# print out all the available WebSocket methods and details
#help(BinanceSocketManager)

