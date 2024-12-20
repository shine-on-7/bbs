#ifndef DOCUMENT_SHOW_HPP
#define DOCUMENT_SHOW_HPP

#include <QObject>
#include <QDebug>
#include <QMessageBox>
#include <QSqlRecord>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QFile>


// 文档类
class DocumentShow
{
private:
    QTextEdit* m_textEdit;      // 文档显示
    QLabel* m_lb_title;         // 标题
    QLabel* m_lb_view;          // 浏览数
    QPushButton* m_btnUpvote;   // 点赞按钮


public:
    DocumentShow(QTextEdit* edit, QLabel* title, QLabel* view, QPushButton* btn)
        : m_textEdit(edit), m_lb_title(title), m_lb_view(view), m_btnUpvote(btn)
    {

    }


    // 绑定数据记录
    void bindRecord(QSqlRecord& rec)
    {
        // 显示标题
        m_lb_title->setText(rec.value(6).toString());

        // 显示浏览数
        m_lb_view->setText("浏览数：" + rec.value(9).toString());

        // 点赞按钮
        m_btnUpvote->setText("点赞数：" + rec.value(8).toString());
        QObject::connect(m_btnUpvote, &QPushButton::clicked, m_btnUpvote->parent(), [=]()
        {
            QString id = rec.value(0).toString();
            // 增加点赞
        });

        // 加载文档
        QString rtfPath = rec.value(5).toString();
        QFile file(rtfPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
        }

        QTextStream in(&file);
        QString text = in.readAll();
        file.close();

        // 显示文档
        m_textEdit->setHtml(text);
    }

};


#endif // DOCUMENT_SHOW_HPP
