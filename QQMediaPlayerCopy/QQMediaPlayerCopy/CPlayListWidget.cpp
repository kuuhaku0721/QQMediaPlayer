#include "CPlayListWidget.h"
#include <QVBoxLayout>

CPlayListWidget::CPlayListWidget(QWidget* parent) : QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint);  //自然，这个列表也是个窗口 为了显示效果也要隐藏掉没必要的东西
	this->setFixedWidth(400); //固定宽度400px

	QVBoxLayout* pVLay = new QVBoxLayout(this);
	
	m_pListView = new QListView(this);
	m_pListView->setFixedWidth(390);
	
	//设置不可编辑
	m_pListView->setEditTriggers(QListView::EditTrigger::NoEditTriggers);

	QString qss = "QListView{background-color:rgb(64,74,84);show-decoration-selected:1;border:none;color:#FFFFFF;outline:none} \
		QListView::item{padding-top:12px;padding-bottom:12px;} \
		QListView::item:alternate{background:#EEEEEE;} \
		QListView::item:selected{border:1px solid #6a6ea9;} \
		QListView::item::selected:!active{background:qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6a6ea9, stop:1 #888dd9);} \
		QListView::item::hover{background:qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #000000, stop:1 #FFFFFF);}";
	m_pListView->setStyleSheet(qss);

	pVLay->addWidget(m_pListView);  //用pVLay这个布局是为了让List在它所在的控件中能有一个规整的样式
	pVLay->setContentsMargins(0, 0, 0, 0);

	setLayout(pVLay);
	//只有一个信号槽，那就是双击开始播放，列表上的东西将会由别的控件传入
	connect(m_pListView, &QListView::doubleClicked, this, &CPlayListWidget::on_Item_doubleClicked); 
	//最终的功能全都汇总到了CMainWindow中，有其他控件发出信号，在CMainWindow中绑定槽函数，当发出信号后就调用CMainWindow中的槽函数去执行
}

CPlayListWidget::~CPlayListWidget()
{
}

void CPlayListWidget::setPlayList(QStringListModel* listModel)
{
	m_pStringListModel = listModel;
	m_pListView->setModel(listModel);
}

QStringList CPlayListWidget::getPlaylist() const
{
	return m_pStringListModel->stringList();  //获取数据模型的StringList
}

void CPlayListWidget::on_Item_doubleClicked(const QModelIndex& index)
{
	QString itemValue = m_pStringListModel->data(index).toString();
	emit sig_doubleClickFileName(itemValue);		//这个信号绑定到了MainWindow上的OnPlay 即 开始播放
}
