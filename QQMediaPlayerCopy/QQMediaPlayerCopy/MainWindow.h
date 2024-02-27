
#pragma once

#include <QtWidgets/QWidget>
#include "CTitleBar.h"          //标题栏
#include "CPlayCtrlBar.h"       //播放控制栏(开始、暂停等)
#include "VideoWidget.h"        //视频播放窗口（只负责划分区域，没有播放功能）
#include <queue>
#include <list>
#include <mutex>
#include <Windows.h>
#include "CPlayListWidget.h"    //播放列表
#include "CVlcKits.h"           //VLC封装模块
#include <memory>

namespace {
    const int kMouseRegionLeft = 5;
    const int kMouseRegionTop = 5;
    const int kMouseRegionButtom = 5;
    const int kMouseRegionRight = 5;
}

//鼠标区域,将窗口分成了9个部分   就是鼠标放在四个角的时候的显示方式
enum MousePosition
{
    kMousePositionLeftTop = 11,
    kMousePositionTop = 12,
    kMousePositionRightTop = 13,
    kMousePositionLeft = 21,
    kMousePositionMid = 22,
    kMousePositionRight = 23,
    kMousePositionLeftButtom = 31,
    kMousePositionButtom = 32,
    kMousePositionRightButtom = 33,
};

class CMainWindow : public QWidget
{
    Q_OBJECT        //反正这就是个Qt项目，所以Qt有的都得有 想用信号和槽，就得有Q_OBJECT

public:
    CMainWindow(QWidget *parent = Q_NULLPTR);

private:
    void InitUI();

protected:
	void resizeEvent(QResizeEvent* event) override;         //一些重写的函数，控制窗口大小，键盘按下以及鼠标事件
	void keyPressEvent(QKeyEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
	
protected slots: 
    void On_Close();                                    //关闭信号
    void On_ShowFullScreen();                           //全屏信号
    void On_ShowMiniMode();                             //最小化信号
	void on_openFile(const QStringList& fileList);      //打开文件信号
    void on_openUrl(const QString& url);                //打开Url信号
    void On_openRightPlaylist();                        //打开播放列表信号
    void On_timer();                                    //计时器(部分控件定时隐藏)
    void OnUpdateTimeText(const QString& str);          //更新时间显示(与播放状态相关)
    void OnUpdateTimeSlider(const int& value);          //更新进度条(与播放状态相关)
    void OnSetPlayRate(double rate);                    //设置播放倍率
    void OnPlay(const QString& fileName);               //播放信号

private:
    //根据鼠标的设置鼠标样式，用于拉伸
    void SetMouseCursor(int x, int y);

    //判断鼠标的区域，用于拉伸
    int GetMouseRegion(int x, int y);  //与上面枚举划分的区域相关

private:
    QPoint last_point_;                             //记录放大之前的位置
    QPoint last_position_;                          //窗口上一次的位置
    bool left_button_pressed_ = false;              //鼠标左键按下
    int mouse_press_region_ = kMousePositionMid;    //鼠标点击的区域，默认在中间区域
    bool  mouse_moved_ = false;

	CTitleBar*  m_pTitleBar = nullptr;              //标题栏
	CPlayCtrlBar*  m_pPlayCtrlBar = nullptr;        //播放控制栏
	VideoWidget*  m_pVideoWidget = nullptr;         //视频播放窗口
	CPlayListWidget* m_pPlaylistWidget = nullptr;   //播放列表
	
    bool  m_bPlaylistIsOpen = false;
	QStringListModel* m_pStringListModel = nullptr;
	std::string m_curPlayFileName;

	std::unique_ptr<CVlcKits> m_pVlc;               //vlc播放器 这里有一个设计要点：当使用了unique_ptr之类的时候，需要把构造函数和Init函数分开实现
	bool m_isPlay = false;                          //              这样，就算初始化途中出错了也可以有的处理，但如果都在构造函数中，出错了没法处理
    bool m_isMiniMode = false;
    QTimer* m_pTimer = nullptr;
    QPoint mouse_last_moved_pos_;
};
