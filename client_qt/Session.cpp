#include "Session.h"
#include <QTcpSocket>
Session::Session(QTcpSocket* socket)
{
    m_socket = socket;
    m_type = RECV_HEAD;//消息类型
    m_head = NULL;//消息头
    m_bufLen = 0;//需要接收消息的长度
    m_readPos = 0;//当前读取到的长度
    m_body = NULL;//消息体
    m_isFinish = false;//接收消息,用来判断是否接收完全
    m_head = new char[sizeof(DeMessageHead)];//协议头初始化
    m_bufLen = sizeof(DeMessageHead);//协议头长度
}

Session::~Session(){
    if (m_head != NULL)
        delete[] m_head;
    if (m_body != NULL)
        delete[] m_body;
    if (m_socket != NULL){
        m_socket->close();
        m_socket = NULL;
    }
};

//解析事件（循环解析消息头，消息体）
int Session::readEvent()
{
    int ret = 0;
    switch (m_type)
    {
    case RECV_HEAD:
        ret = recvHead();//读取消息头
        break;
    case RECV_BODY:
        ret = recvBody();//读取消息体
        break;
    default:
        break;
    }
    if (ret == RET_AGAIN)//消息头读完返回RET_AGAIN 记录读取消息体
        return readEvent();
    return ret;
}

//解析消息头
int Session::recvHead()
{
    if (m_head == NULL)//保证消息头空间内存存在
    {
        m_head = new char[sizeof(DeMessageHead)];
        assert(m_head != NULL);
        m_bufLen = sizeof(DeMessageHead);
        m_readPos = 0;
    }
    int len = m_socket->read(m_head + m_readPos, m_bufLen - m_readPos);//接收网络数据
    if (len < 0)
        return RET_ERROR;
    if (len == 0)//读取结束
        return RET_END;
    m_readPos += len;//更新读取长度
    if (m_readPos == m_bufLen)//协议头读取结束
    {
        //读取协议体
        m_type = RECV_BODY;
        int bufLen = ((DeMessageHead *)m_head)->length;//取出消息体的长度
        m_body = new char[bufLen];//开辟消息体空间

        assert(m_body != NULL);
        m_bufLen = bufLen;
        m_readPos = 0;  //读取的位置置零
        return RET_AGAIN;
    }
    return 0;
}

//解析消息体
int Session::recvBody()
{
    /* 先判断读取的位置是否是 ((DeMessageHead*)m_head)->length 接收头指定的长度 */
    if (m_readPos == m_bufLen)//消息体读完了，继续读消息头
    {
        m_type = RECV_HEAD;
        handleMsgBase();//发送处理消息信号
        m_isFinish = true;//消息读取完毕
        return RET_AGAIN;//返回RET_AGAIN  继续解析消息头
    }
    /* 读取指定 Body 大小的数据 */
    int len = m_socket->read(m_body + m_readPos, m_bufLen - m_readPos);

    if (len < 0)
        return RET_ERROR;

    m_readPos += len;//更新已经读的

    /* 判断读取的位置是否是 ((DeMessageHead*)m_head)->length 接收头指定的长度 */
    if (m_readPos == m_bufLen)//消息体读取完毕
    {
        m_type = RECV_HEAD;
        handleMsgBase();
        m_bufLen = 0;
        m_isFinish = true;
        return RET_AGAIN;//返回RET_AGAIN  继续解析消息头
    }
    return RET_OK;//返回读取完毕
}

//解析出包  发送处理消息信号
int Session::handleMsgBase(){
    recvMsg *rMsg = new recvMsg();
    rMsg->head = m_head;
    rMsg->body = m_body;
    rMsg->bodyLen = m_bufLen;
    emit signal_handleMsg(rMsg);
//    handleMsg(rMsg);
    m_head = NULL;
    m_body = NULL;
    return RET_OK;
}

void Session::cleanSession(){

}
