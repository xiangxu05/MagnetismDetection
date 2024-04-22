#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //ui界面状态初始化
    ui->setupUi(this);
    ui->FilePath->setReadOnly(true);
    timer = new QTimer(this);
    setWindowTitle("磁场检测 V1.13");
    setWindowIcon(QIcon(":/new/prefix1/1.ico"));
    ui->preData->setChecked(true);
    ui->checkCurve->setChecked(true);
    ui->checkX1->setChecked(true);
    ui->checkY1->setChecked(true);
    ui->checkZ1->setChecked(true);

    setupChar();
    // QPixmap pix(":/new/prefix1/logo3.png");
    // QSize size = ui->pictureLabel->size();
    // QPixmap scaledPix = pix.scaled(size, Qt::KeepAspectRatio);
    // ui->pictureLabel->setPixmap(scaledPix);

    //网络初始化
    setupSocket(Socket1);
    setupSocket(Socket3);

    connect(timer, &QTimer::timeout, this, &MainWindow::onTimerTimeout);//关联一个计时器
    connect(ui->checkFile,&QCheckBox::toggled,this,&MainWindow::checkedFile);
    connect(ui->checkCurve,&QCheckBox::toggled,this,&MainWindow::checkChart);
    connect(ui->checkX1,&QCheckBox::toggled,this,&MainWindow::checkChart);
    connect(ui->checkY1,&QCheckBox::toggled,this,&MainWindow::checkChart);
    connect(ui->checkZ1,&QCheckBox::toggled,this,&MainWindow::checkChart);
    connect(ui->action_A,&QAction::triggered,this,&MainWindow::Help_A);
    connect(ui->action_B,&QAction::triggered,this,&MainWindow::Help_B);
    connect(ui->action_C,&QAction::triggered,this,&MainWindow::Help_C);
    connect(ui->action_D,&QAction::triggered,this,&MainWindow::Help_D);
    connect(ui->menu_D,&QMenu::aboutToShow,this,&MainWindow::Menu_D);
}

MainWindow::~MainWindow()
{
    Socket1->close();
    Socket2->close();
    Socket3->close();
    delete ui;
}

//以下是网络相关
void MainWindow::setupSocket(QTcpSocket* &socket){
    socket = new QTcpSocket(this);//初始化socket
    socket->setProxy(QNetworkProxy::NoProxy);//不走代理，防止部分设备开了代理无法内网连接。
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);
    connect(socket, &QTcpSocket::stateChanged, this, &MainWindow::socketStatuChanged);
    connect(socket,&QTcpSocket::errorOccurred,this,&MainWindow::client_error);//socket相关状态连接
}

void MainWindow::socketStatuChanged(QAbstractSocket::SocketState socketState){
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    QString hostName = socket->peerName();
    switch(socketState) {
    case QAbstractSocket::UnconnectedState:{
        // 未连接状态下的操作
        QString styleSheet = "background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                             " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(255, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                             " border-radius: 12px;";
        ui->statuLable2->setText(hostName+"未连接");
        if(hostName == ui->IP->text()){
            ui->statuLabel1->setStyleSheet(styleSheet);
            ui->infoLabel->setText("未连接");
        }
        else if(hostName == ui->IP_2->text()){
            ui->statuLabel1_2->setStyleSheet(styleSheet);
            ui->infoLabel_2->setText("未连接");
        }
        else if(hostName == ui->IP_3->text()){
            ui->statuLabel1_3->setStyleSheet(styleSheet);
            ui->infoLabel_3->setText("未连接");
        }
        ui->connectButton->setText("连接");
        ui->startButton->setText("开始监测");
        ui->GPScorlorLabel->setStyleSheet("background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                                          " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(255, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                                          " border-radius: 12px;");
        ui->GPStateLabel->setText("无GPS模块连接或GPS故障");
        timer->stop();
        break;
    }
    case QAbstractSocket::HostLookupState:{
        // 主机查找状态下的操作
        ui->statuLable2->setText("查找主机"+hostName+"……");
        break;
    }
    case QAbstractSocket::ConnectingState:{
        // 连接状态下的操作
        ui->statuLable2->setText("连接"+hostName+"中……");
        QString styleSheet = "background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                             " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(255, 255, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                             " border-radius: 12px;";
        if(hostName == ui->IP->text()){
            ui->statuLabel1->setStyleSheet(styleSheet);
            ui->infoLabel->setText("连接中");
        }
        else if(hostName == ui->IP_2->text()){
            ui->statuLabel1_2->setStyleSheet(styleSheet);
            ui->infoLabel_2->setText("连接中");
        }
        else if(hostName == ui->IP_3->text()){
            ui->statuLabel1_3->setStyleSheet(styleSheet);
            ui->infoLabel_3->setText("连接中");
        }
        break;
    }
    case QAbstractSocket::ConnectedState:{
        // 已连接状态下的操作
        ui->statuLable2->setText("连接"+hostName+"成功！");
        QString styleSheet = "background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                             " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 255, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                             " border-radius: 12px;";

        if(hostName == ui->IP->text()){
            ui->statuLabel1->setStyleSheet(styleSheet);
            ui->infoLabel->setText("已连接");
        }
        else if(hostName == ui->IP_2->text()){
            ui->statuLabel1_2->setStyleSheet(styleSheet);
            ui->infoLabel_2->setText("已连接");
        }
        else if(hostName == ui->IP_3->text()){
            ui->statuLabel1_3->setStyleSheet(styleSheet);
            ui->infoLabel_3->setText("已连接");
        }

        //这里在设备齐全后需要增加判断
        if(ui->infoLabel->text()=="已连接" &&ui->infoLabel_3->text()=="已连接")
            ui->connectButton->setText("取消连接");

        break;
    }
    case QAbstractSocket::BoundState:
        // 已绑定状态下的操作
        qDebug() << "Socket state changed: BoundState";
        break;
    case QAbstractSocket::ClosingState:{
        // 关闭状态下的操作
        QString styleSheet = "background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                             " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(255, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                             " border-radius: 12px;";
        ui->statuLable2->setText(hostName+"未连接");
        if(hostName == ui->IP->text()){
            ui->statuLabel1->setStyleSheet(styleSheet);
            ui->infoLabel->setText("未连接");
        }
        else if(hostName == ui->IP_2->text()){
            ui->statuLabel1_2->setStyleSheet(styleSheet);
            ui->infoLabel_2->setText("未连接");
        }
        else if(hostName == ui->IP_3->text()){
            ui->statuLabel1_3->setStyleSheet(styleSheet);
            ui->infoLabel_3->setText("未连接");
        }
        ui->connectButton->setText("连接");
        break;
    }
    default:
        // 其他状态下的操作
        qDebug() << "Socket state changed: Unknown State";
        break;
    }
}

void MainWindow::client_error(QAbstractSocket::SocketError error){
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    QString hostName = socket->peerName();
    if (socket) {
        switch (error) {
        case QAbstractSocket::ConnectionRefusedError:
            ui->statuLable2->setText("连接"+hostName+"被拒绝");
            break;
        case QAbstractSocket::RemoteHostClosedError:
            ui->statuLable2->setText("远程主机"+hostName+"关闭了连接");
            break;
        case QAbstractSocket::HostNotFoundError:
            ui->statuLable2->setText("找不到主机名:"+hostName);
            break;
        case QAbstractSocket::SocketAccessError:
            ui->statuLable2->setText("试图访问套接字的方式无效:"+hostName);
            break;
        case QAbstractSocket::SocketResourceError:
            ui->statuLable2->setText("本地系统资源不足，无法完成所请求的操作:"+hostName);
            break;
        case QAbstractSocket::SocketTimeoutError:
            ui->statuLable2->setText("操作超时:"+hostName);
            break;
        case QAbstractSocket::DatagramTooLargeError:
            ui->statuLable2->setText("数据报文太大，无法发送:"+hostName);
            break;
        case QAbstractSocket::NetworkError:
            ui->statuLable2->setText("网络错误:"+hostName);
            break;
        case QAbstractSocket::AddressInUseError:
            ui->statuLable2->setText("地址已在使用中:"+hostName);
            break;
        case QAbstractSocket::SocketAddressNotAvailableError:
            ui->statuLable2->setText("套接字地址不可用:"+hostName);
            break;
        case QAbstractSocket::UnsupportedSocketOperationError:
            ui->statuLable2->setText("不支持请求的操作:"+hostName);
            break;
        case QAbstractSocket::UnfinishedSocketOperationError:
            ui->statuLable2->setText("上次操作没有完成:"+hostName);
            break;
        case QAbstractSocket::ProxyAuthenticationRequiredError:
            ui->statuLable2->setText("代理服务器要求身份验证:"+hostName);
            break;
        case QAbstractSocket::SslHandshakeFailedError:
            ui->statuLable2->setText("SSL/TLS 握手失败:"+hostName);
            break;
        case QAbstractSocket::ProxyConnectionRefusedError:
            ui->statuLable2->setText("代理服务器拒绝连接:"+hostName);
            break;
        case QAbstractSocket::ProxyConnectionClosedError:
            ui->statuLable2->setText("代理服务器关闭连接:"+hostName);
            break;
        case QAbstractSocket::ProxyConnectionTimeoutError:
            ui->statuLable2->setText("代理服务器连接超时:"+hostName);
            break;
        case QAbstractSocket::ProxyNotFoundError:
            ui->statuLable2->setText("无法找到代理服务器:"+hostName);
            break;
        case QAbstractSocket::ProxyProtocolError:
            ui->statuLable2->setText("代理服务器返回的响应与预期的不符:"+hostName);
            break;
        case QAbstractSocket::UnknownSocketError:
            ui->statuLable2->setText("未知的套接字错误:"+hostName);
            break;
        default:
            ui->statuLable2->setText("发生未知错误:"+hostName+" "+socket->errorString());
            break;
        }
    }
    // ui->statuLable2->setText("Socket error occurred: "+mSocket->errorString());
}

//以下是交互操作相关
void MainWindow::on_connectButton_clicked()
{
    if(ui->connectButton->text()=="连接"){
        if(Socket1->state() != QAbstractSocket::ConnectedState){
            Socket1->connectToHost(ui->IP->text(),ui->port->text().toInt());
        }
        if(Socket3->state() != QAbstractSocket::ConnectedState){
            Socket3->connectToHost(ui->IP_3->text(),ui->port_3->text().toInt());
        }
    }
    else if(ui->connectButton->text()=="取消连接"){
        if(ui->startButton->text()=="停止监测"){
            ui->startButton->text()=="开始监测";
            on_startButton_clicked();
        }
        Socket1->close();
        Socket3->close();
        seriesY1->clear();
        seriesY2->clear();
        seriesY3->clear();
        num = 0;
        outputDatas.GNSS_state=0;
    }
}

void MainWindow::on_startButton_clicked()
{
    if(ui->connectButton->text()=="取消连接"){
        if(ui->startButton->text()=="开始监测"){
            ui->startButton->setText("停止监测");
            // editBaud(Socket1);//可能没用
            while(outputDatas.X1==0){
                readHoldRegisters(Socket1);
                waitForSignal();
            }
            timer->start(WAITIME);
        }
        else if(ui->startButton->text()=="停止监测"){
            ui->startButton->setText("开始监测");
            timer->stop();
            ui->XLabel1->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->XLabel1->setText("----");
            ui->YLable1->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->YLable1->setText("----");
            ui->ZLable1->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->ZLable1->setText("----");
            ui->XLabel2->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->XLabel2->setText("----");
            ui->YLable2->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->YLable2->setText("----");
            ui->ZLable2->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->ZLable2->setText("----");
            ui->locationLabel->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->locationLabel->setText("----");
            ui->NVelocity->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->NVelocity->setText("----");
            ui->EVelocity->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->EVelocity->setText("----");
            ui->DVelocity->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->DVelocity->setText("----");
            ui->Height->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->Height->setText("----");
            ui->Roll->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->Roll->setText("----");
            ui->Pitch->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->Pitch->setText("----");
            ui->Heading->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
            ui->Heading->setText("----");
            ui->Heading->setStyleSheet("color: black; font-size: 14px; font-weight: bold;text-align: center;");
            ui->GPStateLabel->setText("GPS信息");
            ui->GPScorlorLabel->setStyleSheet("background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                                              " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(255, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                                              " border-radius: 12px;");
        }
    }
    else if(ui->connectButton->text()=="连接"){
        QMessageBox::warning(this,"警告","请先连接设备！");
    }
}

//以下是与设备通讯相关
void MainWindow::readHoldRegisters(QTcpSocket *socket) {
    QByteArray requestData;
    QString hostName = socket->peerName();
    if(hostName != ui->IP_3->text()){
        QDataStream requestStream(&requestData, QIODevice::WriteOnly);
        requestStream.setByteOrder(QDataStream::BigEndian); // Modbus 使用 Big Endian 格式

        requestStream << quint8(0x01); // device address(1)
        requestStream << quint8(0x60); // command(0x60)
        requestStream << quint16(0x0005);//index(2字节)0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08
                                                     //0.1   1    5   10    50  100  500 1000Hz
        requestStream << calculateCRC16(requestData);

        // 发送请求报文
        socket->write(requestData);
        socket->flush();
    }
}

void MainWindow::editBaud(QTcpSocket *socket){
    QByteArray requestData;
    QString hostName = socket->peerName();
    if(hostName != ui->IP_3->text()){
        QDataStream requestStream(&requestData, QIODevice::WriteOnly);
        requestStream.setByteOrder(QDataStream::BigEndian); // Modbus 使用 Big Endian 格式

        requestStream << quint8(0x01); // device address(1)
        requestStream << quint8(0x60); // command(0x60)
        requestStream << quint16(0x1212);//更改波特率为230400
        requestStream << calculateCRC16(requestData);

        // 发送请求报文
        socket->write(requestData);
        socket->flush();
    }
}

void MainWindow::onSocketReadyRead() {

    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    QString hostName = socket->peerName();
    QByteArray responseData = socket->readAll();
    if(hostName != ui->IP_3->text()){
        if(verifyCRC16(responseData)){

            if(hostName == ui->IP->text()){
                if(responseData.size()>=18){
                    if(responseData[0]==0x01 && responseData[1] == 0x60 && responseData[2] == 0x00){//判断来源是否是设备1
                        QByteArray bytesInRange1 = responseData.mid(4, 4);
                        QByteArray bytesInRange2 = responseData.mid(8, 4);
                        QByteArray bytesInRange3 = responseData.mid(12, 4);
                        double X1 = HEX_to_Float(bytesInRange1,true)*1000;
                        double Y1 = HEX_to_Float(bytesInRange2,true)*1000;
                        double Z1 = HEX_to_Float(bytesInRange3,true)*1000;
                        if(X1<=100000 && X1>=-100000 && X1 != 0){
                            outputDatas.X1=X1;
                            outputDatas.Y1=Y1;
                            outputDatas.Z1=Z1;
                        }
                    }
                }
            }
            else if(hostName == ui->IP_2->text()){
                if(responseData.size()>=18){
                    if(responseData[0]==0x01 && responseData[1] == 0x60 && responseData[2] == 0x00){//判断来源是否是设备1
                        QByteArray bytesInRange1 = responseData.mid(4, 4);
                        QByteArray bytesInRange2 = responseData.mid(8, 4);
                        QByteArray bytesInRange3 = responseData.mid(12, 4);
                        double X2 = HEX_to_Float(bytesInRange1,true)*1000;
                        double Y2 = HEX_to_Float(bytesInRange2,true)*1000;
                        double Z2 = HEX_to_Float(bytesInRange3,true)*1000;
                        if(X2<=100000 && X2>=-100000 && X2 != 0){
                            outputDatas.X2=X2;
                            outputDatas.Y2=Y2;
                            outputDatas.Z2=Z2;
                        }
                    }
                }
            }
        }
        else{
            return;
        }

    }
    else{

        if(verifyCRC16_2(responseData)){
            uint8_t First_Receive;
            uint8_t Receive_1;
            uint8_t Receive_2;
            uint8_t Receive_3;
            First_Receive = responseData[0];
            QByteArray FdArray = responseData.mid(0, STATE_TYPE_LEN);
            if(First_Receive == 252){
                for(int i = 0; i < STATE_TYPE_LEN; i++){
                    FdArray[i] = responseData[i];
                }
                Receive_1=FdArray[1];
                Receive_2=FdArray[2];
                Receive_3=FdArray[STATE_TYPE_LEN-1];
                if(Receive_1==TYPE_STATE&&Receive_2==STATE_LEN&&Receive_3==FRAME_END)
                {
                    uint32_t combinedTime=0;
                    uint8_t parttime;

                    //GPS状态
                    uint8_t GNSS=FdArray[9];
                    GNSS = (GNSS & 0xf0)>>4;
                    outputDatas.GNSS_state = GNSS;


                    //时间
                    for(int i=3;i>=0;i--){
                        parttime=FdArray[11+i];
                        combinedTime = combinedTime<<8 |parttime;
                    }
                    outputDatas.Unix_time=combinedTime;
                    for(int i=3;i>=0;i--){
                        parttime=FdArray[15+i];
                        combinedTime = combinedTime<<8 |parttime;
                    }
                    outputDatas.Microseconds=combinedTime;

                    //经纬度
                    QByteArray temp_8 = responseData.mid(19, 8);
                    double result = HEX_to_double(temp_8,true) * 180 / PI;
                    QString outputString;
                    outputString.setNum(result, 'f', 7);
                    outputDatas.latitude = outputString;
                    temp_8 = responseData.mid(27, 8);
                    result = HEX_to_double(temp_8,true)* 180 / PI;
                    outputString.setNum(result, 'f', 7);
                    outputDatas.longitude = outputString;

                    //海拔
                    temp_8 = responseData.mid(35, 8);
                    outputDatas.height = HEX_to_double(temp_8,true);

                    //速度
                    QByteArray temp_4 = responseData.mid(43, 4);
                    outputDatas.Velocity_north = HEX_to_Float(temp_4,true);
                    temp_4 = responseData.mid(47, 4);
                    outputDatas.Velocity_east = HEX_to_Float(temp_4,true);
                    temp_4 = responseData.mid(51, 4);
                    outputDatas.Velocity_down = HEX_to_Float(temp_4,true);
                    //角度
                    temp_4 = responseData.mid(71, 4);
                    outputDatas.Roll = HEX_to_Float(temp_4,true);
                    temp_4 = responseData.mid(75, 4);
                    outputDatas.Pitch = HEX_to_Float(temp_4,true);
                    temp_4 = responseData.mid(79, 4);
                    outputDatas.Heading = HEX_to_Float(temp_4,true);
                }
            }
        }
        else{

            return;
        }
    }
}

quint16 MainWindow::calculateCRC16(const QByteArray &data){

    quint16 crc = 0xFFFF;

    for (int i = 0; i < data.size(); ++i) {
        crc ^= static_cast<quint8>(data.at(i));

        for (int j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;

}

bool MainWindow::verifyCRC16(const QByteArray &data){

    if (data.size() < 2) {
        return false; // 数据长度不足
    }
    quint8 receivedCRC_1 = data[data.size() - 1];
    quint8 receivedCRC_2 = data[data.size() - 2];
    quint16 receivedCRC = receivedCRC_1 | receivedCRC_2<<8;
    QByteArray dataWithoutCRC = data.mid(0,data.size()-2);
    quint16 calculatedCRC = calculateCRC16(dataWithoutCRC);
    // qDebug()<<QString("%1").arg(calculatedCRC, 0, 16).toUpper()<<" "<< QString("%1").arg(receivedCRC, 0, 16).toUpper();

    return (receivedCRC == calculatedCRC);
}

quint16 MainWindow::calculateCRC16_2(const QByteArray &data){

    quint16 crc16 = 0;
    for (int i = 0; i < data.size(); ++i) {
        quint8 value = data[i];
        crc16 = CRC16Table[((crc16 >> 8) ^ value) & 0xff]^ (crc16 << 8);
    }
    return crc16;
}

bool MainWindow::verifyCRC16_2(const QByteArray &data){

    if (data.size() < 2) {
        return false; // 数据长度不足
    }
    quint8 receivedCRC_1 = data[5];
    quint8 receivedCRC_2 = data[6];
    quint16 receivedCRC = receivedCRC_1<<8 | receivedCRC_2;
    QByteArray dataWithoutCRC = data.mid(7,data.size()-8);
    quint16 calculatedCRC = calculateCRC16_2(dataWithoutCRC);

    // qDebug()<<QString("%1").arg(calculatedCRC, 0, 16).toUpper()<<" "<< QString("%1").arg(receivedCRC, 0, 16).toUpper();
    return (receivedCRC == calculatedCRC);
}

float MainWindow::HEX_to_Float(QByteArray bytes,bool mode){
    unsigned char i;
    UF4 yy;
    UF8 xx;
    double d=0;
    if(bytes.size()==4){
        if(mode==true){
            for(i=0;i<4;i++)
                yy.U1[i]=bytes[i];
            d=yy.f;
        }
        else{
            unsigned char j=3;
            for(i=0;i<4;i++){
                yy.U1[j]=bytes[i];
                j--;
            }
            d=yy.f;
        }
    }
    return d;
}

double MainWindow::HEX_to_double(QByteArray bytes,bool mode){
    unsigned char i;
    UF8 xx;
    double d=0;
    if(bytes.size()==8){
        if(mode==true){
            for(i=0;i<8;i++)
                xx.U1[i]=bytes[i];
            d=xx.f;
        }
        else{
            unsigned char j=7;
            for(i=0;i<8;i++){
                xx.U1[j]=bytes[i];
                j--;
            }
            d=xx.f;
        }
    }
    return d;
}

//以下是计时器相关
void MainWindow::onTimerTimeout(){
    // 每次定时器超时时调用readHoldRegisters方法
    readHoldRegisters(Socket1);
    // readHoldRegisters(Socket3);//可能不需要
    showDatas();
    outputData();
    addValueToChart();
}

void MainWindow::waitForSignal(){
    connect(Socket1, &QTcpSocket::readyRead, this, &MainWindow::onSignalReceived);
    loop.exec();
}

void MainWindow::onSignalReceived(){
    disconnect(Socket1, &QTcpSocket::readyRead, this, &MainWindow::onSignalReceived);
    loop.quit();
}

//输出文件相关操作
void MainWindow::checkedFile(){
    if(ui->checkFile->isChecked()){
        QString defaultFileFormat = "csv";
        QString defaultFileName = "output";
        QString defaultFileFullName = defaultFileName + "." + defaultFileFormat;

        QString fileName = QFileDialog::getSaveFileName(
            nullptr,
            "选择一个文件",
            defaultFileFullName,
            "CSV Files (*.csv);;All Files (*)"
            );

        if(fileName.isEmpty())
        {
            ui->checkFile->setChecked(false);
            QMessageBox::warning(this,"警告","请选择一个文件");
        }
        else
        {
            ui->FilePath->setText(fileName);
        }
    }
    else{
        ui->FilePath->setText("");
    }
}

void MainWindow::outputData(){

    QString fileName = ui->FilePath->text();

    if(fileName.isEmpty())
    {
        return;
    }
    else{
        QFile file(fileName);
        if (!file.open(QIODevice::Append | QIODevice::Text)) {
            QMessageBox::warning(this,"警告","无法保存为该文件，请检查！");
        }
        QTextStream out(&file);
        out<<outputDatas.Unix_time<<","<<outputDatas.Microseconds<<",";
        out<<outputDatas.X1<<","<<outputDatas.Y1<<","<<outputDatas.Z1<<","<<outputDatas.X2<<","<<outputDatas.Y2<<","<<outputDatas.Z2<<",";
        out<<outputDatas.longitude<<","<<outputDatas.latitude<<","<<outputDatas.height<<","<<outputDatas.Roll<<","<<outputDatas.Pitch<<",";
        out<<outputDatas.Heading<<","<<outputDatas.Velocity_north<<","<<outputDatas.Velocity_east<<","<<outputDatas.Velocity_down;
        out<<"\n";

        file.close();
        return;
    }

}

//以下是折线图相关
void MainWindow::setupChar(){

    //初始化图表框架
    chart = new QChart();
    chart->setTitle("实时磁感应强度曲线");
    ui->curve->setChart(chart);

    //初始化折线序列
    seriesY1 = new QLineSeries;
    seriesY2 = new QLineSeries;
    seriesY3 = new QLineSeries;
    seriesY1->setName("X轴");
    seriesY2->setName("Y轴");
    seriesY3->setName("Z轴");
    chart->addSeries(seriesY1);
    chart->addSeries(seriesY2);
    chart->addSeries(seriesY3);

    axisX = new QValueAxis;
    // axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    // axisX->setFormat("hh:mm:ss");

    chart->setAxisX(axisX,seriesY1);
    chart->setAxisX(axisX,seriesY2);
    chart->setAxisX(axisX,seriesY3);

    axisX->setRange(0,num+10);
    // axisX->setMin(QDateTime::currentDateTime().addSecs(-60));  // 60 秒前
    // axisX->setMax(QDateTime::currentDateTime());

    axisY = new QValueAxis;
    axisY->setRange(-60000,60000);
    chart->setAxisY(axisY,seriesY1);
    chart->setAxisY(axisY,seriesY2);
    chart->setAxisY(axisY,seriesY3);

}

void MainWindow::addValueToChart(){
    if(ui->checkCurve->isChecked() && (ui->checkX1->isChecked()||ui->checkY1->isChecked()||ui->checkZ1->isChecked())){
        if(ui->checkX1->isChecked())
            seriesY1->append(num, outputDatas.X1);
        if(ui->checkY1->isChecked())
            seriesY2->append(num, outputDatas.Y1);
        if(ui->checkZ1->isChecked())
            seriesY3->append(num, outputDatas.Z1);
        axisX->setMin(num-((30000/WAITIME)));
        axisX->setMax(num);
        num++;
        qreal maxValue = std::numeric_limits<qreal>::lowest();
        qreal minValue = std::numeric_limits<qreal>::max();

        // 遍历系列中的数据点，找到最大值
        foreach (const QPointF &point, seriesY1->points()) {
            minValue = std::min(minValue, point.y());
            maxValue = std::max(maxValue, point.y());
            // maxValue = std::max(maxValue, std::max(point.x(), point.y()));
        }
        foreach (const QPointF &point, seriesY2->points()) {
            minValue = std::min(minValue, point.y());
            maxValue = std::max(maxValue, point.y());
        }
        foreach (const QPointF &point, seriesY3->points()) {
            minValue = std::min(minValue, point.y());
            maxValue = std::max(maxValue, point.y());
        }
        int intMax = static_cast<int>(maxValue) + 10000;
        int intMin = static_cast<int>(minValue) - 10000;
        axisY->setMin(intMin);
        axisY->setMax(intMax);
    }
    else{
        num=0;
        return;
    }
}

void MainWindow::checkChart(){
    if(!ui->checkCurve->isChecked()){
        seriesY1->clear();
        seriesY2->clear();
        seriesY3->clear();
    }
    if(!ui->checkX1->isChecked()){
        seriesY1->clear();
    }
    if(!ui->checkY1->isChecked()){
        seriesY2->clear();
    }
    if(!ui->checkZ1->isChecked()){
        seriesY3->clear();
    }
}

//以下是输出数据到界面的方法
void MainWindow::showDatas(){
    QString styleSheet = "color: black; font-size: 16px; font-weight: bold;text-align: center;";
    //设备1
    if(ui->checkX1->isChecked()){
        ui->XLabel1->setStyleSheet(styleSheet);
        ui->XLabel1->setText(QString::number(outputDatas.X1));
    }
    else{
        ui->XLabel1->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
        ui->XLabel1->setText("----");
    }
    if(ui->checkY1->isChecked()){
        ui->YLable1->setStyleSheet(styleSheet);
        ui->YLable1->setText(QString::number(outputDatas.Y1));
    }
    else{
        ui->YLable1->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
        ui->YLable1->setText("----");
    }
    if(ui->checkZ1->isChecked()){
        ui->ZLable1->setStyleSheet(styleSheet);
        ui->ZLable1->setText(QString::number(outputDatas.Y1));
    }
    else{
        ui->ZLable1->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
        ui->ZLable1->setText("----");
    }

    //设备2
    if(ui->checkX2->isChecked()){
        ui->XLabel2->setStyleSheet(styleSheet);
        ui->XLabel2->setText(QString::number(outputDatas.X1));
    }
    else{
        ui->XLabel2->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
        ui->XLabel2->setText("----");
    }
    if(ui->checkY2->isChecked()){
        ui->YLable2->setStyleSheet(styleSheet);
        ui->YLable2->setText(QString::number(outputDatas.Y1));
    }
    else{
        ui->YLable2->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
        ui->YLable2->setText("----");
    }
    if(ui->checkZ2->isChecked()){
        ui->ZLable2->setStyleSheet(styleSheet);
        ui->ZLable2->setText(QString::number(outputDatas.Y1));
    }
    else{
        ui->ZLable2->setStyleSheet("color: black; font-size: 36px; font-weight: bold;text-align: center;");
        ui->ZLable2->setText("----");
    }

    //定位传感器
    if(ui->infoLabel_3->text()=="已连接"){
        ui->locationLabel->setStyleSheet(styleSheet);
        ui->locationLabel->setText("Lon: "+outputDatas.longitude+"\n"+"Lat: "+outputDatas.latitude);
        ui->locationLabel->setAlignment(Qt::AlignCenter);
        ui->NVelocity->setStyleSheet(styleSheet);
        ui->NVelocity->setText(QString::number(outputDatas.Velocity_north,'f', 3));
        // ui->NVelocity->setAlignment(Qt::AlignCenter);
        ui->EVelocity->setStyleSheet(styleSheet);
        ui->EVelocity->setText(QString::number(outputDatas.Velocity_east,'f', 3));
        // ui->EVelocity->setAlignment(Qt::AlignCenter);
        ui->DVelocity->setStyleSheet(styleSheet);
        ui->DVelocity->setText(QString::number(outputDatas.Velocity_down,'f', 3));
        // ui->DVelocity->setAlignment(Qt::AlignCenter);
        ui->Height->setStyleSheet(styleSheet);
        ui->Height->setText(QString::number(outputDatas.height));
        // ui->Height->setAlignment(Qt::AlignCenter);
        ui->Roll->setStyleSheet(styleSheet);
        ui->Roll->setText(QString::number((outputDatas.Roll/PI)*180,'f', 3));
        // ui->Roll->setAlignment(Qt::AlignCenter);
        ui->Pitch->setStyleSheet(styleSheet);
        ui->Pitch->setText(QString::number((outputDatas.Pitch/PI)*180,'f', 3));
        // ui->Pitch->setAlignment(Qt::AlignCenter);
        ui->Heading->setStyleSheet(styleSheet);
        ui->Heading->setText(QString::number((outputDatas.Heading/PI)*180,'f', 3));
        // ui->Heading->setAlignment(Qt::AlignCenter);
    }
    QDateTime dateTime = QDateTime::fromSecsSinceEpoch(outputDatas.Unix_time);
    int year = dateTime.date().year();
    int month = dateTime.date().month();
    int day = dateTime.date().day();
    int hour = dateTime.time().hour();
    int minute = dateTime.time().minute();
    int second = dateTime.time().second();
    int msec = outputDatas.Microseconds/1000;
    QString Timel = QString("%1年 %2月 %3日 %4时 %5分 %6秒").arg(year).arg(month).arg(day).arg(hour).arg(minute).arg(second);
    ui->TimeLabel->setStyleSheet("color: black; font-size: 16px; font-weight: bold;text-align: center;");
    ui->TimeLabel->setAlignment(Qt::AlignCenter);
    ui->TimeLabel->setText(Timel);

    // int GPSstate = outputDatas.GNSS_state;
    ui->GPStateLabel->setStyleSheet(styleSheet);
    QString styleColor = "background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                         " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 255, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                         " border-radius: 12px;";

    switch(outputDatas.GNSS_state){
    case 0 :{
        ui->GPScorlorLabel->setStyleSheet("background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                                          " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(255, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                                          " border-radius: 12px;");
        ui->GPStateLabel->setText("无GPS模块连接或GPS故障");
        break;
    }
    case 1:{
        ui->GPScorlorLabel->setStyleSheet("background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                                          " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(255, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                                          " border-radius: 12px;");
        ui->GPStateLabel->setText("GPS没有信号");
        break;
    }
    case 2:{
        ui->GPScorlorLabel->setStyleSheet("background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                                          " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(255, 255, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                                          " border-radius: 12px;");
        ui->GPStateLabel->setText("2D定位");
        break;
    }
    case 3:{
        ui->GPScorlorLabel->setStyleSheet("background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                                          " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(255, 255, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                                          " border-radius: 12px;");
        ui->GPStateLabel->setText("3D定位");
        break;
    }
    case 4:{
        ui->GPScorlorLabel->setStyleSheet(styleColor);
        ui->GPStateLabel->setText("DGPS/SBAS 辅助");
        break;
    }
    case 5:{
        ui->GPScorlorLabel->setStyleSheet(styleColor);
        ui->GPStateLabel->setText("RTK浮点解模式");
        break;
    }
    case 6:{
        ui->GPScorlorLabel->setStyleSheet(styleColor);
        ui->GPStateLabel->setText("RTK固定解模式");
        break;
    }
    case 7:{
        ui->GPScorlorLabel->setStyleSheet(styleColor);
        ui->GPStateLabel->setText("静态定点模式，通常用于基站");
        break;
    }
    case 8:{
        ui->GPScorlorLabel->setStyleSheet(styleColor);
        ui->GPStateLabel->setText("精密单点定位");
        break;
    }
    case 9:{
        ui->GPScorlorLabel->setStyleSheet(styleColor);
        ui->GPStateLabel->setText("双天线均为RTK固定解模式");
        break;
    }
    default:
        break;
    }

}

//以下是帮助信息
void MainWindow::Help_A(){
    // 创建消息框
    QMessageBox helpBox;
    // 设置消息框的图标为帮助图标
    helpBox.setIcon(QMessageBox::Information);
    // 设置消息框的标题
    helpBox.setWindowTitle("操作流程");
    // 设置消息框的文本内容
    helpBox.setText("连接设备后，点击开始推断开始采集数据。");
    // 添加详细信息
    helpBox.setDetailedText("①  首先连接设备WIFI：Magnetism_main；\n"
                            "②  填写设备IP地址（主设备、从设备）；\n"
                            "③  点击连接按钮；\n"
                            "④  勾选保存数据到文件复选框，选择保存路径；\n"
                            "⑤  连接成功后，点击开始推断；\n"
                            "备注：默认IP地址已填入，若IP地址改动，"
                            "可以分别接入Magenetism_main与slave两个WIFI查看，"
                            "其中slave设备由于连接入main设备主网，应该登陆其网关查看连接后的对应地址。");
    // 添加一个“确定”按钮
    helpBox.addButton(QMessageBox::Ok);
    // 显示消息框
    helpBox.exec();
}
void MainWindow::Help_B(){
    QMessageBox helpBox;
    helpBox.setIcon(QMessageBox::Information);
    helpBox.setWindowTitle("设备组网");
    helpBox.setText("应将天线暴露与外部的设备设置为主设备，同时建议保留所有设备的AP功能方便设置。");
    helpBox.setDetailedText("①  连接主设备WIFI，访问局域网网关，设置设备为AP模式，建议打开DHCP，防止上位机难以连接入网络，建议更改主设备WIFI名称，方便后续连接区分；\n"
                            "②  连接从设备WIFI，访问局域网网关，设置设备为APSTA模式，其中STA模式中关闭DHCP，自行分配与主设备同一网段IP，以便于后续访问，建议更改从设备WIFI名称，方便后续连接区分；\n"
                            "③  从设备设置连接主设备WIFI，备注：主从设备保存参数后请重启；\n"
                            "④  重新连接主设备WIFI，根据设置IP填入软件中；\n"
                            "⑤  点击连接。\n");
    helpBox.addButton(QMessageBox::Ok);
    helpBox.exec();
}
void MainWindow::Help_C(){
    QMessageBox helpBox;
    helpBox.setIcon(QMessageBox::Information);
    helpBox.setWindowTitle("RTK配置");
    helpBox.setText("请拆开设备外壳，使用备用j30j9p线与GPS设备连接，并进行供电。\n备注：需提前安装CP210X驱动。");
    helpBox.setDetailedText("①  使用上位机软件FDIGroudbase进行配置，首先在菜单栏中选择添加串口，波特率为921600。\n"
                            "②  在config界面中，找到DGNSS选项\n"
                            "③  选择配置参数：Ntrip，填入域名/ip、端口号、挂载点、账号与密码\n"
                            "④  点击send 按钮进行保存，再点击reflash按钮检验是否保存成功；"
                            "接着点击config 界面左下角的transmit ->write flash按钮保存到flash，最后点击restart按钮重启，账号仅需配置一次即可。\n");
    helpBox.addButton(QMessageBox::Ok);
    helpBox.exec();
}
void MainWindow::Help_D(){
    QMessageBox helpBox;
    helpBox.setIcon(QMessageBox::Information);
    helpBox.setWindowTitle("数据格式");
    helpBox.setText("数据以一行为单位输入设置的CSV文件，数据间以\'，\'分隔");
    helpBox.setDetailedText("UTC秒、UTC微秒、"
                            "X1、Y1、Z1、X2、Y2、Z2、"
                            "经度、纬度、海拔、"
                            "横滚角、俯仰角、偏航角、"
                            "北向速度、东向速度、地向速度");
    helpBox.addButton(QMessageBox::Ok);
    helpBox.exec();
}
void MainWindow::Menu_D(){
    QMessageBox helpBox;
    helpBox.setIcon(QMessageBox::Information);
    helpBox.setWindowTitle("更新日志");
    helpBox.setText("---------------- V1.1 ----------------\n"
                    "2024年3月26日 -------- 对GPS精度进行了修正\n"
                    "2024年3月26日 -------- 增加了GPS设备定位状态\n"
                    "---------------- V1.11 ----------------\n"
                    "2024年3月26日 -------- 删除了时间显示的毫秒部分\n"
                    "2024年3月26日 -------- 解决了开始监测时首次读取数据为0的异常状态\n"
                    "2024年3月26日 -------- 实验证明外壳影响信号，AP性能与信号强度也不足 下一阶段需要解决设备传输信号与丢包问题\n"
                    "---------------- V1.12 ----------------\n"
                    "2024年4月1日   -------- 增加了帮助菜单\n"
                    "2024年4月1日   -------- 缩小了界面\n"
                    "2024年4月1日   -------- 修复了取消勾选传感器三个轴时，曲线仍存在的问题\n"
                    "2024年4月1日   -------- 修复了折线纵轴取值不准确，与X轴值相关的问题\n"
                    "---------------- V1.13 ----------------\n"
                    "2024年4月22日   -------- 适配购置的pad设备。");
    helpBox.addButton(QMessageBox::Ok);
    helpBox.exec();
}
