## YOLOv3 and GStreamer

`YOLOv3` - модель для распознавания объектов на изображении.  
`GStreamer` - мультимедийная библиотека для работы с видео и аудио.  
`yolo_and_gst` - связка этих компонентов.  
На вход даёшь mp4-видео, на выходе получаешь его воспроизведение с подсвеченными обнаруженными объектами.  

![screenshot](https://github.com/itsroar/yolo_and_gst/releases/download/yolov3/screenshot.jpg)

**В этом проекте не используется OpenCV**

Для распознавания образов используется проект [ncnn](https://github.com/Tencent/ncnn) - это реализация алгоритма YOLOv3.

Вы можете попробовать эту программу без сборки. Скачайте готовую версию по [ссылке](https://github.com/itsroar/yolo_and_gst/releases/download/yolov3/yolo_and_gst.tar.gz).  
Для её запуска, возможно, потребуется установить зависимости (см. `Перед сборкой`).  
Также можете скачать [mp4-видео](https://github.com/itsroar/yolo_and_gst/releases/download/yolov3/example.mp4))  
[Оригинал](https://www.youtube.com/watch?v=WeQ-aeJd-Kk)

## Перед сборкой

Требуется установить зависимости. Выполните следующие команды:

>
sudo apt install libgstreamer-1.0 libgstreamer-1.0-dev  
sudo apt install libfreetype6 libfreetype6-dev
>

FreeType используется для отрисовки текста.  
В случае, если пакет libfreetype6 не найден, попробуйте прописать `sudo apt install libfreetype` и вызвать автодополнение (клавиша `Tab`). Возможно, в вашем репозитории пакет называется как-то по-другому.

## Сборка

Откройте в терминале директорию, где хотите расположить проект, и выполните следующие команды:

>
git clone [https://github.com/itsroar/yolo_and_gst]() --recurse-submodules  
cd yolo_and_gst  
mkdir build  
cd build  
cmake .. -DCMAKE_BUILD_TYPE=Release  
make -j{кол-во_потоков}  
>

В случае успешной сборки в папке build появится исполняемый файл yolo_and_gst.

## После сборки

В корне проекта создайте папку res и расположите в ней файлы-ресурсы используемые программой: шрифт для отрисовки текста, датасет для нейронной сети.  
Эти файлы вы можете взять из готовой версии программы, которую можно скачать по [ссылке](https://github.com/itsroar/yolo_and_gst/releases/download/yolov3/yolo_and_gst.tar.gz).  
Возьмите из архива arial.ttf, mobilenetv2_yolov3.bin и mobilenetv2_yolov3.param.

Программа готова для запуску

## Запуск
>
./yolo_and_gst <путь_к_mp4-видео>  
./yolo_and_gst <путь_к_mp4-видео> <желаемый_fps>
>
