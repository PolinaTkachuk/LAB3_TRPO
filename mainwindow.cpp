#include "mainwindow.h"
#include <QSplitter>
#include <QListView>
#include <QTreeView>
#include <QTextEdit>
#include <QFileSystemModel>
#include <QItemSelectionModel>
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QStatusBar>
#include <QDebug>
#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCore/QTime>
#include <QtCharts/QBarCategoryAxis>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
      //QMainWindow(parent)
{
    //Устанавливаем размер главного окна
    //this->resize(1500,500);
    //this->setGeometry(100, 100, 1500, 500);
    setWindowTitle("Laba3"); // заголовк окна
    resize(1500, 500); // стандартный размер
    //this->setStatusBar(new QStatusBar(this));



    //this->statusBar()->showMessage("Choosen Path: ");
    QString homePath = QDir::homePath();
    // Определим  файловой системы:
    dirModel =  new QFileSystemModel(this);
    dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirModel->setRootPath(homePath);

    fileModel = new QFileSystemModel(this);
    fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    fileModel->setRootPath(homePath);


    //Показать как дерево, пользуясь готовым видом:


    label_path = new QLabel("Choosen Path: ");

    label = new QLabel("Выберите тип диаграммы");

    combobox = new QComboBox();
    //combobox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    btnPrint = new QPushButton("Печать графика");
    //btnPrint->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    checkbox = new QCheckBox("Черно-белый график");
    //checkbox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(label);
    layout->addWidget(combobox);
    layout->addWidget(checkbox);
    layout->addWidget(btnPrint);
    //layout->addStretch();
    layout->setAlignment(Qt::AlignCenter);
    //setLayout(layout);

    QSplitter *splitter = new QSplitter(parent);

    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // заполнять всё доступное пространство по вертикали

    treeView = new QTreeView(splitter);
    treeView->setModel(dirModel);
    treeView->expandAll();

    tableView = new QTableView(splitter);
    tableView->setModel(fileModel);

    treeView->setHeaderHidden(true); // скрываем заголовок дерева
    treeView->hideColumn(1); // также выключаем ненужные столбцы
    treeView->hideColumn(2);
    treeView->hideColumn(3);

    //1.Добавление диаграммы
    QChartView *chartView;
    //QChart *chartBar =  themeWidget->createBarChart(5);
    //chartView = new QChartView();


    splitter->setStretchFactor(0, 1); // устанавливаем начальное положение разделителя
    splitter->setStretchFactor(1, 2);

    //splitter->addWidget(treeView);
    //splitter->addWidget(chartView);
    //splitter->addWidget(tableView);
    //setCentralWidget(splitter);


    QVBoxLayout *layout_vertical = new QVBoxLayout(this);
    layout_vertical->addLayout(layout);
    layout_vertical->addWidget(splitter);
    layout_vertical->addWidget(label_path);
    //layout_vertical->addStretch();
    //setLayout(layout_vertical);

    QItemSelectionModel *selectionModel = treeView->selectionModel();
    QModelIndex rootIx = dirModel->index(0, 0, QModelIndex());//корневой элемент

    QModelIndex indexHomePath = dirModel->index(homePath);
    QFileInfo fileInfo = dirModel->fileInfo(indexHomePath);

    connect(combobox, &QComboBox::currentIndexChanged, this, &MainWindow::on_select_comboboxOnChangedSlot);
    combobox->addItem("Bar Chart");
    combobox->addItem("Pie Chart");

    /* Рассмотрим способы обхода содержимого папок на диске.
     * Предлагается вариант решения, которы может быть применен для более сложных задач.
     * Итак, если требуется выполнить анализ содержимого папки, то необходимо организовать обход содержимого. Обход выполняем относительно модельного индекса.
     * Например:*/
    if (fileInfo.isDir()) {
        /*
         * Если fileInfo папка то заходим в нее, что бы просмотреть находящиеся в ней файлы.
         * Если нужно просмотреть все файлы, включая все вложенные папки, то нужно организовать рекурсивный обход.
        */
        QDir dir  = fileInfo.dir();

        if (dir.cd(fileInfo.fileName())) {
            /**
             * Если зашли в папку, то пройдемся по контейнеру QFileInfoList ,полученного методом entryInfoList,
             * */

            foreach (QFileInfo inf, dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Type)) {
                qDebug() << inf.fileName() << "---" << inf.size();
            }

            dir.cdUp();//выходим из папки
        }
    }

    QDir dir = fileInfo.dir();

    foreach (QFileInfo inf, dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::Type)) {

        qDebug() << inf.fileName() << "---" << inf.size();
    }


    treeView->header()->resizeSection(0, 200);
    //Выполняем соединения слота и сигнала который вызывается когда осуществляется выбор элемента в TreeView
    connect(selectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(on_selectionChangedSlot(const QItemSelection &, const QItemSelection &)));
    //Пример организации установки курсора в TreeView относит ельно модельного индекса
    QItemSelection toggleSelection;
    QModelIndex topLeft;
    topLeft = dirModel->index(homePath);
    dirModel->setRootPath(homePath);

    toggleSelection.select(topLeft, topLeft);
    selectionModel->select(toggleSelection, QItemSelectionModel::Toggle);
}
//Слот для обработки выбора элемента в TreeView
//выбор осуществляется с помощью курсора

void MainWindow::on_selectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected)
{
    //Q_UNUSED(selected);
    Q_UNUSED(deselected);
    QModelIndex index = treeView->selectionModel()->currentIndex();
    QModelIndexList indexs =  selected.indexes();
    QString filePath = "";

    // Размещаем инфо в statusbar относительно выделенного модельного индекса

    if (indexs.count() >= 1) {
        QModelIndex ix =  indexs.constFirst();
        filePath = dirModel->filePath(ix);
        label_path->setText(filePath);
        //this->statusBar()->showMessage("Выбранный путь : " + dirModel->filePath(indexs.constFirst()));
    }

    //TODO: !!!!!
    /*
    Тут простейшая обработка ширины первого столбца относительно длины названия папки.
    Это для удобства, что бы при выборе папки имя полностью отображалась в  первом столбце.
    Требуется доработка(переработка).
    */
    int length = 200;
    int dx = 30;

    if (dirModel->fileName(index).length() * dx > length) {
        length = length + dirModel->fileName(index).length() * dx;
        qDebug() << "r = " << index.row() << "c = " << index.column() << dirModel->fileName(index) << dirModel->fileInfo(
                     index).size();

    }

    treeView->header()->resizeSection(index.column(), length + dirModel->fileName(index).length());
    tableView->setRootIndex(fileModel->setRootPath(filePath));
}

void MainWindow::on_select_comboboxOnChangedSlot(const int index)
{
    try
    {
        switch (index) // получаем из выбранного индекса тип отображения
        {
        case 0:
            // устанавливаем круговую диаграмму
            break;
        case 1:
            // устанавливаем вертикальную диаграмму
            break;
        default:
            throw std::runtime_error("Unknown display type selected."); // такой ситуации быть не должно, по этому сообщаем о ней
            break;
        }

    }
    catch (const std::runtime_error &e)
    {
        QMessageBox::about(this, "Error", e.what()); // сообщаем что в процессе выбора вида отображения что-то пошло не так
    }
}

MainWindow::~MainWindow()
{

}
