cmake_minimum_required(VERSION 3.5)
set(OpenCV_DIR "/home/dlinano/opencv-4.2.0/build")
project(HyperLPR-DEMO LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV 4.2 REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories(include)

set(SRC_DETECTION src/PlateDetection.cpp  src/util.h include/PlateDetection.h)
set(SRC_FINEMAPPING src/FineMapping.cpp )
set(SRC_FASTDESKEW src/FastDeskew.cpp )
set(SRC_SEGMENTATION src/PlateSegmentation.cpp )
set(SRC_RECOGNIZE src/Recognizer.cpp src/CNNRecognizer.cpp)
set(SRC_PIPLINE src/Pipeline.cpp)
set(SRC_SEGMENTATIONFREE src/SegmentationFreeRecognizer.cpp )

add_executable(HyperLPR-DEMO
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
  ${SRC_DETECTION}
  ${SRC_FINEMAPPING}
  ${SRC_FASTDESKEW}
  ${SRC_SEGMENTATION}
  ${SRC_RECOGNIZE}
  ${SRC_PIPLINE}
  ${SRC_SEGMENTATIONFREE}
)

target_link_libraries(HyperLPR-DEMO PRIVATE Qt5::Widgets ${OpenCV_LIBS})
