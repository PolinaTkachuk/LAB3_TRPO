#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

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
#include <QFileDialog>
#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>
#include <QPushButton>



QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
QT_END_NAMESPACE


//QT_CHARTS_BEGIN_NAMESPACE;
class QChartView;
class QChart;
//QT_CHARTS_END_NAMESPACE;


typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;

//QT_CHARTS_USE_NAMESPACE;

class WidgetChart:public QWidget
{
    Q_OBJECT
public:
    explicit WidgetChart(QWidget *parent = nullptr){}
    ~WidgetChart(){}


};

//виды диаграмм
enum TypeThemeWidget
{
    Area,
    Bar,
    Pie,
    Line,
    Spline,
    Scatter,
};

class ThemeWidget: public QWidget
{
    Q_OBJECT
public:
    explicit ThemeWidget(QWidget *parent = nullptr);
    ~ThemeWidget();

private Q_SLOTS:
    void updateUI(){}
     void openFileDialogWindow();
private:
    DataTable generateRandomData(int listCount, int valueMax, int valueCount) const;
    QComboBox *AddTypeCharts() const;//определяет тип графика из выпадающего списка
    void connectSignals();
public:
    //создание графиков разных типов, цвет определяем внутри
    QChart *createAreaChart() const;
    QChart *createBarChart() const;
    QChart *createPieChart() const;
    QChart *createLineChart() const;
    QChart *createSplineChart() const;
    QChart *createScatterChart() const;


private:
    int m_listCount;
    int m_valueMax;
    int m_valueCount;

    bool checkColor_=false;
    DataTable m_dataTable; //табличное представление
    QComboBox *typeChart; //список- тип графика
    QCheckBox *BlackWhiteCheck; //чекбокс черно-белый
    QPushButton *printChart;//кнопка печати

    QChartView *chartView;
};
#endif // THEMEWIDGET_H
