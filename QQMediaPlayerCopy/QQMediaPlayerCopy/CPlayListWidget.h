#pragma once

#include <QWidget>
#include <QListView>
#include <QStringListModel>

class CPlayListWidget : public QWidget		//播放列表窗口
{
	Q_OBJECT

public:
	CPlayListWidget(QWidget* parent = Q_NULLPTR);
	~CPlayListWidget();

	void setPlayList(QStringListModel* listModel);
	QStringList getPlaylist() const;
	void setPlay(bool play) { m_isPlay = play; }
	bool getPlay() const { return m_isPlay; }

private slots:
	void on_Item_doubleClicked(const QModelIndex& index);   //双击选择列表中的某一项开始播放

signals:
	void sig_doubleClickFileName(const QString& fileName);

private:
	QListView* m_pListView = nullptr;				//List视图
	QStringListModel* m_pStringListModel = nullptr; //List模型
	bool m_isPlay = false;
};


