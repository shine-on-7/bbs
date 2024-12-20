#ifndef QUERYMVC_HPP
#define QUERYMVC_HPP


#include <QObject>
#include <QDebug>
#include <QMessageBox>
#include <QTableView>
#include <QHeaderView>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlQuery>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <functional>
#include <QLineEdit>
#include <QPushButton>
#include "db.hpp"


// 模型视图控制类
class QueryMVC
{
private:
    QTableView* m_tv = nullptr;
    QWidget* m_parent = nullptr;
    QSqlQueryModel *m_model = nullptr;
    QString m_querySql;     // 绑定的查询语句

    // 菜单相关
    QMenu* m_menu;

    // 查询相关
    QLineEdit* m_le = nullptr;          // 查询输入框
    QPushButton* m_btnQuery = nullptr;  // 查询按钮

public:
    QueryMVC(QTableView* tv) : m_tv(tv), m_parent(tv->parentWidget()), m_menu(new QMenu(m_tv)){}


    // 更新数据
    void UpdateData(const QString& sql = "")
    {
        if(sql.isEmpty())
            m_model->setQuery(m_querySql);
        else
            m_model->setQuery(sql);
    }


    // 设置视图
    void SetView(const QString& sql, const QStringList &tableHeader, const QList<int> &width)
    {
        m_querySql = sql;
        m_model = new QSqlQueryModel(m_parent);
        m_model->setQuery(m_querySql);
        m_tv->setModel(m_model);

        // 设置表头及宽度
        for(int i = 0; i < tableHeader.size(); i++)
        {
            m_model->setHeaderData(i, Qt::Horizontal, tableHeader.at(i));
            m_tv->setColumnWidth(i, width.at(i));

            // 宽度为0则隐藏该列
            if(0 == width.at(i))
                m_tv->hideColumn(i);
        }

        // 不要行号
        m_tv->verticalHeader()->hide();

        // 上下文菜单信号
        m_tv->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(m_tv, &QTableView::customContextMenuRequested, [=](const QPoint &pos)
        {
            Q_UNUSED(pos);
            m_menu->exec(QCursor::pos());
        });
    }


    // 右键菜单：添加
    void SetAddAction(std::function<void()> addActionHandler)
    {
        QAction *actAdd = m_menu->addAction("添加");
        QObject::connect(actAdd, &QAction::triggered, m_parent, addActionHandler);
    }


    // 右键菜单：删除-交调用者删除
    void SetDelAction(std::function<void()> delActionHandler)
    {
        QAction *actDel = m_menu->addAction("删除");
        QObject::connect(actDel, &QAction::triggered, m_parent, delActionHandler);
    }


    ///
    /// \brief SetDelAction 右键菜单：删除-直接删除
    /// \param tableName 表名称
    /// \param primaryKeyName 主键
    /// \param primaryKeyIndex 主键在SELECT语句中的索引
    ///
    void SetDelAction(const QString& tableName, const QString& primaryKeyName = "id", int primaryKeyIndex = 0)
    {
        QAction *actDel = m_menu->addAction("删除");
        QObject::connect(actDel, &QAction::triggered, m_parent, [=]()
        {
            // 获取当前记录
            QSqlRecord rec = this->currentRecord();
            if(rec.isEmpty())
            {
                QMessageBox::critical(m_parent, "警告", "先选中再删除");
                return;
            }

            if(QMessageBox::StandardButton::Yes != QMessageBox::question(nullptr, "提示", "确定要删除"))
                return;

            // 删除操作
            QString primaryKeyVal = rec.value(primaryKeyIndex).toString();
            QSqlQuery query;
            QString sql = QString("DELETE FROM %1 WHERE %2 = %3").
                    arg(tableName).arg(primaryKeyName).arg(primaryKeyVal);
            if(query.exec(sql))
            {
                this->UpdateData();
                QMessageBox::information(m_parent, "提示", "删除成功");
                return;
            }
            else
            {
                qDebug() << "删除失败:" << query.lastError().text();
            }
        });
    }


    // 右键菜单：修改
    void SetModAction(std::function<void()> modActionHandler)
    {
        QAction *actMod = m_menu->addAction("修改");
        QObject::connect(actMod, &QAction::triggered, m_parent, modActionHandler);
    }


    ///
    /// \brief SetQuery 设置查询操作
    /// \param tableName 表名称
    /// \param placeholderText 输入框提示要输入的相关字段信息
    /// \param queryField 查询的字段
    /// \param fuzzy 是否要模糊查询
    ///
    void SetQuery(const QString& tableName, const QString& placeholderText, const QString& queryField = "id", bool fuzzy = false)
    {
        int x = m_tv->geometry().x();
        int y = m_tv->geometry().y();

        int width =  m_tv->width();
        int queryHeight = 41;

        // 输入框
        m_le = new QLineEdit(m_parent);
        m_le->setGeometry(x, y - (queryHeight + 10), width * 0.8 - 10, queryHeight);
        m_le->setStyleSheet("border:1px solid rgb(110,220,255);");
        m_le->setFont(QFont("Agency FB", 18));
        m_le->setPlaceholderText(placeholderText);

        // 查询按钮
        m_btnQuery= new QPushButton(" 查  询", m_parent);
        m_btnQuery->setGeometry(x + m_le->width() + 20, y - (queryHeight + 10), width * 0.2 - 10, queryHeight);

        QString btnStyle = "QPushButton{"
                    "border:1px solid rgb(0, 112, 249);"
                    "color: rgb(255, 255, 255);"
                    "border-radius:4px;"
                    "background-color: rgb(0, 112, 249);"
                "}"
                "QPushButton:hover{"
                    "border:1px solid rgb(0, 128, 0);"
                    "background-color: rgb(0, 128, 0);"
                "}"
                "QPushButton:press{"
                    "border:1px solid rgb(0, 112, 249);"
                    "background-color: rgb(0, 112, 249);"
                "}";

        m_btnQuery->setStyleSheet(btnStyle);
        m_btnQuery->setFont(QFont("Agency FB", 18));
        m_btnQuery->setIcon(QIcon(":/res/find.png"));
        m_btnQuery->setIconSize(QSize(35, 35));

        // 点击查询
        QObject::connect(m_btnQuery, &QPushButton::clicked, m_parent, [=]()
        {
            QString str = m_le->text().trimmed();
            if(str.isEmpty())
            {
                this->UpdateData();
                return;
            }

            // 开始查询
            QString sql = QString("SELECT * FROM %1 WHERE %2=%3").arg(tableName).arg(queryField).arg(str);
            m_model->setQuery(sql);
            if(!m_model->lastError().text().isEmpty())
            {
                if(fuzzy)
                    sql = QString("SELECT * FROM %1 WHERE %2 LIKE '%%3%'").arg(tableName).arg(queryField).arg(str);
                else
                    sql = QString("SELECT * FROM %1 WHERE %2='%3'").arg(tableName).arg(queryField).arg(str);
                m_model->setQuery(sql);

                qDebug() << m_model->lastError().text();
                qDebug() << sql;
            }
        });
    }


    // 双击信号
    void SetDoubleEvent(std::function<void()> doubleHandler)
    {
        QObject::connect(m_tv, &QTableView::doubleClicked, m_parent, doubleHandler);
    }


    // 当前选中行的记录
    QSqlRecord currentRecord()
    {
        int row = m_tv->currentIndex().row();
        if(-1 == row)
        {
            qDebug() << "先选中行";
            return QSqlRecord();
        }

        return m_model->record(row);
    }

};



#endif // QUERYMVC_HPP
