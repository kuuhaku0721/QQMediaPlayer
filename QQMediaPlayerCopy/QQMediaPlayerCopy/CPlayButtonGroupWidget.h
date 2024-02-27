#pragma once

#include <QWidget>
#include <QPushButton>
#include "CVolumeButton.h"

class CPlayButtonGroupWidget : public QWidget	//播放控制的一组按钮，自定义按钮
{
	Q_OBJECT

public:
	CPlayButtonGroupWidget(QWidget *parent = Q_NULLPTR);
	~CPlayButtonGroupWidget();

signals:
	void sig_play();
	void sig_stop();
	void sig_next();
	void sig_pre();

private:
	QPushButton* m_pStopButton = nullptr;	//停止
	QPushButton* m_pPreButton = nullptr;	//上一个
	QPushButton* m_pPlayButton = nullptr;	//播放
	QPushButton* m_pNextButton = nullptr;	//下一个
	CVolumeButton* m_pVolumeButton = nullptr;//音量按钮
};
