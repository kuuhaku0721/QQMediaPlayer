#include "MainWindow.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <string>
#include <iostream>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>
#include <QStringListModel>
#include <QScreen>
#include <Windows.h>
#include <QApplication>

using namespace std;

//负责控制全屏播放的，全屏的时候包括标题在内所有控件都要隐藏，控件在MainWindow内可以根据状态隐藏，标题是自定义控件，要手动设置隐藏
BOOL CALLBACK EnumVLC(HWND hwnd, LPARAM lParam)
{
	TCHAR szTitle[1024];

	int nLen = GetWindowText(hwnd, szTitle, 1024);

	if (nLen > 0)
	{
		EnableWindow(hwnd, FALSE);  //EnableWindow是Win32中启用或禁用窗口的函数，hwnd是窗口句柄，第二个参数决定是否启用窗口输入
		KillTimer(NULL, 1);			//标题栏是自定义控件的窗口，所以用这种方式来控制，当全屏时，窗口隐藏，并禁用窗口输入
	}

	return TRUE;
}

void CALLBACK TimeProc(HWND hwnd, UINT msg, UINT_PTR id, DWORD time)
{
	HWND vlcHwnd = FindWindowEx(NULL, NULL, NULL, L"vlcmain"); //通过标题名字获取窗口句柄

	if (vlcHwnd)
	{
		EnumChildWindows(vlcHwnd, EnumVLC, NULL);  //枚举指定父窗口的子窗口(父窗口是第一个参数) 并将子窗口指向回调函数(第二个参数为回调函数)
													//第三个参数是要传递的参数，没有填NULL
		//作用，还是标题栏自定义窗口，它是主窗口的子窗口，通过枚举子窗口拿到标题栏窗口，然后通过回调函数停用它的输入以实现隐藏
		//		函数名TimeProc的是意思这个是定时器的回调函数，当定时结束后执行，全屏当刻先显示标题栏，一段时间后隐藏
	}
}

//防止中文乱码
string qstr2str(const QString& qstr)
{
	QByteArray cdata = qstr.toLocal8Bit();  //实现一个QString转string
	return string(cdata);
}

CMainWindow::CMainWindow(QWidget* parent)
	: QWidget(parent)
{
	this->setWindowTitle("vlcmain"); //设置窗口标题
	setMouseTracking(true);  //设置鼠标追踪，需要实时判断鼠标位置

	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);  //无边框模式 | 最大最小化按钮隐藏
	setAttribute(Qt::WA_Hover);  //设置窗口属性，窗口内需要具有WA_Hower才会跟踪鼠标时间

	m_pVlc.reset(new CVlcKits()); //实例化vlc控制器

	int nRet = m_pVlc->initVLC();

	switch (nRet) //错误处理
	{	
	case -1:
		QMessageBox::information(this, tr("Warn"),
			tr("libvlc_new failed"));

		exit(EXIT_FAILURE);

	case -2:
		QMessageBox::information(this, tr("Warn"),
			tr("libvlc_media_player_new failed"));

		exit(EXIT_FAILURE);
	}

	//将vlc的回调函数以信号槽的形式调用   主要是根据播放时间的变化更改文字和进度条显示
	//		信号sender				信号					响应体			槽
	connect(m_pVlc.get(), &CVlcKits::sig_UpdateTimeText, this, &CMainWindow::OnUpdateTimeText);
	connect(m_pVlc.get(), &CVlcKits::sig_TimeSliderPos, this, &CMainWindow::OnUpdateTimeSlider);

	//初始化UI，主要也是绑定信号和槽
	InitUI();
}

void CMainWindow::InitUI()
{
	QVBoxLayout* pVLay = new QVBoxLayout(this); //垂直方向Box布局  控制主窗口之间的布局
	pVLay->setSpacing(0); //格子之间间隔为0

	m_pTitleBar = new CTitleBar(this);				//初始化标题栏
	m_pVideoWidget = new VideoWidget(this);			//初始化视频播放区域
	m_pPlayCtrlBar = new CPlayCtrlBar(this);		//初始化播放控制栏
	m_pPlaylistWidget = new CPlayListWidget(this);	//初始化播放列表

	pVLay->addWidget(m_pTitleBar);

	QHBoxLayout* pHLay = new QHBoxLayout; //水平布局
	pHLay->addWidget(m_pVideoWidget);   //添加视频播放区域和播放列表，两个为水平排列
	pHLay->addWidget(m_pPlaylistWidget);
	pVLay->addLayout(pHLay);

	pVLay->addWidget(m_pPlayCtrlBar);  //垂直布局添加控制栏，这样就是三部分，从上到下依次为：标题栏，视频区，控制栏
	pVLay->setContentsMargins(0, 0, 0, 0); //边缘为0 即紧挨着

	m_pPlaylistWidget->hide();  //播放列表默认隐藏
	this->resize(800, 600);

	//绑定信号槽
		//标题栏 关闭信号
	connect(m_pTitleBar, &CTitleBar::sig_close, this, &CMainWindow::On_Close);
		//标题栏 全屏信号
	connect(m_pTitleBar, &CTitleBar::sig_ShowFullFcreen, this, &CMainWindow::On_ShowFullScreen);
		//标题栏 最小化信号
	connect(m_pTitleBar, &CTitleBar::sig_showMiniMode, this, &CMainWindow::On_ShowMiniMode);
		//标题栏 打开文件信号
	connect(m_pTitleBar, &CTitleBar::sig_openfile, this, &CMainWindow::on_openFile);
		//标题栏 打开Url信号
	connect(m_pTitleBar, &CTitleBar::sig_openUrl, this, &CMainWindow::on_openUrl);
		//播放区 打开文件信号
	connect(m_pVideoWidget, &VideoWidget::sig_OpenFile, this, &CMainWindow::on_openFile);
		//播放区 打开Url信号
	connect(m_pVideoWidget, &VideoWidget::sig_OpenUrl, this, &CMainWindow::on_openUrl);
		//播放区 打开播放列表信号
	connect(m_pVideoWidget, &VideoWidget::sig_OpenPlaylist, this, &CMainWindow::On_openRightPlaylist);
		//控制区 全屏信号
	connect(m_pPlayCtrlBar, &CPlayCtrlBar::sig_fullScreen, this, &CMainWindow::On_ShowFullScreen);
		//控制区 设置播放倍率信号
	connect(m_pPlayCtrlBar, &CPlayCtrlBar::sig_playRate, this, &CMainWindow::OnSetPlayRate);
		//控制区 开始播放信号
	connect(m_pPlaylistWidget, &CPlayListWidget::sig_doubleClickFileName, this, &CMainWindow::OnPlay);
}

void CMainWindow::resizeEvent(QResizeEvent* event)
{
	if (!this->isMaximized() && !this->isFullScreen()) //从全屏或最小转回正常尺寸的时候  重新设置回800*600的大小
	{
		QScreen* pScreen = qApp->primaryScreen(); //主要窗口
		QRect appRect = pScreen->availableGeometry();
		int sw = appRect.width();
		int sh = appRect.height();

		int thisW = this->width();
		int thisH = this->height();
	}
}

void CMainWindow::On_Close()
{
	if (m_isPlay)
	{   //如果处于正在播放状态，询问是否确认关闭，确认则退出程序
		QMessageBox::StandardButton rb = QMessageBox::warning(this, tr("warn"),
			tr("Do you want to end player?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		if (rb == QMessageBox::Yes)
		{
			close();
		}
	}
	else
	{
		close();
	}
}

void CMainWindow::on_openFile(const QStringList& fileList)  // 从右侧文件列表打开文件
{
	if (fileList.isEmpty())  // so，如果列表为空就直接返回，无法打开
		return;

	/// 设置列表三步骤 ///
	// 创建数据模型
	m_pStringListModel = new QStringListModel(this);

	// 为模型设置StringList，会导入StringList的内容
	m_pStringListModel->setStringList(fileList);

	// 为listView设置数据模型
	m_pPlaylistWidget->setPlayList(m_pStringListModel);

	// 调用vlc播放器播放视频   要播放的视频            播放区域窗口句柄
	if (0 != m_pVlc->play(fileList, (void*)(m_pVideoWidget->winId())))
	{
		QMessageBox::information(this, tr("Warn"), tr("can't play this video file"));
		return;
	}

	SetTimer(NULL, 1, 300, TimeProc);  //设置一个300ms计数器，第四个参数为计时器到时的响应函数
	m_pVideoWidget->setPlayStatus(true);  //设置播放状态标志
	m_pVideoWidget->showTopWidgetOpenfilesButton(false); //隐藏打开文件控件
	
	QString fileName = fileList[0];
	int index1 = fileName.lastIndexOf('/');
	fileName = fileName.mid(index1 + 1);
	m_pTitleBar->setFileNameLabelText(fileName); //设置标题为文件名
	
	m_isPlay = true;
	m_pPlayCtrlBar->showTimeLabel(true);  //播放时显示时间轴
	
	m_pTimer = new QTimer(this);
	m_pTimer->start(3000); //启动3秒的计数器  隐藏鼠标的
	connect(m_pTimer, &QTimer::timeout, this, &CMainWindow::On_timer);
}

// 打开网络媒体流
// rtsp://wowzaec2demo.streamlock.net/vod/mp4:BigBuckBunny_115k.mp4
void CMainWindow::on_openUrl(const QString& url)
{
	//播放网络流的操作没有特殊处理，与文件几乎相同，都是调用vlc去控制就好，无论什么格式视频，都是交由vlc统一管理
	m_pVlc->play(url, (void*)(m_pVideoWidget->winId()));

	m_isPlay = true;
	m_pPlayCtrlBar->showTimeLabel(true);

	m_pTimer = new QTimer(this);
	m_pTimer->start(3000);
	connect(m_pTimer, &QTimer::timeout, this, &CMainWindow::On_timer);
}

void CMainWindow::On_openRightPlaylist()
{	//打开右侧的播放列表，就是一个show和hidden的状态切换
	if (m_pPlaylistWidget->isHidden())
	{
		m_pPlaylistWidget->show();
	}
	else
	{
		m_pPlaylistWidget->hide();
	}
}

void CMainWindow::On_timer()
{	//开始播放之后的计数器，计时三秒后隐藏鼠标					TODO：好像有bug，鼠标隐藏后就消失了，不会再出来了(全屏模式下不会出来，正常尺寸会)
	if (m_isPlay)
	{
		if (mouse_last_moved_pos_ == QCursor::pos())  //mouse_last记录鼠标上一次停留位置，计时结束，鼠标位置还在这儿没动，说明要隐藏了
		{
			QPoint p1 = m_pVideoWidget->mapToGlobal(QPoint(0, 0));
			QRect rect = m_pVideoWidget->rect();
			QRect rect2(p1.x(), p1.y(), rect.width(), rect.height()); //左上角开始，覆盖长宽的矩形，这么干是为了能够在正常尺寸和最大化都能获得正确尺寸

			QPoint p2 = QCursor::pos(); //保存一下鼠标当前位置

			if (m_isPlay)
			{
				if (rect2.contains(p2)) //这一步一定执行的 只要函数执行时鼠标没动...
				{
					m_pVideoWidget->showTopWidget(false); //隐藏顶部控件，就是隐藏标题栏
					this->setCursor(Qt::BlankCursor);   //隐藏鼠标
					m_pTimer->stop(); //停止定时器
				}
				else
				{
					this->setCursor(Qt::ArrowCursor);  //显示正常鼠标
				}
			}
		}
		else
		{
			this->setCursor(Qt::ArrowCursor);  //显示正常鼠标
		}
	}
}

void CMainWindow::OnUpdateTimeText(const QString& str)
{
	m_pPlayCtrlBar->setCurPlayTime(str);  //跟随vlc控制的播放进度更新进度条的文字显示
}

void CMainWindow::OnUpdateTimeSlider(const int& value)  //更新进度条显示				TODO：有bug，进度条没法点击跳转
{
	//m_pVideoWidget->setTimeSliderPos(value);
	m_pPlayCtrlBar->setSliderValue(value);
	
}

// 倍速播放
void CMainWindow::OnSetPlayRate(double rate)
{
	m_pVlc->setPlayRate(rate);  //设置播放倍率，直接调用vlc的倍率控制就行
}

void CMainWindow::OnPlay(const QString& fileName)
{
	QStringList fileList;  //开始播放只需要把文件名(文件路径)传进来，然后调用vlc的play播放即可
	fileList << fileName;  //不过 在此之前，需要把文件添加到播放列表一下以作显示

	if (0 != m_pVlc->play(fileList, (void*)(m_pVideoWidget->winId())))  //直接调用，第二个参数是播放的依附窗口
	{
		QMessageBox::information(this, tr("Warn"), tr("can't play this video file"));
		return;
	}
}

void CMainWindow::On_ShowFullScreen()
{
	this->showFullScreen();  //调用一下显示全屏的函数
	m_pTitleBar->hide(); //然后隐藏掉不必要的控件
	m_pPlayCtrlBar->hide();
	m_pPlaylistWidget->hide();
}

void CMainWindow::On_ShowMiniMode()
{	//最小化
	if (m_isPlay)
	{
		m_isMiniMode = true;  //设置最小化状态
		m_pTitleBar->hide();  //隐藏掉不必要的控件
		m_pPlayCtrlBar->hide();
		m_pPlaylistWidget->hide();
		this->update(); //调用一下更新，因为最小化的时候是不需要显示窗口的
	}
} 

void CMainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{	//鼠标双击事件
	QPoint p1 = m_pVideoWidget->mapToGlobal(QPoint(0, 0));
	QRect rect = m_pVideoWidget->rect();
	QRect rect2(p1.x(), p1.y(), rect.width(), rect.height()); //还是获取当前的整个窗口的位置

	QPoint p2 = QCursor::pos(); //获取当前鼠标位置

	if (rect2.contains(p2)) //保证鼠标双击的位置在窗口包围范围内
	{
		m_isMiniMode = false;  // 退出迷你模式
		//双击事件：在正常尺寸和最大化尺寸之间转换   该隐藏的隐藏，该显示的显示
		if (this->isFullScreen())
		{
			this->showNormal();
			m_pTitleBar->show();
			m_pPlayCtrlBar->show();
		}
		else
		{
			this->showFullScreen();
			m_pTitleBar->hide();
			m_pPlayCtrlBar->hide();
			m_pPlaylistWidget->hide();
		}
	}
}

void CMainWindow::keyPressEvent(QKeyEvent* event)
{	//键盘按下事件，按下Esc 退出全屏
	if (event->key() == Qt::Key_Escape)
	{
		if (this->isFullScreen())
		{
			this->showNormal();		//显示正常尺寸，该显示显示，该隐藏隐藏
			m_isMiniMode = false;  // 退出迷你模式
			m_pTitleBar->show();
			m_pPlayCtrlBar->show();
		}
	}
}

void CMainWindow::mouseMoveEvent(QMouseEvent* event)
{	//鼠标移动事件，负责窗口缩放拉伸
	mouse_moved_ = true;
	mouse_last_moved_pos_ = QCursor::pos();		//鼠标移动，时刻维护上一次鼠标落点位置

	if (m_pTimer && !m_pTimer->isActive())  //如果定时器存在并且定时器没在计时
		m_pTimer->start(3000);		//那就启动计时，三秒内没移动，就该调用隐藏鼠标的函数了

	QPoint p1 = m_pVideoWidget->mapToGlobal(QPoint(0, 0));
	QRect rect = m_pVideoWidget->rect();
	QRect rect2(p1.x(), p1.y(), rect.width(), rect.height()); //还是获取这个窗口的位置
	QPoint p2 = QCursor::pos(); //获取鼠标当前位置

	if (m_isPlay) //正在播放状态下
	{
		if (rect2.contains(p2))
		{
			m_pVideoWidget->showTopWidget(true);  //鼠标在区域范围内  显示标题
		}
		else
		{
			m_pVideoWidget->showTopWidget(false); //鼠标不在范围内，隐藏标题
		}
	}

	/*********************************窗口拉伸*********************************/
	//最大化状态时不能拉伸
	if (isMaximized())
	{
		return;
	}
	//根据位置设置鼠标样式
	SetMouseCursor(event->pos().x(), event->pos().y());

	if ((event->buttons() == Qt::LeftButton) && left_button_pressed_) //如果触发事件的是左键并且左键是按下状态 即保证拖拽缩放只有左键可以
	{
		QPoint point_offset = event->globalPos() - last_position_;  //偏移量就是要缩放的大小，用全局位置来计算
		if (mouse_press_region_ == kMousePositionMid)  //如果是在中间区域拖动的，那就拖着窗口来回跑
		{
			if (m_isMiniMode)
			{
				// 迷你模式可移动窗口
				setCursor(Qt::ArrowCursor);
				move(point_offset + last_point_);
			}
		}
		else //如果不是在中间区域，那就判断在哪边，然后往那个方向拖拽即可
		{
			QRect rect = geometry();
			switch (mouse_press_region_)
			{
			case kMousePositionLeftTop:
				rect.setTopLeft(rect.topLeft() + point_offset);
				break;
			case kMousePositionTop:
				rect.setTop(rect.top() + point_offset.y());
				break;
			case kMousePositionRightTop:
				rect.setTopRight(rect.topRight() + point_offset);
				break;
			case kMousePositionRight:
				rect.setRight(rect.right() + point_offset.x());
				break;
			case kMousePositionRightButtom:
				rect.setBottomRight(rect.bottomRight() + point_offset);
				break;
			case kMousePositionButtom:
				rect.setBottom(rect.bottom() + point_offset.y());
				break;
			case kMousePositionLeftButtom:
				rect.setBottomLeft(rect.bottomLeft() + point_offset);
				break;
			case kMousePositionLeft:
				rect.setLeft(rect.left() + point_offset.x());
				break;
			default:
				break;
			}
			setGeometry(rect);
			last_position_ = event->globalPos();
		}
	}
	/******************************************************************/
}

void CMainWindow::mouseReleaseEvent(QMouseEvent* event)
{
	left_button_pressed_ = false;
}

void CMainWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->buttons() == Qt::LeftButton) {
		left_button_pressed_ = true; //如果按下的是左键，改变状态，然后判断按下的区域并保存，交给鼠标移动事件去处理(因为窗口有Hover属性，所以能跟踪Move事件)
		last_point_ = pos();
		last_position_ = event->globalPos();
		mouse_press_region_ = GetMouseRegion(event->pos().x(), event->pos().y());
	}
}

void CMainWindow::SetMouseCursor(int x, int y)
{	//设置鼠标样式，主要是缩放的时候鼠标指针的样式
	Qt::CursorShape cursor;
	int region = GetMouseRegion(x, y);
	switch (region)
	{
	case kMousePositionLeftTop:
	case kMousePositionRightButtom:
		cursor = Qt::SizeFDiagCursor; break;
	case kMousePositionRightTop:
	case kMousePositionLeftButtom:
		cursor = Qt::SizeBDiagCursor; break;
	case kMousePositionLeft:
	case kMousePositionRight:
		cursor = Qt::SizeHorCursor; break;
	case kMousePositionTop:
	case kMousePositionButtom:
		cursor = Qt::SizeVerCursor; break;
	case kMousePositionMid:
		cursor = Qt::ArrowCursor; break;
	default:
		break;
	}
	setCursor(cursor);
}

int CMainWindow::GetMouseRegion(int x, int y)
{	//计算鼠标所在区域范围
	int region_x = 0, region_y = 0;
	if (x < kMouseRegionLeft)
	{
		region_x = 1;
	}
	else if (x > (width() - kMouseRegionRight))
	{
		region_x = 3;
	}
	else
	{
		region_x = 2;
	}
	if (y < kMouseRegionTop)
	{
		region_y = 1;
	}
	else if (y > (height() - kMouseRegionButtom))
	{
		region_y = 3;
	}
	else
	{
		region_y = 2;
	}
	return region_y * 10 + region_x;
}