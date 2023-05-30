#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnSendData_clicked();
    void on_btnOpenCOM_clicked();
    void receiveInfo();
    void on_PCDialPress_clicked();
    void on_comboBoxModeSel_currentIndexChanged(int index);
    void on_PCdial_valueChanged(int value);
    void on_PCdial_sliderReleased();
    void on_pushButtontest_clicked();
    void on_dial_Monitor_valueChanged(int value);
    void on_PCdial_sliderMoved(int position);
    void on_RateScrollBar_valueChanged(int value);


private:
    Ui::MainWindow *ui;
    QSerialPort     *serial;
    QTimer* timer;
    QStringList m_portNameList;
    QStringList DatareceivedStringList; //receive the data in StringList
    QGraphicsOpacityEffect *m_pGraphicsOpacityEffect;
    QPropertyAnimation *m_pNameAnimation;

    void convertStringToHex(const QString &str, QByteArray &byteData);
    char convertCharToHex(char ch);
    QString getTime();
    void uiInitial();
    QStringList Dataprocess(QString Data);
    int convertStringHextoInt(QString hexString);
    //void onTimerTimeout();

    QString SendCode;
    QString TrigerMode_Send = "01";
    QString confId_Send = "00";
    QString confIdValue_Send = "01";
    QString confIdValue_Send_Right = "00";

    //Controler UI
    void Mode_Temp_ui();
    void Mode_Mode_ui();
    void Mode_Rate_ui();
    void Mode_Power_ui();

    // protocal define
    enum trigerMode{
        TRIGER_MODE_STRAIN=0, //按压应变片触发
        TRIGER_MODE_KNOB=1,  //旋转电机触发
    };

    enum confId {
        CONFID_TEMP=0,
        CONFID_MODE,
        CONFID_RATE,
        CONFID_POWER,
        CONFID_UNKNOW=0xFF,
    };

    struct confIdValue {
        unsigned int CONFID_VALUE_TEMP=0;
        unsigned int CONFID_VALUE_MODE=0;
        unsigned int CONFID_VALUE_RATE=0;
        unsigned int CONFID_VALUE_POWER=0;
        unsigned int CONFID_VALUE_UNKNOW=0xFF;
    } CurrentConfigureValue;


private:

    QStringList Scan_serial();
    void DisplayReceiveddata();
    void filterdata(QString info);
    void SendCodeGenerator();

};
#endif // MAINWINDOW_H
