#include "mainwindow.h"
#include "./ui_mainwindow.h"


// 主窗口类MainWindow的构造函数实现

/* 构造函数:
 *  - 在C++中，​​构造函数的名称必须与类名完全相同​
 *  - 可以重载​​（即一个类可以有多个参数列表不同的构造函数）
 *  - ​没有返回类型声明​​（连void都不需要）
 */
// 定义在类外部，需用作用域解析运算符::
// QWidget *parent - 参数列表
// QMainWindow(parent) - 初始化基类和成员变量
// QWidget基础窗口部件，所有可视化控件的基类（按钮、文本框、窗口等均继承自它）, QMainWindow(应用程序主窗口)继承自QWidget

// ClassName::ClassName(int param) : 初始化列表 {构造函数体内赋值}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)              // 调用基类QMainWindow的构造函数
    , ui(new Ui::MainWindow)
    , serial(new QSerialPort(this))    // 初始化serial成员（创建串口对象，设置this为父对象）
    , m_settingsPanel(new SettingsPanel(this))
{
    ui->setupUi(this);
    initUI();
    initConnections();
    refreshPorts();
}

MainWindow::~MainWindow()
{
    if (serial->isOpen()) serial->close();
    delete ui;
}

// 初始化界面
void MainWindow::initUI() {

    m_portBox = new QComboBox(this);
    m_portBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        m_portBox->addItem(info.portName());
    }

    m_openCloseButton = new QPushButton("打开串口", this);
    m_refreshButton   = new QPushButton("刷新", this);
    m_settingsButton  = new QPushButton("设置", this);

    // 创建发送历史显示区域
    m_sentHistory = new QTextEdit(this);
    m_sentHistory->setReadOnly(true);
    m_sentHistory->setPlaceholderText("发送历史将显示在这里...");
    m_sentHistory->setMinimumHeight(120);

    // 创建发送toolbar
    m_sendEdit = new QLineEdit(this);
    m_sendEdit->setPlaceholderText("输入要发送的内容，按回车或点击发送按钮");
    m_hexSendCheck = new QCheckBox("Hex发送", this);

    // 创建接收数据显示区域
    m_receiveEdit = new QTextEdit(this);
    m_receiveEdit->setReadOnly(true);
    m_receiveEdit->setPlaceholderText("接收数据将显示在这里...");
    m_receiveEdit->setMinimumHeight(120);
    m_sendButton = new QPushButton("发送", this);

    m_hexReceiveCheck    = new QCheckBox("Hex 接收", this);
    m_clearReceiveButton = new QPushButton("清空", this);
    m_logFileCheck       = new QCheckBox("启用日志:", this);
    m_logFilePath        = new QLabel("", this);
    m_logFilePath->setMinimumWidth(100);

    // toptoolbar
    QToolBar *mainToolBar = new QToolBar("Top Toolbar", this);
    mainToolBar->setMovable(false);  // 禁止拖动
    mainToolBar->setStyleSheet("QToolBar { spacing: 10px; }");
    // 添加右侧控件
    mainToolBar->addWidget(m_openCloseButton);
    mainToolBar->addWidget(m_refreshButton);
    QLabel *deviceLabes = new QLabel("设备:", this);
    mainToolBar->addWidget(deviceLabes);
    mainToolBar->addWidget(m_portBox);
    // 添加弹簧（将右侧控件推到右边）
    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mainToolBar->addWidget(spacer);
    // 添加 Settings 按钮（最左侧）
    mainToolBar->addWidget(m_settingsButton);
    // 将工具栏添加到 MainWindow 顶部
    this->addToolBar(Qt::TopToolBarArea, mainToolBar);

    // sendToolBar
    QToolBar *sendToolBar = new QToolBar("Send Toolbar", this);
    sendToolBar->setMovable(false);
    // 添加发送编辑框到最左边
    sendToolBar->addWidget(new QLabel("发送:", this));
    sendToolBar->addWidget(m_sendEdit);
    sendToolBar->addWidget(m_hexSendCheck);
    sendToolBar->addWidget(m_sendButton);
    // 可以在这里添加其他控件到右边
    // 例如:
    // QWidget *rightSpacer = new QWidget();
    // rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // sendToolBar->addWidget(rightSpacer);
    // sendToolBar->addWidget(new QPushButton("其他按钮", this));

    // receiveToolBar
    QToolBar *receiveToolBar = new QToolBar("Receive Toolbar", this);
    receiveToolBar->setMovable(false);
    receiveToolBar->addWidget(m_hexReceiveCheck);
    receiveToolBar->addWidget(m_clearReceiveButton);
    receiveToolBar->addWidget(m_logFileCheck);
    receiveToolBar->addWidget(m_logFilePath);

    // 创建主布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    // 添加控件到主布局
    mainLayout->addWidget(m_sentHistory);
    mainLayout->addWidget(sendToolBar);
    mainLayout->addWidget(m_receiveEdit);
    mainLayout->addWidget(receiveToolBar);

    // 设置主窗口的中心部件
    this->setCentralWidget(centralWidget);

    m_settingsPanel->setFixedWidth(width());
}

// 信号槽连接
/*
 * QMetaObject::Connection QObject::connect(======
 *  const QObject *sender,        // 发送信号的对象
 *  const char *signal,           // 信号
 *  const QObject *receiver,      // 接收槽的对象
 *  const char *method            // 槽函数
);
 */
void MainWindow::initConnections() {
    connect(m_openCloseButton, &QPushButton::clicked, this, &MainWindow::onOpenCloseClicked);
    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::refreshPorts);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::onSerialDataReceived);
    connect(m_settingsButton, &QPushButton::clicked, this, [this]() { m_settingsPanel->togglePanel(); });
    connect(m_settingsPanel, &SettingsPanel::logFileChanged, this, &MainWindow::onLogFileChanged);
    connect(m_sendEdit, &QLineEdit::returnPressed, this, &MainWindow::onSendClicked);
    connect(m_clearReceiveButton, &QPushButton::clicked, this, [this](){ m_receiveEdit->clear();});
}

void MainWindow::onLogFileChanged(const QString &path) {
    QString displayText = path.isEmpty() ? "未设置日志文件" : path;
    m_logFilePath->setText(displayText);
    statusBar()->showMessage("日志文件设置为: " + displayText, 3000);

    // 自动勾选复选框（如果路径有效）
    m_logFileCheck->setChecked(!path.isEmpty());

    // 可以在这里初始化日志文件
    if (!path.isEmpty()) {
        QFile file(path);
        if (!m_settingsPanel->isAppendMode() && file.exists()) {
            file.remove();
        }
    }
}

void MainWindow::writeToLogFile(const QString &message) {
    // 只有复选框选中时才写入日志
    if (!m_logFileCheck->isChecked() || m_settingsPanel->logFilePath().isEmpty()) {
        return;
    }

    QFile file(m_settingsPanel->logFilePath());
    QIODevice::OpenMode mode = m_settingsPanel->isAppendMode()
                                   ? QIODevice::Append | QIODevice::Text
                                   : QIODevice::WriteOnly | QIODevice::Text;

    if (file.open(mode)) {
        QTextStream stream(&file);
        stream << QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ") << message << "\n";
        file.close();
    }
}

// 刷新串口列表
void MainWindow::refreshPorts() {
    statusBar()->showMessage("刷新串口列表");
    m_portBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        m_portBox->addItem(info.portName());
    }
}

/*
 * 异步通信: 采用固定的数据格式(数据以相同的帧格式传送 - 起始位/数据位/奇偶校验位/停止位)
 * 同步通信: 双方共享一个时钟, 数据开始前传送一两个同步符号
 *
 * 异步串行接口: 通信双方使用相同的波特率来实现同步
 *  - 有TTL和RS232两种电平
 *  - 串口报文: 起始位（1bit）+ 数据位（5~8bit）+ 奇偶校验位（1bit）+ 停止位（1~1.5bit）
 *  - 常用配置: 8N1 -> 数据位（Data Bits）/ 无校验（No Parity）/ 停止位（Stop Bits）= 1 位
 */

// 打开/关闭串口
void MainWindow::onOpenCloseClicked() {
    if (serial->isOpen()) {
        serial->close();
        m_openCloseButton->setText("打开串口");
        statusBar()->showMessage("串口已关闭");
        // 串口关闭后允许配置
        m_settingsButton->setEnabled(true);
    } else {
        // 串口打开时禁止配置
        m_settingsButton->setEnabled(false);

        QIODevice::OpenMode mode =  m_settingsPanel->getopenMode();
        QString modeStr;
        switch (mode) {
        case QIODevice::ReadOnly:  modeStr = "只读"; break;
        case QIODevice::WriteOnly: modeStr = "只写"; break;
        case QIODevice::ReadWrite: modeStr = "读写"; break;
        }

        serial->setPortName(m_portBox->currentText());
        // 波特率: 表示每秒传输的符号数, 通信双方必须使用相同的波特率
        // 比特率: 波特率 × 每个符号包含的比特数
        serial->setBaudRate(m_settingsPanel->getbaudRate());
        serial->setDataBits(m_settingsPanel->getdataBits());
        serial->setStopBits(m_settingsPanel->getstopBits());
        serial->setParity(m_settingsPanel->getparity());
        if (serial->open(mode)) { // 使用动态模式
            m_openCloseButton->setText("关闭串口");
            statusBar()->showMessage(QString("串口已连接: %1 (%2)").arg(serial->portName()).arg(modeStr));
        } else {
            QMessageBox::critical(this, "错误", QString("无法以%1模式打开串口: %2").arg(modeStr).arg(serial->errorString()));
        }
    }
}

void MainWindow::onSendClicked() {
    if (!serial->isOpen()) {
        QMessageBox::warning(this, "警告", "请先打开串口！");
        return;
    }

    if (serial->openMode() == QIODevice::ReadOnly) {
        QMessageBox::warning(this, "警告", "当前串口为只读模式，无法发送数据！");
        return;
    }

    QString data = m_sendEdit->text();
    if (data.isEmpty()) return;

    QString timestamp;
    if(m_settingsPanel->showTimeStamps()) {
        timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss.zzz] ");
    }

    if (m_hexSendCheck->isChecked()) {
        QByteArray hexData;
        // 移除所有空格，确保格式正确
        QString cleanData = data.replace(" ", "");
        // 检查是否为有效的十六进制字符串
        bool ok;
        cleanData.toULongLong(&ok, 16);
        if (!ok || cleanData.isEmpty()) {
            QMessageBox::warning(this, "警告", "无效的十六进制数据！");
            m_sentHistory->append(timestamp + "未发送: " + data);
            return;
        }
        hexData = QByteArray::fromHex(data.toLatin1());
        serial->write(hexData);
        m_sentHistory->append(timestamp + "发送: " + data);
    } else {
        serial->write(data.toUtf8());
        m_sentHistory->append(timestamp + "发送: " + data);
    }

    m_sendEdit->clear();

    if(!m_settingsPanel->logFilePath().isEmpty()) {
        writeToLogFile("发送: " + data);
    }
}

// 转义控制字符
QString MainWindow::escapeControlChars(const QString &input) {
    qDebug() << "escapeControlChars.";
    QString result;
    for(QChar ch : input) {
        ushort unicode = ch.unicode();
        // 转义ASCII控制字符(0x00-0x1F)，但保留换行、回车和制表符
        if(unicode < 32 && unicode != '\n' && unicode != '\r' && unicode != '\t') {
            result += QString("\\x%1").arg(unicode, 2, 16, QChar('0'));
        } else {
            result += ch;
        }
    }
    return result;
}

// 过滤控制字符
QString MainWindow::filterControlChars(const QString &input) {
    qDebug() << "filterControlChars.";
    QString result;
    for(QChar ch : input) {
        ushort unicode = ch.unicode();
        // 保留可见字符和必要的空白字符
        if(unicode >= 32 || unicode == '\n' || unicode == '\r' || unicode == '\t') {
            result += ch;
        }
    }
    return result;
}

// 接收数据
void MainWindow::onSerialDataReceived() {
    QByteArray data = serial->readAll();
    QString displayData;

    if (m_hexReceiveCheck->isChecked()) {
        // 十六进制显示（格式：A1 B2 C3）
        displayData = data.toHex(' ').toUpper();
    } else {
        // 文本模式
        if(m_settingsPanel->showControlCharacters()) {
            // 显示控制字符（转义形式）
            displayData = escapeControlChars(QString::fromUtf8(data));
        } else {
            // 默认处理（过滤控制字符）
            displayData = filterControlChars(QString::fromUtf8(data));
        }
    }

    // 添加时间戳（如果需要）
    QString timestamp;
    if(m_settingsPanel->showTimeStamps()) {
        timestamp = QDateTime::currentDateTime().toString("[hh:mm:ss.zzz] ");
    }

    m_receiveEdit->append(timestamp + "接收: " + displayData);

    // 自动滚动到底部
    QTextCursor cursor = m_receiveEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_receiveEdit->setTextCursor(cursor);

    if(!m_settingsPanel->logFilePath().isEmpty()) {
        writeToLogFile("接收: " + displayData);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    // 保持面板宽度与窗口一致
    if (m_settingsPanel) {
        m_settingsPanel->setFixedWidth(width());
        // 保持面板在按钮下方
        m_settingsPanel->move(0, m_settingsButton->y() + m_settingsButton->height());
    }
}

