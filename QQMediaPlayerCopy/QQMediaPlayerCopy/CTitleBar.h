/*

标题栏

*/

#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class CTitleBar : public QWidget
{
	Q_OBJECT

public:
	CTitleBar(QWidget *parent = Q_NULLPTR);
	~CTitleBar();

	void setFileNameLabelText(QString name);		//设置标题栏要显示的内容

private:
	void InitData();	//初始化数据
	void RestoreUI();	//保存UI

private:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

signals:
	void sig_close();						//关闭信号
	void sig_ShowFullFcreen();				//全屏信号
	void sig_showMiniMode();  // 迷你小窗显示	//小窗口信号
	void sig_openfile(const QStringList& fileList);	//打开文件
	void sig_openUrl(const QString& url);	//打开Url

private slots:
	void onClicked();
	void openFile();
	void openFloder();
	void openNetStream();
	void about();
	void exit();

private:
	QPushButton* m_pLogoBtn;			//Logo按钮
	QLabel* m_pFileNameLabel;			//显示文件名的label
	QPushButton* m_pMinimodeBtn;		//迷你模式按钮  
	QPushButton* m_pSettopBtn;  //置顶	//使窗口显示在最顶层
	QPushButton* m_pMinBtn;				//最小化		（这三个都是自己实现的，窗口自带那个在边框上，隐藏掉了）
	QPushButton* m_pMaxBtn;				//最大化
	QPushButton* m_pCloseBtn;			//关闭按钮
};
