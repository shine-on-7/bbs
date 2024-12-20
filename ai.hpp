#ifndef AI_HPP
#define AI_HPP

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTextEdit>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Ai : public QObject
{
    Q_OBJECT
private:
    // 网络管理
    QNetworkAccessManager *manager;     // 网络会话管理
    QString m_token;                    // ernie 模型token

    // 控件管理
    QTextEdit* m_teShow = nullptr;      // 显示控件
    QTextEdit* m_teEdit= nullptr;       // 编辑控件
    QPushButton *m_btnSend= nullptr;    // 发送按钮

public:
    Ai(QTextEdit *show, QTextEdit *edit, QPushButton* btnSend, const QString& token) :
        m_token(token), m_teShow(show), m_teEdit(edit), m_btnSend(btnSend)
    {
        manager = new QNetworkAccessManager(this);

        // 发送
        QObject::connect(m_btnSend, &QPushButton::clicked, this, [=]()
        {
            if(m_token.isEmpty())
            {
                QMessageBox::warning(nullptr, "警告", "先设置好 token 后再进行聊天");
                return;
            }

            QString text = m_teEdit->toPlainText().trimmed();
            if(text.isEmpty())
            {
                QMessageBox::warning(nullptr, "警告", "不能为空");
                return;
            }

            sendToErnie(text);          // 发送请求
            m_teEdit->clear();          // 清空输入
            addText(text, "我", true);   // 显示
        });
    }


private:
    // 向文心一言大模型发起请求
    void sendToErnie(QString content)
    {
        QNetworkRequest req(QUrl(QString("https://aip.baidubce.com/rpc/2.0/ai_custom/v1/wenxinworkshop/chat/completions?access_token=%1").arg(m_token)));
        req.setRawHeader("Content-Type", "application/json");
        req.setRawHeader("Accept", "application/json");

        QJsonObject jsonMsg;
        jsonMsg.insert("role", "user");
        jsonMsg.insert("content", content);

        QJsonArray jsonArr;
        jsonArr.append(QJsonValue(jsonMsg));

        QJsonDocument doc;
        QJsonObject jsonData;
        jsonData.insert("messages", jsonArr);
        doc.setObject(jsonData);

        // 发送请求
        QNetworkReply* reply = manager->post(req, doc.toJson());
        QObject::connect(reply, &QNetworkReply::finished, this, [=]()
        {
            QNetworkReply* ret = qobject_cast<QNetworkReply*>(sender());
            if (!ret)
            {
                qDebug() << "ret nullptr";
                return;
            }

            // 解析JSON
            QJsonDocument jd = QJsonDocument::fromJson(ret->readAll());
            if (jd.isObject())
            {
                QJsonObject json = jd.object();
                QString content = json.value("result").toString();
                //content = content.replace("#", "").replace("*","");
                addText(content, "AI", false);
            }
            else
            {
                qDebug() << "返回的不是JSON";
            }

            ret->deleteLater();
        });
    }


    // 加入内容
    void addText(QString text, QString nick, bool right)
    {
        QString str;
        if(right)
            str = QString("<table width='100%'>"
                          "<tr><td align='right' style='color:green'><span>%2</span></td></tr>"
                          "<tr><td align='right' style='font:18px'><span>%1</span></td></tr>"
                          "</table>").arg(text).arg(nick);
        else
            str = QString("<table width='100%'>"
                          "<tr><td align='left' style='color:blue'><span>%2</span></td></tr>"
                          "<tr><td align='left' style='font:18px'><span>%1</span></td></tr>"
                          "</table>").arg(text).arg(nick);
        m_teShow->append(str);
    }

};

#endif // AI_HPP
