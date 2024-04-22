#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QNetworkProxy>
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QtMath>
#include <QDateTimeAxis>
#include <QtGlobal>
#include <QEventLoop>

#define WAITIME 50 //在这里设置采样时间
#define STATE_LEN 0x64 //STATE数据长度
#define FRAME_HEAD 0xFC //数据帧头
#define FRAME_END 0xFD //数据帧尾
#define TYPE_STATE 0x50 //STATE数据类别
#define STATE_TYPE_LEN 108 //当数据类型为STATE时，该数组的总长度
#define PI 3.1415926

static const uint16_t CRC16Table[256] ={
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD,
    0xE1CE, 0xF1EF,0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,0x9339, 0x8318, 0xB37B, 0xA35A,
    0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,0xA56A, 0xB54B,
    0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695,
    0x46B4,0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,0x48C4, 0x58E5, 0x6886, 0x78A7,
    0x0840, 0x1861, 0x2802, 0x3823,0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,0x5AF5,
    0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58,
    0xBB3B, 0xAB1A,0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13,
    0x2E32, 0x1E51, 0x0E70,0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,0x9188, 0x81A9,
    0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046,
    0x6067,0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235,
    0x5214, 0x6277, 0x7256,0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,0x34E2, 0x24C3,
    0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D,
    0xD73C,0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8,
    0x89E9, 0xB98A, 0xA9AB,0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1,
    0x1AD0, 0x2AB3, 0x3A92,0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,0x7C26, 0x6C07,
    0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9,
    0x9FF8,0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:

    //socket相关处理
    void socketStatuChanged(QAbstractSocket::SocketState socketState);//检测socket状态变化
    void client_error(QAbstractSocket::SocketError error);//socket错误状态提示

    //交互操作相关处理
    void on_connectButton_clicked();
    void on_startButton_clicked();

    //以下是设备通讯相关
    void readHoldRegisters(QTcpSocket *socket);
    void editBaud(QTcpSocket *socket);
    void onSocketReadyRead();
    quint16 calculateCRC16(const QByteArray &data);//计算CRC16-modbus冗余码
    quint16 calculateCRC16_2(const QByteArray &data);//2 GPS设备
    bool verifyCRC16(const QByteArray &data);
    bool verifyCRC16_2 (const QByteArray &data);

    //计时器相关，与采样频率高度相关
    void onTimerTimeout();
    void waitForSignal();
    void onSignalReceived();

    //输出文件相关操作
    void checkedFile();
    void outputData();

    //折线图相关操作
    void setupChar();
    void addValueToChart();
    void checkChart();

    //以下是输出数据到界面的方法
    void showDatas();

    //帮助信息
    void Help_A();
    void Help_B();
    void Help_C();
    void Help_D();
    void Menu_D();
private:

    Ui::MainWindow *ui;

    //TCPsocket相关
    QTcpSocket *Socket1;//传感器1
    QTcpSocket *Socket2;//传感器2
    QTcpSocket *Socket3;//定位设备
    void setupSocket(QTcpSocket* &socket);

    QTimer *timer;//计时器
    QEventLoop loop;

    //图表相关
    QChart* chart;
    // QDateTimeAxis *axisX;
    QValueAxis* axisX;
    QValueAxis* axisY;
    QLineSeries* seriesY1;
    QLineSeries* seriesY2;
    QLineSeries* seriesY3;
    qreal num=0;

    //解包用到的变量
    float HEX_to_Float(QByteArray bytes,bool mode);
    double HEX_to_double(QByteArray bytes,bool mode);

    //存储数据
    struct opData{
        double X1=0,Y1=0,Z1=0;
        double X2=0,Y2=0,Z2=0;
        QString longitude=0,latitude=0;
        double height=0;
        float Roll=0,Pitch=0,Heading=0;
        float Velocity_north=0,Velocity_east=0,Velocity_down=0;
        uint32_t Unix_time=0,Microseconds=0;
        uint8_t GNSS_state=0;
    }outputDatas;

};

//传感器浮点数解析类型
typedef union UF4
{
    unsigned char U1[4];
    float f;
}UF4;

typedef union UF8
{
    unsigned char U1[8];
    double f;
}UF8;


#endif // MAINWINDOW_H