#include "plot.h"



QString Plot::get_plot_title()
{
    return  plot_title;

}

void Plot::set_widget(QCustomPlot *p)
{
    widget = p;
}

void Plot::set_slider(QSlider *s)
{
    slider = s;
}

void Plot::set_range_y(float range)
{
    range_y = range;
}

float Plot::get_range_y()
{
    return range_y;
}

int Plot::get_num_data()
{
    return (int)data_config.size();
}

void Plot::set_range_x(float range)
{
    range_x = range;
}

int Plot::get_range_x()
{
    return range_x;
}

QString Plot::get_label_x()
{
    return label_x;
}

QString Plot::get_label_y()
{
    return label_y;
}

void Plot::set_line_count(int n)
{
    line_count = n;
}

int Plot::get_line_count()
{
    return line_count;
}

bool Plot::isVisible()
{
    return visible && valid;
}

std::string Plot::get_node()
{
    return nodestr;
}

std::string Plot::get_agent()
{
    return agentstr;
}

data_config_t Plot::get_data_config(int index)
{
    return data_config[index];
}

data_edit_t Plot::get_data_edit(int index)
{
    return data_config[index].edit;
}

//std::string Plot::get_xRange()
//{
//    return xRange;
//}




Plot::Plot(QJsonValue jsonVal)
{
    QJsonObject jsonObj = jsonVal.toObject();
    this->json = jsonVal;
    // get values from json
    plot_title = jsonObj.value("title").toString();
    nodestr = jsonObj.value("node").toString().toStdString();
    if(nodestr.length()<1)
        nodestr = "<No Node>";
    agentstr = jsonObj.value("agent").toString().toStdString();
    if(agentstr.length()<1){
        agentstr = "<No Agent>";
    }

    range_x = jsonObj.value("xRange").toDouble();
    range_y = jsonObj.value("yRange").toDouble();

    label_x = jsonObj.value("xLabel").toString();
    label_y = jsonObj.value("yLabel").toString();

    if( jsonObj.value("visible").isUndefined()){
        visible = false;
    }
    else {
        visible = jsonObj.value("visible").toInt() == 1 ? true : false;
    }

    QJsonValue plot_val = jsonObj.value("values");

    QJsonArray jdata = plot_val.toArray();


    data_config.resize(jdata.size());
    for(int i = 0 ; i < jdata.size(); i++)
    {
        data_config[i].data = jdata.at(i).toObject().value("data").toString().toStdString();
        data_config[i].name = jdata.at(i).toObject().value("name").toString().toStdString();
        data_config[i].units = jdata.at(i).toObject().value("units").toString().toStdString();
        data_config[i].scale = jdata.at(i).toObject().value("scale").toDouble();

        if( jdata.at(i).toObject().value("precision").isUndefined()){
            data_config[i].precision = 7;
        }
        else {
            data_config[i].precision = jdata.at(i).toObject().value("precision").toInt();
        }
        if( jdata.at(i).toObject().value("logdata").isUndefined()){
            data_config[i].logdata = true;
        }
        else {
            data_config[i].logdata = jdata.at(i).toObject().value("logdata").toInt() == 0 ? false : true;
        }
    }
    if(data_config.size()==0){
//        data_config.resize(jdata.size());
        add_data();

    }
    line_count = 0;
    found_node = false;
    valid = false;
}


void Plot::init_line_edit()
{
    plot_edit.title = new QLineEdit();
    plot_edit.title->setText(plot_title);
    plot_edit.node = new QLineEdit();
    plot_edit.node->setText(QString::fromStdString(nodestr));
//    plot_edit.agent = new QLineEdit();
//    plot_edit.agent->setText(QString::fromStdString(agentstr));
    plot_edit.agent = new QComboBox();
    plot_edit.agent->addItem(QString::fromStdString(agentstr));
    plot_edit.visible = new QLineEdit();
    plot_edit.visible->setText(QString::number(visible));
    plot_edit.xRange = new QLineEdit();
    plot_edit.xRange->setText(QString::number(range_x));
    plot_edit.yRange = new QLineEdit();
    plot_edit.yRange->setText(QString::number(range_y));
    plot_edit.xLabel = new QLineEdit();
    plot_edit.xLabel->setText(label_x);
    plot_edit.yLabel = new QLineEdit();
    plot_edit.yLabel->setText(label_y);
    for(size_t i = 0; i < data_config.size(); i++){
//        data_config[i].edit.data = new QLineEdit();
//        data_config[i].edit.data->setText(QString::fromStdString(data_config[i].data));
        data_config[i].edit.data = new QComboBox();
        data_config[i].edit.data->addItem(QString::fromStdString(data_config[i].data));
        data_config[i].edit.name = new QLineEdit();
        data_config[i].edit.name->setText(QString::fromStdString(data_config[i].name));
        data_config[i].edit.scale = new QLineEdit();
        data_config[i].edit.scale->setText(QString::number(data_config[i].scale));
        data_config[i].edit.units = new QLineEdit();
        data_config[i].edit.units->setText(QString::fromStdString(data_config[i].units));
        data_config[i].edit.precision = new QLineEdit();
        data_config[i].edit.precision->setText(QString::number(data_config[i].precision));
        data_config[i].edit.logdata = new QLineEdit();
        data_config[i].edit.logdata->setText(QString::number(data_config[i].logdata));
    }
}


bool Plot::save_changes()
{
    bool retn = false; // returns true if node or data are changed
    QJsonObject *jsonObj = new QJsonObject();

    if(plot_edit.title->text() != plot_title){
        plot_title = plot_edit.title->text();
        retn = true;
    }
    jsonObj->insert(QString("title"), QJsonValue(plot_title));

    if(plot_edit.node->text() != QString::fromStdString(nodestr)){
        nodestr = plot_edit.node->text().toStdString();
        retn = true;
    }
    jsonObj->insert(QString("node"), QJsonValue(plot_edit.node->text()));

    if(plot_edit.agent->currentText() != QString::fromStdString(agentstr)){
        agentstr = plot_edit.agent->currentText().toStdString();
    }
    jsonObj->insert(QString("agent"), QJsonValue(plot_edit.agent->currentText()));

    if(plot_edit.visible->text() != QString::number(visible)){
        visible = plot_edit.visible->text().toInt() == 1 ? true : false;
    }
    jsonObj->insert(QString("visible"), QJsonValue(plot_edit.visible->text().toInt()));

    if(plot_edit.xRange->text() != QString::number(range_x)){
        range_x = plot_edit.xRange->text().toInt();
    }
    jsonObj->insert(QString("xRange"), QJsonValue(plot_edit.xRange->text().toInt()));

    if(plot_edit.yRange->text() != QString::number(range_y)){
        range_y = plot_edit.yRange->text().toDouble();
    }
    jsonObj->insert(QString("yRange"), QJsonValue(plot_edit.yRange->text().toDouble()));

    if(plot_edit.xLabel->text() != label_x){
        label_x = plot_edit.xLabel->text();
    }
    jsonObj->insert(QString("xLabel"), QJsonValue(label_x));

    if(plot_edit.yLabel->text() != label_y){
        label_y = plot_edit.yLabel->text();
    }
    jsonObj->insert(QString("yLabel"), QJsonValue(label_y));

    QJsonArray *data_arr = new QJsonArray();
    for(size_t i = 0; i < data_config.size(); i++){
        QJsonObject *dataObj = new QJsonObject();
        if(data_config[i].data != data_config[i].edit.data->currentText().toStdString()){
            data_config[i].data = data_config[i].edit.data->currentText().toStdString();
            retn = true;
        }
        dataObj->insert(QString("data"), QJsonValue(data_config[i].edit.data->currentText()));

        data_config[i].name = data_config[i].edit.name->text().toStdString();
        dataObj->insert(QString("name"), QJsonValue(data_config[i].edit.name->text()));

        data_config[i].units = data_config[i].edit.units->text().toStdString();
        dataObj->insert(QString("units"), QJsonValue(data_config[i].edit.units->text()));

        data_config[i].precision = data_config[i].edit.precision->text().toInt();
        dataObj->insert(QString("precision"), QJsonValue(data_config[i].edit.precision->text().toInt()));

        data_config[i].scale = data_config[i].edit.scale->text().toDouble();
        dataObj->insert(QString("scale"), QJsonValue(data_config[i].edit.scale->text().toDouble()));

        data_config[i].logdata = data_config[i].edit.logdata->text().toInt() == 0 ? false : true;
        dataObj->insert(QString("logdata"), QJsonValue(data_config[i].logdata ? 1 : 0));

        data_arr->append(QJsonValue(*dataObj));
    }

    jsonObj->insert(QString("values"), QJsonValue(*data_arr));
    json = QJsonValue(*jsonObj);
    return retn;
}

plot_edit_t Plot::get_plot_edit()
{
    return plot_edit;
}

int Plot::add_data()
{
    data_config_t ndata;
    ndata.data = "";
    ndata.name = "";
    ndata.units = "";
    ndata.precision = 7;
    ndata.logdata = true;
    ndata.scale = 1;
    ndata.valid = false;


//    ndata.edit.data = new QLineEdit();
//    ndata.edit.data->setText(QString::fromStdString(ndata.data));
    ndata.edit.data = new QComboBox();
    ndata.edit.name = new QLineEdit();
    ndata.edit.name->setText(QString::fromStdString(ndata.name));
    ndata.edit.scale = new QLineEdit();
    ndata.edit.scale->setText(QString::number(ndata.scale));
    ndata.edit.units = new QLineEdit();
    ndata.edit.units->setText(QString::fromStdString(ndata.units));
    ndata.edit.precision = new QLineEdit();
    ndata.edit.precision->setText(QString::number(ndata.precision));
    ndata.edit.logdata = new QLineEdit();
    ndata.edit.logdata->setText(QString::number(ndata.logdata));

    data_config.push_back(ndata);
    return data_config.size() -1;

}

void Plot::delete_data(int index)
{
    // remove from vector
    if(index <= (int)data_config.size()){
        std::vector<data_config_t> old = data_config;
        data_config.resize(0);
        for(size_t i = 0; i < old.size(); i++){
            if((int)i != index)
                data_config.push_back(old[i]);
        }
    }
}

void Plot::add_agent_option(string agentname)
{
    if(agentname != this->agentstr){
        plot_edit.agent->addItem(QString::fromStdString(agentname));

    }
}

void Plot::add_data_option(int index, string data_name)
{
    if(data_config[index].data != data_name){
        data_config[index].edit.data->addItem(QString::fromStdString(data_name));
    }
}


QJsonValue Plot::get_json_val()
{
    return json;
}

QString Plot::get_data_name(int index)
{
    return QString::fromStdString(data_config[index].name);
}

string Plot::get_data_str(int index)
{
    return data_config[index].data;
}

string Plot::get_data_units(int index)
{
    return data_config[index].units;
}

double Plot::get_data_scale(int index)
{
    return data_config[index].scale;
}

int Plot::get_data_precision(int index)
{
    return data_config[index].precision;
}

void Plot::set_json_invalid(int index)
{
    data_config[index].valid = false;
    // invalid data so prevent it from being logged and plotted
//    data_config[index].logdata = false;
}

void Plot::set_json_valid(int index)
{
    data_config[index].valid = true;
}

bool Plot::is_valid(int index)
{
    return data_config[index].valid;
}

bool Plot::is_logged(int index)
{
    return data_config[index].logdata;
}

void Plot::set_jsonentry(int index, cosmosstruc *cinfo)
{
    jsonentry *jentry;
    std::string req = get_data_str(index);
    if((jentry = json_entry_of(req, cinfo))==NULL)
    {
        std::cout<<"'"<<req<<"' is not in the namespace and won't be tracked "<<std::endl;
        set_json_invalid(index);
        data_config[index].jentry = nullptr;
    }
    else
    {
        set_json_valid(index);
        data_config[index].jentry = jentry;
    }
}

uint16_t Plot::get_data_type(int index)
{
    uint16_t type = JSON_TYPE_NONE;
    if(is_valid(index)){
        type = data_config[index].jentry->type;
    }
    return type;
}

void Plot::set_plot_visibility()
{
    // if all the values to plot are invalid, then the plot won't show
    bool valid = false;
    if(!found_node){
        this->valid = false;
    }
    else if(visible){
        for(data_config_t d:data_config){
            if(d.valid) valid = true;
        }
        this->valid = valid;
    }
}

bool Plot::node_found()
{
    return found_node;
}

void Plot::set_node_found(bool found)
{
    found_node = found;
}

bool Plot::create_datafile()
//returns true if all entries are valid and are set to log = 1
{
    bool ret = true;
    if(this->valid){
        for(int i = 0; i < get_num_data(); i ++){
            if(!is_valid(i) || !is_logged(i))
                ret = false;
        }
    }
    else ret = false;

    return ret;
}
jsonentry *Plot::get_jsonentry(int index)
{
    return data_config[index].jentry;
}

