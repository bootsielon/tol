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
