#pragma once
#include <QWidget>
#include <QPushButton>
#include "COpenFileButton.h"
#include <QSlider>


class CVideoWidgetTopWidget : public QWidget
{
	Q_OBJECT

public:
	CVideoWidgetTopWidget(QWidget* p = nullptr);
	~CVideoWidgetTopWidget();

	void showRightBtn(bool flag);
	void showOpenFileBtn(bool flag);
	
	void setPlayStatus(bool play) { m_isPlay = play; }

protected:
	void resizeEvent(QResizeEvent* event) override;

signals:
	void sig_OpenFile(const QStringList& fileList);
	void sig_OpenFloder(QString path);
	void sig_OpenUrl(const QString& url);
	void sig_OpenPlaylist();

private:
	QPushButton* m_pOpenrRightlistButton = nullptr;	//打开右侧列表的按钮，就是那个小箭头
	COpenFileButton* m_pOpenFileButton = nullptr;
	//CTimeSlider* m_pTimeSlider = nullptr;
	int m_bSliderPressed = false;//是否点击进度条状态
	bool m_isPlay = false; //播放状态
}; 

