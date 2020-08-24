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
 * This is a template header file for a user modules derived from
 * DefaultGUIModel with a custom GUI.
 */

////////////////////////////////////////////////////////////////////////////////
/*!
   \file ownGenewordsrator.h
   \date 7/2020
   \author Alberto Ayala

   \brief Archivo con las definiciones del modulo Words

   \version 1.0
*/
////////////////////////////////////////////////////////////////////////////////


#include <default_gui_model.h>
#include <runningstat.h>
#define MAX_WORDS_BUFF 60000

#define NOT_INITIATED -2
#define ERR -1
#define OK 0

#define MAX_BITS_WORD 10
#define MAX_WORDS 1024
#define FIRST_CALL 0
#define NEW_TIME_WINDOW 1

#define BIT_DETECTED_OUT 1
#define BIT_NOT_DETECTED_OUT 0
#define END_WINDOW_OUT 2
#define WORD_DETECTED_OUT 3
#define END_HISTOGRAM_TIME -2

#define FALSE 0
#define TRUE 1


//!< WbElement equivalente a entero
typedef int WbElement;

/*!
  * @struct      BitsBuffer
  * @brief   estructura usada para gestionar el buffer de bits
  */
typedef struct{
    char bits[MAX_BITS_WORD];//!< Buffer de bits
    int wordLength;//!< Tamanio de la palabra
    int numBits;//!< Numero de bits
    char* init;//!< Puntero al buffer
    char* insert;//!< Puntero de insercion al buffer
  } BitsBuffer;

 /*!
  * @struct      WordsBuffer
  * @brief   estructura usada para gestionar las palabras detectadas
  */ 
typedef struct {
    WbElement words[MAX_WORDS_BUFF]; //!< Buffer de palabras
    BitsBuffer bb; //!< estructura buffer de bits
    int maxWords; //!< maximo numero de palabras
    int numWords; //!< Primer voltaje analizada de la señal
    WbElement* init; //!< Puntero al buffer
    WbElement* insert; //!< Puntero de insercion al buffer
    WbElement* check; //!< Puntero a la palabra a comprobar   
  } WordsBuffer;

/**
 * Clase del modulo Words que hereda de DefaultGUIModel
 * 
 * @author Alberto Ayala
 * @date 7/2020
 
*/
class Words : public DefaultGUIModel
{

  Q_OBJECT

public:
  Words(void);
  virtual ~Words(void);

  void execute(void);
  void createGUI(DefaultGUIModel::variable_t*, int);
  void customizeGUI(void);

  /*!
  * @enum    mode_t
  * @brief   enumeracion usada para gestionar la toma de latencias
  *
  * @constant    NO_L_TEST     No tomar muestras
  * @constant    L_TEST        Tomar muestras
  */
  enum mode_t {
    NO_L_TEST=0, L_TEST,
  };
  

protected:
  virtual void update(DefaultGUIModel::update_flags_t);

private:

  //!< Periodo del sistema
  double period;
  //!< Tiempo en el que se ejecuto la tarea anterior
  long long lastTime;
  //!< Bit detectado
  char out;
  //!< Tiempo de bin
  long long bin;
  //!< Estructura usada para el analisis de las palabras
  WordsBuffer *wb = NULL;
  //!< Numero de ejecuciones de la tarea
  int numCalls;
  //!< Si se ha detectado la palabra o no
  short detectedWord;
  //!< Longitud de la palabra
  int length,maxWords;
  //!< Palabra a detectar
  char *word;
  //!< Palabra auxiliar 
  char newword[100];
  //!< Variable auxiliar
  int result;
  //!< Primer voltaje analizado de la señal
  double lastVolt1;
  //!< Ultimo voltaje analizado de la señal
  double lastVolt2;
  //!< Voltaje umbral
  double threshold;
  //!< Modo de medicion de latencias
  mode_t mode;
  //!< Variable auxiliar
  double timestep;
  //!< Latencia medida
  double latency;
  //!< Desviacion tipica medida
  double jitter;
  //!< Media aritmetica de las latencias medidas
  double latencymean;
  //!< Tiempo de inicio de la tarea anterior
  double lastRead;
  //!< Primer tiempo de ejecucion de la tarea
  bool firstRead;
  //!< Variable auxiliar
  RunningStat latencyStat;
  //!< Variable auxiliar
  QComboBox *LatencyBox;

  
  void initParameters();
  void writeData();
  void charToInt();

private slots:

  void updateMode(int);
  // these are custom functions that can also be connected to events
  // through the Qt API. they must be implemented in plugin_template.cpp


  int detect_spike (double voltage);
};
