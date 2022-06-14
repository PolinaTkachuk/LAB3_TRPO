#include "themewidget.h"
ThemeWidget::ThemeWidget(QWidget *parent) :
    QWidget(parent),
    m_listCount(3), //кол-во графиков
    m_valueMax(10), //максимальное значение
    m_valueCount(7),
    //new_dataTable(generateDataBase(QString& filePath) const),//графики строятся на основе данных бд
    typeChart(AddTypeCharts()),//задаем выпадающий список из разных типов графиков
    BlackWhiteCheck(new QCheckBox("Черно-белый график")),//задаем чекбокс
    printChart(new QPushButton("Печать графика")) //задаем кнопку печати
{
    connectSignals();
    // create layout
    QVBoxLayout *baseLayout = new QVBoxLayout();//вертикальная раскладка
    QHBoxLayout *settingsLayout = new QHBoxLayout(); //горизонтальная
    settingsLayout->addWidget(new QLabel("Выберите тип диаграммы:"));
    //добавление виджетов на раскладку
    settingsLayout->addWidget(typeChart);
    settingsLayout->addWidget(BlackWhiteCheck);
    settingsLayout->addWidget(printChart);
    settingsLayout->addStretch();
    baseLayout->addLayout(settingsLayout);

    //create chart

    //создаем график
    chartView = new QChartView(createAreaChart());

    //добавляем на раскладку горизонтальную
    baseLayout->addWidget(chartView);

    setLayout(baseLayout);

    // Set defaults Изначально- графики цветные чекбокс выключен
    BlackWhiteCheck->setChecked(true);
    updateUI();
}

ThemeWidget::~ThemeWidget()
{
}

void ThemeWidget::connectSignals()
{
   // сигналы- слоты с виджетами
    connect(typeChart,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &ThemeWidget::updateUI);
    connect(BlackWhiteCheck, &QCheckBox::toggled, this, &ThemeWidget::updateUI);
    //при нажатии на кнопку- печать графика
    connect(printChart, SIGNAL(clicked()), this, SLOT(openFileDialogWindow()));
}

void ThemeWidget::openFileDialogWindow()
{
    QFileDialog *fileDialog = new QFileDialog(this);
     // заголовок файла
    fileDialog-> setWindowTitle (tr ("Открыть изображение"));
    //путь к файлу определяем по умолчанию
    fileDialog->setDirectory(".");
             // Установить фильтр файлов, только файлы бд и json-данные
    fileDialog->setNameFilter(tr("Images(*.sqlite *.json)"));
 // Настройка позволяет выбрать несколько файлов, по умолчанию используется только один файл QFileDialog :: ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
             // Установить режим просмотра
    fileDialog->setViewMode(QFileDialog::Detail);
             // выводим путь ко всем выбранным файлам
    QStringList fileNames;
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
    }


    QPdfWriter writer_(fileNames[0] + ".pdf");//сохраняем в формате пдф

    writer_.setPageSize(QPageSize::A4);//размер страницы А4

    writer_.setResolution (300);//разрешение бумаги на 300, чтобы пиксель был 3508 * 2479
    // Добавляем контент с помощью QPainter
    QPainter painter(&writer_);

    //Отрисовка
    chart->ReturnchartView()->render(&painter);
    painter.end();



}

/*
DataTable ThemeWidget::generateRandomData(int listCount, int valueMax, int valueCount) const
{
    DataTable dataTable;

    // set seed for random stuff
    srand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

    // generate random data
    for (int i(0); i < listCount; i++) {
        DataList dataList;
        qreal yValue(0);
        for (int j(0); j < valueCount; j++) {
            yValue = yValue + (qreal)(rand() % valueMax) / (qreal) valueCount;
            QPointF value((j + (qreal) rand() / (qreal) RAND_MAX) * ((qreal) m_valueMax / (qreal) valueCount),
                          yValue);
            QString label = "Slice " + QString::number(i) + ":" + QString::number(j);
            dataList << Data(value, label);
        }
        dataTable << dataList;
    }


    return dataTable;
}
*/
        //график строиться на основе данных бд, передаем в аргументе путь
DataTable ThemeWidget::generateDataBase(QString& filePath) const
{
      //QSqlTableModel *model_db; //создаем модель бд
      QString name_ = filePath;
      DataList dataList;
      DataTable new_dataTable;

      //ДОБАВЛЕНИЕ БД ИСХОДНОЙ
      QSqlDatabase  db=QSqlDatabase::addDatabase("QSQLITE");//указываем тип бд
      db.setConnectOptions("QSQLITE_OPEN_READONLY=1");//Возвращает строку используемых опций соединения.
      db.setDatabaseName(filePath);//полное имя базы данных (зависит от того, на какую мы нажали- передали в аргументе)
      //db.setDatabaseName("./dbase.BLOOD_SUGAR.sqlite");

        if(db.open())//проверка на успешное открытие бд
        {
            qDebug() << " База данных успешно открылась\n";//выводим об успешносм открытие сообщение

            //чтобы работать с данными бд, необходимо в имени бд убрать все символы начиная с начала до слеша
            //а также после точки , то есть оставить только название.
  //remove ( int position, int n )Удаляет n символов из строки, начиная с позиции с индексом position,возвращает ссылку на строку.
            name_.remove(0,name_.lastIndexOf('/') + 1);
            int dif= name_.size() - name_.indexOf('.');
            name_.remove(name_.indexOf('.'),dif);


            //делаем селект к выбранной базе данных с корректным кратким  именем
            QSqlQuery query("SELECT VALUE, TIME FROM " + name_, db);
            int i= 0;
            while (query.next())//проходим по всем данным бд
            {
                //значение 1го столбца и 2го
                float tempVal = query.value(0).toFloat();
                QString tempDate = query.value(1).toString();
                //фиксируем для графика
                QPointF value(i, tempVal);

                //заносим в даталист
                dataList << Data(value, tempDate);

                //идем дальше
                i++;
            }

            new_dataTable << dataList;//храним данные в дататейбл
        }

        else
        {
            //"Ошибка подключения к БД: "
            qDebug() << db.lastError().text();
             //ui->statusbar->showMessage("Ошибка подключения к БД: "+ db.lastError().databaseText());
        }

   return new_dataTable;//возвращаем данные

}


QComboBox *ThemeWidget::AddTypeCharts() const
{
    // вид графиков для выпадающего списка
    QComboBox *themeComboBox = new QComboBox();
    themeComboBox->addItem("Area", TypeThemeWidget::Area);
    themeComboBox->addItem("Bar", TypeThemeWidget::Bar);
    themeComboBox->addItem("Line", TypeThemeWidget::Line);
    themeComboBox->addItem("Pie", TypeThemeWidget::Pie);
    themeComboBox->addItem("Spline", TypeThemeWidget::Spline);
    themeComboBox->addItem("Scatter", TypeThemeWidget::Scatter);
    return themeComboBox;
}


QChart *ThemeWidget::createAreaChart() const
{
    QChart *chart = new QChart();
    chart->setTitle("Area chart");

    // The lower series initialized to zero values
    QLineSeries *lowerSeries = nullptr;
    QString name("Series ");
    int nameIndex = 0;
    for (int i(0); i <new_dataTable.count(); i++) {
        QLineSeries *upperSeries = new QLineSeries(chart);
        for (int j(0); j < new_dataTable[i].count(); j++) {
            Data data =new_dataTable[i].at(j);
            if (lowerSeries) {
                const QVector<QPointF>& points = lowerSeries->pointsVector();
                upperSeries->append(QPointF(j, points[i].y() + data.first.y()));
            } else {
                upperSeries->append(QPointF(j, data.first.y()));
            }
        }

        QAreaSeries *area = new QAreaSeries(upperSeries, lowerSeries);
        area->setName(name + QString::number(nameIndex));

        //isDown() определяет нажата ли кнопка.в нашем случае чекбокс черно-белый
        //для каждого типа графика задаем относительно чексбокса цвет
        if(BlackWhiteCheck->isDown())
        {
             //устанавливаем линейный градиент черно-белый
            QLinearGradient gradient(0, 0, this->width(), this->height());
            gradient.setColorAt(0, Qt::black);
            gradient.setColorAt(1, Qt::white);
//Класс QLinearGradient используется с QBrush для указания кисти с линейным градиентом.
            QBrush brush(gradient);
            area->setBrush(brush);
        }

        nameIndex++;
        chart->addSeries(area);
        chart->createDefaultAxes();
        lowerSeries = upperSeries;
    }

    return chart;
}

QChart *ThemeWidget::createBarChart() const
{
    QChart *chart = new QChart();//создаем объект-график
    chart->setTitle("Bar chart"); //указываем тип- столбачая диаграмма

    QStackedBarSeries *series = new QStackedBarSeries(chart);
    for (int i(0); i < new_dataTable.count(); i++)
    {
        QBarSet *set = new QBarSet("Bar set " + QString::number(i));
        for (const Data &data :new_dataTable[i])
            *set << data.first.y();
    //isDown() определяет нажата ли кнопка.в нашем случае чекбокс черно-белый
    //для каждого типа графика задаем относительно чексбокса цвет
    if(BlackWhiteCheck->isDown()){
         //устанавливаем линейный градиент черно-белый
        QLinearGradient gradient(0, 0, this->width(), this->height());
        gradient.setColorAt(0, Qt::black);
        gradient.setColorAt(1, Qt::white);

        //Класс QLinearGradient используется с QBrush для указания кисти с линейным градиентом.
        QBrush brush(gradient);
        set->setBrush(brush);
       }
     series->append(set);
    }
    chart->addSeries(series);
    chart->createDefaultAxes();

    return chart;
}

QChart *ThemeWidget::createLineChart() const
{
    QChart *chart = new QChart();
    chart->setTitle("Line chart");

    QString name("Series ");
    int nameIndex = 0;
    for (const DataList &list :new_dataTable) {
        QLineSeries *series = new QLineSeries(chart);
        for (const Data &data : list)
            series->append(data.first);
        series->setName(name + QString::number(nameIndex));
        if (BlackWhiteCheck->isDown())
        {
            //для грфика линии черно белый вариант- это установить цвет линии черным при помощь карандаша
            QPen pen(Qt::black);
            series->setPen(pen);
        }
        nameIndex++;
        chart->addSeries(series);
    }
    chart->createDefaultAxes();

    return chart;
}

//создание графика кругового
QChart *ThemeWidget::createPieChart() const
{
    QChart *chart = new QChart();
    chart->setTitle("Pie chart");

    qreal pieSize = 1.0 /new_dataTable.count();
    for (int i = 0; i <new_dataTable.count(); i++) {
        QPieSeries *series = new QPieSeries(chart);
        for (const Data &data :new_dataTable[i]) {
            QPieSlice *slice = series->append(data.second, data.first.y());
            if (data ==new_dataTable[i].first()) {
                slice->setLabelVisible();
                slice->setExploded();
            }
            if (BlackWhiteCheck->isDown())
            {
               //QConicalGradient::QConicalGradient(qreal cx, qreal cy, qreal angle)
                //Интерполировать цвета вокруг центральной точки (QConicalGradient)- круговой
                QConicalGradient gradient( chartView->height(), chartView->width(),360);
               // QLinearGradient gradient(0, 0, chartView->height(), chartView->width());
                gradient.setColorAt(0, Qt::black);
                gradient.setColorAt(1, Qt::white);

                QBrush brush(gradient);
                slice->setBrush(brush);
            }
        }
        qreal hPos = (pieSize / 2) + (i / (qreal)new_dataTable.count());
        series->setPieSize(pieSize);
        series->setHorizontalPosition(hPos);
        series->setVerticalPosition(0.5);
        chart->addSeries(series);
    }

    return chart;
}

QChart *ThemeWidget::createSplineChart() const
{
    // spine chart
    QChart *chart = new QChart();
    chart->setTitle("Spline chart");
    QString name("Series ");
    int nameIndex = 0;
    for (const DataList &list : new_dataTable) {
        QSplineSeries *series = new QSplineSeries(chart);
        for (const Data &data : list)
            series->append(data.first);
        series->setName(name + QString::number(nameIndex));
        if (BlackWhiteCheck->isDown())
        {
            //для диаграмм в виде линий используем карандаш черного цвета
            QPen pen(Qt::black);
            series->setPen(pen);
        }

        nameIndex++;
        chart->addSeries(series);
    }
    chart->createDefaultAxes();
    return chart;
}

QChart *ThemeWidget::createScatterChart() const
{
    // scatter chart
    QChart *chart = new QChart();
    chart->setTitle("Scatter chart");
    QString name("Series ");
    int nameIndex = 0;
    for (const DataList &list :new_dataTable) {
        QScatterSeries *series = new QScatterSeries(chart);
        for (const Data &data : list)
            series->append(data.first);
        series->setName(name + QString::number(nameIndex));


        //isDown() определяет нажата ли кнопка.в нашем случае чекбокс черно-белый
        //для каждого типа графика задаем относительно чексбокса цвет
        if(BlackWhiteCheck->isDown())
        {
             //устанавливаем линейный градиент черно-белый
            QLinearGradient gradient(0, 0, this->width(), this->height());
            gradient.setColorAt(0, Qt::black);
            gradient.setColorAt(1, Qt::white);

            QBrush brush(gradient);
            series->setBrush(brush);
        }

        nameIndex++;
        chart->addSeries(series);
    }
    chart->createDefaultAxes();
    return chart;
}


void ThemeWidget::updateUI()
{

    TypeThemeWidget typeChart_ = static_cast<TypeThemeWidget>(
                typeChart->itemData( typeChart->currentIndex()).toInt());

    //строим графики исходя из типа

     if (typeChart_==TypeThemeWidget::Area)
         chartView->setChart(createAreaChart());
     else if(typeChart_==TypeThemeWidget::Bar)
     {
          chartView->setChart(createBarChart());
     }
     else if(typeChart_==TypeThemeWidget::Pie)
     {
          chartView->setChart(createPieChart());
     }
     else if(typeChart_==TypeThemeWidget::Line)
     {
         chartView->setChart(createLineChart());
     }
     else if(typeChart_==TypeThemeWidget::Spline)
     {
         chartView->setChart(createSplineChart());
     }
     else if(typeChart_==TypeThemeWidget::Scatter)
     {
         chartView->setChart(createScatterChart());
     }

}

