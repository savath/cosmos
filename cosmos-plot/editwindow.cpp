#include "editwindow.h"

EditWindow::EditWindow(std::string jsonfile, QWidget *parent) : QMainWindow(parent)
{
    this->jsonfile = jsonfile;

    initWidget = new QWidget();
    initVLayout = new QVBoxLayout(initWidget);

    QScrollArea *scrollBox = new QScrollArea(initWidget);
    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *sLayout = new QVBoxLayout(scrollWidget);
    scrollBox->setWidget(scrollWidget);
    scrollBox->setWidgetResizable(true);

    QWidget *main = new QWidget();
    mainLayout = new QVBoxLayout(main);
    plot_list_widget = new QWidget(); // displays the json file contents
    plot_list_layout = new QVBoxLayout(plot_list_widget);

    initVLayout->addWidget(scrollBox);
    initVLayout->setMargin(0);

    sLayout->addWidget(main);
    mainLayout->addWidget(plot_list_widget);


    initWidget->setWindowTitle("JSON File Editor");
    setup_menus();
    create_plots_from_json();



}

void EditWindow::show()
{
    delete_menus();
    setup_menus();
    setup_plot_list();
    initWidget->show();
}

EditWindow::~EditWindow()
{

}


void EditWindow::create_plots_from_json()
{

    QString file = QString::fromStdString(jsonfile);
    QFile loadFile;
    loadFile.setFileName(file);
    if(!loadFile.exists()) {
        // json file doesnt exist - create an empty one
        plots.resize(0);
//        add_plot();
        QJsonValue v;
        Plot *p = new Plot(v);
        p->init_line_edit();
        plots.push_back(p);

    }
    else {
        if (!loadFile.open(QIODevice::ReadOnly)) {
            std::cout<<loadFile.errorString().toStdString()<<": "<< jsonfile<<std::endl;
    //        return ;
        }
        QByteArray saveData = loadFile.readAll();

        QJsonDocument loadDoc( QJsonDocument::fromJson(saveData));

    //        json = loadDoc.object();

        //json array of plot
        QJsonArray jplots = loadDoc.array();

        plots.resize(0);

        foreach(const QJsonValue & v, jplots)
        {
            Plot *p = new Plot(v);

            // for edit json window
            p->init_line_edit();
            plots.push_back(p);
        }

    }

}


void EditWindow::setup_plot_list()
{

    // deleting whats already in the layout to display again
    QLayoutItem* child;

    while(plot_list_layout->count()!=0)
    {
        child = plot_list_layout->takeAt(0);
        if(child->layout() != 0)
        {
            delete child->layout();
        }
        else if(child->widget() != 0)
        {
            delete child->widget();
        }

        delete child;
    }


    for(size_t i = 0; i < plots.size(); i++){


        plots[i]->init_line_edit();

        //create form widget
        QGroupBox *plot = new QGroupBox("Plot "+QString::number(i+1));
        QVBoxLayout *vl = new QVBoxLayout();

        QWidget *plot_form = new QWidget();
        QFormLayout *fl = new QFormLayout();


        if(!plots[i]->node_found()){
            QLabel *label_node = new QLabel();
            label_node->setStyleSheet("QLabel { color : red; }");
            label_node->setText("Node "+QString::fromStdString(plots[i]->get_node())+" not found");
            vl->addWidget(label_node);
        }


        fl->addRow(QObject::tr("&Title:"), plots[i]->get_plot_edit().title);
        fl->addRow(QObject::tr("&Node:"), plots[i]->get_plot_edit().node);
        fl->addRow(QObject::tr("&Agent:"), plots[i]->get_plot_edit().agent);
        fl->addRow(QObject::tr("&Visible:"), plots[i]->get_plot_edit().visible);
        fl->addRow(QObject::tr("&x Range:"), plots[i]->get_plot_edit().xRange);
        fl->addRow(QObject::tr("&y Range:"), plots[i]->get_plot_edit().yRange);
        fl->addRow(QObject::tr("&x Label:"), plots[i]->get_plot_edit().xLabel);
        fl->addRow(QObject::tr("&y Label:"), plots[i]->get_plot_edit().yLabel);

        plot_form->setLayout(fl);
        vl->addWidget(plot_form);
        plot->setLayout(vl);

        QWidget *values = new QWidget();
        QVBoxLayout *values_layout = new QVBoxLayout(values);
        vl->addWidget(values);
        for(int j = 0; j < plots[i]->get_num_data(); j++)
        {
            QWidget *f_data = new QWidget();
            QVBoxLayout *vl2 = new QVBoxLayout(f_data);

            QWidget *form_data = new QWidget();
            QFormLayout *fl_data = new QFormLayout();
            form_data->setFixedWidth(300 );

            if(!plots[i]->is_valid(j) && plots[i]->node_found()){
                QLabel *label_d = new QLabel();
                label_d->setStyleSheet("QLabel { color : red; }");
                label_d->setText("invalid");
                vl2->addWidget(label_d);
            }

            fl_data->addRow(QObject::tr("&Data:"), plots[i]->get_data_edit(j).data);
            fl_data->addRow(QObject::tr("&Name:"), plots[i]->get_data_edit(j).name);
            fl_data->addRow(QObject::tr("&Precision:"), plots[i]->get_data_edit(j).precision);
            fl_data->addRow(QObject::tr("&Scale:"), plots[i]->get_data_edit(j).scale);
            fl_data->addRow(QObject::tr("&Units:"), plots[i]->get_data_edit(j).units);
            fl_data->addRow(QObject::tr("&Log data:"), plots[i]->get_data_edit(j).logdata);

            vl2->addWidget(form_data);
            form_data->setLayout(fl_data);
            values_layout->addWidget(f_data);

        }

        plot_list_layout->addWidget(plot);
    }
}

void EditWindow::setup_menus()
{
    /* create actions for menu
     *
     */
    e_save_act = new QAction(QString("Save"), initWidget);
    connect(e_save_act,&QAction::triggered, this, &EditWindow::save_json);

    e_add_plot_act = new QAction(QObject::tr("Add Plot"), initWidget);
    connect(e_add_plot_act, &QAction::triggered, this, &EditWindow::add_plot);


    e_delete_plot_act.resize(plots.size());
    delete_plot_sigmap = new QSignalMapper(this);

    e_add_data_act.resize(plots.size());
    add_data_sigmap = new QSignalMapper(this);

    e_delete_data_act.resize(0);
    delete_data_sigmap = new QSignalMapper(this);
    int k = 0;
    for(size_t i=0; i < plots.size(); i++ ){
        int plot_num = static_cast<int>(i);
        QString plot_title = plots[i]->get_plot_title();
        QString label = "Plot "+ QString::number(i+1)+ ": "+plot_title;

        e_delete_plot_act[i] = new QAction(label, this);
        connect(e_delete_plot_act[i], SIGNAL(triggered()),delete_plot_sigmap, SLOT(map()));
        delete_plot_sigmap->setMapping(e_delete_plot_act[i], plot_num );

        e_add_data_act[i] = new QAction(label, this);
        connect(e_add_data_act[i], SIGNAL(triggered()),add_data_sigmap, SLOT(map()));
        add_data_sigmap->setMapping(e_add_data_act[i], plot_num );

        for(int j=0; j < plots[i]->get_num_data();j++){
            QString data_label = plots[i]->get_data_name(j);
            QAction* data_act = new QAction(label+": "+data_label, this);
            e_delete_data_act.push_back(data_act);
            connect(e_delete_data_act[k], SIGNAL(triggered()),delete_data_sigmap, SLOT(map()));
            delete_data_sigmap->setMapping(e_delete_data_act[k], k );
            k++;
        }
    }
    connect(delete_plot_sigmap, SIGNAL(mapped(int)), this, SLOT(delete_plot(const int&)));
    connect(add_data_sigmap, SIGNAL(mapped(int)), this, SLOT(add_data(const int&)));
    connect(delete_data_sigmap, SIGNAL(mapped(int)), this, SLOT(delete_data(const int&)));


    e_menubar = new QMenuBar();
    e_menubar->setGeometry(QRect(0, 0, 924, 22));
    e_file_menu = e_menubar->addMenu(QObject::tr("&File"));
    e_plot_menu = e_menubar->addMenu(QObject::tr("&Plot"));

    // add actions to menu
    e_file_menu->addAction(e_save_act);
    e_plot_menu->addAction(e_add_plot_act);

    e_del = new QMenu("Delete Plot");
    e_plot_menu->addMenu(e_del);

    e_add_data = new QMenu("Add Data");
    e_plot_menu->addMenu(e_add_data);

    e_delete_data = new QMenu("Delete Data");
    e_plot_menu->addMenu(e_delete_data);

    // add actions to menu
    e_file_menu->addAction(e_save_act);
    e_plot_menu->addAction(e_add_plot_act);

    for(size_t i = 0; i < plots.size(); i ++){
        e_del->addAction(e_delete_plot_act[i]);
        e_add_data->addAction(e_add_data_act[i]);
    }
    for(size_t i = 0; i < e_delete_data_act.size(); i++){
        e_delete_data->addAction(e_delete_data_act[i]);
    }

    initVLayout->setMenuBar(e_menubar);
    e_menubar->setNativeMenuBar(false);
}

void EditWindow::delete_menus()
{
    delete delete_plot_sigmap;
    delete e_menubar;
    e_delete_plot_act.resize(0);
}

int EditWindow::numplots()
{
    return (int)plots.size();
}


void EditWindow::set_node_found(int plot_num, bool val)
{
    plots[plot_num]->set_node_found(val);
}

void EditWindow::save_json()
{
    // update Plot vector
    for(size_t i = 0; i < plots.size(); i++){
        //update json object
        if(plots[i]->save_changes()){
//            reload_nodes(i);
        }
    }
    /* Update json file
     *
     */
    cout<<"Json filename" <<jsonfile<<endl;
    QFile saveFile(QString::fromStdString(jsonfile));
    if (!saveFile.open(QIODevice::WriteOnly)) {

        qWarning("Couldn't open save file.");
    }
    QJsonArray plotsArray;
    for(size_t i = 0; i < plots.size(); i++){
        plotsArray.append(plots[i]->get_json_val());
    }
    QJsonDocument saveDoc(plotsArray);
    saveFile.write(saveDoc.toJson());
    setup_plot_list();
    cout<<"JSON FILE SAVED"<<endl;


}

void EditWindow::add_plot()
{
    // add to plots vector
    QJsonValue v;
    Plot *p = new Plot(v);
    plots.push_back(p);
    p->init_line_edit();

    // creating the form for the new plot
    QGroupBox *plot = new QGroupBox("Plot "+QString::number(plots.size()));
    QVBoxLayout *vl = new QVBoxLayout();

    QWidget *plot_form = new QWidget();
    QFormLayout *fl = new QFormLayout();


    if(!p->node_found()){
        QLabel *label_node = new QLabel();
        label_node->setStyleSheet("QLabel { color : red; }");
        label_node->setText("Node "+QString::fromStdString(p->get_node())+" not found");
        vl->addWidget(label_node);
    }


    fl->addRow(QObject::tr("&Title:"), p->get_plot_edit().title);
    fl->addRow(QObject::tr("&Node:"), p->get_plot_edit().node);
    fl->addRow(QObject::tr("&Agent:"), p->get_plot_edit().agent);
    fl->addRow(QObject::tr("&Visible:"), p->get_plot_edit().visible);
    fl->addRow(QObject::tr("&x Range:"), p->get_plot_edit().xRange);
    fl->addRow(QObject::tr("&y Range:"), p->get_plot_edit().yRange);
    fl->addRow(QObject::tr("&x Label:"), p->get_plot_edit().xLabel);
    fl->addRow(QObject::tr("&y Label:"), p->get_plot_edit().yLabel);

    plot_form->setLayout(fl);
    vl->addWidget(plot_form);
    plot->setLayout(vl);


    for(int j = 0; j < p->get_num_data(); j++)
    {
        QWidget *f_data = new QWidget();
        QVBoxLayout *vl2 = new QVBoxLayout();

        QWidget *form_data = new QWidget();
        QFormLayout *fl_data = new QFormLayout();
        form_data->setFixedWidth(300 );

        if(!p->is_valid(j) && p->node_found()){
            QLabel *label_d = new QLabel();
            label_d->setStyleSheet("QLabel { color : red; }");
            label_d->setText("invalid");
            vl2->addWidget(label_d);
        }

        fl_data->addRow(QObject::tr("&Data:"), p->get_data_edit(j).data);
        fl_data->addRow(QObject::tr("&Name:"), p->get_data_edit(j).name);
        fl_data->addRow(QObject::tr("&Precision:"), p->get_data_edit(j).precision);
        fl_data->addRow(QObject::tr("&Scale:"), p->get_data_edit(j).scale);
        fl_data->addRow(QObject::tr("&Units:"), p->get_data_edit(j).units);
        fl_data->addRow(QObject::tr("&Log data:"), p->get_data_edit(j).logdata);

        vl2->addWidget(form_data);
        f_data->setLayout(vl2);
        form_data->setLayout(fl_data);
        vl->addWidget(f_data);

    }
        plot_list_layout->addWidget(plot);

        //add action to menubar for delete
        int plot_num = (int) plots.size();
        QString label = "Plot "+ QString::number(plot_num);
        QAction *delete_new_plot = new QAction(label, this);
//        e_delete_plot_act[plot_num-1] = delete_new_plot;
        e_delete_plot_act.push_back(delete_new_plot);
        connect(e_delete_plot_act[plot_num-1], SIGNAL(triggered()),delete_plot_sigmap, SLOT(map()));
        delete_plot_sigmap->setMapping(e_delete_plot_act[plot_num-1], plot_num-1 );
        e_del->addAction(e_delete_plot_act[plot_num-1]);


        //add action to menubar to add data
        QAction *add_data_new_plot = new QAction(label, this);
//        e_add_data_act[plot_num-1] = add_data_new_plot;
        e_add_data_act.push_back(add_data_new_plot);
        connect(e_add_data_act[plot_num-1], SIGNAL(triggered()),add_data_sigmap, SLOT(map()));
        add_data_sigmap->setMapping(e_add_data_act[plot_num-1], plot_num-1 );
        e_add_data->addAction(e_add_data_act[plot_num-1]);

        //add action to menubar to delete data
        QAction *del_new_data = new QAction(label, this);
        e_delete_data_act.push_back(del_new_data);
        connect(e_delete_data_act[e_delete_data_act.size()-1], SIGNAL(triggered()),delete_data_sigmap, SLOT(map()));
        delete_data_sigmap->setMapping(e_delete_data_act[e_delete_data_act.size()-1], e_delete_data_act.size()-1 );
        e_delete_data->addAction(e_delete_data_act[e_delete_data_act.size()-1]);

}



void EditWindow::delete_plot(const int plot)
{

    // remove from plots vector
    if(plot <= (int)plots.size()){
        // remove actions from menu
        e_del->removeAction(e_delete_plot_act[plot]);
        vector<QAction*> old_del_act = e_delete_data_act;
        e_delete_data_act.resize(0);

        e_add_data->removeAction(e_add_data_act[plot]);
        vector<QAction*> old_add_act = e_add_data_act;
        e_add_data_act.resize(0);
        vector<Plot*> old = plots;
        plots.resize(0);

        for(size_t i = 0; i < old.size(); i++){
            if((int)i != plot){
                plots.push_back(old[i]);
                e_delete_data_act.push_back(old_del_act[i]);
                e_add_data_act.push_back(old_add_act[i]);
            }

        }
        old_add_act.clear();
        old_del_act.clear();
        // redo plot list
        setup_plot_list();
    }




}

void EditWindow::add_data(const int plot)
{
//    cout<< "Adding data to plot "<<plot<<endl;
    plots[plot]->add_data();
    // TO DO: add action to delete data
    int index=0;
    for(size_t i = 0; i <= plot; i++){

        index+= plots[i]->get_num_data();
    }
    QString label = "Plot "+ QString::number(plot+1)+" new";
    QAction *del_new_data = new QAction(label, this);

    std::vector<QAction*> old = e_delete_data_act;
    e_delete_data_act.resize(0);
    int k = 0;
    for(size_t i = 0; i < plots.size(); i++){
        for(int j = 0; j < plots[i]->get_num_data(); j++){
            if(k < index-1){
                e_delete_data_act.push_back(old[k]);
            }
            else if(k == index -1) {
                e_delete_data_act.push_back(del_new_data);
            }
            else{
                e_delete_data_act.push_back(old[k-1]);
            }
            k++;
        }
    }
    connect(e_delete_data_act[index-1], SIGNAL(triggered()),delete_data_sigmap, SLOT(map()));
    delete_data_sigmap->setMapping(e_delete_data_act[e_delete_data_act.size()-1], e_delete_data_act.size()-1 );
    e_delete_data->addAction(e_delete_data_act[index-1]);
    setup_plot_list();
}

void EditWindow::delete_data(const int index)
{
    cout<< "Deleting data "<<index<<endl;
    int k = 0;
    // removing "delete" action for this data
    e_delete_data->removeAction(e_delete_data_act[index]);
    vector<QAction*> old_act = e_delete_data_act;
    e_delete_data_act.resize(0);
    int plot_index;
    int data_index;

    for(size_t i = 0; i < plots.size(); i ++){
        for(int j = 0; j < plots[i]->get_num_data(); j++){
            if(index == k ){
                plot_index = i;
                data_index = j;
            }
            else {
                e_delete_data_act.push_back(old_act[k]);
            }
            k++;
        }
    }
    old_act.clear();
    // remove from data_config vector in plots[]
    plots[plot_index]->delete_data(data_index);
    setup_plot_list();
}
