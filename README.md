Проект осуществляет чтение файла формата ODF методами Qt и UNO Libreoffice,
формирование списка таблиц, содержащихся в файле, добавление-удаление строк 
и столбцов, редактирование ячеек.

System requirements:
    Libreoffice 6.4 или выше с поддержкой UNO API,
    Astra Linux 2.12 или 
      Debian Linux 9.0 или выше
      Ubuntu Linux 18.0 или выше
      Calculate Linux
   Qt 5.14 или выше
   cmake 3.7 или выше

Compile & install:
    mkdir build
    cd build
    cmake ../;
    make
    make install

На выходе будет библиотека и тестовое приложение

(C) НИИ Рубин
@author
    Ю.Л.Русинов
