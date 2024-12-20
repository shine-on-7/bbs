#include "formlogin.h"
#include "ui_formlogin.h"
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <cstdlib>

FormLogin::FormLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLogin)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    DB.Insignificance();
    LoadNickImg();

    if(1)
    {
        Widget* w = new Widget;
        w->SelfInfo("商家A");
        w->show();
        hide();
    }

    // 登录
    connect(ui->btn_log, &QPushButton::clicked, this, [=]()
    {
        // 判空
        QString user = ui->edit_user_logPage->text().trimmed();
        QString pwd = ui->edit_pwd_logPage->text().trimmed();
        if(user.isEmpty() || pwd.isEmpty())
        {
            QMessageBox::critical(this, "警告", "账号和密码不能为空");
            return;
        }

        if(!DB.CheckLogin(user, pwd))
        {
            QMessageBox::critical(this, "警告", "账号或密码错误");
            return;
        }

        // 登录成功
        this->hide();

        Widget* w = new Widget;
        w->SelfInfo(user);
        w->show();
    });

    // 注册
    connect(ui->btn_reg, &QPushButton::clicked, this, [=]()
    {
        // 判空
        QString user = ui->edit_user_regPage->text().trimmed();
        QString pwd = ui->edit_pwd_regPage->text().trimmed();
        QString cfg = ui->edit_cfg_regPage->text().trimmed();
        QString tel = ui->edit_tel->text().trimmed();
        if(user.isEmpty() || pwd.isEmpty() || tel.isEmpty())
        {
            QMessageBox::critical(this, " ", "账号、密码、电话均不能为空");
            return;
        }

        if(pwd != cfg)
        {
            QMessageBox::critical(this, " ", "两次密码不相同");
            return;
        }

        if(pwd.size() < 6)
        {
            QMessageBox::critical(this, " ", "密码需大于 5 位");
            return;
        }

        if(DB.DuplicationUser(user))
        {
            QMessageBox::critical(this, " ", "已有该ID，请换个ID注册");
            return;
        }

        // 写入数据库
        QString province = ui->edit_province->text().trimmed();
        QString city = ui->edit_city->text().trimmed();
        QString country = ui->edit_country->text().trimmed();
        QString street = ui->edit_street->text().trimmed();
        QString sql;

        QString img = ui->btn_img->property("img").toString();

        // 注册为商家
        if(ui->rb_merchant->isChecked())
        {
            if(province.isEmpty() || city.isEmpty() || country.isEmpty() || street.isEmpty())
            {
                QMessageBox::critical(this, " ", "所有信息均不能为空");
                return;
            }

            sql = QString("INSERT INTO users(user, pwd, tel, type, img, province, city, country, street) VALUES('%1','%2','%3','%4','%5','%6','%7','%8','%9')").
                    arg(user).arg(pwd).arg(tel).arg("商家").arg(img).arg(province).arg(city).arg(country).arg(street);
        }

        // 注册为普通用户
        else
        {
            sql = QString("INSERT INTO users(user, pwd, tel, type, img) VALUES('%1','%2','%3','%4','%5')").
                    arg(user).arg(pwd).arg(tel).arg("普通用户").arg(img);
        }

        if(DB.Exec(sql))
            QMessageBox::information(this, "提示", "注册成功");
        else
        {
            QMessageBox::critical(this, "提示", "注册失败");
            qDebug() << sql;
        }

    });

    // 转到登录注册
    connect(ui->btn_log_sw, &QPushButton::clicked, this, [=](){ ui->stack->setCurrentIndex(0); });
    connect(ui->btn_reg_sw, &QPushButton::clicked, this, [=](){ ui->stack->setCurrentIndex(1); });
    ui->stack->setCurrentIndex(0);

    // 退出
    connect(ui->btn_quit, &QPushButton::clicked, this, [=](){ exit(0); });

    // 随机昵称
    connect(ui->btn_rand, &QPushButton::clicked, this, [=]()
    {
        int index = rand() % m_names.size();
        QString name = m_names.at(index);
        ui->edit_user_regPage->setText(name);
    });

    // 切换头像
    connect(ui->btn_img, &QPushButton::clicked, this, [=]()
    {
        // 随机一个头像
        int index = rand() % m_imgs.size();
        ui->btn_img->setProperty("img", m_imgs.at(index));
        QPixmap pix = m_pixs.at(index);
        pix.scaled(ui->btn_img->width(), ui->btn_img->height(), Qt::KeepAspectRatio);   // 图片缩放至按钮大小
        ui->btn_img->setIcon(QIcon(pix));
        ui->btn_img->setIconSize(pix.size());                                           // 确保图标大小与按钮大小一致
    });

    // 验证器
    ui->edit_tel->setValidator(new QIntValidator(ui->edit_tel));
}


// 加载昵称和头像
void FormLogin::LoadNickImg()
{
    // 加载昵称
    QString filePath = "./res/nickname.txt";
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ERR("昵称文件不存在");
        exit(1);
    }

    while (!file.atEnd())
    {
        QString line = file.readLine();
        line = line.replace("\n", "");
        m_names.append(line);
    }

    // 获取所有头像
    QDir dir("./res/photo");
    foreach(auto img, dir.entryList(QDir::Files))
    {
        QString imgPath = QString("./res/photo/%1").arg(img);
        QPixmap pix(imgPath);
        if(!pix.isNull())
        {
            m_pixs.append(pix);
            m_imgs.append(imgPath);
        }
    }

    srand(time(NULL));
    // 随机一个头像
    int index = rand() % m_imgs.size();
    ui->btn_img->setProperty("img", m_imgs.at(index));
    QPixmap pix = m_pixs.at(index);
    pix.scaled(ui->btn_img->width(), ui->btn_img->height(), Qt::KeepAspectRatio);   // 图片缩放至按钮大小
    ui->btn_img->setIcon(QIcon(pix));
    ui->btn_img->setIconSize(pix.size());                                           // 确保图标大小与按钮大小一致
}

FormLogin::~FormLogin()
{
    delete ui;
}


void FormLogin::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        m_pressed = true;
        m_pos = e->pos();
    }
}


void FormLogin::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        m_pressed = false;
}


void FormLogin::mouseMoveEvent(QMouseEvent *e)
{
    move(e->pos() - m_pos + pos());		// 当前位置减去相对距离
}
