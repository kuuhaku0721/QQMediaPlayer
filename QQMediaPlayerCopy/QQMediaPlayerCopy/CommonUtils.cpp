#include "CommonUtils.h"
#include <QSettings>


CommonUtils::CommonUtils()
{
}

CommonUtils::~CommonUtils()
{
}

//写配置文件
void CommonUtils::writeIni(QString inipath, QString keystr, QString value)
{
	QSettings *configIniWrite = new QSettings(inipath, QSettings::IniFormat); //ini配置文件格式
	configIniWrite->setValue(keystr, value);

	//写入完成后删除指针
	delete configIniWrite;
}

//读配置文件
QString CommonUtils::readIni(QString inipath, QString keystr)
{
	QSettings *configIniRead = new QSettings(inipath, QSettings::IniFormat);
	QString ipResult = configIniRead->value(keystr).toString();

	//读入入完成后删除指针
	delete configIniRead;

	return ipResult;
}