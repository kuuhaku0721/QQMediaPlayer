#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "CPlayButtonGroupWidget.h"
#include "CTimeSlider.h"

class CPlayCtrlBar : public QWidget
{
	Q_OBJECT

public:
	CPlayCtrlBar(QWidget *parent = Q_NULLPTR);
	~CPlayCtrlBar();

	void showTimeLabel(bool isShow);			//显示播放时长
	void setCurPlayTime(const QString& curPlaytime); //设置播放时间进度

	void setTimeSliderRange(qint64 value);  //设置进度条范围
	void setSliderValue(qint64 value); //设置进度条的值，根据视频播放进度转换
	void showSlider(bool flag); //显示进度条

private:
	void InitData();
	void RestoreUI();

private slots:
	void onSliderMoved(int position);

signals:
	void sig_play();
	void sig_stop();
	void sig_fullScreen();
	void sig_playRate(double rate);
	void sig_SliderMove(int value);

private:
	void resizeEvent(QResizeEvent* event) override;
	bool eventFilter(QObject* watched, QEvent* event) override;

private:
	CTimeSlider* m_pTimeSlider = nullptr;		//进度条控件（进度条和底部的控制栏是一体的，跟播放窗口是分离的）
	QLabel* m_pCurPlayTimeLabel = nullptr;      //左下角当前播放时长
	CPlayButtonGroupWidget* m_pPlayButtonGroupWidget = nullptr;  //控制播放的一组按钮（上一个  开始暂停  下一个）
	QPushButton* m_pSpeedButton = nullptr;		//播放速率
	QPushButton* m_pFullscreenButton = nullptr;	//全屏按钮
};
