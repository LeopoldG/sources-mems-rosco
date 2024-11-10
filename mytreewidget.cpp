#include <QtGui>
#include "myTreeWidget.h"

//Forward Declaration of addChildren Method
void addChildren(QTreeWidgetItem* ,QString);

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	QMainWindow *window = new QMainWindow();    
    
        window->setWindowTitle(QString::fromUtf8("QTreeWidget List Directory"));
        window->resize(400, 400);
	
	
        QWidget *centralWidget = new QWidget(window);
	myTreeWidget  *tree = new myTreeWidget (centralWidget);
	tree->setFixedSize(395,395);

	
	//Set QTreeWidget Column Header
	QTreeWidgetItem* headerItem = new QTreeWidgetItem();
	headerItem->setText(0,QString("File Name"));
	headerItem->setText(1,QString("Size (Bytes)"));
	headerItem->setText(2,QString("Path"));
	tree->setHeaderItem(headerItem);
	
	
	QDir* rootDir = new QDir("/");
	QFileInfoList filesList = rootDir->entryInfoList();
	
	
	foreach(QFileInfo fileInfo, filesList)
	{
	  QTreeWidgetItem* item = new QTreeWidgetItem();
	  item->setText(0,fileInfo.fileName());
	  
	  if(fileInfo.isFile())
	  {  
	    item->setText(1,QString::number(fileInfo.size()));
	    item->setIcon(0,*(new QIcon("file.jpg")));
	  }
	  
	  if(fileInfo.isDir())
	  {
	    item->setIcon(0,*(new QIcon("folder.jpg")));
	    addChildren(item,fileInfo.filePath());
	  } 
	  
	  item->setText(2,fileInfo.filePath());
	  tree->addTopLevelItem(item);	  
	}	
	
	window->setCentralWidget(centralWidget);
	window->show();
	return app.exec();
}

void addChildren(QTreeWidgetItem* item,QString filePath)
{
	QDir* rootDir = new QDir(filePath);
	QFileInfoList filesList = rootDir->entryInfoList();	  
	
	foreach(QFileInfo fileInfo, filesList)
	{
	    QTreeWidgetItem* child = new QTreeWidgetItem();
	    child->setText(0,fileInfo.fileName());
	    
	    
	    if(fileInfo.isFile())
	    {  
	      child->setText(1,QString::number(fileInfo.size()));
	    }
	    
	    if(fileInfo.isDir())
	    {
	      child->setIcon(0,*(new QIcon("folder.jpg")));
	      child->setText(2,fileInfo.filePath());
	    }  
	    
	    item->addChild(child);
	}
}
