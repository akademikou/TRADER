import yfinance as yf
import talib as ta
import pandas as pd
import matplotlib.pyplot as plt

import pandas_datareader.data as web
import pandas as pd
import numpy as np
from talib import RSI, BBANDS
import matplotlib.pyplot as plt

start = '2015-04-22'
end = '2017-04-22'

#symbol = 'MCD'
#max_holding = 100
#price = web.DataReader(name=symbol, data_source='quandl', start=start, end=end)
#price = price.iloc[::-1]
#price = price.dropna()
#close = price['AdjClose'].values
#up, mid, low = BBANDS(close, timeperiod=20, nbdevup=2, nbdevdn=2, matype=0)
#rsi = RSI(close, timeperiod=14)
#print("RSI (first 10 elements)\n", rsi[14:24])


power = yf.Ticker("POWERGRID.NS")
df = power.history(start="2020-01-01", end='2020-09-04')
df.head()

df['MA'] = ta.SMA(df['Close'],20)
df[['Close','MA']].plot(figsize=(12,12))
plt.show()

df['EMA'] = ta.EMA(df['Close'], timeperiod = 20)
df[['Close','EMA']].plot(figsize=(12,10))
plt.show()


df['avg'] = ta.ADX(df['High'],df['Low'], df['Close'], timeperiod=20)
df[['avg']].plot(figsize=(12,10))

df['up_band'], df['mid_band'], df['low_band'] = ta.BBANDS(df['Close'], timeperiod =20)
df[['Close','up_band','mid_band','low_band']].plot(figsize=(12,10))
plt.show()

df['Relative'] = ta.RSI(df['Close'],14)
df['Relative'].plot(figsize=(12,10))
plt.show()


