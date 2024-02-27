#pragma once

#include <QDialog>
#include "ui_CNetStreamDlg.h"

class CNetStreamDlg : public QDialog
{
	Q_OBJECT

public:
	CNetStreamDlg(QWidget *parent = Q_NULLPTR);
	~CNetStreamDlg();

	QString getUrl() const;		//打开网络流的对话框，就一个函数，负责获取页面上输入的Url信息，然后就交给别的类去处理了

private slots:
	void on_btnStartPlay_clicked();

private:
	Ui::CNetStreamDlg ui;

	QString m_url;
};
