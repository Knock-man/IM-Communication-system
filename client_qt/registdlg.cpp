#include "registdlg.h"
#include "ui_registdlg.h"

#include <QGraphicsDropShadowEffect>

RegistDlg::RegistDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegistDlg)
{
    ui->setupUi(this);
    Init();
}

RegistDlg::~RegistDlg()
{
    delete ui;
}

void RegistDlg::mousePressEvent(QMouseEvent *event)
{
    mouseWindowTopLeft = event->pos();
}

void RegistDlg::mouseMoveEvent(QMouseEvent *event)
{

    //窗口移动
    if (event->buttons() & Qt::LeftButton)
    {
        mouseDeskTopLeft = event->globalPos();
        windowDeskTopLeft = mouseDeskTopLeft - mouseWindowTopLeft;  //矢量计算
        this->move(windowDeskTopLeft);     //移动到目的地
    }
}

void RegistDlg::disconnectedSlot()
{
    ChatLogInfo()<<"close socket...";
}

//套接字接收消息槽函数
void RegistDlg::readyReadSlot()
{
    DeMessageHead header;
    /*接收注册响应*/
    memset(&header,'\0',sizeof(DeMessageHead));//初始化
    int len = socket->read((char*)&header,sizeof(DeMessageHead));//读取缓冲区包头数据
    ChatLogInfo()<<u8"接收字节数：:"<<len;
    ChatLogInfo()<<u8"包长度："<<header.length;
    char *p = (char*)malloc(header.length);//开辟包 数据+数据段  空间
    DeMessagePacket* pPacket = (DeMessagePacket *)p;
    socket->read((char*)pPacket,header.length);//读取缓冲区 包+数据段
    if(pPacket->error == 0){
        m_status = true;
    }

    RegistInfoResp* resp = (RegistInfoResp*)(p+sizeof(DeMessagePacket));//接收账号
    ChatLogInfo()<<u8"账号:"<<resp->m_account;

   //记录下用户名，账号，密码
    userInfo.m_account = resp->m_account;
    strncpy(userInfo.m_userName,ui->lineEdit_username->text().toStdString().c_str(),ui->lineEdit_username->text().toStdString().size());
    strncpy(userInfo.m_password,ui->lineEdit_password->text().toStdString().c_str(),ui->lineEdit_password->text().size());

    free(p);
    return accept();    //关闭窗口 返回accept()
}

//初始化注册窗体
void RegistDlg::Init()
{
    this->setWindowTitle("WeChat 注册");
    memset(&userInfo,'\0',sizeof (userInfo));
    m_status = false;
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint); // 最小化按钮
    setWindowFlags(windowFlags() | Qt::WindowContextHelpButtonHint); // 帮助按钮

    int width = this->width()-10;
    int height = this->height()-10;
    ui->centerWidget->setGeometry(5,5,width,height);
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
}

//点击注册
void RegistDlg::on_pushBtn_regist_clicked()
{
    socket = new QTcpSocket;
    QString ipAddressStr = SERVER_ADDR;//IP
    quint16 port = SERVER_PORT;//地址

    //连接
    socket->connectToHost(ipAddressStr, port);
    if(socket->waitForConnected(3000))
    {
        ChatLogInfo() << "连接服务器成功";
    }
    else
    {//连接失败
        ChatLogInfo() << socket->errorString();
        return;
    }

    //绑定断开连接 接收消息的信号的槽函数
    connect(socket, SIGNAL(disconnected()),this, SLOT(disconnectedSlot()));     //客户端断开连接
    connect(socket, SIGNAL(readyRead()),this, SLOT(readyReadSlot()));           //接收消息

    RegistInfoReq info;
    memset(&info,'\0',sizeof(RegistInfoReq));
    strncpy(info.m_userName,ui->lineEdit_username->text().toStdString().c_str(),ui->lineEdit_username->text().toStdString().size());
    strncpy(info.m_password,ui->lineEdit_password->text().toStdString().c_str(),ui->lineEdit_password->text().size());
    writeMsg(&info,sizeof(RegistInfoReq),CommandEnum_Registe);//发送注册请求
}

//格式化注册消息 发送注册消息
void RegistDlg::writeMsg(void *buf, int bufLen, int type)
{
    DeMessageHead header;//协议头
    memcpy(header.mark, "DE", sizeof(header.mark));
    header.encoded = '0';//不加密
    header.version = '0';
    header.length = sizeof(DeMessagePacket) + bufLen;//消息包长度+消息体长度

    char *p = (char *)malloc(header.length);//开辟消息包+消息体空间
    DeMessagePacket *pPacket = (DeMessagePacket *)p;
    pPacket->mode = 2;//应答包
    pPacket->sequence = getSeqNum();//序列号(静态局部变量管理)
    pPacket->command = type;//命令号
    pPacket->error = 0;//错误码
    if(buf)
        memcpy(p + sizeof(DeMessagePacket), buf, bufLen);//写入消息体(用户名密码)

    char *sendMsg = new char[sizeof(DeMessageHead) + header.length];//[消息头][包][消息体]  length=包长度+消息体长度
    memset(sendMsg, 0, sizeof(DeMessageHead) + header.length);//初始化为0
    memcpy(sendMsg, &header, sizeof(DeMessageHead));//拷贝消息头
    memcpy(sendMsg + sizeof(DeMessageHead), p, header.length);//拷贝消息包+消息体
    free(p);//释放内存
    socket->write(sendMsg, sizeof(DeMessageHead) + header.length);//发送消息
    delete[] sendMsg;
}

void RegistDlg::on_pushBtn_hide_clicked()
{
    QWidget* pWindow = this->window();
    if(pWindow->isTopLevel())
        pWindow->showMinimized();
}

void RegistDlg::on_pushBtn_close_clicked()
{
   this->close();
}
