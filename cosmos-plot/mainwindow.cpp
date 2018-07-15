#include "mainwindow.h"
#include "ui_mainwindow.h"

// TODO: move to core
#include <ctime>
#include <string>     // std::string, std::to_string
#include <iomanip>
#include <string>

// TODO: data to log
// motion tracker
// - quaternion, omega
// adcs
// - quaternion_estimated, omega, magnetometer, magnetic torquerods, alpha, ...

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // create menubar
    createActions();
    createMenus();


    // display plot widget
    plotWidget = new QWidget();
    plotVLayout = new QVBoxLayout(plotWidget);
    ui->verticalLayout->addWidget(plotWidget);

    // find nodes/agents widget
    findWidget = new QWidget();
    findVLayout = new QVBoxLayout(findWidget);


    setWindowTitle((QString)"CosmosPlot");

}

MainWindow::~MainWindow()
{
    datafile.close();
    delete ui;
}



void MainWindow::startTimers(){

    // start timer
    et.start();

    timer2.start();

}

cosmosstruc *MainWindow::find_cinfo(int plot_num)
{
    cosmosstruc *cinfo = nullptr;
    for(cosmosstruc* c:cinfos)
    {
        if(c->node.name == editWindow->plots[plot_num]->get_node())
        {
            cinfo = c;
        }
    }
    return cinfo;
}

cosmosstruc * MainWindow::add_cinfo(std::string node)
{
    cosmosstruc *cinfo = nullptr;
    int32_t iretn;

    //check if it already exists
//    if(node != ""){
        if ((cinfo = get_cinfo(node)) == nullptr)
        {
            jsonnode jnode;
            if ((iretn=agent->getJson(node, jnode)))
            {
                cinfo = json_create();
                if ((iretn=json_setup_node(jnode, cinfo)) < 0)
                {
                    json_destroy(cinfo);
                    cinfo = nullptr;
                }
                else
                {
                    cinfos.push_back(cinfo);
                }
            }
        }
//    }

    return cinfo;
}


cosmosstruc * MainWindow::get_cinfo(std::string node)
{
    cosmosstruc *cinfo = nullptr;

    //check if it already exists
    for(cosmosstruc* c:cinfos)
    {
        if(c->node.name == node)
        {
            cinfo = c;
        }
    }

    return cinfo;
}


/* Opens JSON file
 * - loads JSON object into json
 * - loads array into jplots
*/
void MainWindow::init_from_json(string filename)
{

    agent = new Agent();
    this->filename = filename;
    this->json_filepath = QString("");
    filenameJSON = QString::fromStdString(filename) + ".json";

    /* TODO:
     * call constructor for EditWindow (filename)
     *
     */
    editWindow = new EditWindow(filenameJSON.toStdString());

    check_plots();


    init_datalog(filename);


}

void MainWindow::init_without_json()
{
    disable_menus();

    sel_json_widget = new QWidget();
    QVBoxLayout *dlayout = new QVBoxLayout;
    sel_json_widget->setLayout(dlayout);
    QLabel *text = new QLabel();
    text->setText("Choose JSON file to configure plots");
    dlayout->addWidget(text);
    QPushButton *open_file = new QPushButton("Create New JSON File", this);
    dlayout->addWidget(open_file);
    connect(open_file, SIGNAL (released()),this, SLOT (create_new_json()));
    QPushButton *sel_file = new QPushButton("Select JSON File", this);
    dlayout->addWidget(sel_file);
    connect(sel_file, SIGNAL (released()),this, SLOT (select_json_file()));
    plotVLayout->addWidget(sel_json_widget);

}

void MainWindow::setup_no_plots()
{
    //. call edit window constructor with default json file
    agent = new Agent();
    editWindow = new EditWindow(filenameJSON.toStdString());
    check_plots();
    init_datalog(this->filename);

    open_edit_window();
    enable_menus();
    delete plotWidget;
    plotWidget = new QWidget();
    plotVLayout = new QVBoxLayout(plotWidget);
    ui->verticalLayout->addWidget(plotWidget);
    this->setup_plot_widget();


}

void MainWindow::nothing_to_plot()
{
    /* main window when there are no plots to display
     *  but there is a json file
     * give option to
     * -update from json
     * -open json
     * -select another json
    */

    sel_json_widget = new QWidget();
    QVBoxLayout *dlayout = new QVBoxLayout;
    sel_json_widget->setLayout(dlayout);
    QLabel *text = new QLabel();
    string msg = "There is nothing to plot, here are possible reasons why: \n";
    msg+= "1. CosmosPlot Agent could not find specified nodes.\n";
    msg+="\t View available nodes:  \"Other\">\"Find Nodes\Agents\" \n";
    msg+= "2. Invalid Data entries in JSON File\n";
    msg+= "\t View invalid entries: \"File\">\"Edit JSON\" \n \t Make necessary changes\n \t Save the file\n";
    msg+= "3. CosmosPlot has not updated from JSON file changes\n";
    msg+="\t Update changes: \"Other\">\"Update Plot from JSON\" \n";
    text->setText(QString::fromStdString(msg));

    plotVLayout->addWidget(text);


}

void MainWindow::save_json()
{

    QFile saveFile(filenameJSON);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
    }
    QJsonArray plotsArray;
    for(size_t i = 0; i < editWindow->plots.size(); i++){
        plotsArray.append(editWindow->plots[i]->get_json_val());
    }
    QJsonDocument saveDoc(plotsArray);
    saveFile.write(saveDoc.toJson());
    cout<<"JSON FILE SAVED"<<endl;
}

void MainWindow::reload_nodes(int plot_num)
{
    cosmosstruc *cinfo= nullptr;
    int numData = editWindow->plots[plot_num]->get_num_data();
    string node_name = editWindow->plots[plot_num]->get_node();
    // make cinfo for each node
    int i = 0;

    do
    {
        if ((cinfo = add_cinfo(node_name)) != nullptr)
        {
            break;
        }
        COSMOS_SLEEP(1);
        i++;
    } while (cinfo == nullptr && i < 2);

    if( cinfo == nullptr){
        std::cout<<"Reloading Nodes: could not find node "<<node_name<<std::endl;
        for(int j = 0; j < numData; j++){
            editWindow->plots[plot_num]->set_json_invalid(j);
        }
        editWindow->set_node_found(plot_num,false);
    }
    else {
        editWindow->set_node_found(plot_num,true);

        for(int j = 0; j < numData; j++)
        {
            editWindow->plots[plot_num]->set_jsonentry(j, cinfo);
        }
    }
    editWindow->plots[plot_num]->set_plot_visibility();

}

void MainWindow::check_plots()
{
    for(size_t k= 0; k < editWindow->plots.size(); k++){
        reload_nodes(k);
    }
}

void MainWindow::populate_namespace_list(int plot_index)
/* Populates the lists in the json editor with all json namespace values
 */
{
        string node = editWindow->plots[plot_index]->get_plot_edit().node->text().toStdString();
        cosmosstruc *cinfo = add_cinfo(node);
        if(cinfo != nullptr){
            for (vector<jsonentry> entryrow : cinfo->jmap)
            {
                for (jsonentry entry : entryrow)
                {
                    for(int j = 0; j < editWindow->plots[plot_index]->get_num_data(); j++){
                        editWindow->plots[plot_index]->add_data_option(j, entry.name);
                    }
                }
            }
        }
}

void MainWindow::populate_agents_list()
/* Populates the list in the json editor with all available agents
 */
{
//    agent_select_sigmap = new QSignalMapper();
    for(size_t i = 0; i < editWindow->plots.size(); i++){
        for(size_t j = 0; j < agent->agent_list.size(); j++){
            string agent_name = agent->agent_list[j].proc;
            editWindow->plots[i]->add_agent_option(agent_name);
        }
        // create signals for when an agent is selected
//        connect(editWindow->plots[i]->get_plot_edit().agent, SIGNAL(&QListWidget::itemClicked),agent_select_sigmap, SLOT(map()));
        connect(editWindow->plots[i]->get_plot_edit().agent, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index){ agent_selected(i); });
//        agent_select_sigmap->setMapping(editWindow->plots[i]->get_plot_edit().agent, i );
//        connect(agent_select_sigmap, SIGNAL(mapped(int)), this, SLOT(agent_selected(const int&)));
    }




}

void MainWindow::disable_menus()
/* menu items won't be available on startup of cosmosplot
 * */
{
    edit_file_act->setEnabled(false);
    update_plot_act->setEnabled(false);
    find_nodes_act->setEnabled(false);
}

void MainWindow::enable_menus()
{
    edit_file_act->setEnabled(true);
    update_plot_act->setEnabled(true);
    find_nodes_act->setEnabled(true);
}

// Populates "Find" tab with all nodes found
void MainWindow::find_nodes()
{
    agentSelect->clear();
    agentList->clear();
    QString node_agent;
    cout<<"agents found: "<<agent->agent_list.size()<<endl;
    for(size_t i = 0; i < agent->agent_list.size(); i++){
        node_agent = QString();
        node_agent+=QString::fromStdString(agent->agent_list[i].node);
        node_agent+=QString(":");
        node_agent+=QString::fromStdString(agent->agent_list[i].proc);
        agentList->addItem(node_agent);
    }

}

void MainWindow::reload_plots()
{
    // close current datafile
    if(datafile.is_open()){
        datafile.close();
    }
    check_plots();
    init_datalog(filename);
    // reload plot widget
    delete plotWidget;
    plotWidget = new QWidget();
    plotVLayout = new QVBoxLayout(plotWidget);
    ui->verticalLayout->addWidget(plotWidget);
    this->setup_plot_widget();

}

void MainWindow::open_edit_window()
{
    for(size_t i = 0; i < editWindow->plots.size(); i++){
        //update json object
        if(editWindow->plots[i]->save_changes()){
            reload_nodes(i);
        }
    }

    editWindow->show();
    populate_agents_list();
    for(int i = 0; i < editWindow->numplots(); i++){
        if(editWindow->plots[i]->node_found())
            populate_namespace_list(i);

    }
}

void MainWindow::open_nodes_window()
{

    findWidget->show();
}

void MainWindow::select_json_file()
/* allows user to select a json file to open
 *
 */
{

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("JSON (*.json)"));
    dialog.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    QString chosen_file = fileNames.at(0);
    QChar c = QChar('/');
    QStringList list = chosen_file.split(c);
   this->filenameJSON = list.at(list.size()-1);
   this->filename = list.at(list.size()-1).split('.').at(0).toStdString();
   chosen_file.chop(list.at(list.size()-1).length());
   this->json_filepath = chosen_file;

   setup_no_plots();


}

void MainWindow::create_new_json()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("New File Name"), QLineEdit::Normal,
                                             QDir::home().dirName(), &ok);

    if (ok && !text.isEmpty()){
        this->filename = text.toStdString();
        this->filenameJSON = text+".json";
        setup_no_plots();
    }

}

void MainWindow::list_namespace()
/* populates list of all namespace values for a chosen node:agent in 'Find' window
 */
{
    agentSelect->clear(); // removing all options already in the box
    QString selected_agent  = agentList->selectedItems()[0]->text();
    string node;
    string agent;
//    cout<<agentList->selectedItems()[0]->text().toStdString()<<endl;
    QStringList l = selected_agent.split(':');
    node = l[0].toStdString();
    agent = l[1].toStdString();
    cosmosstruc *cinfo = add_cinfo(node);
    if(cinfo!= nullptr){
        for (vector<jsonentry> entryrow : cinfo->jmap)
        {
            for (jsonentry entry : entryrow)
            {
                agentSelect->addItem(QString::fromStdString(entry.name));
            }
        }
    }
    else {
        agentSelect->addItem(QString("<Node Information Unavailable>"));
    }
}

void MainWindow::agent_selected(int plot_index)
{


    string agentname = editWindow->plots[plot_index]->get_plot_edit().agent->currentText().toStdString();
//    cout<< "agent selected "<<agentname<<endl;
    string node="";
    // get node name
    for(size_t i = 0; i < agent->agent_list.size(); i++){
        if(agent->agent_list[i].proc == agentname){
            node = agent->agent_list[i].node;
            // set node name
            editWindow->plots[plot_index]->get_plot_edit().node->setText(QString::fromStdString(node));
            editWindow->plots[plot_index]->set_node_found(true);
        }
    }
    populate_namespace_list(plot_index);

}


void MainWindow::setup_plot_widget()
{
    int line_count;
    int total_line_count = 0;
    jsonentry *jdata;
    for(size_t i = 0; i < editWindow->plots.size(); i++)
    {
        line_count = 0;
        editWindow->plots[i]->set_plot_visibility();
        if(editWindow->plots[i]->isVisible() && editWindow->plots[i]->node_found())
        {
            for(int j = 0; j < editWindow->plots[i]->get_num_data(); j++)
            {
                if(editWindow->plots[i]->is_valid(j))
                {
                    //                    jdata = json_entry_of(editWindow->plots[i]->get_data_str(j), cinfo);
                    jdata = editWindow->plots[i]->get_jsonentry(j);
                    switch(jdata->type)
                    {
                    case JSON_TYPE_DOUBLE:
                    case JSON_TYPE_ALIAS:
                        line_count += 1;
                        break;
                    case JSON_TYPE_RVECTOR:
                    case JSON_TYPE_AVECTOR:
                    case JSON_TYPE_CVECTOR:
                    case JSON_TYPE_GVECTOR:
                    case JSON_TYPE_SVECTOR:
                        line_count += 3;
                        break;
                    case JSON_TYPE_QUATERNION:
                        line_count += 4;
                        break;
                    case JSON_TYPE_FLOAT:
                        line_count += 1;
                        break;
                    }
                }
            }
            editWindow->plots[i]->set_line_count(line_count);
            createPlot(i);
        }
        else {
            editWindow->plots[i]->set_line_count(line_count);
        }
        total_line_count+= line_count;

    }

    if(total_line_count>0){

    }
    else{
        // nothing to plot
        cout<< "nothing to plot"<<endl;
        nothing_to_plot();
    }
    startKey = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;

    connect(timer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    timer->start(1000); // 0 = as fast as possible
}

void MainWindow::setup_find_widget()
{
    findButton = new QPushButton("Find all Nodes and Agents");
    connect(findButton, SIGNAL (clicked()), this, SLOT (find_nodes()));
    findVLayout->addWidget(findButton);
    QGroupBox*nodes = new QGroupBox("Nodes");
    QVBoxLayout *node_layout = new QVBoxLayout();

    nodeList = new QListWidget();
    node_layout->addWidget(nodeList);
    nodes->setLayout(node_layout);
    findVLayout->addWidget(nodes);

    QGroupBox*agents = new QGroupBox("Agents");
    QVBoxLayout *agent_layout = new QVBoxLayout();

    agentList = new QListWidget();
    agent_layout->addWidget(agentList);
    agents->setLayout(agent_layout);
    findVLayout->addWidget(agents);

    agentSelect = new QComboBox();
    findVLayout->addWidget(agentSelect);
    connect(agentList, &QListWidget::itemClicked, this, &MainWindow::list_namespace);


}


void MainWindow::createPlot(int index)
{
    QGroupBox *box= new QGroupBox(editWindow->plots[index]->get_plot_title());
    QCustomPlot *plot1 = new QCustomPlot(this);
    editWindow->plots[index]->set_widget(plot1);

    QHBoxLayout *horizontal = new QHBoxLayout;
    horizontal->addWidget(plot1);
    horizontal->setContentsMargins(0,0,0,0);
    box->setLayout(horizontal);
    box->setAlignment(Qt::AlignCenter);

    QVBoxLayout *vertical = new QVBoxLayout;
    horizontal->addLayout(vertical);

    // Text input for scale factor
    QLineEdit *text =new QLineEdit;
    editWindow->plots[index]->input_range = text;
    text->setMaximumWidth(50);
    vertical->addWidget(text);

    // range slider
    QSlider *slider = new QSlider(Qt::Vertical, this);

    editWindow->plots[index]->set_slider(slider);
    vertical->addWidget(slider);

//    ui->verticalLayout->addWidget(box); ///CHANGES
    plotVLayout->addWidget(box); ///CHANGES


    // set plot Y range
    editWindow->plots[index]->input_range->setText(QString::number(editWindow->plots[index]->get_range_y()));

    // monitor text changed
    connect(editWindow->plots[index]->input_range, SIGNAL(returnPressed()), this, SLOT(update_plot_range()));

    // setting up graph

    // plot 1
    plot1->legend->setFont(QFont("Helvetica",9));

    // Set X label
    //QString xaxis = "Time \"hh:mm:ss\"";
    //plot1->xAxis->setLabel(xaxis);

    QPen colors[] = {QPen(Qt::red),QPen(Qt::green),QPen(Qt::blue),QPen(Qt::magenta),QPen(Qt::yellow)};

    int line_count = editWindow->plots[index]->get_line_count();

    for(int i = 0; i < line_count; i++)
    {
        plot1->addGraph(); // red line
        plot1->graph(2*i)->setPen(colors[i%5]);

        plot1->addGraph(); // red dot
        //        plot1->graph((2*i)+1)->setWidth(2);
        plot1->graph((2*i)+1)->setPen(colors[i%5]);
        plot1->graph((2*i)+1)->setLineStyle(QCPGraph::lsNone);
        //        plot1->graph((2*i)+1)->setScatterStyle(QCPScatterStyle::ssDisc);
        plot1->graph((2*i)+1)->removeFromLegend();
    }

    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    plot1->xAxis->setTicker(dateTimeTicker);

    //    plot1->xAxis->setRange(QCPAxisTickerDateTime::dateTimeToKey(QDate(2013, 11, 16)), QCPAxisTickerDateTime::dateTimeToKey(QDate(2015, 5, 2)));
    dateTimeTicker->setDateTimeFormat("hh:mm:ss");

    //    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    //    timeTicker->setTimeFormat("%h:%m:%s");
    //    plot1->xAxis->setTicker(timeTicker);
    plot1->axisRect()->setupFullAxesBox();


    // old qcustomplot setting
    //    plot1->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    //    plot1->xAxis->setDateTimeFormat("hh:mm:ss");
    //    plot1->xAxis->setAutoTickStep(false);
    //    plot1->xAxis->setTickStep(plots[index]->get_range_x()/10);
    //    plot1->axisRect()->setupFullAxesBox();

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(plot1->xAxis, SIGNAL(rangeChanged(QCPRange)), plot1->xAxis2, SLOT(setRange(QCPRange)));
    connect(plot1->yAxis, SIGNAL(rangeChanged(QCPRange)), plot1->yAxis2, SLOT(setRange(QCPRange)));

    // set Y axis range
    plot1->yAxis->setRange(editWindow->plots[index]->get_range_y(), -editWindow->plots[index]->get_range_y());

    // set X, Y axis labels
    plot1->xAxis->setLabel(editWindow->plots[index]->get_label_x());
    plot1->yAxis->setLabel(editWindow->plots[index]->get_label_y());

    // enable user interaction with mouse
    plot1->setInteraction(QCP::iRangeDrag, true);

    plot1->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop); // make legend align in top left corner or axis rect

}







void MainWindow::realtimeDataSlot()
{

    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;

    jsonentry *jdata = nullptr;
    int line_num;
    cosmosstruc *cinfo = nullptr;
    int32_t iretn;
    vector <double> line;

    // Update data
    Agent::messstruc mess;
    while ((iretn = agent->readring(mess, Agent::AgentMessage::BEAT, 0., Agent::Where::TAIL)) > 0)
    {
        if (iretn <2 || iretn > 3)
        {
            continue;
        }

        cinfo = get_cinfo(mess.meta.beat.node);
        if (cinfo != nullptr)
        {
            // update node and device
            iretn = json_parse(mess.adata.c_str(),cinfo);
        }
    }

    // Update plots and log
    double dt = timer2.getElapsedTime();

    // buffer to save on file
    print2file.reset();
    print2file.printOn = false;
    print2file.delimiter_flag = true;
    print2file.precision = 1;
    print2file.scalar(dt);
    print2file.precision = 7;
    print2file.fieldwidth = 7;

    // buffer to print on console
    print2console.reset();
    print2console.delimiter_flag = true;
    print2console.precision = 1;
    print2console.scalar(dt);

    // config for all the values
    print2console.precision = 7;
    print2console.fieldwidth = 7;
    print2console.use_brackets = true;

    for (size_t i=0; i < editWindow->plots.size(); i++)
    {

        // collect the data for this agent
        // TODO: check if we can split the JSON object from the Plot object
        // rather than calling the whole Plot object here (example: jsonArray[i].get_agent() instead

        line_num = 0;
        if(editWindow->plots[i]->isVisible() && editWindow->plots[i]->node_found())
        {
            line.resize(editWindow->plots[i]->get_line_count());
        }

        cinfo = find_cinfo(i);
        for(int j = 0; j < editWindow->plots[i]->get_num_data(); j++)
        {
            if(editWindow->plots[i]->is_valid(j))
            {
                print2console.delimiter = ":";
                print2console.text(editWindow->plots[i]->get_data_name(j).toStdString()+"["+
                                   editWindow->plots[i]->get_data_units(j)+"]");
                print2console.delimiter = ",";
                print2console.precision = editWindow->plots[i]->get_data_precision(j);
                print2file.precision = editWindow->plots[i]->get_data_precision(j);
//                jdata = json_entry_of(editWindow->plots[i]->get_data_config(j).data, cinfo);
                jdata = editWindow->plots[i]->get_jsonentry(j);
                switch(jdata->type)
                {
                case JSON_TYPE_DOUBLE:
                case JSON_TYPE_ALIAS:
                    {
                        double scalar = json_get_double(*jdata, cinfo );
                        print2console.scalar(scalar);
                        if(editWindow->plots[i]->is_logged(j))
                        {
                            print2file.scalar(scalar);
                        }
                        if(editWindow->plots[i]->isVisible() && editWindow->plots[i]->node_found())
                        {
                            line[line_num] = scalar;
                            editWindow->plots[i]->widget->graph(2*line_num)->setName(editWindow->plots[i]->get_data_name(j));
                            line_num += 1;
                        }
                    }
                    break;
                case JSON_TYPE_RVECTOR:
                case JSON_TYPE_AVECTOR:
                case JSON_TYPE_CVECTOR:
                case JSON_TYPE_GVECTOR:
                case JSON_TYPE_SVECTOR:
                    {
                        rvector rdata = json_get_rvector(*jdata, cinfo );
                        print2console.vector(rdata);
                        if(editWindow->plots[i]->is_logged(j))
                        {
                            print2file.vector(rdata);
                        }
                        if(editWindow->plots[i]->isVisible() && editWindow->plots[i]->node_found())
                        {
                            line[line_num] = rdata.col[0];
                            line[line_num+1] = rdata.col[1];
                            line[line_num+2] = rdata.col[2];
                            editWindow->plots[i]->widget->graph(2*line_num)->setName(editWindow->plots[i]->get_data_name(j) + "_x");
                            editWindow->plots[i]->widget->graph(2*(line_num+1))->setName(editWindow->plots[i]->get_data_name(j) + "_y");
                            editWindow->plots[i]->widget->graph(2*(line_num+2))->setName(editWindow->plots[i]->get_data_name(j) + "_z");
                            line_num += 3;
                        }
                    }
                    break;
                case JSON_TYPE_QUATERNION:
                    {
                        quaternion q = json_get_quaternion(*jdata, cinfo );
                        print2console.quat(q,3);
                        if(editWindow->plots[i]->is_logged(j))
                        {
                            print2file.quat(q,3);
                        }
                        if(editWindow->plots[i]->isVisible() && editWindow->plots[i]->node_found())
                        {
                            line[line_num] = q.w;
                            line[line_num+1] = q.d.x;
                            line[line_num+2] = q.d.y;
                            line[line_num+3] = q.d.z;
                            editWindow->plots[i]->widget->graph(2*line_num)->setName(editWindow->plots[i]->get_data_name(j) + "_w");
                            editWindow->plots[i]->widget->graph(2*(line_num+1))->setName(editWindow->plots[i]->get_data_name(j) + "_x");
                            editWindow->plots[i]->widget->graph(2*(line_num+2))->setName(editWindow->plots[i]->get_data_name(j) + "_y");
                            editWindow->plots[i]->widget->graph(2*(line_num+3))->setName(editWindow->plots[i]->get_data_name(j) + "_z");
                            line_num += 4;
                        }
                    }
                    break;
                case JSON_TYPE_FLOAT:
                    {
                        float scalar2 = json_get_double(*jdata, cinfo );
                        print2console.scalar(scalar2);
                        if(editWindow->plots[i]->is_logged(j))
                        {
                            print2file.scalar(scalar2);
                        }
                        if(editWindow->plots[i]->isVisible() && editWindow->plots[i]->node_found())
                        {
                            line[line_num] = scalar2;
                            editWindow->plots[i]->widget->graph(2*line_num)->setName(editWindow->plots[i]->get_data_name(j));
                            line_num += 1;
                        }
                    }
                    break;

                } // end switch(type)
            }
        }//end for (lines)

        if(editWindow->plots[i]->isVisible() && editWindow->plots[i]->node_found())
        {
            editWindow->plots[i]->widget->legend->setVisible(true);

            for(int k = 0; k < editWindow->plots[i]->get_line_count(); k ++)
            {
                editWindow->plots[i]->widget->graph(2*k)->addData(key, line[k]); // red
                //                editWindow->plots[i]->widget->graph((2*k)+1)->clearData();
                editWindow->plots[i]->widget->graph((2*k)+1)->addData(key, line[k]);
                //                editWindow->plots[i]->widget->graph((2*k))->removeDataBefore(key-editWindow->plots[i]->get_range_x()); // remove data of lines that's outside visible range:

            }

            // set X axis range
            // moving time window [key, range]
            editWindow->plots[i]->widget->xAxis->setRange(key+0.25, editWindow->plots[i]->get_range_x(), Qt::AlignRight); // make key axis range scroll with the data (at a constant range size of 8):
            //editWindow->plots[i]->widget->xAxis->setRange(key+0.25, xRange, Qt::AlignRight);

            // set Y axis range
            editWindow->plots[i]->widget->yAxis->setRange(editWindow->plots[i]->slider->value()*editWindow->plots[i]->get_range_y(),
                                              -editWindow->plots[i]->slider->value()*editWindow->plots[i]->get_range_y());

            editWindow->plots[i]->widget->replot();
        }

    } // end for(plots)

    print2console.endline();
    if(write_datafile){
        datafile << print2file.fullMessage << endl;
    }


}

// create the first line for the data file
void MainWindow::init_datalog(string jsonfile)
{

    string local_time = get_local_time();
    string filename = "data-";
    //filename += mjdToGregorianFormat(currentmjd());
    filename += string(local_time);
    filename += "-" + jsonfile;
    filename += ".csv";

    write_datafile = true;
    for (size_t i=0; i < editWindow->plots.size(); i++){
        if(!editWindow->plots[i]->create_datafile())
            write_datafile = false;
    }
    if(write_datafile){
        datafile.open (filename);
        datafile << "time,";
        for (size_t i=0; i < editWindow->plots.size(); i++){
            for(int j = 0; j < editWindow->plots[i]->get_num_data(); j++){
                if(editWindow->plots[i]->is_logged(j)){
                    switch(editWindow->plots[i]->get_data_type(j)){
                    case JSON_TYPE_DOUBLE:
                    case JSON_TYPE_ALIAS:
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j);
                        break;
                    case JSON_TYPE_RVECTOR:
                    case JSON_TYPE_CVECTOR:
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_x,";
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_y,";
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_z";
                        break;
                    case JSON_TYPE_GVECTOR:
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_lat,";
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_lon,";
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_h";
                        break;
                    case JSON_TYPE_SVECTOR:
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_phi,";
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_lambda,";
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_r";
                        break;
                    case JSON_TYPE_AVECTOR:
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_h,";
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_e,";
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_b";
                        break;
                    case JSON_TYPE_QUATERNION:
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_w,";
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_x,";
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_y,";
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j)<<"_z";
                        break;
                    case JSON_TYPE_FLOAT:
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j);
                        break;
                        // TO DO: add cases for the rest of the types
                    default:
                        datafile << editWindow->plots[i]->get_node() << ":" << editWindow->plots[i]->get_data_str(j);
                        break;
                    }
                    datafile<<",";
                }
                //            if(i != log.size()-1) // not the last one
                //                datafile<<",";
            }
        }

        datafile<<endl;
        cout << "created data file: " << filename << endl;
    }

    else {
        cout<<"no valid data to log - data file not created"<<endl;
    }
}

void MainWindow::createMenus()
{
    menuBar()->setNativeMenuBar(false);
    fileMenu = ui->menuBar->addMenu(tr("&File"));
    fileMenu->addAction(edit_file_act);
    fileMenu->addAction(select_file_act);
    fileMenu->addAction(new_file_act);


    fileMenu->addSeparator();
    actionMenu = ui->menuBar->addMenu(tr("&Other"));
    actionMenu->addAction(update_plot_act);
    actionMenu->addAction(find_nodes_act);

}

void MainWindow::createActions()
{
    edit_file_act = new QAction(tr("Edit JSON"), this);
    edit_file_act->setStatusTip(tr("Edit JSON Plot Configuration File "));
    connect(edit_file_act, &QAction::triggered, this, &MainWindow::open_edit_window);

    new_file_act = new QAction(tr("Create New JSON File"), this);
    new_file_act->setStatusTip(tr("Edit JSON Plot Configuration File "));
    connect(new_file_act, &QAction::triggered, this, &MainWindow::create_new_json);

    select_file_act = new QAction(tr("Select JSON File"), this);
    select_file_act->setStatusTip(tr("Edit JSON Plot Configuration File "));
    connect(select_file_act, &QAction::triggered, this, &MainWindow::select_json_file);

    update_plot_act = new QAction(tr("Update Plot from JSON"), this);
    update_plot_act->setStatusTip(tr("Update from JSON Plot Configuration File "));
    connect(update_plot_act, &QAction::triggered, this, &MainWindow::reload_plots);

    find_nodes_act = new QAction(tr("Find Nodes/Agents"), this);
    find_nodes_act->setStatusTip(tr("Find COSMOS Nodes and Agents"));
    connect(find_nodes_act, &QAction::triggered, this, &MainWindow::open_nodes_window);
}



void MainWindow::update_plot_range()
{

    for (size_t plot_index=0; plot_index < editWindow->plots.size(); plot_index++) {
        //    QString input = ui->plot1_input_range->text();
        if(editWindow->plots[plot_index]->isVisible()){
            // get new range
            QString input = editWindow->plots[plot_index]->input_range->text();
            // set range factor
            editWindow->plots[plot_index]->set_range_y(stof(input.toStdString()));
            qDebug() << "new scale factor for plot "<<plot_index<<" " << editWindow->plots[plot_index]->get_range_y();
            //TO DO: update range

        }
    }
}

void MainWindow::closeEvent(QCloseEvent *) {
    if(timer->isActive()) timer->stop();      // had to stop timer here !!!!!!!!
    QApplication::quit();
}
