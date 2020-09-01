# Herramienta en tiempo real para estimulación bidireccional dirigida por codificación temporal de sistemas biológicos.

## Descripción

Por favor, si usa alguno de esto módulos cita en tu publicación el siguiente trabajo:

Ayala Valencia, A., Lareo Fernández, Á., & Rodríguez Ortiz, F. d. (2020). Estudio y desarrollo de una herramienta en tiempo real para estimulación bidireccional dirigida por codificación temporal en el contexto de peces. eléctricos. Madrid.


Este proyecto preliminar forma parte de mi Trabajo Fin de Grado en la EPS-UAM, cuyo título es:  Estudio y desarrollo de una herramienta en tiempo real para estimulación bidireccional dirigida por codificación temporal en el contexto de peces. eléctricos.

El software desarrollado en este proyecto es libre por lo que cualquier persona o grupo de investigación puede hacer uso de él para realizar sus propios experimentos.

Esta herramienta está formada por tres módulos distintos diseñada para el estudio de la codificación de información en el
sistema nervioso mediante la estimulación bidireccional en ciclo cerrado.

El generador de señales pregrabadas se encarga de enviar en tiempo real la señal especificada por el usuario a través de la interfaz gráfica. Este estímulo se enviará constantemente hasta que el usuario decida finalizar el experimento

El módulo Words es el encargado de ejecutar el protocolo Temporal code-driven stimulation (https://doi.org/10.3389/fninf.2016.00041). Si el módulo detecta la palabra buscada, activa una bandera como salida del módulo indicando este hecho.

El generador de estímulos al igual que el generador de señales pregrabadas carga un estímulo a través de la interfaz gráfica al seleccionar un archivo. Además, funciona igual que un disparador, se encuenta en un estado de espera constante hasta que recibe una señal que le indica que debe comenzar con el envío del estímulo. A diferencia que el generador de señales pregrabadas, una vez que termina de enviar el estímulo, vuelve al estado de espera inicial.

## Instalación

Para poder usar esta herramienta, se necesita la instalación usual de RTXI (http://rtxi.org/install/).

Para instalar cada uno de los módulos simplemente se debe clonar el repositorio, dirigirse a cada uno de los módulos (cd MODULE_NAME) e instalarlos mediante el comando "sudo make install".

Para más información consultar http://rtxi.org/modules/.

## Uso

Para usar estos módulos debe conectar la salida del generador de señales o de cualquier generador ya desarrollado a la entrada del módulo Words. A su vez, la salida del módulo Words debe ser conectada a la entrada del generador de estímulos. Por último, la salida del generador de estímulos debe estar conectada al sistema que desea estimular (artificial o biológico).

Antes de comenzar la ejecución, se deben inicializar los parámetros de cada módulo y elegir los ficheros utilizados por los generadores.

Se recomienda el uso del módulo Sync para sincronizar todos los módulos en los experimentos.

## Desarrollo

Para más información técnica se puede contactar al correo alberto.ayala@estudiante.uam.es (Alberto Ayala Valencia)

## Autores

Alberto Ayala Valenci, Ángel Lareo Fernández y Francisco de Borja Rodríguez Ortiz,



Departamento de Ingeniería Informática, Grupo de Neurocomputación Biológica, Escuela Politécnica Superior, Universidad Autónoma de Madrid, Madrid, Spain

## Licencia

[GNU GPL 3](https://www.gnu.org/licenses/gpl-3.0.html)