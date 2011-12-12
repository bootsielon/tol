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
//        String code= "Set  set1 = SetOfText(\"alfa01\", \"beta01\", \"alfa02\", \"beta03\", \"alfa04\");";
//        String code= "Set  set1 = SetOfReal(5, 1.4, .23, 456, 83.85);";
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
