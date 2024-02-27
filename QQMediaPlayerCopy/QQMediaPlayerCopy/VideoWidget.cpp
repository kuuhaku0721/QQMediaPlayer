#include "VideoWidget.h"
#include <QPainter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>
#include <QDebug>
#include <qt_windows.h>	//window_ 啥啥的消息 由重写的eventFilter时间过滤函数处理
#include <QApplication>
#include <QMessageBox>
#include <iostream>

using namespace std;

VideoWidget::VideoWidget(QWidget* p) :
	QWidget(p)
{
	setAttribute(Qt::WA_StyledBackground);  // 禁止父窗口样式影响子控件样式
	this->setAttribute(Qt::WA_OpaquePaintEvent);  //设置完该属性后，窗体第一次绘制将绘制全部内容，第二次绘制将不再绘制背景而只改变主要画面

	qApp->installNativeEventFilter(this);

	setMouseTracking(true); //跟踪鼠标事件 以隐藏鼠标指针

	//设置渐变色
	this->setStyleSheet("QWidget{\
                       background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #09115C, stop:1 #000000 );\
                        font-family:Microsoft YaHei\
                         }");

	QString openfile_button_qss = "QPushButton::menu-indicator:open{"
		"image:url(:/titleBar/resources/titleBar/down_arrow.svg);"
		"subcontrol-position:right center;"
		"subcontrol-origin:padding;border:none;}"
		"QPushButton::menu-indicator:closed{"
		"image:url(:/titleBar/resources/titleBar/up_arrow.svg);"
		"subcontrol-position:right center;"
		"subcontrol-origin:padding;border:none;}";

	setMinimumSize(800, 450);

	m_pTopWidget = new CVideoWidgetTopWidget(this);

	connect(m_pTopWidget, &CVideoWidgetTopWidget::sig_OpenFile, this, &VideoWidget::sig_OpenFile);
	connect(m_pTopWidget, &CVideoWidgetTopWidget::sig_OpenUrl, this, &VideoWidget::sig_OpenUrl);
	connect(m_pTopWidget, &CVideoWidgetTopWidget::sig_OpenPlaylist, this, &VideoWidget::sig_OpenPlaylist);
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::showTopWidget(bool show)
{
	if (show)
	{
		m_pTopWidget->show();
		m_pTopWidget->showOpenFileBtn(false);
	}
	else
	{
		m_pTopWidget->hide();
	}
}

void VideoWidget::setPlayStatus(bool play)
{
	m_isPlay = play;
}

void VideoWidget::showTopWidgetOpenfilesButton(bool show)
{
	m_pTopWidget->showOpenFileBtn(show);
}

//不设大小，而是在鼠标点着播放窗口的时候让窗口跟着鼠标跑
void VideoWidget::resizeEvent(QResizeEvent* event)
{
	m_dPos = this->pos();

	QPoint pos = this->mapToGlobal(QPoint(0, 0));
	m_pTopWidget->resize(this->width(), this->height());
	m_pTopWidget->move(pos);
	m_pTopWidget->show();
}

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
bool VideoWidget::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
#else
bool VideoWidget::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
#endif
{
	if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") 
	{
		MSG* pMsg = reinterpret_cast<MSG*>(message);   //到这里为止，上面的部分是规程，都要这么写，目的是捕获Windows系统级消息，然后转型，供下面使用
		
		// 从这里开始根据上面转型后的pMsg，判断类型，分类处理，等等操作
		if (pMsg->message == 1025 || pMsg->message == 512)  //至于1025和512是啥，不清楚，因该是和窗口移动相关
		{
			QPoint pos = this->window()->mapToGlobal(QPoint(0, 0));
			m_pTopWidget->move(pos + m_dPos);
		}
	}

	return false;
}
