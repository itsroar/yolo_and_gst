## YOLOv3 and GStreamer

<details>
<summary>Русский</summary>
<br/>

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

> sudo apt install libgstreamer-1.0 libgstreamer-1.0-dev  
> sudo apt install libfreetype6 libfreetype6-dev

## Сборка

Откройте в терминале директорию, где хотите расположить проект, и выполните следующие команды:

> git clone [https://github.com/itsroar/yolo_and_gst]() --recurse-submodules  
> cd yolo_and_gst  
> mkdir build  
> cd build  
> cmake .. -DCMAKE_BUILD_TYPE=Release  
> make -j{кол-во_потоков}

В случае успешной сборки в папке `build` появится исполняемый файл `yolo_and_gst`.

## После сборки

В корне проекта создайте папку `res` и расположите в ней файлы-ресурсы используемые программой: шрифт для отрисовки текста, датасет для нейронной сети.  
Эти файлы вы можете взять из готовой версии программы, которую можно скачать по [ссылке](https://github.com/itsroar/yolo_and_gst/releases/download/yolov3/yolo_and_gst.tar.gz).  
Возьмите из архива `arial.ttf`, `mobilenetv2_yolov3.bin` и `mobilenetv2_yolov3.param`.

**Программа готова к запуску**

## Запуск

> ./yolo_and_gst &lt;путь_к_mp4-видео&gt;  
> ./yolo_and_gst &lt;путь_к_mp4-видео&gt; &lt;желаемый_fps&gt;

</details>

<details>
<summary>English</summary>
<br/>

`YOLOv3` - model for object detection in image.  
`GStreamer` - multimedia library for working with video and audio.  
`yolo_and_gst` - combination of these elements.  
Input: mp4 video  
Output: playback video with object detection

![screenshot](https://github.com/itsroar/yolo_and_gst/releases/download/yolov3/screenshot.jpg)

**This project does not use OpenCV**

For object detection uses project [ncnn](https://github.com/Tencent/ncnn) project - this is an implementation of the YOLOv3 algorithm.

You can try this program without building. Download the finished program from the [link](https://github.com/itsroar/yolo_and_gst/releases/download/yolov3/yolo_and_gst.tar.gz).  
To run it, you may need to install dependencies (see `Before building`).  
Also you may download [mp4-video](https://github.com/itsroar/yolo_and_gst/releases/download/yolov3/example.mp4))  
[Original](https://www.youtube.com/watch?v=WeQ-aeJd-Kk)

## Before building

Dependencies need to be installed. Run the following commands:

> sudo apt install libgstreamer-1.0 libgstreamer-1.0-dev  
> sudo apt install libfreetype6 libfreetype6-dev

## Building

Open the directory where you want to locate this project. Run the following commands:

> git clone [https://github.com/itsroar/yolo_and_gst]() --recurse-submodules  
> cd yolo_and_gst  
> mkdir build  
> cd build  
> cmake .. -DCMAKE_BUILD_TYPE=Release  
> make -j{thread_count}

If the build is successful, the executable file `yolo_and_gst` will appear in the `build` folder.

## After building

In the root of the project, create `res` folder and place the resource files used by the program in it: font for drawing text, dataset for neural network.  
You can take these files from the finished version of the program, which can be downloaded from [link](https://github.com/itsroar/yolo_and_gst/releases/download/yolov3/yolo_and_gst.tar.gz).  
Take from the archive `arial.ttf`, `mobilenetv2_yolov3.bin` и `mobilenetv2_yolov3.param`.

**The program is ready to run**

## Running

> ./yolo_and_gst &lt;mp4_path&gt;  
> ./yolo_and_gst &lt;mp4_path&gt; &lt;fps&gt;

</details>
