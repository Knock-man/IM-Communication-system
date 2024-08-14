#include "widget.h"
#include "common.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    if(w.getLoginStatus() == false)//是否已经登录(防止重复登录)
        return 0;
    w.show();//显示主窗口
    return a.exec();
}
