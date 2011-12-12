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
