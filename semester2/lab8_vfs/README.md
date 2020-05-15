### Как с этим работать

Загружаем модуль ядра
```
sudo insmod myfs.ko 
dmesg | grep MYFS | tail -4
```
Монтируем нашу ФС
```
touch file
mkdir dir
sudo mount -o loop -t myfs ./file ./dir
dmesg | grep MYFS | tail -1
```
Размонтируем нашу ФС
```
sudo umount ./dir
dmesg | grep MYFS | tail -1
```
Выгружаем модуль ядра
```
sudo rmmod myfs
dmesg | grep MYFS | tail -1
```
