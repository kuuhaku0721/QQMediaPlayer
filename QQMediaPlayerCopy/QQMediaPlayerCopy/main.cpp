
#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <string>
#include <QTranslator>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/resources/logo.png"));  //先设置一下图标，不然就是默认图标

    //加载翻译文件，这一步需要在main函数，也就是程序一启动就完成
    //Translation_zh_Hans.qm
    QTranslator* trans = new QTranslator();

    //注意路径的反斜线，翻译文件加载错误将不会翻译
    QString qm_path = a.applicationDirPath() + "/translations/Translation_zh_Hans.qm";
    trans->load(qm_path);
    a.installTranslator(trans);

    CMainWindow w;          //实例化主窗口 主窗口调用show 然后剩下的部分全都在MainWindow中完成 一样是Qt的步骤
    w.show();
    return a.exec();
}
