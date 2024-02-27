/*

音量调节按钮

功能：
	1. 鼠标悬浮到音量时显示slider dialog
	2. 点击时mute

*/

#pragma once

#include <QPushButton>
#include "CVolumeSliderDialog.h"


class CVolumeButton : public QPushButton
{
	Q_OBJECT

public:
	CVolumeButton(QWidget* parent);
	~CVolumeButton();

	bool getMute() const
	{
		return m_isMute;
	}

	void setMute(bool mute) { m_isMute = mute; }		//点击时设置音量，分别是满 半 空

signals:
	void sig_VolumeValue(int value);

protected:
	void paintEvent(QPaintEvent* event) override;	//绘画事件 画样式表
	void enterEvent(QEnterEvent* event) override;	//重写进入事件，达到效果：鼠标进入时显示音量条
	//void leaveEvent(QEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;	//鼠标按下事件 点击调节音量
	void timerEvent(QTimerEvent* event) override;		//重写时间事件 定时隐藏

private:
	bool m_isMute = false;  //是否静音
	CVolumeSliderDialog* m_pVolumeSliderDlg = nullptr;
};
