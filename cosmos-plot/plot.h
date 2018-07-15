#ifndef PLOT_H
#define PLOT_H

// Qt headers
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QDir>
#include <QSlider>
#include <QLineEdit>

// C++ headers
#include <iostream>
#include <stdio.h>
#include <string.h>

// project headers
#include "qcustomplot/qcustomplot.h"

//cosmos libraries
#include "jsonlib.h"
 using namespace std;

struct plot_edit_t
{
    QLineEdit *title;
    QLineEdit *node;
//    QLineEdit *agent;
    QComboBox *agent;
    QLineEdit *visible;
    QLineEdit *xRange;
    QLineEdit *yRange;
    QLineEdit *xLabel;
    QLineEdit *yLabel;
};
struct data_edit_t
{
//    QLineEdit *data;
    QComboBox *data;
    QLineEdit *name;
    QLineEdit *scale;
    QLineEdit *precision;
    QLineEdit *units;
    QLineEdit *logdata;
};

struct data_config_t
{
    std::string data;
    std::string name;
    std::string units;
    int precision;
    double scale;
    bool valid;
    bool logdata; // print this data in log
    jsonentry *jentry;
    data_edit_t edit;
};

class Plot
{
public:
    Plot(QJsonValue jsonVal);

    //stuff for line inputs
    void init_line_edit();
    bool save_changes();
    plot_edit_t get_plot_edit();
    int add_data();
    void delete_data(int index);
    void add_agent_option(std::string agentname);
    void add_data_option(int index, std::string data_name);

    QJsonValue get_json_val();

    QString get_plot_title();
    void set_widget(QCustomPlot *p);
    void set_slider(QSlider *s);

    void set_range_y(float range);
    float get_range_y();

    void set_range_x(float range);
    int get_range_x();

    QString get_label_x();
    QString get_label_y();

    void set_line_count(int n); // number of lines to appear on plot
    int get_line_count();
    bool isVisible();
    std::string get_node();
    std::string get_agent();

    int get_num_data();

    void set_plot_visibility();

    bool node_found();
    void set_node_found(bool found);

    QCustomPlot *widget;
    QSlider *slider;
    QLineEdit *input_range;

    bool create_datafile();

    // accessing members of data_config vector by index
    data_config_t get_data_config(int index);
    data_edit_t get_data_edit(int index);
    QString get_data_name(int index);
    std::string get_data_str(int index);
    std::string get_data_units(int index);
    double get_data_scale(int index);
    int get_data_precision(int index);
    void set_json_invalid(int index);
    void set_json_valid(int index);
    bool is_valid(int index);
    bool is_logged(int index);
    jsonentry *get_jsonentry(int index);
    void set_jsonentry(int index, cosmosstruc *cinfo);
    uint16_t get_data_type(int index);



private:
    QString plot_title;
    QString label_x;
    QString label_y;
    std::string nodestr;
    std::string agentstr;
    bool visible;
    bool valid;
    bool found_node;
    std::vector<data_config_t> data_config;
    plot_edit_t plot_edit;
    QJsonValue json;


    int range_x = 20;
    double range_y = 0.1;
    int line_count;

};
#endif // PLOT_H
