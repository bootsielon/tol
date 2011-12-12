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
