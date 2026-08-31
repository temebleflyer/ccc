#include "widget.h"
#include "ui_widget.h"
#include <QProcess>
#include <QDebug>
#include <QtGlobal>
#include <QClipboard>
#include <QMap>
#include <windows.h>
#include <shellapi.h>

//过滤掉系统进程
QSet<QString> FilterList = {"SearchHost.exe", "ctfmon.exe", "msedgewebview2.exe", "explorer.exe", "rail.exe"
                         , "backgroundTaskHost.exe", "ShellHost.exe", "nvsphelper64.exe", "NVIDIA Overlay.exe"
                         , "RuntimeBroker.exe", "winlogon.exe", "WidgetService.exe", "conhost.exe", "ApplicationFrameHost.exe"
                         , "csrss.exe", "AcPowerNotification.exe", "dllhost.exe", "AppActions.exe", "NVDisplay.Container.exe", "SystemSettings.exe"
                         , "CrossDeviceResume.exe", "tasklist.exe", "nvcontainer.exe", "HwMonitor64.exe", "CrossDeviceService.exe", "StartMenuExperienceHost.exe"
                         , "qtcreator.exe", "dwm.exe", "crashpad_handler.exe", "crashpad_handler_.exe", "clangbackend.exe", "fontdrvhost.exe", "ChsIME.exe"
                         , "TextInputHost.exe", "pallas.exe", "atieclxx.exe", "rundll32.exe", "SecurityHealthSystray.exe", "ShellExperienceHost.exe"
                         , "taskhostw.exe", "splwow64.exe", "UserOOBEBroker.exe", "Widgets.exe", "sihost.exe"};

//因特尔cpu性能核对应关系
QMap<QString, QString> IntelCpuP = {{"285K", "0xFF"}, {"265K", "0XFF"}, {"245K", "0X3F"}, {"230F", "0X3F"},
                                {"14900", "0XFFFF"}, {"14700", "0XFFFF"}, {"14790", "0XFFFF"}, {"14600", "0XFFF"},{"14490", "0XFFF"}, {"14400", "0XFFF"},
                                {"13900", "0XFFFF"}, {"13700", "0XFFFF"},{"13790", "0XFFFF"}, {"13600", "0XFFF"}, {"13490", "0XFFF"}, {"13400", "0XFFF"},
                                {"12900", "0XFFFF"}, {"12700", "0XFFFF"}, {"12600K", "0XFFF"}};
//因特尔cpu能效核对应关系
QMap<QString, QString> IntelCpuE = {{"285K", "0xFFFF00"}, {"265K", "0XFFF00"}, {"245K", "0X3FC0"}, {"230F", "0X3C0"},
                                {"14900", "0XFFFF0000"}, {"14700", "0XFFF0000"}, {"14790", "0XFFF000"}, {"14600", "0XFF000"},{"14490", "0XF000"}, {"14400", "0XF000"},
                                {"13900", "0XFFFF0000"}, {"13700", "0XFF0000"},{"13790", "0XFF0000"}, {"13600", "0XFF000"}, {"13490", "0XF000"}, {"13400", "0XF000"},
                                {"12900", "0XFF0000"}, {"12700", "0XF0000"}, {"12600K", "0XF000"}};
QString CoreFlagStr = "";

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    Qb_onlyUserProcess = false;
    Qi_procNum = 0;
    cpuinfo = "";
    Qs_stringListModel = nullptr;
    Qs_gameBeforProcName.clear();
    Qs_gameLastProcName.clear();
    Qs_clearProcName.clear();
    Q_cpuInfo.m_name = "";
    Q_cpuInfo.m_corenum = 0;
    Q_cpuInfo.m_logicalprocess = 0;
}

Widget::~Widget()
{
    delete ui;
}

bool runPowerShellAsAdmin_ShellExecuteEx(const QString &command)
{
    QString cmdLine = command;
    std::wstring wCmdLine = cmdLine.toStdWString();

    // 准备 ShellExecuteEx 参数
    SHELLEXECUTEINFOW sei = { 0 };
    sei.cbSize = sizeof(SHELLEXECUTEINFOW);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"runas";
    sei.lpFile = L"powershell.exe";
    sei.lpParameters = wCmdLine.c_str();
    sei.nShow = SW_HIDE;

    // 执行
    BOOL success = ShellExecuteExW(&sei);

    if (success && sei.hProcess) {
        // 等待进程完成
        WaitForSingleObject(sei.hProcess, 5000);
        CloseHandle(sei.hProcess);
        return true;
    }

    qDebug() << "ShellExecuteEx 失败，错误码:" << GetLastError();
    return false;
}

//cpu后一半线程
QString cpuSetProcess(CpuInfo &cpuinfo, int model)
{
    QString coreflag = "";
    switch(model)
    {
    case BEFORE_MODEL:
        {
            int mid = cpuinfo.m_logicalprocess / 2;
            for (int i = 0; i < cpuinfo.m_logicalprocess; i++)
            {
                if (i < mid)
                {
                    coreflag += "1";
                }
                else
                {
                    coreflag += "0";
                }
            }
        }
        break;
    case LAST_MODEL:
    case CLEAR_MODEL:
        for (int i = 0; i < cpuinfo.m_logicalprocess; i++)
        {
            coreflag += "1";
        }
        break;
    default :
        return "error";
    }

    bool ok;
    uint value = coreflag.toUInt(&ok, 2);
    if (ok)
    {
        QString ret = "0x" + QString::number(value, 16).toUpper();
        //qDebug() << "看看指定的核心:" << ret;
        return ret;
    }
    return "error";
}

QString getCoreFlag(CpuInfo &cpuinfo, int model)
{
    QString coreflag = "";
    switch (model)
    {
    case BEFORE_MODEL:
        {
            for (auto p = IntelCpuE.begin(); p != IntelCpuE.end(); p++)
            {
                if (cpuinfo.m_name.contains(p.key(), Qt::CaseInsensitive))
                {
                    return p.value();
                }
            }
            coreflag = cpuSetProcess(cpuinfo, model);
            if (coreflag == "error")
            {
                return "error";
            }
            return coreflag;
        }
        break;
    case LAST_MODEL:
        {
            for (auto p = IntelCpuP.begin(); p != IntelCpuP.end(); p++)
            {
                if (cpuinfo.m_name.contains(p.key(), Qt::CaseInsensitive))
                {
                    return p.value();
                }
            }
            coreflag = cpuSetProcess(cpuinfo, model);
            if (coreflag == "error")
            {
                return "error";
            }
            return coreflag;
        }
        break;
    case CLEAR_MODEL:
        return cpuSetProcess(cpuinfo, model);
    default:
        return "error"; //不在大小核列表的cpu，游戏直接全核就行;
    }
}

//组装进程配置cpu相关性命令
QString productCommand(QSet<QString> &proc, CpuInfo &cpuinfo, int model)
{
    QString commandpre = "powershell.exe -Command \"{ Get-Process | Where-Object { $_.Name -eq '";
    QString commandmid = "' } | ForEach-Object { $_.ProcessorAffinity = ";
    QString commandsuff = " } }\"";
    QString coreflag = getCoreFlag(cpuinfo, model);
    QString cmd = "";
    for (auto str : proc)
    {
        QString processname = str.left(str.length() - 4);
        cmd += commandpre;
        cmd += processname;
        cmd += commandmid;
        cmd += coreflag;
        cmd += commandsuff;
        cmd += "\n\n";
    }
    return cmd;
}

bool executeMultiCommands(QSet<QString> &proc, CpuInfo &cpuinfo, int model)
{
    QProcess process;
    QString command = productCommand(proc, cpuinfo, model);
    if (command == "error")
    {
        return false;
    }
    //qDebug() << "看看组装的命令:" << command;
    return runPowerShellAsAdmin_ShellExecuteEx(command);
}

//AMD模式将系统进程及用户进程绑定到后一半线程
bool Widget::setCpuRelevance(QSet<QString> &proc)
{
    bool ret = executeMultiCommands(proc, Q_cpuInfo, Qi_model);
    qDebug() << "看看逻辑核心数量:" << Q_cpuInfo.m_logicalprocess << "看看模式" << Qi_model;
    return ret;
}

int Widget::searchProcessName(QSet<QString> &beforproc, QSet<QString> &lastproc, bool user_flag)
{
    QProcess process;
    process.start("tasklist", QStringList() << "/NH" << "/FO" << "CSV");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();

    // 如果进程执行出错，可以读取标准错误
    QString error = process.readAllStandardError();

    if (process.exitCode() == 0) {
        QStringList lines = output.split("\n");

        for (const QString &line : lines)
        {
            if (line.trimmed().isEmpty())
            {
                continue;
            }
            QStringList fields = line.split(",");
            QString processName = fields[0].remove("\"").trimmed();
            //QString pid = fields[1].remove("\"").trimmed();
            QString sessionName = fields[2].remove("\"").trimmed();
            QString sessionId = fields[3].remove("\"").trimmed();
            //QString memory = fields[4].remove("\"").trimmed();
            //会话名为 "Services" 或会话ID为 "0" 的视为系统进程
            if (user_flag)
            {
                if (Qi_model == LAST_MODEL)
                {
                    lastproc.insert(processName);
                    Qs_clearProcName.insert(processName);
                }
                else
                {
                    beforproc.insert(processName);
                    Qs_clearProcName.insert(processName);
                }
            }
            else
            {
                if (sessionName == "Services" || sessionId == "0")
                {

                }
                else
                {
                    bool exist = false;
                    for (auto str : FilterList)
                    {
                        if (processName == str)
                        {
                            exist = true;
                        }
                    }

                    if (!exist)
                    {
                        if (Qi_model == LAST_MODEL)
                        {
                            lastproc.insert(processName);
                            Qs_clearProcName.insert(processName);
                        }
                        else
                        {
                            beforproc.insert(processName);
                            Qs_clearProcName.insert(processName);
                        }
                    }
                }
            }
        }
    }
    else
    {
        qDebug() << "查询进程失败:" << error;
        return -1;
    }

    if (lastproc.size() > 0)
    {
        QSet<QString> newprocess;
        for (auto str : lastproc)
        {
            bool exist = false;
            for (auto str1 : beforproc)
            {
                if (str == str1)
                {
                    exist = true;
                }
            }
            if (!exist)
            {
               newprocess.insert(str);
            }
        }
        lastproc.clear();
        lastproc = newprocess;
        viewChangeProc(newprocess);
    }
    else
    {
        viewChangeProc(beforproc);
    }
    return 0;
}

void Widget::viewChangeProc(QSet<QString> &proc)
{
    Qi_procNum = proc.size();
    QStringList proclist;
    setChange(proc, proclist);
    setupModel(proclist);
    setupListView();
    qDebug() << "查询进程:" << proclist << "数量" << Qi_procNum;
}

int Widget::setChange(QSet<QString> &data, QStringList &list)
{
    if (data.size() == 0)
    {
        return -1;
    }
    for (const QString &str : data)
    {
        list.append(str);
    }
    return 0;
}

void Widget::setupModel(QStringList proclist)
{
    Qs_stringListModel = new QStandardItemModel(this);
    for (const QString &text : proclist) {
       QStandardItem *item = new QStandardItem(text);
       Qs_stringListModel->appendRow(item);
    }

    ui->listView->setModel(Qs_stringListModel);

}

void Widget::setupListView()
{
   // 启用拖拽功能
   ui->listView->setDragEnabled(true);
   ui->listView->setAcceptDrops(true);
   ui->listView->setDragDropMode(QAbstractItemView::InternalMove);
   ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
   // 可选：设置选择模式
   ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);

   // 可选：设置显示模式
   ui->listView->setViewMode(QListView::ListMode);  // 列表模式
   // ui->listView->setViewMode(QListView::IconMode); // 图标模式

   // 可选：设置网格大小
   // ui->listView->setGridSize(QSize(200, 50));
}

#if 0
//设置右键菜单
void Widget::setupContextMenu()
{
    // 连接右键信号到槽
    connect(ui->listView, &QListView::customContextMenuRequested,
            this, &Widget::on_listView_customContextMenuRequested);
}
// ===== 右键菜单响应 =====
void Widget::on_listView_customContextMenuRequested(const QPoint &pos)
{
    // 获取点击位置对应的索引
    QModelIndex index = ui->listView->indexAt(pos);

    // 如果点击在空白区域，可选择是否显示菜单
    // bool hasItem = index.isValid();

    // 创建右键菜单
    QMenu menu(this);

    // 创建动作
    QAction *actionAdd = new QAction("模式一", this);
    QAction *actionDelete = new QAction("模式二", this);
    QAction *actionEdit = new QAction("模式三", this);
    QAction *actionMoveUp = new QAction("模式四", this);
    QAction *actionMoveDown = new QAction("下移", this);
    QAction *actionClear = new QAction("清空全部", this);
    QAction *actionCopy = new QAction("复制项目", this);

    // 根据是否有选中项目，决定哪些菜单可点击
    bool hasSelection = !ui->listView->selectionModel()->selectedIndexes().isEmpty();
    actionDelete->setEnabled(hasSelection);
    actionEdit->setEnabled(hasSelection);
    actionMoveUp->setEnabled(hasSelection);
    actionMoveDown->setEnabled(hasSelection);
    actionCopy->setEnabled(hasSelection);

    // 添加动作到菜单
    menu.addAction(actionAdd);
    menu.addSeparator();  // 分割线
    menu.addAction(actionDelete);
    menu.addAction(actionEdit);
    menu.addSeparator();
    menu.addAction(actionMoveUp);
    menu.addAction(actionMoveDown);
    menu.addSeparator();
    menu.addAction(actionCopy);
    menu.addAction(actionClear);

    // 连接信号槽（使用 lambda 或普通函数）
    connect(actionAdd, &QAction::triggered, this, &Widget::onActionAdd);
    connect(actionDelete, &QAction::triggered, this, &Widget::onActionDelete);
    connect(actionEdit, &QAction::triggered, this, &Widget::onActionEdit);
    connect(actionMoveUp, &QAction::triggered, this, &Widget::onActionMoveUp);
    connect(actionMoveDown, &QAction::triggered, this, &Widget::onActionMoveDown);
    //connect(actionClear, &QAction::triggered, this, &Widget::onActionClear);
    connect(actionCopy, &QAction::triggered, this, &Widget::onActionCopy);

    // 在鼠标位置显示菜单
    menu.exec(ui->listView->viewport()->mapToGlobal(pos));
}

// ===== 菜单动作实现 =====

int Widget::getCurrentRow()
{
    QModelIndexList selected = ui->listView->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        return -1;
    }
    return selected.first().row();
}

void Widget::onActionAdd()
{
    bool ok;
    QString text = QInputDialog::getText(this, "添加项目",
                                         "请输入新项目名称:",
                                         QLineEdit::Normal,
                                         QString(), &ok);
    if (ok && !text.isEmpty()) {
        int row = Qs_stringListModel->rowCount();
        Qs_stringListModel->insertRow(row);
        Qs_stringListModel->setData(Qs_stringListModel->index(row, 0), text);
        qDebug() << "添加项目:" << text;
    }
}

void Widget::onActionDelete()
{
    QModelIndexList selected = ui->listView->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        return;
    }

    // 从后往前删除，避免索引变化
    for (int i = selected.size() - 1; i >= 0; --i) {
        int row = selected[i].row();
        QString text = Qs_stringListModel->data(Qs_stringListModel->index(row, 0)).toString();
        Qs_stringListModel->removeRow(row);
        qDebug() << "删除项目:" << text;
    }
}

void Widget::onActionEdit()
{
    int row = getCurrentRow();
    if (row < 0) {
        return;
    }

    QString oldText = Qs_stringListModel->data(Qs_stringListModel->index(row, 0)).toString();
    bool ok;
    QString newText = QInputDialog::getText(this, "编辑项目",
                                            "请输入新的项目名称:",
                                            QLineEdit::Normal,
                                            oldText, &ok);
    if (ok && !newText.isEmpty()) {
        Qs_stringListModel->setData(Qs_stringListModel->index(row, 0), newText);
        qDebug() << "编辑项目: " << oldText << " -> " << newText;
    }
}

void Widget::onActionMoveUp()
{
    int row = getCurrentRow();
    if (row <= 0) {
        return;
    }

    // 交换数据
    QString current = Qs_stringListModel->data(Qs_stringListModel->index(row, 0)).toString();
    QString previous = Qs_stringListModel->data(Qs_stringListModel->index(row - 1, 0)).toString();

    Qs_stringListModel->setData(Qs_stringListModel->index(row, 0), previous);
    Qs_stringListModel->setData(Qs_stringListModel->index(row - 1, 0), current);

    // 保持选中状态
    ui->listView->selectionModel()->select(Qs_stringListModel->index(row - 1, 0),
                                           QItemSelectionModel::ClearAndSelect);
}

void Widget::onActionMoveDown()
{
    int row = getCurrentRow();
    if (row < 0 || row >= Qs_stringListModel->rowCount() - 1) {
        return;
    }

    // 交换数据
    QString current = Qs_stringListModel->data(Qs_stringListModel->index(row, 0)).toString();
    QString next = Qs_stringListModel->data(Qs_stringListModel->index(row + 1, 0)).toString();

    Qs_stringListModel->setData(Qs_stringListModel->index(row, 0), next);
    Qs_stringListModel->setData(Qs_stringListModel->index(row + 1, 0), current);

    // 保持选中状态
    ui->listView->selectionModel()->select(Qs_stringListModel->index(row + 1, 0),
                                           QItemSelectionModel::ClearAndSelect);
}

void Widget::onActionCopy()
{
    int row = getCurrentRow();
    if (row < 0) {
        return;
    }

    QString text = Qs_stringListModel->data(Qs_stringListModel->index(row, 0)).toString();

    // 复制到剪贴板
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);

    qDebug() << "复制到剪贴板:" << text;
}

#endif

QString Widget::getCpuInfo()
{
    QProcess process;
    process.start("wmic cpu get Name,NumberOfCores, NumberOfLogicalProcessors, MaxClockSpeed /format:list");
    process.waitForFinished();
    QString info = process.readAllStandardOutput();
    QStringList lines = info.split('\n');
    qDebug() << "cpu信息:" << info;

    for (auto str: lines)
    {
        QStringList lines = str.split('=');
        if (lines.size() >= 2)
        {
            QString temp;
            if (lines[0] == "Name")
            {
                temp = "CPU型号：" + lines[1].trimmed();
                Q_cpuInfo.m_name = temp;
            }
            if (lines[0] == "NumberOfCores")
            {
                temp = "物理核心：" + lines[1].trimmed();
                Q_cpuInfo.m_corenum = lines[1].trimmed().toInt();
            }
            if (lines[0] == "NumberOfLogicalProcessors")
            {
                temp = "逻辑核心：" + lines[1].trimmed();
                qDebug() << "kkkk:" << temp;
                Q_cpuInfo.m_logicalprocess = lines[1].trimmed().toInt();
            }
            if (lines[0] == "MaxClockSpeed")
            {
                temp = "CPU频率：" + lines[1].trimmed();
            }
            cpuinfo = cpuinfo + temp + "\n";
        }
    }
    cpuinfo = cpuinfo + "当前进程数量：" + QString::number(Qi_procNum);
    ui->textEdit->clear();
    ui->textEdit->insertPlainText(cpuinfo);
    if (lines.size() >= 2) {
        return lines[1].trimmed(); // 第二行就是CPU型号
    }

    return "Unknown";
}

QMessageBox::StandardButton Widget::setAffinityWithConfirm(QString &cpuflag, int procnum, int model)
{
    // 先询问用户
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认操作",
                                 QString("确定要将 %1 个进程的 CPU 相关性设置为 %2 吗？")
                                 .arg(procnum)
                                 .arg(cpuflag),
                                 QMessageBox::Yes | QMessageBox::No);
    return reply;
}

//勾选包含系统进程
void Widget::on_checkBox_stateChanged()
{
    Qs_gameBeforProcName.clear();
    Qs_gameLastProcName.clear();
    Qb_onlyUserProcess = ui->checkBox->isChecked();
}

//打开游戏前
void Widget::on_pushButton_clicked()
{
    cpuinfo = ""; //清空cpu信息
    Qi_model = BEFORE_MODEL;
    Qs_gameLastProcName.clear();
    Qs_gameBeforProcName.clear();

    searchProcessName(Qs_gameBeforProcName, Qs_gameLastProcName, Qb_onlyUserProcess);   //查询用户进程
    getCpuInfo();   //获取cpu信息
    QString coreflag = getCoreFlag(Q_cpuInfo, Qi_model);
    auto ret = setAffinityWithConfirm(coreflag, Qs_gameBeforProcName.size(), Qi_model);
    if (ret == QMessageBox::Yes)
    {
        // 执行设置
        if (setCpuRelevance(Qs_gameBeforProcName)) {
            QMessageBox::information(this, "成功",
                                    QString("%1 个进程的 CPU 相关性已设置为 %2")
                                    .arg(Qs_gameBeforProcName.size())
                                    .arg(coreflag));
        } else {
            QMessageBox::critical(this, "失败", "设置失败，请检查进程是否存在！");
        }
    }
    // 用户进程的cpu相关性设置
}

//打开游戏后
void Widget::on_pushButton_3_clicked()
{
    cpuinfo = ""; //清空cpu信息
    Qi_model = LAST_MODEL;
    searchProcessName(Qs_gameBeforProcName, Qs_gameLastProcName, Qb_onlyUserProcess);   //查询新的用户进程
    getCpuInfo();   //获取cpu信息
    qDebug() << "查询进程数量" << Qs_gameLastProcName.size();
    QString coreflag = getCoreFlag(Q_cpuInfo, Qi_model);
    auto ret = setAffinityWithConfirm(coreflag, Qs_gameLastProcName.size(), Qi_model);
    if (ret == QMessageBox::Yes)
    {
        // 执行设置
        if (Qs_gameLastProcName.size() == 0)
        {
            QMessageBox::critical(this, "失败", "设置失败，没有检测到新进程启动！");
            return;
        }
        if (setCpuRelevance(Qs_gameLastProcName)) {
            QMessageBox::information(this, "成功",
                                    QString("%1 个进程的 CPU 相关性已设置为 %2")
                                    .arg(Qs_gameLastProcName.size())
                                    .arg(coreflag));
        } else {
            QMessageBox::critical(this, "失败", "设置失败，请检查进程是否存在！");
        }
    }
}

void Widget::on_pushButton_2_clicked()
{
    cpuinfo = ""; //清空cpu信息
    Qi_model = CLEAR_MODEL;
    searchProcessName(Qs_gameBeforProcName, Qs_gameLastProcName, Qb_onlyUserProcess);   //查询新的用户进程
    viewChangeProc(Qs_clearProcName);
    getCpuInfo();   //获取cpu信息
    QString coreflag = getCoreFlag(Q_cpuInfo, Qi_model);
    qDebug() << "查询进程数量" << Qi_model << "   " << coreflag;
    auto ret = setAffinityWithConfirm(coreflag, Qs_clearProcName.size(), Qi_model);
    if (ret == QMessageBox::Yes)
    {
        // 执行设置
        if (setCpuRelevance(Qs_clearProcName)) {
            QMessageBox::information(this, "成功",
                                    QString("%1 个进程的 CPU 相关性已设置为 %2")
                                    .arg(Qs_clearProcName.size())
                                    .arg(coreflag));
        } else {
            //if ()
            QMessageBox::critical(this, "失败", "设置失败，请检查进程是否存在！");
        }
    }
}
