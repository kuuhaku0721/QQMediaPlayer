#include "CTitleBar.h"		//注意TitleBar继承的是Widget
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QMenu>
#include <QMouseEvent>
#include "qss.h"			//样式表统一管理
#include <Windows.h>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QFileDialog>
#include "CNetStreamDlg.h"

#include <qt_windows.h>
#pragma comment(lib, "user32.lib")


CTitleBar::CTitleBar(QWidget *parent)
	: QWidget(parent)
{
	this->setAttribute(Qt::WA_DeleteOnClose);	//setAttribute设置窗口属性，删掉关闭按钮
	setMouseTracking(true); //追踪鼠标事件
	InitData(); //初始化数据，单独做一个函数出来，以方便对错误进行处理
}

CTitleBar::~CTitleBar()
{
}

void CTitleBar::setFileNameLabelText(QString name)
{
	m_pFileNameLabel->show();		//显示当前播放的文件名
	m_pFileNameLabel->setText(name);
}

void CTitleBar::InitData()
{
	//禁止父窗口影响子窗口样式
	setAttribute(Qt::WA_StyledBackground);  //设置顶层窗口的属性：应该使用设置的样式表进行渲染控件背景  否则样式表不生效
	int w = 20;
	int h = 20;
	int margin = 5;
	this->setFixedHeight(h + margin * 2);
	this->setStyleSheet("background-color:rgb(54,54,54)");

	m_pLogoBtn = new QPushButton(this);   //设置Logo按钮
	m_pLogoBtn->setFixedWidth(120);
	m_pLogoBtn->setText(tr("Media Player"));
	m_pLogoBtn->setStyleSheet(logo_button_qss);  //样式表来自qss.h统一管理

	QMenu* pMenu = new QMenu(this);  //点击左上角的Logo之后显示的菜单栏
	pMenu->setStyleSheet(menu_qss);

	/// 以下四步骤是设置菜单栏的一般步骤 ///
	//1、声明五个Action行为
	QAction* pAc1 = new QAction(tr("open file"), this);
	QAction* pAc2 = new QAction(tr("open floder"), this);
	QAction* pAc3 = new QAction(tr("open net stream"), this);
	QAction* pAc4 = new QAction(tr("about"), this);
	QAction* pAc5 = new QAction(tr("exit"), this);
	//2、将行为添加到Menu菜单栏中
	pMenu->addAction(pAc1);
	pMenu->addAction(pAc2);
	pMenu->addAction(pAc3);
	pMenu->addAction(pAc4);
	pMenu->addAction(pAc5);
	//3、把菜单栏添加到按钮点击之后显示的地方
	m_pLogoBtn->setMenu(pMenu);
	//4、然后分别绑定这几个行为对应的槽函数
	connect(pAc1, &QAction::triggered, this, &CTitleBar::openFile);
	connect(pAc2, &QAction::triggered, this, &CTitleBar::openFloder);
	connect(pAc3, &QAction::triggered, this, &CTitleBar::openNetStream);
	connect(pAc4, &QAction::triggered, this, &CTitleBar::about);
	connect(pAc5, &QAction::triggered, this, &CTitleBar::exit);

	//设置文件标题的label标签，默认情况下隐藏，选择文件播放时再显示
	m_pFileNameLabel = new QLabel(this);
	m_pFileNameLabel->setAlignment(Qt::AlignCenter);
	m_pFileNameLabel->hide();  // 视频文件标题，默认隐藏
	m_pFileNameLabel->setStyleSheet("QLabel{font-family:Microsoft YaHei; \
		font-size:14px;color:#FFFFFF;background-color:#363636;}");

	//设置标题栏右侧的几个功能性按钮
	//设置迷你模式按钮
	m_pMinimodeBtn = new QPushButton(this);
	m_pMinimodeBtn->setObjectName("m_pMinimodeBtn");
	m_pMinimodeBtn->setText("");
	m_pMinimodeBtn->setFixedSize(w, h);
	m_pMinimodeBtn->setStyleSheet(minimode_qss);

	//设置窗口置顶按钮
	m_pSettopBtn = new QPushButton(this);
	m_pSettopBtn->setObjectName("m_pSettopBtn");
	m_pSettopBtn->setText("");
	m_pSettopBtn->setToolTip(tr("set to top window"));
	m_pSettopBtn->setFixedSize(w, h);
	m_pSettopBtn->setStyleSheet(settop_qss);
	
	//设置最小化按钮
	m_pMinBtn = new QPushButton(this);
	m_pMinBtn->setFixedSize(w, h);
	m_pMinBtn->setStyleSheet(min_qss);

	//设置最大化按钮
	m_pMaxBtn = new QPushButton(this);
	m_pMaxBtn->setObjectName("m_pMaxBtn");
	m_pMaxBtn->setText("");
	m_pMaxBtn->setFixedSize(w, h);
	m_pMaxBtn->setStyleSheet(max_normal_qss);

	//设置关闭按钮
	m_pCloseBtn = new QPushButton(this);
	m_pCloseBtn->setObjectName("m_pCloseBtn");
	m_pCloseBtn->setText("");
	m_pCloseBtn->setFixedSize(w, h);
	m_pCloseBtn->setStyleSheet(close_qss);

	//绑定这几个按钮的槽函数   这里槽函数只设置了一个，在槽函数内部去判断点击的是哪一个按钮，然后响应对应的事件
	connect(m_pMinBtn, &QPushButton::clicked, this, &CTitleBar::onClicked);
	connect(m_pMaxBtn, &QPushButton::clicked, this, &CTitleBar::onClicked);
	connect(m_pCloseBtn, &QPushButton::clicked, this, &CTitleBar::onClicked);
	connect(m_pMinimodeBtn, &QPushButton::clicked, this, &CTitleBar::onClicked);
	connect(m_pSettopBtn, &QPushButton::clicked, this, &CTitleBar::onClicked);
}

void CTitleBar::RestoreUI()
{	//设置每个按钮的位置，没啥技巧，一点点微调出来就行
	m_pLogoBtn->move(5, 5);

	int w = this->width();
	int h = this->height();

	m_pCloseBtn->move(w - 21, 5);
	m_pMaxBtn->move(w - 21 - 18 - 16, 5);
	m_pMinBtn->move(w - 21 - 34 * 2, 5);
	m_pSettopBtn->move(w - 21 - 34 * 3, 5);
	m_pMinimodeBtn->move(w - 21 - 34 * 4, 5);

	// 此处设置视频标题的宽度
	m_pFileNameLabel->setFixedWidth(w - 120 - 5 - 21 - 34 * 4 - 5);
	m_pFileNameLabel->move(120 + 5 + 5, 5);
}

void CTitleBar::mousePressEvent(QMouseEvent* event)
{
	if (ReleaseCapture())	//ReleaseCapture功能：释放鼠标捕获，还原正常鼠标功能
	{
		QWidget* pWindow = this->window();  //拿到当前窗口句柄
		if (pWindow->isTopLevel())
		{
			SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);	//TODO:这个东西的作用暂且未知，在复刻代码的时候注意一下
		}	//SendMessage:将指定消息发给一个或多个窗口
	}		//第一个参数是要发给的目标窗口 第二个参数为消息  后面俩为wParam和lParam，也就是其他特定信息
			//当用户从“ 窗口 ”菜单选择命令时，或者当用户选择“最大化”按钮、最小化按钮、还原按钮或关闭按钮时，窗口会收到此消息。

	event->ignore();
	//在默认情况下，按钮上的点击不会牵扯父亲窗口。
	//如果需要让父亲窗口也捕捉到鼠标点击事件，那么需要在mousePressEvent 末尾添加上event->ignore(); ，
	//这样m_accept 参量就会设置为false ，点击按钮时，会输出两条信息。 本按钮处理一次，父窗口处理一次
	//与ignore() 对应的是accept() ，功能相反，设置m_accept为true表示该组件想要该事件，不需要的事件可能会传播到父Widget。
	// 另外也可以等价地调用setAccepted(bool)来设置。
	//比如在复写closeEvent 时，注意如果调用event->ignore()，意味着关闭事件没有被处理，窗口不关闭；
	// 不调用（默认）或者显式调用了event->accept() 则代表窗口允许被关闭。

	//概括一下：如果使用了ignore表示把当前事件传递给父窗口继续执行 如果使用accept则代表当前窗口直接执行不再传递给父窗口 默认为accept
}

void CTitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	emit sig_ShowFullFcreen();
}

void CTitleBar::resizeEvent(QResizeEvent* event)  //重设大小的时候需要重新计算各个控件的相对位置
{
	RestoreUI();
}

void CTitleBar::onClicked()
{
	QPushButton* pButton = qobject_cast<QPushButton*>(sender());  //由sender函数能获取是哪个控件发出的信号，用于分辨控件

	QWidget* pWindow = this->window(); //当前窗口句柄，用来控制用的

	//下面的都能看懂，就不废话了
	if (pButton == m_pMinBtn)
	{
		pWindow->showMinimized();
	}
	else if (pButton == m_pMaxBtn)
	{
		if (pWindow->isMaximized())
		{
			pWindow->showNormal();
			m_pMaxBtn->setStyleSheet(max_normal_qss);
		}
		else
		{
			pWindow->showMaximized();
			m_pMaxBtn->setStyleSheet(maxed_qss);
		}
	}
	else if (pButton == m_pMinimodeBtn)
	{
		emit sig_showMiniMode();
	}
	else if (pButton == m_pSettopBtn)
	{	//设置为顶层窗口，就是设置窗口一直置顶显示
		HWND hwnd = (HWND)(pWindow->winId());
		if (::GetWindowLong(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST)
		{	//如果当前窗口已经是顶层，那就取消顶层，改变文字显示，反之一样
			// The window is topmost.
			::SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			m_pSettopBtn->setStyleSheet(settop_qss);
			m_pSettopBtn->setToolTip(tr("set to top window"));
		}
		else
		{	//窗口不是顶层，那就设置为顶层并改变文字显示
			// The window is not topmost.
			::SetWindowPos(HWND(pWindow->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			m_pSettopBtn->setStyleSheet(settoped_qss);
			m_pSettopBtn->setToolTip(tr("cancal top window"));
		}
	}
	else if (pButton == m_pCloseBtn)
	{
		emit sig_close();
	}
}

void CTitleBar::openFile()
{
	QString cfgPath = "HKEY_CURRENT_USER\\Software\\CMainWindow";			//TODO:so，这是啥？看不懂，得问
	
	QSettings settings(cfgPath, QSettings::NativeFormat);
	//QSettings类：简单理解为，能够与平台无关的读写配置文件  参数一是文件路径  参数二是配置格式
	//NativeFormat：最适合本地的格式
	
	QString lastPath = settings.value("openfile_path").toString();  // 从注册表获取路径

	if (lastPath.isEmpty())
	{
		lastPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);  //获取默认的'文档'路径 
	}

	//所以上面那么多步骤只是为了获取一下文件路径然后方便让QFileDialog打开而已，so，不至于这么麻烦，但是对于技术学习来说，还是有必要的

	//可以同时打开多个文件
	QStringList filelist = QFileDialog::getOpenFileNames(this, tr("select video file"), lastPath,
		tr("video file (*.flv *.rmvb *.avi *.mp4);; all files(*.*);; "));

	if (filelist.isEmpty())
	{	//没有选择文件直接返回
		return;
	}

	int end = filelist[0].lastIndexOf("/");
	QString tmppath = filelist[0].left(end + 1);
	settings.setValue("openfile_path", tmppath);  // 将当前打开的路径写入到注册表
	//这样如果第二次打开的话，注册表的路径就不是空了，就可以从上次打开的路径开始继续选择

	emit sig_openfile(filelist);
}

void CTitleBar::openFloder()
{
	QString cfgPath = "HKEY_CURRENT_USER\\Software\\CMainWindow";
	QSettings settings(cfgPath, QSettings::NativeFormat);
	QString lastPath = settings.value("openfile_path").toString();  // 从注册表获取路径

	if (lastPath.isEmpty())
	{
		lastPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);  //获取默认的文档路径 
	}

	QString path = QFileDialog::getExistingDirectory(this,
		tr("choose video directory"), lastPath);

	if (path.isEmpty())
	{
		return;
	}

	//以上的步骤都是一样的，区别在于下面，主要是文件和文件夹的区别

	QDir dir(path);

	QStringList nameFilters;
	nameFilters << "*.mp4" << "*.flv" << "*.rmvb" << "*.avi";

	//将过滤后的文件名称存入到files列表中
	QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);

	int _size = files.size();
	if (_size == 0)
	{
		QMessageBox::information(this, tr("Warn"), tr("this floder no video files"));
		return;
	}

	QStringList fileList;
	for (int i = 0; i < _size; i++)
	{
		QString fileName = path + "/" + files[i];  //拼出来完整路径
		fileList << fileName;
	}

	settings.setValue("openfile_path", path);  // 将当前打开的路径写入到注册表

	emit sig_openfile(fileList);
}

void CTitleBar::openNetStream()
{
	CNetStreamDlg dlg;
	//Dialog对话框的两种显示形式
	//open：显示模态窗口，会继续执行open之后的代码
	//exec：显示模态窗口，会一直阻塞，exec后面的代码会阻塞不会执行，直到用户关闭它，才会执行后面的代码
	if (dlg.exec() == QDialog::Accepted)
	{
		QString strText = dlg.getUrl();  //getUrl是自定义函数，作用就是获取文本框输入的文本（这个文本是个Url串）
		emit sig_openUrl(strText);
	}
}

void CTitleBar::about()
{
	QMessageBox::information(this, tr("tips"), tr("Qt libvlc mediaplayer"));
}

void CTitleBar::exit()
{
	::exit(0);
}
