#ifndef DB_HPP
#define DB_HPP

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>
#include <QValidator>
#include <QIntValidator>
#include <QSqlRecord>
#include <QRandomGenerator>
#include <QLabel>
#include <QPushButton>
#define DB Db::Instance()
#define ERR(str) QMessageBox::critical(this, "警告", str);
#define OK(str) QMessageBox::information(this, "提示", str)

enum PAGE{HOME, LIST, SHOW, EDIT, ANN, PERSON, MANAGE};

// 单例模式的类
class Db
{
public:
    // 静态成员函数，用于获取唯一的实例
    static Db& Instance()
    {
        static Db instance; // 在第一次调用时创建唯一实例
        return instance;
    }

    // 防止拷贝构造函数和赋值操作符的调用
    Db(Db const&) = delete;
    void operator=(Db const&) = delete;

    void Insignificance(){}


    // 执行SQL语句
    bool Exec(const QString& sql)
    {
        QSqlQuery query;
        bool ret = query.exec(sql);
        if(!ret)
        {
            qDebug() << query.lastError();
            qDebug() << sql;
        }

        return ret;
    }


    // 是否存在
    bool Exist(const QString& sql)
    {
        m_model.setQuery(sql);
        return m_model.rowCount() > 0;
    }


    // 获取数据记录
    QSqlRecord Record(const QString& tableName, const QString& condition)
    {
        QString sql = QString("SELECT * FROM %1 WHERE %2").arg(tableName).arg(condition);
        QSqlQueryModel model;
        model.setQuery(sql);
        if(model.rowCount() > 0)
            return model.record(0);

        return QSqlRecord();
    }


    // 获取数据记录
    QMap<QString, QString> RecordMap(const QString& tableName, const QString& condition)
    {
        QString sql = QString("SELECT * FROM %1 WHERE %2").arg(tableName).arg(condition);
        QSqlQueryModel model;
        model.setQuery(sql);
        if(model.rowCount() > 0)
        {
            QMap<QString, QString> map;
            for(int i = 0; i < model.record(0).count(); i++)
                map[model.record(0).fieldName(i)] = model.record(0).value(i).toString();

            return map;
        }

        return QMap<QString, QString>();
    }



    // 是否重名
    bool DuplicationUser(QString user)
    {
        QSqlQueryModel model;
        model.setQuery(QString("SELECT * FROM users WHERE user='%1'").arg(user));
        return model.rowCount() > 0;
    }

    // 登录验证
    bool CheckLogin(QString user, QString pwd)
    {
        QSqlQueryModel model;
        model.setQuery(QString("SELECT * FROM users WHERE user='%1' AND pwd='%2'").arg(user).arg(pwd));
        return model.rowCount() > 0;
    }

    // 注册成功，保存用户信息
    void AddUser(QString user, QString pwd)
    {
        QSqlQuery query;
        query.exec(QString("INSERT INTO users VALUES('%1', '%2')").arg(user).arg(pwd));
    }


    // 按钮显示图片
    void BtnShowImg(QPushButton *btn, const QString path)
    {

        QPixmap pix(path);
        pix.scaled(btn->width(), btn->height(), Qt::KeepAspectRatio);   // 图片缩放至按钮大小
        btn->setIcon(QIcon(pix));
        btn->setIconSize(QSize(btn->width(), btn->height()));                                           // 确保图标大小与按钮大小一致
        btn->setStyleSheet("QPushButton { border: none; padding: 0px; }");      // 调整按钮的布局，以确保图片完全填充按钮
    }


    // 标签显示图片
    void LabelShowImg(QLabel *lab, QString path)
    {
        lab->setPixmap(QPixmap(path));			// 为标签设置图片
        lab->setScaledContents(true);			// 图片自适应控件大小
    }


    ///
    /// \brief Widget::SetBg 设置背景
    /// \param parent 要设置背景的父控件
    /// \param path 背景图片路径
    ///
    void SetBg(QWidget *parent, const QString &path)
    {
        QLabel *lb = new QLabel(parent);
        lb->lower();
        lb->setGeometry(0, 0, parent->width() * 2.2, parent->height() * 2);
        LabelShowImg(lb, path);
    }


private:
    // 私有构造函数，防止类被实例化
    Db()
    {
        // QMYQL QODBC 连接MySQL
        ConnectDataBase("QMYSQL", "127.0.0.1", 3306, "qt_breedbbs", "root", "root");
    }


    // 连接数据库
    void ConnectDataBase(const QString &engine, const QString &hostname, int port, const QString &dbname, const QString &username, const QString &password)
    {
        m_db = QSqlDatabase::addDatabase(engine);
        m_db.setHostName(hostname);         // MySQL服务器主机名
        m_db.setPort(port);                 // MySQL服务器端口号，默认为3306
        m_db.setDatabaseName(dbname);    	// 数据库名称
        m_db.setUserName(username);
        m_db.setPassword(password);

        // 打开数据库连接
        if (!m_db.open())
        {
            qDebug() << "Failed to connect to database:" << m_db.lastError().text();
            exit(0);
        }
    }


public:
    QSqlDatabase m_db;
    QSqlQueryModel m_model;
};


#endif // DB_HPP
