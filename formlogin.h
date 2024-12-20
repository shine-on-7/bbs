#ifndef FORMLOGIN_H
#define FORMLOGIN_H

#include <QWidget>
#include <QMouseEvent>
#include "db.hpp"
#include "widget.h"
#include <ctime>

namespace Ui {
class FormLogin;
}

class FormLogin : public QWidget
{
    Q_OBJECT

public:
    explicit FormLogin(QWidget *parent = nullptr);

    // 加载昵称和头像
    void LoadNickImg();

    ~FormLogin();

private:
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);

private:
    bool m_pressed = false;     // 按下状态
    QPoint m_pos;       		// 按下时的鼠标位置
    Ui::FormLogin *ui;
    QStringList m_names;
    QStringList m_imgs;
    QVector<QPixmap> m_pixs;
};

#endif // FORMLOGIN_H
