#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QFileDialog>
#include<QString>
#include<QTextStream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("2024112643林杰-数据模型");


    // 初始化部分
    model = new QStandardItemModel(3,FixedColumnCount,this);  // 数据模型初始化
    selection = new QItemSelectionModel(model);               // Item选择模型

    // 为TableView设置数据模型
    ui->tableView->setModel(model);               // 设置数据模型
    ui->tableView->setSelectionModel(selection);  // 设置选择模型

    // 默认禁用所有Action选项,只保留打开
    ui->actionSave->setEnabled(false);
    ui->actionView->setEnabled(false);
    ui->actionAppend->setEnabled(false);
    ui->actionDelete->setEnabled(false);
    ui->actionInsert->setEnabled(false);

    // 创建状态栏组件,主要来显示单元格位置
    LabCurFile = new QLabel("当前文件：",this);
    LabCurFile->setMinimumWidth(200);

    LabCellPos = new QLabel("当前单元格：",this);
    LabCellPos->setMinimumWidth(180);
    LabCellPos->setAlignment(Qt::AlignHCenter);

    LabCellText = new QLabel("单元格内容：",this);
    LabCellText->setMinimumWidth(150);

    ui->statusBar->addWidget(LabCurFile);
    ui->statusBar->addWidget(LabCellPos);
    ui->statusBar->addWidget(LabCellText);

    // 选择当前单元格变化时的信号与槽
    connect(selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(on_currentChanged(QModelIndex,QModelIndex)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
   Q_UNUSED(previous);

    if (current.isValid())                                        // 当前模型索引有效
    {
        LabCellPos->setText(QString::asprintf("当前单元格：%d行，%d列",current.row(),current.column())); // 显示模型索引的行和列号
        QStandardItem   *aItem;
        aItem=model->itemFromIndex(current);                      // 从模型索引获得Item
        this->LabCellText->setText("单元格内容："+aItem->text());   // 显示item的文字内容
    }
}


void MainWindow::on_actionOpen_triggered()
{
    QString curPath=QCoreApplication::applicationDirPath(); // 获取应用程序的路径
    // 调用打开文件对话框打开一个文件
    QString aFileName=QFileDialog::getOpenFileName(this,"打开一个文件",curPath,"数据文件(*.txt);;所有文件(*.*)");
    if (aFileName.isEmpty())
    {
        return; // 如果未选择文件则退出
    }

    QStringList fFileContent;                              // 文件内容字符串列表
    QFile aFile(aFileName);                                // 以文件方式读出
    if (aFile.open(QIODevice::ReadOnly | QIODevice::Text)) // 以只读文本方式打开文件
    {
        QTextStream aStream(&aFile);       // 用文本流读取文件
        ui->plainTextEdit->clear();        // 清空列表
        // 循环读取只要不为空
        while (!aStream.atEnd())
        {
            QString str=aStream.readLine();          // 读取文件的一行
            ui->plainTextEdit->appendPlainText(str); // 添加到文本框显示
            fFileContent.append(str);                // 添加到StringList
        }
        aFile.close();                               // 关闭文件

        iniModelFromStringList(fFileContent);        // 从StringList的内容初始化数据模型
    }

    // 打开文件完成后,就可以将Action全部开启了
    ui->actionSave->setEnabled(true);
    ui->actionView->setEnabled(true);
    ui->actionAppend->setEnabled(true);
    ui->actionDelete->setEnabled(true);
    ui->actionInsert->setEnabled(true);

    // 打开文件成功后,设置状态栏当前文件列
    this->LabCurFile->setText("当前文件："+aFileName);//状态栏显示
}

void MainWindow::iniModelFromStringList(QStringList& aFileContent)
{
    int rowCnt=aFileContent.count();     // 文本行数,第1行是标题
    model->setRowCount(rowCnt-1);        // 实际数据行数,要在标题上减去1

    // 设置表头
    QString header=aFileContent.at(0);                // 第1行是表头

    // 一个或多个空格、TAB等分隔符隔开的字符串、分解为一个StringList
    QStringList headerList=header.split(QRegExp("\\s+"),QString::SkipEmptyParts);
    model->setHorizontalHeaderLabels(headerList);    // 设置表头文字

    // 设置表格中的数据
    int x = 0,y = 0;
    QStandardItem *Item;

    // 有多少列数据就循环多少次
    for(x=1; x < rowCnt; x++)
    {
        QString LineText = aFileContent.at(x);    // 获取数据区的一行
        // 一个或多个空格、TAB等分隔符隔开的字符串、分解为一个StringList
        QStringList tmpList=LineText.split(QRegExp("\\s+"),QString::SkipEmptyParts);

        // 循环列数,也就是循环FixedColumnCount,其中tmpList中的内容也是.
        for(y=0; y < FixedColumnCount-1; y++)
        {
            Item = new QStandardItem(tmpList.at(y)); // 创建item
            model->setItem(x-1,y,Item);              // 为模型的某个行列位置设置Item
        }

        // 最后一个数据需要取出来判断,并单独设置状态
        Item=new QStandardItem(headerList.at(y));   // 最后一列是Checkable,需要设置
        Item->setCheckable(true);                   // 设置为Checkable

        // 判断最后一个数值是否为0
        if (tmpList.at(y) == "0")
            Item->setCheckState(Qt::Unchecked);     // 根据数据设置check状态
        else
            Item->setCheckState(Qt::Checked);

        model->setItem(x-1,y,Item);                 // 为模型的某个行列位置设置Item
    }
}

void MainWindow::on_actionSave_triggered()
{
    QString curPath=QCoreApplication::applicationDirPath(); // 获取应用程序的路径

    // 调用打开文件对话框选择一个文件
    QString aFileName=QFileDialog::getSaveFileName(this,tr("选择一个文件"),curPath,"数据文件(*.txt);;所有文件(*.*)");

    if (aFileName.isEmpty()) // 未选择文件则直接退出
    {
        return;
    }

    QFile aFile(aFileName);

    // 以读写、覆盖原有内容方式打开文件
    if (!(aFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)))
        return;

    QTextStream aStream(&aFile);    // 用文本流读取文件
    QStandardItem *Item;
    QString str;
    int x = 0,y = 0;

    ui->plainTextEdit->clear();

    // 获取表头文字
    for (x=0; x<model->columnCount(); x++)
    {
        Item=model->horizontalHeaderItem(x);     // 获取表头的项数据
        str= str + Item->text() + "\t\t";        // 以TAB制表符隔开
    }
    aStream << str << "\n";                      // 文件里需要加入换行符\n
    ui->plainTextEdit->appendPlainText(str);

    // 获取数据区文字
    for ( x=0; x < model->rowCount(); x++)
    {
        str = "";
        for( y=0; y < model->columnCount()-1; y++)
        {
            Item=model->item(x,y);
            str=str + Item->text() + QString::asprintf("\t\t");
        }

        // 对最后一列需要转换一下,如果判断为选中则写1否则写0
        Item=model->item(x,y);
        if (Item->checkState()==Qt::Checked)
        {
            str= str + "1";
        }
        else
        {
            str= str + "0";
        }

         ui->plainTextEdit->appendPlainText(str);
         aStream << str << "\n";
    }

}

void MainWindow::on_actionDelete_triggered()
{

    QModelIndex curIndex = selection->currentIndex();  // 获取当前选择单元格的模型索引

    // 先判断是不是最后一行
    if (curIndex.row()==model->rowCount()-1)
    {
        model->removeRow(curIndex.row());    // 删除最后一行
    }
    else
    {
        model->removeRow(curIndex.row());    // 删除一行，并重新设置当前选择行
        selection->setCurrentIndex(curIndex,QItemSelectionModel::Select);
    }
}

void MainWindow::on_pushButton_clicked()
{
    if (!selection->hasSelection())
    {
        return;
    }

    QModelIndexList selectedIndex=selection->selectedIndexes();

    QModelIndex Index;
    QStandardItem *Item;

    for (int i=0; i<selectedIndex.count(); i++)
    {
        Index=selectedIndex.at(i);
        Item=model->itemFromIndex(Index);
        Item->setTextAlignment(Qt::AlignHCenter);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    // 没有选择的项
    if (!selection->hasSelection())
    {
        return;
    }

    // 获取选择的单元格的模型索引列表，可以是多选
    QModelIndexList selectedIndex=selection->selectedIndexes();

    for (int i=0;i<selectedIndex.count();i++)
    {
        QModelIndex aIndex=selectedIndex.at(i);             // 获取其中的一个模型索引
        QStandardItem* aItem=model->itemFromIndex(aIndex);  // 获取一个单元格的项数据对象
        aItem->setTextAlignment(Qt::AlignLeft);             // 设置文字对齐方式
    }

}

void MainWindow::on_pushButton_3_clicked()
{

    if (!selection->hasSelection())
    {
        return;
    }

    QModelIndexList selectedIndex=selection->selectedIndexes();

    QModelIndex aIndex;
    QStandardItem *aItem;

    for (int i=0;i<selectedIndex.count();i++)
    {
        aIndex=selectedIndex.at(i);
        aItem=model->itemFromIndex(aIndex);
        aItem->setTextAlignment(Qt::AlignRight);
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    if (!selection->hasSelection())
    {
        return;
    }

    // 获取选择单元格的模型索引列表
    QModelIndexList selectedIndex=selection->selectedIndexes();

    for (int i=0;i<selectedIndex.count();i++)
    {
        QModelIndex aIndex=selectedIndex.at(i);            // 获取一个模型索引
        QStandardItem* aItem=model->itemFromIndex(aIndex); // 获取项数据
        QFont font=aItem->font();                          // 获取字体
        font.setBold(true);                                // 设置字体是否粗体
        aItem->setFont(font);                              // 重新设置字体
    }
}

void MainWindow::on_actionAppend_triggered()
{
    QList<QStandardItem *> ItemList;   // 创建临时容器
    QStandardItem *Item;

    // 模拟添加一列的数据
    for(int x=0; x<FixedColumnCount-1; x++)
    {
        Item = new QStandardItem("测试(追加行)");    // 循环创建每一列
        ItemList << Item;                          // 添加到链表中
    }

    // 创建最后一个列元素,由于是选择框所以需要单独创建
    // 1.获取到最后一列的表头下标,最后下标为6
    QString str = model->headerData(model->columnCount()-1,Qt::Horizontal,Qt::DisplayRole).toString();

    Item=new QStandardItem(str); // 创建 "是否合格" 字段
    Item->setCheckable(true);    // 设置状态为真
    ItemList << Item;            // 最后一个选项追加进去

    model->insertRow(model->rowCount(),ItemList);                 // 插入一行，需要每个Cell的Item
    QModelIndex curIndex=model->index(model->rowCount()-1,0);     // 创建最后一行的ModelIndex
    selection->clearSelection();                                      // 清空当前选中项
    selection->setCurrentIndex(curIndex,QItemSelectionModel::Select); // 设置当前选中项为当前选择行

}

void MainWindow::on_actionView_triggered()
{

    ui->plainTextEdit->clear();  // 清空文本框

    QStandardItem *item;
    QString str;
    int x = 0, y = 0;

    // 获取表头文字
    for (y = 0; y < model->columnCount(); y++)
    {
        item = model->horizontalHeaderItem(y);  // 获取表头项
        str += item->text();
        // 如果不是最后一列，添加两个制表符分隔
        if (y < model->columnCount() - 1) {
            str += "\t\t";
        }
    }
    ui->plainTextEdit->appendPlainText(str);  // 添加表头到文本框

    // 获取数据区内容
    for (x = 0; x < model->rowCount(); x++)
    {
        str = "";  // 清空字符串
        for (y = 0; y < model->columnCount(); y++)
        {
            item = model->item(x, y);  // 获取单元格项

            if (y == model->columnCount() - 1)  // 最后一列（复选框列）
            {
                // 根据复选框状态添加0或1
                str += (item->checkState() == Qt::Checked) ? "1" : "0";
            }
            else  // 普通文本列
            {
                str += item->text();
                // 如果不是最后一列，添加两个制表符分隔
                if (y < model->columnCount() - 1) {
                    str += "\t\t";
                }
            }
        }
        ui->plainTextEdit->appendPlainText(str);  // 添加行数据到文本框
    }
}

void MainWindow::on_actionInsert_triggered()
{
    QList<QStandardItem *> ItemList;   // 创建临时容器
    QStandardItem *Item;

    int insertRow = 0;  // 默认插入位置为第0行（开头）

    // 获取当前选择行（如果有的话）
    QModelIndex curIndex = selection->currentIndex();
    if (curIndex.isValid()) {
        insertRow = curIndex.row();  // 在当前选择行上方插入
    }
    // 如果没有选择行，则插入到当前行数的位置（即末尾）
    else {
        insertRow = model->rowCount();
    }

    // 创建新行的数据项
    for(int x = 0; x < FixedColumnCount - 1; x++) {
        Item = new QStandardItem("测试(插入行)");  // 修改文本提示为插入行
        ItemList << Item;
    }

    // 创建最后一列（复选框列）
    QString header = model->headerData(model->columnCount() - 1, Qt::Horizontal, Qt::DisplayRole).toString();
    Item = new QStandardItem(header);
    Item->setCheckable(true);
    ItemList << Item;

    // 插入新行
    model->insertRow(insertRow, ItemList);  // 在指定位置插入行

    // 设置新插入的行为当前选择行
    QModelIndex newIndex = model->index(insertRow, 0);
    selection->clearSelection();
    selection->setCurrentIndex(newIndex, QItemSelectionModel::Select);
}
