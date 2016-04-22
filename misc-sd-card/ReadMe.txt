古いカーネル(digilent-3.3)と + digilent-3.3 版 dtb
old にはただコンパイルしただけの zImage と dtb

kernel
	xilinx gpio を有効にしてみた。
	kernel をつくるときに gpio を 3.6 からコピーした。
	うえに axi-gpio を認識するようにした。
dtb
	xilinx gpio を有効にしてみた。

gpio/BOOT-gpio.bin
	gpio をちょっと変えたもの

BOOT.bin
	CQ gpio を追加したもの

