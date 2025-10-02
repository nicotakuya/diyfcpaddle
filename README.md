# DIY FC Paddle
![paddle](https://user-images.githubusercontent.com/5597377/131238282-01c276a1-2150-40f5-a323-94220cffceed.jpg)
## Overview
趣味で作ったファミコン版アルカノイド/アルカノイドIIの「パドルコントローラ」です。
アルカノイドIIは1Pモードのみ対応です(2PモードやVSモードは未対応)。

・回路はファミコンの外部端子に接続します。電源はファミコンから5Vが供給されます。

・マイコンはArduino（ATmega168/328）を使います。

・回転軸の検出には可変抵抗を使っています。アーケード版とは違いロータリエンコーダではないので、軸が無限回転しません。

・カウント値はP2_DAT端子で転送します。P2_CLK端子で同期を取ります。

・ボタンのON/OFFはP1_DAT端子で転送します。P1_CLK端子で同期は取りません。

## Parts
U1 : Atmega168/328

U2 : USBシリアル変換モジュール(FT232RQ)	 

X1 : 16MHz発振子

VR1 : 100K ohm可変抵抗 B

R1 : 10k ohm抵抗	 

R2 : 3.3k ohm抵抗	 

C1 : 100μFコンデンサ	 

C2/C3 : 0.1μFコンデンサ	 

LED1 : LED

CN1 : ファミコン外部端子15pinケーブル　メス型	 
	 
SW1 : タクトスイッチ	 

## movie

https://www.youtube.com/watch?v=KH9H3LFpBtk
