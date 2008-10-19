* (06-Feb-2008) Novedades en Tol v1.1.7 b.0.6.alpha:

  Soluciona los siguientes errores reportados:
    539 : InnerPoint no encuentra un punto válido
          http://bugs.tol-project.org/show_bug.cgp?id=539
    540 : Fallo aleatorio de la función arms
          http://bugs.tol-project.org/show_bug.cgp?id=540
    541 : TOL se cae escribiendo un mensaje largo de error de parseo
          http://bugs.tol-project.org/show_bug.cgp?id=541
    542 : El tipo Anything no funciona bien
          http://bugs.tol-project.org/show_bug.cgp?id=542
    544 : Tol se cae usando la función FibonacciMin
          http://bugs.tol-project.org/show_bug.cgp?id=544
    545 : Fallo en la función FibonacciMin con baja tolerancia
          http://bugs.tol-project.org/show_bug.cgp?id=545
    547 : TextOccurrences no encuentra cadenas con tamaño mayor de un caracter
          http://bugs.tol-project.org/show_bug.cgp?id=547
    549 : TolServerEval falla con expresiones regulares que contienen \n
          http://bugs.tol-project.org/show_bug.cgp?id=549
    554 : TextToDate no conmvierte bien
          http://bugs.tol-project.org/show_bug.cgp?id=554

  NameBlock
    Si se añade el miembro Text _.autodoc.member.<member_name> a un NameBlock entonces el miembro de nombre <member_name> adopta ese texto como descripción
    
  Set    
    Nueva función built-in HasIndexByName para ver si un conjunto está indexado
    Nuevo metodo StdLib::getOptArg para trabajar con conjuntos de argumentos opcionales
    
  Text
    Nuevo método StdLib::RegReplace para reemplazamientos con expressiones regulares
        
  Statistics
    NameBlock StdLib::Rapi  : API basada en ficheros para llamar a programas en R
    NameBlock StdLib::LP    : API para el paquete R Rglpk, El interfaz de R para la librería GNU Linear Programing Kit (GLPK version 4.25)
    NameBlock StdLib::QP    : API para el paquete R quadprog para resolver problemas de Programación Cuadrática
    NameBlock StdLib::RCODA : API para el paquete R CODA, Para el análisis y el diagnóstico de cadenas de Markov de MonteCarlo (MCMC)
    
* (26-Dic-2007) Novedades en Tol v1.1.7 b.0.5.alpha:

  Soluciona los siguientes errores reportados:
    530: Nueva API extendida para la función Classify
         http://bugs.tol-project.org/show_bug.cgp?id=530
    533: La función InnerPoint hace caer a TOL en un ciclo
         http://bugs.tol-project.org/show_bug.cgp?id=533
    535: Caída de Tol con DifEq
         http://bugs.tol-project.org/show_bug.cgp?id=535
           
  Álgebra Lineal
    Nuevo método InnerPoint para encontrar un punto que cumpla un sistema de inecualciones lineales
  
  Conjuntos
    Nueva función FindIndexByName para encontrar en un conjunto indexado por nombre con SetIndexByName, la posición de un elemento conociendo su nombre
    Mejora en la comparación de conjuntos
  
  StdLib
    Nuevo NameBlock Tpa (Tol Parallel Api) para el manejo de servidores tolsh
    Nuevas funciones RecDir.Count.Line y RecDir.Count.Char para el conteo recursivo del número de líneas y caracteres de los ficheros especificados bajo un directorio
    Corregido un error en ReportAllBatteries que impedía a uptol trabajar correctamente con ciertos caminos
    
  Proyecto de Wisual C++
    Eliminación de la configuración para DUMA
    
* (10-Dic-2007) Novedades en Tol v1.1.7 b.0.4.alpha:

  Soluciona los siguientes errores reportados:
    524: La función Replace falla en determinadas circunstancias.
         http://bugs.tol-project.org/show_bug.cgp?id=524
    526: La función Datch falla en determinadas circunstancias.
         http://www.tol-project.org/show_bug.cgi?id=526
    528: Tol v1.1.6 no carga la stdlib en unix/linux.
         http://www.tol-project.org/show_bug.cgi?id=528
    
* (26-Nov-2007) Novedades en Tol v1.1.7 b.0.3.alpha

  Time
    Nueva variable Real CpuClock que devuelve el uso de CPU en segundos transcurridos desde el inicio de la sesión.
    Ahora la variable Time devuelve el tiempo real en segundos transcurridos desde el inicio de la sesión.
  
  NameBlock
    Corregido el bug 523. Los miembros de un NameBlock deben no considerarse como locales por las series temporales a la hora de borrar su fechado
    Las funciones globales deben deshabilitar el uso el NameBlock en curso para evitar accesos inválidos a sus miembros

  StdLib
    Añadida extensión a la función Msg en    
      general/grammars/_real.tol
    Corregidos errores relativos a la descomosición SVD de matrices de covarianzas en
      math/stat/models/bayesian/arima/_bmtsarima.tol
      math/stat/models/mle/arima/_fun_combination.tol
      
* (19-Nov-2007) Novedades en Tol v1.1.7 b.0.2.alpha:

  OIS
    Corregido el bug 520 mediante el parámetro opcional Real oisHasPriorityOnConflict en la función Ois.Load. 
    Si oisHasPriorityOnConflict es falso entonces, cuando exista un objeto referenciable con 
    el mismo nombre que un objeto dentro de la imagen OIS, el objeto anterior reemplazará al del OIS.
    Puede ser útil para recuperar OIS de versiones viejas en los que hayan podido quedar objetos 
    deteriorados, como por ejemplo fechados recortados en las fechas por defecto de cuando se creó 
    el OIS. Si oisHasPriorityOnConflict es cierto, que es el comportamiento por defecto, será el 
    OIS el utilizado para garantizar que se carga exactamente lo que se almacenó
    
  NameBlock
    Las funciones globales deben deshabilitar el uso el NameBlock en curso para evitar accesos inválidos a sus miembros

  Matrix 
    Corregido el bug 522. Se asegura el acceso a celdas válidas en la función Matrix Sub
    Se añade un mensaje de aviso de acceso inválido en MatDat y PutMatDat

  StdLib
    La función CheckTOLRelease debe chequear las tres posibles versiones: old-stable, official y development
    
    
* (8-Nov-2007) Novedades en Tol-1.1.6:

Resolución de bugs

    * Desde la puesta en marcha del desarrollo tras la publicación de la versión 1.1.5, se han resuelto los siguientes 45 bugs de TOL
    * Están postergados 19 bugs de TOL para decidir sobre qué hacer en cada caso.
    * Permanecen pendientes de resolución 10 bugs de TOL

Cambios internos reseñables

Los siguientes cambios introducidos en la implementación interna de TOL, le proporcionan una mayor robustez y eficiencia con respecto a la versión 1.1.5, con ahorros de entorno a un 25% de CPU que pueden alcanzar el 50% o más según el tipo de cósigo ejecutado:

    * Se han reemplazado los buscadores internos como mapeador ad-hoc de la tabla de símbolos globales, y los buscadores de funcioens especiales, estructuras, gramáticas, etc.; por el hash_map de Google que introduce mayor limpieza y rapidez en los accesos
    * Se ha acelerado el mecanismo de asignación de memoria de objetos pequeños que son la mayoría en la evaluación de TOL.
    * El tipo BDat para representar números reales se ha simplificado y homologado con el tipo double de C para facilitar su uso interno y en llamadas a librerías externas como la GSL, BLAS, etc.; al poder usar el casting directo en lugar de funciones de transformación mucho más lentas.
    * Se reducen las construcciones de objetos static en la línea de declaración C++ para evitar problemas con el orden de creación de los mismos en la carga inicial, lo cual producía a menudo errores muy difíciles de detectar.
    * Se han corregido algunos errores de tipo memory leak reduciendose el consumo de memoria en determinadas circunstancias sbre todo en ciertos ciclados largos.

Kernel

    * Cuando el resultado de evaluar la condición de las funciones If y Case da desconocido se interrumpe la evaluación y se devuelve el objeto desconocido correspondiente al tipo de datos a devolver.
    * La función Parser devuelve ahora información acerca de los paréntesis de la expresión original en cada token para permitir la creación de la función pseudo-inversa Unparse.
    * La función Text Grammar(Anything) devuelve "Struct" si se le pasa una estructura.
    * Se ha implementado, aunque está aún en periodo de pruebas, un nuevo tipo de datos llamado NameBlock que pronto será de vital importancia en el desarrollo de TOL. Para saber más acerca de este tema pulse aquí.

StdLib

La librería estándar de TOL pasa a llamarse StdLib desapareciendo la hasta ahora InitLibrary a la cual incluye por el momento de forma íntegra para asegurar la compatibilidad hacia atrás. En próximas versiones la StdLib debe alcanzar un mayor grado de madurez y organización gracias a los NameBlock y otras ventajas en el campo de la modularidad que están aún pendientes.

StdLib incluye además otra serie de nuevas incorporaciones entre las que destacan

    * Estimadores bayesianos de modelos lineales, probit y ARIMA vectoriales, todos ellos con inputs jerárquicos, información a priori y restricciones lineales.
    * Un AIA más robusto que el que se proporciona built-in con TOL.
    * La función CheckTOLRelease chequea si existen nuevas versiones de TOL para Windows y avisa al usuario de TOLBase.
    * La función Text Grammar(Anything) devuelve "Struct" si se le pasa una estructura.
    * Nuevas funciones como HasUnknown, HasPosInf, HasNegInf, HasAllFinite, CheckBST, CheckBDT, CheckBMT, IncludeLocalTol, PlainNamedObjects, y un largo etcétera.
    * Mejoras en las funciones HasUnknown, HasPosInf, HasNegInf, HasAllFinite, MaxMatrix, MinMatrix, ... 

Álgebra de conjuntos

    * Nueva función Set Append(Set old, Set newElements) para añadir elementos de una forma más eficiente que con Set old := old << newElements
    * Nueva función Set EvalSetOfCode(Set funciones, Anything arg) que devuelve el conjunto de las evaluaciones sucesivas de un conjunto de funciones tomando el mismo argumento. Se trata de la función complementaria de EvalSet
    * Nuevo operador :: de acceso por nombre a elementos de un conjunto más comodo de usar y con mejor apariencia que [""]
    * Nueva función Real SetIndexByName(Set s) para indexar los elementos de un conjunto tal que todos ellos tengan nombre único y no vacío. Una vez indexado las búsquedas con :: y [""] serán de complejidad O(1) por estar basado el índice en el hash_map de Google.

Aritmética real

    * Nuevas funciones Real IsPosInf(Real x), Real IsNegInf(Real x), Real IsFinite(Real x)
    * Nuevo comportamiento en comparaciones de desconocidos e infinitos:
      Ningún número por desconocido que sea puede ser mayor que +infinito, lo cual permite afirmar que

          ? >  +Inf == false; 
          ? <= +Inf == true; 

      Ningún número por desconocido que sea puede ser menor que -infinito, lo cual permite afirmar que

          ? <  -Inf == false; 
          ? >= -Inf == true; 

Álgebra lineal real

    * Nuevas funciones PreProdDiag y PostProdDiag para el producto virtual por matrices diagonales.
    * Nuevas funciones SubTriang y SetTriang para el almacenamiento optimizado de matrices triangulares.
    * Nueva función TrSolve basada en la función dtrsm BLAS (level-3) para la resolución eficiente de sistemas triangulares.
    * Nuevas funciones Matrix Sign(Matrix m), Matrix SetSum(Set s), Real MatMin(Matrix m), Real MatMax(Matrix m), .
    * Cuando Choleski no funciona muestra un mensaje de warning.

Interpolación y aproximación de funciones reales

Se han implementado API's para el uso en TOL de funciones de interpolación y aproximación de funciones reales

    * La API de GSL permite usar las funciones de interpolación de GSL mediante las funciones de generación escalar gsl_interp y vectorial gsl_vector_interp, con las cuales se crean a su vez funciones TOL que devuelven Real y Matrix respectivamente. Los tipos de interpolación admitidos son Lineal, Polinomial, Spline Cúbico y Spline Akima. Estos dos últimos pueden ser o no periódicos.
    * La API de AlgLib permite usar las funciones de interpolación y aproximación por mínimos cuadradosde AlgLib mediante las funciones de generación escalar AlgLib.Interp.Scalar y vectorial AlgLib.Interp.Vector, con las cuales se crean a su vez funciones TOL que devuelven Real y Matrix respectivamente. Los tipos de interpolación admitidos son Lineal, Racional Baricéntrica, Spline Cúbico y Spline Akima mientras que los de aproximación por mínimos cuadrados pueden ser Spline, Chebychev o Polinomial. AlgLib es en general más eficiente que GSL tanto en CPU como en RAM y además permite almacenar toda la información necesaria en OIS con la función AlgLib.Interp.Vector.CreateWorkSpace. Para restaurarla posteriormente se emplea AlgLib.Interp.Vector.LoadWorkSpace.

Funciones de Estadística y Probabilidad

    * Se han corregido los estadísticos R2, de Pearson y de correlación parcial
    * Nueva función UniKerDnsEstOptBnw, abreviatura de Univariate Kernel Density Estimation with Optimal Bandwith Un programa desarrollado por Vikas C. Raykar bajo la licencia Lessr GPL y disponible aquí.
    * Nueva función RandMultinomial basada en la función de GSL gsl_ran_multinomial.
    * Se ha mejorado de forma considerable el comportamiento, la variedad de argumentos admitidos y la documentación de la función GibbsConstrainedMNorm 

Input/output

Acceso a bases de datos

        * Se han resuelto varios bugs especialmente relacionados con los tipos numéricos particulares
        * EN concreto, el tipo de datos MYSQL_TYPE_NEWDECIMAL está disponible para ser tratado adecuadamente desde TOL pero sólo para la versión del servidor de MySQL MYSQL_VERSION_ID >= 50024

Ficheros

        * Se ha aumentado de 16384 a 1048576 el tamaño límite por línea en ficheros BDT, BST y BMT. Cuando el tamaño se excede salta un mensaje de error para que se solucione y no pase desapercibido.
        * Las matrices en BMT pueden ya almacenar datos desconocidos y de diferente longitud de texto para cada celda
        * Nueva función MatAppendFile para añadir filas a una matriz en formato binario BBM
        * Nueva función MatReadCell para acceder a una celda concreta de una matriz en formato binario BBM
        * Corregido el error de campo final no delimitado en BDT's

OIS

        * Para las imágenes de módulos OIS con itegridad referencial (Ois.Module) ya se almacena la información del archivo de origen de cada objeto para que sea visible desde el interfaz de TOLBase.
        * Sólo se harán globales los símbolos que no creen conflicto. En cualquier caso se aconseja la carga en ámbito local siempre que se pueda.
        * Se han deshabilitado ciertos mensajes de aviso que sólo tienen importancia a nivel de depuración interna de TOL.
        * Si existe una variable de entorno del sistema operativo de nombre Ois.DefRoot, la variable TOL del mismo nombre tomará de ella su valor, para facilitar así la configuración personalizada más adecuada en cada caso sin tocar código TOL que pueda afectar a otros usuarios.


        
* (20-Feb-2007) Novedades en Tol-1.1.5:

Acceso a bases de datos

El acceso a base de datos de Tol se compone de tres componentes: la API de funciones de usuario Tol para bases de datos.Tol-Base de datos funciona API, el conector y los drivers. Los dos últimos han sido prácticamente reescritos para esta versión.

El conector tiene una nueva puesta en práctica mejorada que asegura una conexión más confiable. Aparte de ése, ahora el conector devolverá a los usuarios los mensajes que vienen del gestor de la base de datos (BRMS) con respecto a las operaciones que se setán realizando.

Se ha creado un driver nativo nuevo para PostgreSQL, y también se ha mejorado el de MySql. Si tus proyectos usan gestores de MySQL o de PostgreSQL, te animamos a que utilices estos drivers nativos nuevos en vez del conductor más lento de ODBC-Tol, que también ha sido mejorado y corregido.

Se pueden ver ejemplos en los tests de Tol
OIS

El cambio más importante de Tol 1.1.5 es OIS, que son las siglas de Object Indexed Serialization, o sea, serialización indexada de objetos. Es un subsistema de gran alcance que proporciona una manera de ahorrar objetos vivos y recuperarlos de nuevo en otra sesión posterior o en otra máquina, incluso con diferentes sistemas operativos.

OIS fue desarrollado principalmente durante la segunda mitad de 2006 y bastante probada durante este último ciclo de desarrollo.

Esta versión oficial provee a tol de métodos de almacenaje de datos y funciones:

    * Integridad referencial implementada durante la carga de codigo fuente TOL
    * InitLibrary se ha convertido a un módulo OIS
    * Almacenamiento y recuperación de datos en imágenes OIS, con o sin integridad referencial.
    * Los Mensajes de aviso para los formatos BSI y BMM pasan a considerarse obsoletos, ya que estos formatos deberían ser reemplazados por imágenes OIS.

Siga estos enlaces para ver una introducción de OIS, el manual de usuario de OIS o algunos ejemplos de uso de OIS.
Kernel

Se ha implementado un administrador de memoria denominado Bayes Fixed Size Memory con el objetivo de mejorar el manejo masivo de objetos pequeños.

Con ello se reduce la fragmentación de la memoria y se gana en tiempo de asignación y liberación de los objetos, también se tiene mas control sobre el ciclo de vida de los objetos.

Esto es una característica transparente al usuario de TOL pero se traduce en las siguientes ventajas visibles:

    * Asegura que un objeto está vivo antes de ser usado en la pila local
    * Evita fisuras de memoria relacionadas con fechados locales asociados a time serie en circunstancias como las detectadas en los errores 224, 428, 444, 456
    * Mejoras de memoria en los métodos de BText

Par una descripción más detallada visite el documento FSMEM.odt (documentoen formato Formato ODF).
Álgebra Booleana

Una nueva implementación del álgebra booleana. Veánse los ejemplos de los tests de TOL

    * Manejo correcto de desconocidos
      And(?,0) -> false
      Or(?,1) -> true
    * Optimización de la evaluación basada en corto-circuitos booleanos:
          o Primer argmento 'false' involucrado en la evaluación del And detendrá la evaluación y retornará 'false'
          o Primer argmento 'true' involucrado en la evaluación del Or detendrá la evaluación y retornará 'true'
          o Cuando la condición del If es desconocida entonces ninguna de las ramas, verdadera o falsa (Else) es evaluada.
          o Cuando la condición del Case es desconocida entonces ninguna de sus ramas es evaluada. 

Funciones de Estadística y Probabilidad

Se ha implementado un nuevo método de AIA (Análisis de Intervención Automática). Para conocer más acerca esta característica wscrciba en la consola tol -vU -c"WriteLn(Description(AIA))" o visite los tests de TOL
Data Mining

Se ha incorporado a TOL una nueva función built-in llamada Cluster para la clasificación jerárquica de vectores.
Está basada en la librería de código abierto de métodos de clasificación estadística llamada The C Clustering Library.
Posteriormente se podrán incorporar otros métodos: k-means, Self-Organizing Maps, Principal Component Analysis.
Para ver una descripción ejecute en la consola tol -vU -c"WriteLn(Description(Cluster))"
TOLSH

Nuevo ejecutable en modo de línea de comandos capaz de llamar a TCL basado en la extension Toltcl que permite comunicar Tcl con TOL.


* (30-Nov-2006) Novedades en Tol-1.1.4:

Kernel

    * Mejora en la eficiencia de los métodos y estructuras usados para la resolución de símbolos en TOL:

          o Los elementos sintácticos detectan constantes literales (números, cadenas and fechas)
          o Los elemenos sintácticos almacenan referencias a objetos del sistema que no se pueden destruir como: nombres de tipo y funciones internas
          o Nueva implementación de estructura de pila con un árbol de búsqueda super-rápido con coste O(n) en memoria y ejecución.
    * Se admite la definición de "Struct" locales. Veáse un ejemplo en el test del bug 246
    * Integridad de las constantes del sistema: ya no se pueden modificar los objetos constantes del sistema. Cuando se hacen asignaciones de constantes se crea una copia interna del objeto de forma que no se produzcan efectos colaterales impredictibles. Ver ejemplos de uso en los tests de TOL (en construcción)
    * Paso por valor de los objetos para el operador de construcción (=) excepto para el tipo Set. Ver ejemplos de uso en los tests de TOL CreationAndCopy.
    * Los objetos TOL de usuario tienen ahora Description y Expression como funciones separadas, pero sólo la descripción es modificable por el usuario mientras que la expresión la rellenará TOL sólo cuando la variable Real  DoUnparseNodes sea cierta. Cuando no hay descripción, el usuario recibirá la  expresión, si es que existe o la cadena vacía en otro caso. La función Identify devuelve el primer texto no vacío de la lista Name(), LocalName(), Expression(), Description().

TOL-Tester

Creación de TOL-Tester: el sistema de chequeo automático de versiones de TOL. Se trata de un sistema estándar de chequeo del comportamiento del lenguaje TOL y de las herramientas relacionadas como TOLBase. Antes de publicar cada nueva versión se debe comprobar que TOL actúa como se espera lo cual no suele ser nada trivial dado el extenso número de aplicaciones del lenguaje. Lea aquí más al respecto.
Sistema

    * Nuevo argumento en la funcion interna WinSystem (Text command, Real showMode [, Real wait=FALSE]), que permite controlar la espera del proceso.
    * Nuevas funciones multiplataforma de acceso al sistema. Véase el codigo en general/_system.tol

Mensajes

    * Extendido el comportamiento de las funciones internas Write y WriteLn mediante un argumento opcional que clasifica el tipo de mensaje indicando si este  es un mensaje de error, wraning  o info. Se han añadido funciones de conveniencia a InitLibrary: Error y Warning para enviar mensajes de error y aviso respectivamente.
    * Se ha mejorado las opciones de línea de comando que controlan el tipo de salida generado por la consola de TOL. Para conocer más acerca de esta caracaterística   ejcute en su consola el comando tol -help
    * LAs funciones que retornan Anything  pueden no retornan objeto sin que esto implique un mensaje de error o aviso. También pueden ser usadas en cualquier scope.
    * El fichero de registro de TOL start.log, se ha movido a %APPDATA%\to\syslog para la plataforma Windows y ~/.tol para la plataforma unix.
    * Implementada la función PutLanguage para establecer el idioma actual de los mensajes.
    * Cada mensaje de error o de aviso llevará adjunto un número correlativo para que el usuario pueda encontrar más facilmente el primero de ellos y seguirles la pista más cómodamente.
    * Se han creado las variables TOL Real NError y Real NWarning que devuelven el número de errores y avisos emitidos hasta el momento en la sesión, para que los usuarios puedan comprobar en tiempo de ejecución si un proceso está funcionando correctamente.

Matrix

Se han implementado nuevas funciones de matrices.

    * Las funciones TOL built-in de tipo Matrix f01, F01 e Inv01 implementan la densidad, la distribución y su inversa para variables aleatorias con distribución multinormal estándar incorrelada.
    * La función TOL built-in de multiplicación de matrices celda a celda WeightProd(A,B) puede ahora escribirse como A $* B
    * Los operadores de matrices +,-,* admiten ahora como segundo elemento tanto una matriz como un número real.

Ver ejemplos de uso en los tests de TOL (en construcción)
TimeSet

La implementación del álgebra del tiempo (TimeSet) se ha mejorado considerablemente usando los conceptos descritos aquí.
Las ventajas fundamentales son:

    * Casi todos los bugs asociados a TimeSet han sido resueltos.
    * Los metodos que iteran sobre los conjuntos temporales son ahora más rápidos
    * Mejora en los métodos que precalculan el  conjunto temporal
    * Detección en un itervalo acotado de conjutnos vacíos para evitar ciclos infinitos. En tal caso se emite un mensaje de aviso.
    * La fecha de inicio se ha establecido a 31-dic-1582 (El calendario gregoriano comienza el 15-oct-1582)

Ver ejemplos en los tests de TOL.
Serie

    * Las series que se generan a  partir de un formato BDT ya no tienen conportamiento retardado (Lazy).
    * Nueva función interna Serie EvalSerie(Serie S, Code realToRealFunction)

Text

    * Mejora de las fuciones: BeginWith, EndAt, Reverse
    * Nuevas variable de iformación del sistema: TOLSessionPath and TOLVersion

Funciones de Probabilidad y Estadística

    * Mejora del tratamiento del desconocido ? en las funciones de distribución.
    * Nuevos métodos más rápidos para distribución normal truncada:
          o Scalar: DensMultiNormalTrunc, DensTruncatedNormal, DistTruncatedNormal, DistTruncatedNormalInv, DrawTruncatedNormal,
          o Vectorial: RandTruncatedMultNormal, LogDensTruncatedMultNormal
    * Nuevos métodos Markov Chain Monte Carlo (MCMC) para el muestreo de distribuciones de probabilidad definidas implícitamente mediante las funciones built-in GibbsSampler y MetropolisHastings. Para ver una descripción de estas funciones ejecute en la consola tol -vU -c"WriteLn(Description(GibbsSampler))" -c"WriteLn(Description(MetropolisHastings))"
    * Nueva función para realizar una regresión lineal sobre matrices sparse: SparseLinnearRegression. Para ver una descripción de la función ejecute en la consola tol -vU -c"WriteLn(Description(SparseLinearRegression))". La definición de una matriz sparse se base en la especificación descrita en tolsparse.txt
    * Corregidos bugs asociados a la definición, nombrado  uso de los estadísticos R2, Pearson, y Correlation
    * Nuevo conjuno de funciones internas de interfaz con funciones de GSL que retorna tipo Real y tambien sus argumentos son de tipo Real. Lea más en el siguiente enlace.
    * Mejoras en la función Estimate (Estimador máximo-verosímil de modelos ARIMAX):
          o En la estimación de omitidos, cuando  en el paso actual se obtienen peor resultado que el paso anterior entonces los actuales no se consideran y se mantienen los previos.
          o Se genera un error si la estructura de definición del modelo no tiene InputDef definida.
          o En la función ARMAAutoCovarianzeVector utilizada internamente por Estimate había un acceso inválido de memoria cuando no hay parte AR en un modelo y se solicita un número de autocovarianzas menor o igual que el grado MA, lo cual podía dar lugar a caídas o comportamientos extraños de forma aleatoria e irreproducible.


          
* (16-Feb-2005) What is new in Tol-1.1.3:

** New user function to generate a markov chain for a truncated multivariate
   normal, the truncation region given by a set of linear constraints Bx <=
   b. The functions are:

	GibbsConstrainedMNormal -- to obtain the whole chain.
	
	RandConstrainedMNormal -- to obtain only the last sample in the
	chain.

** User function RandIChisq now accept a scale parameter in order to simulate
   a scaled inversed chi-squared.

** New Tol feature: User Functions Overloading.

   Now Tol let us create more than one function with the same name, but
   returning different data types. What does this new feature mean?

   We can write code like this below:
     ____________________________
     Real func (Real param) { param };
     Text func (Text param) { param };
     Real a = func(12);
     Text b = func("It Works!");

   After execute it, both variables, "Real a" and "Text b", has their own
   respective values. In both of these sentences, Tol can get the Type involved
   in the operation, reading the Left-side sentence. But sometimes, when we
   give a User Function as an argument for another function, Tol can not
   recognise which is the Tol Type associated with the overloaded function we
   need. In those cases Tol returns the first function encountered. 

   Next example shows that particular case:
     _______________________________________________________________________
     Set  getNameSP(Text vorname, Text name) { SetOfAnything(vorname, name) };
     Set  getNameUK(Text vorname, Text name) { SetOfAnything(name, vorname) };
     Text getNameSP(Text vorname, Text name) { vorname + " " + name };
     Text getNameUK(Text vorname, Text name) { name + ", " + vorname };
     Text names(Text vorname, Text name, Code fun) { fun(vorname, name) };

     Text aText = names("Peter", "Jackson", Text getNameSP);

   There are 5 functions. Two of them called "getNameSP" returning a Set and a
   Text. Other two more called "getNameUK" returning also a Set and a
   Text. And the last of them called "names" and returning a Text. This last
   one receive a Code as one of their arguments.
   Then function "names" is called using  "Text getNameSP" as its 3th
   argument. If this argument was given without a particular type, Tol will
   try to get the first function created that was called "getNameSP", that
   would be "Set getNameSP" function.

   Other example of use of this new feature is the possibility to create a
   User Function with the same name as a Built-In function:

     _____________________________________________________
     Set DensCauchy(Set s) 
       { Set EvalSet(s, Real (Real e) { DensCauchy(e) }) };

     Set sIn = SetOfReal(1,2,3,4);
     Set sOut = DensCauchy(sIn);
   
   DensCauchy is a Built-In Real Type function, but in this example we have
   created a User Function called also DensCauchy. We can create a new User
   Function called as other one, always that the new one returns something
   different than the old one.

** MakeGlobal changes.
   
   MakeGlobal function let us export more than one local variable to the
   global scope, and in every place of the local code. Until now we can export
   only one local variable and it must be related with the result
   returned. Now we can execute code like this:
   _______________________
   Real func(Real param) {
     Real locvar1 = 10;
     MakeGloba(locvar1);  // exporting to global scope
     Real locvar2 = 20;   // exporting to global scope
     MakeGlobal(locvar2);
     param
   };	    
   Real x = 1;
   Real y = func(x);
   Real a = locvar1;  // now, after func call, locvar1 is global
   Real b = locvar2;  // now, after func call, locvar2 is global

** Dynamic Scope avaliable at compile time.

   Like many common languages, Tol uses a Lexical implementation of the
   Scope. But at the beginning, Tol had another implementation: a Dynamic
   Scope, meaning that a code like this:
     _______________________________
     Real r = 4;
     Real fun(Real q) {q+r};
     Real b = { Real r = 1; fun(8) };

   Assigns 9 to variable "b".

   A Dynamic Scope means that each time Tol needs a variable, looks for it
   into the calls stack.
   To enable Dynamic Scope you must compile Tol with --enable-DS (under
   Linux/Unix) or defining a macro called __USE_DYNSCOPE__ in compile time.

** External compilation of TOL DB Access using --enable-dbdrivers

   New configure option --enable-dbdrivers force Tol to be compiled with
   support for direct access to databases. In order to acomplish this we can
   specify several configure command line options. If you don't give such
   options, configure looks for the sources and libraries that needs to build
   Tol properly with DB Direct Access Support. 
   Check INSTALL file to know which options command line options are related
   with this new feature.

** Fixed bugs: 165, 171, 184, 191, 192, 194, 197, 199, 205, 214, 215, 216,
   218, 219, 223, 225, 226, 227, 229, 230, 233, 237, 240, 246, 247, 249

   You can access them directly using next URL:

http://bugs.tol-project.org/buglist.cgi?product=TOL&bug_status=RESOLVED&resolution=FIXED&bugidtype=include&chfield=resolution&chfieldfrom=2004-10-27&chfieldto=2005-02-15&cmdtype=doit

----------------------------------------------------------------------

* What is new in Tol-1.1.2:

** Tol is much more stable now than few months ago. We have resolved more than
   100 bugs since the first version, but Tol is still an unstable
   product, for many reasons. One of them is related with the Documentation
   Project, we are still without manuals, tutorials, technical
   reports...
   On the other side, Tol continues having some implementation problems with
   itsinternal modules, which are blocking all possibilities to release an
   stable product before their correction.
   Among the new features list for this version, we can emphasize:

** TIT: Tol Integrity Tests
   In the main directory yoo can see a new file called test-it.tol. Running
   this program you can start a global language consistency test across the
   directory tree. The program looks for "test" directories where we are
   adding individual tests to verify specific features of Tol.

** The Evalutor
   It does not allow creating variables using an identifier allready used by
   other variable of a different type.
   A great number of corrections have been applied to the Evaluation Engine.

** The Parser
   Improves its behavior with a better recognition of comments inside code,
   giving more complete information about the places where syntactical errors
   are.On the other side, the maximal identifier length has been limited to
   255.

** Random Number Generation
   Random and distributed number generator has been added, based on:
    * Cauchy  (RandCaucy)
    * LogNormal (RandLogNormal)

** Tcl_Eval implemented, as taken from the function description

	Set Tcl_Eval(Text script)

	Evaluate a Tcl script. The return value is a Set with two elements
	    status : a Real indicating success (1) or fail (0)
	    result : a Text with the evaluation result
	Example:

	    Set tcl_res = Tcl_Eval("set tcl_platform(platform)");
	    If (tcl_res["status"], WriteLn("Running on platform
	    "+tcl_res["result"]), WriteLn("Error in Tcl_Eval"))

** Database access
   TOL DDBB API has been re-programmed to open up the possibility of using
   multiple native drivers. At the same time, ODBC and MySQL drivers have been
   developed. Of course, no changes have been made to the TOL DB API, and
   a big change: Dynamic libraries saves some memory if a driver is not used.

** There are many little changes that we can't list here. Nevertheless we can
   give the URL of TOL Bug Report System (based on Bugzilla) to track all bugs
   which have been resolved:
   http://bugs.tol-project.org/buglist.cgi?bug_status=RESOLVED&resolution=FIXED

----------------------------------------------------------------------

* What was new in Tol-1.1.1:

** New estimation engine:
  -Maximum likelihood estimation of ARIMA models
  -Non linear least squares method improvement

** New user function QRDecomposition to implement QR decomposition of a
   matrix.

** New random generators from distributions:
   -RandChisq : random variate from a xi-squared distribution with n degree of
    freedom.
   -RandIChisq : random variate from an inverse xi-squared distribution with n
    degree of freedom.
   -RandGamma : random variate from a gamma distribution with parameters a and
    b.

** Changed version numbers to unify all packages: Tol, Toltcl and Tolbase

** Bugs fixed:
  -BMTFile(Set) (http://bugs.tol-project.org/show_bug.cgi?id=2)
  -CholeskiInverse (http://bugs.tol-project.org/show_bug.cgi?id=3)
  -QuantileS (http://bugs.tol-project.org/show_bug.cgi?id=4)
  -configure checks GSL version (http://bugs.tol-project.org/show_bug.cgi?id=5)
  -Timing traces (http://bugs.tol-project.org/show_bug.cgi?id=15)
  -If sentence (http://bugs.tol-project.org/show_bug.cgi?id=18)


* What was new in Tol-0.1.9:

** Changed project structure in order to put under gnu autoconf utilities
Now support autoconf shell script "configure" to build the library

** tol library become a new Open Source project under gnu/GPL license terms.
TOL was an internal project developed from 1995 at Bayes Decisión, SA 
(Spain, Madrid). Is the base software used with TOLBase, another open source
project. TOLBase is a user interface written in tcl/tk for programming in tol.
