# Importing libraries 

import os
import configparser
import pandas as pd
import talib as ta
from binance.client import Client
from binance.websockets import BinanceSocketManager
from twisted.internet import reactor 
from time import sleep
#############################################################################################################
def streaming_data_process(msg):
    """
    Function to process the received messages
    param msg: input message
    """
    print(f"message type: {msg['e']}")
    print(f"close price: {msg['c']}")
    print(f"best ask price: {msg['a']}")
    print(f"best bid price: {msg['b']}")
    print("---------------------------")

def btc_trade_history(msg):
	''' define how to process incoming WebSocket messages '''
	if msg['e'] != 'error':
		print(msg['c'])
		btc_price['last'] = msg['c']
		btc_price['bid'] = msg['b']
		btc_price['last'] = msg['a']
	else:
		btc_price['error'] = True
#############################################################################################################
os.system('cls')
config = configparser.ConfigParser()    # Loading keys from config file
config.read_file(open('secret.cfg'))
actual_api_key = config.get('BINANCE', 'ACTUAL_API_KEY')
actual_secret_key = config.get('BINANCE', 'ACTUAL_SECRET_KEY')
test_api_key = config.get('BINANCE', 'TEST_API_KEY')
test_secret_key = config.get('BINANCE', 'TEST_SECRET_KEY')

client = Client(actual_api_key, actual_secret_key)
#client = Client(test_api_key, test_secret_key)
"""
client.API_URL = 'https://testnet.binance.vision/api'  # To change endpoint URL for test account  
info = client.get_account()  # Getting account info
print(info)
"""
#############################################################################################################
#balance = client.get_asset_balance(asset=pair)
#print(balance)
###################################################################################
#############     GETTING HISTORICAL DATA       ###################################
###################################################################################
# valid intervals - 1m, 3m, 5m, 15m, 30m, 1h, 2h, 4h, 6h, 8h, 12h, 1d, 3d, 1w, 1M
interval = '1d'
pair = 'BTCUSDT'
earliest_timestamp = client._get_earliest_valid_timestamp(pair, interval)  
print('EARLISET TIMESTAMP ', earliest_timestamp)
# Getting historical data (candle data or kline)
#candle = client.get_historical_klines(pair, interval, earliest_timestamp, limit=1000)
candle = client.get_historical_klines(pair, Client.KLINE_INTERVAL_1MINUTE, "10 day ago UTC") # fetch 1 minute klines for the last day up until now
#candle = client.get_historical_klines(pair, Client.KLINE_INTERVAL_30MINUTE, "1 Dec, 2017", "1 Jan, 2018") # fetch 30 minute klines for the last month of 2017
#candle = client.get_historical_klines(pair, Client.KLINE_INTERVAL_1WEEK, "1 Jan, 2017") # fetch weekly klines since it listed

print('SON MUM ',candle[1])

pair_df = pd.DataFrame(candle, columns=['dateTime', 'open', 'high', 'low', 'close', 'volume', 'closeTime', 'quoteAssetVolume', 'numberOfTrades', 'takerBuyBaseVol', 'takerBuyQuoteVol', 'ignore'])
pair_df.dateTime = pd.to_datetime(pair_df.dateTime, unit='ms')
pair_df.closeTime = pd.to_datetime(pair_df.closeTime, unit='ms')
pair_df.set_index('dateTime', inplace=True)

yaz_df = pd.DataFrame(candle, columns=['dateTime', 'open', 'high', 'low', 'close', 'volume', 'closeTime', 'quoteAssetVolume', 'numberOfTrades', 'takerBuyBaseVol', 'takerBuyQuoteVol', 'ignore'])
yaz_df.set_index('dateTime', inplace=True)
SUTUN = ['open', 'high', 'low', 'close']
yaz_df.to_csv(pair+'_1.csv',sep="\t", columns = SUTUN)
yaz_df.to_csv(pair+'_1.txt', header=None, index=None, columns = SUTUN,sep='\t', mode='w')
###################################################################################
#############       CALCULATE INDICATORS        ###################################
###################################################################################
DONEM=99
data_high=pair_df.high
data_low=pair_df.low
data_close=pair_df.close
#mean = data_close.tail(20).mean()	# calculate just the last value for the 20 moving average
#max_val = data_close['2020'].max()	# get the highest closing price in 2020
#min, max = ta.MINMAX(data_close, timeperiod=20)
sma = ta.SMA(data_close, timeperiod=DONEM)
ema = ta.EMA(data_close, timeperiod=DONEM)
kma = ta.KAMA(data_close, timeperiod=30)
rsi = ta.RSI(data_close, timeperiod=14)
arn = ta.AROONOSC(data_high, data_low, timeperiod=30)

macd, macdsignal, macdhist       = ta.MACD(data_close, fastperiod=12, slowperiod=26, signalperiod=9)
fastk, fastd = ta.STOCHRSI(data_close, timeperiod=14, fastk_period=3, fastd_period=3, fastd_matype=0)
upperband, middleband, lowerband = ta.BBANDS(data_close, timeperiod=5, nbdevup=2, nbdevdn=2, matype=0)



pair_df['sma'] = sma
pair_df['ema'] = ema
pair_df['kma'] = kma
pair_df['rsi'] = rsi
pair_df['arn'] = arn
pair_df['macd'] = macd
pair_df['macdsignal'] = macdsignal
pair_df['macdhist'] = macdhist
pair_df['fastk'] = fastk
pair_df['fastd'] = fastd
pair_df['upperband'] = upperband
pair_df['middleband'] = middleband
pair_df['lowerband'] = lowerband
###################################################################################
#############          WRITE to FILE            ###################################
###################################################################################
pair_df.drop(index=pair_df.index[0:DONEM], axis=0, inplace=True)
SUTUN = ['open', 'high', 'low', 'close','sma','ema','kma','rsi','arn','macd','macdsignal','macdhist','fastk','fastd','upperband','middleband','lowerband']
pair_df.to_csv(pair+'_candle.csv',sep="\t", columns = SUTUN)
pair_df.to_csv(pair+'_candle.txt', header=None, index=None, columns = SUTUN,sep='\t', mode='w')


print(pair_df.tail(10))
###################################################################################
#############      GETTING REAL TIME DATA       ###################################
###################################################################################
bm = BinanceSocketManager(client)
conn_key = bm.start_symbol_ticker_socket(pair, streaming_data_process)
#conn_key = bm.start_symbol_ticker_socket(pair, btc_trade_history)
bm.start()
sleep(10)
bm.stop_socket(conn_key)
# Websockets utilise a reactor loop from the Twisted library. Using the stop method above 
# will stop the websocket connection but it won’t stop the reactor loop so your code may 
# not exit when you expect. When you need to exit
reactor.stop()
os.system("pause")