#include "widget.h"
#include "ui_widget.h"

#include <QFileDialog>
#include <QLayout>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    DB.Insignificance();
    InitNewsList();
    InitNewsShow();
    InitNav();
    InitPersonalHomepage();
    InitMerchant();
    InitEdit();
    InitAiChat();

    ui->stackedWidget->setCurrentIndex(0);
    DB.LabelShowImg(ui->label, "./res/head11.png");
    for(int i = 0; i < ui->stackedWidget->count(); i++)
        DB.SetBg(ui->stackedWidget->widget(i), "./res/bg.jpg");
}

Widget::~Widget()
{
    delete ui;
}


// 初始化导航
void Widget::InitNav()
{
    // 一、初始化顶部导航
    // 论坛主页
    connect(ui->btn_home, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentIndex(0);
    });

    // 商家信息
    connect(ui->btn_merchant, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentWidget(ui->page_merchant);
    });

    // 发布帖子
    connect(ui->btn_edit_nav, &QPushButton::clicked, this, [=]()
    {
        m_docEdit->clear();
        m_docEdit->setUid(m_map["id"]);
        ui->stackedWidget->setCurrentWidget(ui->page_edit);
    });

    // AI咨询
    connect(ui->btn_ai_nav, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentWidget(ui->page_ai);
    });

    // 个人主页
    connect(ui->btn_person_nav, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentWidget(ui->page_person);
    });

    // 退出
    connect(ui->btn_exit, &QPushButton::clicked, this, [=]()
    {
        exit(0);
    });


    // 二、初始化版面导航
    // 肉禽
    connect(ui->btn_bar_meat, &QPushButton::clicked, this, [=]()
    {
        EnterPostListPage("肉禽板块");
    });
    DB.BtnShowImg(ui->btn_bar_meat, "./res/bar/1.bmp");

    // 蛋禽
    connect(ui->btn_bar_egg, &QPushButton::clicked, this, [=]()
    {
        EnterPostListPage("蛋禽板块");
    });
    DB.BtnShowImg(ui->btn_bar_egg, "./res/bar/2.bmp");

    // 水禽
    connect(ui->btn_bar_water, &QPushButton::clicked, this, [=]()
    {
        EnterPostListPage("水禽板块");
    });
    DB.BtnShowImg(ui->btn_bar_water, "./res/bar/3.bmp");

    // 灌水板块
    connect(ui->btn_bar_exp, &QPushButton::clicked, this, [=]()
    {
        EnterPostListPage("灌水板块");
    });
    DB.BtnShowImg(ui->btn_bar_exp, "./res/bar/4.bmp");

    // 疾病防治
    connect(ui->btn_bar_ctr, &QPushButton::clicked, this, [=]()
    {
        EnterPostListPage("疾病防治");
    });
    DB.BtnShowImg(ui->btn_bar_ctr, "./res/bar/5.bmp");

    // 商品展示
    connect(ui->btn_bar_goods, &QPushButton::clicked, this, [=]()
    {
        EnterPostListPage("商品展示");
    });
    DB.BtnShowImg(ui->btn_bar_goods, "./res/bar/6.bmp");
}


// 初始化帖子列表
void Widget::InitNewsList()
{
    ui->tv_news->setSelectionBehavior(QAbstractItemView::SelectRows);   // 被点击时：选择整行

    // 初始化MVC
    m_news_list = new QueryMVC(ui->tv_news);
    m_news_list->SetView("SELECT * FROM news",
                         QStringList{"ID", "UID", "类型", "标签", "状态", "内容", "标题", "时间", "点赞量", "浏览量"},
                         QList<int>{ 0, 0, 0, 0, 0, 0, 1150, 189, 0, 0});

    // 查询
    m_news_list->SetQuery("news", "标题", "title", true);

    // 双击查看
    m_news_list->SetDoubleEvent([=]()
    {
        // 获取当前行记录
        QSqlRecord rec = m_news_list->currentRecord();
        m_docShow->bindRecord(rec);
        ui->stackedWidget->setCurrentIndex(2);
    });
}


// 初始化帖子显示
void Widget::InitNewsShow()
{
    m_docShow = new DocumentShow(ui->te_show, ui->title_show, ui->view_show, ui->btn_upvote);
}


// 初始化个人主页
void Widget::InitPersonalHomepage()
{
    ui->groupBox_2->hide();

    // 更改信息
    connect(ui->btn_startMod_2, &QPushButton::clicked, this, [=]()
    {
        ui->groupBox_2->show();
    });
}


// 初始化商家查询页
void Widget::InitMerchant()
{
    ui->tv_merchant->setSelectionBehavior(QAbstractItemView::SelectRows);   // 被点击时：选择整行

    // 初始化MVC
    m_merchant = new QueryMVC(ui->tv_merchant);
    m_merchant->SetView("SELECT id,user,tel,province,city,country,street,img FROM users WHERE type='商家'",
                         QStringList{"ID", "用户名", "电话", "省份", "城市", "区县", "乡镇(街道)", "头像"},
                         QList<int>{ 0, 290, 209, 200, 200, 200, 200, 0});

    // 双击查看
    m_merchant->SetDoubleEvent([=]()
    {
        // 获取当前行记录
        QSqlRecord r = m_merchant->currentRecord();

        // 当前所选商家信息
        QString id = r.value(0).toString();
        QString user = r.value(1).toString();
        QString tel = r.value(2).toString();
        QString img = r.value(7).toString();
        QString addr = QString("%1 %2 %3 %4").arg(r.value(3).toString())
                .arg(r.value(4).toString()).arg(r.value(5).toString()).arg(r.value(6).toString());
        DB.LabelShowImg(ui->lb_merchant_img, img);

        ui->lb_merchant_info->setText(QString("用 户 名：%1        联系方式：%2 \n\n地    址：%3").arg(user).arg(tel).arg(addr));

        // 跳转商家所发表的商品展示页
        ui->stackedWidget->setCurrentWidget(ui->page_merchant_news_list);

        // 查询商家所发表的展示
        std::shared_ptr<QueryMVC> mvcPtr = std::make_shared<QueryMVC>(ui->tv_merchant_new_list);
        mvcPtr->SetView(QString("SELECT * FROM news WHERE uid='%1' AND type='商品展示'").arg(id),
                             QStringList{"ID", "UID", "类型", "标签", "状态", "内容", "标题", "时间", "点赞量", "浏览量"},
                             QList<int>{ 0, 0, 0, 0, 0, 0, 1100, 199, 0, 0});

        // 跳转到文章页
        mvcPtr->SetDoubleEvent([=]()
        {
            // 获取当前行记录
            QSqlRecord rec = mvcPtr->currentRecord();
            m_docShow->bindRecord(rec);
            ui->stackedWidget->setCurrentIndex(2);
        });
    });
}


// 初始化帖子发布页
void Widget::InitEdit()
{
    m_docEdit = new DocumentEdit(ui->textEdit, ui->title, ui->type, ui->btn_submit);
}


// 初始化AI聊天
void Widget::InitAiChat()
{
    QString token = "";
    m_ai = new Ai(ui->te_show_2, ui->te_input, ui->btn_send, token);
}


// 进入帖子列表页
void Widget::EnterPostListPage(const QString &type)
{
    QString sql = QString("SELECT * FROM news WHERE type='%1'").arg(type);
    m_news_list->UpdateData(sql);
    ui->stackedWidget->setCurrentIndex(1);
}


// 获取自身信息
void Widget::SelfInfo(const QString& user)
{
    // 一、设置自身信息
    m_user = user;
    QSqlRecord r = DB.Record("users", QString("user='%1'").arg(user));
    if(r.isEmpty())
        return;

    for(int i = 0; i < r.count(); i++)
    {
        m_map[r.fieldName(i)] = r.value(i).toString();
    }

    // 显示基本信息
    QString info = QString("姓名:%1\n\n电话:%2\n\n地址:%3 %4 %5 %6").arg(m_map["user"]).arg(m_map["tel"]).
            arg(m_map["province"]).arg(m_map["city"]).arg(m_map["country"]).arg(m_map["street"]);
    ui->lb_info_2->setText(info);

    // 显示图片
    QPixmap pix(m_map["img"]);
    ui->lb_img_2->setPixmap(pix);
    ui->lb_img_2->setScaledContents(true);


    // 二、主页历史帖子
    ui->tv_news_history->setSelectionBehavior(QAbstractItemView::SelectRows);   // 被点击时：选择整行

    // 初始化MVC
    m_news_history = new QueryMVC(ui->tv_news_history);
    m_news_history->SetView(QString("SELECT * FROM news WHERE uid='%1'").arg(m_map["id"]),
                         QStringList{"ID", "UID", "类型", "标签", "状态", "内容", "标题", "时间", "点赞量", "浏览量"},
                         QList<int>{ 0, 0, 0, 0, 0, 0, 1100, 189, 0, 0});

    // 查询
    m_news_history->SetQuery("news", "标题", "title", true);
    // 删除
    m_news_history->SetDelAction("news");
    // 修改
    m_news_history->SetModAction([=]()
    {
        auto r = m_news_history->currentRecord();
        QString id = r.value(0).toString();

        m_docEdit->showEditDoc(id, m_map["id"]);
        ui->stackedWidget->setCurrentWidget(ui->page_edit);
    });
}


// 查询商家
void Widget::on_btn_query_merchant_clicked()
{
    // 获取输入
    QString province = ui->le_province->text().trimmed();
    QString city = ui->le_city->text().trimmed();
    QString country = ui->le_country->text().trimmed();
    QString street = ui->le_street->text().trimmed();

    // 构造查询语句条件
    QString sql = "SELECT id,user,tel,province,city,country,street FROM users WHERE type='商家'";
    if(!province.isEmpty()) sql += QString(" AND province LIKE '%%1%'").arg(province);
    if(!city.isEmpty()) sql += QString(" AND city LIKE '%%1%'").arg(city);
    if(!country.isEmpty()) sql += QString(" AND country LIKE '%%1%'").arg(country);
    if(!street.isEmpty()) sql += QString(" AND street LIKE '%%1%'").arg(street);

    qDebug() << sql;
    m_merchant->SetView(sql, QStringList{"ID", "用户名", "电话", "省份", "城市", "区县", "乡镇(街道)"},
                         QList<int>{ 0, 290, 209, 200, 200, 200, 200});
}


void Widget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        m_pressed = true;
        m_pos = e->pos();
    }
}


void Widget::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        m_pressed = false;
}


void Widget::mouseMoveEvent(QMouseEvent *e)
{
    move(e->pos() - m_pos + pos());		// 当前位置减去相对距离
}

