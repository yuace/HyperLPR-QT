#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/Pipeline.h"
#include <QProcess>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStringList strList;

    ui->comboBoxPOWER->clear();
    strList<<"10W"<<"5W";
    ui->comboBoxPOWER->addItems(strList);

    ui->comboBoxFAN->clear();
    strList.clear();
    strList<<"开"<<"关";
    ui->comboBoxFAN->addItems(strList);

    QProcess process;
    process.start("ls /dev/video*");
    process.waitForFinished();
    QByteArray result = process.readAllStandardOutput();
    qDebug() << result;
    ui->comboBoxCAM->clear();
    strList.clear();
    strList<<"VIDEO0";
    ui->comboBoxCAM->addItems(strList);

    ui->comboBoxPX->clear();
    strList.clear();
    strList<<"640x480"<<"1280x720";
    ui->comboBoxPX->addItems(strList);

    ui->comboBoxFPS->clear();
    strList.clear();
    strList<<"20"<<"30";
    ui->comboBoxFPS->addItems(strList);

    ui->lineEditThreshold->setText("0.95");

    ui->endButton->setEnabled(false);

    model->setColumnCount(2);
    model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("车牌号"));
    model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("置信度"));
    ui->tableViewResults->setModel(model);
    ui->tableViewResults->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableViewResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewResults->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewResults->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewResults->setContextMenuPolicy(Qt::CustomContextMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::TEST_CAM()
{
    int frameSize = ui->comboBoxPX->currentIndex();
    int camNum = ui->comboBoxCAM->currentIndex();
    int power = ui->comboBoxPOWER->currentIndex();
    int fan = ui->comboBoxFAN->currentIndex();
    int fps = ui->comboBoxFPS->currentText().toInt();
    float threshold = ui->lineEditThreshold->text().toFloat();


    if(power == 0){
        system("echo 'dlinano' | sudo -S /usr/sbin/nvpmodel -m 0");
    }
    else{
        system("echo 'dlinano' | sudo -S /usr/sbin/nvpmodel -m 1");
    }
    system("echo 'dlinano' | sudo -S /usr/bin/jetson_clocks");

    if(fan == 0){
        system("echo 'dlinano' | sudo -S sh -c ' echo 255 > /sys/devices/pwm-fan/target_pwm'");
    }
    else{
        system("echo 'dlinano' | sudo -S sh -c ' echo 0 > /sys/devices/pwm-fan/target_pwm'");
    }

    cv::VideoCapture capture(camNum);
    cv::Mat frame;
    if(frameSize == 0){
        capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);//宽度
        capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);//高度
    }
    else if(frameSize == 1){
        capture.set(cv::CAP_PROP_FRAME_WIDTH, 1280);//宽度
        capture.set(cv::CAP_PROP_FRAME_HEIGHT, 720);//高度
    }
    capture.set(cv::CAP_PROP_FPS, fps);//帧率 帧/秒
   // capture.set(CV_CAP_PROP_BRIGHTNESS, 1);//亮度 1
   // capture.set(CV_CAP_PROP_CONTRAST,40);//对比度 40
    //capture.set(CV_CAP_PROP_SATURATION, 50);//饱和度 50
    //capture.set(CV_CAP_PROP_HUE, 50);//色调 50
    //capture.set(CV_CAP_PROP_EXPOSURE, 50);//曝光 50

    pr::PipelinePR prc("/home/dlinano/HyperLPR-DEMO/model/cascade.xml",
                       "/home/dlinano/HyperLPR-DEMO/model/HorizonalFinemapping.prototxt","/home/dlinano/HyperLPR-DEMO/model/HorizonalFinemapping.caffemodel",
                       "/home/dlinano/HyperLPR-DEMO/model/Segmentation.prototxt","/home/dlinano/HyperLPR-DEMO/model/Segmentation.caffemodel",
                       "/home/dlinano/HyperLPR-DEMO/model/CharacterRecognization.prototxt","/home/dlinano/HyperLPR-DEMO/model/CharacterRecognization.caffemodel",
                       "/home/dlinano/HyperLPR-DEMO/model/SegmenationFree-Inception.prototxt","/home/dlinano/HyperLPR-DEMO/model/SegmenationFree-Inception.caffemodel"
    );
    int count = 0;
    while(videoFlag) {
        //读取下一帧
        if (!capture.read(frame)) {
            std::cout << "读取视频失败" << std::endl;
            exit(1);
        }
//
//        cv::transpose(frame,frame);
//        cv::flip(frame,frame,2);
//        cv::resize(frame,frame,cv::Size(frame.cols/2,frame.rows/2));

        std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(frame,pr::SEGMENTATION_FREE_METHOD);

        for(auto st:res) {
            if(st.confidence>threshold) {
               // std::cout << st.getPlateName() << " " << st.confidence << std::endl;
                model->setItem(count,0,new QStandardItem(QString::fromStdString(st.getPlateName())));
                model->setItem(count,1,new QStandardItem(QString("%1").arg(st.confidence)));
                ui->tableViewResults->scrollToBottom();
                count++;
                cv::Rect region = st.getPlateRect();

                cv::rectangle(frame,cv::Point(region.x,region.y),cv::Point(region.x+region.width,region.y+region.height),cv::Scalar(255,255,0),2);
            }
        }
        QImage Qtemp = QImage((const unsigned char*)(frame.data), frame.cols, frame.rows, frame.step, QImage::Format_RGB888).rgbSwapped();
        //Qtemp = Qtemp.scaled(ui->label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->labelImage->setPixmap(QPixmap::fromImage(Qtemp));
        ui->labelImage->resize(Qtemp.size());
        ui->labelImage->show();

        //cv::imshow("image",frame);
        cv::waitKey(1);

    }

}

void MainWindow::on_beginButton_clicked()
{
    videoFlag = true;
    ui->comboBoxPOWER->setEnabled(false);
    ui->comboBoxFAN->setEnabled(false);
    ui->comboBoxCAM->setEnabled(false);
    ui->comboBoxPX->setEnabled(false);
    ui->comboBoxFPS->setEnabled(false);
    ui->beginButton->setEnabled(false);
    ui->lineEditThreshold->setEnabled(false);
    ui->endButton->setEnabled(true);
    TEST_CAM();
}

void MainWindow::on_endButton_clicked()
{
    videoFlag = false;
    ui->comboBoxPOWER->setEnabled(true);
    ui->comboBoxFAN->setEnabled(true);
    ui->comboBoxCAM->setEnabled(true);
    ui->comboBoxPX->setEnabled(true);
    ui->comboBoxFPS->setEnabled(true);
    ui->beginButton->setEnabled(true);
    ui->lineEditThreshold->setEnabled(true);
    ui->endButton->setEnabled(false);
}
