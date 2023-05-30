#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <time.h>
#include <windows.h>
#include <iostream>
#include <sstream>
#include <QTextCodec>
#include <string.h>
#include <typeinfo>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    uiInitial();
    serial = new QSerialPort(this);
    timer = new QTimer(this);

    //confIdValue CurrentConfigureValue;
    connect(serial, SIGNAL(readyRead()), this, SLOT(receiveInfo()));
    //connect(timer, &QTimer::timeout, this, &MainWindow::onTimerTimeout);
    m_portNameList = Scan_serial();
    ui->comboBoxPortName->addItems(m_portNameList);

    //test
    m_pGraphicsOpacityEffect = new QGraphicsOpacityEffect(ui->label_14);
    m_pGraphicsOpacityEffect->setOpacity(1);
    ui->label_14->setGraphicsEffect(m_pGraphicsOpacityEffect);
    m_pNameAnimation = new QPropertyAnimation(m_pGraphicsOpacityEffect,"opacity",this);
    m_pNameAnimation->setEasingCurve(QEasingCurve::Linear);
    m_pNameAnimation->setDuration(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_pGraphicsOpacityEffect;
    delete m_pNameAnimation;
}

/* 遍历可用串口设备 */
/*
     * @brief Scan all the avaliable serial devices
     *
     * @param[in] void
     *
     * @return Ports in QStringList
     */
QStringList MainWindow::Scan_serial()
{
    QStringList m_serialPortName;
        foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
        {
            m_serialPortName << info.portName();
            qDebug()<<"serialPortName:"<<info.portName();
        }
        return m_serialPortName;
}

/*
     * @brief Display the Code received and interperate the
     *  Mode & configure & Value of it
     *
     * @param[in] void
     *
     * @return void
*/
void MainWindow::DisplayReceiveddata()
{
    //QString testcode = "88FF022A00" ;
    //DatareceivedStringList = Dataprocess(testcode);
    int FirstElmentint = convertStringHextoInt(DatareceivedStringList[1]);
    int SecondElmentint = convertStringHextoInt(DatareceivedStringList[2]);
    int ThirdElmentint = convertStringHextoInt(DatareceivedStringList[3]);
    int FourthElmentint = convertStringHextoInt(DatareceivedStringList[4]);
    //qDebug()<<"测试转Qstring转int效果: "<<firstElmentint;

    //trigerMode
    if(FirstElmentint == TRIGER_MODE_STRAIN){
        ui->lineEdit_Mode->setText("TRIGER_MODE_STRAIN");
        ui->lineEdit_Code->setText("Pressed");
        //88000000
    }
    else{
        ui->lineEdit_Mode->setText("TRIGER_MODE_KNOB");
        on_dial_Monitor_valueChanged(ThirdElmentint*3.6);
    }

    //confId
    if(SecondElmentint == CONFID_TEMP){
        ui->lineEdit_ConfID->setText("CONFID_TEMP");
        CurrentConfigureValue.CONFID_VALUE_TEMP = ThirdElmentint;
        ui->lineEdit_confIdValue->setText(QString::number(ThirdElmentint));

    }
    else if(SecondElmentint == CONFID_MODE) {
        ui->lineEdit_ConfID->setText("CONFID_MODE");
        CurrentConfigureValue.CONFID_VALUE_MODE = ThirdElmentint;
        ui->lineEdit_confIdValue->setText(QString::number(ThirdElmentint));

    }
    else if(SecondElmentint == CONFID_RATE) {
        ui->lineEdit_ConfID->setText("CONFID_RATE");
        CurrentConfigureValue.CONFID_VALUE_RATE = ThirdElmentint;
        ui->lineEdit_confIdValue->setText(QString::number(ThirdElmentint));
        ui->lineEdit_ConfID_right->setText(QString::number(FourthElmentint));

    }
    else if(SecondElmentint == CONFID_POWER) {
        ui->lineEdit_ConfID->setText("CONFID_POWER");
        CurrentConfigureValue.CONFID_VALUE_POWER = ThirdElmentint;
        ui->lineEdit_confIdValue->setText(QString::number(ThirdElmentint));

    }
    else{
        ui->lineEdit_ConfID->setText("CONFID_UNKNOW");
        CurrentConfigureValue.CONFID_VALUE_UNKNOW = ThirdElmentint;
        ui->lineEdit_confIdValue->setText(QString::number(ThirdElmentint));

    }

// reflect the data to the dial monitor

    int Current_Value = ui->lineEdit_confIdValue->text().toInt();
    QString Current_Mode = ui->lineEdit_ConfID->text();
    //display
    if(Current_Mode == "CONFID_TEMP"){
        ui->dial_Monitor->setValue(Current_Value*5);
        ui->lineEdit_ConfID_right->hide();
        ui->label_2->hide();
        ui->label_13->hide();

    }
    else if(Current_Mode == "CONFID_MODE"){
        ui->dial_Monitor->setValue(Current_Value*20);
        ui->lineEdit_ConfID_right->hide();
        ui->label_2->hide();
        ui->label_13->hide();
    }
    else if(Current_Mode == "CONFID_RATE"){
        ui->lineEdit_ConfID_right->show();
        ui->label_2->show();
        ui->label_13->show();
        ui->dial_Monitor->setValue(Current_Value);
    }
    else if(Current_Mode == "CONFID_POWER"){
        ui->lineEdit_ConfID_right->hide();
        ui->label_2->hide();
        ui->label_13->hide();
        ui->dial_Monitor->setValue(Current_Value*8);
    }
}

/*
     * @brief Filter the data received and get the code starts with header "88"
     *
     * @param[in] QString infro
     *
     * @return void
*/
void MainWindow::filterdata(QString info)
{
    //定义缓存区
    QByteArray buffer;
    //读取串口数据
    //QByteArray data = serial->readAll();
    //将数据添加到缓存区
    buffer.append(info);

    //查找数据头
    int headerIndex = buffer.indexOf("88");
    if (headerIndex >= 0) { //找到数据头
        //从缓存区中截取整个数据包
        QByteArray packet = buffer.mid(headerIndex,10);
        //处理数据包
        ui->lineEdit_Code->setText(packet);
        DatareceivedStringList = Dataprocess(ui->lineEdit_Code->text());
        DisplayReceiveddata();
        qDebug()<<"看看找到了没 : "<<packet;
        //清空缓存区
        buffer.clear();
    } else { //未找到数据头
        //继续等待数据
        ui->txttest->setText("未找到");


    }

//    以上代码中，`headerData` 表示数据头的字节数组。缓存区使用 `QByteArray` 类型进行定义，`append` 方法用于添加新读入的数据，`indexOf` 方法用于查找数据头。
//    当找到数据头时，可以使用 `mid` 方法从缓存区中截取整个数据包，并进行处理。处理完成后，需要清空缓存区，以便下一组数据的读取。
//    当未找到数据头时，需要继续等待新的数据
}






/*
     * @brief generate the sending code based on the PC Cotroller
     *
     * @param[in] void
     *
     * @return void
*/

void MainWindow::SendCodeGenerator()
{
    int Value_Get = 255;
    int Value_Get_Right = 255;
    confIdValue_Send_Right = "00";

    if(ui->spinBoxTemperature->isVisible()){
        confId_Send = "00";
        Value_Get = ui->spinBoxTemperature->value();
        //QString hexStr = QString::number(num, 16)

    }else if(ui->mode1FunctionSel->isVisible()){
        confId_Send = "01";
        Value_Get = ui->mode1FunctionSel->currentIndex();


    }else if(ui->RateScrollBar->isVisible()){
        confId_Send = "02";
        QString value_right_String = ui->lineEdit_sendRate_Right->text();
        QString value_left_String = ui->lineEdit_sendRate->text();
        Value_Get = value_left_String.toInt();
        Value_Get_Right = value_right_String.toInt();

        confIdValue_Send_Right = QString::number(Value_Get_Right, 16).toUpper().rightJustified(2,'0');


    }else if(ui->PowerspinBox->isVisible()){
        confId_Send = "03";
        Value_Get = ui->PowerspinBox->value();
    }else{

        confId_Send = "FF";
    }
    confIdValue_Send = QString::number(Value_Get, 16).toUpper().rightJustified(2,'0');

    SendCode = "88" + TrigerMode_Send + confId_Send + confIdValue_Send + confIdValue_Send_Right;
    ui->txtSend->setText(SendCode);


}

/*
     * @brief SendData clicked
     *
     * @param[in] void
     *
     * @return void
 */
void MainWindow::on_btnSendData_clicked()
{
    SendCodeGenerator();
    QString m_strSendData = ui->txtSend->text();

        if(ui->checkBoxHexSend->isChecked())
        {
            if (m_strSendData.contains(" "))
            {
                m_strSendData.replace(QString(" "),QString(""));    //把空格去掉
            }

            QByteArray sendBuf;

            convertStringToHex(m_strSendData, sendBuf);             //把QString 转换 为 hex

            qDebug()<<"Write to serial: "<<sendBuf;
            serial->write(sendBuf);

        }
        else
        {
            serial->write(m_strSendData.toLocal8Bit());
            qDebug()<<"Write to serial: "<<m_strSendData.toLocal8Bit();

        }
}


/*写两个函数 向单片机发送数据
void sendInfo(char* info,int len){

    for(int i=0; i<len; ++i)
    {
        printf("0x%x\n", info[i]);
    }
    m_serialPort->write(info,len);//这句是真正的给单片机发数据 用到的是QIODevice::write 具体可以看文档
}

void sendInfo(const QString &info){

    QByteArray sendBuf;
    if (info.contains(" "))
    {
        info.replace(QString(" "),QString(""));//我这里是把空格去掉，根据你们定的协议来
    }
    qDebug()<<"Write to serial: "<<info;
    convertStringToHex(info, sendBuf); //把QString 转换 为 hex

    m_serialPort->write(sendBuf);这句是真正的给单片机发数据 用到的是QIODevice::write 具体可以看文档

}*/

/*
     * @brief convertStringToHex
     *
     * @param[in] QString
     *
     * @return void
     */

void MainWindow::convertStringToHex(const QString &str, QByteArray &byteData)
{
    int hexdata,lowhexdata;
            int hexdatalen = 0;
            int len = str.length();
            byteData.resize(len/2);
            char lstr,hstr;
            for(int i=0; i<len; )
            {
                //char lstr,
                hstr=str[i].toLatin1();
                if(hstr == ' ')
                {
                    i++;
                    continue;
                }
                i++;
                if(i >= len)
                    break;
                lstr = str[i].toLatin1();
                hexdata = convertCharToHex(hstr);
                lowhexdata = convertCharToHex(lstr);
                if((hexdata == 16) || (lowhexdata == 16))
                    break;
                else
                    hexdata = hexdata*16+lowhexdata;
                i++;
                byteData[hexdatalen] = (char)hexdata;
                hexdatalen++;
            }
            byteData.resize(hexdatalen);
}


/*
     * @brief Convert Char To Hex
     *
     * @param[in] Char
     *
     * @return Char
  */
char MainWindow::convertCharToHex(char ch)
{
    if((ch >= '0') && (ch <= '9'))
                 return ch-0x30;
             else if((ch >= 'A') && (ch <= 'F'))
                 return ch-'A'+10;
             else if((ch >= 'a') && (ch <= 'f'))
                 return ch-'a'+10;
    else return (-1);
}


// Receive Date From Serial
/*
     * @brief function called when data received form the serial
     *
     * @param[in] void
     *
     * @return void
*/
void MainWindow::receiveInfo()
{
    qDebug()<<"receiveInfo()";
    QByteArray info = serial->readAll();
    ui->txtReceiveData->append(info);
    QString strReceiveData = "";
    if(ui->checkBoxHexReceive->isChecked())
    {
        QByteArray hexData = info.toHex();
        strReceiveData = hexData.toUpper();

        qDebug()<<"接收到串口数据: "<<strReceiveData;

        for(int i=0; i<strReceiveData.size(); i+=2+1)
            strReceiveData.insert(i, QLatin1String(" "));
        strReceiveData.remove(0, 1);

        qDebug()<<"处理后的串口数据: "<<strReceiveData;
        //DatareceivedStringList = Dataprocess(strReceiveData);
        ui->txtReceiveData->append(strReceiveData);
        DisplayReceiveddata();
        }
    else
    {
        strReceiveData = info;

        //避免中文乱码
        QTextCodec *tc = QTextCodec::codecForName("GBK");
        QString tmpQStr = tc->toUnicode(info);

        //DatareceivedStringList = Dataprocess(tmpQStr);
        ui->txtReceiveData->append(tmpQStr);
        filterdata(tmpQStr);
        //DisplayReceiveddata();
        }

        //ui->txtReceiveData->append("\r\n");
}
/*void receiveInfo()
    {
        QByteArray info = m_serialPort->readAll();
        QByteArray hexData = info.toHex()();
        //这里面的协议 自己定义机器发什么 代表什么
        if(hexData == "0x10000")
        {
            //do something
        }
        else if(hexData  == "0x100001")
        {
            //do something
        }

    }*/

// Open the Serial
/*
     * @brief open all the serials
     *
     * @param[in] void
     *
     * @return void
*/
void MainWindow::on_btnOpenCOM_clicked()
{
      if (ui->btnOpenCOM->text()=="Open")
          {
              if(serial->isOpen())
              {
                  serial->clear();
                  serial->close();
              }
              serial->setPortName(m_portNameList[ui->comboBoxPortName->currentIndex()]);
              if(!serial->open(QIODevice::ReadWrite))
              {
                  qDebug()<<m_portNameList[ui->comboBoxPortName->currentIndex()]<<"Fail to Open!";
                  return;
              }

              //打开成功
              //serial->setBaudRate(ui->comboBoxBaudRate->currentText().toInt(),QSerialPort::AllDirections);//设置波特率和读写方向
              serial->setBaudRate(QSerialPort::Baud115200);
              serial->setDataBits(QSerialPort::Data8);              //数据位为8位
              serial->setFlowControl(QSerialPort::NoFlowControl);   //无流控制
              serial->setParity(QSerialPort::NoParity);             //无校验位
              serial->setStopBits(QSerialPort::OneStop);            //一位停止位

              connect(serial,SIGNAL(readyRead()),this,SLOT(receiveInfo()));

              ui->btnOpenCOM->setText("Close");
          }
      else{
              serial->close();
              ui->btnOpenCOM->setText("Open");
          }
}

/*
     * @brief action takes when you press the dial
     *
     * @param[in] void
     *
     * @return void
*/
void MainWindow::on_PCDialPress_clicked()
{
    ui->label_14->show();
    timer->start(500);

    m_pNameAnimation->setStartValue(1);
    m_pNameAnimation->setEndValue(0);
    m_pNameAnimation->start(QAbstractAnimation::KeepWhenStopped);

    QString m_strSendData = "8800"+ confId_Send+"0000";
    qDebug()<<"PC dial pressed";
    ui->txtSend->setText(m_strSendData);
    TrigerMode_Send = "00";
    serial->write(m_strSendData.toLocal8Bit());
    SendCodeGenerator();


}

// return current time
/*
     * @brief return the current time
     *
     * @param[in] void
     *
     * @return Current time in Qstring
*/
QString MainWindow::getTime()
{
    time_t timep;
    time (&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );
    return tmp;
}

//Ui initialization
/*
     * @brief UI initialization
     *
     * @param[in] void
     *
     * @return void
*/
void MainWindow::uiInitial()
{
    Mode_Temp_ui();
    ui->PCdial->setValue(50);
    ui->checkBoxHexReceive->hide();
    ui->checkBoxHexSend->hide();
    ui->lineEdit_ConfID_right->hide();
    ui->label_2->hide();
    ui->label_13->hide();
    ui->label_14->hide();
}

/*
     * @brief sperate the 10 bits hexString to 5 parts
     *
     * @param[in] 10 bits Hex in QString
     *
     * @return QSting List
*/
QStringList MainWindow::Dataprocess(QString hexString)
{
    //QString hexString = "1A2B3C4D90"; // 10位16进制数
    QStringList DataList;
    QString Header = hexString.mid(0, 2); // 截取第1-2位的数值
    DataList << Header;
    QString TriggerMode = hexString.mid(2, 2); // 截取第3-4位的数值
    DataList << TriggerMode;
    QString ConfigID = hexString.mid(4, 2); // 截取第5-6位的数值
    DataList << ConfigID;
    QString Value = hexString.mid(6, 2); // 截取第7-8位的数值
    DataList << Value;
    QString Right_Value = hexString.mid(8, 2); // 截取第8-10位的数值
    DataList << Right_Value;
    return DataList;
}

/*
     * @brief convertStringHextoInt
     *
     * @param[in] 10 bits Hex in QString
     *
     * @return Hex in int
*/
int MainWindow::convertStringHextoInt(QString hexString)
{
    QString standardHexString = hexString.remove("0x").remove("0X");
    unsigned int uintVar = standardHexString.toUInt(nullptr, 16);
    int intVar = static_cast<int>(uintVar);
    return intVar;
}
/*
//void MainWindow::onTimerTimeout()
//{
//    qDebug()<<"onTimerTimeou调用了";
//    timer->stop();
//    //ui->label_14->hide();
//    //ui->label_14->setWindowOpacity(0.2);

//    int opacity = (ui->label_14->windowOpacity()) * 100 - 10;
//    if (opacity < 0){
//        timer->stop();
//        ui->label_14->hide();
//    }
//    else
//    {
//        ui->label_14->setWindowOpacity(opacity / 100.0);
//    }
//    qDebug()<<opacity;


//}
*/

//set UI for each mode
void MainWindow::Mode_Temp_ui()
{
    ui->spinBoxTemperature->show();
    ui->label_5->show();

    ui->mode1FunctionSel->hide();
    ui->RateScrollBar->hide();
    ui->PowerspinBox->hide();
    ui->lineEdit_sendRate->hide();
    ui->label_12->hide();
    ui->radioButton_left->hide();
    ui->radioButton_right->hide();
    ui->lineEdit_sendRate_Right->hide();


}

void MainWindow::Mode_Mode_ui()
{
    ui->mode1FunctionSel->show();

    ui->spinBoxTemperature->hide();
    ui->label_5->hide();
    ui->RateScrollBar->hide();
    ui->PowerspinBox->hide();
    ui->label_12->hide();
    ui->lineEdit_sendRate->hide();
    ui->radioButton_left->hide();
    ui->radioButton_right->hide();
    ui->lineEdit_sendRate_Right->hide();


}

void MainWindow::Mode_Rate_ui()
{
    ui->spinBoxTemperature->hide();
    ui->mode1FunctionSel->hide();
    ui->label_5->hide();
    ui->PowerspinBox->hide();
    ui->label_12->hide();

    ui->lineEdit_sendRate->show();
    ui->radioButton_left->show();
    ui->radioButton_right->show();
    ui->RateScrollBar->show();
    ui->lineEdit_sendRate_Right->show();


}

void MainWindow::Mode_Power_ui()
{
    ui->PowerspinBox->show();
    ui->label_12->show();

    ui->spinBoxTemperature->hide();
    ui->mode1FunctionSel->hide();
    ui->label_5->hide();
    ui->RateScrollBar->hide();
    ui->lineEdit_sendRate->hide();
    ui->radioButton_left->hide();
    ui->radioButton_right->hide();
    ui->lineEdit_sendRate_Right->hide();
}

/*
     * @brief set the UI in the monitor
     *
     * @param[in] 10 bits Hex in QString
     *
     * @return Hex in int
*/
void MainWindow::on_comboBoxModeSel_currentIndexChanged(int index)
{
    if(index == 0){
        Mode_Temp_ui();
    }
    else if(index == 1){
        Mode_Mode_ui();
    }
    else if(index == 2){
        Mode_Rate_ui();
    }
    else if(index == 3){
        Mode_Power_ui();
    }
    else{
        confId_Send = "FF";
        confIdValue_Send = "FF";
    }
}


//设置PCdial旋转角度
void MainWindow::on_PCdial_valueChanged(int value)
{
    ui->PCdial->setValue(value);
}


//设置PCdial释放后复位
void MainWindow::on_PCdial_sliderReleased()
{
    //ui->txtSend->setText(QString::number(ui->PCdial->value()));
    //ui->PCdial->setValue(0);

}


//test
void MainWindow::on_pushButtontest_clicked()
{
    DatareceivedStringList = Dataprocess("1A2B3C4D90");
    ui->txttest->setText(DatareceivedStringList[4]);
}



void MainWindow::on_dial_Monitor_valueChanged(int value)
{
    /*表明dial值已经改变，并传递新值作为参数。
    此信号的参数值将为当前dial的位置以角度测量的值。
    如果要判断dial是向左转还是向右转，可以通过比较当前值与上一个值的大小来判断。
    在valueChanged(int)槽函数中，可以存储当前的值并进行比较
    static int prevValue = 0;
        if (value > prevValue) {
            //向右转
            if(ui->spinBoxTemperature->isVisible()){
                ui->spinBoxTemperature->setValue(value);

            }
        } else if (value < prevValue) {
            //向左转
            if(ui->spinBoxTemperature->isVisible()){
                ui->spinBoxTemperature->setValue(prevValue-value);
            }
        }

        prevValue = value;
        */
    /*
//    int Current_Value = ui->lineEdit_confIdValue->text().toInt();
//    QString Current_Mode = ui->lineEdit_ConfID->text();
//    //display
//    if(Current_Mode == "CONFID_TEMP"){
//        ui->dial_Monitor->setValue(Current_Value*5);
//    }
//    else if(Current_Mode == "CONFID_MODE"){
//        ui->dial_Monitor->setValue(Current_Value*8);
//    }
//    else if(Current_Mode == "CONFID_RATE"){
//        ui->dial_Monitor->setValue(Current_Value);
//    }
//    else if(Current_Mode == "CONFID_POWER"){
//        ui->dial_Monitor->setValue(Current_Value*8);
//    }
*/
}






void MainWindow::on_PCdial_sliderMoved(int position)
{

    TrigerMode_Send = "01";
    static int prevValue = ui->PCdial->value();
    /*if(ui->spinBoxTemperature->isVisible()){
        ui->spinBoxTemperature->setValue(position/5);
    }
    else if(ui->mode1FunctionSel->isVisible()){

    }*/

    if(ui->spinBoxTemperature->isVisible()){
        if (position > prevValue + 2) {
            //向右转
              ui->spinBoxTemperature->setValue(ui->spinBoxTemperature->value()+ 1);
              prevValue = position;

        } else if (position < prevValue -2) {
            //向左转
              ui->spinBoxTemperature->setValue(ui->spinBoxTemperature->value() - 1);
              prevValue = position;
        }
    }else if(ui->mode1FunctionSel->isVisible()){
        if (position > prevValue + 2) {
            //向右转
              ui->mode1FunctionSel->setCurrentIndex(ui->mode1FunctionSel->currentIndex() + 1);
              if(ui->mode1FunctionSel->currentIndex() == 4){
                  ui->mode1FunctionSel->setCurrentIndex(0);
              }
              prevValue = position;
        } else if (position < prevValue -2) {
            //向左转
              ui->mode1FunctionSel->setCurrentIndex(ui->mode1FunctionSel->currentIndex() - 1);
              if(ui->mode1FunctionSel->currentIndex() == 0){
                  ui->mode1FunctionSel->setCurrentIndex(4);
              }
              prevValue = position;
        }

    }else if(ui->RateScrollBar->isVisible()){
        if (position > prevValue) {
            //向右转
              ui->RateScrollBar->setValue(ui->RateScrollBar->value() + 1);
              prevValue = position;

        } else if (position < prevValue) {
            //向左转
              ui->RateScrollBar->setValue(ui->RateScrollBar->value() - 1);
              prevValue = position;
        }

    }else if(ui->PowerspinBox->isVisible()){
        if (position > prevValue + 2) {
            //向右转
              ui->PowerspinBox->setValue(ui->PowerspinBox->value()+ 1);
              prevValue = position;

        } else if (position < prevValue -2) {
            //向左转
              ui->PowerspinBox->setValue(ui->PowerspinBox->value() - 1);
              prevValue = position;
        }
    }
    else{
        confId_Send = "FF";
        confIdValue_Send = "FF";
    }
    SendCodeGenerator();
    QString m_strSendData = ui->txtSend->text();
    serial->write(m_strSendData.toLocal8Bit());
    qDebug()<<"Dial move and write to serial";
    /*static int prevValue = ui->PCdial->value();
        if (position > prevValue + 2) {
            //向右转
            if(ui->spinBoxTemperature->isVisible()){
                ui->spinBoxTemperature->setValue(ui->spinBoxTemperature->value()+ 1);
                prevValue = position;
            }
        } else if (position < prevValue -2) {
            //向左转
            if(ui->spinBoxTemperature->isVisible()){
                ui->spinBoxTemperature->setValue(ui->spinBoxTemperature->value() - 1);
                prevValue = position;
            }
        } */
}

void MainWindow::on_RateScrollBar_valueChanged(int value)
{
    if(ui->radioButton_left->isChecked()){
        ui->lineEdit_sendRate->setText(QString::number(value));
    }
    else{
        ui->lineEdit_sendRate_Right->setText(QString::number(value));
    }
}

