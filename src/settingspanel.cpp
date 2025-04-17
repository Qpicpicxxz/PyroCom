#include "settingspanel.h"

SettingsPanel::SettingsPanel(QWidget *parent)
    : QWidget(parent),
    m_expanded(false),
    m_expandedHeight(100)
{
    setFixedHeight(0);
    setMinimumWidth(200);
    setAutoFillBackground(true);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(
        "SettingsPanel {"
        "   background-color: #f9f5f9;"  // 背景色
        "   border: 2px solid #d0d0d0;"  // 边框（宽度+样式+颜色）
        "   border-radius: 5px;"         // 圆角（可选）
        "}"
        );
    initUI();
    initConnections();
    initAnimation();
}

void SettingsPanel::initUI()
{
    m_baudRateBox = new QComboBox(this);
    m_baudRateBox->addItems({"9600", "19200", "38400", "57600", "115200"});
    m_baudRateBox->setCurrentText("115200");

    m_openModeBox = new QComboBox(this);
    m_openModeBox->addItems({"只读", "只写", "读/写"});
    m_openModeBox->setCurrentText("读/写");

    m_dataBitsBox = new QComboBox(this);
    m_dataBitsBox->addItems({"5", "6", "7", "8"});
    m_dataBitsBox->setCurrentText("8");

    m_stopBitsBox = new QComboBox(this);
    m_stopBitsBox->addItems({"1", "1.5", "2"});
    m_stopBitsBox->setCurrentText("1");

    m_showCtrlCharsCheckbox = new QCheckBox(tr("显示控制字符"), this);
    m_showCtrlCharsCheckbox->setChecked(false);

    m_showTimeStampsCheckbox = new QCheckBox(tr("显示时间戳"), this);
    m_showTimeStampsCheckbox->setChecked(false);

    // 无校验(NoParity)适用于大多数现代通信（因为硬件可靠性高）
    m_parityBox = new QComboBox(this);
    m_parityBox->addItems({"None", "Even", "Odd", "Mark", "Space"});
    m_parityBox->setCurrentText("None");

    m_togglePanelButton = new QPushButton(this);
    m_togglePanelButton->setText("▲");
    m_togglePanelButton->setToolTip("展开/收起设置面板");

    m_logFilePathEdit = new QLineEdit(this);
    m_logFilePathEdit->setFixedWidth(120);
    m_logFilePathEdit->setPlaceholderText("未设置日志文件路径");
    m_logFilePathEdit->setReadOnly(true);

    m_browseLogFileBtn = new QPushButton("...", this);
    m_browseLogFileBtn->setFixedWidth(30);

    m_appendLogCheckbox = new QCheckBox("追加", this);
    m_appendLogCheckbox->setChecked(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QHBoxLayout *row1Layout = new QHBoxLayout();
    addLabelAndCombo(row1Layout, "波特率  :", m_baudRateBox, 100);
    addLabelAndCombo(row1Layout, "数据位  :", m_dataBitsBox, 60);
    m_showCtrlCharsCheckbox->setFixedWidth(100);
    row1Layout->addWidget(m_showCtrlCharsCheckbox);
    row1Layout->addSpacing(1);
    row1Layout->addStretch();

    QHBoxLayout *row2Layout = new QHBoxLayout();
    addLabelAndCombo(row2Layout, "校验位  :", m_parityBox, 100);
    addLabelAndCombo(row2Layout, "停止位  :", m_stopBitsBox,60);
    m_showTimeStampsCheckbox->setFixedWidth(100);
    row2Layout->addWidget(m_showTimeStampsCheckbox);
    row2Layout->addSpacing(1);
    row2Layout->addStretch();

    QHBoxLayout *row3Layout = new QHBoxLayout();
    addLabelAndCombo(row3Layout, "打开方式:", m_openModeBox, 100);
    row3Layout->addWidget(m_logFilePathEdit);
    row3Layout->addWidget(m_browseLogFileBtn);
    row3Layout->addWidget(m_appendLogCheckbox);
    row3Layout->addStretch();
    m_togglePanelButton->setFixedSize(30, 30);
    row3Layout->addWidget(m_togglePanelButton);
    row3Layout->addSpacing(1);

    // 将三行添加到主布局
    mainLayout->addLayout(row1Layout);
    mainLayout->addLayout(row2Layout);
    mainLayout->addLayout(row3Layout);
}

void SettingsPanel::initConnections()
{
    connect(m_togglePanelButton, &QPushButton::clicked, this, &SettingsPanel::togglePanel);
    connect(m_browseLogFileBtn, &QPushButton::clicked, this, &SettingsPanel::browseLogFile);
}

void SettingsPanel::addLabelAndCombo(QHBoxLayout* layout, const QString& labelText, QComboBox*& comboBox, int width)
{
    QLabel *label = new QLabel(labelText, this);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    comboBox->setFixedWidth(width);
    layout->addWidget(label);
    layout->addWidget(comboBox);
    layout->addSpacing(1);
}

int SettingsPanel::getbaudRate() const
{
    return m_baudRateBox->currentText().toInt();
}

QIODevice::OpenMode SettingsPanel::getopenMode() const
{
    int openMode = m_openModeBox->currentIndex(); // 假设索引对应 0=只读, 1=只写, 2=读/写
    QIODevice::OpenMode mode;
    switch (openMode) {
    case 0:  mode = QIODevice::ReadOnly; break;
    case 1:  mode = QIODevice::WriteOnly; break;
    case 2:  mode = QIODevice::ReadWrite; break;
    default: mode = QIODevice::ReadWrite; // 默认读写
    }
    return mode;
}

QSerialPort::DataBits SettingsPanel::getdataBits() const
{
    return static_cast<QSerialPort::DataBits>(m_dataBitsBox->currentText().toInt());
}

QSerialPort::Parity SettingsPanel::getparity() const
{
    return static_cast<QSerialPort::Parity>(m_stopBitsBox->currentIndex());
}

QSerialPort::StopBits SettingsPanel::getstopBits() const
{
    return static_cast<QSerialPort::StopBits>(m_parityBox->currentIndex() + 1);
}

bool SettingsPanel::showControlCharacters() const
{
    return m_showCtrlCharsCheckbox->isChecked();
}

bool SettingsPanel::showTimeStamps() const
{
    return m_showTimeStampsCheckbox->isChecked();
}

void SettingsPanel::initAnimation()
{
    // 初始化最小高度动画
    m_animMin = new QPropertyAnimation(this, "minimumHeight");
    m_animMin->setDuration(100);

    // 初始化最大高度动画
    m_animMax = new QPropertyAnimation(this, "maximumHeight");
    m_animMax->setDuration(100);

    // 创建并行动画组
    m_animationGroup = new QParallelAnimationGroup(this);
    m_animationGroup->addAnimation(m_animMin);
    m_animationGroup->addAnimation(m_animMax);

    // 初始状态为收起
    setMinimumHeight(0);
    setMaximumHeight(0);
}

void SettingsPanel::togglePanel()
{
    m_expanded = !m_expanded;
    raise();
    // 设置动画的起始值和结束值
    m_animMin->setStartValue(height());
    m_animMin->setEndValue(m_expanded ? m_expandedHeight : 0);

    m_animMax->setStartValue(height());
    m_animMax->setEndValue(m_expanded ? m_expandedHeight : 0);

    // 启动动画
    m_animationGroup->start();
}

bool SettingsPanel::isExpanded() const
{
    return m_expanded;
}

void SettingsPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // 保持宽度与父窗口同步（如果父窗口存在）
    if (parentWidget()) {
        setFixedWidth(parentWidget()->width());
    }
}

void SettingsPanel::browseLogFile() {
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "选择日志文件",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "文本文件 (*.txt);;所有文件 (*.*)");

    if (!fileName.isEmpty()) {
        m_logFilePathEdit->setText(fileName);
        emit logFileChanged(fileName);
    }
}

QString SettingsPanel::logFilePath() const {
    return m_logFilePathEdit->text();
}

bool SettingsPanel::isAppendMode() const {
    return m_appendLogCheckbox->isChecked();
}
