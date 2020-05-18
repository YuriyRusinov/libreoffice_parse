Проект осуществляет чтение файла формата ODF методами Qt и UNO Libreoffice,
формирование списка таблиц, содержащихся в файле, добавление-удаление строк 
и столбцов, редактирование ячеек.

System requirements:
    Libreoffice 6.4 или выше с поддержкой UNO API,
    Astra Linux 2.12 или 
      Debian Linux 9.0 или выше
      Ubuntu Linux 18.0 или выше
      Calculate Linux 17.15
   Qt 5.11 или выше
   cmake 3.7 или выше

Compile & install из исходных кодов:
    mkdir build
    cd build
    cmake ../;
    make
    root : make install

Формирование прекомпилированного пакета для Debian/Ubuntu:
    mkdir build
    cd build
    cmake ../
    make
    cpack -G DEB

Установка пакета в Debian/Ubuntu
    root : dpkg -i qtuno-1.0.0-Linux.deb

На выходе будет библиотека и тестовое приложение

(C) НИИ Рубин
@author
    Ю.Л.Русинов
