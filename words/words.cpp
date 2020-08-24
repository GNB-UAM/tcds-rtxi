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
 * @file words.cpp
 * @date 7/2020
 * @author Alberto Ayala
 * 
 * @brief Archivo del modulo Words
 * 
 * Archivo del modulo Words. Este modulo se encarga de ejecutar el protocolo TCDS
 * y activar el generador de estimulos en caso de detectar una palabra.
*/
////////////////////////////////////////////////////////////////////////////////


#include "words.h"
#include <iostream>
#include <main_window.h>
#include <math.h>
#include <string.h>
#include <stdio.h>


int wbInit(WordsBuffer* wb, int length, int maxWords);
void bbAdvancePtr (BitsBuffer* bb, char** ptr);
int wbBitInsert(WordsBuffer* wb, char bit);
int Bits2Int(char* bb, int length);
int wbBits2Int(WordsBuffer *wb);
int wbWordInsert (WordsBuffer* wb, int word);
int wbStoreWord (WordsBuffer *wb);
int wbCheckWordMatch(WordsBuffer *wb, char *word);

extern "C" Plugin::Object*
createRTXIPlugin(void)
{
  return new Words();
}

static DefaultGUIModel::variable_t vars[] = {
  { "IV", "Fish Voltage(V)", DefaultGUIModel::INPUT, },
  { "Now", "detecting time", DefaultGUIModel::OUTPUT, },
  { "O", "stimulus generator input", DefaultGUIModel::OUTPUT, },
  { "latency", "task latency", DefaultGUIModel::OUTPUT, },

  { "Bin Time (ns)", "window time",
  DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },

  { "Threshold (V)", "minimum voltage to detect a spike",
  DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE, },

  { "Word length", "number of bits of the word",
  DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER, },

  { "Word", "word to detect",
  DefaultGUIModel::PARAMETER, },
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

Words::Words(void)
  : DefaultGUIModel("Words with Custom GUI", ::vars, ::num_vars)
{
  setWhatsThis("<p><b>Words:</b><br>This is the module Words.</p>");
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

Words::~Words(void)
{
}

void
Words::execute(void)
{

  long long now;
  now = RT::OS::getTime();//nanoseconds

  if(mode == L_TEST){

    if(firstRead){
      lastRead = now;
      firstRead = false;
    }else{
      period = RT::System::getInstance()->getPeriod();
      timestep = now - lastRead;
      latency = (now - lastRead) - period;
      lastRead = now;
      latencyStat.push(latency);
      output(2) = latency;
    }
  }
  
  double voltage = input(0);
  output(0) = now;

  if(numCalls==NEW_TIME_WINDOW){
    lastTime = now;
    numCalls ++;
  }

  if(detect_spike(voltage)&& out!=BIT_DETECTED_OUT){
    
    out = BIT_DETECTED_OUT;
    wbBitInsert(wb,out); //store 1 bit array
    result = wbStoreWord(wb);

    if(ERR != result){
      detectedWord = wbCheckWordMatch(wb,word);
    }
    
  }

  if(bin<(now-lastTime)){
    if(out == BIT_NOT_DETECTED_OUT){
      
      wbBitInsert(wb,out); //store 0 bit array
      result =wbStoreWord(wb);
      if(ERR != result){
        detectedWord = wbCheckWordMatch(wb,word);
      }
    }

    out = BIT_NOT_DETECTED_OUT;
    numCalls=NEW_TIME_WINDOW;

    if(detectedWord){
      output(1) = 1;
      detectedWord = FALSE;
    }else{
      output(1) = 0;
    }
    
  }
  return;
}

void
Words::initParameters(void)
{
  
  out = BIT_NOT_DETECTED_OUT;
  detectedWord = FALSE;
  result = -2;

  if (wb){
    free(wb);
  }

  wb = (WordsBuffer*)malloc(sizeof(WordsBuffer));
  wbInit(wb,length,maxWords);
  numCalls = NEW_TIME_WINDOW;
  lastVolt1 = 0;
  lastVolt2 = 0;


  latency = 0;
  jitter = 0;
  timestep = 0;
  lastRead = 0;
  
  firstRead = true;
}

/**
 * Escribe en memoria la media y la desviación típica de las latencias
 * cuando se pausa el experimento
 * 
 * @date 7/2020
 * 
*/
void
Words::writeData(void)
{
  jitter = latencyStat.std();
  latencymean = latencyStat.mean();
      
  FILE *fptr;
  fptr = fopen("stats.txt", "w");
  if (fptr == NULL) {
    return;
  }

  fprintf(fptr, "jitter: %lf\n", jitter);
  fprintf(fptr, "mean: %lf\n", latencymean);
  fclose(fptr);
}

void
Words::update(DefaultGUIModel::update_flags_t flag)
{
  switch (flag) {
    case INIT:
      bin = 60000000;
      threshold = 2;
      word[0] = '0';
      word[1] = '1';
      word[2] = '0';
      word[3] = '0';
      length = 4;
      maxWords = 5;
      mode = NO_L_TEST;

      setParameter("Word length", length);
      setParameter("Threshold (V)", threshold);
      setParameter("Bin Time (ns)", bin);
      setParameter("Word", word);
      
      charToInt();
      word =  newword;
      // printf("Palabra a analizar: %d\n",Bits2Int(word,length));

      LatencyBox->setCurrentIndex(0);
      updateMode(0);
    
      break;

    case MODIFY:
      mode = mode_t(LatencyBox->currentIndex());
      length = getParameter("Word length").toInt();
      threshold = getParameter("Threshold (V)").toDouble();
      bin = getParameter("Bin Time (ns)").toLong();
      word = (char*)getParameter("Word").toStdString().c_str();
      charToInt();
      word = newword;
      break;

    case UNPAUSE:
      break;

    case PAUSE:
      writeData();
      initParameters();
      break;

    case PERIOD:
      period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
      break;

    default:
      break;
  }
}

/**
 * Cambia el modo entre medir o no las latencias durante el experimento
 * 
 * @date 7/2020
 *
*/
void Words::updateMode(int index)
{
	switch (index) {
		case 0:
			mode = NO_L_TEST;
			break;
		case 1:
			mode = L_TEST;
			break;
	}
	update(MODIFY);
}


void
Words::customizeGUI(void)
{
  QGridLayout* customlayout = DefaultGUIModel::getLayout();
  QGroupBox *modeBox = new QGroupBox("Mode");
	QVBoxLayout *modeBoxLayout = new QVBoxLayout(modeBox);
	LatencyBox = new QComboBox;
	modeBoxLayout->addWidget(LatencyBox);
	LatencyBox->insertItem(0, "Only words");
	LatencyBox->insertItem(1, "Latency testing");
	QObject::connect(LatencyBox,SIGNAL(activated(int)), this, SLOT(updateMode(int)));
	customlayout->addWidget(modeBox, 0, 0);
  setLayout(customlayout);
}

/**
 * Detecta si hay un pulso por encima de un umbral determinado
 * 
 * @date 7/2020
 * 
 * @return 1 si detecta pulso, 0 en otro caso
*/
int Words::detect_spike (double voltage)
{
    
    int spike=0;
    
    if ((lastVolt2<=lastVolt1) &&
        (lastVolt1>=voltage) &&
        (lastVolt1>threshold))
    {
        spike=1;
    }

    lastVolt2 = lastVolt1;
    lastVolt1 = voltage;

    return spike;
}



/**
 * Inicializa los valores y punteros de la estructura WordsBuffer
 * 
 * @author Ángel Lareo
 * @date 1/2013
 * 
 * @param[in,out]   wb       puntero a la estructura a rellenar
 * @param[in]       length   longitud de palabras en estructura expresada en bits    
 * @param[in]       maxWords máximo de palabras a almacenar en la estructura  
*/
int wbInit(WordsBuffer* wb, int length, int maxWords){
    wb->insert=wb->words;
    wb->init=wb->words;
    wb->check = wb->words;
    wb->bb.wordLength=length;
    wb->numWords=0;
    wb->maxWords=maxWords; 
    wb->bb.init = wb->bb.bits;
    wb->bb.insert = wb->bb.bits;
	
    return OK;
}

/**
 * Avanza el puntero en la estructura de bits, convirtiéndola en un buffer circular
 * 
 * @author Ángel Lareo
 * @date 1/2013
 * 
 * @param[in]   wb      puntero a la estructura a rellenar
 * @param[in,out]       ptr     puntero a avanzar
*/
void bbAdvancePtr (BitsBuffer* bb, char** ptr){
    if (*ptr==&(bb->bits[bb->wordLength-1])) *ptr = bb->bits;
    else (*ptr)++;
}

/**
 * Inicializa los valores y punteros de la estructura WordsBuffer
 * 
 * @author Ángel Lareo
 * @date 1/2013
 * 
 * @param[in,out]   wb      puntero a la estructura a rellenar
 * @param[in]       bit     bit a introducir
*/
int wbBitInsert(WordsBuffer* wb, char bit){  //Inserts bit on BitBuffer
    *(wb->bb.insert)=bit;
    bbAdvancePtr(&(wb->bb), &(wb->bb.insert));
    wb->bb.numBits++;
    return OK;
}

int Bits2Int(char* bb, int length){ 
	char *auxPtr;
	int i, exp;
	int wordResult = 0;
	
	exp=length - 1;
	
	auxPtr=bb;
    for (i=0; i<length; ++i){
        if ((int)*auxPtr){
            switch (exp){
                case 0: wordResult += 1; break;
                case 1: wordResult += 2; break;
                default: wordResult += pow(2,exp); break;
            }
        }
        exp--;
        auxPtr++;
    }
    
    return wordResult;
}

int wbBits2Int(WordsBuffer *wb){ 
	char *auxPtr;
	int i, exp;
	int wordResult = 0;
	
	exp=wb->bb.wordLength - 1;
	
	auxPtr=wb->bb.init;
    for (i=0; i<wb->bb.wordLength; ++i){
        if ((int)*auxPtr){
            switch (exp){
                case 0: wordResult += 1; break;
                case 1: wordResult += 2; break;
                default: wordResult += pow(2,exp); break;
            }
        }
        exp--;
        bbAdvancePtr(&(wb->bb), &auxPtr);
    }
    
    return wordResult;
}



/**
 * Inserta una palabra con el valor de word en la estructura WordsBuffer
 * 
 * @author Ángel Lareo
 * @date 1/2013
 * 
 * @param[in,out]   wb      puntero a la estructura a rellenar
 * @param[in]       word    valor de la palabra
*/
int wbWordInsert (WordsBuffer* wb, int word){ //Inserts word on WordsBuffer
                                                 //Called by StoreWord

    *wb->insert = word; //copy word value
	wb->check=wb->insert;
    wb->numWords++; 

    if (wb->insert == wb->words+wb->maxWords-1){
        wb->insert = wb->words;
        if (wb->init == wb->words+wb->maxWords-1) wb->init = wb->words;
        else wb->init++;
    } else wb->insert++;

    return OK;
}

/**
 * Inserta la palabra almacenada en el buffer de bits de la estructura en el buffer de palabras.
 * Para ello primero realiza una transformacion de bits a entero.
 * Si no se han introducido suficientes bits para establecer una palabra, devuelve ERR.
 * 
 * @author Ángel Lareo
 * @date 1/2013
 * 
 * @see wbWordInsert
 * 
 * @param[in,out]   wb      puntero a la estructura donde se insertará la palabra
 * 
 * @return retorna el valor entero de la palabra o ERR
*/
int wbStoreWord (WordsBuffer *wb){ 
    int wordResult;

    if (wb->bb.numBits<wb->bb.wordLength) return ERR;
    
    wordResult = wbBits2Int(wb);

    //Advance bit init
    bbAdvancePtr(&(wb->bb),&(wb->bb.init));

    //Insert word
    wbWordInsert(wb, wordResult);

    return wordResult;
}


/**
 * Comprueba si la palabra binaria word coincide con la que se encuentra en la estructura de bits
 * 
 * @author Ángel Lareo
 * @date 1/2013
 * 
 * @param[in]   wb      puntero a la estructura donde se encuentran los bits de una palabra a comparar
 * @param[in]   word    array de bits con la otra palabra a comparar
*/
int wbCheckWordMatch(WordsBuffer *wb, char *word){
	return (*(wb->check)==Bits2Int(word, wb->bb.wordLength));
}

/**
 * Transforma la palabra a analizar de ASCII a entero
 * 
 * @date 7/2020
 * 
*/
void
Words::charToInt(){

  char h;
  for(int i=0;i<length;i++){
    h = (int)word[i] - 48;
    newword[i] = h;
  }

}