/*

libvlc qt功能封装

*/

#pragma once

#ifdef _WIN32
#include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif

#include "vlc/vlc.h"
#include <QObject>
#include <vector>

using namespace std;

class CVlcKits : public QObject
{
    Q_OBJECT

public:
	CVlcKits();
	~CVlcKits();

    int initVLC();      //初始化vlc控制器

    libvlc_media_player_t* media_player();  //获取一个媒体播放器

    void setTimeSliderPos(int value);   //进度条位置
    void setVolumeSliderPos(int value); //音量控制
    void setTimeText(const QString& str);   //设置进度的文字显示
    libvlc_time_t getDuration(); //获取视频总时长 设置进度用的
    void setCurrentDuration(libvlc_time_t value); //设置当前进度，点击进度条之后跳转视频进度用的

    int play(QStringList fileList, void* hwnd);
    int play(const QString& url, void* hwnd);
    void play();
    void pause();
    void stop();

    vector<libvlc_time_t> getVecDuration(); //从播放列表播放复数个视频的时候用
    int getCurrentIndex();
    void addCurrentIndex();

    void setVideoPostion(int value);

    void setPlayRate(double rate);

signals:
    void sig_TimeSliderPos(const int& value);       //更新进度条信号
    void sig_VolumeSliderPos(const int& value);     //更新音量信号
    void sig_UpdateTimeText(const QString& str);    //更新时间信号

private:
    libvlc_instance_t* m_pInstance = nullptr;           //vlc控制器的实例
    libvlc_media_player_t* m_pMediaPlayer = nullptr;    //媒体播放器
    libvlc_media_t* m_pMedia = nullptr;                 //媒体文件
    libvlc_event_manager_t* m_pEvent_manager = nullptr; //事件管理器

    libvlc_media_list_player_t* m_pMediaPlayerList;     //复数个媒体播放器，一个视频一个
    libvlc_media_list_t* m_medialist;                   //播放列表中的复数个媒体文件

    libvlc_time_t m_CurrentDuration = 0;                //当前进度时长
    vector<libvlc_time_t> m_vecDurations;               //复数个视频的时长

    int m_CurrentInex = 0;
};

