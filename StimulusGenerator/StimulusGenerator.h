/*
* Copyright (C) 2020 Universidad Autonoma de Madrid, Madrid, Spain, Escuela
* Politecnica Superior, Departamento de Ingenieria Informatica, Grupo de 
* Neurocomputacion Biologica (contact: alberto.ayala@estudiante.uam.es Alberto
* Ayala Valencia)
*/

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



////////////////////////////////////////////////////////////////////////////////
/*!
   \file ownGenerator.h
   \date 7/2020
   \author Alberto Ayala

   \brief Archivo con las definiciones del Generador de estimulos

   \version 1.0
*/
////////////////////////////////////////////////////////////////////////////////


#include <default_gui_model.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef STIMULUSGENERATOR_H
#define STIMULUSGENERATOR_H


/**
 * Clase Generador de estimulos que hereda de DefaultGUIModel
 * 
 * @author Alberto Ayala
 * @date 7/2020
 
*/
class StimulusGenerator : public DefaultGUIModel
{

  Q_OBJECT

public:
  StimulusGenerator(void);
  virtual ~StimulusGenerator(void);

  void execute(void);
  void createGUI(DefaultGUIModel::variable_t*, int);
  void customizeGUI(void);

protected:
  virtual void update(DefaultGUIModel::update_flags_t);

private:
  //!< Numero de datos del estimulo
  int max;
  //!< Columna del fichero a leer
  int column;
  
  /*!
  * @struct      Cola
  * @brief   estructura que permite la insercion y el acceso
  * de forma circular
  */
  struct Cola{

      int index; //!< indice de lectura
      int rear; //!< lugar de insercion
      int n_elems; //!< numero de elementos
      int nmax; //!< tamanio maximo de la cola
      double *dato; //!< puntero al dato, datos que contiene

  };

  
  //!< Declaracion cola
  Cola *cola = NULL;
  //!< Ruta fichero de lectura
  const char *path;
  //!< Periodo del sistema
  double period;
  int flag;

  void initParameters();

private slots:
  // these are custom functions that can also be connected to events
  // through the Qt API. they must be implemented in plugin_template.cpp

  Cola * createQueue(int max);
  void freeQueue(Cola * c);
  int insertElementQueue(Cola *c, const double e);
  Cola * readFile(const char * path, int tam);
  Cola * readFileAux(int tam);
  void loadFile();
};

#endif
