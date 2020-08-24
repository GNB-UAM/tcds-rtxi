/*
 * Copyright (C) 2011 Georgia Institute of Technology, University of Utah,
 * Weill Cornell Medical College
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This is a template implementation file for a user module derived from
 * DefaultGUIModel with a custom GUI.
 */


////////////////////////////////////////////////////////////////////////////////
/*!
 * @file ownGenerator.cpp
 * @date 7/2020
 * @author Alberto Ayala
 * 
 * @brief Archivo del modulo generador de seniales pregrabadas
 * 
 * Archivo del modulo generador de seniales pregrabadas. Este modulo genera
 * una senial en funcion del voltage leido por fichero y la transmite en bucle
*/
////////////////////////////////////////////////////////////////////////////////


#include "ownGenerator.h"
#include <iostream>
#include <string>
#include <main_window.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <cstdlib>

#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

vector<string> split(string phrase, string delimiter);

extern "C" Plugin::Object*
createRTXIPlugin(void)
{
  return new OwnGenerator();
}

static DefaultGUIModel::variable_t vars[] = {

  { "V", "Voltage", DefaultGUIModel::OUTPUT, },
  { "Buffer size", "size of the data queue",
  DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, },
  { "Column", "column of line to read",
  DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, },
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

OwnGenerator::OwnGenerator(void)
  : DefaultGUIModel("OwnGenerator with Custom GUI", ::vars, ::num_vars)
{
  setWhatsThis("<p><b>OwnGenerator:</b><br>This is the module pre-recorded signal generator</p>");
  DefaultGUIModel::createGUI(vars,
                             num_vars); // this is required to create the GUI
  customizeGUI();
  initParameters();
  update(INIT); // this is optional, you may place initialization code directly
                // into the constructor
  refresh();    // this is required to update the GUI with parameter and state
                // values
  QTimer::singleShot(0, this, SLOT(resizeMe()));
}

OwnGenerator::~OwnGenerator(void)
{
}

void
OwnGenerator::execute(void)
{
  output(0) = cola->dato[cola->index];
  cola->index = (cola->index + 1) % max;
  return;
}

void
OwnGenerator::initParameters(void)
{
  max = 100;
  column = 3;
}

void
OwnGenerator::update(DefaultGUIModel::update_flags_t flag)
{
  switch (flag) {
    case INIT:

      setParameter("Buffer size", max);
      setParameter("Column", column);
      break;

    case MODIFY:
      max = getParameter("Buffer size").toInt();
      column = getParameter("Column").toInt();
      break;

    case UNPAUSE:
      break;

    case PAUSE:
      break;

    case PERIOD:
      // period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      break;

    default:
      break;
  }
}


void
OwnGenerator::customizeGUI(void)
{

  QGridLayout *mylayout = DefaultGUIModel::getLayout();
	
	QGroupBox *fileBox = new QGroupBox("File:");
  QHBoxLayout *fileBoxLayout = new QHBoxLayout;
  fileBox->setLayout(fileBoxLayout);
  QButtonGroup *fileButtons = new QButtonGroup;
  QPushButton *loadBttn = new QPushButton("Load File");
  fileBoxLayout->addWidget(loadBttn);
  fileButtons->addButton(loadBttn);
  QObject::connect(loadBttn, SIGNAL(clicked()), this, SLOT(loadFile())); 
  
  mylayout->addWidget(fileBox, 0, 0);
	setLayout(mylayout);

}

/**
 * Reserva memoria para la estructura Cola
 * 
 * @author Alberto Ayala
 * @date 7/2020
 * 
 * @param[in]   max    tamanio de la estructura
 * @param[out]   max    tamanio de la estructura
 * @return La estructura cola creada e inicializada
*/
OwnGenerator::Cola * OwnGenerator::createQueue(int max){

    Cola *c = NULL;

    c = (Cola*)malloc(sizeof(Cola));

    if(c == NULL){
        return NULL;
    }

    c->dato = NULL;
    c->dato = (double*)calloc(max,sizeof(double));
    if(c->dato==NULL){
        return NULL;
    }

    c->index = 0;
    c->rear = 0;
    c->n_elems = 0;
    c->nmax = max;
    return c;
}

/**
 * Libera memoria de la estructura cola
 * 
 * @author Alberto Ayala
 * @date 7/2020
 * 
 * @param[in, out]   c    cola a eliminar
*/
void OwnGenerator::freeQueue(Cola * c){

    if(c!=NULL){
        free(c->dato);
    }
    free(c);
}

/**
 * Inserta un valor (Voltaje) a la cola
 * 
 * @author Alberto Ayala
 * @date 7/2020
 * 
 * @param[in, out]   c    estructura en la que se inserta
 * @param[in]   e    valor a insertar
 * @return 1 si se inserta correctamente, -1 en otro caso
*/
int OwnGenerator::insertElementQueue(Cola *c, const double e){

    if(c == NULL || c->rear == c->nmax){
        return-1;
    }

    c->dato[c->rear] = e;
    c->rear ++;
    return 1;

}

/**
 * Obtiene datos de voltaje de un fichero y los inserta a la cola
 * 
 * @author Alberto Ayala
 * @date 7/2020
 * 
 * @param[in]   path1    ruta del fichero
 * @param[in]   tam    numero de datos a leer
 * @return la cola tras insertar todos los valores leidos
*/
OwnGenerator::Cola * OwnGenerator::readFile(const char * path1, int tam){

    cout << "LC_ALL: " << setlocale(LC_ALL, NULL) << endl;
    cout << "LC_CTYPE: " << setlocale(LC_CTYPE, NULL) << endl;

    setlocale(LC_ALL,"en_US.utf8");
    Cola * cola = NULL;  
    cola = createQueue(tam);
    int tamaux = 0;

    if(cola == NULL){
        return NULL;
    }

    string path = string(path1);

    fstream fp;
    fp.open(path, ios::in);


    if(!fp.is_open()){
      cout << "file closed" <<endl;
      return NULL;
    }else{
      cout << "file opened" <<endl;
    }

    string line, word, temp;

    while(getline(fp, line)){

      if(line.length()>1){
        vector <string> tokens = split(line, " ");
        double value = std::stod(tokens.at(column));

        insertElementQueue(cola,value);
        tamaux++;

        if(cola->rear == tam){
          break;
        }
      }  
    }
    if (tamaux != max){
      max = tamaux;
      update(INIT);
    }
    fp.close();
    return cola;
}


/**
 * separador de palabras por linea en funcion de un delimitador
 * 
 * @date 7/2020
 * 
 * @param[in]   phrase    linea a separar
 * @param[in]   delimiter    delimitador de palabras
 * @return lista de palabras separadas
*/
vector<string> split(string phrase, string delimiter){
    vector<string> list;
    string s = string(phrase);
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        list.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    list.push_back(s);
    return list;
}

/**
 * Permite elegir fichero de lectura al usuario a traves de la GUI
 * 
 * @author Alberto Ayala
 * @date 7/2020
 * 
 *
*/
void OwnGenerator::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Text file"), "", tr("Text Files (*.txt)"));
    const char *fn =fileName.toLatin1().data();
    path = fn;
    cola = readFile(path,max);
}
