/*

视频显示窗口类

*/


#pragma once

#include <QWidget>
#include <QPushButton>
#include <QAbstractNativeEventFilter>
#include "CVideoWidgetTopWidget.h"

//如果需要处理window_...消息，则需要使用QAbstractNativeEventFilter接口类 然后重写nativeEventFilter函数
class VideoWidget : public QWidget, public QAbstractNativeEventFilter
{
	Q_OBJECT

public:
	VideoWidget(QWidget* p = NULL);
	~VideoWidget();

	void showTopWidget(bool show);  //显示顶层Widget 就是居于顶层显示的'打开文件'按钮
	void setPlayStatus(bool play);  //设置播放状态
	void showTopWidgetOpenfilesButton(bool show);  //显示打开文件按钮(分开写是因为Widget中包含了按钮和小箭头，开始播放之后按钮隐藏，但是小箭头不隐藏)

protected:
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
	bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
#else
	bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;
#endif
	void resizeEvent(QResizeEvent* event) override;

signals:
	void sig_OpenFile(const QStringList& fileList);
	void sig_OpenFloder(QString path);
	void sig_OpenPlaylist();
	void sig_Totalms(const qint64& duration);			//发给主界面  总时长(涉及到底部文字显示)
	void sig_VideoPositionChanged(const qint64& pos);   //发给主界面  视频播放位置改变
	void sig_SliderMoved(int value);  //移动进度条
	void sig_OpenUrl(const QString& url);

private:
	QPoint m_dPos;   //VideoWidget相对于父窗口的位置   关系到全屏正常以及迷你模式时播放窗口的位置
	CVideoWidgetTopWidget* m_pTopWidget = nullptr;
	bool m_isPlay = false;
};
