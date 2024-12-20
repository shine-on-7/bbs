#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <memory>
#include <QMouseEvent>
#include "db.hpp"
#include "queryMVC.hpp"
#include "documentshow.hpp"
#include "documentedit.hpp"
#include "ai.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    // 初始化导航
    void InitNav();

    // 初始化帖子列表
    void InitNewsList();

    // 初始化帖子显示
    void InitNewsShow();

    // 初始化个人主页
    void InitPersonalHomepage();

    // 初始化商家查询页
    void InitMerchant();

    // 初始化帖子发布页
    void InitEdit();

    // 初始化AI聊天
    void InitAiChat();

    // 进入帖子列表页
    void EnterPostListPage(const QString& type);


    // 获取自身信息
    void SelfInfo(const QString& user);

private slots:
    // 查询商家
    void on_btn_query_merchant_clicked();

private:
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);

private:
    bool m_pressed = false;         // 按下状态
    QPoint m_pos;                   // 按下时的鼠标位置
    Ui::Widget *ui;

    QueryMVC* m_news_list;          // 帖子列表
    QueryMVC* m_news_history;       // 个人帖子列表
    QueryMVC* m_merchant;           // 商家信息

    DocumentShow* m_docShow;        // 帖子显示
    DocumentEdit* m_docEdit;        // 帖子编辑

    QString m_user;                 // 登录用户
    QMap<QString, QString> m_map;   // 登录用户的信息
    bool m_isMerchant = false;      // 是否是商家

    Ai *m_ai;   // 聊天AI
};
#endif // WIDGET_H
