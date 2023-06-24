# pingtu
一个基于gtk的拼图小游戏

所用版本
Ubuntu 12.04  gcc 4.6.3  sqlite3 3.8.1  gtk+-2.0 2.24.10

在其文件目录下执行指令 gcc pt.c sqlite3.c -o pt `pkg-config --cflags --libs gtk+-2.0` -lpthread -ldl
然后运行pt即可

![Alt text](/image/test/main.jpg)
![Alt text](/image/test/rank.jpg)
