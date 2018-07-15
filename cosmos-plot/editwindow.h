#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QSignalMapper>
#include <QScrollArea>
#include <QObject>
#include <QAction>
#include <QCloseEvent>
#include <QMessageBox>

#include "plot.h"


class EditWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit EditWindow(std::string jsonfile, QWidget *parent = nullptr);
    void show();
    ~EditWindow();
    void create_plots_from_json();
    void setup_plot_list();
    void setup_menus();
    void delete_menus();
    int numplots();

    std::vector<Plot*> plots;
    void set_node_found(int plot_num, bool val);

signals:

public slots:
    void save_json();
    void add_plot();
    void delete_plot(const int plot);
    void add_data(const int plot);
    void delete_data(const int index);
private:

    QWidget *initWidget;
    QVBoxLayout *initVLayout;
    QVBoxLayout *mainLayout;
    QWidget *plot_list_widget;
    QVBoxLayout *plot_list_layout;

    QMenuBar *e_menubar;
    QMenu *e_file_menu;
    QMenu *e_plot_menu;
    QAction *e_save_act;
    QAction *e_add_plot_act;
    // menu delete plot things:
    std::vector<QAction*> e_delete_plot_act;
    QSignalMapper *delete_plot_sigmap;
    QMenu *e_del;
    // menu add data things:
    std::vector<QAction*> e_add_data_act;
    QSignalMapper *add_data_sigmap;
    QMenu *e_add_data;
    // menu delete data things:
    std::vector<QAction*> e_delete_data_act;
    QSignalMapper *delete_data_sigmap;
    QMenu *e_delete_data;
    //


    std::string jsonfile;
};

#endif // EDITWINDOW_H
