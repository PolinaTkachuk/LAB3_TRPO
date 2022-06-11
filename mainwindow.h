#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QTreeView>
#include <QListView>
#include <QSplitter>
#include <QTableView>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private slots:

    void on_selectionChangedSlot(const QItemSelection &selected, const QItemSelection &deselected);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    QTableView *tableView;

    Ui::MainWindow *ui;
    QFileSystemModel *fileModel;//объект класса QFileSystemModel для работы с папками
    QTreeView *listView;//вид- список папок
    QFileSystemModel *dirModel;
    QTreeView *treeView;//вид- дерево папок
};
#endif // MAINWINDOW_H
