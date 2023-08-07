import os
import time
import configparser
import multiprocessing as mp
import pandas as pd
import numpy as np
import talib as ta
from binance.client import Client

from colorama import Fore
from colorama import Style
from colorama import init
#####################################################################################################
def INFO_ASSET(_TRADE,_name,_price_0,_wallet_USDT,_wallet):    
    if _TRADE > 0:
        time.sleep(5)
        client.time_offset = 0
        res = client.get_server_time()
        client.time_offset =  res['serverTime'] - int(time.time() * 1000)
        timestamp = int(time.time() * 1E3 + client.time_offset)
        balance = client.get_asset_balance(asset='USDT',timestamp=timestamp,recvWindow=60000)
        _wallet_USDT = float(balance['free'])
        print(Fore.RED,'\t','USDT','\t',round(_wallet_USDT,3),Style.RESET_ALL)

        for i in range(len(_name)):  
            
            client.time_offset = 0
            res = client.get_server_time()
            client.time_offset =  res['serverTime'] - int(time.time() * 1000)
            timestamp = int(time.time() * 1E3 + client.time_offset)

            balance = client.get_asset_balance(asset=_name[i],timestamp=timestamp,recvWindow=60000)
            _wallet[i] = float(balance['free'])
            print(Fore.GREEN,'\t',_name[i],'\t',round(_wallet[i]*_price_0[i],3),'\t',round(_wallet[i],3),Style.RESET_ALL)

    return _wallet_USDT, _wallet
#####################################################################################################
def TRADE_BOT(_TRADE,_ONC_trade,_RSI_trade, _MAC_trade, _SMA_trade, _TRG_trade,_name, _currency, _price_0, _wallet, _wallet_USDT):
    
    KOSUL_A = _RSI_trade > 0  or  _MAC_trade > 0 or _SMA_trade > 0 or _TRG_trade > 0
    KOSUL_S = _RSI_trade < 0  or  _MAC_trade < 0 or _SMA_trade < 0 or _TRG_trade < 0

    if (KOSUL_A == False and KOSUL_S == False) or (KOSUL_A == True and KOSUL_S == True):
        return _TRADE, _wallet, _wallet_USDT

    _TRADE = 1
    BAS_USDT  = 50.0
    LOW_USDT  = 250.0

    if _SMA_trade != 0: LOW_USDT = 2000 # reserve for others

    BAS_COIN  = BAS_USDT / _price_0
    LOW_COIN  = LOW_USDT / _price_0

    if   _name=='BTC' : R = 4 #            0.0001 BTC
    elif _name=='LINK': R = 2 # 0.1   LINK 0.0001 BTC
    elif _name=='BCH' : R = 3 # 0.001 BCH  0.0001 BTC
    elif _name=='BNB' : R = 3 # 0.01  BNB  0.0001 BTC
    elif _name=='XRP' : R = 1 # 1     XRP
    elif _name=='ETH' : R = 3 # 0.001 ETH  0.0001 BTC
    elif _name=='BTT' : R = 0 # 40000 BTT

    client.time_offset = 0
    res = client.get_server_time()
    client.time_offset =  res['serverTime'] - int(time.time() * 1000)
    timestamp = int(time.time() * 1000 + client.time_offset)
    
    if KOSUL_S: #SATIM
        balance = client.get_asset_balance(asset=_name,timestamp=timestamp,recvWindow=60000)
        _wallet = float(balance['free'])

        MIKTAR_COIN = 0
        if _ONC_trade < 0 : MIKTAR_COIN = MIKTAR_COIN + BAS_COIN * 20        # 1000 $
        if _RSI_trade < 0 : MIKTAR_COIN = MIKTAR_COIN + BAS_COIN             #  250 $
        if _MAC_trade < 0 : MIKTAR_COIN = MIKTAR_COIN + BAS_COIN * 20 * 2    # 2000 $
        if _SMA_trade < 0 : MIKTAR_COIN = MIKTAR_COIN + BAS_COIN * 20 * 8    # 8000 $
        if _TRG_trade < 0 : MIKTAR_COIN = MIKTAR_COIN + BAS_COIN * 20 * 8    # 8000 $
        if (_wallet-MIKTAR_COIN) <= LOW_COIN: MIKTAR_COIN=(_wallet - LOW_COIN)

        MIKTAR_COIN = round(MIKTAR_COIN,R) 
       
        if MIKTAR_COIN >= (BAS_COIN*0.9):
 #           sell_order = client.order_market_sell(symbol=_currency,quantity=MIKTAR_COIN)
            if _ONC_trade < 0 : print('SAT (ONC)',_name, flush=True)
            if _RSI_trade < 0 : print('SAT (RSI)',_name, flush=True)
            if _MAC_trade < 0 : print('SAT (MAC)',_name, flush=True)
            if _SMA_trade < 0 : print('SAT (SMA)',_name, flush=True)
            if _TRG_trade < 0 : print('SAT (TRG)',_name, flush=True)
        else:
            print( 'CUZDAN SATIM ICIN YETERLI DEGIL',_name)
            _TRADE = 0        

    elif KOSUL_A:  # ALIM 
        balance = client.get_asset_balance(asset='USDT',timestamp=timestamp,recvWindow=60000)
        _wallet_USDT = float(balance['free'])  

        MIKTAR_USDT = 0
        if _ONC_trade > 0 : MIKTAR_USDT =MIKTAR_USDT + BAS_USDT * 20         # 1000 $
        if _RSI_trade > 0 : MIKTAR_USDT =MIKTAR_USDT + BAS_USDT              #  250 $
        if _MAC_trade > 0 : MIKTAR_USDT =MIKTAR_USDT + BAS_USDT * 20 * 2     # 2000 $
        if _SMA_trade > 0 : MIKTAR_USDT =MIKTAR_USDT + BAS_USDT * 20 * 8     # 8000 $
        if _TRG_trade > 0 : MIKTAR_USDT =MIKTAR_USDT + BAS_USDT * 20 * 8     # 8000 $
        if (_wallet_USDT-MIKTAR_USDT) <= LOW_USDT: MIKTAR_USDT=(_wallet_USDT - LOW_USDT)

        MIKTAR_COIN = round((MIKTAR_USDT / _price_0),R)
        MIKTAR_USDT = MIKTAR_COIN * _price_0

        if MIKTAR_USDT >= (BAS_USDT*0.9) and MIKTAR_COIN >= (BAS_COIN*0.9):
#            buy_order = client.order_market_buy(symbol=_currency,quantity=MIKTAR_COIN)            
            if _ONC_trade > 0 : print('AL (ONC)',_name, flush=True)
            if _RSI_trade > 0 : print('AL (RSI)',_name, flush=True)
            if _MAC_trade > 0 : print('AL (MAC)',_name, flush=True)
            if _SMA_trade > 0 : print('AL (SMA)',_name, flush=True)
            if _TRG_trade > 0 : print('AL (TRG)',_name, flush=True)
        else:
            print( 'CUZDAN ALIM ICIN YETERLI DEGIL',_name)
            _TRADE = 0

    return _TRADE, _wallet, _wallet_USDT
#####################################################################################################
#####################################################################################################
#####################################################################################################
if __name__ == '__main__':
    os.system('cls')
    init()
    config = configparser.ConfigParser()
    config.read_file(open('secret.cfg'))
    actual_api_key = config.get('BINANCE', 'ACTUAL_API_KEY')
    actual_secret_key = config.get('BINANCE', 'ACTUAL_SECRET_KEY')
    test_api_key = config.get('BINANCE', 'TEST_API_KEY')
    test_secret_key = config.get('BINANCE', 'TEST_SECRET_KEY')

    client = Client(actual_api_key, actual_secret_key)
#    client = Client(test_api_key, test_secret_key)
    #client.API_URL = 'https://testnet.binance.vision/api'  # To change endpoint URL for test account
    #info = client.get_account()
    #print(info)    
    #############################################################################################################
    limit     = 150
    interval  = '1m'
    #name      = ['BTC','LINK','BCH','BNB','XRP','ETH','BTT']
    #currency  = ['BTCUSDT','LINKUSDT','BCHUSDT','BNBUSDT','XRPUSDT','ETHUSDT','BTTUSDT']
    
    name        = ['BTC']
    currency    = ['BTCUSDT']

    ONC_ISLEM   = [0,0,0,0,0,0,0]
    RSI_ISLEM   = [0,0,0,0,0,0,0]
    MAC_ISLEM   = [0,0,0,0,0,0,0]
    SMA_ISLEM   = [0,0,0,0,0,0,0]
    TRG_ISLEM   = [0,0,0,0,0,0,0]

    ONC_trade   = [0,0,0,0,0,0,0]
    RSI_trade   = [0,0,0,0,0,0,0]
    MAC_trade   = [0,0,0,0,0,0,0]
    SMA_trade   = [0,0,0,0,0,0,0]
    TRG_trade   = [0,0,0,0,0,0,0]
    price_0     = [0,0,0,0,0,0,0]
    price_1     = [0,0,0,0,0,0,0]
    price_2     = [0,0,0,0,0,0,0]
    wallet      = [0,0,0,0,0,0,0]
    RSI         = [0,0,0,0,0,0,0]
    SMA_99_L    = [0,0,0,0,0,0,0]
    SMA_99_H    = [0,0,0,0,0,0,0]
    SMA_99_0    = [0,0,0,0,0,0,0]
    CYCLE_TRADE = [0,0,0,0,0,0,0]
    SAYICI      = [0,0,0,0,0,0,0]
    last_HIST   = [0,0,0,0,0,0,0]
    prev_HIST   = [0,0,0,0,0,0,0]
    HIST        = [0,0,0,0,0,0,0]
    LAST_BUY    = [0,0,0,0,0,0,0]
    LAST_SEL    = [0,0,0,0,0,0,0]
    YON         = [0,0,0,0,0,0,0]
    YOK_COIN    = [0,0,0,0,0,0,0]
    YOK_USDT    = [0,0,0,0,0,0,0]

    CURR_TRADE = [' ',' ',' ',' ',' ',' ',' ']
    wallet_USDT=0
    RSI_high=85
    RSI_low =15


    wallet_USDT, wallet =INFO_ASSET(1,name,price_0,wallet_USDT,wallet)

    EMIR=0
    #############################################################################################################
    for i in range(len(name)):
        try:
            klines = client.get_klines(symbol=currency[i], interval=interval, limit=limit)
        except Exception as exp:
            print(exp.status_code, flush=True)
            print(exp.message, flush=True)
        close = [float(entry[4]) for entry in klines]
        price_0[i]  = close[-1]
        LAST_BUY[i] = price_0[i]
        LAST_SEL[i] = price_0[i]
    #############################################################################################################
    while True:
        TRADE = 0        
        #############################################################################################################
        for i in range(len(name)):
            SAYICI[i] = SAYICI[i] + 1
            CYCLE_TRADE[i]=CYCLE_TRADE[i]+1;
            if CYCLE_TRADE[i] >= 0:
                CYCLE_TRADE[i]=0;
            else:
                print(Fore.RED,'bypassed', CURR_TRADE[i], CYCLE_TRADE[i],name[i],Style.RESET_ALL)
                continue
            try:
                klines = client.get_klines(symbol=currency[i], interval=interval, limit=limit)
            except Exception as exp:
                print(exp.status_code, flush=True)
                print(exp.message, flush=True)
            close = [float(entry[4]) for entry in klines]

            old_price_1=price_1[i]
            old_price_2=price_2[i]
            price_0[i] = close[-1]
            price_1[i] = close[-2]
            price_2[i] = close[-3]
            close_array = np.asarray(close)
            close_finished = close_array[:-1] 

            if old_price_1 != price_1[i] or old_price_2 != price_2[i]:
                SMA_trade[i] = 0
                SAYICI[i] = 0
                wallet_USDT, wallet =INFO_ASSET(1,name,price_0,wallet_USDT,wallet)
                ################################################################################
                # 1. MACD and MACD signal have recently intersected. If MACD signal is rising on intersection buy, otherwise sell
                mac, macsignal, machist = ta.MACD(close_finished, fastperiod=12, slowperiod=26, signalperiod=9)
                if len(mac) > 0:
                    #last_mac        = mac[-1]
                    #prev_mac        = mac[-2]
                    #last_mac_signal = macsignal[-1]                
                    #prev_mac_signal = macsignal[-2]
                    #if   last_mac > last_mac_signal and prev_mac < prev_mac_signal and RSI[i] > 50: MAC_trade[i] =  1
                    #elif last_mac < last_mac_signal and prev_mac > prev_mac_signal and RSI[i] < 50: MAC_trade[i] = -1
                    #else: MAC_trade[i] = 0
                    
                    prev_HIST[i] = last_HIST[i]
                    last_HIST[i] = machist[-1]
                    
                    if   last_HIST[i]>prev_HIST[i]: YON[i] =  1
                    elif last_HIST[i]<prev_HIST[i]: YON[i] = -1
                    else: YON[i]=0
                #####################################################################################  
                # 2. current price crosses sma (cross upwards = buy, downwards = sell)     
                #sma_99 = ta.SMA(close_array, timeperiod=99)     
                #if len(sma_99) > 0:
                #    last_sma_99    = sma_99[-1]
                #    if   price_1[i] > last_sma_99 and price_2[i] < last_sma_99: SMA_trade[i] =  1
                #    elif price_1[i] < last_sma_99 and price_2[i] > last_sma_99: SMA_trade[i] = -1
                #    else: SMA_trade[i] = 0
                # 3. sma25 crosses sma99 (cross upwards = buy, downwards = sell)
                #sma_25 = ta.SMA(close_array, timeperiod=25)
                #if len(sma_99) > 0 and len(sma_25)>0:
                #    last_sma_25  = sma_25[-1]
                #    prev_sma_25  = sma_25[-2]
                #    last_sma_99  = sma_99[-1]
                #    prev_sma_99  = sma_99[-2]
                #    if   last_sma_25 > last_sma_99 and prev_sma_25 < prev_sma_99: TRG_trade[i] =  1
                #    elif last_sma_25 < last_sma_99 and prev_sma_25 > prev_sma_99: TRG_trade[i] = -1
                #    else: TRG_trade[i] = 0
                #print('_______________________________________________________________')
            # if old_price_1 != price_1[i] or old_price_2 != price_2[i]: SONU
            #####################################################################################
            # 4. rsi < 30 and increasing (buy) or > 70 and decreasing (sell)
            rsi    = ta.RSI(close_array, timeperiod=14)
            if len(rsi) > 0:
                RSI[i]=round(rsi[-1],1)                
                last_rsi  = rsi[-1]
                prev_rsi  = rsi[-2]
                RSI_TREND = last_rsi - prev_rsi
                if  (last_rsi < RSI_low  and RSI_TREND >  0.3 and last_rsi > RSI_low-5 ):  RSI_trade[i] =  1 # AL
                elif(last_rsi > RSI_high and RSI_TREND < -0.3 and last_rsi < RSI_high+5):  RSI_trade[i] = -1 # SAT
                else:  RSI_trade[i] = 0

                
            #####################################################################################     
            sma_99 = ta.SMA(close_array, timeperiod=99)     
            if len(sma_99) > 0 and len(rsi) > 0:
                SMA_99_0[i]= float(sma_99[-1])
                SMA_99_L[i]= SMA_99_0[i] * 0.9985
                SMA_99_H[i]= SMA_99_0[i] * 1.0015
                if   RSI[i]>51 and price_0[i] > SMA_99_H[i] : SMA_trade[i] =  1 # AL
                elif RSI[i]<49 and price_0[i] < SMA_99_L[i] : SMA_trade[i] = -1 # SAT
                else: SMA_trade[i] =  0 
            SMA_trade[i] =  0
            #####################################################################################
            # son satılan fiyattan % 2 düşük ise ve alım bölgesinde ise AL
            # son alınan fiyattan  % 2 büyük ise ve satım bölgesinde ise SAT
            MARG_U = LAST_BUY[i]*(1+0.03)
            MARG_A = LAST_SEL[i]*(1-0.03)
            if   LAST_SEL[i] != 0 and price_0[i] < MARG_A and RSI[i]<40: MAC_trade[i] =  1 # AL
            elif LAST_BUY[i] != 0 and price_0[i] > MARG_U and RSI[i]>60: MAC_trade[i] = -1 # SAT
            else: MAC_trade[i] = 0
            #####################################################################################
            # ortalamadan % 1.5 düşük ise AL
            # ortalamadan % 1.5 büyük ise SAT
            MARG_U = SMA_99_0[i]*(1+0.015)
            MARG_A = SMA_99_0[i]*(1-0.015)
            if   price_0[i] < MARG_A and RSI[i]<40: ONC_trade[i] =  1 # AL
            elif price_0[i] > MARG_U and RSI[i]>60: ONC_trade[i] = -1 # SAT
            else: ONC_trade[i] = 0
            #####################################################################################
            # TUTARSIZ EMIRLER
            TUTARSIZ = 0
            if ((RSI_trade[i]>0 or MAC_trade[i]>0 or SMA_trade[i]>0 or TRG_trade[i]>0) and ONC_trade[i]<0) or \
               ((ONC_trade[i]>0 or MAC_trade[i]>0 or SMA_trade[i]>0 or TRG_trade[i]>0) and RSI_trade[i]<0) or \
               ((ONC_trade[i]>0 or RSI_trade[i]>0 or SMA_trade[i]>0 or TRG_trade[i]>0) and MAC_trade[i]<0) or \
               ((ONC_trade[i]>0 or RSI_trade[i]>0 or MAC_trade[i]>0 or TRG_trade[i]>0) and SMA_trade[i]<0) or \
               ((ONC_trade[i]>0 or RSI_trade[i]>0 or MAC_trade[i]>0 or SMA_trade[i]>0) and TRG_trade[i]<0): TUTARSIZ=1

            if ((RSI_trade[i]<0 or MAC_trade[i]<0 or SMA_trade[i]<0 or TRG_trade[i]<0) and ONC_trade[i]>0) or \
               ((ONC_trade[i]<0 or MAC_trade[i]<0 or SMA_trade[i]<0 or TRG_trade[i]<0) and RSI_trade[i]>0) or \
               ((ONC_trade[i]<0 or RSI_trade[i]<0 or SMA_trade[i]<0 or TRG_trade[i]<0) and MAC_trade[i]>0) or \
               ((ONC_trade[i]<0 or RSI_trade[i]<0 or MAC_trade[i]<0 or TRG_trade[i]<0) and SMA_trade[i]>0) or \
               ((ONC_trade[i]<0 or RSI_trade[i]<0 or MAC_trade[i]<0 or SMA_trade[i]<0) and TRG_trade[i]>0): TUTARSIZ=1

            if TUTARSIZ == 1:
                ONC_trade[i] = 0
                RSI_trade[i] = 0
                MAC_trade[i] = 0
                SMA_trade[i] = 0
                TRG_trade[i] = 0

            if   ONC_ISLEM[i]>0 and ONC_trade[i] > 0: ONC_trade[i] = 0
            elif ONC_ISLEM[i]<0 and ONC_trade[i] < 0: ONC_trade[i] = 0
            if   RSI_ISLEM[i]>0 and RSI_trade[i] > 0: RSI_trade[i] = 0
            elif RSI_ISLEM[i]<0 and RSI_trade[i] < 0: RSI_trade[i] = 0
            if   MAC_ISLEM[i]>0 and MAC_trade[i] > 0: MAC_trade[i] = 0
            elif MAC_ISLEM[i]<0 and MAC_trade[i] < 0: MAC_trade[i] = 0
            if   SMA_ISLEM[i]>0 and SMA_trade[i] > 0: SMA_trade[i] = 0
            elif SMA_ISLEM[i]<0 and SMA_trade[i] < 0: SMA_trade[i] = 0
            if   TRG_ISLEM[i]>0 and TRG_trade[i] > 0: TRG_trade[i] = 0
            elif TRG_ISLEM[i]<0 and TRG_trade[i] < 0: TRG_trade[i] = 0

            if  YON[i]>0 : # ARTMAYA DEVAM EDIYOR SATACAKSAN BEKLE
                if ONC_trade[i] < 0: ONC_trade[i] = 0 
                if RSI_trade[i] < 0: RSI_trade[i] = 0 
                if MAC_trade[i] < 0: MAC_trade[i] = 0
                if SMA_trade[i] < 0: SMA_trade[i] = 0
                if TRG_trade[i] < 0: TRG_trade[i] = 0

            elif YON[i]<0 : # AZALMAYA DEVAM EDIYOR ALACAKSAN BEKLE
                if ONC_trade[i] > 0: ONC_trade[i] = 0
                if RSI_trade[i] > 0: RSI_trade[i] = 0
                if MAC_trade[i] > 0: MAC_trade[i] = 0
                if SMA_trade[i] > 0: SMA_trade[i] = 0
                if TRG_trade[i] > 0: TRG_trade[i] = 0
            


            if   ONC_trade[i]>0 or RSI_trade[i]>0 or  MAC_trade[i]>0 or SMA_trade[i]>0 or TRG_trade[i]>0: EMIR =  1
            elif ONC_trade[i]<0 or RSI_trade[i]<0 or  MAC_trade[i]<0 or SMA_trade[i]<0 or TRG_trade[i]<0: EMIR = -1
            else: EMIR = 0

            if EMIR ==  1 and wallet_USDT < 300: EMIR = 0
            if EMIR == -1 and (wallet[i]*price_0[i]) < 300: EMIR = 0            

            if EMIR != 0:
  #              TRADE, wallet[i], wallet_USDT = TRADE_BOT(TRADE,ONC_trade[i], RSI_trade[i],MAC_trade[i], SMA_trade[i], TRG_trade[i], \
  #                  name[i],  currency[i],  price_0[i],  wallet[i],  wallet_USDT)

                if TRADE == 1:
                    if   EMIR > 0: LAST_BUY[i] = price_0[i]
                    elif EMIR < 0: LAST_SEL[i] = price_0[i]

                    if ONC_trade[i]!=0: ONC_ISLEM[i] = ONC_trade[i]
                    if RSI_trade[i]!=0: RSI_ISLEM[i] = RSI_trade[i]
                    if MAC_trade[i]!=0: MAC_ISLEM[i] = MAC_trade[i]
                    if SMA_trade[i]!=0: SMA_ISLEM[i] = SMA_trade[i]
                    if TRG_trade[i]!=0: TRG_ISLEM[i] = TRG_trade[i]                

                    if ONC_trade[i] != 0: CYCLE_TRADE[i]=-1 
                    if RSI_trade[i] != 0: CYCLE_TRADE[i]=-1 
                    if MAC_trade[i] != 0: CYCLE_TRADE[i]=-1   
                    if SMA_trade[i] != 0: CYCLE_TRADE[i]=-1
                    if TRG_trade[i] != 0: CYCLE_TRADE[i]=-20

                    if   ONC_trade[i] >0 : CURR_TRADE[i]='ONC AL'
                    elif ONC_trade[i] <0 : CURR_TRADE[i]='ONC SAT'
                    if   RSI_trade[i] >0 : CURR_TRADE[i]='RSI AL'
                    elif RSI_trade[i] <0 : CURR_TRADE[i]='RSI SAT'
                    if   MAC_trade[i] >0 : CURR_TRADE[i]='MAC AL'
                    elif MAC_trade[i] <0 : CURR_TRADE[i]='MAC SAT'  
                    if   SMA_trade[i] >0 : CURR_TRADE[i]='SMA AL'
                    elif SMA_trade[i] <0 : CURR_TRADE[i]='SMA SAT'
                    if   TRG_trade[i] >0 : CURR_TRADE[i]='TRG AL'
                    elif TRG_trade[i] <0 : CURR_TRADE[i]='TRG SAT' 

                    ONC_trade[i] = 0
                    RSI_trade[i] = 0
                    MAC_trade[i] = 0
                    SMA_trade[i] = 0
                    TRG_trade[i] = 0
                # if TRADE == 1: SONU
            # if EMIR != 0: SONU
        # for i in range(len(name)): SONU
        #############################################################################################################
        total_USD = 0
        for i in range(len(name)):
            total_USD = total_USD + wallet[i]*price_0[i] 

        wallet_USDT, wallet =INFO_ASSET(TRADE,name,price_0,wallet_USDT,wallet)
        total_USD = total_USD + wallet_USDT
        total_BTC = total_USD/price_0[0]

        print(SAYICI[i],round(wallet_USDT),Fore.GREEN,'BTC:',round(total_BTC,3),Style.RESET_ALL, Fore.RED,'USDT:',round(total_USD),Style.RESET_ALL,'YON',YON[i],'HIST', round(last_HIST[i],1),
            'AL' ,round(1000*(price_0[i]-LAST_BUY[i])/price_0[i],1),'SAT',round(1000*(price_0[i]-LAST_SEL[i])/price_0[i],1),
           ONC_ISLEM[i],RSI_ISLEM[i],MAC_ISLEM[i],SMA_ISLEM[i],TRG_ISLEM[i])
        if SMA_trade[i] > 0:
            print('(BTC) RSI', RSI[0],'PRICE', Fore.RED  ,price_0[0],Style.RESET_ALL,'SMA (L-H)',round(SMA_99_L[i]),Fore.RED,round(SMA_99_H[i]),Style.RESET_ALL)
        elif SMA_trade[i]< 0:
            print('(BTC) RSI', RSI[0],'PRICE', Fore.GREEN,price_0[0],Style.RESET_ALL,'SMA (L-H)',Fore.GREEN,round(SMA_99_L[i]),Style.RESET_ALL,round(SMA_99_H[i]))
    # while True: SONU