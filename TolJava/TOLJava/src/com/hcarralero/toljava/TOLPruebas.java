/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.hcarralero.toljava;

import com.hcarralero.toljava.*;

/**
 *
 * @author hcarralero
 */
public class TOLPruebas {

    /**
     * Primer Test de TOLJava para Consola:
     *  1) Se crea una instancia de TOLMachine.
     *  2) Se ejecuta una sentencia TOL.
     *  3) Se procesa el resultado.
     *  4) Se muestran los mensajes devueltos por el intéprete de TOL.
     *
     * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
     * @version 1.0
     * @see com.hcarralero.toljava.TOLMachine
     */
    private void doTest01() {
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

    /**
     * Segundo Test de TOLJava para Consola:
     *  1) Se crea una instancia de TOLMachine.
     *  2) Se ejecuta una sentencia TOL para definir las variables x (Text) e y (Real).
     *  3) Se procesa el resultado.
     *  4) Se muestran los mensajes devueltos por el intéprete de TOL.
     *
     * @author Humberto Andrés Carralero López (hcarralero@gmail.com)
     * @version 1.0
     * @see com.hcarralero.toljava.TOLMachine
     */
    private void doTest02() {
        TOLMachine tm= new TOLMachine();
        String code= "Text x=\"Hola. Bienvenido a TOLJava.\"; Real y= 3.5+7;";
        Object res[]= tm.execute(code);
        System.out.println();
        if (res==null) {
            System.out.println("ERROR ACCEDIENDO A TOL->execute");
        } else {
            int ne= 0;
            try {
                ne= Integer.parseInt((String)res[0]);
                if (ne>0) {
                    System.out.println("Se ha(n) detectado "+ne+" error(es)");
                } else {
                    System.out.println("No se han detectado errores");
                }
                for (int i= 1; i<res.length; ++i) {
                    String strRes= (String)res[i];
                    int l= strRes.length();
                    if (strRes.charAt(0)=='<'&&strRes.charAt(2)=='>'&&(strRes.charAt(1)=='E'||strRes.charAt(1)=='W')&&l>=7) {
                        System.out.println("\t"+strRes.substring(3, l-5));
                    } else {
                        System.out.println("\t"+strRes);
                    }
                }
                Object objx= tm.getVariable("x");
                System.out.println("Valor de x:["+objx+"]");
                tm.setVariableValue( "x", new TOLText("Hasta la vista TOL."));
                objx= tm.getVariable("x");
                System.out.println("NUEVO Valor de x:["+objx+"]");
                Object objy= tm.getVariable("y");
                System.out.println("Valor de y:["+objy+"]");
                tm.setVariableValue( "y", new TOLReal(4567.89));
                objy= tm.getVariable("y");
                System.out.println("NUEVO Valor de y:["+objy+"]");
            } catch (NumberFormatException ex) {
                System.out.println("ERROR DE ACCESO AL NÚMERO DE ERRORES");
            } catch (TOLMachineException ex) {

            }
        }
    }

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
    private void doTest03() {
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

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        TOLPruebas m= new TOLPruebas();
        System.out.println("*************************************************************************************");
        System.out.println("Test 01: Chequeo del acceso a TOL");
        m.doTest01();
        System.out.println("*************************************************************************************");
        System.out.println("*************************************************************************************");
        System.out.println("Test 02: Acceso y cambio de valores a variables definidas en TOL");
        m.doTest02();
        System.out.println("*************************************************************************************");
        System.out.println("*************************************************************************************");
        System.out.println("Test 03: Obtener el listado de todas las variables definidas en TOL");
        m.doTest03();
        System.out.println("*************************************************************************************");
    }

}
