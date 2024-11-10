//myTreeWidget.h file
#include <QtGui>

class myTreeWidget:public QTreeWidget
{
  Q_OBJECT
  public:
    myTreeWidget(QWidget* parent):
    QTreeWidget(parent)
    {
      //Add Subdirectories as children 	when user clicks on a file item,
      //otherwise adding all children recursively may consume HUGE amount of memory
      connect(this , SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(showDirectory(QTreeWidgetItem*,int)));
    };
    ~myTreeWidget(){};
  
    
  public slots:
  void showDirectory(QTreeWidgetItem* item, int /*column*/)
  {
	QDir* rootDir = new QDir(item->text(2));
	QFileInfoList filesList = rootDir->entryInfoList();	  
	
	foreach(QFileInfo fileInfo, filesList)
	{
	    QTreeWidgetItem* child = new QTreeWidgetItem();
	    child->setText(0,fileInfo.fileName());	  
	    
	    if(fileInfo.isFile())
	    {  
	      child->setText(1,QString::number(fileInfo.size()));
	      child->setIcon(0,*(new QIcon("file.jpg")));
	    }
	    
	    if(fileInfo.isDir())
	    {
	      child->setIcon(0,*(new QIcon("folder.jpg")));
	      child->setText(2,fileInfo.filePath());
	    } 
	    
	    item->addChild(child);
	    
	    resizeColumnToContents(0);
	}
  };  
    
};
