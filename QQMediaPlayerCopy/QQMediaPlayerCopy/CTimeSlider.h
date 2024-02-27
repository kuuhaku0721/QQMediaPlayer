#pragma once

#include <QSlider>

class CTimeSlider : public QSlider
{
	Q_OBJECT

public:
	CTimeSlider(QWidget* p = nullptr);
	~CTimeSlider();

private:
	bool eventFilter(QObject* watched, QEvent* event) override;

private:
	void slider_mouseLButtonPress(QObject* slider, QEvent* event); //鼠标拖着进度条移动
	void setTimeSliderRange(qint64 value);	//设置进度条范围
	void setSliderValue(qint64 value); //设置进度条的值

private slots:
	void onSliderMoved(int position); //进度条移动的槽函数(这样响应快)

signals:
	void sig_SliderMove(int value);

private:
	int m_bSliderPressed = false;
	int m_SliderHeight = 15;
};

