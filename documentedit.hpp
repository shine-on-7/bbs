#ifndef EDITOR_HPP
#define EDITOR_HPP

#include <QObject>
#include <QFileDialog>
#include <QWidget>
#include <QComboBox>
#include <QMenu>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include "db.hpp"


// 文章编辑类
class DocumentEdit
{
public:
    // 控件管理
    QTextEdit* m_textEdit = nullptr;        // 文章编辑区
    QLineEdit* m_title = nullptr;           // 文章标题
    QComboBox* m_type = nullptr;            // 文章类型
    QPushButton* m_btn_submit = nullptr;    // 发布文章
    QMenu m_menu;                           // 右键菜单
    QAction *m_actionInsertImg;             // 菜单子项

    // 状态管理
    QString m_uid;                          // 文章所属用户ID
    QString m_id;                           // 修改文章所需的ID

    // 文档管理
    QTextDocument *m_doc;
    QTextFrame *m_frame;

public:
    DocumentEdit(QTextEdit* textEdit, QLineEdit* title, QComboBox* box, QPushButton* btn)
        : m_textEdit(textEdit), m_title(title), m_type(box), m_btn_submit(btn)
    {
        m_doc = m_textEdit->document();
        m_frame = m_doc->rootFrame();

        // 字体大小
        QTextCharFormat fmt = m_textEdit->currentCharFormat();
        fmt.setFontPointSize(16);
        m_textEdit->mergeCurrentCharFormat(fmt);

        // 发布
        QObject::connect(m_btn_submit, &QPushButton::clicked, m_textEdit->parentWidget(), [=](){ submit(); });

        // 右键菜单
        m_textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
        m_actionInsertImg = m_menu.addAction("插入图片");
        QObject::connect(m_actionInsertImg, &QAction::triggered, m_textEdit->parentWidget(), [=]
        {
            // 文件对话框获取路径
            QString path = QFileDialog::getOpenFileName(nullptr, "打开文件", "D:\\", "Images (*.png *.jpg *.jpeg *.bmp)");
            if(path.isEmpty()) return;

            QTextImageFormat format;
            format.setName(path);
            m_textEdit->textCursor().insertImage(format);
        });

        QObject::connect(m_textEdit, &QLineEdit::customContextMenuRequested, [=]()
        {
            m_menu.exec(QCursor::pos());        // 显示上下文菜单，无此则不会显示上下文菜单
        });
    }


    // 设置文章所属用户的ID
    void setUid(const QString& uid)
    {
        m_uid = uid;
        m_id.clear();
    }


    // 清空所有内容：用以新增
    void clear()
    {
        m_title->clear();
        m_textEdit->clear();
        m_type->setCurrentIndex(0);
        m_id.clear();
    }


    // 显示要编辑的文档
    void showEditDoc(const QString& id, const QString& uid)
    {
        // 获取要编辑的文档ID
        m_id = id;
        m_uid = uid;

        auto map = DB.RecordMap("news", QString("id=%1").arg(m_id));
        if(map.isEmpty())
        {
            qDebug() << "查询文档记录失败, id=" << m_id;
            return;
        }

        // 显示信息
        m_title->setText(map["title"]);
        m_type->setCurrentText(map["type"]);

        // 读取文档
        QFile file(map["content"]);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream in(&file);
        QString text = in.readAll();
        file.close();

        // 显示文档
        m_textEdit->setHtml(text);
    }


    // 发布
    void submit()
    {
        // 判断
        QString title = m_title->text().trimmed();

        // 选中全部文本
        QTextCursor cursor(m_doc);
        cursor.select(QTextCursor::Document);
        QString text = m_textEdit->toHtml();

        if(title.isEmpty() || text.isEmpty())
        {
            QMessageBox::warning(m_textEdit->parentWidget(), "警告", "标题和内容不能为空");
            return;
        }

        // 获取要保存的路径
        QString resPath = QString("./res/data/%1_%2.rtf").arg(m_uid).arg(QDateTime::currentMSecsSinceEpoch());

        // 将文本转换为RTF格式并进行保存
        QFile file(resPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "文件打开失败";
            return;
        }
        QTextStream out(&file);
        out << text;
        file.close();

        // 保存到数据库
        // 当前要修改的id为空则说明是新建
        QString sql;
        if(m_id.isEmpty())
        {
            sql = QString("INSERT INTO news(uid, type, content, title, view, upvote) VALUES('%1', '%2', '%3', '%4', 0, 0)").
                    arg(m_uid).arg(m_type->currentText()).arg(resPath).arg(title);
        }
        // 修改
        else
        {
            sql = QString("UPDATE news SET uid='%2',type='%3',content='%4',title='%5' WHERE id=%1")
                        .arg(m_id).arg(m_uid).arg(m_type->currentText()).arg(resPath).arg(title);
        }

        // 执行SQL语句
        if(false == DB.Exec(sql))
        {
            QMessageBox::warning(m_textEdit->parentWidget(), "警告", "发表失败");
            return;
        }
        QMessageBox::information(m_textEdit->parentWidget(), "提示", "发布成功");
    }


};


#endif // EDITOR_HPP
