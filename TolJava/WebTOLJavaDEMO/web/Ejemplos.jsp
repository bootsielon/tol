<?xml version="1.0" encoding="UTF-8"?>
<!-- 
    Document   : Docs
    Created on : Nov 28, 2010, 1:55:24 PM
    Author     : hcarralero
-->
<jsp:root version="2.1" xmlns:f="http://java.sun.com/jsf/core" xmlns:h="http://java.sun.com/jsf/html" xmlns:jsp="http://java.sun.com/JSP/Page" xmlns:webuijsf="http://www.sun.com/webui/webuijsf">
    <jsp:directive.page contentType="text/html;charset=UTF-8" pageEncoding="UTF-8"/>
    <f:view>
        <webuijsf:page id="page1">
            <webuijsf:html id="html1">
                <webuijsf:head id="head1" title="TOLJava-WebDEMO==&gt; Programas Ejemplos">
                    <webuijsf:link id="link1" url="/resources/stylesheet.css"/>
                    <webuijsf:script type="text/javascript" url="sh/scripts/shCore.js"/>
                    <webuijsf:script type="text/javascript" url="sh/scripts/shBrushJava.js"/>
                    <webuijsf:link id="link2" rel="stylesheet" type="text/css" url="sh/styles/shCore.css"/>
                    <webuijsf:link id="link3" rel="stylesheet" type="text/css" url="sh/styles/shThemeDefault.css"/>
                </webuijsf:head>
                <webuijsf:body id="body1" style="background-color: rgb(224, 224, 255); -rave-layout: grid">
                    <webuijsf:form id="form1">
                        <div style="left: 0px; top: 0px; position: absolute; width: 100%">
                            <jsp:directive.include file="Encabezado.jspf"/>
                        </div>
                        <div style="left: 2px; top: 68px; position: absolute; width: 200px">
                            <jsp:directive.include file="MenuIzquierdo.jspf"/>
                        </div>
                        <h:panelGrid id="pnlDocs" style="border: 1px solid navy; left: 202px; top: 68px; position: absolute">
                            <webuijsf:staticText id="staticText1"
                                style="border-bottom: solid navy 1px; background-color: rgb(204, 204, 255); color: navy; font-size: 18px; font-variant: small-caps; font-weight: bolder; height: 28px; text-align: center; width: 100%" text="Proyecto TOLJava: Programas de Ejemplo para Consola"/>
                            <h:panelGrid id="gridPanel1" style="border-top: solid navy 1px; height: 4px; width: 100%"/>
                            <webuijsf:tabSet id="tabSet1" selected="tab1">
                                <webuijsf:tab id="tab1" text="Ejemplo #1">
                                    <h:panelGrid id="gridPanel2">
                                        <webuijsf:staticText id="staticText4"
                                            style="font-size: 12px; font-weight: bolder; margin-left: 10px; text-decoration: underline" text="Verificación del acceso a TOL"/>
                                        <webuijsf:staticText id="staticText5" style="font-size: 12px; margin-left: 10px; text-align: justify" text="En este programa se muestra cómo chequear que se tiene acceso a una instancia de TOL."/>
                                        <script class="brush: java; toolbar: false;" type="syntaxhighlighter"><![CDATA[
                                            package com.hcarralero.toljava.ejemplos;

                                            //Se deben importar del paquete com.hcarralero.toljava todas las clases a usarse,
                                            //para este ejemplo sólo se requiere la clase TOLMachine
                                            import com.hcarralero.toljava.TOLMachine;

                                            /**
                                             * Primer Ejemplo de TOLJava para Consola:
                                             *  1) Se crea una instancia de TOLMachine.
                                             *  2) Se ejecuta una sentencia TOL.
                                             *  3) Se procesa el resultado.
                                             *  4) Se muestran los mensajes devueltos por el intéprete de TOL.
                                             *
                                             * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
                                             * @version 1.0
                                             * @see com.hcarralero.toljava.TOLMachine
                                             */
                                            public class Ejemplo01 {

                                                /**
                                                 * @param args los argumenetos de la línea de comandos.
                                                 */
                                                public static void main(String[] args) {
                                                    //Se crea una instancia de la clase TOLMachine.
                                                    TOLMachine tm= new TOLMachine();
                                                    //En una cadena se almacena el código TOL que se ejecutará.
                                                    String code= "Write(\"Hola. Bienvenido a TOLJava.\")";
                                                    //Se invoca el intérprete de TOL para ejecutar el código pasado por parámetro.
                                                    //El resultado es un arreglo de cadenas Object.
                                                    Object res[]= tm.execute(code);
                                                    System.out.println();
                                                    if (res==null) {
                                                        //Si el resultado es nulo, entonces se ha producido un erroe en el acceso a TOL.
                                                        System.out.println("ERROR ACCEDIENDO A TOL->execute");
                                                    } else {
                                                        //Se ha logrado acceder al intérprete y ejecutar el código.
                                                        int ne= 0;
                                                        try {
                                                            //La primera cadena representa el número de errores detectado por el Intérprete de TOL.
                                                            ne= Integer.parseInt((String)res[0]);
                                                            if (ne>0) {
                                                                //El intérprete de TOL reportó al menos un error.
                                                                System.out.println("Se ha(n) detectado "+ne+" error(es)");
                                                            } else {
                                                                //El intérprete de TOL no reportó errores.
                                                                System.out.println("No se han detectado errores");
                                                            }
                                                            //Los siguientes objetos que conforman la respuesta del intérprete pueden ser:
                                                            //Warnings, Errores o mensajes generados por el código TOL ejecutado.
                                                            //Para procesar los mensajes generados debe comprender el formato de
                                                            //estos mensajes. (Consulte la documentación de TOL).
                                                            for (int i= 1; i<res.length; ++i) {
                                                                //Cada objeto es una cadena.
                                                                String strRes= (String)res[i];
                                                                //Se obtiene la longitud de la cadena.
                                                                int l= strRes.length();
                                                                if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                                                                    //En este caso el mensaje es un 'W'arning o un 'E'rror.
                                                                    System.out.println("\t"+strRes.substring(3, l-5));
                                                                } else {
                                                                    //En este caso es un mensaje común.
                                                                    System.out.println("\t"+strRes);
                                                                }
                                                            }
                                                        } catch (NumberFormatException ex) {
                                                            //Algo inusual (ERROR INTERNO DE TOLJava) ha ocurrido.
                                                            System.out.println("ERROR DE ACCESO AL NÚMERO DE ERRORES");
                                                        }
                                                    }
                                                }

                                            }
                                        ]]></script>
                                        <webuijsf:script type="text/javascript">
                                            SyntaxHighlighter.all()
                                        </webuijsf:script>
                                    </h:panelGrid>
                                </webuijsf:tab>
                                <webuijsf:tab id="tab2" text="Ejemplo #2">
                                    <h:panelGrid id="gridPanel3">
                                        <webuijsf:staticText id="staticText3"
                                            style="font-size: 12px; font-weight: bolder; margin-left: 10px; text-decoration: underline" text="Acceso a Variables de TOL"/>
                                        <webuijsf:staticText id="staticText6" style="font-size: 12px; margin-left: 10px; text-align: justify" text="Este programa muestra cómo definir variables en TOL, acceder a ellas para obtener su valor, así como cambiar el mismo."/>
                                        <script class="brush: java; toolbar: false;" type="syntaxhighlighter"><![CDATA[
                                            package com.hcarralero.toljava.ejemplos;

                                            //Se deben importar del paquete com.hcarralero.toljava todas las clases a usarse,
                                            //para este ejemplo se requieren las clases: TOLMachine, TOLMachineException, TOLReal y TOLText.
                                            import com.hcarralero.toljava.TOLMachine;
                                            import com.hcarralero.toljava.TOLMachineException;
                                            import com.hcarralero.toljava.TOLReal;
                                            import com.hcarralero.toljava.TOLText;
                                            /**
                                             * Segundo Ejemplo de TOLJava para Consola:
                                             *  1) Se crea una instancia de TOLMachine.
                                             *  2) Se ejecuta una sentencia TOL para definir las variables x (Text) e y (Real).
                                             *  3) Se procesa el resultado.
                                             *  4) Se muestran los mensajes devueltos por el intéprete de TOL.
                                             *  5) Se obtiene el valor de la variable x.
                                             *  6) Se muestra el valor de la variable x.
                                             *  7) Se cambia el valor de la variable x por un nuevo valor.
                                             *  8) Se muestra el nuevo valor de la variable x.
                                             *  9) Se obtiene el valor de la variable y.
                                             *  10) Se muestra el valor de la variable y.
                                             *  11) Se cambia el valor de la variable y por un nuevo valor.
                                             *  12) Se muestra el nuevo valor de la variable y.
                                             *
                                             * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
                                             * @version 1.0
                                             * @see com.hcarralero.toljava.TOLMachine, com.hcarralero.toljava.TOLMachineException,
                                             *      com.hcarralero.toljava.TOLReal, com.hcarralero.toljava.TOLText
                                             */
                                            public class Ejemplo02 {

                                                /**
                                                 * @param args los argumenetos de la línea de comandos.
                                                 */
                                                public static void main(String[] args) {
                                                    //Se crea una instancia de la clase TOLMachine.
                                                    TOLMachine tm= new TOLMachine();
                                                    //En una cadena se almacena el código TOL que se ejecutará; en este caso
                                                    //el código es para definir las variables x de tipo Text e y de tipo Real.
                                                    String code= "Text x=\"Hola. Bienvenido a TOLJava.\"; Real y= 3.5+7;";
                                                    //Se invoca el intérprete de TOL para ejecutar el código pasado por parámetro.
                                                    //El resultado es un arreglo de cadenas Object.
                                                    Object res[]= tm.execute(code);
                                                    System.out.println();
                                                    if (res==null) {
                                                        //Si el resultado es nulo, entonces se ha producido un error en el acceso a TOL.
                                                        System.out.println("ERROR ACCEDIENDO A TOL->execute");
                                                    } else {
                                                        //Se ha logrado acceder al intérprete y ejecutar el código.
                                                        int ne= 0;
                                                        try {
                                                            //La primera cadena representa el número de errores detectado por el Intérprete de TOL.
                                                            ne= Integer.parseInt((String)res[0]);
                                                            if (ne>0) {
                                                                //El intérprete de TOL reportó al menos un error.
                                                                System.out.println("Se ha(n) detectado "+ne+" error(es)");
                                                            } else {
                                                                //El intérprete de TOL no reportó errores.
                                                                System.out.println("No se han detectado errores");
                                                            }
                                                            //Los siguientes objetos que conforman la respuesta del intérprete pueden ser:
                                                            //Warnings, Errores o mensajes generados por el código TOL ejecutado.
                                                            //Para procesar los mensajes generados debe comprender el formato de
                                                            //estos mensajes. (Consulte la documentación de TOL).
                                                            for (int i= 1; i<res.length; ++i) {
                                                                //Cada objeto es una cadena
                                                                String strRes= (String)res[i];
                                                                //Se obtiene la longitud de la cadena.
                                                                int l= strRes.length();
                                                                if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                                                                    //En este caso el mensaje es un 'W'arning o un 'E'rror.
                                                                    System.out.println("\t"+strRes.substring(3, l-5));
                                                                } else {
                                                                    //En este caso es un mensaje común.
                                                                    System.out.println("\t"+strRes);
                                                                }
                                                            }
                                                            //Se obtiene el valor de la variable x.
                                                            Object objx= tm.getVariable("x");
                                                            //Se imprime el valor de la variable x.
                                                            System.out.println("Valor de x:["+objx+"]");
                                                            //Se asigna un nuevo valor a x, el texto: "Hasta la vista TOL."
                                                            tm.setVariableValue( "x", new TOLText("Hasta la vista TOL."));
                                                            //Se obtiene nuevamente el valor de la variable x (ya modificado).
                                                            objx= tm.getVariable("x");
                                                            //Se imprime el nuevo valor de la variable x.
                                                            System.out.println("NUEVO Valor de x:["+objx+"]");
                                                            //Se obtiene el valor de la variable y.
                                                            Object objy= tm.getVariable("y");
                                                            //Se imprime el valor de la variable y.
                                                            System.out.println("Valor de y:["+objy+"]");
                                                            //Se asigna un nuevo valor a y, el número: 4567.89.
                                                            tm.setVariableValue( "y", new TOLReal(4567.89));
                                                            //Se obtiene nuevamente el valor de la variable y (ya modificado).
                                                            objy= tm.getVariable("y");
                                                            //Se imprime el nuevo valor de la variable y.
                                                            System.out.println("NUEVO Valor de y:["+objy+"]");
                                                        } catch (NumberFormatException ex) {
                                                            //Algo inusual (ERROR INTERNO DE TOLJava) ha ocurrido.
                                                            System.out.println("ERROR DE ACCESO AL NÚMERO DE ERRORES");
                                                        } catch (TOLMachineException ex) {
                                                            //Algún error ha ocurrido en el acceso a las variables de TOL.
                                                            System.out.println("ERROR procesando la información introducida: "+ex.getMessage());
                                                        }
                                                    }
                                                }
                                            }
                                        ]]></script>
                                        <webuijsf:script type="text/javascript">
                                            SyntaxHighlighter.all()
                                        </webuijsf:script>
                                    </h:panelGrid>
                                </webuijsf:tab>
                                <webuijsf:tab id="tab3" text="Ejemplo #3">
                                    <h:panelGrid id="gridPanel4">
                                        <webuijsf:staticText id="staticText7"
                                            style="font-size: 12px; font-weight: bolder; margin-left: 10px; text-decoration: underline" text="Obtención de un listado de variables definidas en TOL"/>
                                        <webuijsf:staticText id="staticText8" style="font-size: 12px; margin-left: 10px; text-align: justify" text="Este programa muestra cómo se puede obtener un listado de las variables de un tipo (Text, Real, Complex, etc.) que están definidas en TOL."/>
                                        <script class="brush: java; toolbar: false;" type="syntaxhighlighter"><![CDATA[
                                            package com.hcarralero.toljava.ejemplos;

                                            //Se deben importar del paquete com.hcarralero.toljava todas las clases a usarse,
                                            //para este ejemplo se requieren las clases: TOLMachine y TOLVariable.
                                            import com.hcarralero.toljava.TOLMachine;
                                            import com.hcarralero.toljava.TOLVariable;
                                            /**
                                             * Tercer Ejemplo de TOLJava para Consola:
                                             *  1) Se crea una instancia de TOLMachine.
                                             *  2) Se ejecuta una sentencia TOL para definir las variables mensaje1 (Text),
                                             *     mensaje2 (Text), mensaje3 (Text), num1(Real), b (real) y fnac (Date).
                                             *  3) Se procesa el resultado.
                                             *  4) Se muestran los mensajes devueltos por el intéprete de TOL.
                                             *  5) Se obtiene un arreglo con las varibale de tipo Text (podría ser similar
                                             *     para cualquier otro tipo de dato.
                                             *  6) Se recorre el arreglo obtenido y se muestra por pantalla.
                                             *
                                             * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
                                             * @version 1.0
                                             * @see com.hcarralero.toljava.TOLMachine, com.hcarralero.toljava.TOLVariable
                                             */
                                            public class Ejemplo03 {

                                                /**
                                                 * @param args los argumenetos de la línea de comandos.
                                                 */
                                                public static void main(String[] args) {
                                                    //Se crea una instancia de la clase TOLMachine.
                                                    TOLMachine tm= new TOLMachine();
                                                    //En una cadena se almacena el código TOL que se ejecutará; en este caso
                                                    //el código es para definir las variables mensaje1 (Text),
                                                    //mensaje2 (Text), mensaje3 (Text), num1(Real), b (real) y fnac (Date).
                                                    String code= "Text mensaje1= \"Ejecutando el Test #3\";Text mensaje2=\"Hola.\"; Text mensaje3=\"Bienvenido a TOLJava.\"; Real num1= 3.5+7; Real b= 56.9; Date fnac=y1967m02d04";
                                                    Object res[]= tm.execute(code);
                                                    //Se invoca el intérprete de TOL para ejecutar el código pasado por parámetro.
                                                    //El resultado es un arreglo de cadenas.
                                                    System.out.println();
                                                    if (res==null) {
                                                        //Si el resultado es nulo, entonces se ha producido un error en el acceso a TOL.
                                                        System.out.println("ERROR ACCEDIENDO A TOL->execute");
                                                    } else {
                                                        //Se ha logrado acceder al intérprete y ejecutar el código.
                                                        int ne= 0;
                                                        try {
                                                            //La primera cadena representa el número de errores detectado por el Intérprete de TOL.
                                                            ne= Integer.parseInt((String)res[0]);
                                                            if (ne>0) {
                                                                //El intérprete de TOL reportó al menos un error.
                                                                System.out.println("Se ha(n) detectado "+ne+" error(es)");
                                                            } else {
                                                                //El intérprete de TOL no reportó errores.
                                                                System.out.println("No se han detectado errores");
                                                            }
                                                            //Los siguientes objetos que conforman la respuesta del intérprete pueden ser:
                                                            //Warnings, Errores o mensajes generados por el código TOL ejecutado.
                                                            //Para procesar los mensajes generados debe comprender el formato de
                                                            //estos mensajes. (Consulte la documentación de TOL).
                                                            for (int i= 1; i<res.length; ++i) {
                                                                //Cada objeto es una cadena
                                                                String strRes= (String)res[i];
                                                                //Se obtiene la longitud de la cadena.
                                                                int l= strRes.length();
                                                                if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                                                                    //En este caso el mensaje es un 'W'arning o un 'E'rror.
                                                                    System.out.println("\t"+strRes.substring(3, l-5));
                                                                } else {
                                                                    //En este caso es un mensaje común.
                                                                    System.out.println("\t"+strRes);
                                                                }
                                                            }
                                                            //Se obtiene un listado de las variables tipo Text definidas en TOL.
                                                            TOLVariable vars[]= tm.getTextVariables();
                                                            if (vars==null) {
                                                                //Algo inusual (ERROR INTERNO DE TOLJava) ha ocurrido.
                                                                System.out.println("ERROR DE ACCESO AL MÉTODO NATIVO: nativeGetTextVariables");
                                                            } else {
                                                                //Se obtiene el número de variables tipo Text.
                                                                int nv= vars.length;
                                                                if (nv>0) {
                                                                    //En este caso se ha definido al menos una variable.
                                                                    System.out.println("Se han definido "+nv+" variable(s) de tipo Text:");
                                                                    //Iteramos sobre el arreglo de variables tipo Text definidas.
                                                                    for (int i= 0; i<nv; ++i) {
                                                                        //Cada elemento del arreglo es una variable.
                                                                        TOLVariable var= vars[i];
                                                                        //Nombre de la variable, se inicializa con un valor "basura".
                                                                        String name= "NO_NAME";
                                                                        //Valor de la variable, se inicializa con un valor "basura".
                                                                        Object value= "NO_VALUE";
                                                                        //Descripción de la variable, se inicializa a un valor "basura".
                                                                        String desc= "NO_DESC";
                                                                        if (var!=null) {
                                                                            //La variable no es NULA (esto es lo CORRECTO).
                                                                            if (var.getName()!=null) {
                                                                                //Obtenemos el nombre de la variable (NO DEBERÍA SER NULO).
                                                                                name= var.getName();
                                                                            }
                                                                            if (var.getValue()!=null) {
                                                                                //Obtenenos el valor de la variable.
                                                                                value= var.getValue();
                                                                            }
                                                                            if (var.getDesc()!=null) {
                                                                                //Obtenemos la descripción (Pidiera ser vacía.
                                                                                desc= var.getDesc();
                                                                            }
                                                                        }
                                                                        //Se imprimen todos los atributos de la variabe: nombre. valor y descripción.
                                                                        System.out.println("\t["+name+"]==>["+value.toString()+"]==>["+desc+"]");
                                                                    }
                                                                } else {
                                                                    System.out.println("No se han definido Variables tipo Text");
                                                                }
                                                            }
                                                            //Para hacer este proceso para otro tipo de variables, sólo debe usar el método adecuado; los
                                                            //posibles métodos son: getRealVariables(), getComplexVariables(), ...;
                                                        } catch (NumberFormatException ex) {
                                                            //Algo inusual (ERROR INTERNO DE TOLJava) ha ocurrido.
                                                            System.out.println("ERROR DE ACCESO AL NÚMERO DE ERRORES");
                                                        }
                                                    }
                                                }

                                            }
                                        ]]></script>
                                        <webuijsf:script type="text/javascript">
                                            SyntaxHighlighter.all()
                                        </webuijsf:script>
                                    </h:panelGrid>
                                </webuijsf:tab>
                                <webuijsf:tab id="tab4" text="Ejemplo #4">
                                    <h:panelGrid id="gridPanel5">
                                        <webuijsf:staticText id="staticText9"
                                            style="font-size: 12px; font-weight: bolder; margin-left: 10px; text-decoration: underline" text="Manejo de Objetos tipo TOLVMatrix"/>
                                        <webuijsf:staticText id="staticText10" style="font-size: 12px; margin-left: 10px; text-align: justify" text="Este programa muestra cómo acceder desde Java a TOL y manejar variables de tipo TOLVMatrix; se ilustra como crear variables tipo VMatrix y como acceder a ellas para procesar sus datos en Java."/>
                                        <script class="brush: java; toolbar: false;" type="syntaxhighlighter"><![CDATA[
                                            package com.hcarralero.toljava.ejemplos;

                                            //Se deben importar del paquete com.hcarralero.toljava todas las clases a usarse,
                                            //para este ejemplo se requieren las clases: TOLMachine, TOLMachineException, TOLVMatrix y TOLVariable.
                                            import com.hcarralero.toljava.TOLMachine;
                                            import com.hcarralero.toljava.TOLMachineException;
                                            import com.hcarralero.toljava.TOLVMatrix;
                                            import com.hcarralero.toljava.TOLVariable;

                                            /**
                                             * Cuarto Ejemplo de TOLJava para Consola:
                                             *  1) Se crea una instancia de TOLMachine.
                                             *  2) Se ejecuta una sentencia TOL para definir 3 variables de tipo VMatrix: ma, mb y mc
                                             *  3) Se procesa el resultado para determinar si se produjo algún error.
                                             *  4) Se obtiene la variable "mc" definida en TOL y de ella se toma la matriz de sus valores.
                                             *  4) Se recorre la matriz obtenida y se muestra por pantalla.
                                             *
                                             * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
                                             * @version 1.0
                                             * @see com.hcarralero.toljava.TOLMachine, com.hcarralero.toljava.TOLVariable
                                             */
                                            public class Ejemplo04 {

                                                /**
                                                 * @param args los argumenetos de la línea de comandos.
                                                 */
                                                public static void main(String[] args) {
                                                    //Se crea una instancia de la clase TOLMachine.
                                                    TOLMachine tm= new TOLMachine();
                                                    //En una cadena se almacena el código TOL que se ejecutará; en este caso
                                                    //el código es para definir las variables tipo VMatrix: ma, mb y mc.
                                                    String code= "VMatrix ma= Rand(4, 3, 0, 1); VMatrix mb= SetCol( [[2,3,4,7]] ) | SetCol( [[1,5,7,9]] ) | SetCol( [[3,2,0,1]] ); VMatrix mc=ma+mb;";
                                                    Object res[]= tm.execute(code);
                                                    //Se invoca el intérprete de TOL para ejecutar el código pasado por parámetro.
                                                    //El resultado es un arreglo de cadenas.
                                                    System.out.println();
                                                    if (res==null) {
                                                        //Si el resultado es nulo, entonces se ha producido un error en el acceso a TOL.
                                                        System.out.println("ERROR ACCEDIENDO A TOL->execute");
                                                    } else {
                                                        //Se ha logrado acceder al intérprete y ejecutar el código.
                                                        int ne= 0;
                                                        try {
                                                            //La primera cadena representa el número de errores detectado por el Intérprete de TOL.
                                                            ne= Integer.parseInt((String)res[0]);
                                                            if (ne>0) {
                                                                //El intérprete de TOL reportó al menos un error.
                                                                System.out.println("Se ha(n) detectado "+ne+" error(es)");
                                                            } else {
                                                                //El intérprete de TOL no reportó errores.
                                                                System.out.println("No se han detectado errores");
                                                                //Se obtiene de TOL la variable mc y se muestra por pantalla.
                                                                TOLVariable varmc= tm.getVariable("mc");
                                                                //Se hace cast al valor de la variable para convertirlo al tipo correcto: VMatrix.
                                                                TOLVMatrix valmc= (TOLVMatrix)varmc.getValue();
                                                                //Se obtiene el número filas y el de columnas de la matriz.
                                                                int rows= valmc.getRows();
                                                                int cols= valmc.getColumns();
                                                                double values[][]= valmc.getValues();
                                                                for (int i= 0; i<rows; ++i) {
                                                                    //Se cambia de línea antes de cada fila
                                                                    System.out.println();
                                                                    for (int j= 0; j<cols; ++j) {
                                                                        //Se imprime cada celda de la matriz
                                                                        System.out.print(values[i][j]+"\t");
                                                                    }
                                                                }
                                                            }
                                                        } catch (NumberFormatException ex) {
                                                            //Algo inusual (ERROR INTERNO DE TOLJava) ha ocurrido.
                                                            System.out.println("ERROR DE ACCESO AL NÚMERO DE ERRORES");
                                                        } catch (TOLMachineException ex) {
                                                            //Se ha producido un error en el acceso a la Máquina Virtual de TOL.
                                                            System.out.println("Error accediendo a TOL: "+ex.getMessage());
                                                        }
                                                    }
                                                }
                                            }]]></script>
                                        <webuijsf:script type="text/javascript">
                                            SyntaxHighlighter.all()
                                        </webuijsf:script>
                                    </h:panelGrid>
                                </webuijsf:tab>
                                <webuijsf:tab id="tab5" text="Ejemplo #5">
                                    <h:panelGrid id="gridPanel6">
                                        <webuijsf:staticText id="staticText12"
                                            style="font-size: 12px; font-weight: bolder; margin-left: 10px; text-decoration: underline" text="Utilización de Variable de tipo TOLCode y llamada a funciones en TOL"/>
                                        <webuijsf:staticText id="staticText13" style="font-size: 12px; margin-left: 10px; text-align: justify" text="Este programa muestra el manejo de variables de tipo TOLCode (obtención y modificación de su valor), así la realización de llamadas a funciones definidas en TOL."/>
                                        <script class="brush: java; toolbar: false;" type="syntaxhighlighter"><![CDATA[
                                            package com.hcarralero.toljava.ejemplos;

                                            //Se deben importar del paquete com.hcarralero.toljava todas las clases a usarse,
                                            //para este ejemplo se requieren las clases: TOLMachine, TOLMachineException, TOLReal y TOLCode.
                                            import com.hcarralero.toljava.TOLMachine;
                                            import com.hcarralero.toljava.TOLMachineException;
                                            import com.hcarralero.toljava.TOLReal;
                                            import com.hcarralero.toljava.TOLCode;
                                            import com.hcarralero.toljava.TOLObject;

                                            /**
                                             * 5to Ejemplo de TOLJava para Consola:
                                             *  1) Se crea una instancia de TOLMachine.
                                             *  2) Se ejecuta una sentencia TOL para definir las funciones sumar y restar, así como las
                                             *     la variable f de tipo Code con valor inicial sumar y la variable x de tipo Real.
                                             *  3) Se procesa el resultado.
                                             *  4) Se muestran los mensajes devueltos por el intéprete de TOL.
                                             *  5) Se obtiene el valor de la variable f (debe ser el valor inicial "sumar").
                                             *  6) Se muestra el valor de la variable f.
                                             *  7) Se cambia el valor de la variable f por un nuevo valor: restar.
                                             *  8) Se muestra el nuevo valor de la variable f.
                                             *  9) Se crea un arreglo de objetos que servirá como lista de parámetros a la llamada a
                                             *     la función f, el 1er parámetro es una constante de Tipo TOLReal y el 2do es una
                                             *     cadena que representa el nombre de variable "x".
                                             *  10) Se hace una llamada a la función f pasando como argumentos el nombre de
                                             *      función "f" y el arreglo de parámetros.  El resultado es asignado a la variable r3.
                                             *  11) Se muestra el valor de r3 que es resultado de invocar la función f.
                                             *
                                             * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
                                             * @version 1.0
                                             * @see com.hcarralero.toljava.TOLMachine, com.hcarralero.toljava.TOLMachineException,
                                             *      com.hcarralero.toljava.TOLReal, com.hcarralero.toljava.TOLCode.
                                             */
                                            public class Ejemplo05 {

                                                /**
                                                 * @param args los argumenetos de la línea de comandos.
                                                 */
                                                public static void main(String[] args) {
                                                    //Se crea una instancia de la clase TOLMachine.
                                                    TOLMachine tm= new TOLMachine();
                                                    //En una cadena se almacena el código TOL que se ejecutará; en este caso
                                                    //el código es para definir las funciones sumar y restar, así como la viariable f de tipo Code.
                                                    String code= "Real sumar(Real a, Real b) {Real r= a+b; r}; Real restar(Real a, Real b) {Real r= a-b; r}; Code f= sumar; Real x=5;";
                                                    //Se invoca el intérprete de TOL para ejecutar el código pasado por parámetro.
                                                    //El resultado es un arreglo de cadenas Object.
                                                    Object res[]= tm.execute(code);
                                                    System.out.println();
                                                    if (res==null) {
                                                        //Si el resultado es nulo, entonces se ha producido un error en el acceso a TOL.
                                                        System.out.println("ERROR ACCEDIENDO A TOL->execute");
                                                    } else {
                                                        //Se ha logrado acceder al intérprete y ejecutar el código.
                                                        int ne= 0;
                                                        try {
                                                            //La primera cadena representa el número de errores detectado por el Intérprete de TOL.
                                                            ne= Integer.parseInt((String)res[0]);
                                                            if (ne>0) {
                                                                //El intérprete de TOL reportó al menos un error.
                                                                System.out.println("Se ha(n) detectado "+ne+" error(es)");
                                                            } else {
                                                                //El intérprete de TOL no reportó errores.
                                                                System.out.println("No se han detectado errores");
                                                            }
                                                            //Los siguientes objetos que conforman la respuesta del intérprete pueden ser:
                                                            //Warnings, Errores o mensajes generados por el código TOL ejecutado.
                                                            //Para procesar los mensajes generados debe comprender el formato de
                                                            //estos mensajes. (Consulte la documentación de TOL).
                                                            for (int i= 1; i<res.length; ++i) {
                                                                //Cada objeto es una cadena
                                                                String strRes= (String)res[i];
                                                                //Se obtiene la longitud de la cadena.
                                                                int l= strRes.length();
                                                                if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                                                                    //En este caso el mensaje es un 'W'arning o un 'E'rror.
                                                                    System.out.println("\t"+strRes.substring(3, l-5));
                                                                } else {
                                                                    //En este caso es un mensaje común.
                                                                    System.out.println("\t"+strRes);
                                                                }
                                                            }
                                                            //Se obtiene el valor de la variable f (debe ser el valor inicial "sumar").
                                                            Object objf= tm.getVariable("f");
                                                            //Se imprime el valor de la variable f.
                                                            System.out.println("Valor de f:["+objf+"]");
                                                            //Se asigna un nuevo valor a f, la función "restar" (el valor de información no se tiene en cuenta).
                                                            tm.setVariableValue( "f", new TOLCode("restar", null));
                                                            //Se obtiene nuevamente el valor de la variable f (ya modificado debería ser "restar").
                                                            objf= tm.getVariable("f");
                                                            //Se imprime el nuevo valor de la variable f.
                                                            System.out.println("NUEVO Valor de f:["+objf+"]");
                                                            //Se crea un arreglo de objetos que servirá como lista de parámetros a la llamada a
                                                            //la función f, el 1er parámetro es una constante de Tipo TOLReal y el 2do es una
                                                            //cadena que representa el nombre de variable "x".
                                                            Object pars[]= {new TOLReal(12), "x"};
                                                            //Se hace una llamada a la función f pasando como argumentos el nombre de
                                                            //función "f" y el arreglo de parámetros. El resultado es asignado a la variable r3.
                                                            TOLObject or3= tm.callCode("f", pars);
                                                            TOLReal r3= (TOLReal)or3;
                                                            //Se muestra el valor de r3 que es resultado de invocar la función f.
                                                            System.out.println("f(12, x)="+r3);
                                                        } catch (NumberFormatException ex) {
                                                            //Algo inusual (ERROR INTERNO DE TOLJava) ha ocurrido.
                                                            System.out.println("ERROR DE ACCESO AL NÚMERO DE ERRORES");
                                                        } catch (TOLMachineException ex) {
                                                            //Algún error ha ocurrido en el acceso a la variable f.
                                                            System.out.println("ERROR accediendo a la variable f: "+ex.getMessage());
                                                        }
                                                    }
                                                }
                                            }
                                        ]]></script>
                                        <webuijsf:script type="text/javascript">
                                            SyntaxHighlighter.all()
                                        </webuijsf:script>
                                    </h:panelGrid>
                                </webuijsf:tab>
                                <webuijsf:tab id="tab6" text="Ejemplo #6">
                                    <h:panelGrid id="gridPanel7">
                                        <webuijsf:staticText id="staticText14"
                                            style="font-size: 12px; font-weight: bolder; margin-left: 10px; text-decoration: underline" text="Definición de una variable tipo Serie y acceso a items específicos de la misma"/>
                                        <webuijsf:staticText id="staticText15" style="font-size: 12px; margin-left: 10px; text-align: justify" text="Este programa muestra cómo definir una serie y los dos métodos disponibles para acceder a sus datos."/>
                                        <script class="brush: java; toolbar: false;" type="syntaxhighlighter"><![CDATA[
                                            package com.hcarralero.toljava.ejemplos;

                                            //Se deben importar del paquete com.hcarralero.toljava todas las clases a usarse,
                                            //para este ejemplo se requieren las clases: TOLMachine, TOLMachineException, TOLDate y TOLDateException.
                                            import com.hcarralero.toljava.TOLMachine;
                                            import com.hcarralero.toljava.TOLMachineException;
                                            import com.hcarralero.toljava.TOLDate;
                                            import com.hcarralero.toljava.TOLDateException;

                                            /**
                                             * 6to Ejemplo de TOLJava para Consola:
                                             *  1) Se crea una instancia de TOLMachine.
                                             *  2) Se ejecuta una sentencia TOL para definir la serie ser1.
                                             *  3) Se procesa el resultado.
                                             *  4) Se muestran los mensajes devueltos por el intéprete de TOL.
                                             *  5) Se obtiene el valor (val1) de la serie en una fecha específica.
                                             *  6) Se muestra val1.
                                             *  7) Se obtiene el valor (val2) de la serie en una posición específica indicada por un índice.
                                             *  8) Se muestra val2.
                                             *
                                             * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
                                             * @version 1.0
                                             * @see com.hcarralero.toljava.TOLMachine, com.hcarralero.toljava.TOLMachineException,
                                             *      com.hcarralero.toljava.TOLReal, com.hcarralero.toljava.TOLCode.
                                             */
                                            public class Ejemplo06 {

                                                /**
                                                 * @param args los argumenetos de la línea de comandos.
                                                 */
                                                public static void main(String[] args) {
                                                    //Se crea una instancia de la clase TOLMachine.
                                                    TOLMachine tm= new TOLMachine();
                                                    //En una cadena se almacena el código TOL que se ejecutará; en este caso
                                                    //el código es para definir una serie.
                                                    String code= "Serie ser1 = SubSer( Gaussian( 0, 1 ), y2009m1d1, y2009m12d31 );";
                                                    //Se invoca el intérprete de TOL para ejecutar el código pasado por parámetro.
                                                    //El resultado es un arreglo de cadenas Object.
                                                    Object res[]= tm.execute(code);
                                                    System.out.println();
                                                    if (res==null) {
                                                        //Si el resultado es nulo, entonces se ha producido un error en el acceso a TOL.
                                                        System.out.println("ERROR ACCEDIENDO A TOL->execute");
                                                    } else {
                                                        //Se ha logrado acceder al intérprete y ejecutar el código.
                                                        int ne= 0;
                                                        try {
                                                            //La primera cadena representa el número de errores detectado por el Intérprete de TOL.
                                                            ne= Integer.parseInt((String)res[0]);
                                                            if (ne>0) {
                                                                //El intérprete de TOL reportó al menos un error.
                                                                System.out.println("Se ha(n) detectado "+ne+" error(es)");
                                                            } else {
                                                                //El intérprete de TOL no reportó errores.
                                                                System.out.println("No se han detectado errores");
                                                            }
                                                            //Los siguientes objetos que conforman la respuesta del intérprete pueden ser:
                                                            //Warnings, Errores o mensajes generados por el código TOL ejecutado.
                                                            //Para procesar los mensajes generados debe comprender el formato de
                                                            //estos mensajes. (Consulte la documentación de TOL).
                                                            for (int i= 1; i<res.length; ++i) {
                                                                //Cada objeto es una cadena
                                                                String strRes= (String)res[i];
                                                                //Se obtiene la longitud de la cadena.
                                                                int l= strRes.length();
                                                                if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                                                                    //En este caso el mensaje es un 'W'arning o un 'E'rror.
                                                                    System.out.println("\t"+strRes.substring(3, l-5));
                                                                } else {
                                                                    //En este caso es un mensaje común.
                                                                    System.out.println("\t"+strRes);
                                                                }
                                                            }
                                                            //Se crea una variable fecha con valor dentro del rango de fechas de la serie
                                                            TOLDate date= new TOLDate(2009, 3, 11);
                                                            //Se obtiene el valor de la serie en la fecha escogida
                                                            double val1= tm.getSerieValue("ser1", date);
                                                            //Se imprime el valor obtenido
                                                            System.out.println("Valor de val1:["+val1+"]");
                                                            //Se obtiene el valor de la serie en la posición 2 (tercer elemento de la serie)
                                                            double val2= tm.getSerieValue("ser1", 2);
                                                            //Se imprime el valor obtenido
                                                            System.out.println("Valor de val2:["+val2+"]");
                                                        } catch (NumberFormatException ex) {
                                                            //Algo inusual (ERROR INTERNO DE TOLJava) ha ocurrido.
                                                            System.out.println("ERROR DE ACCESO AL NÚMERO DE ERRORES");
                                                        } catch (TOLMachineException ex) {
                                                            //Algún error ha ocurrido en el acceso a la variable f.
                                                            System.out.println("ERROR accediendo a la serie ser1: "+ex.getMessage());
                                                        } catch (TOLDateException ex) {
                                                            //Algún error ha ocurrido en el manejo de la variable de fecha (date).
                                                            System.out.println("ERROR accediendo a la serie ser1: "+ex.getMessage());
                                                        }
                                                    }
                                                }
                                            }
                                        ]]></script>
                                        <webuijsf:script type="text/javascript">
                                            SyntaxHighlighter.all()
                                        </webuijsf:script>
                                    </h:panelGrid>
                                </webuijsf:tab>
                                <webuijsf:tab id="tab7" text="Ejemplo #7">
                                    <h:panelGrid id="gridPanel8">
                                        <webuijsf:staticText id="staticText16"
                                            style="font-size: 12px; font-weight: bolder; margin-left: 10px; text-decoration: underline" text="Obtención de un conjunto de elementos de una serie"/>
                                        <webuijsf:staticText id="staticText17" style="font-size: 12px; margin-left: 10px; text-align: justify" text="En este programa se muestra cómo obtener un conjunto de valores de una serie usando los dos métodos posibles, a partir de una fecha o de una posición."/>
                                        <script class="brush: java; toolbar: false;" type="syntaxhighlighter"><![CDATA[
                                            package com.hcarralero.toljava.ejemplos;

                                            //Se deben importar del paquete com.hcarralero.toljava todas las clases a usarse,
                                            //para este ejemplo se requieren las clases: TOLMachine, TOLMachineException, TOLReal y TOLCode.
                                            import com.hcarralero.toljava.TOLMachine;
                                            import com.hcarralero.toljava.TOLMachineException;
                                            import com.hcarralero.toljava.TOLDate;
                                            import com.hcarralero.toljava.TOLDateException;
                                            import com.hcarralero.toljava.TSItem;
                                            import java.util.Calendar;
                                            import java.util.Date;

                                            /**
                                             * 7mo Ejemplo de TOLJava para Consola:
                                             *  1) Se crea una instancia de TOLMachine.
                                             *  2) Se ejecuta una sentencia TOL para definir la serie ser1.
                                             *  3) Se procesa el resultado.
                                             *  4) Se muestran los mensajes devueltos por el intéprete de TOL.
                                             *  5) Se obtiene un arreglo de valores (tsis1) de la serie a partir una fecha específica.
                                             *  6) Se muestran los valores obtenidos.
                                             *  7) Se obtiene un arreglo de valores (tsis2) de la serie a partir una posición específica.
                                             *  8) Se muestran los valores obtenidos.
                                             *
                                             * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
                                             * @version 1.0
                                             * @see com.hcarralero.toljava.TOLMachine, com.hcarralero.toljava.TOLMachineException,
                                             *      com.hcarralero.toljava.TOLReal, com.hcarralero.toljava.TOLCode.
                                             */
                                            public class Ejemplo07 {

                                                /**
                                                 * @param args los argumenetos de la línea de comandos.
                                                 */
                                                public static void main(String[] args) {
                                                    //Se crea una instancia de la clase TOLMachine.
                                                    TOLMachine tm= new TOLMachine();
                                                    //En una cadena se almacena el código TOL que se ejecutará; en este caso
                                                    //el código es para definir una serie con todos los valores.
                                                    String code= "Serie ser1 = SubSer( Gaussian( 0, 1 ), y2009m1d1, y2009m12d31 );";
                                                    //Se invoca el intérprete de TOL para ejecutar el código pasado por parámetro.
                                                    //El resultado es un arreglo de cadenas Object.
                                                    Object res[]= tm.execute(code);
                                                    System.out.println();
                                                    if (res==null) {
                                                        //Si el resultado es nulo, entonces se ha producido un error en el acceso a TOL.
                                                        System.out.println("ERROR ACCEDIENDO A TOL->execute");
                                                    } else {
                                                        //Se ha logrado acceder al intérprete y ejecutar el código.
                                                        int ne= 0;
                                                        try {
                                                            //La primera cadena representa el número de errores detectado por el Intérprete de TOL.
                                                            ne= Integer.parseInt((String)res[0]);
                                                            if (ne>0) {
                                                                //El intérprete de TOL reportó al menos un error.
                                                                System.out.println("Se ha(n) detectado "+ne+" error(es)");
                                                            } else {
                                                                //El intérprete de TOL no reportó errores.
                                                                System.out.println("No se han detectado errores");
                                                            }
                                                            //Los siguientes objetos que conforman la respuesta del intérprete pueden ser:
                                                            //Warnings, Errores o mensajes generados por el código TOL ejecutado.
                                                            //Para procesar los mensajes generados debe comprender el formato de
                                                            //estos mensajes. (Consulte la documentación de TOL).
                                                            for (int i= 1; i<res.length; ++i) {
                                                                //Cada objeto es una cadena
                                                                String strRes= (String)res[i];
                                                                //Se obtiene la longitud de la cadena.
                                                                int l= strRes.length();
                                                                if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                                                                    //En este caso el mensaje es un 'W'arning o un 'E'rror.
                                                                    System.out.println("\t"+strRes.substring(3, l-5));
                                                                } else {
                                                                    //En este caso es un mensaje común.
                                                                    System.out.println("\t"+strRes);
                                                                }
                                                            }
                                                            //Se crea una variable fecha con valor dentro del rango de fechas de la serie
                                                            TOLDate date= new TOLDate(2009, 3, 11);
                                                            //Se obtiene un arreglo de a lo sumo 10 valores de la serie a partir de la fecha dada
                                                            TSItem tsis1[]= tm.getSerieItems("ser1", date, 10);
                                                            //Se imprime el número real de elementos obtenidos (pudiera ser inferior a 10)
                                                            System.out.println("Número de elementos leídos: "+tsis1.length);
                                                            //Se crea una variable calendario usado para posteriormente imprimir las fechas de la serie
                                                            Calendar cal= Calendar.getInstance();
                                                            //Se recorre el arreglo obtenido
                                                            for (int i= 0; i<tsis1.length; ++i) {
                                                                //Se obtiene la fecha de cada item
                                                                TOLDate tsid= tsis1[i].getDate();
                                                                //Se establece en el calendario la fecha obtenida para su impresión
                                                                cal.set(tsid.getYear(), tsid.getMonth(), tsid.getDay(), tsid.getHour(), tsid.getMinute(), (int)tsid.getSecond());
                                                                //Se imprime la fecha y el valor de cada item de la serie
                                                                System.out.println("["+new Date(cal.getTimeInMillis()).toString()+"]==>["+tsis1[i].getValue()+"]");
                                                            }
                                                            //Se obtiene un arreglo de a lo sumo 15 valores de la serie a partir de la posición dada
                                                            TSItem tsis2[]= tm.getSerieItems("ser1", 0, 15);
                                                            //Se imprime el número real de elementos obtenidos (pudiera ser inferior a 15)
                                                            System.out.println("Número de elementos leídos: "+tsis2.length);
                                                            //Se recorre el arreglo obtenido
                                                            for (int i= 0; i<tsis2.length; ++i) {
                                                                //Se obtiene la fecha de cada item
                                                                TOLDate tsid= tsis2[i].getDate();
                                                                //Se establece en el calendario la fecha obtenida para su impresión
                                                                cal.set(tsid.getYear(), tsid.getMonth(), tsid.getDay(), tsid.getHour(), tsid.getMinute(), (int)tsid.getSecond());
                                                                //Se imprime la fecha y el valor de cada item de la serie
                                                                System.out.println("["+new Date(cal.getTimeInMillis()).toString()+"]==>["+tsis2[i].getValue()+"]");
                                                            }
                                                        } catch (NumberFormatException ex) {
                                                            //Algo inusual (ERROR INTERNO DE TOLJava) ha ocurrido.
                                                            System.out.println("ERROR DE ACCESO AL NÚMERO DE ERRORES");
                                                        } catch (TOLMachineException ex) {
                                                            //Algún error ha ocurrido en el acceso a la variable f.
                                                            System.out.println("ERROR accediendo a la serie ser1: "+ex.getMessage());
                                                        } catch (TOLDateException ex) {
                                                            //Algún error ha ocurrido en el manejo de la variable de fecha (date).
                                                            System.out.println("ERROR accediendo a la serie ser1: "+ex.getMessage());
                                                        }
                                                    }
                                                }
                                            }
                                        ]]></script>
                                        <webuijsf:script type="text/javascript">
                                            SyntaxHighlighter.all()
                                        </webuijsf:script>
                                    </h:panelGrid>
                                </webuijsf:tab>
                                <webuijsf:tab id="tab8" text="Ejemplo #8">
                                    <h:panelGrid id="gridPanel9">
                                        <webuijsf:staticText id="staticText18"
                                            style="font-size: 12px; font-weight: bolder; margin-left: 10px; text-decoration: underline" text="Obtención del primer y último elemento de un TimeSet (intervalo acotado)"/>
                                        <webuijsf:staticText id="staticText19" style="font-size: 12px; margin-left: 10px; text-align: justify" text="Este programa muestra el acceso a variables de tipo TimeSet desde Java; se define un TimeSet correspondiente a todos los lunes del año 2011 y luego se muestra el primer y último lunes del 2011."/>
                                        <script class="brush: java; toolbar: false;" type="syntaxhighlighter"><![CDATA[
                                            package com.hcarralero.toljava.ejemplos;

                                            //Se deben importar del paquete com.hcarralero.toljava todas las clases a usarse,
                                            //para este ejemplo se requieren las clases: TOLMachine, TOLMachineException, TOLDate y TOLDateException.
                                            import com.hcarralero.toljava.TOLMachine;
                                            import com.hcarralero.toljava.TOLMachineException;
                                            import com.hcarralero.toljava.TOLDate;
                                            import com.hcarralero.toljava.TOLDateException;
                                            import java.util.Calendar;
                                            import java.util.Date;

                                            /**
                                             * 8vo Ejemplo de TOLJava para Consola:
                                             *  1) Se crea una instancia de TOLMachine.
                                             *  2) Se ejecuta una sentencia TOL para definir el TimeSet ts1.
                                             *  3) Se procesa el resultado.
                                             *  4) Se muestran los mensajes devueltos por el intéprete de TOL.
                                             *  5) Se obtiene la primera fecha de ts1 y se muestra por pantalla.
                                             *  6) Se obtiene la última fecha de ts1 y se muestra por pantalla.
                                             *
                                             * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
                                             * @version 1.0
                                             * @see com.hcarralero.toljava.TOLMachine, com.hcarralero.toljava.TOLMachineException,
                                             *      com.hcarralero.toljava.TOLReal, com.hcarralero.toljava.TOLCode.
                                             */
                                            public class Ejemplo08 {

                                                /**
                                                 * @param args los argumenetos de la línea de comandos.
                                                 */
                                                public static void main(String[] args) {
                                                    //Se crea una instancia de la clase TOLMachine.
                                                    TOLMachine tm= new TOLMachine();
                                                    //En una cadena se almacena el código TOL que se ejecutará; en este caso
                                                    //el código es para definir un TimeSet que representa a "todos los lunes del año 2011"
                                                    String code= "TimeSet ts1= WD(1)*Y(2011);";
                                                    //Se invoca el intérprete de TOL para ejecutar el código pasado por parámetro.
                                                    //El resultado es un arreglo de cadenas Object.
                                                    Object res[]= tm.execute(code);
                                                    System.out.println();
                                                    if (res==null) {
                                                        //Si el resultado es nulo, entonces se ha producido un error en el acceso a TOL.
                                                        System.out.println("ERROR ACCEDIENDO A TOL->execute");
                                                    } else {
                                                        //Se ha logrado acceder al intérprete y ejecutar el código.
                                                        int ne= 0;
                                                        try {
                                                            //La primera cadena representa el número de errores detectado por el Intérprete de TOL.
                                                            ne= Integer.parseInt((String)res[0]);
                                                            if (ne>0) {
                                                                //El intérprete de TOL reportó al menos un error.
                                                                System.out.println("Se ha(n) detectado "+ne+" error(es)");
                                                            } else {
                                                                //El intérprete de TOL no reportó errores.
                                                                System.out.println("No se han detectado errores");
                                                            }
                                                            //Los siguientes objetos que conforman la respuesta del intérprete pueden ser:
                                                            //Warnings, Errores o mensajes generados por el código TOL ejecutado.
                                                            //Para procesar los mensajes generados debe comprender el formato de
                                                            //estos mensajes. (Consulte la documentación de TOL).
                                                            for (int i= 1; i<res.length; ++i) {
                                                                //Cada objeto es una cadena
                                                                String strRes= (String)res[i];
                                                                //Se obtiene la longitud de la cadena.
                                                                int l= strRes.length();
                                                                if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                                                                    //En este caso el mensaje es un 'W'arning o un 'E'rror.
                                                                    System.out.println("\t"+strRes.substring(3, l-5));
                                                                } else {
                                                                    //En este caso es un mensaje común.
                                                                    System.out.println("\t"+strRes);
                                                                }
                                                            }
                                                            //Se crea una variable calendario usado para posteriormente imprimir las fechas del TimeSet
                                                            Calendar cal= Calendar.getInstance();
                                                            //Para obtener la fecha inicial usaremos una fecha que sabemos es anterior a la primera fecha del TimeSet
                                                            TOLDate f1= new TOLDate(2010, 12, 31);
                                                            //Se obtiene la primera fecha del TimeSet buscando el sucesor de una fecha del año anterior
                                                            TOLDate fini= tm.getTimeSetNextDate("ts1", f1);
                                                            //Se establece en el calendario la fecha obtenida para su impresión
                                                            cal.set(fini.getYear(), fini.getMonth()-1, fini.getDay(), fini.getHour(), fini.getMinute(), (int)fini.getSecond());
                                                            //Se imprime la primera fecha del TimeSet
                                                            System.out.println("Fecha Inicial==> ["+new Date(cal.getTimeInMillis()).toString()+"]");
                                                            //Para obtener la fecha final usaremos una fecha que sabemos es posterior a la última fecha del TimeSet
                                                            TOLDate f2= new TOLDate(2012, 1, 1);
                                                            //Se obtiene la última fecha del TimeSet buscando el antecesor de una fecha del año siguiente
                                                            TOLDate ffin= tm.getTimeSetPrevDate("ts1", f2);
                                                            //Se establece en el calendario la fecha obtenida para su impresión
                                                            cal.set(ffin.getYear(), ffin.getMonth()-1, ffin.getDay(), ffin.getHour(), ffin.getMinute(), (int)ffin.getSecond());
                                                            //Se imprime la primera fecha del TimeSet
                                                            System.out.println("Fecha Final==> ["+new Date(cal.getTimeInMillis()).toString()+"]");
                                                        } catch (NumberFormatException ex) {
                                                            //Algo inusual (ERROR INTERNO DE TOLJava) ha ocurrido.
                                                            System.out.println("ERROR DE ACCESO AL NÚMERO DE ERRORES");
                                                        } catch (TOLMachineException ex) {
                                                            //Algún error ha ocurrido en el acceso a la variable f.
                                                            System.out.println("ERROR accediendo al TimeSet ts1: "+ex.getMessage());
                                                        } catch (TOLDateException ex) {
                                                            //Algún error ha ocurrido en el manejo de la variable de fecha (date).
                                                            System.out.println("ERROR accediendo al TimeSet ts1: "+ex.getMessage());
                                                        }
                                                    }
                                                }
                                            }
                                        ]]></script>
                                        <webuijsf:script type="text/javascript">
                                            SyntaxHighlighter.all()
                                        </webuijsf:script>
                                    </h:panelGrid>
                                </webuijsf:tab>
                                <webuijsf:tab id="tab9" text="Ejemplo #9">
                                    <h:panelGrid id="gridPanel10">
                                        <webuijsf:staticText id="staticText20"
                                            style="font-size: 12px; font-weight: bolder; margin-left: 10px; text-decoration: underline" text="Manipulación de un conjunto"/>
                                        <webuijsf:staticText id="staticText21" style="font-size: 12px; margin-left: 10px; text-align: justify" text="Este programa muestra la manipulación de una variable tipo Set de TOL desde Java; las operaciones ilustradas son la obtención del tamaño, el acceso a un elemento específico y la obtención en un arreglo de todos los elementos del conjunto."/>
                                        <script class="brush: java; toolbar: false;" type="syntaxhighlighter"><![CDATA[
                                            package com.hcarralero.toljava.ejemplos;

                                            //Se deben importar del paquete com.hcarralero.toljava todas las clases a usarse,
                                            //para este ejemplo se requieren las clases: TOLMachine, TOLMachineException, TOLDate y TOLDateException.
                                            import com.hcarralero.toljava.TOLMachine;
                                            import com.hcarralero.toljava.TOLMachineException;
                                            import com.hcarralero.toljava.TOLObject;

                                            /**
                                             * 9no Ejemplo de TOLJava para Consola:
                                             *  1) Se crea una instancia de TOLMachine.
                                             *  2) Se ejecuta una sentencia TOL para definir el Set set1.
                                             *  3) Se procesa el resultado.
                                             *  4) Se muestran los mensajes devueltos por el intéprete de TOL.
                                             *  5) Se obtiene el elemento de la posición 3 y se muestra.
                                             *  6) Se obtiene el tamaño de set1 y se muestra.
                                             *  7) Se obtienen todos los elementos del conjunto y se muestran.
                                             *
                                             * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
                                             * @version 1.0
                                             * @see com.hcarralero.toljava.TOLMachine, com.hcarralero.toljava.TOLMachineException,
                                             *      com.hcarralero.toljava.TOLReal, com.hcarralero.toljava.TOLCode.
                                             */
                                            public class Ejemplo09 {

                                                /**
                                                 * @param args los argumenetos de la línea de comandos.
                                                 */
                                                public static void main(String[] args) {
                                                    //Se crea una instancia de la clase TOLMachine.
                                                    TOLMachine tm= new TOLMachine();
                                                    //En una cadena se almacena el código TOL que se ejecutará; en este caso
                                                    //el código es para definir un Set de 5 números reales"
                                                    String code= "Set  set1 = SetOfDate(y1995m12d3, y1996m11d15, y1994m1d4, y1994m12d04);";
                                                    //Se invoca el intérprete de TOL para ejecutar el código pasado por parámetro.
                                                    //El resultado es un arreglo de cadenas Object.
                                                    Object res[]= tm.execute(code);
                                                    System.out.println();
                                                    if (res==null) {
                                                        //Si el resultado es nulo, entonces se ha producido un error en el acceso a TOL.
                                                        System.out.println("ERROR ACCEDIENDO A TOL->execute");
                                                    } else {
                                                        //Se ha logrado acceder al intérprete y ejecutar el código.
                                                        int ne= 0;
                                                        try {
                                                            //La primera cadena representa el número de errores detectado por el Intérprete de TOL.
                                                            ne= Integer.parseInt((String)res[0]);
                                                            if (ne>0) {
                                                                //El intérprete de TOL reportó al menos un error.
                                                                System.out.println("Se ha(n) detectado "+ne+" error(es)");
                                                            } else {
                                                                //El intérprete de TOL no reportó errores.
                                                                System.out.println("No se han detectado errores");
                                                            }
                                                            //Los siguientes objetos que conforman la respuesta del intérprete pueden ser:
                                                            //Warnings, Errores o mensajes generados por el código TOL ejecutado.
                                                            //Para procesar los mensajes generados debe comprender el formato de
                                                            //estos mensajes. (Consulte la documentación de TOL).
                                                            for (int i= 1; i<res.length; ++i) {
                                                                //Cada objeto es una cadena
                                                                String strRes= (String)res[i];
                                                                //Se obtiene la longitud de la cadena.
                                                                int l= strRes.length();
                                                                if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                                                                    //En este caso el mensaje es un 'W'arning o un 'E'rror.
                                                                    System.out.println("\t"+strRes.substring(3, l-5));
                                                                } else {
                                                                    //En este caso es un mensaje común.
                                                                    System.out.println("\t"+strRes);
                                                                }
                                                            }
                                                            //Se obtiene el tercer elemento de set1
                                                            TOLObject to= tm.getSetElement("set1", 3);
                                                            //Se imprime el tercer elemento de set1
                                                            System.out.println("Tercer elemento de set1=> ["+to.toString()+"]");
                                                            //Se obtiene el tamaño de set1
                                                            int size= tm.getSetSize("set1");
                                                            //Se imprime el tamaño de set1
                                                            System.out.println("Tamaño de set1=> ["+size+"]");
                                                            //Se obtienen todos los elementos del conjunto
                                                            TOLObject tos[]= tm.getSetElements("set1");
                                                            //Se imprime el arreglo que contiene los elementos del conjunto
                                                            System.out.println("Elementos de set1:");
                                                            for (int i= 0; i<tos.length; ++i) {
                                                                System.out.println("\tset1("+(i+1)+")==>"+tos[i].toString());
                                                            }
                                                        } catch (NumberFormatException ex) {
                                                            //Algo inusual (ERROR INTERNO DE TOLJava) ha ocurrido.
                                                            System.out.println("ERROR DE ACCESO AL NÚMERO DE ERRORES");
                                                        } catch (TOLMachineException ex) {
                                                            //Algún error ha ocurrido en el acceso a la variable f.
                                                            System.out.println("ERROR accediendo al Set set1: "+ex.getMessage());
                                                        }
                                                    }
                                                }
                                            }
                                        ]]></script>
                                        <webuijsf:script type="text/javascript">
                                            SyntaxHighlighter.all()
                                        </webuijsf:script>
                                    </h:panelGrid>
                                </webuijsf:tab>
                            </webuijsf:tabSet>
                        </h:panelGrid>
                    </webuijsf:form>
                </webuijsf:body>
            </webuijsf:html>
        </webuijsf:page>
    </f:view>
</jsp:root>
