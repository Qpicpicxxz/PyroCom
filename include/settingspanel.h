#ifndef SETTINGSPANEL_H
#define SETTINGSPANEL_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QComboBox>
#include <QParallelAnimationGroup>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckbox>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>

class SettingsPanel : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsPanel(QWidget *parent = nullptr);

    void togglePanel();
    bool isExpanded() const;
    int getbaudRate() const;
    QIODevice::OpenMode getopenMode() const;
    QSerialPort::DataBits getdataBits() const;
    QSerialPort::Parity getparity() const;
    QSerialPort::StopBits getstopBits() const;
    bool showControlCharacters() const;
    bool showTimeStamps() const;
    QString logFilePath() const;
    bool isAppendMode() const;

protected:
    void resizeEvent(QResizeEvent *event) override;

signals:
    void logFileChanged(const QString &path);

private slots:
    void browseLogFile();

private:
    void initAnimation();
    void initUI();
    void initConnections();
    void addLabelAndCombo(QHBoxLayout* layout, const QString& labelText, QComboBox*& comboBox, int width);
    QPropertyAnimation *m_animMin;
    QPropertyAnimation *m_animMax;
    QParallelAnimationGroup *m_animationGroup;
    bool m_expanded;
    int m_expandedHeight;
    QComboBox *m_baudRateBox;
    QComboBox *m_openModeBox;
    QComboBox *m_dataBitsBox;
    QComboBox *m_parityBox;
    QComboBox *m_stopBitsBox;
    QCheckBox *m_showCtrlCharsCheckbox;
    QCheckBox *m_showTimeStampsCheckbox;
    QPushButton *m_togglePanelButton;
    QLineEdit *m_logFilePathEdit;
    QPushButton *m_browseLogFileBtn;
    QCheckBox *m_appendLogCheckbox;
};

#endif // SETTINGSPANEL_H
