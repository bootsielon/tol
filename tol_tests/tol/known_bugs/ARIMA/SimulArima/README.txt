===================
S I M U L A R I M A
===================

SimulArima es un paquete TOL de simulaci�n de modelos ARIMA con funci�n de 
transferencia e interrupciones que tiene los siguientes objetivos:

 �servir de motor de pruebas del estimador de modelos ARIMA, e
 �ilustrar a los usuarios de TOL sobre el modo de funcionamiento del mismo 
 �servir de ejemplo de programaci�n en TOL para nuevos usuarios


El paquete se compone de los siguientes ficheros

 1)   SimulArimaParam.tol           : Par�metros de la simulaci�n
 1.1) SimulArimaParam_HighGen.tol   : Simulaci�n de modelos largos 
 1.2) SimulArimaParam_MediumGen.tol : Simulaci�n de modelos medianos
 1.3) SimulArimaParam_ShortGen.tol  : Simulaci�n de modelos cortos 
 2)   SimulArimaFunc.tol            : Funciones utilizadas
 3)   SimulArima.tol                : Simulador de la parte ARIMA y tests
 4)   SimulArimaTF.tol              : Simulador de func. de transf. e interrup.
 5)   SimulArimaTFEstim.tol         : Estimaci�n puntual y tests
 6)   SimulaArimaTFEstimMassive.tol : Estimaci�n masiva

Es conveniente leer al menos los encabezamientos de cada fichero TOL antes
de intentar ejecutar nada.

En el fichero SimulArimaParam.tol la variable RandomSeed_ debe estar a 0 si 
se quiere generar una secuencia de n�meros aleatorios distinta cada vez, o 
bien ser igual a cualquier entero positivo si se quiere generar siempre una 
secuencia concreta.

En los ficheros SimulArimaParam_*Gen.tol se pueden cambiar los diferentes
par�metros de configuraci�n de la simulaci�n 

  Real Sigma            //Desviaci�n t�pica de los residuos
  Real MinCycles        //M�nimo de ciclos estacionales del output
  Real MaxData          //M�xima longitud de la serie output 
  Real MaxInterruptions //M�ximo n� de interrupciones
  Real MaxInputs        //M�ximo n� de inputs
  Real MaxRegDIF        //M�ximo n� de diferencias regulares
  Real MaxSeaDIF        //M�ximo n� de diferencias estacionales
  Real MaxRegAR         //M�ximo grado AR regular
  Real MaxRegMA         //M�ximo grado AR estacional
  Real MaxSeaAR         //M�ximo grado MA regular
  Real MaxSeaMA         //M�ximo grado MA estacional


Para obtener TOLBase y poder ejecutar este c�digo visite las webs

http://www.tol-project.org
http://tol.bayesforecast.com


Para cualquier consulta, duda o contribuci�n p�ngase en contacto con

V�ctor de Buen Remiro (vdebuen@tol-project.org)

