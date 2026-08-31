#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <cstring>
#include <QModelIndex>
#include <QSet>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QListView>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <QGuiApplication>
#include <QString>
#include <QList>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

struct CpuInfo
{
    int m_corenum;  //物理核心
    int m_logicalprocess;   //逻辑核心
    QString m_name;
    QString m_cpuflag;
};

enum model
{
    BEFORE_MODEL = 0,
    LAST_MODEL,
    CLEAR_MODEL

};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked();

    void on_checkBox_stateChanged();
    // 新增：右键菜单信号
    //void on_listView_customContextMenuRequested(const QPoint &pos);
    QMessageBox::StandardButton setAffinityWithConfirm(QString &cpuflag, int procnum, int model);
    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

private:
    int searchProcessName(QSet<QString> &proc, QSet<QString> &proc1, bool user_flag);
    bool setCpuRelevance(QSet<QString> &proc);
    int setChange(QSet<QString> &data, QStringList &list);
    QString getCpuInfo();

    bool Qb_onlyUserProcess;

    int Qi_model;
    int Qi_procNum;
    QString cpuinfo;
    CpuInfo Q_cpuInfo;
    QStandardItemModel *Qs_stringListModel;
    QSet<QString> Qs_gameBeforProcName;
    QSet<QString> Qs_gameLastProcName;
    QSet<QString> Qs_clearProcName;


private:
    void setupModel(QStringList proclist);
    void setupListView();
    void viewChangeProc(QSet<QString> &proc);
#if 0
    // 菜单动作槽
    void onActionAdd();
    void onActionDelete();
    void onActionEdit();
    void onActionMoveUp();
    void onActionMoveDown();
    void onActionClear();
    void onActionCopy();
    void setupContextMenu();
    int getCurrentRow();
#endif
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
