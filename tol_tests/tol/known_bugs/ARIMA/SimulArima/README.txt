===================
S I M U L A R I M A
===================

SimulArima es un paquete TOL de simulación de modelos ARIMA con función de 
transferencia e interrupciones que tiene los siguientes objetivos:

 ·servir de motor de pruebas del estimador de modelos ARIMA, e
 ·ilustrar a los usuarios de TOL sobre el modo de funcionamiento del mismo 
 ·servir de ejemplo de programación en TOL para nuevos usuarios


El paquete se compone de los siguientes ficheros

 1)   SimulArimaParam.tol           : Parámetros de la simulación
 1.1) SimulArimaParam_HighGen.tol   : Simulación de modelos largos 
 1.2) SimulArimaParam_MediumGen.tol : Simulación de modelos medianos
 1.3) SimulArimaParam_ShortGen.tol  : Simulación de modelos cortos 
 2)   SimulArimaFunc.tol            : Funciones utilizadas
 3)   SimulArima.tol                : Simulador de la parte ARIMA y tests
 4)   SimulArimaTF.tol              : Simulador de func. de transf. e interrup.
 5)   SimulArimaTFEstim.tol         : Estimación puntual y tests
 6)   SimulaArimaTFEstimMassive.tol : Estimación masiva

Es conveniente leer al menos los encabezamientos de cada fichero TOL antes
de intentar ejecutar nada.

En el fichero SimulArimaParam.tol la variable RandomSeed_ debe estar a 0 si 
se quiere generar una secuencia de números aleatorios distinta cada vez, o 
bien ser igual a cualquier entero positivo si se quiere generar siempre una 
secuencia concreta.

En los ficheros SimulArimaParam_*Gen.tol se pueden cambiar los diferentes
parámetros de configuración de la simulación 

  Real Sigma            //Desviación típica de los residuos
  Real MinCycles        //Mínimo de ciclos estacionales del output
  Real MaxData          //Máxima longitud de la serie output 
  Real MaxInterruptions //Máximo nº de interrupciones
  Real MaxInputs        //Máximo nº de inputs
  Real MaxRegDIF        //Máximo nº de diferencias regulares
  Real MaxSeaDIF        //Máximo nº de diferencias estacionales
  Real MaxRegAR         //Máximo grado AR regular
  Real MaxRegMA         //Máximo grado AR estacional
  Real MaxSeaAR         //Máximo grado MA regular
  Real MaxSeaMA         //Máximo grado MA estacional


Para obtener TOLBase y poder ejecutar este código visite las webs

http://www.tol-project.org
http://tol.bayesforecast.com


Para cualquier consulta, duda o contribución póngase en contacto con

Víctor de Buen Remiro (vdebuen@tol-project.org)

