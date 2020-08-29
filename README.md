# Herramienta en tiempo real para estimulación bidireccional dirigida por codificación temporal de sistemas biológicos.

## Descripción

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

## Apoyo

Para más información puede contactar al correo alberto.ayala@estudiante.uam.es

## Autores y reconocimiento

Especial reconocimiento a Ángel Lareo y Francisco de Borja Rodríguez Ortiz.
