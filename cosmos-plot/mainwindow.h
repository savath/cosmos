#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// cosmos libraries
#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "support/timeutils.h"
#include "support/timelib.h"
#include "jsonlib.h"
#include "support/elapsedtime.h"
#include "support/print_utils.h"
#include "support/convertlib.h"

// standard C++ libraries
#include <stdio.h>
#include <iostream>
#include <fstream>

// Qt libraries
#include <QMainWindow>
#include <QTimer>
#include <QJsonObject>
#include <QFormLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <QJsonArray>
#include <QLineEdit>
#include <QMessageBox>
#include <QWidget>
#include <QRadioButton>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QDialogButtonBox>

// project libraries
#include "qcustomplot/qcustomplot.h"
//#include "plot.h"
#include "editwindow.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
//    void setup_init_widget();
    void setup_plot_list();
    void setup_plot_widget();
    void setup_find_widget();
    void init_from_json(string filename);
    void init_without_json();
    void setup_no_plots();
    void nothing_to_plot();
    void save_json();
    void reload_nodes(int plot_num);
    void check_plots();
    void populate_namespace_list(int plot_index);
    void populate_agents_list();
    void disable_menus();
    void enable_menus();


    void createPlot(int index);
    void closeEvent(QCloseEvent *);
    void startTimers();

    cosmosstruc *find_cinfo(int plot_num);
    cosmosstruc *add_cinfo(std::string node);
    cosmosstruc *get_cinfo(std::string node);


private slots:
    void realtimeDataSlot();
    void update_plot_range();
//    void updateJsonSlot();
    void find_nodes();
    void reload_plots();
//    void add_plot();
    //menu stuff
    void open_edit_window();
    void open_nodes_window();
    void select_json_file();
    void create_new_json();
    void list_namespace();
    void agent_selected(int plot_index);

private:
    Ui::MainWindow *ui;

    // Main Widgets / Layouts
    // Widgets for Json File Editor Window
//    QWidget *initWidget;
//    QVBoxLayout *initVLayout;
//    QVBoxLayout *mainLayout;
//    QWidget *plot_list_widget;
//    QVBoxLayout *plot_list_layout;

    QWidget *plotWidget;
    QVBoxLayout *plotVLayout;

    QWidget *findWidget;
    QVBoxLayout *findVLayout;
    QPushButton *findButton;
    QListWidget *nodeList;
    QListWidget *agentList;
    QComboBox *agentSelect;


    int numPlots;
    QString filenameJSON; // json filename with ".json"
    std::string filename; // json filename without ".json"
    QString json_filepath; // path of json file
    bool write_datafile;
    QJsonArray jplots;  // json array with info for each plot

//    std::vector<Plot*> plots;
    struct logvalue
    {
        jsonentry *j;
        string node;
    };

    std::vector<logvalue> log;
    std::vector<cosmosstruc*> cinfos;


    // Qt timers
    QTimer *timer = new QTimer(this);

    // cosmos timers
    ElapsedTime et, timer2;

    QCPItemTracer *itemDemoPhaseTracer;
    int currentDemoIndex;
    void collect_data_loop();
    void init_datalog(string jsonfile);

    Agent * agent;

    double startKey;
    std::ofstream datafile;


    PrintUtils print2console;
    PrintUtils print2file;

    // menu stuff
    void createMenus();
    void createActions();
    QMenu *fileMenu;
    QMenu *actionMenu;
    QAction *edit_file_act;
    QAction *new_file_act;
    QAction *select_file_act;
    QAction *update_plot_act;
    QAction *find_nodes_act;
    QActionGroup *alignmentGroup;

    QWidget *sel_json_widget;
    EditWindow *editWindow;


    QSignalMapper *agent_select_sigmap;

};



#endif // MAINWINDOW_H
