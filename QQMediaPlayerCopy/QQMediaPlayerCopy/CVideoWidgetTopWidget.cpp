#include "CVideoWidgetTopWidget.h"
#include <QEvent>
#include <QMouseEvent>
#include <string>
#include <iostream>

using namespace std;

//这个类设置的是依附在主屏幕上层的一个Widget控件，也就是包含了打开文件按钮和小箭头的一个Widget
//比较意外的是，这俩东西是一体的，两个控件是两个按钮，一个自定义按钮，一个正常按钮，然后依附于一个透明的窗口Widget
//一同显示于播放窗口上层

CVideoWidgetTopWidget::CVideoWidgetTopWidget(QWidget* p)
	:QWidget(p)
{
	setAttribute(Qt::WA_StyledBackground);  //设置窗口的属性，不然样式表不会起效
	this->setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::SubWindow);  //无边框模式，工具窗口，子窗口
	setAutoFillBackground(true);
	setMouseTracking(true);

	m_pOpenFileButton = new COpenFileButton(this);  //打开文件按钮也是自定义控件
	m_pOpenrRightlistButton = new QPushButton(this); //那个小箭头

	m_pOpenrRightlistButton->setText(u8"");
	m_pOpenrRightlistButton->setFixedSize(36, 80);  //宽36 高80

	//图片的偏移 80 / 2 - 32 / 2 = 40 - 16 = 24
	m_pOpenrRightlistButton->setStyleSheet("QPushButton{background-color:rgb(54,54,54);background-image:url(:/videoWidget/resources/videoWidget/left_arrow.svg); \
		background-position:center; \
		padding-top: 24px; \
		background-repeat: no-repeat; \
		border:none;} \
		QPushButton:hover{background-image:url(:/videoWidget/resources/videoWidget/left_arrow_hover.svg);border:none;} \
		QPushButton:pressed{background-image:url(:/videoWidget/resources/videoWidget/left_arrow.svg);border:none;}");

	connect(m_pOpenFileButton, &COpenFileButton::sig_openfile, this, &CVideoWidgetTopWidget::sig_OpenFile);
	connect(m_pOpenFileButton, &COpenFileButton::sig_openUrl, this, &CVideoWidgetTopWidget::sig_OpenUrl);
	connect(m_pOpenrRightlistButton, &QPushButton::clicked, this, &CVideoWidgetTopWidget::sig_OpenPlaylist);
}

CVideoWidgetTopWidget::~CVideoWidgetTopWidget()
{

}

void CVideoWidgetTopWidget::showRightBtn(bool flag) //决定箭头是否显示
{
	flag ? m_pOpenrRightlistButton->show() : m_pOpenrRightlistButton->hide();
}

void CVideoWidgetTopWidget::showOpenFileBtn(bool flag) //决定打开文件按钮是否显示(窗口正中央的打开文件按钮)
{
	flag ? m_pOpenFileButton->show() : m_pOpenFileButton->hide();
}

void CVideoWidgetTopWidget::resizeEvent(QResizeEvent* event) //无论窗口如何变化，'打开文件'按钮一定在中间
{
	if (m_pOpenFileButton && m_pOpenrRightlistButton)
	{
		int x1 = this->width() / 2 - m_pOpenFileButton->width() / 2;
		int y1 = this->height() / 2 - m_pOpenFileButton->height() / 2;
		m_pOpenFileButton->move(x1, y1);

		int x2 = this->width() - m_pOpenrRightlistButton->width();
		int y2 = this->height() / 2 - m_pOpenrRightlistButton->height() / 2;
		m_pOpenrRightlistButton->move(x2, y2);
	}
}



