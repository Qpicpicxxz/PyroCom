#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPropertyAnimation>
#include <QComboBox>
#include <QVBoxLayout>
#include <QSerialPort>
#include <QSerialPortInfo>

class SettingsPanel; // 前向声明

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

protected:
    void resizeEvent(QResizeEvent *event) override;

    // 表明这些函数是被动触发的
private slots:
    // 槽函数: on_控件对象名_信号名
    void onOpenCloseClicked();    // 打开/关闭串口
    void onSendClicked();         // 发送数据
    void onSerialDataReceived();  // 接收数据
    void refreshPorts();          // 刷新串口列表

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    SettingsPanel *m_settingsPanel;  // 替换原来的QWidget和动画(m_是C++中标识成员变量的命名约定)
    bool panelVisible = false;       // 面板是否可见
    void initUI();                   // 初始化界面
    void initConnections();          // 连接信号槽
};
#endif // MAINWINDOW_H
