#include "logindlg.h"
#include "ui_logindlg.h"

#include <QBitmap>
#include <QGraphicsDropShadowEffect>
#include <QPainter>

LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
{
    ui->setupUi(this);
    Init();//初始化登录窗口属性
}

LoginDlg::~LoginDlg()
{
    delete ui;
}

void LoginDlg::Init()
{
    this->setWindowTitle("WeChat 登录");
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint); // 最小化按钮
    setWindowFlags(windowFlags() | Qt::WindowContextHelpButtonHint); // 帮助按钮

#if 0
    QWidget *container;
    container=new QWidget(this);
    container->setGeometry(5,5,width,height);
    container->setStyleSheet("QWidget{border-radius:4px;background:rgba(255,255,255,1);}");

    this->setWindowFlags(Qt::FramelessWindowHint);      //去掉标题栏无边框
    this->setAttribute(Qt::WA_TranslucentBackground,true);
    //实例阴影shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    //设置阴影距离
    shadow->setOffset(0, 0);
    //设置阴影颜色
    shadow->setColor(QColor(39,40,43,100));
    //设置阴影圆角
    shadow->setBlurRadius(10);
    //给嵌套QWidget设置阴影
    container->setGraphicsEffect(shadow);
#else
    int width = this->width()-10;//设置大小
    int height = this->height()-10;
    ui->centerWidget->setGeometry(5,5,width,height);//中心窗口大小
    ui->centerWidget->setStyleSheet("QWidget{border-radius:4px;background:rgba(255,255,255,1);}");  //设置圆角

    this->setWindowFlags(Qt::FramelessWindowHint);          //去掉标题栏无边框
    this->setAttribute(Qt::WA_TranslucentBackground,true);
    //实例阴影shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    //设置阴影距离
    shadow->setOffset(0, 0);
    //设置阴影颜色
    shadow->setColor(QColor(39,40,43,100));
    //设置阴影圆角
    shadow->setBlurRadius(10);
    //给嵌套QWidget设置阴影
    ui->centerWidget->setGraphicsEffect(shadow);
#endif


    memset(&m_userInfo,'\0',sizeof(m_userInfo));
}

void LoginDlg::mousePressEvent(QMouseEvent *event)
{
    mouseWindowTopLeft = event->pos();
}

void LoginDlg::mouseMoveEvent(QMouseEvent *event)
{
    //窗口移动
    if (event->buttons() & Qt::LeftButton)
    {
        mouseDeskTopLeft = event->globalPos();
        windowDeskTopLeft = mouseDeskTopLeft - mouseWindowTopLeft;  //矢量计算
        this->move(windowDeskTopLeft);     //移动到目的地
    }
}

//点击注册按钮
void LoginDlg::on_pushbtn_regist_clicked()
{
    RegistDlg registDlg;
    registDlg.show();
    if(registDlg.exec() == QDialog::Accepted)//窗口返回accept
    {
        if(registDlg.getStatus()){//注册成功
            UserInfo info = registDlg.getUserInfo();
            ui->lineEdit_account->setText(QString::number(info.m_account));
            strncpy(m_userInfo.m_userName,info.m_userName,sizeof(info.m_userName));
            ChatLogInfo()<<"注册成功，账号:"<<info.m_account;
        }
    }
}

//点击登录
void LoginDlg::on_pushButton_login_clicked()
{
    m_userInfo.m_account = ui->lineEdit_account->text().toInt();
    strncpy(m_userInfo.m_password,ui->lineEdit_password->text().toStdString().c_str(),ui->lineEdit_password->text().size());
    ChatLogInfo()<<u8"获得账号:"<<m_userInfo.m_account;
    ChatLogInfo()<<u8"获得密码:"<<m_userInfo.m_password<<","<<ui->lineEdit_password->text();

    return accept();    //accept()会关闭对话框，将退出状态设置为QDialog::Accepted
}

//点击最小化按钮
void LoginDlg::on_pushBtn_hide_clicked()
{
    QWidget* pWindow = this->window();
    if(pWindow->isTopLevel())
        pWindow->showMinimized();
}

//点击关闭按钮
void LoginDlg::on_pushBtn_close_clicked()
{
    this->close();
}
